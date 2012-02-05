// -----------------------------------------------------------------------------
//
// Ns3DCacheObject.h
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

#ifndef NS3D_CACHE_OBJECT_H
#define NS3D_CACHE_OBJECT_H

#include <NglExtensions.h>

#include <NgField.h>
#include <NgString.h>

namespace Ngl {
class SuperTileLayout;
class Texture3D;
class VertexBuffer;
}


class Ns3DCacheObject
{
public:     // Interface

    explicit
    Ns3DCacheObject();

    virtual
    ~Ns3DCacheObject();

    //
    // Create resources.
    //

    // Create 3D texture from raw data.

    const Ngl::Texture3D*
    createTexture3D(const Ng::String& name,
                    const GLvoid*     data,
                    GLsizei           width,
                    GLsizei           height,
                    GLsizei           depth,
                    GLint             wrapS          = GL_CLAMP_TO_EDGE,
                    GLint             wrapT          = GL_CLAMP_TO_EDGE,
                    GLint             wrapR          = GL_CLAMP_TO_EDGE,
                    GLint             internalFormat = GL_RGBA32F_ARB, // ARB!?
                    GLenum            format         = GL_RGBA,
                    GLenum            type           = GL_FLOAT,
                    GLint             border         = 0,
                    GLint             minFilter      = GL_LINEAR,
                    GLint             magFilter      = GL_LINEAR);

    // Create super-tile layout from tile layout.

    const Ngl::SuperTileLayout*
    createSuperTileLayout(const Ng::String&     name,
                          const Ng::TileLayout& tl,
                          const int             connectivity);

    //
    // Query resources.
    // Functions return null if resource cannot be found.
    // TODO: Change names to queryConstVertexBuffer(), etc.
    //

    const Ngl::Texture3D*
    queryConstTexture3D(const Ng::String& name) const;

    const Ngl::SuperTileLayout*
    queryConstSuperTileLayout(const Ng::String& name) const;

    //
    // Free resources.
    //

    void destroyVertexBuffer(const Ng::String& name);
    void destroyTexture3D(const Ng::String& name);
    void destroySuperTileLayout(const Ng::String& name);



    // Create vertex buffers directly from Ng::Field channels.
    // For floating point channels the only possible target is
    // GL_ARRAY_BUFFER.

    const Ngl::VertexBuffer*
    createChannel1fVertexBuffer(const Ng::String& fieldChannel,
                                GLenum            usage = GL_STATIC_DRAW);

    const Ngl::VertexBuffer*
    createChannel3fVertexBuffer(const Ng::String& fieldChannel,
                                GLenum            usage = GL_STATIC_DRAW);

    const Ngl::VertexBuffer*
    createChannel3fFrom1fVertexBuffer(const Ng::String& fieldChannel,
                                      GLenum            usage = GL_STATIC_DRAW);

    const Ngl::VertexBuffer*
    createChannel3fFrom1i64VertexBuffer(const Ng::String& fieldChannel,
                                        GLenum          usage = GL_STATIC_DRAW);

    // Create vertex attributes by sampling Ng::Field channels.

    const Ngl::VertexBuffer*
    sampleChannel1fVertexBuffer(const Ng::String& fieldChannel,
                                const Ng::String& samplingChannel,
                                GLenum            usage = GL_STATIC_DRAW);

    const Ngl::VertexBuffer*
    sampleChannel3fVertexBuffer(const Ng::String& fieldChannel,
                                const Ng::String& samplingChannel,
                                GLenum            usage = GL_STATIC_DRAW);


    // Create 3D textures by sampling field channels.

    const Ngl::Texture3D*
    sampleChannel1fTexture3D(const Ng::String&         name,
                             const Ng::String&         fieldChannel,
                             const Ng::Vec3i&          texDim,
                             const em::vec<3,GLfloat>& wsMin,
                             const em::vec<3,GLfloat>& wsMax,
                             em::vec<2,GLfloat>*       valRange = 0);


private:        // Member variables

    typedef std::map<Ng::String, Ngl::SuperTileLayout*> _STLayoutMap;
    typedef std::map<Ng::String, Ngl::Texture3D*>       _Tex3DMap;
    typedef std::map<Ng::String, Ngl::VertexBuffer*>    _VertexBufferMap;

    _STLayoutMap     _stLayoutMap;
    _Tex3DMap        _tex3DMap;
    _VertexBufferMap _vtxBufMap;

private:        // Utility functions

    static
    Ng::String
    _channelVertexBufferName(const Ng::String& fieldChannel,
                             const Ng::String& samplingChannel);

    static
    bool
    _validSamplingShape(const Ng::String& samplingChannel);
};

#endif // NS3D_CACHE_OBJECT_H
