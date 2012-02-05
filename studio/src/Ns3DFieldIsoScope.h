// -----------------------------------------------------------------------------
//
// Ns3DFieldIsoScope.h
//
// Naiad Studio Field Iso Scope - renders a 3D iso valued field.
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

#ifndef NS3D_FIELD_ISO_SCOPE_H
#define NS3D_FIELD_ISO_SCOPE_H

#include "Ns3DOpBoxItem.h"
#include "Ns3DFieldScope.h"

#include <Ni.h>
#include <NgBodyOp.h>
#include <NgInput.h>

#include <NglShaderProgram.h>
#include <NglVertexAttrib.h>
#include <NglTexture3D.h>
#include <NglSuperTile.h>
#include <NglSuperTileLayout.h>
#include <NglViewport.h>
#include <NglSlicing.h>

#include <NglUtils.h>
//#include <NglTileManager.h>

#include <em_mat44_algo.h>

#include <sstream>
//#include <map>

#if 0
#include <sys/time.h>
#endif

// -----------------------------------------------------------------------------

class Ns3DFieldIsoScope : public Ns3DFieldScope
{
public:

    Ns3DFieldIsoScope()
        : Ns3DFieldScope(),_shader(0) {}

    virtual
    ~Ns3DFieldIsoScope() 
    {
        delete _shader;
    }

    virtual NtString
    typeName() const
    { return "Field-Iso-Scope"; }

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
    draw(Ns3DField*             fld,
         const Ns3DCameraScope* cam,
         const Ngl::Viewport&   vp)
    {
        drawSlicedClipBox(
            name(),
            "",
            param1i("Component")->eval(Nb::ZeroTimeBundle),
            fld,
            _shader,
            param3f("Translate"),
            param3f("Rotate"),
            param3f("Scale"),
            param1i("Slice Count"),
            param3f("Supersampling")            
            );

        return hudAddField(fld);
    }

public:
    const QGLContext* ctx;


private:     // Member variables

    Ngl::ShaderProgram* _shader;
};

// -----------------------------------------------------------------------------

#endif  // NS3D_FIELD_ISO_SCOPE_H
