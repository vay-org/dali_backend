# The MIT License (MIT)
#
# Copyright (c) 2020 NVIDIA CORPORATION
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

cmake_minimum_required(VERSION 3.17)

project(tritondalibackend C CXX)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()
message(STATUS "Build configuration: " ${CMAKE_BUILD_TYPE})

#
# Options
#
# Must include options required for this project as well as any
# projects included in this one by FetchContent.
#
option(TRITON_DALI_SKIP_DOWNLOAD "Don't download DALI. Use the version provided in the system" OFF)
option(TRITON_ENABLE_GPU "Enable GPU support in backend" ON)
option(TRITON_ENABLE_STATS "Include statistics collections in backend" ON)
option(WERROR "Trigger error on warnings" ON)

set(TRITON_COMMON_REPO_TAG "main" CACHE STRING "Tag for triton-inference-server/common repo")
set(TRITON_CORE_REPO_TAG "main" CACHE STRING "Tag for triton-inference-server/core repo")
set(TRITON_BACKEND_REPO_TAG "main" CACHE STRING "Tag for triton-inference-server/backend repo")

# Handle installation paths
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "/opt/tritonserver" CACHE PATH "..." FORCE)
else()
  set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
endif()

#
# Dependencies
#
# FetchContent's composibility isn't very good. We must include the
# transitive closure of all repos so that we can override the tag.
#
include(FetchContent)

FetchContent_Declare(
  repo-common
  GIT_REPOSITORY https://github.com/triton-inference-server/common.git
  GIT_TAG ${TRITON_COMMON_REPO_TAG}
  GIT_SHALLOW ON
)
FetchContent_Declare(
  repo-core
  GIT_REPOSITORY https://github.com/triton-inference-server/core.git
  GIT_TAG ${TRITON_CORE_REPO_TAG}
  GIT_SHALLOW ON
)
FetchContent_Declare(
  repo-backend
  GIT_REPOSITORY https://github.com/triton-inference-server/backend.git
  GIT_TAG ${TRITON_BACKEND_REPO_TAG}
  GIT_SHALLOW ON
)
FetchContent_MakeAvailable(repo-common repo-core repo-backend)

if (${TRITON_ENABLE_GPU})
    find_package(CUDAToolkit REQUIRED)
endif ()  # TRITON_ENABLE_GPU

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-variable -Wno-unused-function")

if (WERROR)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif()

add_subdirectory(src)

add_subdirectory(extern/Catch2)

#
# Install
#
include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/TritonDaliBackend)

install(
  TARGETS
    triton-dali-backend
  EXPORT
    triton-dali-backend-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/backends/dali COMPONENT backend
  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/backends/dali COMPONENT backend
)

install(
  DIRECTORY
    ${DALI_LIB_DIR}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/backends/dali/
  COMPONENT backend
)

message(WARNING ${DALI_LIB_DIR})

install(
  EXPORT
    triton-dali-backend-targets
  FILE
    TritonDaliBackendTargets.cmake
  COMPONENT
    backend-config
  NAMESPACE
    TritonDaliBackend::
  DESTINATION
    ${INSTALL_CONFIGDIR}
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
  ${CMAKE_CURRENT_LIST_DIR}/cmake/TritonDaliBackendConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/TritonDaliBackendConfig.cmake
  INSTALL_DESTINATION ${INSTALL_CONFIGDIR} 
)

install(
  FILES
  ${CMAKE_CURRENT_BINARY_DIR}/TritonDaliBackendConfig.cmake
  DESTINATION ${INSTALL_CONFIGDIR}
  COMPONENT backend-config
)

#
# Export from build tree
#
export(
  EXPORT triton-dali-backend-targets
  FILE ${CMAKE_CURRENT_BINARY_DIR}/TritonDaliBackendTargets.cmake
  NAMESPACE TritonDaliBackend::
)

export(PACKAGE TritonDaliBackend)

#
# CPack settings
#
set(TRITON_VERSION "2.11.0")
set(CPACK_GENERATOR "RPM")
set(CPACK_SET_DESTDIR OFF)
string(STRIP "${CMAKE_SYSTEM_PROCESSOR}" CPACK_RPM_PACKAGE_ARCHITECTURE)
set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/triton")
set(CPACK_PACKAGE_NAME "Triton DALI Backend")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "NVIDIA Triton DALI backend.")
set(CPACK_PACKAGE_DESCRIPTION "Native binaries for the NVIDIA Triton DALI backend.")
set(CPACK_PACKAGE_VENDOR "NVIDIA CORPORATION")
set(CPACK_PACKAGE_VERSION "${TRITON_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}_${CPACK_RPM_PACKAGE_ARCHITECTURE}")
set(CPACK_RPM_COMPONENT_INSTALL ON)
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_URL "https://github.com/triton-inference-server/dali_backend")
set(CPACK_RPM_PACKAGE_AUTOPROV ON)
set(CPACK_RPM_PACKAGE_AUTOREQ OFF)
set(CPACK_RPM_MAIN_COMPONENT backend)
set(CPACK_COMPONENTS_ALL backend backend-config)
include(CPack)   
