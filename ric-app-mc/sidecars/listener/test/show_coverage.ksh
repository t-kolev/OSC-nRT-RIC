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
#	Mnemonic:	show_coverage.ksh
#	Abstract:	This script parses the coverage file associated with the .c file
#				listed on the command line and displays a more readable summary.
#
#	Date:		10 December 2019
#	Author:		E. Scott Daniels
# -------------------------------------------------------------------------

show_all=0
ignore_list="main"
module_cov_target=80
chatty=0
show_all=0
cfail="DCHK"

gcov  -f $1 | sed "s/'//g" | awk \
		-v cfail=$cfail \
		-v show_all=$show_all \
		-v ignore_list="$iflist" \
		-v module_cov_target=$module_cov_target \
		-v chatty=$verbose \
		'
		BEGIN {
			announce_target = 1;
			nignore = split( ignore_list, ignore, " " )
			for( i = 1; i <= nignore; i++ ) {
				imap[ignore[i]] = 1
			}

			exit_code = 0		# assume good
		}

		/^TARGET:/ {
			if( NF > 1 ) {
				target[$2] = $NF
			}
			next;
		}

		/File.*_test/ || /File.*test_/ {		# dont report on test files
			skip = 1
			file = 1
			fname = $2
			next
		}

		/File/ {
			skip = 0
			file = 1
			fname = $2
			next
		}

		/Function/ {
			fname = $2
			file = 0
			if( imap[fname] ) {
				fname = "skipped: " fname		# should never see and make it smell if we do
				skip = 1
			} else {
				skip = 0
			}
			next
		}

		skip { next }

		/Lines executed/ {
			split( $0, a, ":" )
			pct = a[2]+0

			if( file ) {
				if( announce_target ) {				# announce default once at start
					announce_target = 0;
					printf( "\n[INFO] default target coverage for modules is %d%%\n", module_cov_target )
				}

				if( target[fname] ) {
					mct = target[fname]
					announce_target = 1;
				} else {
					mct = module_cov_target
				}

				if( announce_target ) {					# annoucne for module if different from default
					printf( "[INFO] target coverage for %s is %d%%\n", fname, mct )
				}

				if( pct < mct ) {
					printf( "[%s] %3d%% %s\n", cfail, pct, fname )	# CAUTION: write only 3 things  here
					exit_code = 1
				} else {
					printf( "[PASS] %3d%% %s\n", pct, fname )
				}

				announce_target = 0;
			} else {
				if( pct < 70 ) {
					printf( "[LOW]  %3d%% %s\n", pct, fname )
				} else {
					if( pct < 80 ) {
						printf( "[MARG] %3d%% %s\n", pct, fname )
					} else {
						if( show_all ) {
							printf( "[OK]   %3d%% %s\n", pct, fname )
						}
					}
				}
			}

		}

		END {
			printf( "\n" );
			exit( exit_code )
		}
	'
