#
# CMAKE project for all Exotic Matter's Naiad Studio projects (NSTUDIO_ROOT)
#
# Copyright (c) 2009-2010 Exotic Matter AB.  All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
# * Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# * Neither the name of Exotic Matter AB nor its contributors may be used to
#   endorse or promote products derived from this software without specific 
#   prior written permission. 
# 
#    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
#    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,  INCLUDING,  BUT NOT 
#    LIMITED TO,  THE IMPLIED WARRANTIES OF  MERCHANTABILITY AND FITNESS
#    FOR  A  PARTICULAR  PURPOSE  ARE DISCLAIMED.  IN NO EVENT SHALL THE
#    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
#    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
#    BUT  NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE GOODS  OR  SERVICES; 
#    LOSS OF USE,  DATA,  OR PROFITS; OR BUSINESS INTERRUPTION)  HOWEVER
#    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,  STRICT
#    LIABILITY,  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN
#    ANY  WAY OUT OF THE USE OF  THIS SOFTWARE,  EVEN IF ADVISED OF  THE
#    POSSIBILITY OF SUCH DAMAGE.
#

# Ensure 2.6 (and onward) cmake compatibility

cmake_minimum_required(VERSION 2.6)
cmake_policy(VERSION 2.6)

# This is the top-level Naiad Studio project

project(NSTUDIO_ROOT)

# Intel compiler flags

if ("$ENV{EM_COMPILER}" STREQUAL "intel")

set (DEBUG_COMP_FLAGS "-ansi-alias -fexceptions -Wall -fPIC -pipe -fno-common -g3 -DDEBUG=1 -DOMP_ENABLED -O0 -diag-disable 981,383,1572,869,1418,193 -gcc-version=430 -gxx-name=g++-4.3 -gcc-name=gcc-4.3")
set (RELEASE_COMP_FLAGS "-DRELEASE -DNDEBUG -DQT_NO_DEBUG_OUTPUT -DOMP_ENABLED -fexceptions -Wall -fPIC -pipe -fno-common -O3 -fomit-frame-pointer -diag-disable 981,383,1572,869,1418,193 -gcc-version=430 -gxx-name=g++-4.3 -gcc-name=gcc-4.3")

# OpenMP 
if ("$ENV{OMP_ENABLED}" STREQUAL "YES")
add_definitions (-DOMP_ENABLED -openmp -parallel)
endif ("$ENV{OMP_ENABLED}" STREQUAL "YES")

endif ("$ENV{EM_COMPILER}" STREQUAL "intel")

# gcc compiler flags

if ("$ENV{EM_COMPILER}" STREQUAL "gcc")

set (DEBUG_COMP_FLAGS "-fexceptions -Wall -fPIC -pipe -fno-common -g3 -DDEBUG -DOMP_ENABLED -O0")
set (RELEASE_COMP_FLAGS "-DRELEASE -DNDEBUG -DQT_NO_DEBUG_OUTPUT -DOMP_ENABLED -fexceptions -Wall -fPIC -pipe -fno-common -O3 -fomit-frame-pointer")

# OpenMP 
if ("$ENV{OMP_ENABLED}" STREQUAL "YES")
add_definitions(-DOMP_ENABLED -fopenmp)
endif ("$ENV{OMP_ENABLED}" STREQUAL "YES")

endif ("$ENV{EM_COMPILER}" STREQUAL "gcc")

# MSVC compiler flags

if ("$ENV{EM_COMPILER}" STREQUAL "MSVC")

if(CMAKE_BUILD_TYPE MATCHES "DEBUG")
set(EM_D "d")
else(CMAKE_BUILD_TYPE MATCHES "DEBUG")
set(EM_D "")
endif(CMAKE_BUILD_TYPE MATCHES "DEBUG")

add_definitions(/wd4820 /wd4626 /MP /EHa)
set (DEBUG_COMP_FLAGS "/DDEBUG /MDd /Zi /Od")
set (RELEASE_COMP_FLAGS "/DRELEASE -DQT_NO_DEBUG_OUTPUT /MD /O2")
set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
endif ("$ENV{EM_COMPILER}" STREQUAL "MSVC")

# LINUX

if (UNIX)
set (DEBUG_COMP_FLAGS "${DEBUG_COMP_FLAGS} -DLINUX")
set (RELEASE_COMP_FLAGS "${RELEASE_COMP_FLAGS} -DLINUX")
endif (UNIX)

# OSX

if ("$ENV{EM_PLAT}" STREQUAL "OSX")
set (DEBUG_COMP_FLAGS "${DEBUG_COMP_FLAGS} -DOSX")
set (RELEASE_COMP_FLAGS "${RELEASE_COMP_FLAGS} -DOSX")
endif ("$ENV{EM_PLAT}" STREQUAL "OSX")

# Set the C/C++ compiler flags

set(CMAKE_C_FLAGS_DEBUG ${DEBUG_COMP_FLAGS})
set(CMAKE_CXX_FLAGS_DEBUG ${DEBUG_COMP_FLAGS})
set(CMAKE_C_FLAGS_RELEASE ${RELEASE_COMP_FLAGS})
set(CMAKE_CXX_FLAGS_RELEASE ${RELEASE_COMP_FLAGS})

# The source directories we are building

subdirs(src launcher Ngl Scopes shaders styles)
