#! /usr/bin/env python3
#----------------------------------------------------------------------------------
#
#   Copyright (c) 2021 AT&T Intellectual Property.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
#---------------------------------------------------------------------------------


#   Abstract:   This will read a json file and generate it as an escaped string on
#               standard out.  Simple way of stuffing the config into a curl command.
#
#   Date:       28 January 2021
#   Author:     E. Scott Daniels
# ---------------------------------------------------------------------------------

import json
import sys

f = open( sys.argv[1] )
j = json.load( f )
qj = json.dumps( j )
print( qj.replace( '"', '\\"' ) )
