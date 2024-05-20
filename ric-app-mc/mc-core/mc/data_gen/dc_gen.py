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

import sys
import random
import time
import os
import datetime

import sgnb_reconfiguration_complete_pb2
import ue_context_release_pb2
import x2ap_streaming_pb2


# --------------------------------
#	Operating params
n_ue = 50
events_per_fl = 5
epoch_len = 1
ue_id_range = 100000

conn_flnm = "MT_000010280"
dis_flnm = "MT_000010050"

fifo_dir = "/tmp/mcl/fifos"

# -------------------------------
#	Create records

def process_conn_record(ofl,gnb_id, ue_id, ts):
#	print "Connected ue_iud="+str(ue_id)+", gnb_id="+str(gnb_id)
	X2APStreaming = x2ap_streaming_pb2.X2APStreaming() # header message
	X2APStreaming.header.gNbID.value = "Bar"
	sec = ts / 1000
        ms = ts % 1000
        utc_time = datetime.datetime.utcfromtimestamp(sec)
        diff = utc_time - datetime.datetime(1900, 1, 1, 0, 0, 0)
        ntp_hi = diff.days*24*60*60+diff.seconds
        ntp_lo = (ms << 32) / 1000
        X2APStreaming.header.timestamp = ntp_hi << 32 | ntp_lo

#	root = sgnb_reconfiguration_complete_pb2.SgNBReconfigurationComplete()
	root = X2APStreaming.sgNBReconfigurationComplete

	root.id_MeNB_UE_X2AP_ID = ue_id
	root.id_SgNB_UE_X2AP_ID = gnb_id
	child = root.id_ResponseInformationSgNBReconfComp
	child.success_SgNBReconfComp.meNBtoSgNBContainer.value = "foo"

	v = X2APStreaming.SerializeToString()
	vlen = len(v)
	ofl.write("@MCL")
	ofl.write("{0:7}".format(vlen))
	ofl.write('\0')
	ofl.write(str(ts))
	ofl.write('\0')
	ofl.write('\0')
	ofl.write('\0')
	ofl.write(v)

	

def process_dis_record(ofl, gnb_id, old_enb_id, new_enb_id, ts):
#	print "Disconnected old_ue_iud="+str(old_enb_id)+", gnb_id="+str(gnb_id)+", new_ue_id="+str(new_enb_id)
	X2APStreaming = x2ap_streaming_pb2.X2APStreaming() # header message
	X2APStreaming.header.gNbID.value = "Bar"
	sec = ts / 1000
        ms = ts % 1000
        utc_time = datetime.datetime.utcfromtimestamp(sec)
        diff = utc_time - datetime.datetime(1900, 1, 1, 0, 0, 0)
        ntp_hi = diff.days*24*60*60+diff.seconds
        ntp_lo = (ms << 32) / 1000
        X2APStreaming.header.timestamp = ntp_hi << 32 | ntp_lo

#	root = ue_context_release_pb2.UEContextRelease()
	root = X2APStreaming.ueContextRelease

	root.id_Old_eNB_UE_X2AP_ID = old_enb_id
	root.id_New_eNB_UE_X2AP_ID = new_enb_id
	root.id_SgNB_UE_X2AP_ID.value = gnb_id

	v = X2APStreaming.SerializeToString()
	vlen = len(v)
	ofl.write("@MCL")
	ofl.write("{0:7}".format(vlen))
	ofl.write('\0')
	ofl.write(str(ts))
	ofl.write('\0')
	ofl.write('\0')
	ofl.write('\0')
	ofl.write(v)

# -------------------------------
#	main body of data generator

enb_to_ue = {}	# avoid duplicate enb_ue_ids
gnb_to_ue = {}	# avoid duplicate gnb_ue_ids
ue_list = []

# initialize
for i in xrange(0,n_ue):
	enb_id = random.randint(1,ue_id_range)
	while enb_id in enb_to_ue:
		enb_id = random.randint(1,ue_id_range)
	ue_list.append({"state": "dis", "enb_id": enb_id})
	enb_to_ue[enb_id] = i

if not os.path.exists(fifo_dir + "/" + conn_flnm):
	os.mkfifo(fifo_dir + "/" + conn_flnm)

if not os.path.exists(fifo_dir + "/" + dis_flnm):
	os.mkfifo(fifo_dir + "/" + dis_flnm)

cfl = open(fifo_dir + "/" + conn_flnm,"w", 0)
dfl = open(fifo_dir + "/" + dis_flnm, "w", 0)


# main loop
while(1):

	curr_time = int(time.time())

	processed = []
	for i in xrange(0, events_per_fl):
		ueix = random.randint(0,n_ue-1)
		if ueix in processed:
			ueix = random.randint(0,n_ue-1)
		processed.append(ueix)

		curr_ue = ue_list[ueix]
		if curr_ue["state"] == "dis":
			gnb_id = random.randint(1,ue_id_range)
			while gnb_id in gnb_to_ue:
				gnb_id = random.randint(1,ue_id_range)
			gnb_to_ue[gnb_id] = ueix
			curr_ue["gnb_id"] = gnb_id
			curr_ue["state"] = "conn"
			process_conn_record(cfl,curr_ue["gnb_id"], curr_ue["enb_id"],1000*curr_time +50*i)
		else:
			new_enb_id = random.randint(1,ue_id_range)
			while new_enb_id in enb_to_ue:
				new_enb_id = random.randint(1,ue_id_range)
			old_enb_id = curr_ue["enb_id"]
			gnb_id = curr_ue["gnb_id"]
			del enb_to_ue[old_enb_id]
			del gnb_to_ue[gnb_id]
			enb_to_ue[new_enb_id] = ueix

			curr_ue["enb_id"] = new_enb_id
			curr_ue["state"] = "dis"
			process_dis_record(dfl,gnb_id, old_enb_id, new_enb_id,1000*curr_time +50*i)

	time.sleep(epoch_len)
			
			
	
