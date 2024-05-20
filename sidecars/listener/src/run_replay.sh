#!/usr/bin/env ksh
# Do NOT use bash; it cannot handle constructing variables in while loops.


# vim: ts=4 sw=4 noet:
#----------------------------------------------------------------------------------
#
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
#
#---------------------------------------------------------------------------------


# ----------------------------------------------------------------------
# Mnemonic:	run_replay.sh
# Abstract: Simple script to make starting the replay binary easier.
#			Defaults:
#				/var/lib/mc/listener  -- directory for fifos
#
#			The input file can be supplied with -f and if omitted then
#			standard intput is assumed. This should allow the following
#			when run in a docker container:
#				docker run --rm -i -v /tmp/replay_fifos:/var/lib/mc/listener run_replay.sh <data-file
#
# Date:		20 November 2019
# Author:	E. Scott Daniels
# ----------------------------------------------------------------------


fifo_dir=/var/lib/mc/listener
data=""								# stdin by default
pre_open=0
mtype_list=""
gate=""
delay=0

while [[ $1 == -* ]]
do
	case $1 in 
		-f) data="$2"; shift;;
		-d)	fifo_dir=$2; shift;;
		-D) delay=$2; shift;;
		-g) gate="$2"; shift;;
		-m)	mtype_list="$2"; shift;;
		-p) pre_open=1;;

		*)	echo "$1 is not a recognised option"
			echo "usage: $0 [-d fifo-dir] [-D seconds] [-f data-file] [-g gate-file] [-m mtype-list] [-p]"
			echo "   -p causes FIFOs to be pre-allocated"
			echo "   -m supplies a comma separated list of message types to preopen FIFOs for"
			echo "      if -p is given and -m is omitted, the input file is examined to determine message types"
			echo "   -D seconds will cause a delay of the specified number of seconds before starting the replay"
			echo "   -g file  will cause the script to wait for file to appear before starting the replay"
			echo "      if both -D and -g are used, the delay happens after the gate file is found"
			exit 1
			;;
	esac

	shift
done

if (( pre_open )) 
then
	if [[ -z $mtype_list ]]
	then
		if [[ -z $data ]]
		then
			echo "error: cannot determine a mtype list from stdin:"
			echo "	-p given with out a list (-m) and input file set to default to stdin (missing -f)"
			exit 1
		fi

		rdc_extract $data 0 | sort -u | while read t
		do
			mtype_list="$mtype_list$t "
		done
	fi

	(
		cd $fifo_dir
		count=0
		for t in ${mtype_list//,/ }
		do
			name=$( printf "MT_%09d" t )
			echo "making FIFO: $name"
			mkfifo -m 664 $name 2>/dev/null		# if these are there, don't natter on about them
			(( count++ ))
		done

		ls MT_* | wc -l | read found
		if (( count != found ))
		then
			echo "warn:  after pre-create, expected $count FIFOs, but found only $found"
		fi
	)
fi

if [[ -n $data ]]
then
	if [[ ! -r $data ]]
	then
		echo "abort: cannot find data file: $data"
		exit 1
	fi

	data="-f $data"
fi

if [[ -n $gate ]]
then
	echo "waiting for gate file to appear: $gate"
	while true
	do
		if [[ -e $gate ]]
		then
			break
		fi
		sleep 1
	done
fi

if (( delay ))
then
	echo "pausing $delay seconds before starting rdc_display..."
	sleep $delay
fi

echo "starting rdc_replay"
rdc_replay -d $fifo_dir $data

