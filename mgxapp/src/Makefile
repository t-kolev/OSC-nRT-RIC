# vi: ts=4 sw=4 noet:
#==================================================================================
#    Copyright (c) 2020 AT&T Intellectual Property.
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


coverage_opts = -ftest-coverage -fprofile-arcs -DSELF_TEST=1 -g

ric_libs = -lricxfcpp -lrmr_si -lmdclog
sys_libs = -lcurlpp -lcurl -lpthread -lm

# bloody log installs in a non-standard/constant place it seems; sigh
LD_PATH = /usr/lib/x86_64-linux-gnu

# ---------- meta rules ------------------------------------------

# x.c which is generally built into its own object
%_otest.o: %.c
	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$(LD_PATH) gcc $(coverage_opts) $< -c -o $@  $(ric_libs) $(sys_libs)

%_test: %_test.c to_%.o
	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$(LD_PATH) gcc $(coverage_opts) $< -o $@  $(ric_libs) $(sys_libs)

%_test:: %_test.cpp
	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$(LD_PATH) g++ $(coverage_opts) $< -g -o $@   -L. -lmunch $(ric_libs) $(sys_libs)

% :: %.cpp
	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$(LD_PATH) g++ $< -g -o $@  -L. -lmunch -lrmr_si $(ric_libs) $(sys_libs)

%.o :: %.cpp
	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$(LD_PATH) g++ $< -g -o $@  -c

# ------------------------------------------------------------------
lib_src = tools.o context.o rthing.o symtab.o ves_msg.o

all:: libmunch.a
#munchkin

munchkin:: libmunch.a munchkin.cpp munchkin.h

libmunch.a::	$(lib_src)
	ar r libmunch.a $(lib_src)

# ------------- unit test ---------------------------------------
unit_test::	tools_test symtab_test rthing_test

# test needs to build the test module with gcov flags and link it rather than
# including it.
tools_test:: tools_test.cpp libmunch.a
	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$(LD_PATH) gcc $(coverage_opts) tools.c -c -o tools_otest.o
	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$(LD_PATH) g++ $(coverage_opts) $< -g -o $@  tools_otest.o  -lmdclog
#-L. -lmunch  $(ric_libs) $(sys_libs)


#	symtab_test
#	tools_test
#	rthing_test
#	gcov *_test.cpp

symtab_test:: symtab_test.cpp symtab.cpp symtab.h


# ------------- housekeeping ------------------------------------

# intermediates only
clean:
	rm -f *.o *.gcda *.gcno *.log

# anything that can be built
nuke: clean
	rm -f test_tools symtab_test *.a *.gcov  munchkin
	find . -name "*_test" -executable -type f | xargs rm -f
