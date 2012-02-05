// -----------------------------------------------------------------------------
//
// Ns3DIsoScope.h
//
// Naiad Studio Isosurface scope - renders an isosurface from a distance field
// channel in a body.
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

#ifndef NS3D_ISO_SCOPE_H
#define NS3D_ISO_SCOPE_H

#include "Ns3DOpBoxItem.h"
#include "Ns3DCameraScope.h"
#include "Ns3DSliceScopeUtils.h"

#include <Ni.h>
#include <NgBodyOp.h>
#include <NgInput.h>

#include <NglUtils.h>

#include <em_mat44_algo.h>

#include <sstream>

/*

class Ns3DIsoScope : public Ns3DBodyScope
{
public:

    Ns3DIsoScope()
        : Ns3DBodyScope(),
          _shader(0),
          _isoFieldChannel(""),
          _isoValue(0.f),
          _isoUpperBand(0.f),
          _isoLowerBand(0.f),
          _sliceCount(256),
          _sliceVtxBuf(
              new Ngl::VertexBuffer(
                  _sliceCount*(4*sizeof(em::vec<3,GLfloat>)),
                  0, GL_ARRAY_BUFFER, GL_STATIC_DRAW)),
          _texSamplingRate(1.f, 1.f, 1.f),
          _lastTexSamplingRate(1.f, 1.f, 1.f),
          _matFrontAmb(0.2f, 0.2f, 0.2f, 1.f),
          _matFrontDiff(0.4f, 0.5f, 0.6f, 1.f),
          _matFrontSpec(0.1f, 0.1f, 0.1f, 1.f),
          _matFrontShine(64.f),
          _matBackAmb(0.2f, 0.2f, 0.2f, 1.f),
          _matBackDiff(0.6f, 0.5f, 0.4f, 1.f),
          _matBackSpec(0.1f, 0.1f, 0.1f, 1.f),
          _matBackShine(64.f),
          _lightPos(0.f, 0.f, 0.f, 1.f),
          _lightAmb(1.f, 1.f, 1.f, 1.f),
          _lightDiff(1.f, 1.f, 1.f, 1.f),
          _lightSpec(1.f, 1.f, 1.f, 1.f),
          _translate(0.f, 0.f, 0.f),
          _rotate(0.f, 0.f, 0.f),
          _scale(1.f, 1.f, 1.f)
    {
        // Create shaders.

        _shader
            = new Ngl::ShaderProgram(NtString(shaderPath() + "iso.vs"),
                                     NtString(shaderPath() + "iso.fs"));
    }

    virtual
    ~Ns3DIsoScope() {
        delete _shader;
        delete _sliceVtxBuf;
    }

    virtual NtString
    typeName() const
    { return "Iso-Scope"; }

    virtual void
    setContext(const QGLContext* context)
    { ctx = context; }

    virtual void
    refresh()
    {}

    virtual void
    drawBodies(const Ns3DCameraScope* cam,
               const Ngl::Viewport&   vp,
               const int              frame)
    {
        Ns3DBodyScope::drawBodies(cam, vp, frame);
    }

    virtual bool
    draw(NsBody*                nsBody,
         const Ns3DCameraScope* cam,
         const Ngl::Viewport&   vp)
    {
        const Nb::Body* body(nsBody->ngBody());

        _getTransform();
        Ngl::makeXform(_translate, _rotate, _scale, _xf);
        Ngl::invMat44f(_xf, _invXf);

        glGetFloatv(GL_PROJECTION_MATRIX, _projXf);
        glGetFloatv(GL_MODELVIEW_MATRIX, _mvXf);
        Ngl::normalMatrix(_mvXf, _normalXf);

        // Slice clip-box.
        // TODO: Check that clip-box is on-screen!

        Ngl::projectEye(_xf, _mvXf,
                        &Ns3DOpBoxItem::vtx[0], 8,
                        &_eyeMin, &_eyeMax);

        _sliceCount
            = Nb::Op::param1i("Slice Count")->eval(Nb::ZeroTimeBundle, 0);

        if (0 == Ngl::slice(_mvXf, _eyeMin, _eyeMax, _sliceCount, &_sliceWsx)) {
            // No (visible) slices.

            return true;
        }

        const int sliceSize(sizeof(em::vec<3,GLfloat>)*_sliceWsx.size());
        if ( sliceSize != _sliceVtxBuf->size()) {
            delete _sliceVtxBuf;
            _sliceVtxBuf =
                new Ngl::VertexBuffer(
                    sliceSize, 0, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
        }

        _sliceVtxBuf->setSubData(0, sliceSize, &_sliceWsx[0]);

        const GLsizei minCount(
            Ngl::VertexAttrib::connect(
                _shader->constAttrib("position"), *_sliceVtxBuf));

        // Get super-tile layout.

        const NtString stLayoutName(_superTileLayoutName(body));

        const Ngl::SuperTileLayout* stLayout(
            nsBody->ns3DBody()->queryConstSuperTileLayout());

        if (0 == stLayout) {
            const Nb::TileLayout& layout(body->constLayout());
            stLayout =
                nsBody->ns3DBody()->createSuperTileLayout(_xf);
        }

        _isoFieldChannel
            = Nb::Op::param1s("Iso Field Channel")->eval(Nb::ZeroTimeBundle, 0);

        const Nb::Value3f* texSamplingRate(
            Nb::Op::param3f("Texture Sampling Rate"));
        _texSamplingRate[0] = texSamplingRate->eval(Nb::ZeroTimeBundle, 0);
        _texSamplingRate[1] = texSamplingRate->eval(Nb::ZeroTimeBundle, 1);
        _texSamplingRate[2] = texSamplingRate->eval(Nb::ZeroTimeBundle, 2);

        if (_texSamplingRate[0] != _lastTexSamplingRate[0] ||
            _texSamplingRate[1] != _lastTexSamplingRate[1] ||
            _texSamplingRate[2] != _lastTexSamplingRate[2]) {
            // New sampling rate. Destroy old textures.

            for (int st(0); st < stLayout->superTileCount(); ++st) {
                nsBody->ns3DBody()->destroyTexture3D(
                    name(), _isoFieldChannel, st);
            }

            _lastTexSamplingRate = _texSamplingRate;
        }

        for (int st(0); st < stLayout->superTileCount(); ++st) {
            const Ngl::Texture3D* tex3D(0);
            tex3D = nsBody->ns3DBody()->queryConstTexture3D(
                name(), _isoFieldChannel, st
                );

            if (0 == tex3D) {
                NtVec3f wsMin;
                NtVec3f wsMax;
                stLayout->superTile(st).bounds(wsMin, wsMax);
                const em::vec<3,GLfloat> wsDim(wsMax - wsMin);

                const Nb::TileLayout& layout(body->constLayout());
                const GLfloat invCellSize(1.f/layout.cellSize());

                nsBody->ns3DBody()->sampleChannel1fTexture3D(
                    stTex3DName,
                    _isoFieldChannel,
                    NtVec3i(
                        _texSamplingRate[0]*(wsDim[0]*invCellSize) + 1,
                        _texSamplingRate[1]*(wsDim[1]*invCellSize) + 1,
                        _texSamplingRate[2]*(wsDim[2]*invCellSize) + 1),
                    wsMin,
                    wsMax);
            }
        }

        _shader->use();
        _setUniforms();

        for (int st(0); st < stLayout->superTileCount(); ++st) {
            // TODO: Check if super-tile intersects clip-box!

            NtVec3f wsMin;
            NtVec3f wsMax;
            stLayout->superTile(st).bounds(wsMin, wsMax);

            const NtVec3f invWsRange(
                1.f/(wsMax[0] - wsMin[0]),
                1.f/(wsMax[1] - wsMin[1]),
                1.f/(wsMax[2] - wsMin[2]));

            _shader->storeUniform3f("wsMin", wsMin);
            _shader->storeUniform3f("invWsRange", invWsRange);

            const Ngl::Texture3D* tex3D(0);
            tex3D
                = nsBody->ns3DBody()->queryTexture3D(
                    _superTileTex3DName(
                        body, _isoFieldChannel, st));
            tex3D->bind();

            glDrawArrays(GL_QUADS, 0, minCount);
        }

        _shader->unuse();

        Ngl::VertexAttrib::disconnect(_shader->constAttrib("position"));

        return true;
    }

public:
    const QGLContext* ctx;

private:     // Member variables

    void
    _setUniforms() {
        // Vertex shader.

        _shader->storeUniform4m("modelview", _mvXf);
        _shader->storeUniform4m("projection", _projXf);

        _getLighting();
        _setLighting();

        // Fragment shader.

        _getIso();
        _setIso();

        _getMaterial();
        _setMaterial();

        _shader->storeUniform4m("worldToBox", _invXf);
        _shader->storeUniform3m("normalMatrix", _normalXf);
    }
*/

// -----------------------------------------------------------------------------

class Ns3DIsoScope : public Ns3DBodyScope
{
public:

    explicit
    Ns3DIsoScope()
        : Ns3DBodyScope(),_shader(0)
    {}

    virtual
    ~Ns3DIsoScope() 
    {
        delete _shader;
    }

    virtual NtString
    typeName() const
    { return "Iso-Scope"; }

    virtual void
    setContext(const QGLContext* context)
    { ctx = context; }

    virtual void
    reset()
    {
        // Create shaders.

        _shader = new Ngl::ShaderProgram(NtString(shaderPath() + "iso.vs"),
                                         NtString(shaderPath() + "iso.fs"));

        // Bind parameters to shader uniforms.

        _shader->bindUniform4f("lightPos", param3f("Light Position"));
        _shader->bindUniform4f("lightAmb", param3f("Light Ambient"));
        _shader->bindUniform4f("lightDiff", param3f("Light Diffuse"));
        _shader->bindUniform4f("lightSpec", param3f("Light Specular"));
        
        _shader->bindUniform4f("matFrontAmb", param3f("Front Ambient"));
        _shader->bindUniform4f("matFrontDiff", param3f("Front Diffuse"));
        _shader->bindUniform4f("matFrontSpec", param3f("Front Specular"));
        _shader->bindUniform1f("matFrontShine", param1f("Front Shininess"));

        _shader->bindUniform4f("matBackAmb", param3f("Back Ambient"));
        _shader->bindUniform4f("matBackDiff", param3f("Back Diffuse"));
        _shader->bindUniform4f("matBackSpec", param3f("Back Specular"));
        _shader->bindUniform1f("matBackShine", param1f("Back Shininess"));

        _shader->bindUniform1f("isoValue", param1f("Iso Value"));
        _shader->bindUniform1f("isoLowerBand", param1f("Iso Lower-Band"));
        _shader->bindUniform1f("isoUpperBand", param1f("Iso Upper-Band"));
    }

    virtual bool
    draw(NsBodyObject*          nsBody,
         const Ns3DCameraScope* cam,
         const Ngl::Viewport&   vp)
    {
        const Nb::Body* body(&nsBody->nbBody());
        const NtString isoFieldChannel =
            param1s("Iso Field Channel")->eval(Nb::ZeroTimeBundle);

        if(!body->constFieldShape().hasChannels1f(isoFieldChannel)) {
            NB_WARNING("Body '" << body->name() << 
                       "' skipped due to missing channel: " << isoFieldChannel);
            return false;
        }        

        drawSlicedClipBox(
            name(),
            isoFieldChannel,
            0,
            nsBody->ns3DBody(),
            _shader,
            param3f("Translate"),
            param3f("Rotate"),
            param3f("Scale"),
            param1i("Slice Count"),
            param3f("Supersampling")
            );

        ssHud << "Body: '" << fromQStr(nsBody->name()) << "\n";

        return true;
    }

public:
    const QGLContext* ctx;
    
private:     // Member variables
    
    Ngl::ShaderProgram* _shader;

};

// -----------------------------------------------------------------------------

#endif  // NS3D_ISO_SCOPE_H
