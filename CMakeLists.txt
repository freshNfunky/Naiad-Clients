#
# CMAKE project for all Exotic Matter's Naiad Client Applications
#
# Copyright (c) 2012 Exotic Matter AB.  All rights reserved.
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

project(NCLIENTS)

# Set the C/C++ compiler flags

set(CMAKE_C_FLAGS_DEBUG "")
set(CMAKE_CXX_FLAGS_DEBUG "")
set(CMAKE_C_FLAGS_RELEASE "")
set(CMAKE_CXX_FLAGS_RELEASE "")

#
# Standard include & link paths
#
include_directories (. 
		     $ENV{NAIAD_PATH}/server/include/Ni
		     $ENV{NAIAD_PATH}/server/include/Nb
		     $ENV{NAIAD_PATH}/server/include/Ng
		     $ENV{NAIAD_PATH}/server/include/em
		     $ENV{NAIAD_PATH}/server/include/3rdparty)

link_directories ($ENV{NAIAD_PATH}/server/lib
                  $ENV{NAIAD_PATH}/lib
                  $ENV{LMX_ROOT}/win64_x64)

# Intel compiler flags

if ("$ENV{EM_COMPILER}" STREQUAL "intel")
add_definitions(-ansi-alias -fexceptions -Wall -fPIC -pipe -fno-common -diag-disable 981,383,1572,869,1418,193 -gcc-version=440 -gxx-name=g++-4.4 -gcc-name=gcc-4.4 -DOMP_ENABLED)
if(CMAKE_BUILD_TYPE MATCHES "DEBUG")
add_definitions(-g3 -DDEBUG=1 -O0 -inline-level=0)
else()
add_definitions(-DRELEASE -DNDEBUG -DQT_NO_DEBUG_OUTPUT -fast -inline-level=1)
endif(CMAKE_BUILD_TYPE MATCHES "DEBUG")
# OpenMP 
if ("$ENV{OMP_ENABLED}" STREQUAL "YES")
add_definitions (-DOMP_ENABLED -openmp -parallel)
endif ("$ENV{OMP_ENABLED}" STREQUAL "YES")
endif ("$ENV{EM_COMPILER}" STREQUAL "intel")

# gcc compiler flags

if ("$ENV{EM_COMPILER}" STREQUAL "gcc")
add_definitions(-fexceptions -Wall -fPIC -pipe -fno-common -DOMP_ENABLED)
if(CMAKE_BUILD_TYPE MATCHES "DEBUG")
add_definitions(-g3 -DDEBUG -O0)
else()
add_definitions(-DRELEASE -DNDEBUG -DQT_NO_DEBUG_OUTPUT -O3 -fomit-frame-pointer)
endif(CMAKE_BUILD_TYPE MATCHES "DEBUG")
# OpenMP 
if ("$ENV{OMP_ENABLED}" STREQUAL "YES")
add_definitions(-DOMP_ENABLED -fopenmp)
endif ("$ENV{OMP_ENABLED}" STREQUAL "YES")
endif ("$ENV{EM_COMPILER}" STREQUAL "gcc")

# MSVC compiler flags

if ("$ENV{EM_COMPILER}" STREQUAL "MSVC")
add_definitions(/wd4820 /wd4626 /MP /EHa)
if(CMAKE_BUILD_TYPE MATCHES "DEBUG")
set(EM_D "d")
add_definitions(/DDEBUG /MDd /Zi /Od)
else()
set(EM_D "")
add_definitions(/DRELEASE -DQT_NO_DEBUG_OUTPUT /MD /O2)
endif(CMAKE_BUILD_TYPE MATCHES "DEBUG")
if(DIST_BUILD_TYPE)
set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
else()
set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
endif(DIST_BUILD_TYPE)
endif ("$ENV{EM_COMPILER}" STREQUAL "MSVC")

# LINUX

if (UNIX)
add_definitions(-DLINUX)
if(CMAKE_BUILD_TYPE MATCHES "DEBUG")
else()
endif(CMAKE_BUILD_TYPE MATCHES "DEBUG")
endif (UNIX)

# OSX

if ("$ENV{EM_PLAT}" STREQUAL "OSX")
add_definitions(-DOSX)
if(CMAKE_BUILD_TYPE MATCHES "DEBUG")
else()
endif(CMAKE_BUILD_TYPE MATCHES "DEBUG")
endif ("$ENV{EM_PLAT}" STREQUAL "OSX")

# The source directories we are building

subdirs(cmd studio)
