#
#==================================================================================
#	Copyright (c) 2020 Nokia
#	Copyright (c) 2020 AT&T Intellectual Property.
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

# This CMake causes all .c/.cpp files to be mixed together into the libmunch
# archive. Makes unit tests easier.  Because it's used only for building
# the munchkin, there is no need to worry about exporting header files or
# installing. For that reason we generate a static lib.

# Grab everything to eliminate maintenence if new source added.
file ( GLOB lib_src *.c *.cpp *.h )

add_library( munch STATIC ${lib_src} )


