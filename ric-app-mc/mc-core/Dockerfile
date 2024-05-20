# -------------------------------------------------------------------------------
#    Copyright (c) 2018-2019 AT&T Intellectual Property.
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
# -------------------------------------------------------------------------------

ARG STAGE_DIR=/mc

# builders are now in release (10002)
FROM nexus3.o-ran-sc.org:10002/o-ran-sc/bldr-ubuntu18-c-go:1.9.0 AS project-build

ARG STAGE_DIR

ARG RMR_VER=4.7.4
ARG RMR_PC_REPO=release
#ARG RMR_PC_REPO=staging
RUN wget -nv --content-disposition https://packagecloud.io/o-ran-sc/${RMR_PC_REPO}/packages/debian/stretch/rmr_${RMR_VER}_amd64.deb/download.deb
RUN wget -nv --content-disposition https://packagecloud.io/o-ran-sc/${RMR_PC_REPO}/packages/debian/stretch/rmr-dev_${RMR_VER}_amd64.deb/download.deb
RUN dpkg -i rmr_${RMR_VER}_amd64.deb
RUN dpkg -i rmr-dev_${RMR_VER}_amd64.deb
RUN ldconfig

COPY mc ${STAGE_DIR}
WORKDIR ${STAGE_DIR}
RUN apt-get update && \
	apt-get install -y libboost-all-dev && \
	apt-get install -y libhiredis-dev && \
	apt-get clean
RUN git clone https://gerrit.o-ran-sc.org/r/ric-plt/sdl
WORKDIR ${STAGE_DIR}/sdl
RUN ./autogen.sh
RUN ./configure
RUN make all
RUN make install
WORKDIR ${STAGE_DIR}
RUN git clone https://github.com/protocolbuffers/protobuf.git
WORKDIR ${STAGE_DIR}/protobuf
RUN ./autogen.sh
RUN ./configure
RUN make clean
RUN make
RUN make install
RUN ldconfig

# hack to get protobufs to build
RUN cp src/google/protobuf/stubs/int128.h /usr/local/include/google/protobuf/stubs

WORKDIR ${STAGE_DIR}
RUN git clone https://github.com/protobuf-c/protobuf-c.git
WORKDIR protobuf-c
RUN ./autogen.sh
ENV PKG_CONFIG_PATH /usr/local/lib/pkgconfig
RUN ./configure
RUN make
RUN make install
RUN ldconfig
WORKDIR ${STAGE_DIR}
RUN git clone -b release/0.3.0 https://gerrit.o-ran-sc.org/r/com/gs-lite
WORKDIR ${STAGE_DIR}/gs-lite
COPY mc/cfg/packet_schema.txt ${STAGE_DIR}/gs-lite/cfg/
COPY mc/cfg/ifres.xml ${STAGE_DIR}/gs-lite/cfg/
COPY mc/cfg/localhost.ifq ${STAGE_DIR}/gs-lite/cfg/
COPY mc/local_datasource ${STAGE_DIR}/gs-lite/src/lib/gscprts/local_datasource
COPY mc/local ${STAGE_DIR}/gs-lite/include/lfta/local
COPY mc/queries ${STAGE_DIR}/gs-lite/demo/queries
COPY mc/mcnib/* ${STAGE_DIR}/gs-lite/src/tools/
RUN ./build_and_install.sh
WORKDIR ${STAGE_DIR}/gs-lite/demo/queries
ENV GSLITE_ROOT ${STAGE_DIR}/gs-lite
RUN bash ${STAGE_DIR}/gs-lite/bin/buildit
RUN python generate_runall.py


# now install the binaries and libraries into smaller docker image
FROM nexus3.o-ran-sc.org:10002/o-ran-sc/ric-app-mc-listener:1.12.0

# keep this close to the top to prevent lengthy rebuilds during testing
RUN apt-get update && \
    apt-get install -y curl python python-pip libboost-all-dev libhiredis-dev && \
    apt-get clean

ARG STAGE_DIR

COPY --from=project-build ${STAGE_DIR}/gs-lite/demo/queries /mc/gs-lite/demo/queries
COPY --from=project-build ${STAGE_DIR}/gs-lite/bin /mc/gs-lite/bin
COPY --from=project-build ${STAGE_DIR}/data_gen /mc/data_gen
COPY --from=project-build ${STAGE_DIR}/extract_params.py /mc/
COPY --from=project-build ${STAGE_DIR}/extract_rmr_port.py /mc/
COPY --from=project-build /usr/local/lib/libproto* /usr/local/lib/
COPY --from=project-build /usr/local/lib/libsdl* /usr/local/lib/

WORKDIR /opt/ric/config
COPY --from=project-build ${STAGE_DIR}/mc_deployment.json /opt/ric/config/config-file.json

COPY container_start.sh /playpen/bin/
COPY package/*.py package/*.sh /playpen/bin/

RUN ldconfig
RUN pip install protobuf

ENV XAPP_DESCRIPTOR_PATH /opt/ric/config/
WORKDIR /playpen
ENV GSLITE_ROOT /mc/gs-lite

CMD ["/playpen/bin/container_start.sh"]
