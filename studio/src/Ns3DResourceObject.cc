// -----------------------------------------------------------------------------
//
// Ns3DResourceObject.cc
//
// ...
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

#include "Ns3DResourceObject.h"

#include <NglSlicing.h>
#include <NglShaderProgram.h>
#include <NglVertexAttrib.h>
#include <NglSuperTileLayout.h>
#include <NglSuperTile.h>
#include <NglTextureUtils.h>
#include <NglVertexBuffer.h>

#include <Nbx.h>    // NB_THROW
#include <NbLog.h>  // NB_WARNING

#include <sstream>


// Ns3DResourceObject
// ---------------
//! Constructor.

Ns3DResourceObject::Ns3DResourceObject()
    : _superLayout(0)
{
#if 0
    std::cerr << "Create Ns3DResourceObject" << std::endl;
#endif
}


// Ns3DResourceObject
// ---------------
//! Destructor.

Ns3DResourceObject::~Ns3DResourceObject()
{
    // Free resources.
    //

    // Super-tile layout

    delete _superLayout;

    // Textures

    for (_Tex3DMap::iterator iter = _tex3DMap.begin();
         iter != _tex3DMap.end();
         ++iter)
    {
        delete iter->second;
    }

    // Vertex buffers

    for (_VertexBufferMap::iterator iter = _vtxBufMap.begin();
         iter != _vtxBufMap.end();
         ++iter)
    {
        delete iter->second;
    }

    // Framebuffers

    for (_FBOMap::iterator iter = _fboMap.begin();
         iter != _fboMap.end();
         ++iter)
    {
        delete iter->second;
    }

#if 0
    std::cerr << "Destroy Ns3DResourceObject\n";
#endif
}


// createConstSuperTileLayout
// --------------------------
//! Create and store a super-tile layout.

const Ngl::SuperTileLayout*
Ns3DResourceObject::createSuperTileLayout(const float clipXform[16],
                                          const int   connectivity)
{
    if(_superLayout) {
        NB_WARNING("Super-tile layout '" << name() << "' already exists");
    } else {
        _superLayout = 
            new Ngl::SuperTileLayout(
                constLayoutPtr(),clipXform,connectivity
                ); // May throw.
    }

    return _superLayout;
}


// createVertexBuffer
// ------------------
//! Create vertex buffer from raw data.

Ngl::VertexBuffer*
Ns3DResourceObject::createVertexBuffer(const NtString& clientName,
                                       const NtString& bufferName,
                                       const GLsizeiptr  size,
                                       const GLvoid*     data,
                                       const GLenum      target,
                                       const GLenum      usage)
{
    Ngl::VertexBuffer* vtxBuf=
        queryMutableVertexBuffer(clientName,bufferName);

    const NtString resourceName(longName(clientName,bufferName));

    qDebug() << "Ns3DResourceObject::createVertexBuffer - '" 
             << resourceName.str().c_str() << "'";

    if(vtxBuf) {
        NB_THROW("Vertex buffer '" << resourceName << "' already exists");
    } else {
        vtxBuf = new Ngl::VertexBuffer(size, data, target, usage);
        _vtxBufMap.insert(_VertexBufferMap::value_type(resourceName, vtxBuf));
    }

    return vtxBuf;
}


// createTexture3D
// ---------------
//! Create texture from raw data.

const Ngl::Texture3D*
Ns3DResourceObject::createTexture3D(const int         superTile,
                                    const NtString& clientName,
                                    const NtString& bufferName,
                                    const GLvoid*     data,
                                    const GLsizei     width,
                                    const GLsizei     height,
                                    const GLsizei     depth,
                                    const GLint       wrapS,
                                    const GLint       wrapT,
                                    const GLint       wrapR,
                                    const GLint       internalFormat,
                                    const GLenum      format,
                                    const GLenum      type,
                                    const GLint       border,
                                    const GLint       minFilter,
                                    const GLint       magFilter)
{
    Ngl::Texture3D* tex3D=
        queryMutableTexture3D(clientName,bufferName,superTile);

    const NtString resourceName(longName(clientName,bufferName));

    if(tex3D) {
        NB_WARNING("Texture3D '" << resourceName << "' already exists");
    } else {
        tex3D = new Ngl::Texture3D(
            data,
            width, height, depth,
            wrapS, wrapT, wrapR,
            internalFormat,
            format,
            type,
            border,
            minFilter, magFilter
            );
    
        _tex3DMap.insert(_Tex3DMap::value_type(resourceName, tex3D));
    }

    return tex3D;
}


// createFramebufferObject
// -----------------------
//! Create Framebuffer Object

QGLFramebufferObject*
Ns3DResourceObject::createFramebufferObject(const NtString&   clientName,
                                            const NtString&   bufferName,
                                            const int           width, 
                                            const int           height, 
                                            const FBOAttachment attachment, 
                                            const GLenum        target, 
                                            const GLenum        internalFormat)
{
    QGLFramebufferObject* fbo=
        queryMutableFramebufferObject(clientName,bufferName);

    const NtString resourceName(longName(clientName,bufferName));

    if(fbo) {
        NB_WARNING("FBO '" << resourceName << "' already exists");
    } else {
        fbo = new QGLFramebufferObject(
            width,height,attachment,target,internalFormat
            );
        _fboMap.insert(_FBOMap::value_type(resourceName, fbo));
    }

    return fbo;
}


// queryConstSuperTileLayout
// --------------------
//! Query a super-tile layout by name.

const Ngl::SuperTileLayout*
Ns3DResourceObject::queryConstSuperTileLayout() const
{
    return _superLayout;
}


// queryConstVertexBuffer
// ----------------------
//! Query a vertex buffer by name.

const Ngl::VertexBuffer*
Ns3DResourceObject::queryConstVertexBuffer(const NtString& clientName,
                                           const NtString& bufferName) const
{
    const NtString resourceName(longName(clientName,bufferName));
    const _VertexBufferMap::const_iterator find(_vtxBufMap.find(resourceName));

    if (find != _vtxBufMap.end()) {
        return find->second;    // Found.
    }

    return 0;   // Null
}


// queryMutableVertexBuffer
// ------------------------
//! Query a vertex buffer by name.

Ngl::VertexBuffer*
Ns3DResourceObject::queryMutableVertexBuffer(const NtString& clientName,
                                             const NtString& bufferName)
{
    const NtString resourceName(longName(clientName,bufferName));
    _VertexBufferMap::iterator find(_vtxBufMap.find(resourceName));
    
    if (find != _vtxBufMap.end()) {
        return find->second;    // Found.
    }

    return 0;   // Null
}


// queryConstTexture3D
// -------------------
//! Query a 3D texture by name.

const Ngl::Texture3D*
Ns3DResourceObject::queryConstTexture3D(const NtString& clientName,
                                        const NtString& bufferName,
                                        const int         superTile) const
{
    const NtString resourceName(longName(clientName,bufferName,superTile));
    const _Tex3DMap::const_iterator find(_tex3DMap.find(resourceName));

    if (find != _tex3DMap.end()) {
        return find->second;    // Found.
    }

    return 0;   // Null
}


// queryMutableTexture3D
// -------------------
//! Query a 3D texture by name.

Ngl::Texture3D*
Ns3DResourceObject::queryMutableTexture3D(const NtString& clientName,
                                          const NtString& bufferName,
                                          const int         superTile)
{
    const NtString resourceName(longName(clientName,bufferName,superTile));
    _Tex3DMap::iterator find(_tex3DMap.find(resourceName));

    if (find != _tex3DMap.end()) {
        return find->second;    // Found.
    }

    return 0;   // Null
}


// queryMutableFramebuffer
// -----------------------
//! Query a frame buffer by name.

QGLFramebufferObject*
Ns3DResourceObject::queryMutableFramebufferObject(const NtString& clientName,
                                                  const NtString& bufferName)
{
    const NtString resourceName(longName(clientName,bufferName));
    _FBOMap::iterator find(_fboMap.find(resourceName));
    
    if (find != _fboMap.end()) {
        return find->second;    // Found.
    }

    return 0;   // Null
}


// destroySuperTileLayout
// ----------------------
//! Destroy super-tile layout.

void
Ns3DResourceObject::destroySuperTileLayout()
{
    delete _superLayout;
}


// destroyVertexBuffer
// -------------------
//! Destroy vertex buffer.

void
Ns3DResourceObject::destroyVertexBuffer(const NtString& clientName,
                                        const NtString& bufferName)
{
    const NtString resourceName(longName(clientName,bufferName));
    const _VertexBufferMap::iterator find(_vtxBufMap.find(resourceName));

    qDebug() << "Ns3DResourceObject::destroyVertexBuffer - '" 
             << resourceName.str().c_str() << "'";

    if (find != _vtxBufMap.end()) {
        // Found.

        qDebug() << "Found";

        delete find->second;
        _vtxBufMap.erase(find);
    }
}


// destroyTexture3D
// ----------------
//! Destroy texture3D.

void
Ns3DResourceObject::destroyTexture3D(const NtString& clientName,
                                     const NtString& bufferName,
                                     const int         superTile)
{
    const NtString resourceName(longName(clientName,bufferName,superTile));
    const _Tex3DMap::iterator find(_tex3DMap.find(resourceName));

    if (find != _tex3DMap.end()) {
        // Found.

        delete find->second;
        _tex3DMap.erase(find);
    }
}


// destroyFramebufferObject
// ------------------------
//! Destroy frame buffer.

void
Ns3DResourceObject::destroyFramebufferObject(const NtString& clientName,
                                             const NtString& bufferName)
{
    const NtString resourceName(longName(clientName,bufferName));
    const _FBOMap::iterator find(_fboMap.find(resourceName));

    if (find != _fboMap.end()) {
        // Found.

        delete find->second;
        _fboMap.erase(find);
    }
}


// computeSuperTileTextures
// ------------------------
//! Computes/recomputes 3D textures corresponding to each super tile in the
//! super-tile layout.

void
Ns3DResourceObject::computeSuperTileTextures(const NtString& clientName,
                                             const NtString& bufferName,
                                             const NtVec3f&  supersampling,
                                             const float       clipXform[16],
                                             const bool        gradientTexture,
                                             const int         component,
                                             Nb::Vec2f*        valRange)
{
    // Create a SuperTileLayout if one does not exist.

    if (0 == _superLayout)
        createSuperTileLayout(clipXform, 26);

    // Get a worldspace texture-sampling spacing from the actual tile-layout
    // if it exists, otherwise just assume the supersampling is in worldpace
    // units.

    GLfloat invCellSize(1);
    const Nb::TileLayout* layout(constLayoutPtr());
    if(layout)
        invCellSize=(1.f/layout->cellSize());

    // Update each supertile's texture
    
    for(int superTile=0; superTile<_superLayout->superTileCount(); ++superTile){

        // Compute current texture resolution for each supertile
        NtVec3f wsMin, wsMax;
        _superLayout->superTile(superTile).bounds(wsMin, wsMax);

        const NtVec3f wsDim(wsMax - wsMin);
        NtVec3i texRes(
            supersampling[0]*(wsDim[0]*invCellSize) + 1,
            supersampling[1]*(wsDim[1]*invCellSize) + 1,
            supersampling[2]*(wsDim[2]*invCellSize) + 1
            );
        
        // Check if a texture already exists for this supertile
        const Ngl::Texture3D* tex3D = 
            queryConstTexture3D(clientName,bufferName,superTile);
        
        // Wipe texture if resolution has changed
        if(tex3D &&
           (tex3D->width()  != texRes[0] ||
            tex3D->height() != texRes[1] ||
            tex3D->depth()  != texRes[2])) {
            destroyTexture3D(clientName,bufferName,superTile);
            tex3D = 0;
        }
        
        // Create super-tile texture if necessary
        if(0 == tex3D) {
            if(gradientTexture) {
                Nb::Vec2f vr(
                     (std::numeric_limits<float>::max)(),
                    -(std::numeric_limits<float>::max)()
                    );
                superTileGradientTexture3D(
                    clientName,
                    bufferName,
                    superTile,
                    texRes,
                    wsMin,
                    wsMax,
                    component,
                    &vr
                    );
                if(valRange) {
                    (*valRange)[0] = std::min((*valRange)[0], vr[0]);
                    (*valRange)[1] = std::max((*valRange)[1], vr[1]);
                }
            }
            else {
                superTileTexture3D(
                    clientName,bufferName,superTile,texRes,wsMin,wsMax
                    );
            }
        }


    }
}

// superTileTexture3D
// --------------------------
//! Create a 3D texture for the given client, field and super-tile.
//! The field values are mapped across RGB, with A = 1.

const Ngl::Texture3D*
Ns3DResourceObject::superTileTexture3D(const NtString&         clientName,
                                       const NtString&         bufferName,
                                       const int                 superTile,
                                       const NtVec3i&          texDim,
                                       const NtVec3f& wsMin,
                                       const NtVec3f& wsMax)
{
    Ngl::Texture3D* tex3D=
        queryMutableTexture3D(clientName, bufferName, superTile);

    if(tex3D) {
        NB_WARNING("Texture3D '" << name() << "' already exists");
    } else {
        if(constLayoutPtr())
            tex3D = Ngl::createTexture3DFromField3f(
                *constLayoutPtr(),
                constNbField(bufferName,0),
                constNbField(bufferName,1),
                constNbField(bufferName,2),
                texDim,
                wsMin,
                wsMax
                );
        else
            tex3D = Ngl::createTexture3DFrom3f(
                backgroundValue(),texDim,wsMin,wsMax
                );

        const NtString resourceName(
            longName(clientName, bufferName, superTile)
            );
        _tex3DMap.insert(_Tex3DMap::value_type(resourceName, tex3D));
    }

    return tex3D;
}


// superTileGradientTexture3D
// --------------------------
//! Create a 3D gradient texture for the given client, field and super-tile.
//! The gradient of the field is stored in RGB, and the actual field values
//! in A.

const Ngl::Texture3D*
Ns3DResourceObject::superTileGradientTexture3D(const NtString& clientName,
                                               const NtString& bufferName,
                                               const int         superTile,
                                               const NtVec3i&  texDim,
                                               const NtVec3f&  wsMin,
                                               const NtVec3f&  wsMax,
                                               const int         component,
                                               Nb::Vec2f*        valRange)
{
    Ngl::Texture3D* tex3D=
        queryMutableTexture3D(clientName, bufferName, superTile);

    if(tex3D) {
        NB_WARNING("Texture3D '" << name() << "' already exists");
    } else {
        if(constLayoutPtr())
            tex3D = Ngl::createTexture3DFromField1f(
                *constLayoutPtr(),
                constNbField(bufferName,component),
                texDim,
                wsMin,
                wsMax,
                true,
                valRange
                );
        else
            tex3D = Ngl::createTexture3DFrom1f(
                backgroundValue()[component],texDim,wsMin,wsMax,valRange
                );

        const NtString resourceName(
            longName(clientName, bufferName, superTile)
            );

        _tex3DMap.insert(_Tex3DMap::value_type(resourceName, tex3D));
    }
    
    return tex3D;
}
