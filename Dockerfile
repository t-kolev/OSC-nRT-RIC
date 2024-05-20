#
#  Copyright (c) 2019 AT&T Intellectual Property.
#  Copyright (c) 2018-2019 Nokia.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
#   This source code is part of the near-RT RIC (RAN Intelligent Controller)
#   platform project (RICP).
#

FROM golang:1.16 as gobuild

# Install utilities
RUN apt update && apt install -y iputils-ping net-tools curl sudo

# Set the Working Directory for ves-agent inside the container
RUN mkdir -p $GOPATH/src/VESPA
WORKDIR $GOPATH/src/VESPA

# Clone VES Agent v0.3.0 from github
RUN git clone -b v0.3.0 https://github.com/nokia/ONAP-VESPA.git $GOPATH/src/VESPA

RUN GO111MODULE=on go mod download
ENV GOLANG_VERSION 1.16
RUN wget --quiet https://dl.google.com/go/go$GOLANG_VERSION.linux-amd64.tar.gz \
       && tar xvzf go$GOLANG_VERSION.linux-amd64.tar.gz -C /usr/local

# Install VES Agent
RUN export GOPATH=$HOME/go && \
    export PATH=$GOPATH/bin:$GOROOT/bin:$PATH && \
    go install -v ./ves-agent

# Set the Working Directory for vespamgr inside the container
RUN mkdir -p /go/src/vespamgr
RUN mkdir -p /cfg
COPY . /go/src/vespamgr
WORKDIR /go/src/vespamgr

RUN ./build_vesmgr.sh

# Final, executable and deployable container
FROM ubuntu:20.04

RUN mkdir -p /etc/ves-agent

COPY --from=gobuild /usr/local/lib /usr/local/lib
COPY --from=gobuild /root/go/bin /root/go/bin
COPY --from=gobuild /root/go/bin/vespamgr /usr/local/bin/vesmgr
COPY --from=gobuild /root/go/bin/vespamgr /vespamgr
COPY --from=gobuild /go/src/vespamgr/config/* /cfg/
COPY --from=gobuild /go/src/vespamgr/config/plt-counter.json /cfg/plt-counter.json
RUN ldconfig

ENV CFG_FILE=/cfg/config-file.json
ENV RMR_SEED_RT=/cfg/uta_rtg.rt

ENV PATH="/root/go/bin:${PATH}"

ENTRYPOINT ["/vespamgr"]
