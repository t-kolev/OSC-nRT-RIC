#!/usr/bin/env bash
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
# Mnemonic:	verify_replay.sh
# Abstract: Simple script to attempt to verify that the replay utility
#			works as expected. This assumes that verify.sh has been run
#			and that at least one RDC file is in /tmp/rdc. This script
#			will start the replay utility and a few pipe listeners to
#			parse the data.
#
# Date:		19 November 2019
# Author:	E. Scott Daniels
# ----------------------------------------------------------------------

# set the various sleep values based on long test or short test
function set_wait_values {
	reader_wait=12
	main_wait=20
}

function run_replay {
	echo "starting replayer"
	file=$( ls ${stage_dir}/MCLT_*|head -1 )
	chmod 644 $file

	set -x
	$bin_dir/rdc_replay -f $file -d $fifo_dir >$log_dir/replay.log 2>&1
	lpid=$!
	set +x
	echo "replay finished"
}

# run a pipe reader for one message type
function run_pr {
	echo "starting pipe reader $1"
	$bin_dir/pipe_reader $ext_hdr -m $1 -d $fifo_dir  >$log_dir/pr.$1.log 2>&1 &
	typeset prpid=$!

	sleep $reader_wait
	echo "stopping pipe reader $1"
	kill -1 $prpid
}

# ---- run everything ---------------------------------------------------

log_dir=/tmp/mcl_verify
mkdir -p $log_dir

ext_hdr=""					# run with extended header enabled (-e turns extended off)
run_listener=0				# -a turns on to run all
while [[ $1 == -* ]]
do
	case $1 in
		-a)	run_listener=1;;
		*)	echo "$1 is not a recognised option"
			echo "usage: $0 [-a]"
			echo "-a will cause the listener verification to run which generates files for this script"
			exit 1
			;;
	esac

	shift
done

if [[ -d /playpen/bin ]]	# designed to run in the container, but this allows unit test by jenkins to drive too
then
	bin_dir=/playpen/${si}bin
else
	bin_dir="."
fi

if (( run_listener ))
then
	echo "running listener to generate test files to replay..."
	set -e
	verify.sh				# assumed to be in the path
	set +e
fi

set_wait_values

if (( ! raw_capture ))		# -n set, turn off capture
then
	export MCL_RDC_ENABLE=0
fi

if [[ -d /data/final ]]			# assume if we find data that final directory goes here
then
	echo "### found /data/final using that as final directory"
	export MCL_RDC_FINAL=/data/final
fi

stage_dir=${MCL_RDC_STAGE:-/tmp/rdc/stage}
if [[ ! -d $stage_dir ]]
then
	echo "abort: cannot find stage directory to replay from: $stage_dir"
	exit 1
fi

fifo_dir=/tmp/fifos
if [[ ! -d $fifo_dir ]]
then
	mkdir -p $fifo_dir			# redirect fifos so we don't depend on mount
fi


for p in 0 1 2 3 4 5 6
do
	run_pr $p &
done

sleep 2
run_replay &				 # start after readers are going

sleep $main_wait			# long enough for all functions to finish w/o having to risk a wait hanging
echo "all functions stopped; looking at logs"

# ---------- validation -------------------------------------------------

errors=0

# logs should be > 0 in size
echo "----- logs ---------"
ls -al $log_dir/*.log

# pipe reader log files 1-6 should have 'stand up and cheer' messages
# pipe reader log for MT 0 will likley be empty as sender sends only
# one of those and buffer not likely flushed. So, we only check 1-6
#
for l in 1 2 3 4 5 6
do
	if [[ ! -s $log_dir/pr.$l.log ]]
	then
		echo "[FAIL] log $l was empty"
		(( errors++ ))
	else
		if ! grep -q -i "stand up and cheer" $log_dir/pr.$l.log
		then
			echo "[FAIL] pipe reader log did not have any valid messages: $log_dir/pr.$l.log"
			(( errors++ ))
		fi
	fi
done

if (( ! errors ))
then
	echo "[OK]    All logs seem good"
fi

nfifos=$( ls /tmp/fifos/MT_* | wc -l )
if (( nfifos < 7 ))
then
	echo "didn't find enough fifos"
	ls -al /tmp/fifos/*
	(( errors++ ))
else
	echo "[OK]    Found expected fifos"
fi

if (( errors ))
then
	echo "[FAIL] $errors errors noticed"
else
	echo "[PASS]"
fi

