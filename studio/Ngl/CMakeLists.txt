#
# CMAKE project for Exotic Matter's Naiad Graph OpenGL utility library
#
# Copyright (c) 2009-2010 Exotic Matter AB.  All rights reserved.
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

project               (NGL)

find_package          (Qt4 4.6 COMPONENTS QtCore QtGui QtOpenGL REQUIRED)

find_package          (OpenGL)

set                   (QT_USE_QTOPENGL TRUE)
include               (${QT_USE_FILE})

include_directories   (.
                       $ENV{DEV_ROOT_NCLIENTS}/studio/em_gl
                       $ENV{NAIAD_PATH}/server/include/Ni
                       $ENV{NAIAD_PATH}/server/include/Ng
                       $ENV{NAIAD_PATH}/server/include/Nb
                       $ENV{NAIAD_PATH}/server/include/em
                       /usr/include/nvidia)

link_directories      ($ENV{NAIAD_PATH}/server/lib
                       /usr/lib64/nvidia)

file(GLOB_RECURSE NGL_HDRS "*.h")
file(GLOB_RECURSE NGL_SRCS "*.cc")

if(WIN32)
SET (LIB_TYPE STATIC)
else(WIN32)
SET (LIB_TYPE SHARED)
endif(WIN32)

add_library           (Ngl ${LIB_TYPE} ${NGL_SRCS} ${NGL_HDRS})

if ("$ENV{EM_COMPILER}" STREQUAL "intel")
target_link_libraries (Ngl Nb -static-intel ${QT_LIBRARIES} ${OPENGL_LIBRARIES})
endif ("$ENV{EM_COMPILER}" STREQUAL "intel")

if ("$ENV{EM_COMPILER}" STREQUAL "gcc")
target_link_libraries (Ngl Nb ${QT_LIBRARIES} ${OPENGL_LIBRARIES})
endif ("$ENV{EM_COMPILER}" STREQUAL "gcc")

if ("$ENV{EM_COMPILER}" STREQUAL "MSVC")
target_link_libraries (Ngl Nb${EM_D} 
                           ${QT_LIBRARIES} 
                           ${OPENGL_LIBRARIES})
endif ("$ENV{EM_COMPILER}" STREQUAL "MSVC")

install               (TARGETS Ngl DESTINATION clients/nstudio/lib)

