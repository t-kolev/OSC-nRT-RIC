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
import time
from binascii import unhexlify
import datetime

# from https://stackoverflow.com/questions/8730927/convert-python-long-int-to-fixed-size-byte-array
def long_to_bytes(val, endianness='big'):

	width = 64
	fmt = '%%0%dx' % (width // 4)
	s = unhexlify(fmt % val)
	if endianness == 'little':
		s = s[::-1]
	return s



# ------------------------------------
#	START generated code
import rrctransfer_pb2
import x2ap_streaming_pb2

#dataset = {
#"id_MeNB_UE_X2AP_ID": 1,
#"id_SgNB_UE_X2AP_ID": 2,
#"MeasResultServMO_data": [
#	{"servCellID": 3, "physCellId": 4, "rsrq": 5, "rsrp": 6, "sinr": 7},
#]
#}

def create_record(dataset, ofl, ts):
	X2APStreaming = x2ap_streaming_pb2.X2APStreaming() # header message
	X2APStreaming.header.gNbID.value = dataset["sgnb_id"]
	sec = ts / 1000
        ms = ts % 1000
        utc_time = datetime.datetime.utcfromtimestamp(sec)
        diff = utc_time - datetime.datetime(1900, 1, 1, 0, 0, 0)
        ntp_hi = diff.days*24*60*60+diff.seconds
        ntp_lo = (ms << 32) / 1000
        X2APStreaming.header.timestamp = ntp_hi << 32 | ntp_lo


	# print "sgnb_id="+X2APStreaming.header.gNbID.value+", ts="+str(ts)

	RRCTransfer = X2APStreaming.rrcTransfer # message RRCTransfer
	
	RRCTransferIEs = RRCTransfer.rrcTransfer_IEs # field rrcTransfer_IEs
	RRCTransferIEs.id_MeNB_UE_X2AP_ID = dataset["id_MeNB_UE_X2AP_ID"]	# field id_MeNB_UE_X2AP_ID
	RRCTransferIEs.id_SgNB_UE_X2AP_ID = dataset["id_SgNB_UE_X2AP_ID"]
	NRUeReport = RRCTransferIEs.id_UENRMeasurement
	RRCContainer = NRUeReport.uENRMeasurements
	ULDCCHMessageType = RRCContainer.UL_DCCH_message
	MeasurementReport = ULDCCHMessageType.measurementReport
	MeasurementReportIEs = MeasurementReport.measurementReport
	MeasResults = MeasurementReportIEs.measResults
	MeasResultServMOList = MeasResults.measResultServingMOList
	for d in dataset["MeasResultServMO_data"]:
		MeasResultServMO = MeasResultServMOList.items.add()
		MeasResultServMO.servCellID = d["servCellID"]
		MeasResultNR = MeasResultServMO.measResultServingCell
		MeasResultNR.physCellId.value = d["physCellId"]
		MeasResult = MeasResultNR.measResult
		CellResults = MeasResult.cellResults
		MeasQuantityResults = CellResults.resultsCSI_RS_Cell
		MeasQuantityResults.rsrq.value = d["rsrq"]
		MeasQuantityResults.rsrp.value = d["rsrp"]
		MeasQuantityResults.sinr.value = d["sinr"]
	
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

#	END generated code
# ------------------------------------

import os
import time
import random

def triangle_measurements(d):
	for dd in d["MeasResultServMO_data"]:
		dd["rsrp"] = int(random.triangular(0,100,50))
		dd["rsrq"] = int(random.triangular(0,100,50))
		dd["sinr"] = int(random.triangular(0,100,50))


sleep_interval = 1;
fifo_dir = "/tmp/mcl/fifos"
dst_flnm = "MT_000010350"


dataset1 = {
"sgnb_id": "g1",
"id_MeNB_UE_X2AP_ID": 1,
"id_SgNB_UE_X2AP_ID": 11,
"MeasResultServMO_data": [
	{"servCellID": 0, "physCellId": 10, "rsrq": 20, "rsrp": 40, "sinr": 60},
	{"servCellID": 1, "physCellId": 20, "rsrq": 60, "rsrp": 40, "sinr": 20}
]
}
dataset2 = {
"sgnb_id": "g1",
"id_MeNB_UE_X2AP_ID": 2,
"id_SgNB_UE_X2AP_ID": 12,
"MeasResultServMO_data": [
	{"servCellID": 0, "physCellId": 10, "rsrq": 25, "rsrp": 45, "sinr": 65},
	{"servCellID": 1, "physCellId": 20, "rsrq": 65, "rsrp": 45, "sinr": 25}
]
}
dataset3 = {
"sgnb_id": "g1",
"id_MeNB_UE_X2AP_ID": 3,
"id_SgNB_UE_X2AP_ID": 13,
"MeasResultServMO_data": [
	{"servCellID": 0, "physCellId": 10, "rsrq": 25, "rsrp": 45, "sinr": 65},
	{"servCellID": 1, "physCellId": 20, "rsrq": 65, "rsrp": 45, "sinr": 25}
]
}
dataset4 = {
"sgnb_id": "g2",
"id_MeNB_UE_X2AP_ID": 4,
"id_SgNB_UE_X2AP_ID": 14,
"MeasResultServMO_data": [
	{"servCellID": 0, "physCellId": 15, "rsrq": 25, "rsrp": 45, "sinr": 65},
	{"servCellID": 1, "physCellId": 25, "rsrq": 65, "rsrp": 45, "sinr": 25}
]
}
dataset5 = {
"sgnb_id": "g2",
"id_MeNB_UE_X2AP_ID": 5,
"id_SgNB_UE_X2AP_ID": 15,
"MeasResultServMO_data": [
	{"servCellID": 0, "physCellId": 15, "rsrq": 25, "rsrp": 45, "sinr": 65},
	{"servCellID": 1, "physCellId": 25, "rsrq": 65, "rsrp": 45, "sinr": 25}
]
}
dataset6 = {
"sgnb_id": "g2",
"id_MeNB_UE_X2AP_ID": 6,
"id_SgNB_UE_X2AP_ID": 16,
"MeasResultServMO_data": [
	{"servCellID": 0, "physCellId": 15, "rsrq": 25, "rsrp": 45, "sinr": 65},
	{"servCellID": 1, "physCellId": 25, "rsrq": 65, "rsrp": 45, "sinr": 25}
]
}

if not os.path.exists(fifo_dir +"/" + dst_flnm):
	os.mkfifo(fifo_dir +"/" + dst_flnm)
ofl = open(fifo_dir +"/" + dst_flnm,"w", 0)

while(True):
	curr_time = int(time.time())
	triangle_measurements(dataset1)
	create_record(dataset1, ofl, 1000*curr_time + 100)
	triangle_measurements(dataset2)
	create_record(dataset2, ofl, 1000*curr_time + 200)
	triangle_measurements(dataset3)
	create_record(dataset3, ofl, 1000*curr_time + 300)
	triangle_measurements(dataset4)
	create_record(dataset4, ofl, 1000*curr_time + 400)
	triangle_measurements(dataset5)
	create_record(dataset5, ofl, 1000*curr_time + 500)
	triangle_measurements(dataset6)
	create_record(dataset6, ofl, 1000*curr_time + 600)
	time.sleep(sleep_interval)

