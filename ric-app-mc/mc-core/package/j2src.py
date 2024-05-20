#!/usr/bin/env python3
#   vi: et ts=4 sw=4 :

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


#   Abstract:   A config file (json) parser that looks for one or more "descriptions" and outputs
#               shell styled variable assignments that can be sourced by a script. Descriptions
#               are of the form:
#                   <field>:[<field>:...:{*|shell_var_name}
#
#               The description "xapp_name:*"  will find the field xapp_name at the top level
#               and generate the variable assignment using the field name. The description
#               "xapp_name:xname" would find the same field but generate "xname=value" in the
#               output.
#
#               It may be necssary to pull a field from an array of objects where a second field
#               in the object has a desired value.  As an example, in the messaging section there
#               is an expected array of ports with each port having a name. To exctact a field
#               like this, the final field in the list may have the form:
#                   <name>[]<match_name>=<desired-value>@<field-name>
#
#               For   "messaging:port[]name=rmr-data@port"
#               The messaging object is first located, and each element in the port array is examined.
#               when the element which contains the field "name:, with a value of "rmr-data",
#               the value of "port" is assigned to the output shell variable name.
#
#               Limitations:   This only allows one array to be traversed, and it is assumed to be the
#               last field.  In other words, nested object arrays are not supported.
#
#               Usage:      j2src <config-file-name> <description> [<description>...]
#
#   Date:       29 Janurary 2021
#   Author:     E. Scott Daniels
# ------------------------------------------------------------------------------------------------------
import sys
import json

#   Parse the description (see above) and return name and value to the caller. None,None is returned
#   when we have an error, or cannot find the field. Debug strings MUST start with # so that they don't
#   affect shell parsing of output.
#
def parse( pj, description, debug=False ) :
    tokens = description.split( ":" )           # split fields, last is the output name or *
    out_name = tokens[-1]
    value = None

    if len( tokens ) < 2 :
        print( "## ERR ## badly formed description: %s" % description )
        return None, None

    for i in range( len( tokens ) - 1 )  :
        atoks = tokens[i].split( "[]" )
        if len( atoks ) > 1 :                            # array;  [0] is the name, [1] is name=value@desired-name
            if atoks[0] in pj  :
                nv = atoks[1].split( "=" )
                name = nv[0]
                sv = nv[1].split( "@" )
                if len( sv ) < 2 :
                    if( debug ) :
                        print( "## ERR ## badly formed capture string: missing 'value<desired'" )
                    return None, None

                match_val = sv[0]
                pull_name = sv[1]
                if out_name == "*" :
                    out_name = pull_name

                ao = pj[atoks[0]]                       # directly at the array object
                for i in range( len( ao ) ) :           # run each element
                    if name in ao[i] :
                        if ao[i][name] == match_val :       # this is the one we want
                            if pull_name in ao[i] :
                                return out_name, str( ao[i][pull_name] )            # all things go back as string

                    if debug :
                        print( "## WRN ## field is not in, or match value %s does NOT match, in %s[%d]: %s" % ( match_val, atoks[0], i, ao[i][name] ) )

                return None, None               # nothing matched and returned; bail now; array must be last field

            else :
                if debug :
                    print( "## WRN ## array %s is not found in %s" % (atoks[0], tokens[i]) )
                return None, None
        else :
            if atoks[0] in pj :
                pj = pj[atoks[0]]           # if not last, this will be an object (we hope)
                name = atoks[0]
                value = pj                  # last one should just be a field to yank
            else :
                if debug :
                    print( "## WRN ## field not found: %s" % atoks[0] )
                return None, None


    if out_name == "*" :
        return name, value
    return out_name, value


# take the name, value and print it as a valid shell variable. We convert True/False or true/false to
# 1 and 0.
#
def print_svar( vname, value ) :
        if value == "True" or value == "true" :
            value = "1"
        if value == "False" or value == "false" :
            value = "0"
        if not value.isnumeric() :
            value = '"' + value + '"'

        vname = vname.replace( " ", "_" )       # ensure it's a valid shell variable name
        vname = vname.replace( "-", "_" )

        print( "%s=%s" % ( vname, value ) )



# --------------------------------------------------------------------------------------------------------------

aidx = 1
debug = False
if sys.argv[1] == "debug" :
    aidx += 1
    debug = True

f = open( sys.argv[aidx] )
pj = json.load( f )
f.close()
aidx += 1

for i in range( aidx, len( sys.argv ) ) :
    name, val = parse( pj, sys.argv[i], debug )

    if name != None  and  val != None :
        print_svar( name, val )
