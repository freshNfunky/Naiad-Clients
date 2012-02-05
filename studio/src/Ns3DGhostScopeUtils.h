// -----------------------------------------------------------------------------
//
// Ns3DGhostScopeUtils.h
//
// Utilities & Tools for ghost scopes.
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
//
//
// -----------------------------------------------------------------------------

#ifndef NS3D_GHOST_SCOPE_UTILS_H
#define NS3D_GHOST_SCOPE_UTILS_H

#include "Ns3DScopeUtils.h"

#include <NglShaderProgram.h>
#include <NglVertexAttrib.h>
#include <NglTexture3D.h>
#include <NglSuperTile.h>
#include <NglSuperTileLayout.h>
#include <NglViewport.h>
#include <NglSlicing.h>

// -----------------------------------------------------------------------------

inline void
computeLightXforms(const Ngl::SuperTileLayout* superLayout,
                   const Nb::Value3f*          lightDirParam,
                   GLfloat*                    lightModelviewXf,
                   GLfloat*                    lightProjectionXf,
                   const NtVec3f&            worldUp = NtVec3f(0,1,0))
{
    NtVec3f superLayoutMin, superLayoutMax;
    superLayout->bounds(superLayoutMin, superLayoutMax);
    
    const NtVec3f superLayoutCenter(
        0.5f*(superLayoutMin[0] + superLayoutMax[0]),
        0.5f*(superLayoutMin[1] + superLayoutMax[1]),
        0.5f*(superLayoutMin[2] + superLayoutMax[2])
        );
    
    const NtVec3f superLayoutHalfDim(
        superLayoutMax[0] - superLayoutCenter[0],
        superLayoutMax[1] - superLayoutCenter[1],
        superLayoutMax[2] - superLayoutCenter[2]
        );
    
    const GLfloat superLayoutRadius(
        sqrtf(superLayoutHalfDim[0]*superLayoutHalfDim[0] +
              superLayoutHalfDim[1]*superLayoutHalfDim[1] +
              superLayoutHalfDim[2]*superLayoutHalfDim[2])
        );
    
    // Normalized light direction from GUI.
    
    const NtVec3f lightDir(
        em::normalized(
            NtVec3f(
                lightDirParam->eval(Nb::ZeroTimeBundle, 0),
                lightDirParam->eval(Nb::ZeroTimeBundle, 1),
                lightDirParam->eval(Nb::ZeroTimeBundle, 2)
                )
            )
        );
    
    // Light is positioned outside the super-tile-layout.
    
    const NtVec3f lightPos(
        superLayoutCenter - (2.f*superLayoutRadius)*lightDir
        );

    computeModelViewXform(lightPos, lightDir, worldUp, lightModelviewXf);
    
    // Symmetrical orthograpic perspective transformation.
    // The entire super-tile-layout is inside the light frustum.
    
    const GLfloat nearDepth(0.5f*superLayoutRadius);
    const GLfloat farDepth(4.f*superLayoutRadius);
    const GLfloat invDepthRange(1.f/(farDepth - nearDepth));
    const GLfloat invSuperLayoutRadius(1.f/superLayoutRadius);
    
    lightProjectionXf[0]  = invSuperLayoutRadius;  // Column 0
    lightProjectionXf[1]  = 0.f;
    lightProjectionXf[2]  = 0.f;
    lightProjectionXf[3]  = 0.f;
    lightProjectionXf[4]  = 0.f;                   // Column 1
    lightProjectionXf[5]  = invSuperLayoutRadius;
    lightProjectionXf[6]  = 0.f;
    lightProjectionXf[7]  = 0.f;
    lightProjectionXf[8]  = 0.f;                   // Column 2
    lightProjectionXf[9]  = 0.f;
    lightProjectionXf[10] = -2.f*invDepthRange;
    lightProjectionXf[11] = 0.f;
    lightProjectionXf[12] = 0.f;                   // Column 3
    lightProjectionXf[13] = 0.f;
    lightProjectionXf[14] = -((farDepth + nearDepth)*invDepthRange);
    lightProjectionXf[15] = 1.f;   
}

// -----------------------------------------------------------------------------

inline void
drawGhostSlicedClipBox(const NtString&      clientName,
                       const NtString&      fieldName,
                       Ns3DResourceObject*    robject,
                       Ngl::ShaderProgram*    shader1,
                       Ngl::ShaderProgram*    shader2,
                       Ngl::ShaderProgram*    shader3,
                       const Ngl::Viewport&   vp,
                       //Nb::Vec2f&             valRange,
                       const Nb::Value3f*     translateParam,
                       const Nb::Value3f*     rotateParam,
                       const Nb::Value3f*     scaleParam,
                       const Nb::Value1i*     sliceCountParam,
                       const Nb::Value3f*     supersamplingParam,
                       const Nb::Value1i*     lightBufSizeParam,
                       const Nb::Value3f*     lightDirParam,
                       const Nb::Value3f*     lightColorParam,
                       const Nb::Value1f*     lightAlphaParam,
                       const Nb::Value3f*     reflColorParam,
                       const Nb::Value1f*     reflAlphaParam)
{   
    const int sliceCount(sliceCountParam->eval(Nb::ZeroTimeBundle));    
    const int lightBufSize(lightBufSizeParam->eval(Nb::ZeroTimeBundle));

    // Ensure the window coordinate vertex buffer exists.

    const Nb::Vec2f winCoords[] =
    {
        Nb::Vec2f(0.f, 0.f),
        Nb::Vec2f(1.f, 0.f),
        Nb::Vec2f(1.f, 1.f),
        Nb::Vec2f(0.f, 1.f)
    };

    const Ngl::VertexBuffer* winVtxBuf = 
        robject->queryConstVertexBuffer(clientName,"win");
    if(!winVtxBuf)
        winVtxBuf = robject->createVertexBuffer(
            clientName,
            "win",
            4*sizeof(Nb::Vec2f),
            &winCoords[0],
            GL_ARRAY_BUFFER,
            GL_STATIC_DRAW
            );

    // Compute clip-box matrices.
    
    em::glmat44f clipBoxXform;
    em::glmat44f invClipBoxXform;
    computeClipBoxXforms(
        translateParam,
        rotateParam,
        scaleParam,
        clipBoxXform,
        invClipBoxXform);

    const Ngl::SuperTileLayout* superLayout = 
        robject->queryConstSuperTileLayout();
    if(!superLayout)
        superLayout = robject->createSuperTileLayout(&clipBoxXform[0][0]);

    // Modelview & Projection from pov of light

    GLfloat lightModelviewXf[16];
    GLfloat lightProjectionXf[16];
    computeLightXforms(
        superLayout,
        lightDirParam,
        lightModelviewXf,
        lightProjectionXf
        );           

    // Sync Light Buffer to (lightBufSize,lightBufSize) 

    QGLFramebufferObject* lightBuffer = syncFramebufferObject(
        clientName,
        "lightBuffer",
        robject,
        lightBufSize,
        lightBufSize,
        GL_RGBA32F_ARB
        );   

    // Modelview & Projection from pov of eye
    
    GLfloat eyeModelviewXf[16];
    GLfloat eyeProjectionXf[16];
    glGetFloatv(GL_MODELVIEW_MATRIX,  eyeModelviewXf);
    glGetFloatv(GL_PROJECTION_MATRIX, eyeProjectionXf);
        
    // Compute "sliceModelview" matrix based on eye & light directions.
    
    const NtVec3f eyePos(
        -eyeModelviewXf[12],-eyeModelviewXf[13],-eyeModelviewXf[14]
        );
    
    const NtVec3f eyeView(
        -eyeModelviewXf[2],-eyeModelviewXf[6],-eyeModelviewXf[10]
        );
    
    const NtVec3f lightDir(
        -lightModelviewXf[2],-lightModelviewXf[6],-lightModelviewXf[10]
        );
    
    NtVec3f superLayoutMin, superLayoutMax;
    superLayout->bounds(superLayoutMin, superLayoutMax);
    
    const NtVec3f superLayoutCenter(
        0.5f*(superLayoutMin[0] + superLayoutMax[0]),
        0.5f*(superLayoutMin[1] + superLayoutMax[1]),
        0.5f*(superLayoutMin[2] + superLayoutMax[2])
        );
    
    NtVec3f sliceDir(lightDir + eyeView);
    
    // Over operator:  Back-to-Front
    // Under operator: Front-to-Back
    //
    // "For the Over operator, the source blending factor is set to 1 and
    //  the destination blending factor is set to (1 – source alpha). For
    //  the Under operator, the source blending factor is set to
    //  (1 – destination alpha) and the destination factor is set to 1"
    //
    // -- GPU Gems
    
    GLenum  srcFactor = GL_ONE_MINUS_DST_ALPHA;            // Under.
    GLenum  dstFactor = GL_ONE;
    
    if (em::dot(eyeView, lightDir) < 0.f) {
        sliceDir = lightDir - eyeView;        
        srcFactor = GL_ONE;                        // Over.
        dstFactor = GL_ONE_MINUS_SRC_ALPHA;
    }
    
    em::normalize(sliceDir);    
    const NtVec3f slicePos(superLayoutCenter - em::mag(eyePos)*sliceDir);
    
    const NtVec3f worldUp(0,1,0);

    const em::vec3f right(em::normalized(em::cross(worldUp, -sliceDir)));
    const em::vec3f up(em::cross(-sliceDir, right));
    const em::glmat44f sliceModelViewXform =
        make_cam_view(right,
                      up,
                      -sliceDir,
                      slicePos);
    
     // Create the vertex buffer by slicing the clip box
    GLfloat sliceSpacing;    
    Ngl::VertexBuffer* sliceVtxBuf = 
        sliceClipBoxVertexBuffer(clientName,
                                 robject,
                                 sliceCountParam,
                                 clipBoxXform,
                                 sliceModelViewXform,
                                 &sliceSpacing);

    Nb::Vec2f valRange(
        (std::numeric_limits<float>::max)(),
        -(std::numeric_limits<float>::max)());
    robject->computeSuperTileTextures(
        clientName,
        fieldName,
        NtVec3f(
            supersamplingParam->eval(Nb::ZeroTimeBundle, 0),
            supersamplingParam->eval(Nb::ZeroTimeBundle, 1),
            supersamplingParam->eval(Nb::ZeroTimeBundle, 2)),
        &clipBoxXform[0][0],
        true,
        0,
        &valRange
        );

    for(int st = 0; st < superLayout->superTileCount(); ++st){
        const Ngl::Texture3D* tex3D(
            robject->queryConstTexture3D(clientName, fieldName, st)
            );
        tex3D->computeMinMax(valRange[0], valRange[1]);
    }
    //valRange[0] = 0.f;

    // OpenGL fixed pipeline state.
    
    Ngl::FlipState<GL_BLEND>      blendState;
    Ngl::FlipState<GL_DEPTH_TEST> depthTestState;
    Ngl::FlipState<GL_TEXTURE_2D> tex2DState;
    Ngl::FlipState<GL_TEXTURE_3D> tex3DState;
    Ngl::FlipState<GL_BLEND>::enable();
    Ngl::FlipState<GL_DEPTH_TEST>::disable();
    Ngl::FlipState<GL_TEXTURE_2D>::enable();
    Ngl::FlipState<GL_TEXTURE_3D>::enable();
    
    //Ngl::FlipState<GL_ALPHA_TEST> alphaTestState;
    //Ngl::FlipState<GL_ALPHA_TEST>::disable();
    
    // Before using our FBO's, check if there is a currently bound FBO.
    // This would be the case when playblasting. In this case, store
    // the binding and re-enable it for the final compositing (pass 3).
    // If no FBO was bound this value is zero, and we render to the screen.
    
    GLint fbBinding;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &fbBinding);

    // Sync Eye Buffer to Viewport dimensions

    QGLFramebufferObject* eyeBuffer = syncFramebufferObject(
        clientName,
        "eyeBuffer",
        robject,
        vp.width(),
        vp.height(),
        GL_RGBA32F_ARB
        );

    // Clear the Eye Buffer

    eyeBuffer->bind();
    glPushAttrib(GL_COLOR_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glPopAttrib();
    eyeBuffer->release();

    // Initialize light buffer to light color.

    const Nb::Vec4f lightColor(
        lightColorParam->eval(Nb::ZeroTimeBundle, 0),
        lightColorParam->eval(Nb::ZeroTimeBundle, 1),
        lightColorParam->eval(Nb::ZeroTimeBundle, 2),
        lightAlphaParam->eval(Nb::ZeroTimeBundle)
        );        
    lightBuffer->bind();
    glPushAttrib(GL_COLOR_BUFFER_BIT);
    glClearColor(lightColor[0],lightColor[1],lightColor[2],lightColor[3]);
    glClear(GL_COLOR_BUFFER_BIT);
    glPopAttrib();
    lightBuffer->release();
    
    // Get parameters for rendering

    const GLfloat invValRange(1.f/(valRange[1] - valRange[0]));    
    const Nb::Vec4f reflectiveColor(
        reflColorParam->eval(Nb::ZeroTimeBundle, 0),
        reflColorParam->eval(Nb::ZeroTimeBundle, 1),
        reflColorParam->eval(Nb::ZeroTimeBundle, 2),
        reflAlphaParam->eval(Nb::ZeroTimeBundle)
        );    
    const GLfloat samplingRate(
        sliceSpacing/robject->constLayoutPtr()->cellSize()
        );

#if 1
    std::cerr << "Min: " << valRange[0] << " | Max: " << valRange[1] << "\n";
#endif

    // Bind/Copy uniforms on Shader1.
    
    shader1->storeUniform4m("eyeModelview", eyeModelviewXf);
    shader1->storeUniform4m("eyeProjection", eyeProjectionXf);
    shader1->storeUniform4m("lightModelview", lightModelviewXf);
    shader1->storeUniform4m("lightProjection", lightProjectionXf);
    shader1->storeUniform4m("wsxToClipBox", &invClipBoxXform[0][0]);
    
    shader1->storeUniform1f("samplingRate", samplingRate);
    shader1->storeUniform1f("minVal", valRange[0]);
    shader1->storeUniform1f("invValRange", invValRange);
    shader1->storeUniform4f("reflectiveColor", reflectiveColor);
    
    shader1->storeUniform1i("ghostTex", 0); // Texture unit 0.
    shader1->storeUniform1i("lightTex", 1);  // Texture unit 1.
    
    // Bind/Copy uniforms on Shader2.
    
    shader2->storeUniform4m("lightModelview", lightModelviewXf);
    shader2->storeUniform4m("lightProjection", lightProjectionXf);
    shader2->storeUniform4m("wsxToClipBox", &invClipBoxXform[0][0]);
    
    shader2->storeUniform1f("samplingRate", samplingRate);
    shader2->storeUniform1f("minVal", valRange[0]);
    shader2->storeUniform1f("invValRange", invValRange);
    
    shader2->storeUniform1i("ghostTex", 0); // Texture unit 0.
        
    for(int superTile(0); superTile<superLayout->superTileCount(); ++superTile){
        // TODO: Check if super-tile intersects clip-box.
        
        NtVec3f min, max;        
        superLayout->bounds(min, max);
        const NtVec3f superTileMin(min);
        const NtVec3f invSuperTileRange(
            1.f/(max[0] - min[0]),
            1.f/(max[1] - min[1]),
            1.f/(max[2] - min[2])
            );
        
        // Update super-tile shader uniforms only.
        
        shader1->storeUniform3f("superTileMin", superTileMin);
        shader1->storeUniform3f("invSuperTileRange", invSuperTileRange);
        
        shader2->storeUniform3f("superTileMin", superTileMin);
        shader2->storeUniform3f("invSuperTileRange", invSuperTileRange);
        
        // Pass super-tile information to shader.
        
        shader1->use();
        shader1->uploadUniforms(Nb::ZeroTimeBundle);
        shader1->unuse();
        
        shader2->use();
        shader2->uploadUniforms(Nb::ZeroTimeBundle);
        shader2->unuse();
        
        // TODO: Check errors!?
        const Ngl::Texture3D* tex3D(
            robject->queryConstTexture3D(clientName, fieldName, superTile)
            );
        
        // Bind ghost texture to texture unit 0.
        
        glActiveTexture(GL_TEXTURE0);
        tex3D->bind();
        
        // Bind light buffer to texture unit 1.
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, lightBuffer->texture());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        
        // Render slices.
        
        for (GLint s(0); s < sliceCount; ++s) {
            // Pass 1, set blending depending on eye/light configuration,
            // computed when slicing the clip-box.

            eyeBuffer->bind();
            glPushAttrib(GL_VIEWPORT_BIT);
            glViewport(0, 0, eyeBuffer->width(), eyeBuffer->height());
            
            glBlendFunc(srcFactor, dstFactor);
            
            Ngl::VertexAttrib::connect(
                shader1->constAttrib("wsx"),*sliceVtxBuf
                );
            shader1->use();
            glDrawArrays(GL_QUADS, s*4, 4);
            shader1->unuse();
            
            Ngl::VertexAttrib::disconnect(shader1->constAttrib("wsx"));
            
            glPopAttrib();  // GL_VIEWPORT_BIT
            eyeBuffer->release();
            
            // Pass 2, make light buffer current render target.
            
            lightBuffer->bind();
            glPushAttrib(GL_VIEWPORT_BIT);
            glViewport(0, 0, lightBuffer->width(), lightBuffer->height());
            
            // Always render front-to-back, using over-blending.
            
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);  // Over
            
            Ngl::VertexAttrib::connect(
                shader2->constAttrib("wsx"),*sliceVtxBuf
                );
            shader2->use();
            glDrawArrays(GL_QUADS, s*4, 4);
            shader2->unuse();
            
            Ngl::VertexAttrib::disconnect(shader2->constAttrib("wsx"));
            
            glPopAttrib();  // GL_VIEWPORT_BIT
            lightBuffer->release();
        }
        
        // Release light texture.
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glActiveTexture(GL_TEXTURE0);
        tex3D->unbind();
    }
    
    // Pass 3, composite eye-buffer onto the screen, or the FBO that was
    // bound before we started rendering.
    
    if (0 != fbBinding) {
        // A framebuffer was bound when we started drawing, re-enable
        // that framebuffer.
        
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbBinding);
    }
    
    // glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0) =
    const GLfloat orthoProjection[] =
    {
        2.f, 0.f, 0.f, 0.f,
        0.f, 2.f, 0.f, 0.f,
        0.f, 0.f, -1.f, 0.f,
        -1.f, -1.f, 0.f, 1.f
    };
    
    glPushAttrib(GL_VIEWPORT_BIT);
    glViewport(0, 0, vp.width(), vp.height());
    
    // Bind eye buffer to texture unit 0.
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, eyeBuffer->texture());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    Ngl::VertexAttrib::connect(shader3->constAttrib("win"),*winVtxBuf);
    
    shader3->storeUniform4m("orthoProjection", orthoProjection);
    shader3->storeUniform1i("eyeTex", 0); // Texture unit 0.
    
    shader3->use();
    shader3->uploadUniforms(Nb::ZeroTimeBundle);
    glDrawArrays(GL_QUADS, 0, 4);
    shader3->unuse();
    
    Ngl::VertexAttrib::disconnect(shader3->constAttrib("win"));
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glPopAttrib();  // GL_VIEWPORT_BIT
    
    if (0 != fbBinding) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    }
    
#if 0
    const QImage eyeImg(eyeBuffer->toImage());
    eyeImg.save("../../../eye.png");
    
    const QImage lightImg(lightBuffer->toImage());
    lightImg.save("../../../light.png");
#endif
}

// -----------------------------------------------------------------------------

#endif // NS3D_GHOST_SCOPE_UTILS_H
