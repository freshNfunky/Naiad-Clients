#
# CMAKE project for curve-test
# 
# Copyright (c) 2009 Exotic Matter AB.  All rights reserved.
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

project               (CURVE_TEST)

find_package          (Qt4 4.5 COMPONENTS QtCore QtGui QtOpenGL REQUIRED)

set                   (QT_USE_QTOPENGL TRUE)
include               (${QT_USE_FILE})

include_directories   (. $ENV{NAIAD_PATH}/include /usr/include/nvidia)
link_directories      ($ENV{NAIAD_PATH}/lib /usr/lib64/nvidia)

set                   (CURVE_TEST_SRCS
		       main
		       NsCurveCamera
		       NsCurveViewer
		       NsCurveScene
		       NsCurveGrid
		       NsCurveItem
		       NsCurveKeyframe
		       NsParameter)

set                   (CURVE_TEST_MOC_HDRS 
		       NsCurveViewer.h
		       NsCurveScene.h
		       NsCurveGrid.h
		       NsCurveItem.h
		       NsCurveKeyframe.h)

qt4_wrap_cpp          (CURVE_TEST_MOC_SRCS ${CURVE_TEST_MOC_HDRS})
add_executable        (curve-test ${CURVE_TEST_SRCS} ${CURVE_TEST_MOC_SRCS})
target_link_libraries (curve-test ${QT_LIBRARIES} ${OPENGL_LIBRARIES})
install               (TARGETS curve-test DESTINATION bin)
