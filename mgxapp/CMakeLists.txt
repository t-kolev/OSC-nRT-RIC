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

# This CMake definition supports several -D command line options:
#
#	-DDEBUG=n			Enable debugging level n
#	-DPRESERVE_PTYPE=1	Do not change the processor type when naming deb packages
#	-DPACK_EXTERNALS=1	Include external libraries used to build in the run-time package
#						(This makes some stand-alone unit testing of bindings possible, it
#						is not meant to be used for production package generation.)
#	-DGPROF=1			Enable profiling compile time flags
#	-DMAN_PREFIX=<path>	Supply a path where man pages are installed (default: /usr/share/man)

#	See ci/build_all for an example of how to build and test

project( munchkin LANGUAGES C CXX )
cmake_minimum_required( VERSION 3.5 )

set( major_version "1" )		# should be automatically populated from git tag later, but until CI process sets a tag we use this
set( minor_version "0" )
set( patch_level "0" )

set( install_root "${CMAKE_INSTALL_PREFIX}" )
if( MAN_PREFIX )
	set( install_man ${MAN_PREFIX} )			# is there a cmake var for this -- can't find one
else()
	set( install_man "/usr/share/man" )			# this needs to be fixed so it's not hard coded
endif()

# Must use GNUInstallDirs to install libraries into correct locations on all platforms.
include( GNUInstallDirs )


if( DEBUG )					# if set, we'll set debugging on in the compile
	set( debugging ${DEBUG} )
	message( "+++ debugging is being set to ${DEBUG}" )
else()
	set( debugging 0 )
	message( "+++ debugging is set to off" )
endif()
unset( DEBUG CACHE )					# we don't want this to persist


# define constants used in the version string, debugging, etc.
add_definitions(
	-DGIT_ID=${git_id}
	-DMAJOR_VER=${major_version}
	-DMINOR_VER=${minor_version}
	-DPATCH_VER=${patch_level}
	-DDEBUG=${debugging}
)

# ---------------- suss out pkg gen tools so we don't fail generating packages that the system cannot support --------------

# deb packages use underbars, and package manager(s) seem to flip the *_64 processor type
# to the old (non-standard) amd64 string, so we do it here for consistency. Set -DPRESERVE_PTYPE=1
# to prevent the flip. RPM packages will always be given the system generated processor type string.
#
if( ${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64" )
	if( NOT PRESERVE_PTYPE )
		set( deb_sys_name "amd64" )
	else()
		set( deb_sys_name ${CMAKE_SYSTEM_PROCESSOR} )
	endif()
else()
	set( deb_sys_name ${CMAKE_SYSTEM_PROCESSOR} )
endif()
unset( PRESERVE_PTYPE CACHE )					# we don't want this to persist

set( rpm_sys_name ${CMAKE_SYSTEM_PROCESSOR} )

set( deb_pkg_name "mgxapp" )
set( rpm_pkg_name "mgxapp" )

set( pkg_label "mgxapp${spoiled_str}-${major_version}.${minor_version}.${patch_level}-${sys_name}" )
set( rpm_pkg_label "${rpm_pkg_name}${spoiled_str}-${major_version}.${minor_version}.${patch_level}-${rpm_sys_name}" )
set( deb_pkg_label "${deb_pkg_name}${spoiled_str}_${major_version}.${minor_version}.${patch_level}_${deb_sys_name}" )
message( "+++ pkg name: ${deb_pkg_label}.deb" )

#set( out_yml /tmp/build_output.yml )					# we will record package names (we record only untainted names)
find_program( rpm NAMES rpmbuild )						# rpm package gen requires this to be installed

set( gen_rpm 0 )
if( "${rpm}" MATCHES "rpm-NOTFOUND" )			       # cannot build rpm
	set( pkg_list "DEB" )
	message( "### make package will generate only deb package; cannot find support to generate rpm packages" )
else()
	message( "+++ pkg name: ${rpm_pkg_label}.rpm" )		# debugging if we think we can gen rpm too
	set( pkg_list "DEB;RPM" )
	set( gen_rpm 1 )
	message( "+++ make package will generate both deb and rpm packages" )
endif()


# Compiler flags
#
set( CMAKE_POSITION_INDEPENDENT_CODE ON )
#set( CMAKE_C_FLAGS "-g -Wall " )
set( CMAKE_C_FLAGS "-g " )
set( CMAKE_CXX_FLAGS "-g " )
if( GPROF )					# if set, we'll set profiling flag on compiles
	message( "+++ profiling is on" )
	set( CMAKE_C_FLAGS "-pg " )
else()
	message( "+++ profiling is off" )
endif()
unset( GPROF CACHE )					# we don't want this to persist

# pull in the underlying dir specific things
add_subdirectory( lib )
add_subdirectory( src )
#add_subdirectory( doc )


# ------------- testing -------------------------------------------------------
enable_testing()
# cmake cannot set env vars, so we have to passed desired vars on the wrapper command
add_test(
		NAME drive_unit_tests
		COMMAND bash ../test/unit_test.sh -d ${CMAKE_CURRENT_BINARY_DIR}
		WORKING_DIRECTORY ../test
)

# ------------- packaging -----------------------------------------------------

#
# Define what should be installed, and where they should go.
# only the RMr headers, man pages and archive (.a) files.  The run-time package gets just
install( TARGETS ${target_list} EXPORT LibraryConfig
	LIBRARY  DESTINATION ${install_lib}
	ARCHIVE  DESTINATION ${install_lib}
	PUBLIC_HEADER DESTINATION ${install_inc}
)

IF( EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake" )
	include( InstallRequiredSystemLibraries )

	set( CPACK_DEBIAN_PACKAGE_NAME ${deb_pkg_name} )
	set( CPACK_RPM_PACKAGE_NAME ${rpm_pkg_name} )

	# auto dependency checking makes the RPM install fail, see NOTES
	set( CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION="/usr/local /usr/local/bin /usr/local/lib" )

	set( CPACK_set_DESTDIR "on" )
	set( CPACK_PACKAGING_INSTALL_PREFIX "${install_root}" )
	set( CPACK_GENERATOR "${pkg_list}" )

	set( CPACK_PACKAGE_DESCRIPTION "Thin library for RIC xAPP messaging routed based on message type." )
	set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "RIC message routing library" )
	set( CPACK_PACKAGE_VENDOR "None" )
	set( CPACK_PACKAGE_CONTACT "None" )
	set( CPACK_PACKAGE_VERSION_MAJOR "${major_version}" )
	set( CPACK_PACKAGE_VERSION_MINOR "${minor_version}" )
	set( CPACK_PACKAGE_VERSION_PATCH "${patch_level}" )
	set( CPACK_PACKAGE "${pkg_label}" )						# generic name for old versions of cpack
	set( CPACK_DEBIAN_FILE_NAME "${deb_pkg_label}.deb" )
	set( CPACK_RPM_FILE_NAME "${rpm_pkg_label}.rpm" )

	set( CPACK_DEBIAN_PACKAGE_PRIORITY "optional" )
	set( CPACK_DEBIAN_PACKAGE_SECTION "ric" )
	set( CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR} )
	set( CPACK_RPM_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR} )

	# this seems ingnored if included
	#set( CPACK_COMPONENTS_ALL Libraries ApplicationData )

	INCLUDE( CPack )
ENDIF()
