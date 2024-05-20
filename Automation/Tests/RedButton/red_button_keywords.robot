##############################################################################
#
#   Copyright (c) 2019 AT&T Intellectual Property.
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
#
##############################################################################

*** Settings ***
Variables  ../Scripts/variables.py
Documentation   Keywords file
Resource    ../Resource/resource.robot
Resource    ../Resource/Keywords.robot
Library     ../Scripts/e2mdbscripts.py
Library    Collections
Library    OperatingSystem
Library    json
Library    REST      ${url}

*** Variables ***
${url}  ${e2mgr_address}

*** Keywords ***
Verify connected and associated
   Get Request nodeb
   Integer  response status  200
   String   response body ranName   ${ranName}
   String   response body connectionStatus    CONNECTED
   String   response body associatedE2tInstanceAddress  ${e2t_alpha_address}

Verify shutdown for gnb
    Get Request nodeb
    Integer  response status  200
    String   response body ranName    ${ranName}
    String   response body connectionStatus    SHUT_DOWN
    Missing  response body associatedE2tInstanceAddress

Verify E2T instance has no associated RANs
    ${result}    e2mdbscripts.verify_e2t_instance_has_no_associated_rans     ${e2t_alpha_address}
    Should Be True    ${result}

Execute Shutdown
   PUT       /v1/nodeb/shutdown
   Integer   response status   204





