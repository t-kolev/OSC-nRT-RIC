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
#	Mnemonic:	pub_cov.ksh
#	Abstract:	This script will push any coverage files (.gcov) in the current
#				directory to the indicated target directory (/tmp/gcov_rpts by
#				default.  Because sonar needs to match the source listed in the
#				.gcov file with what it perceives was analysed by it's code, we
#				must fiddle the name in the .gcov file to be a path relative to
#				the project root.  For example, if the .gcov file has the line:
#
#						0:Source:mc_listener.c
#
#				we must change that to be the path of sidecars/listener/src
#
#	Date:		3 September 2020
#	Author:		E. Scott Daniels
# -------------------------------------------------------------------------

sdir=sidecars/listener/src			# default source directory
pdir=/tmp/gcov_rpts					# publish directory

while [[ $1 == -* ]]
do
	case $1 in
		-p)	pdir=$2; shift;;
		-s) sdir=$2; shift;;
	esac

	shift
done


echo "publishing gcov files to: $pdir"

for f in *.gcov
do
	echo "publishing: $f to ${pdir}/$f"

	# rel replace will replace things like ../src based on the source directory
	awk -v sdir="$sdir"  \
		-v rel_rep="../${sdir##*/}/" '
		/0:Source:/ {
			gsub( rel_rep, "", $0 )			# relative replace must be first
			n = split( $0, a, ":" )			# file name into a[n]
			gsub( a[n], sdir "/" a[n], $0 )
		}

		{ print }
	' $f >${pdir}/$f
done
