#!/usr/bin/env bash

#==================================================================================
#        Copyright (c) 2018-2019 AT&T Intellectual Property.
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
#	Mnemonic:	discount_chk.ksh
#	Abstract:	This checks the gcov output from unit tests to determine
#				what number of lines of code are highly untestable. Primararily
#				these are lines in blocks which are executed when systems
#				calles (e.g. malloc) fail.
#
#	Date:		10 December 2019
#	Author:		E. Scott Daniels
# -------------------------------------------------------------------------


#
#	Parse the .gcov file and discount any unexecuted lines which are in if()
#	blocks that are testing the result of alloc/malloc calls, or testing for
#	nil pointers.  The feeling is that these might not be possible to drive
#	and shoudn't contribute to coverage deficiencies.
#
#	In verbose mode, the .gcov file is written to stdout and any unexecuted
#	line which is discounted is marked with ===== replacing the ##### marking
#	that gcov wrote.
#
#	The return value is 0 for pass; non-zero for fail.
function discount_gcov {
	typeset f="$1"

	#mct=$( get_mct ${1%.gcov} )			# see if a special coverage target is defined for this
	mct=80

	if [[ ! -f $1 ]]
	then
		if [[ -f ${1##*/} ]]
		then
			f=${1##*/}
		else
			echo "cant find: $f"
			return
		fi
	fi

	awk -v module_cov_target=$mct \
		-v cfail=${cfail:-WARN} \
		-v show_all=$show_all \
		-v full_name="${1}"  \
		-v module="${f%.*}"  \
		-v chatty=$chatty \
		-v replace_flags=$replace_flags \
	'
	function spit_line( ) {
		if( chatty ) {
			printf( "%s\n", $0 )
		}
	}

	/-:/ { 				# skip unexecutable lines
		spit_line()
		seq++					# allow blank lines in a sequence group
		next
	}

	{
		nexec++			# number of executable lines
	}

	/#####:/ {
		unexec++;
		if( $2+0 != seq+1 ) {
			prev_malloc = 0
			prev_if = 0
			seq = 0
			spit_line()
			next
		}

		if( prev_if && prev_malloc ) {
			if( prev_malloc ) {
				#printf( "allow discount: %s\n", $0 )
				if( replace_flags ) {
					if( replace_flags == 1 ) {
						gsub( "#####", "    1", $0 )
					} else {
						gsub( "#####", "=====", $0 )
					}
				}
				discount++;
			}
		}

		seq++;;
		spit_line()
		next;
	}

	/if[(].*errno ==.*{/ ||			# if( errno == ... ) assume error check

	/if[(].*alloc.*{/ {			# if( (x = malloc( ... )) != NULL ) or if( (p = sym_alloc(...)) != NULL )
		prev_malloc = 1
		prev_if = 1
		spit_line()
		seq = $2+0
		next
	}

	/if[(].* == NULL/ {				# a nil check likely not easily forced if it wasnt driven
		prev_malloc = 1
		prev_if = 1
		spit_line()
		seq = $2+0
		next
	}

	/if[(]/ {
		if( seq+1 == $2+0 && prev_malloc ) {		# malloc on previous line
			prev_if = 1
		} else {
			prev_malloc = 0
			prev_if = 0
		}
		spit_line()
		next
	}

	/alloc[(]/ {
		seq = $2+0
		prev_malloc = 1
		spit_line()
		next
	}

	{
		spit_line()
	}

	END {
		net = unexec - discount
		orig_cov = ((nexec-unexec)/nexec)*100		# original coverage
		adj_cov = ((nexec-net)/nexec)*100			# coverage after discount
		pass_fail = adj_cov < module_cov_target ? cfail : "PASS"
		rc = adj_cov < module_cov_target ? 1 : 0
		if( pass_fail == cfail || show_all ) {
			if( chatty ) {
				printf( "[%s] %s executable=%d unexecuted=%d discounted=%d net_unex=%d  cov=%d%% ==> %d%%  target=%d%%\n",
					pass_fail, full_name ? full_name : module, nexec, unexec, discount, net, orig_cov, adj_cov, module_cov_target )
			} else {
				printf( "[%s] %d%% (%d%%) %s\n", pass_fail, adj_cov, orig_cov, full_name ? full_name : module )
			}
		}

		exit( rc )
	}
	' $f
}

# ----------------------------------------------------------------

ignore_list="main"
module_cov_target=80
chatty=0
show_all=1
replace_flags=1
cfail="FAIL"

while [[ $1 == -* ]]
do
	case $1 in 
		-a)	show_all-1;;					# only show things not passing
		-c) module_cov_target=$2;;			# coverage target for module
		-r)	replace_flag=$2; shift;;		# replace with ==== if 2; no repalce if 0
		-V)	chatty=1;;						# show "dcov" output
	esac

	shift
done

for ckf in "$@"
do
	discount_gcov ${ckf%.gcov}.gcov
done
