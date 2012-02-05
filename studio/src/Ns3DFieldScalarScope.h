// -----------------------------------------------------------------------------
//
// Ns3DFieldScalarScope.h
//
// Naiad Studio Field Scalar Scope - renders a 3D scalar valued field.
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

#ifndef NS3D_FIELD_SCALAR_SCOPE_H
#define NS3D_FIELD_SCALAR_SCOPE_H

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

#include <em_mat44_algo.h>

#include <sstream>
//#include <map>

#if 0
#include <sys/time.h>
#endif

// -----------------------------------------------------------------------------

class Ns3DFieldScalarScope : public Ns3DFieldScope
{
public:

    Ns3DFieldScalarScope()
        : Ns3DFieldScope(),_shader(0)
    {
    }

    virtual
    ~Ns3DFieldScalarScope() 
    {
        delete _shader;
    }

    virtual NtString
    typeName() const
    { return "Field-Scalar-Scope"; }

    virtual void
    setContext(const QGLContext* context)
    { ctx = context; }

    virtual void
    refresh()
    {}

    virtual void
    reset()
    {
        // Create shaders.

        _shader = 
            new Ngl::ShaderProgram(NtString(shaderPath() + "field-scalar.vs"),
                                   NtString(shaderPath() + "field-scalar.fs"));

        // Bind parameters to shader uniforms.

        _shader->bindUniform1f("minVisible", param1f("Min Visible Value"));
        _shader->bindUniform1f("maxVisible", param1f("Max Visible Value"));
        
        const QString opName=name().c_str();
        // HACK/TODO
        //Ns::ns()->setOpParameter(opName , "Min Value", "9999999");
        //Ns::ns()->setOpParameter(opName, "Max Value", "-9999999");
        //Ns::ns()->setOpParameter(opName, "Interactive Cell Size", "");
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
            param3f("Supersampling"),
            param1f("Min Value"),
            param1f("Max Value")
            );

        // set the param value to itself, just to trigger the proper
        // qt signals etc
        const QString opName=name().c_str();

// HACK/TODO
//        Ns::ns()->setOpParameter(
//            opName, "Min Value", param("Min Value")->expr().c_str()
//            );
//        Ns::ns()->setOpParameter(
//            opName, "Max Value", param("Max Value")->expr().c_str()
//            );
//        std::stringstream ss; ss << fld->constLayoutPtr()->cellSize();
//        Ns::ns()->setOpParameter(
//            opName, "Interactive Cell Size", ss.str().c_str()
//            );

        return hudAddField(fld);
    }

public:
    const QGLContext* ctx;

private:     // Member variables

    Ngl::ShaderProgram* _shader;
};

// -----------------------------------------------------------------------------

#endif  // NS3D_FIELD_SCALAR_SCOPE_H
