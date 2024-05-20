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

import json
import sys

usage = "extract_rmr_port.py xapp_descriptor_file port_name"

if len(sys.argv) < 3 :
    sys.exit(-1)

xapp_descriptor_file = sys.argv[1]
port_name = sys.argv[2]

ret = ''

with open(xapp_descriptor_file) as f:
    data = json.load(f)
    
    if 'messaging' in data.keys() and 'ports' in data['messaging'].keys():
        port_list = data['messaging']['ports']
        for port in port_list :
            if 'name' in port.keys() and 'port' in port.keys() and port['name'] == port_name :
                ret = port['port']

print ret
