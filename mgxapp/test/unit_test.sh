#!/usr/bin/env bash
# vim: ts=4 sw=4 noet:

#==================================================================================
#       Copyright (c) 2020 Nokia
#       Copyright (c) 2020 AT&T Intellectual Property.
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
#	Mnemonic:	unit_test.sh
#	Abstract:	This drives the unit tests and combs out the needed .gcov
#				files which are by some magic collected for Sonar.
#
#				This test assumes that the CMake build environment (../[.]build)
#				is what should be used as the base for the test.
#
#	Date:		23 March 2020
#	Author:		E. Scott Daniels
# -----------------------------------------------------------------------------

PATH=$PATH:.			# must be able to pick up our generated binaries

# ditch any .gcov files that were not generated from our source
function scrub_gcov {
	(
		grep -l "Source:/" *.gcov		# nothing that comes from /usr...
		ls -1 *_test.cpp.gcov				# nothing that is one of our test programmes
	) | xargs rm -f
}

# Make a list of our modules under test so that we don't look at gcov
# files that are generated for system lib headers in /usr/*
# (bash makes the process of building a list of names  harder than it
# needs to be, so use caution with the printf() call.)
#
function mk_list {
	grep -l "Source:\.\./src"  *.gcov | while read f
	do
		printf "$f "		# do NOT use echo or add \n!
	done
}

function abort_if_error {
	if (( $1 == 0 ))
	then
		return
	fi

	if [[ -n /tmp/PID$$.log ]]
	then
		$spew /tmp/PID$$.log
	fi
	echo "abort: $2"

	rm -f /tmp/PID$$.*
	exit 1
}

# -------------------------------------------------------------------------

spew="cat"					# default to dumping all make output on failure (-q turns it to ~40 lines)
run_verbose=0

build_dir="../.build"			# the usual place for CMake builds

while [[ $1 == "-"* ]]
do
	case $1 in
		-d)	lib_dir="$2"; shift;;
		-q) spew="head -40";;
		-v)	spew="cat";;
		-V)	run_verbose=1;;

		*)	echo "unrecognsied option: $1"
			echo "usage: $0 [-d build-dir] [-q|-v] [-V]"
			exit 1
			;;
	esac

	shift
done


if [[ ! -d $build_dir ]]
then
	if [[ ! -d "../build" ]]
	then
		echo "abort: cannot seem to find a build dir in either ../.build or ../build"
		echo "info: use the -d option to specify a directory other than the customary ones"
		echo "if the -d option was set, ($build_dir) isn't there"
		exit 1
	fi

	build_dir="../build"
fi

export build_dir

rm -f *.log
make nuke >/dev/null
abort_if_error $? "unable to nuke things prior to test"

#rm -f *.o								# ditch objects not built with coverage flags
errors=0

for p in *_test.cpp
do
	if [[ $p != *"xxcontext"* ]]
	then
		if (( run_verbose ))
		then
			echo "" >&2
			echo "----- make and run: ${p%.*} -------------" >&2
		fi

		make -B  ${p%.*} >/tmp/PID$$.log 2>&1
		abort_if_error $? "unable to make $p"
		if ! ${p%.*} >/tmp/PID$$.log 2>&1
		then
			$spew /tmp/PID$$.log
			echo "<FAIL>  ${p%.*} had errors"
			(( errors++ ))
		else
			if (( run_verbose ))		# -V given
			then
				cat $p.log
			fi
		fi
	fi
done

if (( errors ))
then
	echo "<FAIL> $errors unit tests failed"
	exit
fi

for p in *_test.cpp
do
	if [[ $p != *"xxcontext"* ]]
	then
		gcov $p >/dev/null 2>&1
	fi
done

# must force this as it's compiled stand alone and not executed
gcov tools_otest.c >/dev/null 2>&1

scrub_gcov			# eliminate the trash

echo ""
echo "----- coverage summaries ------"
#for f in *.cpp *.c
for f in *.gcov
do
	if [[ $f == *"_test."* || $f == "ut_"* ]]
	then
		continue
	fi

	./parse_gcov.sh $f						# generate simple, short, coverage stats
done

rm -f /tmp/PID$$.*

