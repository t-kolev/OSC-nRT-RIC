# ==================================================================================
#
#       Copyright (c) 2024 Samsung Electronics Co., Ltd. All Rights Reserved.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#          http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
# ==================================================================================
FROM python:3.8

# RMR setup
RUN mkdir -p /opt/route/

ARG RMRVERSION=4.9.0
RUN wget --no-check-certificate --content-disposition https://packagecloud.io/o-ran-sc/release/packages/debian/stretch/rmr_${RMRVERSION}_amd64.deb/download.deb && dpkg -i rmr_${RMRVERSION}_amd64.deb
RUN wget --no-check-certificate --content-disposition https://packagecloud.io/o-ran-sc/release/packages/debian/stretch/rmr-dev_${RMRVERSION}_amd64.deb/download.deb && dpkg -i rmr-dev_${RMRVERSION}_amd64.deb
RUN rm -f rmr_${RMRVERSION}_amd64.deb rmr-dev_${RMRVERSION}_amd64.deb

# copy rmr libraries from builder image in lieu of an Alpine package
# COPY --from=nexus3.o-ran-sc.org:10002/o-ran-sc/bldr-alpine3-rmr:4.0.5 /usr/local/lib64/librmr* /usr/local/lib64/

COPY init/test_route.rt /opt/route/test_route.rt
ENV LD_LIBRARY_PATH /usr/local/lib/:/usr/local/lib64
ENV RMR_SEED_RT /opt/route/test_route.rt
ENV C_INCLUDE_PATH /usr/local/include

# sdl needs gcc
RUN apt update && apt install -y gcc g++ 

RUN pip install influxdb-client

RUN pip install numpy pandas

RUN pip install xgboost==1.2.0

RUN pip install ricxappframe

# Install
COPY setup.py /tmp
COPY README.md /tmp
COPY LICENSE.txt /tmp/
COPY src/ /tmp/src
COPY init/ /tmp/init
RUN pip install /tmp

RUN pip install flask-restful

RUN pip install scikit-learn

# Env - TODO- Configmap
ENV PYTHONUNBUFFERED 1
ENV CONFIG_FILE=/tmp/init/config-file.json

# For Default DB connection, modify for resp kubernetes env
ENV DBAAS_SERVICE_PORT=6379
ENV DBAAS_SERVICE_HOST=service-ricplt-dbaas-tcp.ricplt.svc.cluster.local

#Run
CMD main.py