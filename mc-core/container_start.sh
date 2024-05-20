#!/usr/bin/env bash
# vim: ts=4 sw=4 noet:
#----------------------------------------------------------------------------------
#
#	Copyright (c) 2018-2020 AT&T Intellectual Property.
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
# Mnemonic:	container_start.sh
# Abstract: For some "pod" environments a single container is required.
#			This starts all of the related processes which normally would
#			be started in individual containers.
#
#			There are environment variables which affect the operation
#			of this script:
#
#				GSLITE_ROOT -- Assumed to be the root directory for the
#							core MC xAPP. If not defined, /mc/gs-lite is
#							assumed.
#
#			When NOT running in simulation mode, a registration message is
#			sent to the xAPP manager via the registration script in /playpen.
#			An unregister message is "queued" and should be sent when this
#			script receives a terminating event, or exits normally.
#
# Date:		13 February 2019
# Author:	E. Scott Daniels
# ----------------------------------------------------------------------

# MUST have a posix style function declaration!
unreg() {
	trap - EXIT						# prevent running this again when we force the exit
	/playpen/bin/xam_register.sh -U
	exit
}

set -e

FIFO_DIR="/tmp/mcl/fifos"

SIMULATOR_MODE=`python /mc/extract_params.py ${XAPP_DESCRIPTOR_PATH}/config-file.json simulator_mode`
RMR_PORT=`python /mc/extract_rmr_port.py ${XAPP_DESCRIPTOR_PATH}/config-file.json rmr-data`

mkdir -p $FIFO_DIR

if [ "$SIMULATOR_MODE" != "true" ]
then
	# --- start "sidecars" first. They are expected to need /playpen as the working dir

	(
		export RMR_SEED_RT=/tmp/empty.rt		# must ensure this exists; we won't use
		export RMR_STASH_RT=/tmp/stash.rt		# listener's rmr will stash the table received from RM
		>$RMR_SEED_RT
		>$RMR_STASH_RT							# mc-core apps will set this as their seed; it must be emptied here

		cd /playpen
		if [ "$RMR_PORT" != "" ]
		then
			bin/mc_listener -p $RMR_PORT
		else
			bin/mc_listener
		fi
	) >/tmp/listener.std 2>&1 &

	echo "$(date) listener was started; stashing route tables in $RMR_STASH_RT" >&2

	trap 'unreg' EXIT 1 2 3 4 15				# unregister on exit/hup/quit/term
	/playpen/bin/xam_register.sh				# register the xapp now that listener is up
fi


# ---- finally, start the core MC application -----------------------------
cd ${GSLITE_ROOT:-/mc/gs-lite}/demo/queries

export RMR_SEED_RT=/tmp/stash.rt		# must match above; place where "writers" will pick up the route table
export RMR_RTG_SVC=-1					# force all writers to use only the seed rt
./runall

