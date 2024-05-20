#
#==================================================================================
#	Copyright (c) 2018-2020 AT&T Intellectual Property.
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

# This top level CMake definition is intended only to suppor the LF jenkins
# environment which seems not to be customisable. It does NOT build anything.
# Run tests like this:
#
#	mkdir .build
#	cd .build
#	cmake ..
#	make tests ARGS=-v

#	See ci/build_all for an example of how to build and test

project( mc_testing LANGUAGES C )
cmake_minimum_required( VERSION 3.5 )

# ------------- testing -------------------------------------------------------
enable_testing()

# cmake cannot set env vars, so we have to passed desired vars on the wrapper command
# and assume the wrapper will do the right thing with them.  CMake also seems unable
# to reference ../dir1/dir2 (../dir1 works), so we have to use the crazy syntax
# with the absurdly long variable name before the ../.
#
add_test(
		NAME drive_listener_tests
		COMMAND  bash run_unit_test.ksh CMBUILD=${CMAKE_CURRENT_BINARY_DIR}
		WORKING_DIRECTORY   ${CMAKE_CURRENT_BINARY_DIR}/../sidecars/listener/test
)

# the listener application tests for sonar reports
add_test(
		NAME drive_mcl_app_tests
		COMMAND  bash run_app_tests.ksh CMBUILD=${CMAKE_CURRENT_BINARY_DIR}
		WORKING_DIRECTORY   ${CMAKE_CURRENT_BINARY_DIR}/../sidecars/listener/test
)
