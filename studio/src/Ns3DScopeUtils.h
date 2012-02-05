// -----------------------------------------------------------------------------
//
// Ns3DScopeUtils.h
//
// Utilities & Tools for scopes.
//
// Copyright (c) 2011 Exotic Matter AB.  All rights reserved. 
//
// This file is part of Open Naiad Studio..
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// * Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// * Neither the name of Exotic Matter AB nor its contributors may be used to
// endorse or promote products derived from this software without specific
// prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// -----------------------------------------------------------------------------

#ifndef NS3D_SCOPE_UTILS_H
#define NS3D_SCOPE_UTILS_H

#include "Ns3DResourceObject.h"
#include "NsQuery.h"
#include <NglUtils.h>
#include <NglShaderProgram.h>
#include <NglVertexAttrib.h>
#include <NglViewport.h>

#include <em_glmat44_algo.h>

#include <cstring>
#include <QtGlobal>

// -----------------------------------------------------------------------------

inline Nb::String
shaderPath() 
{
    std::stringstream ss;
    ss << qgetenv("NAIAD_PATH").constData() << "/clients/nstudio/shaders/";
    return ss.str();
}

// -----------------------------------------------------------------------------

inline QGLFramebufferObject*
syncFramebufferObject(const NtString&   clientName,
                      const NtString&   bufferName,
                      Ns3DResourceObject* robject,
                      const int           width,
                      const int           height,
                      const GLenum        internalFormat)
{
    QGLFramebufferObject* buffer = 
        robject->queryMutableFramebufferObject(clientName,bufferName);
    
    if (!buffer || buffer->width() != width || buffer->height() != height) {
        robject->destroyFramebufferObject(clientName,bufferName);
        buffer = robject->createFramebufferObject(
            clientName,
            bufferName,
            width, 
            height,
            QGLFramebufferObject::NoAttachment,
            GL_TEXTURE_2D,
            internalFormat
            );
    }
    
    return buffer;
}

// -----------------------------------------------------------------------------

inline void
computeModelViewXform(const NtVec3f& pos,
                      const NtVec3f& view,
                      const NtVec3f& worldUp,
                      GLfloat*         modelviewXf)
{
    const NtVec3f right(em::normalized(em::cross(view, worldUp)));    
    const NtVec3f up(em::normalized(em::cross(right, view)));
    
    modelviewXf[0] = right[0];                  // Column 0
    modelviewXf[1] = up[0];
    modelviewXf[2] = -view[0];
    modelviewXf[3] = 0.f;
    modelviewXf[4] = right[1];                  // Column 1
    modelviewXf[5] = up[1];
    modelviewXf[6] = -view[1];
    modelviewXf[7] = 0.f;
    modelviewXf[8] = right[2];                  // Column 2
    modelviewXf[9] = up[2];
    modelviewXf[10] = -view[2];
    modelviewXf[11] = 0.f;
    modelviewXf[12] = -(em::dot(right, pos));   // Column 3
    modelviewXf[13] = -(em::dot(up, pos));
    modelviewXf[14] = em::dot(view, pos);
    modelviewXf[15] = 1.f;
}

// -----------------------------------------------------------------------------

inline void
computeClipBoxXforms(const Nb::Value3f*  translateParam,
                     const Nb::Value3f*  rotateParam,
                     const Nb::Value3f*  scaleParam,
                     em::glmat44f&       clipXform,
                     em::glmat44f&       invClipXform)
{
    // Create clip-box transform (and inverse).

    NtVec3f translate(
        translateParam->eval(Nb::ZeroTimeBundle, 0),
        translateParam->eval(Nb::ZeroTimeBundle, 1),
        translateParam->eval(Nb::ZeroTimeBundle, 2)
        );
    
    NtVec3f rotate(
        rotateParam->eval(Nb::ZeroTimeBundle, 0),
        rotateParam->eval(Nb::ZeroTimeBundle, 1),
        rotateParam->eval(Nb::ZeroTimeBundle, 2)
        );
    
    NtVec3f scale(
        scaleParam->eval(Nb::ZeroTimeBundle, 0),
        scaleParam->eval(Nb::ZeroTimeBundle, 1),
        scaleParam->eval(Nb::ZeroTimeBundle, 2)
        );
    
    clipXform = em::make_transform(translate, rotate, scale);

    em::glmat44d xfd;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            xfd[i][j] = static_cast<double>(clipXform[i][j]);
        }
    }
    //em::invert(clipXform, invClipXform);
    em::glmat44d invXfd;
    em::invert(xfd, invXfd);

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            invClipXform[i][j] = static_cast<float>(invXfd[i][j]);
        }
    }

}

// -----------------------------------------------------------------------------

inline void
drawCellShadow(const Ns3DResourceObject* robject,
               const Nb::Value1e*        shadowCellSizeParam,
               const float mcs = 0.1)
{        
    float cs;
    if(shadowCellSizeParam->eval(Nb::ZeroTimeBundle)=="Body" &&
       robject->constLayoutPtr()) {
        cs=robject->constLayoutPtr()->cellSize();
    }
    else { // Master
        //cs=Ns::masterCellSize();
        cs = evalParam1f("Global.Master Cell Size");
        //EM_ASSERT(false);
    }
    
    NtVec3f bmin, bmax;
    robject->bounds(bmin,bmax);
    
    glLineWidth(1);
    
    float xend=bmin[0]+cs, zend=bmin[2]+cs;
    for(; xend<bmax[0]; xend += cs) ;
    for(; zend<bmax[2]; zend += cs) ;
    
    glBegin(GL_LINES);
    
    const NtVec3f gridColor(0.85f);
    
    const float y=bmin[1]-cs*0.5f;
    
    for(float x=bmin[0]; x<=xend; x += cs) {
        glColor3f(gridColor[0],gridColor[1],gridColor[2]);
        glVertex3f(x,y,bmin[2]);
        glVertex3f(x,y,zend);
    }
    
    for(float z=bmin[2]; z<=zend; z += cs) {
        glColor3f(gridColor[0],gridColor[1],gridColor[2]);            
        glVertex3f(bmin[0],y,z);
        glVertex3f(xend,y,z);
    }
    
    glEnd();
}

// -----------------------------------------------------------------------------

#endif // NS3D_SCOPE_UTILS_H
