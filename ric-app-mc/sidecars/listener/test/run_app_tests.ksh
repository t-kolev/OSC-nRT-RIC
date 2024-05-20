#!/usr/bin/env bash

#==================================================================================
#        Copyright (c) 2018-2020 AT&T Intellectual Property.
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
#==================================================================================


#
#	Mnemonic:	run_app_test.ksh
#	Abstract:	This script drives various applications (both mc_listener  related
#				and testers) to generate coverage stats for sonar.  There is little
#				we can validate with these tests other than the programme doesn't
#				crash.  The verify scripts are run at code checkin and will flag
#				any real problems, so this is just to keep sonar's knickers unbunched.
#
#				Assumptions:
#					- execution directory is listener/test
#					- source directory is ../src
#
#	Date:		2 September 2020
#	Author:		E. Scott Daniels
# -------------------------------------------------------------------------


# This is a hack! There seems not to be an easy way to have the LF
# environment adds RMR (or other needed packages) for testing. If we don't
# find RMR in the /usr/local part of the filesystem, we'll force it into
# /tmp which doesn't require root.  We'll be smart and get the desired
# rmr version from the repo root just as we _expected_ the CI environmnt
# would do (but seems not to).
#
function ensure_pkgs {
	if (( no_rmr_load ))
	then
		return
	fi

	if (( force_rmr_load )) || [[ -d /usr/local/include/rmr ]]
	then
		echo "[INFO] found RMR installed in /usr/local"
		return
	fi

	rv=$( grep "version:" ../../rmr-version.yaml | awk '{ print $NF; exit( 0 ) }' )
	rr=$( grep "repo:" ../../rmr-version.yaml | awk '{ print $NF; exit( 0 ) }' )
	if [[ -z $rv ]]
	then
		rv="4.2.1"			# some sane version if not found
	fi
	if [[ -z $rr ]]
	then
		rr="release"
	fi
	echo "[INFO] RMR seems not to be installed in /usr/local; pulling private copy: v=$rv"

	pkg_dir=/tmp/ut_pkg
	mkdir -p $pkg_dir

	(
		set -e
		opts="-nv --content-disposition"
		url_base="https://packagecloud.io/o-ran-sc/$rr/packages/debian/stretch"
		cd /tmp
		wget $opts ${url_base}/rmr_${rv}_amd64.deb/download.deb
		wget $opts ${url_base}/rmr-dev_${rv}_amd64.deb/download.deb

		for x in *rmr*deb
		do
			dpkg -x $x $pkg_dir
		done
	)
	if (( $? != 0 ))
	then
		echo "[FAIL] unable to install one or more RMR packages"
		exit 1
	fi

	LD_LIBRARY_PATH=$pkg_dir/usr/local/lib:$LD_LIBRARY_PATH
	LIBRARY_PATH=$pkg_dir/usr/local/lib:$LIBRARY_PATH
	export C_INCLUDE_PATH="$pkg_dir/usr/local/include:$C_INCLUDE_PATH"
}

# ------------------------------------------------------------------------------------------------

# these aren't set by default in some of the CI environments
#
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
export LIBRARY_PATH=/usr/local/lib:$LIBRARY_PATH

force_rmr_load=0
no_rmr_load=0
test_dir=$PWD

# defined in the CI configuration where jenkins jobs are looking for gcov files
gcov_dir=/tmp/gcov_rpts
if [[ ! -d $cov_dir ]]
then
	echo "<INFO> making $gcov_dir"
	mkdir $gcov_dir
fi


verbose=0
while [[ $1 == -* ]]
do
	case $1 in
		-f)	force_rmr_load=1;;
		-N) no_rmr_load=1;;					# for local testing
		-v)	verbose=1; vflag="-v";;

		*)	echo "unrecognised option: $1"
			exit 1
			;;
	esac

	shift
done

ensure_pkgs									# some CI enviroments may not have RMR; get it

cd ../src

# build the binaries with coverage options set
export TEST_COV_OPTS="-ftest-coverage -fprofile-arcs"		# picked up by make so we get coverage on tools for sonar
make clean			# ensure coverage files removed
make -B				# force build under the eyes of sonar build wrapper
if (( $? != 0 ))
then
	echo "[FAIL] build failed"
	exit
fi

rm -fr *.gcov *.gcda			# ditch any previously generated coverage info

# drive with full complement to test good branches, then with bad (missing value) to drive exceptions
./mc_listener -p 4567 -q -r 10 -e -d foo -x  >/dev/null 2>&1		# -x (invalid) prevents execution loop
for x in d p r \? h						# drive with missing values for d, p, r and singletons -h and -?
do
	./mc_listener -$x >/dev/null 2>&1
done
gcov  mc_listener.c					# debugging because jenkins gcov doesn't seem to be accumulating data

./pipe_reader -d foo -e -f -m 0 -s  -x >/dev/null 2>&1		# drive for all "good" conditions
for x in d m \? h
do
	./pipe_reader  -$x >/dev/null 2>&1		# drive each exception (missing value) or 'help'
done

./rdc_replay -d foo -f bar -t 0  -x >/dev/null 2>&1			# drive for all "good" conditions
for x in d f t  \? h
do
	./rdc_replay  -$x >/dev/null 2>&1		# drive each exception (missing value) or 'help'
done

./verify.sh $vflag					# verify MUST be first (replay relies on its output)
./verify_replay.sh

# generate and copy coverage files to parent which is where the CI jobs are looking for them
# we do NOT gen stats for the library functions; the unit test script(s) do that
#
for x in mc_listener sender rdc_replay pipe_reader
do
	gcov  $x.c
	#cp $x.c.gcov $gcov_dir/
done
$test_dir/publish_cov.ksh			# publish coverage files and fixup source names

echo "[INFO] ----- published coverage information ----------------------------------"
ls -al $gcov_dir
grep -i "0:Source:" $gcov_dir/*.gcov

echo "------------------------------------------------------------------------------"
echo "run_app_tests finished"

exit
