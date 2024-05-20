#
# Copyright (c) 2019 AT&T Intellectual Property.
# Copyright (c) 2018-2019 Nokia.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# This source code is part of the near-RT RIC (RAN Intelligent Controller)
# platform project (RICP).
#
cmake_minimum_required (VERSION 3.5)

file(
  DOWNLOAD https://raw.githubusercontent.com/hunter-packages/gate/master/cmake/HunterGate.cmake
  ${CMAKE_BINARY_DIR}/HunterGate.cmake)

include("${CMAKE_BINARY_DIR}/HunterGate.cmake")

HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.23.5.tar.gz"
    SHA1 "2c5c6fc1cf609d0856695d60f147594daf4e6b3e"
)

project(tracelibcpp LANGUAGES CXX C)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/cxx1.cmake)
add_compile_options(-Wall -Wextra -Werror)
set(CMAKE_CXX_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set (tracelibcpp_VERSION_MAJOR "0")
set (tracelibcpp_VERSION_MINOR "0")
set (tracelibcpp_VERSION_MICRO "5")
set (tracelibcpp_VERSION_STRING
    "${tracelibcpp_VERSION_MAJOR}.${tracelibcpp_VERSION_MINOR}.${tracelibcpp_VERSION_MICRO}")

# Set up cpack
# Common
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++ implementation RIC tracing initialization")
set(CPACK_PACKAGE_VENDOR "Nokia")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_VERSION_MAJOR ${tracelibcpp_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${tracelibcpp_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${tracelibcpp_VERSION_MICRO})
set(CPACK_COMPONENTS_ALL DIST DEVEL)
set(CPACK_COMPONENTS_GROUPING ONE_PER_GROUP )
set(CPACK_GENERATOR "RPM;DEB")
set(CPACK_COMPONENTS_IGNORE_GROUPS 1)
set(CPACK_PACKAGE_CONTACT None)

# RPM
set(CPACK_RPM_COMPONENT_INSTALL ON)
set(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")
set(CPACK_RPM_FILE_NAME RPM-DEFAULT)
set(CPACK_RPM_PACKAGE_AUTOREQ 1)

# Debian
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_DEB_PACKAGE_COMPONENT ON)
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)

# Add jaeger
hunter_add_package(jaegertracing)
find_package(jaegertracing CONFIG REQUIRED)

# Unit testing support of off by default
# Enable it with cmake -DWITH_TESTING=ON <srcdir>
option(WITH_TESTING "Include using testing support" OFF)

include_directories ("${PROJECT_SOURCE_DIR}/include/tracelibcpp")
include_directories ("${PROJECT_SOURCE_DIR}/src")

add_library(tracelibcpp SHARED
    src/tracelib.cpp
)

# Library versions are on by default
# i.e. so versions
option(WITH_VERSION "Support for library versioning" ON)

if (WITH_VERSION)
    set_target_properties(tracelibcpp PROPERTIES VERSION ${tracelibcpp_VERSION_STRING}
                                  SOVERSION ${tracelibcpp_VERSION_MAJOR})
endif (WITH_VERSION)

if (WITH_VERSION)
set_target_properties(tracelibcpp PROPERTIES VERSION ${tracelibcpp_VERSION_STRING}
                                  SOVERSION ${tracelibcpp_VERSION_MAJOR})
endif (WITH_VERSION)

target_link_libraries(tracelibcpp PUBLIC jaegertracing::jaegertracing-static)

include(GNUInstallDirs)
if (NOT DEFINED LIB_INSTALL_DIR)
    set(LIB_INSTALL_DIR lib)
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/tracelibcpp
        DESTINATION include
        COMPONENT DEVEL)

install(TARGETS tracelibcpp
        COMPONENT DIST
        LIBRARY DESTINATION ${LIB_INSTALL_DIR}
        NAMELINK_SKIP
        )

install(TARGETS tracelibcpp
        COMPONENT DEVEL
        LIBRARY DESTINATION ${LIB_INSTALL_DIR}
        NAMELINK_ONLY
        )

# Add google test
if (WITH_TESTING)
  file(
    DOWNLOAD https://raw.githubusercontent.com/bilke/cmake-modules/72d804cfbcf82a1e171200c9c02748fa4b7ea033/CodeCoverage.cmake
    ${CMAKE_BINARY_DIR}/CodeCoverage.cmake)

  include("${CMAKE_BINARY_DIR}/CodeCoverage.cmake")
  APPEND_COVERAGE_COMPILER_FLAGS()
  hunter_add_package(GTest)
  find_package(GTest CONFIG REQUIRED)
  add_executable(testrunner
      tst/testcreate.cpp
  )
  target_link_libraries(testrunner GTest::main) # GTest::gtest will be linked automatically
  target_link_libraries(testrunner GTest::gtest)
  target_link_libraries(testrunner tracelibcpp)
  add_test(UnitTest testrunner)
  add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS testrunner)
  ENABLE_TESTING()
endif (WITH_TESTING)
