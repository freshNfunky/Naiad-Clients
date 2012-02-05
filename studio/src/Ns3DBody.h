// -----------------------------------------------------------------------------
//
// Ns3DBody.h
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

#ifndef NS3D_BODY_H
#define NS3D_BODY_H

#include "Ns3DResourceObject.h"

#include <NglExtensions.h>

#include <NbBody.h>
#include <NbFieldShape.h>

// -----------------------------------------------------------------------------

class Ns3DBody : public Ns3DResourceObject
{
public:     // Interface

    explicit
    Ns3DBody(const Nb::Body* body);


    virtual
    ~Ns3DBody();


    const Nb::Body* body() const { return _body; }


    // Get the basic type of a channel

    Nb::ValueBase::Type
    channelType(const NtString& bodyChannel) const
    { return _body->channel(bodyChannel)->type(); }


    //
    // Query resources.
    // Functions return null if resource cannot be found.
    // TODO: Change names to queryConstVertexBuffer(), etc.
    //

    const Ngl::VertexBuffer*
    queryConstVertexBufferByChannel(const NtString& bodyChannel,
                                    const NtString& samplingChannel) const;

    Ngl::VertexBuffer*
    queryMutableVertexBufferByChannel(const NtString& bodyChannel,
                                      const NtString& samplingChannel);

    // Create vertex buffers directly from Nb::Body channels.
    // For floating point channels the only possible target is
    // GL_ARRAY_BUFFER.

    Ngl::VertexBuffer*
    createChannel1fVertexBuffer(const NtString& bodyChannel,
                                const int         block0,
                                const int         block1,
                                GLenum            usage = GL_STATIC_DRAW);

    Ngl::VertexBuffer*
    createChannel3fVertexBuffer(const NtString& bodyChannel,
                                const int         block0,
                                const int         block1,
                                GLenum            usage = GL_STATIC_DRAW);

    // TMP!! For drawing particles as lines.
    Ngl::VertexBuffer*
    createChannel3fVertexBufferLine(const NtString& bodyChannel,
                                    const int       block0,
                                    const int       block1,
                                    GLenum          usage = GL_STATIC_DRAW);

    Ngl::VertexBuffer*
    createConst3fVertexBuffer(const NtString   &bodyChannel,
                              const NtString   &samplingChannel,
                              int               block0,
                              int               block1,
                              const Ngl::vec3f &value,
                              GLenum            usage = GL_STATIC_DRAW);

//    Ngl::VertexBuffer*
//    createParticlePointPositions(const em::block3_array3f& pos,
//                                 int                       block0,
//                                 int                       block1,
//                                 GLenum                 usage = GL_STATIC_DRAW);

    Ngl::VertexBuffer*
    createParticleLinePositions(const em::block3_array3f& pos,
                                const em::block3_array3f& vel,
                                float                     scaledDt,
                                int                       block0,
                                int                       block1,
                                GLenum                    usage = GL_STATIC_DRAW);

    // ----------

    Ngl::VertexBuffer*
    createChannel3fFrom1fVertexBuffer(const NtString& bodyChannel,
                                      const int         block0,
                                      const int         block1,
                                      GLenum            usage = GL_STATIC_DRAW);

    // TMP!! For drawing particles as lines.
    Ngl::VertexBuffer*
    createChannel3fFrom1fVertexBufferLine(const NtString& bodyChannel,
                                          const int         block0,
                                          const int         block1,
                                          GLenum            usage = GL_STATIC_DRAW);
    // ----------

    Ngl::VertexBuffer*
    createChannel3fFrom1i64VertexBuffer(const NtString& bodyChannel,
                                        const int       block0,
                                        const int       block1,
                                        GLenum          usage = GL_STATIC_DRAW);

    // ----------

    Ngl::VertexBuffer*
    createChannel3fFrom1iVertexBufferLine(const NtString& bodyChannel,
                                          const int       block0,
                                          const int       block1,
                                          GLenum        usage = GL_STATIC_DRAW);

    // TMP!! For drawing particles as lines.
    Ngl::VertexBuffer*
    createChannel3fFrom1i64VertexBufferLine(const NtString& bodyChannel,
                                            const int         block0,
                                            const int         block1,
                                            GLenum            usage = GL_STATIC_DRAW);
    // ----------

    // Create vertex attributes by sampling Nb::Body channels.

    Ngl::VertexBuffer*
    sampleChannel1fVertexBuffer(const NtString& bodyChannel,
                                const NtString& samplingChannel,
                                const int         block0,
                                const int         block1,
                                GLenum            usage = GL_STATIC_DRAW);

    Ngl::VertexBuffer*
    sampleChannel3fVertexBuffer(const NtString& bodyChannel,
                                const NtString& samplingChannel,
                                const int         block0,
                                const int         block1,
                                GLenum            usage = GL_STATIC_DRAW);

    // TMP!! For drawing particles as lines.
    Ngl::VertexBuffer*
    sampleChannel3fVertexBufferLine(const NtString& bodyChannel,
                                    const NtString& samplingChannel,
                                    const int         block0,
                                    const int         block1,
                                    GLenum            usage = GL_STATIC_DRAW);
    // ----------

    //! Returns the name of the resource.
    virtual const NtString 
    name() const 
    { return _body->name(); }

    //! Returns the resource's tile-layout, if it exists
    virtual const Nb::TileLayout* 
    constLayoutPtr() const 
    { return &_body->constLayout(); }

    //! Returns the resource's world space bounding box
    virtual void
    bounds(NtVec3f& bmin, NtVec3f& bmax) const
    { _body->bounds(bmin,bmax); }

    //! Returns the given named field
    virtual const Nb::Field1f&
    constNbField(const NtString& fieldName) const
    { return _body->constFieldShape().constField1f(fieldName); }

    virtual Nb::Field1f&
    mutableNbField(const NtString& fieldName)
    { NB_THROW("Error!"); }

    //! Returns the given named component field
    virtual const Nb::Field1f&
    constNbField(const NtString& fieldName, const int component) const
    { 
        if(component==0 && !_body->constFieldShape().hasChannels3f(fieldName))
            return _body->constFieldShape().constField1f(fieldName); 
        else
            return _body->constFieldShape().constField3f(fieldName,component); 
    }

    virtual Nb::Field1f&
    mutableNbField(const NtString& fieldName, const int component)
    { NB_THROW("Error!"); }

private:        // Member variables

    // Ownership of the Nb::Body lies with NsBodyObject.

    const Nb::Body* _body;

private:        // Utility functions

    static bool
    _validSamplingShape(const NtString& samplingChannel);
};

#endif // NS3D_BODY_H
