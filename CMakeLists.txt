#
# Copyright (C) 2021, Xilinx Inc - All rights reserved
# Alveo U30 Examples
#
# Licensed under the Apache License, Version 2.0 (the "License"). You may
# not use this file except in compliance with the License. A copy of the
# License is located at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations
# under the License.
#

cmake_minimum_required(VERSION 2.8.11)
set(EXAMPLES_PROJ "xilinx-alveo-u30-examples")
set(EXAMPLES_VER  "3.0.3")

find_program(LSB_RELEASE_EXEC lsb_release)

execute_process(COMMAND ${LSB_RELEASE_EXEC} -is
    OUTPUT_VARIABLE LINUX_FLAVOR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (${LINUX_FLAVOR} MATCHES "^(Ubuntu|Debian)")
        # amd64
        execute_process(
        COMMAND dpkg --print-architecture
        OUTPUT_VARIABLE CPACK_ARCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
elseif (${LINUX_FLAVOR} MATCHES "^(RedHat|CentOS|Amazon|Fedora|SUSE)")
        # x86_64
        execute_process(
        COMMAND uname -m
        OUTPUT_VARIABLE CPACK_ARCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
else()
        message( SEND_ERROR "Trying to build on incompatible system \"${LINUX_FLAVOR}\"." )
endif()

# Grab OS release short name
execute_process(COMMAND ${LSB_RELEASE_EXEC} -rs
        OUTPUT_VARIABLE LSB_RELEASE_REL_SHORT
        OUTPUT_STRIP_TRAILING_WHITESPACE)

if (${LINUX_FLAVOR} MATCHES "^Amazon")
        set(LSB_RELEASE_REL_SHORT "amzn${LSB_RELEASE_REL_SHORT}")
endif()

set(EXAMPLES_PROJ_FULL_NAME "${EXAMPLES_PROJ}_${EXAMPLES_VER}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_FILE_LICENSE_PATH "/usr/share/doc/xilinx/u30")

project(${EXAMPLES_PROJ})

install(DIRECTORY ${CMAKE_SOURCE_DIR}/examples/
        USE_SOURCE_PERMISSIONS
        DESTINATION ${CMAKE_INSTALL_PREFIX}/examples/ PATTERN ".git*" EXCLUDE)

install(FILES ${CPACK_RESOURCE_FILE_LICENSE} CONFIGURATIONS Release RUNTIME DESTINATION ${CPACK_FILE_LICENSE_PATH}/${EXAMPLES_PROJ})

# Packaging section
set(CPACK_PACKAGE_NAME ${EXAMPLES_PROJ})
set(CPACK_PACKAGE_VERSION ${EXAMPLES_VER})
set(CPACK_PACKAGE_FILE_NAME ${EXAMPLES_PROJ_FULL_NAME}_${LSB_RELEASE_REL_SHORT}-${CPACK_ARCH})
set(CPACK_PACKAGE_CONTACT "Caleb Farrand <calebf@amd.com>")
set(CPACK_PACKAGE_VENDOR "Xilinx")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Xilinx/AMD Alveo U30 Examples")
set(CPACK_PACKAGE_DESCRIPTION "Provide examples for using the Alveo U30 solution")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "xilinx-alveo-u30-ffmpeg (>= 3.0.0), xilinx-alveo-u30-ffmpeg (<= 3.9.9), xilinx-alveo-u30-gstreamer (>= 3.0.0), xilinx-alveo-u30-gstreamer (<= 3.9.9)")
set(CPACK_RPM_PACKAGE_REQUIRES   "xilinx-alveo-u30-ffmpeg  >= 3.0.0,  xilinx-alveo-u30-ffmpeg  <= 3.9.9,  xilinx-alveo-u30-gstreamer  >= 3.0.0,  xilinx-alveo-u30-gstreamer  <= 3.9.9")
set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/opt" "/opt/xilinx" "/usr/lib" "/usr/lib/pkgconfig" "/usr/share/doc/xilinx" "/usr/share/doc/xilinx/u30")
include(CPack)
