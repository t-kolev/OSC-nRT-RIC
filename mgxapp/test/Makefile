# vi: ts=4 sw=4 noet:
#==================================================================================
#    Copyright (c) 2020 AT&T Intellectual Property.
#    Copyright (c) 2020 Nokia
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


coverage_opts = -O0 -ftest-coverage -fprofile-arcs -DSELF_TEST=1 -g
inc_opts = -I ../lib

ric_libs = -lricxfcpp -lrmr_si -lmdclog
sys_libs = -lcurlpp -lcurl -lpthread -lm

# bloody log installs in a non-standard/constant place it seems; sigh
# also need to pick up the lib from src for some things
LD_PATH = $(build_dir)/lib:/usr/lib/x86_64-linux-gnu:$(LD_LIBRARY_PATH)

# ---------- meta rules ------------------------------------------

# x.c which is generally built into its own object
%_otest.o: %.c
	LD_LIBRARY_PATH=$(LD_PATH) gcc $(inc_opts) $(coverage_opts) $< -c -o $@  $(ric_libs) $(sys_libs)

%_test: %_test.c to_%.o
	LD_LIBRARY_PATH=$(LD_PATH) gcc $(inc_opts) $(coverage_opts) $< -o $@  $(ric_libs) $(sys_libs)

%_test:: %_test.cpp
	LIBRARY_PATH=$(LD_PATH) LD_LIBRARY_PATH=$(LD_PATH) g++ $(inc_opts) $(coverage_opts) $< -g -o $@    -lmunch $(ric_libs) $(sys_libs)

% :: %.cpp
	LD_LIBRARY_PATH=$(LD_PATH) g++ $< -g -o $@  -L$(build_dir)/lib -lmunch -lrmr_si $(ric_libs) $(sys_libs)

%.o :: %.cpp
	LD_LIBRARY_PATH=$(LD_PATH) g++ $< -g -o $@  -c


# ------------- unit test ---------------------------------------
unit_test::	tools_test symtab_test rthing_test context_test ves_msg_test

# test needs to build the test module with gcov flags and link it rather than
# including it.
tools_test:: tools_test.cpp
	LD_LIBRARY_PATH=$(LD_PATH) gcc $(inc_opts) $(coverage_opts) ../lib/tools.c -c -o tools_otest.o
	LD_LIBRARY_PATH=$(LD_PATH) g++ $(inc_opts) $(coverage_opts) $< -g -o $@  tools_otest.o  -lmdclog


# ------------- housekeeping ------------------------------------

# intermediates only
clean:
	rm -f *.o *.gcda *.gcno *.log

# anything that can be built
nuke: clean
	rm -f *.gcov
	find . -name "*_test" -executable -type f | xargs rm -f
