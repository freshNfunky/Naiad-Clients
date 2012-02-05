// -----------------------------------------------------------------------------
//
// Ns3DSliceScopeUtils.h
//
// Utilities & Tools for non-ghost slicing scopes.
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

#ifndef NS3D_SLICE_SCOPE_UTILS_H
#define NS3D_SLICE_SCOPE_UTILS_H

#include "Ns3DScopeUtils.h"
#include "Ns3DOpBoxItem.h"

#include <em_glmat44_algo.h>

#include <NglSlicing.h>
#include <NglTexture3D.h>
#include <NglSuperTile.h>
#include <NglSuperTileLayout.h>

// -----------------------------------------------------------------------------

inline Ngl::VertexBuffer*
sliceClipBoxVertexBuffer(const NtString&     clientName,
                         Ns3DResourceObject* robject,                         
                         const Nb::Value1i*  sliceCountParam,
                         const em::glmat44f  xfModel,
                         const em::glmat44f  xfView,
                         GLfloat*            spacing=0)
{   
    const int sliceCount(sliceCountParam->eval(Nb::ZeroTimeBundle));

    // We begin by constructing/updating the vertex buffer holding
    // the slices.

    const em::glmat44f mv(xfView*xfModel);

    em::vec4f eye[8];
    em::vec4f eyeMin;
    em::vec4f eyeMax;
    em::projectEye(mv,
                   &Ns3DOpBoxItem::vtx[0],
                   eye,
                   8,
                   eyeMin,
                   eyeMax);

    std::vector<em::vec3f> sliceWsx;

    if (!Ngl::slice(xfView,
                    eyeMin,
                    eyeMax,
                    sliceCount,
                    &sliceWsx,
                    spacing)) {
        robject->destroyVertexBuffer(clientName, "slice");
        return 0; // No (visible) slices.
    }

    Ngl::VertexBuffer* sliceVtxBuf = 
        robject->queryMutableVertexBuffer(clientName,"slice");
    
    const int sliceSize(sizeof(NtVec3f)*sliceWsx.size());

    if (!sliceVtxBuf || sliceSize != sliceVtxBuf->size()) {
        robject->destroyVertexBuffer(clientName,"slice");
        robject->createVertexBuffer(
            clientName, "slice", sliceSize, 0, GL_ARRAY_BUFFER, GL_STATIC_DRAW
            );
        sliceVtxBuf = robject->queryMutableVertexBuffer(clientName,"slice");
    }
    
    sliceVtxBuf->setSubData(0, sliceSize, &sliceWsx[0]);

    return sliceVtxBuf;
}

// -----------------------------------------------------------------------------

inline void
drawSlicedClipBox(const NtString&   clientName,
                  const NtString&   fieldName,
                  const int           component,
                  Ns3DResourceObject* robject,
                  Ngl::ShaderProgram* shader,
                  const Nb::Value3f*  translateParam,
                  const Nb::Value3f*  rotateParam,
                  const Nb::Value3f*  scaleParam,
                  const Nb::Value1i*  sliceCountParam,
                  const Nb::Value3f*  supersamplingParam,
                  Nb::Value1f*        minValue=0,
                  Nb::Value1f*        maxValue=0)
{       
    // Compute clip-box matrices.

    em::glmat44f clipXform;
    em::glmat44f invClipXform;
    computeClipBoxXforms(
        translateParam,
        rotateParam,
        scaleParam,
        clipXform,
        invClipXform);

    // Camera matrices.

    em::glmat44f modelViewXform;
    em::glmat44f projectionXform;
    glGetFloatv(GL_PROJECTION_MATRIX, &projectionXform[0][0]);
    glGetFloatv(GL_MODELVIEW_MATRIX, &modelViewXform[0][0]);
    const em::glmat33f normalXform(em::make_cam_normal_matrix(modelViewXform));

    // Update uniforms in vertex shader
    
    shader->storeUniform4m("modelview", &modelViewXform[0][0]);
    shader->storeUniform4m("projection", &projectionXform[0][0]);
    
    // Update uniforms in fragment shader
    
    shader->storeUniform4m("worldToBox", &invClipXform[0][0]);
    shader->storeUniform3m("normalMatrix", &normalXform[0][0]);

    // Create the vertex buffer by slicing the clip box

    Ngl::VertexBuffer* sliceVtxBuf = 
        sliceClipBoxVertexBuffer(
            clientName,
            robject,
            sliceCountParam,
            clipXform,
            modelViewXform);

    if(!sliceVtxBuf)
        return;

    const GLsizei minCount=
        Ngl::VertexAttrib::connect(
            shader->constAttrib("position"), *sliceVtxBuf
            );

    // Make the super-tile textures for this scope & channel.

    const Ngl::SuperTileLayout* superLayout = 
        robject->queryConstSuperTileLayout();
    if(!superLayout) {
        superLayout = robject->createSuperTileLayout(&clipXform[0][0]);
    }

    Nb::Vec2f valRange(minValue ? minValue->eval(Nb::ZeroTimeBundle) : 0,
                       maxValue ? maxValue->eval(Nb::ZeroTimeBundle) : 0);

    robject->computeSuperTileTextures(
        clientName,
        fieldName,
        NtVec3f(supersamplingParam->eval(Nb::ZeroTimeBundle, 0),
                supersamplingParam->eval(Nb::ZeroTimeBundle, 1),
                supersamplingParam->eval(Nb::ZeroTimeBundle, 2)),
        &clipXform[0][0],
        true, // gradientTextures
        component,
        &valRange);

    if(minValue) {
        std::stringstream ss; ss << valRange[0];
        minValue->setExpr(ss.str());
    }

    if(maxValue) {
        std::stringstream ss; ss << valRange[1];
        maxValue->setExpr(ss.str());
    }

    // We continue by rendering the slice vertex buffer.
    
    shader->use();    
        
    for(int superTile=0; superTile<superLayout->superTileCount(); ++superTile) {

        // TODO: Check if super-tile intersects clip-box!
        
        NtVec3f wsMin, wsMax;
        superLayout->superTile(superTile).bounds(wsMin, wsMax);
        
        const NtVec3f invWsRange(
            1.f/(wsMax[0] - wsMin[0]),
            1.f/(wsMax[1] - wsMin[1]),
            1.f/(wsMax[2] - wsMin[2])
            );
        
        shader->storeUniform3f("wsMin", wsMin);
        shader->storeUniform3f("invWsRange", invWsRange);
        shader->uploadUniforms(Nb::ZeroTimeBundle);
        
        const Ngl::Texture3D* tex3D = 
            robject->queryConstTexture3D(clientName, fieldName, superTile);
        tex3D->bind();

        glDrawArrays(GL_QUADS, 0, minCount);
    }

    shader->unuse();
    
    Ngl::VertexAttrib::disconnect(shader->constAttrib("position"));
}

// -----------------------------------------------------------------------------

#endif // NS3D_SLICE_SCOPE_UTILS_H
