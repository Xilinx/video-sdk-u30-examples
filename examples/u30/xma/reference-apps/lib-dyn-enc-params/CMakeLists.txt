# Copyright (C) 2022, AMD/Xilinx Inc - All rights reserved
# Xilinx Worker-Launcher Application

# Licensed under the Apache License, Version 2.0 (the "License"). You may
# not use this file except in compliance with the License. A copy of the
# License is located at

#     http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations
# under the License.

cmake_minimum_required(VERSION 2.8.11)
include (CMakeDependentOption)
set(ENC_DYN_PARAM_LIB_PROJ "xilinx-u30-enc-dyn-params-lib")
set(ENC_DYN_PARAM_LIB_VER  "1.0.0")

find_program(UNAME_EXEC uname)
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

set(ENC_DYN_PARAM_LIB_PROJ_FULL_NAME "${ENC_DYN_PARAM_LIB_PROJ}_${ENC_DYN_PARAM_LIB_VER}_${LSB_RELEASE_REL_SHORT}-${CPACK_ARCH}")

project(${ENC_DYN_PARAM_LIB_PROJ})

set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/lib/pkgconfig")

#TBD
# Linking variables
find_package(PkgConfig REQUIRED)
pkg_check_modules(XRT REQUIRED libxma2api)
pkg_check_modules(XVBM REQUIRED xvbm)
pkg_check_modules(LIBXRM REQUIRED libxrm)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_C_FLAGS "-std=gnu99 -Wall -Wextra -DDSA64 ${CMAKE_C_FLAGS}")


# Building encoder dynamic params library
add_library(u30_enc_dyn_params SHARED src/xlnx_enc_dyn_params.c)
target_compile_options(u30_enc_dyn_params PUBLIC	${XRT_CFLAGS})
target_include_directories(u30_enc_dyn_params
	PUBLIC ${XRT_INCLUDE_DIRS}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(u30_enc_dyn_params ${XRT_LDFLAGS})

install(TARGETS u30_enc_dyn_params DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

# Packaging section
set(CPACK_PACKAGE_NAME ${ENC_DYN_PARAM_LIB_PROJ})
set(CPACK_PACKAGE_VERSION ${ENC_DYN_PARAM_LIB_VER})
set(CPACK_PACKAGE_FILE_NAME ${ENC_DYN_PARAM_LIB_PROJ_FULL_NAME})
set(CPACK_PACKAGE_CONTACT "Hasan Sayani <hasans@xilinx.com>")
set(CPACK_PACKAGE_VENDOR "AMD/Xilinx")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Xilinx Dynamic Encoder Parameters library")
set(CPACK_PACKAGE_DESCRIPTION "AMD/Xilinx's shared library that handles run time changing of encoder paramters.")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "xrt (>= 2.11), xilinx-u30-xmpsoccodecs (>= 1.1.0), xilinx-u30-xlookahead (>= 1.1.0), xilinx-u30-xmapropstojson (>= 1.0.1), xrm (>= 1.3), xilinx-u30-xrm-encoder (>= 1.1.0)" )
set(CPACK_RPM_PACKAGE_REQUIRES "xrt >= 2.11, xilinx-u30-xmpsoccodecs >= 1.1.0, xilinx-u30-xlookahead >= 1.1.0, xilinx-u30-xmapropstojson >= 1.0.1, xrm >= 1.3, xilinx-u30-xrm-encoder >= 1.1.0 " )
set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/opt" "/opt/xilinx" "/usr/lib" "/usr/lib/pkgconfig" "/usr/share/doc/xilinx" "/usr/share/doc/xilinx/u30")
include(CPack)
