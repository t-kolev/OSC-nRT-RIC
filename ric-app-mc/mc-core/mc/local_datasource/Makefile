
# ------------------------------------------------
#   Copyright 2014 AT&T Intellectual Property
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
# -------------------------------------------

CC=gcc  -g -O3 -msse4.2 -fexpensive-optimizations -I libdag/include -I ../../../../include/lfta/ -I ../../../../include/ -I .././include/ -I ../../gscphost/include -I ../../../../include/lfta/local -I /usr/local/include
CXX=g++  -g -O3 -msse4.2 -fexpensive-optimizations -I libdag/include -I ../../../../include/lfta/ -I ../../../../include/ -I .././include/ -I ../../gscphost/include -I ../../../../include/lfta/local

SOURCE_C = 

SOURCE_CC =   
SOURCE =   $(SOURCE_C) $(SOURCE_CC) 


OBJECTS = $(SOURCE_C:.c=.o) $(SOURCE_CC:.cc=.o)


all: rts_proto.o wrappers.o sgnb_change_required.pb-c.o sn_status_transfer.pb-c.o common_types.pb-c.o rrc_sib.pb-c.o rrc_cg_config_info.pb-c.o secondary_rat_data_usage_report.pb-c.o rrc_failure_information.pb-c.o sgnb_addition_request.pb-c.o sgnb_activity_notification.pb-c.o sgnb_modification_refuse.pb-c.o sgnb_modification_request.pb-c.o rrc_reconfiguration.pb-c.o sgnb_addition_request_reject.pb-c.o rrc_measurement_report.pb-c.o rrc_common_types.pb-c.o rrc_cg_config.pb-c.o rrc_system_information.pb-c.o rrc_general_message_types.pb-c.o sgnb_modification_required.pb-c.o sgnb_modification_request_acknowledge.pb-c.o sgnb_release_request.pb-c.o sgnb_change_refuse.pb-c.o sgnb_release_required.pb-c.o rrc_reconfiguration_complete.pb-c.o gnb_status_indication.pb-c.o sgnb_reconfiguration_complete.pb-c.o error_cause.pb-c.o sgnb_change_confirm.pb-c.o sgnb_modification_request_reject.pb-c.o x2ap_streaming.pb-c.o rrctransfer.pb-c.o sgnb_release_confirm.pb-c.o sgnb_addition_request_acknowledge.pb-c.o sgnb_release_request_acknowledge.pb-c.o x2ap_common_types.pb-c.o sgnb_modification_confirm.pb-c.o ue_context_release.pb-c.o


wrappers.o : google/protobuf/wrappers.pb-c.c
	cc  -g -O3 -msse4.2 -fexpensive-optimizations -I libdag/include -I ../../../../include/lfta/ -I ../../../../include/ -I .././include/ -I ../../gscphost/include -I ../../../../include/lfta/local -I /usr/local/include -I . google/protobuf/wrappers.pb-c.c -c -o wrappers.o

INCDIR=../../../include
LFTA_DIR=$(INCDIR/lfta)


clean:
	rm -f *.o *.a core

