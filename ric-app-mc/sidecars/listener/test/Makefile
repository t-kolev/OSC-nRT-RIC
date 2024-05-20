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

# this make file assuems that both NNG and RMR are installed and that the variables
# LD_LIBRARY_PATH, LIBRARY_PATH are set correctly.

testers = sender pipe_reader sender

test_progs = sender unit_test pipe_reader
lib_obj = mcl.o rdc.o
lib_h = mcl.h

coverage_opts = -ftest-coverage -fprofile-arcs

# make with no parms should build tests
all: tests


# ---- testing stuff -----------------------------------------------------------------
tests: $(test_progs)


unit_test: unit_test.c ../src/mcl.c ../src/rdc.c
	gcc -g $(coverage_opts) unit_test.c -o unit_test -lrmr_si -lm -lpthread

# ---- housekeeping stuff -------------------------------------------------------------
# remove only intermediates
clean:
	rm -f *.o *.gcda *.gcno *.gcov

# remove anything that can be rebuilt
nuke: clean
	rm -f *mcl.a $(test_progs)

