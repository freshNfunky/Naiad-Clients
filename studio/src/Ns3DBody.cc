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

#include "Ns3DBody.h"

#include <NglSuperTileLayout.h>
#include <NglSuperTile.h>
#include <NglTextureUtils.h>
#include <NglVertexBuffer.h>

#include <Nbx.h>    // NB_THROW
#include <NbLog.h>  // NB_WARNING
#include <NbParticleShape.h>
#include <NbBufferChannelBase.h>
#include <NbBufferShape.h>

#include <sstream>


// Ns3DBody
// ---------------
//! Constructor.

Ns3DBody::Ns3DBody(const Nb::Body* body)
    : Ns3DResourceObject(),_body(body)
{
#if 0
    std::cerr << "Create Ns3DBody: '" << _body->name() << "'\n";
#endif
}


// Ns3DBody
// ---------------
//! Destructor.

Ns3DBody::~Ns3DBody()
{
#if 0
    std::cerr << "Destroy Ns3DBody\n";
#endif
}


// queryConstVertexBufferByChannel
// --------------------------
//! Query a vertex buffer by providing two channel names.

const Ngl::VertexBuffer*
Ns3DBody::queryConstVertexBufferByChannel(const NtString& bodyChannel,
                                          const NtString& samplingChannel) const
{
    const _VertexBufferMap::const_iterator find(
        _vtxBufMap.find(longName(bodyChannel, samplingChannel))
        );

    if (find != _vtxBufMap.end()) {
        return find->second;    // Found.
    }

    return 0;   // Null
}


// queryConstVertexBufferByChannel
// --------------------------
//! Query a vertex buffer by providing two channel names.

Ngl::VertexBuffer*
Ns3DBody::queryMutableVertexBufferByChannel(const NtString& bodyChannel,
                                            const NtString& samplingChannel)
{
    _VertexBufferMap::iterator find(
        _vtxBufMap.find(longName(bodyChannel, samplingChannel))
        );

    if (find != _vtxBufMap.end()) {
        return find->second;    // Found.
    }

    return 0;   // Null
}


// ------------------------------------

// Specialized functions for creating resources from data in a Nb::Body


// createChannel1fVertexBuffer
// ---------------------------
//! Create a vertex buffer directly from a Nb::Body channel1f.

Ngl::VertexBuffer*
Ns3DBody::createChannel1fVertexBuffer(const NtString& bodyChannel,
                                      const int         block0,
                                      const int         block1,
                                      const GLenum      usage)
{
    const NtString name(longName(bodyChannel,""));
    Ngl::VertexBuffer* vtxBuf(queryMutableVertexBuffer(bodyChannel,""));

    if (0 != vtxBuf) {
        NB_THROW("Vertex buffer '" << name << "' already exists");
        return vtxBuf;        // Return existing buffer
    }

    const NtString shStr(bodyChannel.parent());   // Shape name
    const NtString chStr(bodyChannel.child());    // Channel name

    if ("Field" == shStr) {
        // We cannot create a float vertex buffer directly from field data.

        NB_THROW("Cannot create vertex buffer from: '" << bodyChannel << "'");
    }

    if ("Particle" == shStr) {
        // Particle data is stored in blocks.

        const Nb::ParticleShape&  particle(_body->constParticleShape());
        const em::block3_array1f& blocks1f(particle.constBlocks1f(chStr));

        Ngl::VertexBuffer* newVtxBuf
            = new Ngl::VertexBuffer(
                blocks1f, block0, block1, GL_ARRAY_BUFFER, usage
                );

        // Insert vertex attribute in map and return it.

        return _vtxBufMap.insert(
                   _VertexBufferMap::value_type(
                       name, newVtxBuf)).first->second;
    }

    // General case, all other (known) shapes

    const Nb::BufferShape&       bufShape(_body->constBufferShape(shStr));
    const Nb::BufferChannelBase& bufChBase(bufShape.constChannelBase(chStr));

    if (bufChBase.type() != Nb::ValueBase::FloatType) {
        NB_THROW("Channel '" << chStr << "' is not of type Float");
    }

    const Nb::Buffer1f& buf1f(bufShape.constBuffer1f(chStr));

    Ngl::VertexBuffer* newVtxBuf
        = new Ngl::VertexBuffer(sizeof(float)*buf1f.size(),
                                buf1f.data,
                                GL_ARRAY_BUFFER,
                                usage);

    return _vtxBufMap.insert(_VertexBufferMap::value_type(name,newVtxBuf)).
        first->second;
}


// createChannel3fVertexBuffer
// ---------------------------
//! Create a vertex buffer directly from a Nb::Body channel3f.

Ngl::VertexBuffer*
Ns3DBody::createChannel3fVertexBuffer(const NtString& bodyChannel,
                                      const int         block0,
                                      const int         block1,
                                      const GLenum      usage)
{
    const NtString name(longName(bodyChannel,""));
    Ngl::VertexBuffer* vtxBuf(queryMutableVertexBuffer(bodyChannel,""));

    if (0 != vtxBuf) {
        NB_THROW("Vertex buffer '" << name << "' already exists");
        return vtxBuf;      // Return existing buffer
    }

    const NtString shStr(bodyChannel.parent());   // Shape name
    const NtString chStr(bodyChannel.child());    // Channel name

    if ("Field" == shStr) {
        // We cannot create a Vec3f vertex buffer directly from field data.

        NB_THROW("Cannot create vertex buffer from: '" << bodyChannel << "'");
    }

    if ("Particle" == shStr) {
        // Particle data is stored in blocks.

        const Nb::ParticleShape&  particle(_body->constParticleShape());
        const em::block3_array3f& blocks3f(particle.constBlocks3f(chStr));

        Ngl::VertexBuffer* newVtxBuf
            = new Ngl::VertexBuffer(blocks3f,
                                    block0,
                                    block1,
                                    GL_ARRAY_BUFFER,
                                    usage);

        // Insert vertex attribute in map and return it.

        return _vtxBufMap.insert(
                   _VertexBufferMap::value_type(
                       name, newVtxBuf)).first->second;
    }

    // General case, all other (known) shapes

    const Nb::BufferShape&       bufShape(_body->constBufferShape(shStr));
    const Nb::BufferChannelBase& bufChBase(bufShape.constChannelBase(chStr));

    if (bufChBase.type() != Nb::ValueBase::Vec3fType) {
        NB_THROW("Channel '" << chStr << "' is not of type Vec3f");
    }

    const Nb::Buffer3f& buf3f(bufShape.constBuffer3f(chStr));

    Ngl::VertexBuffer* newVtxBuf
        = new Ngl::VertexBuffer(sizeof(NtVec3f)*buf3f.size(),
                                buf3f.data,
                                GL_ARRAY_BUFFER,
                                usage);

    return _vtxBufMap.insert(_VertexBufferMap::value_type(name, newVtxBuf)).
        first->second;
}


//Ngl::VertexBuffer*
//Ns3DBody::createParticlePointPositions(const em::block3_array3f& pos,
//                             const int                 block0,
//                             const int                 block1,
//                             const GLenum              usage)
//{
//    const NtString name("Particle.point-positions");
//    Ngl::VertexBuffer* vtxBuf(queryMutableVertexBuffer(name,""));

//    if (0 != vtxBuf) {
//        NB_THROW("Vertex buffer '" << name << "' already exists");
//        //return vtxBuf;      // Return existing buffer
//    }

//    Ngl::VertexBuffer* newVtxBuf(
//        new Ngl::VertexBuffer(pos,
//                              block0,
//                              block1,
//                              GL_ARRAY_BUFFER,
//                              usage));

//    // Insert vertex attribute in map and return it.

//    return _vtxBufMap.insert(
//               _VertexBufferMap::value_type(name, newVtxBuf)).first->second;
//}


Ngl::VertexBuffer*
Ns3DBody::createParticleLinePositions(const em::block3_array3f& pos,
                                      const em::block3_array3f& vel,
                                      float                     scaledDt,
                                      int                       block0,
                                      int                       block1,
                                      GLenum                    usage)
{
    const NtString name(longName("Particle.position-line",""));
    Ngl::VertexBuffer* vtxBuf(queryMutableVertexBuffer(name, ""));

    if (0 != vtxBuf) {
        NB_THROW("Vertex buffer '" << name << "' already exists");
        //return vtxBuf;      // Return existing buffer
    }

    std::vector<em::vec<3,GLfloat> > positions;
    std::vector<em::vec<3,GLfloat> > velocities;
    // TODO: reserve!

    for (int b(0); b < pos.block_count(); ++b) {
        const em::block3vec3f &posb(pos(b));
        const em::block3vec3f &velb(vel(b));
        for (int p(0); p < posb.size(); ++p) {
            positions.push_back(posb(p));
            velocities.push_back(velb(p));
        }
    }

    // Copy positions.
    std::vector<em::vec<3,GLfloat> > vboData(positions);

    // Insert (pos + dt*vel) after original positions.

    for (int i(0); i < positions.size(); ++i) {
        vboData.push_back(positions[i] + scaledDt*velocities[i]);
    }

    Ngl::VertexBuffer* newVtxBuf(
        new Ngl::VertexBuffer(sizeof(em::vec<3,GLfloat>)*vboData.size(),
                              &vboData[0],
                              GL_ARRAY_BUFFER,
                              usage));

    // Insert vertex attribute in map and return it.

    qDebug() << "Create VBO: '" << name.str().c_str() << "'";
    return _vtxBufMap.insert(
               _VertexBufferMap::value_type(name, newVtxBuf)).first->second;
}




Ngl::VertexBuffer*
Ns3DBody::createChannel3fVertexBufferLine(const NtString& bodyChannel,
                                          const int         block0,
                                          const int         block1,
                                          GLenum            usage)
{
    const NtString name(longName(bodyChannel + NtString("-line"),""));
    Ngl::VertexBuffer* vtxBuf(queryMutableVertexBuffer(name,""));

    if (0 != vtxBuf) {
        NB_THROW("Vertex buffer '" << name << "' already exists");
        return vtxBuf;      // Return existing buffer
    }

    const NtString shStr(bodyChannel.parent());   // Shape name
    const NtString chStr(bodyChannel.child());    // Channel name

    if ("Field" == shStr) {
        // We cannot create a Vec3f vertex buffer directly from field data.

        NB_THROW("Cannot create vertex buffer from: '" << bodyChannel << "'");
    }

    if ("Particle" == shStr) {
        // Particle data is stored in blocks.

        const Nb::ParticleShape&  particle(_body->constParticleShape());
        const em::block3_array3f& blocks3f(particle.constBlocks3f(chStr));

        std::vector<em::vec<3, GLfloat> > values;
        for (int b(0); b < blocks3f.block_count(); ++b) {
            const em::block3vec3f &srcb(blocks3f(b));
            for (int p(0); p < srcb.size(); ++p) {
                // HACK! Duplicate values!
                values.push_back(srcb(p));
            }
        }

        // Duplicate!

        std::vector<em::vec<3, GLfloat> > vboData(values);
        vboData.insert(vboData.end(), values.begin(), values.end());

        Ngl::VertexBuffer* newVtxBuf(
            new Ngl::VertexBuffer(sizeof(em::vec<3,GLfloat>)*vboData.size(),
                                  &vboData[0],
                                  GL_ARRAY_BUFFER,
                                  usage));

        // Insert vertex attribute in map and return it.

        qDebug() << "Create VBO: '" << name.str().c_str() << "'";
        return _vtxBufMap.insert(
                   _VertexBufferMap::value_type(name, newVtxBuf)).first->second;
    }

    // General case, all other (known) shapes

    const Nb::BufferShape&       bufShape(_body->constBufferShape(shStr));
    const Nb::BufferChannelBase& bufChBase(bufShape.constChannelBase(chStr));

    if (bufChBase.type() != Nb::ValueBase::Vec3fType) {
        NB_THROW("Channel '" << chStr << "' is not of type Vec3f");
    }

    const Nb::Buffer3f& buf3f(bufShape.constBuffer3f(chStr));

    Ngl::VertexBuffer* newVtxBuf
        = new Ngl::VertexBuffer(sizeof(NtVec3f)*buf3f.size(),
                                buf3f.data,
                                GL_ARRAY_BUFFER,
                                usage);

    return _vtxBufMap.insert(_VertexBufferMap::value_type(name, newVtxBuf)).
        first->second;
}


Ngl::VertexBuffer*
Ns3DBody::createConst3fVertexBuffer(const NtString   &bodyChannel,
                                    const NtString   &samplingChannel,
                                    int               block0,
                                    int               block1,
                                    const Ngl::vec3f &value,
                                    GLenum            usage)
{
    const NtString name(longName(bodyChannel,samplingChannel));
    Ngl::VertexBuffer* vtxBuf(queryMutableVertexBuffer(name,samplingChannel));

    if (0 != vtxBuf) {
        NB_THROW("Vertex buffer '" << name << "' already exists");
        return vtxBuf;      // Return existing buffer
    }


//    const NtString shStr(bodyChannel.parent());   // Shape name
//    const NtString chStr(bodyChannel.child());    // Channel name

    const NtString bodyShStr(bodyChannel.parent());   // Shape name
    const NtString bodyChStr(bodyChannel.child());    // Channel name
    const NtString sampShStr(samplingChannel.parent());
    const NtString sampChStr(samplingChannel.child());


//    if ("Field" == shStr) {
//        // We cannot create a Vec3f vertex buffer directly from field data.

//        NB_THROW("Cannot create vertex buffer from: '" << bodyChannel << "'");
//    }

    if ("Particle" == sampShStr) {
        // Particle data is stored in blocks.

        const Nb::ParticleShape&  particle(_body->constParticleShape());
        const em::block3_array3f& blocks3f(particle.constBlocks3f(bodyShStr));

        std::vector<em::vec<3, GLfloat> > vboData;
        for (int b(0); b < blocks3f.block_count(); ++b) {
            const em::block3vec3f &srcb(blocks3f(b));
            for (int p(0); p < srcb.size(); ++p) {
                vboData.push_back(value);
            }
        }

        Ngl::VertexBuffer* newVtxBuf(
            new Ngl::VertexBuffer(sizeof(em::vec<3,GLfloat>)*vboData.size(),
                                  &vboData[0],
                                  GL_ARRAY_BUFFER,
                                  usage));

        // Insert vertex attribute in map and return it.

        qDebug() << "Create VBO: '" << name.str().c_str() << "'";
        return _vtxBufMap.insert(
                   _VertexBufferMap::value_type(name, newVtxBuf)).first->second;
    }

    return 0;   // Fail!
}


// createChannel3fFrom1fVertexBuffer
// ---------------------------
//! Create a vertex buffer directly from a Nb::Body channel1f.

Ngl::VertexBuffer*
Ns3DBody::createChannel3fFrom1fVertexBuffer(const NtString& bodyChannel,
                                            const int         block0,
                                            const int         block1,
                                            const GLenum      usage)
{
    const NtString name(longName(bodyChannel,""));
    Ngl::VertexBuffer* vtxBuf(queryMutableVertexBuffer(bodyChannel,""));

    if (0 != vtxBuf) {
        NB_THROW("Vertex buffer '" << name << "' already exists");
        return vtxBuf;      // Return existing buffer
    }

    const NtString shStr(bodyChannel.parent());   // Shape name
    const NtString chStr(bodyChannel.child());    // Channel name

    if ("Field" == shStr) {
        // We cannot create a float vertex buffer directly from field data.

        NB_THROW("Cannot create vertex buffer from: '" << bodyChannel << "'");
    }

    if ("Particle" == shStr) {
        // Particle data is stored in blocks.

        const Nb::ParticleShape&  particle(_body->constParticleShape());
        const em::block3_array1f& blocks1f(particle.constBlocks1f(chStr));

        // Expand 1f to 3f (mapping value to the red channel)

        em::block3_array3f blocks3f;
        blocks3f.sync(blocks1f);
        for(int b=0; b<blocks3f.block_count(); ++b) {
            const em::block3f& srcb(blocks1f(b));
            em::block3vec3f&   dstb(blocks3f(b));
            for(int p=0; p<dstb.size(); ++p) {
                dstb(p)[0]=srcb(p);
                dstb(p)[1]=0;
                dstb(p)[2]=0;
            }
        }

        Ngl::VertexBuffer* newVtxBuf
            = new Ngl::VertexBuffer(blocks3f,
                                    block0,
                                    block1,
                                    GL_ARRAY_BUFFER,
                                    usage);

        // Insert vertex attribute in map and return it.

        return _vtxBufMap.insert(
                   _VertexBufferMap::value_type(name, newVtxBuf)).first->second;
    }

    // General case, all other (known) shapes

    const Nb::BufferShape&       bufShape(_body->constBufferShape(shStr));
    const Nb::BufferChannelBase& bufChBase(bufShape.constChannelBase(chStr));

    if (bufChBase.type() != Nb::ValueBase::FloatType) {
        NB_THROW("Channel '" << chStr << "' is not of type Float");
    }

    const Nb::Buffer1f& buf1f(bufShape.constBuffer1f(chStr));

    Ngl::VertexBuffer* newVtxBuf
        = new Ngl::VertexBuffer(sizeof(float)*buf1f.size(),
                                buf1f.data,
                                GL_ARRAY_BUFFER,
                                usage);

    return _vtxBufMap.insert(_VertexBufferMap::value_type(name, newVtxBuf)).
        first->second;
}


Ngl::VertexBuffer*
Ns3DBody::createChannel3fFrom1fVertexBufferLine(const NtString& bodyChannel,
                                      const int         block0,
                                      const int         block1,
                                      GLenum            usage)
{
    const NtString name(longName(bodyChannel + NtString("-line"),""));
    Ngl::VertexBuffer* vtxBuf(queryMutableVertexBuffer(name,""));

    if (0 != vtxBuf) {
        NB_THROW("Vertex buffer '" << name << "' already exists");
        return vtxBuf;      // Return existing buffer
    }

    const NtString shStr(bodyChannel.parent());   // Shape name
    const NtString chStr(bodyChannel.child());    // Channel name

    if ("Field" == shStr) {
        // We cannot create a float vertex buffer directly from field data.

        NB_THROW("Cannot create vertex buffer from: '" << bodyChannel << "'");
    }

    if ("Particle" == shStr) {
        // Particle data is stored in blocks.

        const Nb::ParticleShape&  particle(_body->constParticleShape());
        const em::block3_array1f& blocks1f(particle.constBlocks1f(chStr));

        // Expand 1f to 3f (mapping value to the red channel)

//        em::block3_array3f blocks3f;
//        blocks3f.sync(blocks1f);
//        for(int b=0; b<blocks3f.block_count(); ++b) {
//            const em::block3f& srcb(blocks1f(b));
//            em::block3vec3f&   dstb(blocks3f(b));
//            for(int p=0; p<dstb.size(); ++p) {
//                dstb(p)[0]=srcb(p);
//                dstb(p)[1]=0;
//                dstb(p)[2]=0;
//            }
//        }

        std::vector<em::vec<3, GLfloat> > values;
        // TODO: reserve!

        for (int b(0); b < blocks1f.block_count(); ++b) {
            const em::block3f& srcb(blocks1f(b));
            //em::block3vec3f&   dstb(blocks3f(b));
            for (int p(0); p < srcb.size(); ++p) {
                // HACK! Duplicate values!
                values.push_back(em::vec<3, GLfloat>(srcb(p), 0, 0));
            }
        }

        // Duplicate!

        std::vector<em::vec<3, GLfloat> > vboData(values);
        vboData.insert(vboData.end(), values.begin(), values.end());

        Ngl::VertexBuffer* newVtxBuf
            = new Ngl::VertexBuffer(sizeof(em::vec<3,GLfloat>)*vboData.size(),
                                    &vboData[0],
                                    GL_ARRAY_BUFFER,
                                    usage);

        // Insert vertex attribute in map and return it.

        qDebug() << "Create VBO: '" << name.str().c_str() << "'";
        return _vtxBufMap.insert(
                _VertexBufferMap::value_type(name, newVtxBuf)).first->second;
    }

    // General case, all other (known) shapes

    const Nb::BufferShape&       bufShape(_body->constBufferShape(shStr));
    const Nb::BufferChannelBase& bufChBase(bufShape.constChannelBase(chStr));

    if (bufChBase.type() != Nb::ValueBase::FloatType) {
        NB_THROW("Channel '" << chStr << "' is not of type Float");
    }

    const Nb::Buffer1f& buf1f(bufShape.constBuffer1f(chStr));

    Ngl::VertexBuffer* newVtxBuf
        = new Ngl::VertexBuffer(sizeof(float)*buf1f.size(),
                                buf1f.data,
                                GL_ARRAY_BUFFER,
                                usage);

    return _vtxBufMap.insert(
            _VertexBufferMap::value_type(name, newVtxBuf)).first->second;
}



// createChannel3fFrom1i64VertexBuffer
// ---------------------------
//! Create a vertex buffer directly from a Nb::Body channel1f.

Ngl::VertexBuffer*
Ns3DBody::createChannel3fFrom1i64VertexBuffer(const NtString& bodyChannel,
                                              const int         block0,
                                              const int         block1,
                                              const GLenum      usage)
{
    const NtString name(longName(bodyChannel,""));
    Ngl::VertexBuffer* vtxBuf(
        queryMutableVertexBuffer(bodyChannel,"")
        );

    if (0 != vtxBuf) {
        NB_THROW("Vertex buffer '" << bodyChannel << "' already exists");

        // Return existing buffer

        return vtxBuf;
    }
    
    const NtString shStr(bodyChannel.parent());   // Shape name
    const NtString chStr(bodyChannel.child());    // Channel name

    if ("Field" == shStr) {
        // We cannot create a float vertex buffer directly from field data.

        NB_THROW("Cannot create vertex buffer from: '" << bodyChannel << "'");
    }

    if ("Particle" == shStr) {
        // Particle data is stored in blocks.

        const Nb::ParticleShape&    particle(_body->constParticleShape());
        const em::block3_array1i64& blocks1i64(particle.constBlocks1i64(chStr));

        // Expand 1f to 3f (mapping value to the red channel)

        em::block3_array3f blocks3f;
        blocks3f.sync(blocks1i64);
        for(int b=0; b<blocks3f.block_count(); ++b) {
            const em::block3<int64_t>& srcb(blocks1i64(b));
            em::block3vec3f&     dstb(blocks3f(b));
            for(int p=0; p<dstb.size(); ++p) {
                dstb(p)[0]=srcb(p);
                dstb(p)[1]=0;
                dstb(p)[2]=0;
            }
        }

        Ngl::VertexBuffer* newVtxBuf
            = new Ngl::VertexBuffer(blocks3f,
                                    block0,
                                    block1,
                                    GL_ARRAY_BUFFER,
                                    usage);

        // Insert vertex attribute in map and return it.

        return _vtxBufMap.insert(_VertexBufferMap::value_type(name, newVtxBuf)).
            first->second;
    }

    // General case, all other (known) shapes

    NB_THROW("Only supporting 64-bit integer Particle channels!");
/*
    const Nb::BufferShape&       bufShape(_body->constBufferShape(shStr));
    const Nb::BufferChannelBase& bufChBase(bufShape.constChannelBase(chStr));

    if (bufChBase.type() != Nb::ValueBase::Int64Type) {
        NB_THROW("Channel '" << chStr << "' is not of type Int64");
    }

    const Nb::Buffer1i64& buf1i64(bufShape.constBuffer1i64(chStr));

    Ngl::VertexBuffer* newVtxBuf
        = new Ngl::VertexBuffer(sizeof(float)*buf1i64.size(),
                                buf1i64.data,
                                GL_ARRAY_BUFFER,
                                usage);

    return _vtxBufMap.insert(
               _VertexBufferMap::value_type(
                   _channelVertexBufferName(bodyChannel, ""),
                   newVtxBuf)).first->second;
*/
}


Ngl::VertexBuffer*
Ns3DBody::createChannel3fFrom1i64VertexBufferLine(const NtString& bodyChannel,
                                                  const int       block0,
                                                  const int       block1,
                                                  GLenum          usage)
{
    const NtString name(longName(bodyChannel + NtString("-line"),""));
    Ngl::VertexBuffer* vtxBuf(queryMutableVertexBuffer(bodyChannel,""));

    if (0 != vtxBuf) {
        NB_THROW("Vertex buffer '" << name << "' already exists");

        // Return existing buffer

        return vtxBuf;
    }

    const NtString shStr(bodyChannel.parent());   // Shape name
    const NtString chStr(bodyChannel.child());    // Channel name

    if ("Field" == shStr) {
        // We cannot create a float vertex buffer directly from field data.

        NB_THROW("Cannot create vertex buffer from: '" << bodyChannel << "'");
    }

    if ("Particle" == shStr) {
        // Particle data is stored in blocks.

        const Nb::ParticleShape&    particle(_body->constParticleShape());
        const em::block3_array1i64& blocks1i64(particle.constBlocks1i64(chStr));

        // Expand 1f to 3f (mapping value to the red channel)

        em::block3_array3f blocks3f;
        blocks3f.sync(blocks1i64);
        for(int b=0; b<blocks3f.block_count(); ++b) {
            const em::block3<int64_t>& srcb(blocks1i64(b));
            em::block3vec3f&     dstb(blocks3f(b));
            for(int p=0; p<dstb.size(); ++p) {
                dstb(p)[0]=srcb(p);
                dstb(p)[1]=0;
                dstb(p)[2]=0;
            }
        }

        std::vector<em::vec<3, GLfloat> > values;
        for (int b(0); b < blocks3f.block_count(); ++b) {
            const em::block3vec3f &v(blocks3f(b));
            for (int p(0); p < v.size(); ++p) {
                values.push_back(v(p));
            }
        }

        // Duplicate!

        std::vector<em::vec<3, GLfloat> > vboData(values);
        vboData.insert(vboData.end(), values.begin(), values.end());

        Ngl::VertexBuffer* newVtxBuf(
            new Ngl::VertexBuffer(sizeof(em::vec<3,GLfloat>)*vboData.size(),
                                  &vboData[0],
                                  GL_ARRAY_BUFFER,
                                  usage));

        // Insert vertex attribute in map and return it.

        qDebug() << "Create VBO: '" << name.str().c_str() << "'";
        return _vtxBufMap.insert(
                _VertexBufferMap::value_type(name, newVtxBuf)).first->second;
    }

    // General case, all other (known) shapes

    NB_THROW("Only supporting 64-bit integer Particle channels!");
}


Ngl::VertexBuffer*
Ns3DBody::createChannel3fFrom1iVertexBufferLine(const NtString& bodyChannel,
                                                const int       block0,
                                                const int       block1,
                                                GLenum          usage)
{
    const NtString name(longName(bodyChannel + NtString("-line"),""));
    Ngl::VertexBuffer* vtxBuf(queryMutableVertexBuffer(name,""));

    if (0 != vtxBuf) {
        NB_THROW("Vertex buffer '" << name << "' already exists");

        // Return existing buffer

        return vtxBuf;
    }

    const NtString shStr(bodyChannel.parent());   // Shape name
    const NtString chStr(bodyChannel.child());    // Channel name

    if ("Field" == shStr) {
        // We cannot create a float vertex buffer directly from field data.

        NB_THROW("Cannot create vertex buffer from: '" << bodyChannel << "'");
    }

    if ("Particle" == shStr) {
        // Particle data is stored in blocks.

        const Nb::ParticleShape&  particle(_body->constParticleShape());
        const em::block3_array1i& blocks1i(particle.constBlocks1i(chStr));

        // Expand 1f to 3f (mapping value to the red channel)

        em::block3_array3f blocks3f;
        blocks3f.sync(blocks1i);
        for(int b=0; b<blocks3f.block_count(); ++b) {
            const em::block3i& srcb(blocks1i(b));
            em::block3vec3f& dstb(blocks3f(b));
            for(int p=0; p<dstb.size(); ++p) {
                dstb(p)[0]=srcb(p);
                dstb(p)[1]=0;
                dstb(p)[2]=0;
            }
        }

        std::vector<em::vec<3, GLfloat> > values;
        for (int b(0); b < blocks3f.block_count(); ++b) {
            const em::block3vec3f &v(blocks3f(b));
            for (int p(0); p < v.size(); ++p) {
                values.push_back(v(p));
            }
        }

        // Duplicate!

        std::vector<em::vec<3, GLfloat> > vboData(values);
        vboData.insert(vboData.end(), values.begin(), values.end());

        Ngl::VertexBuffer* newVtxBuf(
            new Ngl::VertexBuffer(sizeof(em::vec<3,GLfloat>)*vboData.size(),
                                  &vboData[0],
                                  GL_ARRAY_BUFFER,
                                  usage));

        // Insert vertex attribute in map and return it.

        qDebug() << "Create VBO: '" << name.str().c_str() << "'";
        return _vtxBufMap.insert(
                _VertexBufferMap::value_type(name, newVtxBuf)).first->second;
    }

    // General case, all other (known) shapes

    NB_THROW("Only supporting integer Particle channels!");
}


// sampleChannel1fVertexBuffer
// ---------------------------
//! Create a vertex buffer by sampling a Nb::Body channel.

Ngl::VertexBuffer*
Ns3DBody::sampleChannel1fVertexBuffer(const NtString& bodyChannel,
                                      const NtString& samplingChannel,
                                      const int         block0,
                                      const int         block1,
                                      const GLenum      usage)
{
    const NtString name(longName(bodyChannel,samplingChannel));
    Ngl::VertexBuffer* vtxBuf(
        queryMutableVertexBuffer(bodyChannel,samplingChannel)
        );

    if (0 != vtxBuf) {
        NB_THROW("Vertex buffer '" << name << "' already exists");
        return vtxBuf;  // Return existing buffer
    }

    //const Nb::FieldShape& field(_body->constFieldShape());

    const NtString bodyShStr(bodyChannel.parent());   // Shape name
    //const NtString bodyChStr(bodyChannel.child());    // Channel name
    const NtString sampShStr(samplingChannel.parent());
    //const NtString sampChStr(samplingChannel.child());

    if (!("Field" == bodyShStr)) {
        NB_THROW("Invalid Shape for sampling: '" << bodyShStr <<
                 "' (must be 'Field'");
    }

    if (!_validSamplingShape(sampShStr)) {
        NB_THROW("Invalid sampling shape: '" << sampShStr <<
                 "' (must be 'Particle' or 'Point'");
    }

    NB_THROW("TODO: Implement!");
}


// sampleChannel3fVertexBuffer
// ---------------------------
//! Create a vertex attribute by sampling a Nb::Body channel.

Ngl::VertexBuffer*
Ns3DBody::sampleChannel3fVertexBuffer(const NtString& bodyChannel,
                                      const NtString& samplingChannel,
                                      const int         block0,
                                      const int         block1,
                                      const GLenum      usage)
{
    const NtString name(longName(bodyChannel,samplingChannel));
    Ngl::VertexBuffer* vtxBuf=
        queryMutableVertexBuffer(bodyChannel,samplingChannel);
    
    if (0 != vtxBuf) {
        NB_THROW("Vertex buffer '" << name << "' already exists");
        return vtxBuf;      // Return existing buffer
    }

    //const Nb::FieldShape& field(_body->constFieldShape());

    const NtString bodyShStr(bodyChannel.parent());   // Shape name
    const NtString bodyChStr(bodyChannel.child());    // Channel name
    const NtString sampShStr(samplingChannel.parent());
    const NtString sampChStr(samplingChannel.child());

    if (!("Field" == bodyShStr)) {
        NB_THROW("Invalid Shape for sampling: '" << bodyShStr <<
                 "' (must be 'Field'");
    }

    if (!_validSamplingShape(sampShStr)) {
        NB_THROW("Invalid sampling shape: '" << sampShStr <<
                 "' (must be 'Particle' or 'Point'");
    }


    if ("Particle" == sampShStr && _body->hasShape("Field")) {
        // Use a particle channel to determine sampling locations

        // Will throw if _body has no field shape!

        const Nb::FieldShape& field(_body->constFieldShape());

        const Nb::ParticleShape&  particle(_body->constParticleShape());
        const em::block3_array3f& blocks3f(particle.constBlocks3f(sampChStr));

        const int b0(std::max(0,block0));
        const int b1(std::min(block1+1,blocks3f.block_count()));
        const int blockCount(b1-b0);
        if(blockCount<0)
            NB_THROW("Invalid block range");

        // Query sampling locations.

        em::array1i baseCount(blockCount);
        int count(0);
        for(int b(b0); b < b1; ++b) {
            baseCount[b-b0] = count;
            count += blocks3f(b-b0).size();
        }

        const Nb::TileLayout& layout(_body->constLayout());

        // If the requested channel has scalar values, sample the gradients.

        if (channelType(bodyChannel) == Nb::ValueBase::FloatType) {

            const Nb::FieldChannel1f& fieldChannel1f(
                field.constChannel1f(bodyChannel)
                );

            std::vector<NtVec3f> gradients(count, NtVec3f(0.f));

#pragma omp parallel for schedule(guided)
            for(int b=b0; b < b1; ++b) {
                int count(baseCount[b-b0]);
                for(int p(0); p < blocks3f(b-b0).size(); ++p) {
                    NtVec3f grad;
                    fieldChannel1f.sampleGradientLinear(
                        blocks3f(b-b0)(p), layout, grad);
                    gradients[count++] = grad;
                }
            }

            qDebug() << "Ns3DBody::sampleChannel3fVertexBuffer - Create VBO: '" 
                     << name.str().c_str() << "'";
            return
                createVertexBuffer(
                    bodyChannel,
                    samplingChannel,
                    sizeof(NtVec3f)*gradients.size(),
                    &gradients[0],
                    GL_ARRAY_BUFFER,
                    usage);
        }
        else if (channelType(bodyChannel) == Nb::ValueBase::Vec3fType) {
            // If field is a vector field LERP values at sampling locations.
            NB_THROW("TODO: Implement!");
        }
    }
    else if ("Point" == sampShStr) {
        //const Nb::PointShape& point(_body->constPointShape());

        // Body has point shape.

        NB_THROW("TODO: Implement!");
    }

    return 0;   // Null.
}


// sampleChannel3fVertexBufferLine
// -------------------------------
//! Create a vertex attribute by sampling a Nb::Body channel.

Ngl::VertexBuffer*
Ns3DBody::sampleChannel3fVertexBufferLine(const NtString& bodyChannel,
                                          const NtString& samplingChannel,
                                          const int         block0,
                                          const int         block1,
                                          const GLenum      usage)
{
    const NtString name(longName(bodyChannel + NtString("-line"), samplingChannel));
    Ngl::VertexBuffer* vtxBuf=
        queryMutableVertexBuffer(bodyChannel + NtString("-line"), samplingChannel);

    if (0 != vtxBuf) {
        NB_THROW("Vertex buffer '" << name << "' already exists");
        return vtxBuf;      // Return existing buffer
    }

    //const Nb::FieldShape& field(_body->constFieldShape());

    const NtString bodyShStr(bodyChannel.parent());   // Shape name
    const NtString bodyChStr(bodyChannel.child());    // Channel name
    const NtString sampShStr(samplingChannel.parent());
    const NtString sampChStr(samplingChannel.child());

    if (!("Field" == bodyShStr)) {
        NB_THROW("Invalid Shape for sampling: '" << bodyShStr <<
                 "' (must be 'Field'");
    }

    if (!_validSamplingShape(sampShStr)) {
        NB_THROW("Invalid sampling shape: '" << sampShStr <<
                 "' (must be 'Particle' or 'Point'");
    }


    if ("Particle" == sampShStr && _body->hasShape("Field")) {
        // Use a particle channel to determine sampling locations

        // Will throw if _body has no field shape!

        const Nb::FieldShape& field(_body->constFieldShape());

        const Nb::ParticleShape&  particle(_body->constParticleShape());
        const em::block3_array3f& blocks3f(particle.constBlocks3f(sampChStr));

        const int b0(std::max(0, block0));
        const int b1(std::min(block1 + 1, blocks3f.block_count()));
        const int blockCount(b1-b0);

        if(blockCount<0) {
            NB_THROW("Invalid block range");
        }

        // Query sampling locations.

        em::array1i baseCount(blockCount);
        int count(0);
        for(int b(b0); b < b1; ++b) {
            baseCount[b] = count;
            count += blocks3f(b).size();
        }

        if (0 == count) {
            NB_THROW("Invalid count");
        }

        const Nb::TileLayout& layout(_body->constLayout());

        // If the requested channel has scalar values, sample the gradients.

        if (channelType(bodyChannel) == Nb::ValueBase::FloatType) {

            const Nb::FieldChannel1f& fieldChannel1f(
                field.constChannel1f(bodyChannel)
                );

            std::vector<NtVec3f> gradients(count, NtVec3f(0.f));

#pragma omp parallel for schedule(guided)
            for(int b=b0; b < b1; ++b) {
                int count(baseCount[b]);
                for(int p(0); p < blocks3f(b).size(); ++p) {
                    NtVec3f grad;
                    fieldChannel1f.sampleGradientLinear(
                        blocks3f(b)(p), layout, grad);
                    gradients[count++] = grad;
                }
            }

            // Duplicate!

            std::vector<NtVec3f> vboData(gradients);
            vboData.insert(vboData.end(), gradients.begin(), gradients.end());

            qDebug() << "Ns3DBody::sampleChannel3fVertexBufferLine - Create VBO: '" 
                     << name.str().c_str() << "'";

            return
                createVertexBuffer(
                    bodyChannel + NtString("-line"),
                    samplingChannel,
                    sizeof(NtVec3f)*vboData.size(),
                    &vboData[0],
                    GL_ARRAY_BUFFER,
                    usage);
        }
        else if (channelType(bodyChannel) == Nb::ValueBase::Vec3fType) {
            // If field is a vector field LERP values at sampling locations.
            NB_THROW("TODO: Implement!");
        }
    }
    else if ("Point" == sampShStr) {
        //const Nb::PointShape& point(_body->constPointShape());

        // Body has point shape.

        NB_THROW("TODO: Implement!");
    }

    return 0;   // Null.
}


// _validSamplingShape
// ---------------------
//! Returns true if

bool
Ns3DBody::_validSamplingShape(const NtString& shape)
{
    return ("Particle" == shape ||
            "Point"    == shape);
}

