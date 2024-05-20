#!/usr/bin/env bash
# vim: ts=4 sw=4 noet:

#--------------------------------------------------------------------------------
#	Copyright (c) 2018-2019 AT&T Intellectual Property.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#	   http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#--------------------------------------------------------------------------------

#------------------------------------------------------------------------------------
#	Mnemonic:	build_images.sh
#	Abstract:	This script will create both the mc_listener runtime and development
#				images when "all" is given on the command line, otherwise it builds
#				just a runtime environment.
#	Date:		22 August 2-19
#	Author:		E. Scott Daniels
# -----------------------------------------------------------------------------------

skip_dev=1
if [[ $1 == "all" ]]
then
	skip_dev=0
	shift
fi

if [[ $1 == "-?" || $1 == "-h" ]]
then
	echo "usage: $0 [all] [mcl-version-tag [patch-level]]"
	echo "   using all as first keyword causes both runtime and dev images to build"
	exit 0
fi


ver=${1:-1.3}
patch=${2:-0}

if (( skip_dev == 0 ))
then
	echo "building development image"
	docker build -f mcl_dev.df -t mcl_dev:$ver.$patch .
fi

# use the main dockerfile for the runtime image
echo "building runtime image mc_listener:$ver"
if docker build -f Dockerfile -t mc_listener:$ver.$patch .
then
	echo "build finished"
	echo ""
	docker images|grep mc_
fi

