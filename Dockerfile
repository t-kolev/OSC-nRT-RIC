#   Copyright (c) 2019 AT&T Intellectual Property.
#   Copyright (c) 2019 Nokia.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

#----------------------------------------------------------
FROM nexus3.o-ran-sc.org:10002/o-ran-sc/bldr-ubuntu20-c-go:1.0.0 AS o1mediator-build

RUN apt update && apt install --reinstall -y \
  ca-certificates \
  && \
  update-ca-certificates

RUN apt-get update -y && apt-get install -y jq \
      git \
      cmake \
      build-essential \
      vim \
      supervisor \
      libpcre3-dev \
      pkg-config \
      libavl-dev \
      libev-dev \
      libprotobuf-c-dev \
      protobuf-c-compiler \
      #libssh-dev \
      libssl-dev \
      swig \
      iputils-ping \
      python-dev
#ENV GOLANG_VERSION 1.13.10
#RUN wget --quiet https://dl.google.com/go/go$GOLANG_VERSION.linux-amd64.tar.gz \
#        && tar xvzf go$GOLANG_VERSION.linux-amd64.tar.gz -C /usr/local 
#ENV PATH="/usr/local/go/bin:${PATH}"
ENV GOPATH="/go"

# ======================================================================
# First make the netconf sysrepo stuff
# add netconf user
RUN \
      adduser --system netconf && \
      echo "netconf:netconf" | chpasswd

# generate ssh keys for netconf user
RUN \
      mkdir -p /home/netconf/.ssh && \
      ssh-keygen -A && \
      ssh-keygen -t dsa -P '' -f /home/netconf/.ssh/id_dsa && \
      cat /home/netconf/.ssh/id_dsa.pub > /home/netconf/.ssh/authorized_keys

# use /opt/dev as working directory
RUN mkdir /opt/dev
WORKDIR /opt/dev

# libyang
RUN \
      cd /opt/dev && \
      git clone -b libyang1 https://github.com/CESNET/libyang.git && \
      cd libyang && mkdir build && cd build && \
      cmake -DCMAKE_BUILD_TYPE:String="Release" -DENABLE_BUILD_TESTS=OFF .. && \
      make -j2 && \
      make install && \
      ldconfig
# sysrepo
RUN \
      cd /opt/dev && \
      git clone -b libyang1 https://github.com/sysrepo/sysrepo.git && \
      cd sysrepo && sed -i -e 's/2000/30000/g;s/5000/30000/g' src/common.h.in && \
      mkdir build && cd build && \
      cmake -DCMAKE_BUILD_TYPE:String="Release" -DENABLE_TESTS=OFF -DREPOSITORY_LOC:PATH=/etc/sysrepo .. && \
      make -j2 && \
      make install && make sr_clean && \
      ldconfig

# libssh Newest
RUN \
      cd /opt/dev && \
      git clone https://git.libssh.org/projects/libssh.git && cd libssh && \
      mkdir build && cd build && \
      cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE="Release" -DWITH_ZLIB=ON -DWITH_NACL=OFF -DWITH_PCAP=OFF .. && \
      make -j2 && \
      make install

# libnetconf2
RUN \
      cd /opt/dev && \
      git clone -b libyang1 https://github.com/CESNET/libnetconf2.git && \
      cd libnetconf2 && mkdir build && cd build && \
      cmake -DCMAKE_BUILD_TYPE:String="Release" -DENABLE_BUILD_TESTS=OFF .. && \
      make -j2 && \
      make install && \
      ldconfig

# netopeer2
RUN \
      cd /opt/dev && \
      git clone -b libyang1 https://github.com/CESNET/Netopeer2.git && \
      cd Netopeer2 && mkdir build && cd build && \
      cmake -DCMAKE_BUILD_TYPE:String="Release" -DNP2SRV_DATA_CHANGE_TIMEOUT=30000 -DNP2SRV_DATA_CHANGE_WAIT=ON .. && \
      make -j2 && \
      make install
      
# ======================================================================

# RMR
ARG RMRVERSION=4.9.4
ARG RMRLIBURL=https://packagecloud.io/o-ran-sc/release/packages/debian/stretch/rmr_${RMRVERSION}_amd64.deb/download.deb
ARG RMRDEVURL=https://packagecloud.io/o-ran-sc/release/packages/debian/stretch/rmr-dev_${RMRVERSION}_amd64.deb/download.deb

RUN wget --content-disposition ${RMRLIBURL} && dpkg -i rmr_${RMRVERSION}_amd64.deb
RUN wget --content-disposition ${RMRDEVURL} && dpkg -i rmr-dev_${RMRVERSION}_amd64.deb
RUN rm -f rmr_${RMRVERSION}_amd64.deb rmr-dev_${RMRVERSION}_amd64.deb

# Install kubectl from Docker Hub
COPY --from=lachlanevenson/k8s-kubectl:v1.16.0 /usr/local/bin/kubectl /usr/local/bin/kubectl

RUN ldconfig

# Swagger
RUN mkdir -p /go/bin
RUN cd /go/bin \
    && wget --quiet https://github.com/go-swagger/go-swagger/releases/download/v0.19.0/swagger_linux_amd64 \
    && mv swagger_linux_amd64 swagger \
    && chmod +x swagger

RUN mkdir -p /go/src/ws
COPY . /go/src/ws
WORKDIR "/go/src/ws/agent"

# Module prepare (if go.mod/go.sum updated)
RUN GO111MODULE=on go mod download

# Fetch xApp Manager REST API spec
RUN mkdir -p api \
    && mkdir -p pkg \
    && git clone "https://gerrit.o-ran-sc.org/r/ric-plt/appmgr" \
    && cp appmgr/api/appmgr_rest_api.yaml api/ \
    && rm -rf appmgr
    
# generate swagger client
RUN /go/bin/swagger generate client -f api/appmgr_rest_api.yaml -t pkg/ -m appmgrmodel -c appmgrclient

# build and test o1agent
RUN ./build_o1agent.sh

# Install the data models based on the ric yang model
RUN /usr/local/bin/sysrepoctl -i /go/src/ws/agent/yang/o-ran-sc-ric-xapp-desc-v1.yang
RUN /usr/local/bin/sysrepoctl -i /go/src/ws/agent/yang/o-ran-sc-ric-ueec-config-v1.yang
RUN /usr/local/bin/sysrepoctl -i /go/src/ws/agent/yang/o-ran-sc-ric-gnb-status-v1.yang
RUN /usr/local/bin/sysrepoctl -i /go/src/ws/agent/yang/o-ran-sc-ric-alarm-v1.yang

CMD ["/bin/bash"]

#----------------------------------------------------------
FROM ubuntu:20.04 as o1mediator

RUN apt-get update -y && DEBIAN_FRONTEND=noninteractive apt-get install -y jq \
      net-tools \
      tcpdump \
      netcat \
      keychain \
      nano \
      supervisor \
      openssl \
      python3-pip \
      libpcre3-dev \
      pkg-config \
      libavl-dev \
      libev-dev \
      libprotobuf-c-dev \
      protobuf-c-compiler \
      #libssh-dev \
      libssl-dev \
      swig \
      python-dev \
      wget \
      && pip3 install supervisor-stdout \
      && pip3 install psutil \
      && apt-get clean

# Install psutil for python2.X
RUN wget https://bootstrap.pypa.io/pip/2.7/get-pip.py \
    && python get-pip.py \
    && python -m pip install psutil

RUN rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# update password policy 
RUN \
      sed -i 's/pam_unix.so obscure sha512/pam_unix.so obscure sha512 rounds=12000/' /etc/pam.d/common-password

# add netconf user
RUN \
      adduser --system netconf && \
      echo "netconf:netconf" | chpasswd

# generate ssh keys for netconf user
RUN \
      mkdir -p /home/netconf/.ssh && \
      ssh-keygen -A && \
      ssh-keygen -t dsa -P '' -f /home/netconf/.ssh/id_dsa && \
      cat /home/netconf/.ssh/id_dsa.pub > /home/netconf/.ssh/authorized_keys

# copy the supervisor config
ARG CONFIGDIR=/etc/supervisor
RUN mkdir -p ${CONFIGDIR}
COPY config/supervisord.conf ${CONFIGDIR}/supervisord.conf
    
# libraries and binaries & config
COPY --from=o1mediator-build /usr/local/share/ /usr/local/share/
COPY --from=o1mediator-build /usr/local/etc/ /usr/local/etc/
COPY --from=o1mediator-build /usr/local/bin/ /usr/local/bin/
COPY --from=o1mediator-build /usr/local/lib/ /usr/local/lib/
COPY --from=o1mediator-build /usr/local/bin/kubectl /usr/local/bin/kubectl

COPY --from=o1mediator-build /usr/include/libssh/ /usr/include/libssh/
COPY --from=o1mediator-build /usr/lib/x86_64-linux-gnu/libssh.so.4.* /usr/lib/x86_64-linux-gnu/
RUN ln -s libssh.so.4 /usr/lib/x86_64-linux-gnu/libssh.so

RUN ldconfig

# copy yang models with data
COPY --from=o1mediator-build /etc/sysrepo /etc/sysrepo

COPY --from=o1mediator-build /go/src/ws/agent/o1agent /usr/local/bin
COPY --from=o1mediator-build /go/src/ws/manager/src/process-state.py /usr/local/bin
RUN mkdir -p /etc/o1agent
COPY --from=o1mediator-build /go/src/ws/agent/config/* /etc/o1agent/

# ports available outside 8080 for mediator and 9001 supervise http control interrface
# port 830 for netconf client ssh session
# port 3000 for process-event handler web server
EXPOSE 9001 830 8080 3000

CMD ["/usr/bin/supervisord"]
