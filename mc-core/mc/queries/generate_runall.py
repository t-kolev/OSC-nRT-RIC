# ------------------------------------------------
#Copyright 2020 AT&T Intellectual Property
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

import xml.etree.ElementTree as ET
import json
from optparse import OptionParser
import sys
import os



optparser = OptionParser(usage="usage: %prog [options] path_file [path_file*]")
optparser.add_option("-Q", "--querydir", dest="querydir",
                 default=".",help="directory with qtree.xml and output_spec.cfg" )
(options, args) = optparser.parse_args()

odoc = ""

nibflnm = options.querydir+"/nib.json"
nibfl = open(nibflnm, "r")
nib = json.load(nibfl)
nibfl.close()

osflnm = options.querydir+"/output_spec.cfg"
osfl = open(osflnm, "r")
oqy = {}
for line in osfl:
	flds = line.split(",")
	oqy[flds[0]]=1

runall = """
#!/bin/bash

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

set -m

FIFO_DIR="/tmp/mcl/fifos"

SIMULATOR_MODE=`python /mc/extract_params.py ${XAPP_DESCRIPTOR_PATH}/config-file.json simulator_mode`

DEBUG_MODE=`python /mc/extract_params.py ${XAPP_DESCRIPTOR_PATH}/config-file.json debug_mode`

WINDOW=`python /mc/extract_params.py ${XAPP_DESCRIPTOR_PATH}/config-file.json measurement_interval`

RMR_PORT=`python /mc/extract_rmr_port.py ${XAPP_DESCRIPTOR_PATH}/config-file.json rmr-data-out`

# export DBAAS_SERVICE_HOST=`python /mc/extract_params.py ${XAPP_DESCRIPTOR_PATH}/config-file.json __DBAAS_SERVICE_HOST__`
# export DBAAS_SERVICE_PORT=`python /mc/extract_params.py ${XAPP_DESCRIPTOR_PATH}/config-file.json __DBAAS_SERVICE_PORT__`

if [ "$SIMULATOR_MODE" = "true" ]
then
	python /mc/data_gen/dc_gen.py &
	python /mc/data_gen/rrcx_gen.py &
fi

if [ "$WINDOW" = "" ]
then
	WINDOW="10000"
fi

VES_COLLECTOR=`python /mc/extract_params.py ${XAPP_DESCRIPTOR_PATH}/config-file.json ves_collector_address`

VES_NAME=`echo $VES_COLLECTOR | awk 'BEGIN{FS=":"} {print $1}'`
VES_PORT=`echo $VES_COLLECTOR | awk 'BEGIN{FS=":"} {print $2}'`
VES_IP=`getent ahosts $VES_NAME | awk '{ print $1; exit }'`

echo "Clearing MC NIB namespace" >&2
/mc/gs-lite/bin/mc_clear >&2

echo "Storing MC NIB schemas" >&2
/mc/gs-lite/bin/mc_store_schema >&2

./runit
sleep 1

"""

debug_q = ""
for q in oqy:
	if "debug" in q:
		debug_q += "\t/mc/gs-lite/bin/gsprintconsole -e `cat gshub.log` default "+q+" window=$WINDOW &\n"

if len(debug_q)>0:
	runall += """
if [ "$DEBUG_MODE" = "true" ]
then
    # invoke gsprintconsole for all the queries with debug in their name
"""
	runall += debug_q
	runall += """
fi
"""

runall += """

if [ "$RMR_PORT" != "" ]
then
    RMR_OPTION="-R $RMR_PORT"
fi

# invoke gsprintconsole_ves gsmcnib for all non-debug queries
"""

for q in oqy:
	if "debug" not in q:
		runall += " /mc/gs-lite/bin/gsprintconsole_ves -C $VES_IP:$VES_PORT -U /vescollector/eventListener/v7 -V 7 $RMR_OPTION `cat gshub.log` default "+q+" window=$WINDOW &\n"
		keys = nib[q]["keys"]
		if len(keys)>0:
			keys_str = ",".join(keys)
			runall += " /mc/gs-lite/bin/gsmcnib -K "+keys_str+" `cat gshub.log` default "+q+" window=$WINDOW &\n"
		else:
			runall += " /mc/gs-lite/bin/gsmcnib `cat gshub.log` default "+q+" window=$WINDOW &\n"

runall += """

sleep 1
bash /mc/gs-lite/bin/start_processing
sleep infinity
# fg %1
"""

rflnm = options.querydir+"/runall"
rfl = open(rflnm, "w")
rfl.write(runall)
rfl.close()

cmd = "chmod +x "+rflnm
sys.stderr.write("Executing "+cmd+"\n")
ret = os.system(cmd)
if ret != 0:
	sys.stderr.write("Error executing "+cmd+"\n")
	exit(1)
exit(0)
