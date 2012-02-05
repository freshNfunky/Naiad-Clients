// -----------------------------------------------------------------------------
//
// Ns3DResourceObject.h
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
// -----------------------------------------------------------------------------

#ifndef NS3D_RESOURCE_OBJECT_H
#define NS3D_RESOURCE_OBJECT_H

#include <NglExtensions.h>

#include <Ni.h>
#include <NbField.h>

#include <sstream>

namespace Ngl {
class SuperTileLayout;
class Texture3D;
class VertexBuffer;
class ShaderProgram;
}


class Ns3DResourceObject
{
public:     // Interface

    typedef QGLFramebufferObject::Attachment FBOAttachment;

    explicit
    Ns3DResourceObject();

    virtual
    ~Ns3DResourceObject();

    //
    // Create resources.
    //

    //! Create super-tile layout resource from tile layout.

    const Ngl::SuperTileLayout*
    createSuperTileLayout(const float clipXform[16],
                          const int   connectivity = 26);

    //! Create vertex buffer from raw data. This allows Scopes to submit
    //! generic vertex buffers to the cache.

    Ngl::VertexBuffer*
    createVertexBuffer(const NtString& clientName,
                       const NtString& bufferName,
                       GLsizeiptr        size,  // [bytes]
                       const GLvoid*     data,
                       GLenum            target = GL_ARRAY_BUFFER,
                       GLenum            usage  = GL_STATIC_DRAW);

    //! Create 3D texture resource from raw data.

    const Ngl::Texture3D*
    createTexture3D(const int         superTile,
                    const NtString& clientName,
                    const NtString& bufferName,
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

    //! Create a 3D texture for the given client, field and super-tile.
    //! The field values are mapped across RGB, with A = 1.

    const Ngl::Texture3D*
    superTileTexture3D(const NtString&         clientName,
                       const NtString&         bufferName,
                       const int                 superTile,
                       const NtVec3i&          texDim,
                       const NtVec3f& wsMin,
                       const NtVec3f& wsMax);

    //! Create a 3D gradient texture for the given client, field and super-tile.
    //! The gradient of the field is stored in RGB, and the actual field values
    //! in A.

    const Ngl::Texture3D*
    superTileGradientTexture3D(const NtString& clientName,
                               const NtString& bufferName,
                               const int         superTile,
                               const NtVec3i&  texDim,
                               const NtVec3f&  wsMin,
                               const NtVec3f&  wsMax,
                               const int         component = 0,
                               Nb::Vec2f*        valRange = 0);

    //! Create FBO resource.

    QGLFramebufferObject*
    createFramebufferObject(const NtString&   clientName,
                            const NtString&   bufferName,
                            const int           width, 
                            const int           height, 
                            const FBOAttachment attachment, 
                            const GLenum        target = GL_TEXTURE_2D, 
                            const GLenum        internalFormat = GL_RGBA8);

    //
    // Query resources.
    // Functions return null if resource cannot be found.
    //

    const Ngl::SuperTileLayout*
    queryConstSuperTileLayout() const;

    const Ngl::Texture3D*
    queryConstTexture3D(const NtString& clientName,
                        const NtString& bufferName,
                        const int         superTile) const;

    Ngl::Texture3D*
    queryMutableTexture3D(const NtString& clientName,
                          const NtString& bufferName,
                          const int         superTile);

    const Ngl::VertexBuffer*
    queryConstVertexBuffer(const NtString& clientName,
                           const NtString& bufferName) const;

    Ngl::VertexBuffer*
    queryMutableVertexBuffer(const NtString& clientName,
                             const NtString& bufferName);

    QGLFramebufferObject*
    queryMutableFramebufferObject(const NtString& clientName,
                                  const NtString& bufferName);


    //
    // Free resources.
    //

    void destroySuperTileLayout();
    void destroyTexture3D(const NtString& clientName,
                          const NtString& bufferName,
                          const int         superTile);
    void destroyVertexBuffer(const NtString& clientName,
                             const NtString& bufferName);
    void destroyFramebufferObject(const NtString& clientName,
                                  const NtString& bufferName);
    

    //! Computes/recomputes 3D textures corresponding to each super tile in the
    //! super-tile layout.

    void computeSuperTileTextures(const NtString&   clientName,
                                  const NtString&   bufferName,
                                  const NtVec3f&    supersampling,
                                  const float         clipXform[16],
                                  const bool          gradientTexture,
                                  const int           component = 0,
                                  Nb::Vec2f*          valRange = 0);
    
    //! Returns the name of the resource object.
    virtual const NtString name() const = 0;

    //! Returns the resource object's tile-layout, if it exists
    virtual const Nb::TileLayout* constLayoutPtr() const = 0;

    //! Returns the resource's object's background value
    virtual NtVec3f backgroundValue() const { return NtVec3f(0); }

    //! Returns the resource's world space bounding box
    virtual void bounds(NtVec3f& bmin, NtVec3f& bmax) const = 0;

    //! Returns the given named field
    virtual const Nb::Field1f&
    constNbField(const NtString& fieldName) const = 0;

    virtual Nb::Field1f&
    mutableNbField(const NtString& fieldName) = 0;

    //! Returns the given named component field
    virtual const Nb::Field1f&
    constNbField(const NtString& fieldName, const int component) const = 0;

    virtual Nb::Field1f&
    mutableNbField(const NtString& fieldName, const int component) = 0;

    static NtString
    longName(const NtString& clientName, 
             const NtString& bufferName,
             const int         superTile=0)
    {
        std::stringstream ss;
        ss  << clientName << "/" << bufferName << ":" << superTile;
        return ss.str();
    }

protected:        // Member variables

    typedef std::map<NtString, Ngl::Texture3D*>       _Tex3DMap;
    typedef std::map<NtString, Ngl::VertexBuffer*>    _VertexBufferMap;
    typedef std::map<NtString, QGLFramebufferObject*> _FBOMap;

    _Tex3DMap        _tex3DMap;
    _VertexBufferMap _vtxBufMap;
    _FBOMap          _fboMap;

    Ngl::SuperTileLayout* _superLayout;

};

#endif // NS3D_RESOURCE_OBJECT_H
