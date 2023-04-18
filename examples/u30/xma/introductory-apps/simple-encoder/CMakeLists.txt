# Copyright (C) 2023, Xilinx Inc - All rights reserved

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
set(XMA_APPS_PROJ "xma-encoder-simple")
set(XMA_APPS_VER  "2.0.0")



set(XMA_APPS_PROJ_FULL_NAME "${XMA_APPS_PROJ}_${XMA_APPS_VER}")

project(${XMA_APPS_PROJ})

set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/lib/pkgconfig")

# Linking variables
find_package(PkgConfig REQUIRED)
pkg_check_modules(XRT REQUIRED libxma2api)
pkg_check_modules(XVBM REQUIRED xvbm)
pkg_check_modules(LIBXRM REQUIRED libxrm)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_C_FLAGS "-std=gnu99 -Wall -Wextra -DDSA64 ${CMAKE_C_FLAGS}")

# Building encoder xma app
file(GLOB ENCODER_SRC "./*.c" "./*.cpp")
message("${ENCODER_SRC}")

add_executable(u30_xma_encoder_simple ${ENCODER_SRC})

target_include_directories(u30_xma_encoder_simple
	PUBLIC ${XRT_INCLUDE_DIRS}
	PUBLIC ${LIBXRM_INCLUDE_DIRS}
	PUBLIC ${XVBM_INCLUDE_DIRS}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

# Compile and link options for xma apps
set_property(TARGET u30_xma_encoder_simple 
    APPEND PROPERTY COMPILE_OPTIONS ${XRT_CFLAGS} ${LIBXRM_CFLAGS} ${XVBM_CFLAGS})

set_property(TARGET u30_xma_encoder_simple 
    APPEND PROPERTY LINK_LIBRARIES ${XRT_LDFLAGS} ${LIBXRM_LDFLAGS} ${XVBM_LDFLAGS} -lm)


