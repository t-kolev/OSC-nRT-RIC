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
# Mnemonic:	verify.sh
# Abstract: Simple script to attempt to verify that the mc_listener is
#			capable of running. This will start a listener and a sender
#			and then will cat a few lines from one of the FIFOs.
#			This script is designed to run using the geneated runtime
#			image; in other words, it expects to find the binaries
#			in /playpen/bin if that directory exists. If it does not it
#			assumes the current working directory is where the binaries
#			exist.
#
# Date:		26 August 2019
# Author:	E. Scott Daniels
# ----------------------------------------------------------------------

# set the various sleep values based on long test or short test
function set_wait_values {
	if (( long_test ))
	then
		export MCL_RDC_FREQ=13		# file cycle after 13s
		sender_wait=100
		listener_wait=105
		reader_wait=102
		main_wait=120
	else
		sender_wait=10
		listener_wait=15
		reader_wait=12
		main_wait=20
	fi
}

# run sender at a 2 msg/sec rate (500000 musec delay)
# sender sends msg types 0-6 and the route table in /tmp
# will direct them to the listener. We also need to switch
# the RT listen port so there is no collision with the listen
# preocess.
#
function run_sender {
	echo "starting sender"
	RMR_SEED_RT=/tmp/local.rt RMR_RTG_SVC=9989 $bin_dir/sender 43086 10000 >/tmp/sender.log 2>&1 &
	spid=$!
	sleep $sender_wait

	echo "stopping sender $spid"
	kill -15 $spid
}

function run_listener {
	echo "starting listener"
	$bin_dir/mc_listener $ext_hdr -r 1 -d $fifo_dir >/tmp/listen.log 2>&1 &
	lpid=$!

	sleep $listener_wait
	echo "stopping listener $lpid"
	kill -15 $lpid
}

# run a pipe reader for one message type
function run_pr {
	echo "starting pipe reader $1  $max_flag"
	set -x
	$bin_dir/pipe_reader $ext_hdr $max_flag -m $1 -d $fifo_dir  >/tmp/pr.$1.log 2>&1 &
	set +x
	typeset prpid=$!

	sleep $reader_wait
	echo "stopping pipe reader $ppid"
	kill -15 $prpid
}

# generate a dummy route table that the sender needs
function gen_rt {
	cat <<endKat >/tmp/local.rt
	newrt|start
	mse | 0 | -1 | localhost:4560
	mse | 1 | -1 | localhost:4560
	mse | 2 | -1 | localhost:4560
	mse | 3 | -1 | localhost:4560
	mse | 4 | -1 | localhost:4560
	mse | 5 | -1 | localhost:4560
	mse | 6 | -1 | localhost:4560
	mse | 7 | -1 | localhost:4560
	mse | 8 | -1 | localhost:4560
	mse | 9 | -1 | localhost:4560
	newrt|end
endKat
}

# ---- run everything ---------------------------------------------------

si=""						# if -s given then we add this to sender/listener to run SI95 versions
ext_hdr=""					# run with extended header enabled (-e turns extended off)
long_test=0
raw_capture=1
verbose=0
while [[ $1 == -* ]]
do
	case $1 in
		-l)	long_test=1;;
		-n)	raw_capture=0;;
		-s)	si="si_";;
		-v) verbose=1;;
		*)	echo "$1 is not a recognised option"
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

if [[ -d /data/stage ]]
then
	echo "### found /data/staging using that as stage directory"
	export MCL_RDC_STAGE=/data/stage
fi
final_dir=${MCL_RDC_FINAL:-/tmp/rdc/final}
stage_dir=${MCL_RDC_STAGE:-/tmp/rdc/stage}

fifo_dir=/tmp/fifos
mkdir -p $fifo_dir			# redirect fifos so we don't depend on mount

gen_rt						# generate a dummy route table
run_listener &
sleep 4

# the sender will send types 0-8 inclusive; we only start 7 readers to
# endure listener doesn't hang on pipes without a reader
#
for p in 0 1 2 3 4 5
do
	run_pr $p &				# all but last have no max read
done
max_flag="-M 10"
run_pr 6 &

sleep 1						# let the readers settle
run_sender &

sleep $main_wait			# long enough for all functions to finish w/o having to risk a wait hanging
echo "all functions stopped; looking at logs"

if (( verbose ))
then
	echo "[INFO] ---- mc_lisener log follwos --------------------------"
	cat /tmp/listen.log
	echo "[INFO] ------------------------------------------------------"
fi


# ---------- validation -------------------------------------------------

errors=0

# logs should be > 0 in size
echo "----- logs ---------"
ls -al /tmp/*.log

# pipe reader log files 1-6 should have 'stand up and cheer' messages
# pipe reader log for MT 0 will likley be empty as sender sends only
# one of those and buffer not likely flushed. So, we only check 1-6
#
for l in 1 2 3 4 5 6
do
	if [[ ! -s /tmp/pr.$l.log ]]
	then
		echo "[FAIL] log $l was empty"
		(( errors++ ))
	else
		if ! grep -q -i "stand up and cheer" /tmp/pr.$l.log
		then
			echo "[FAIL] pipe reader log did not have any valid messages: /tmp/pr.$l.log"
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

if (( raw_capture ))		# not an error if not capturing
then
	if [[ -d $stage_dir ]]
	then
		echo "[OK]    Found staging direcory ($stage_dir)"
		ls -al $stage_dir
	else
		(( errors++ ))
		echo "[FAIL]  No staging directory found ($stage_dir)"
	fi


	if [[ -d $final_dir ]]
	then
		echo "[OK]    Found final direcory ($final_dir)"
		ls -al $final_dir

		if (( long_test ))		# look for files in final dir to ensure roll
		then
			found=$( ls $final_dir/MC* | wc -l )
			if (( found > 0 ))
			then
				echo "[OK]   Found $found files in final directory ($final_dir)"
			else
				echo "[FAIL] Did not find any files in the final directory ($final_dir)"
				(( errors++ ))
			fi
		fi
	else
		(( errors++ ))
		echo "[FAIL]  No final directory found"
	fi
fi

if (( errors ))
then
	echo "[FAIL] $errors errors noticed"
else
	echo "[PASS]"
fi

