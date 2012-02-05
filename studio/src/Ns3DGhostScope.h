// -----------------------------------------------------------------------------
//
// Ns3DGhostScope.h
//
// Naiad Studio Ghost scope - renders a scalar field volumetrically.
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

#ifndef NS3D_GHOST_SCOPE_H
#define NS3D_GHOST_SCOPE_H

#include "Ns3DOpBoxItem.h"
#include "Ns3DCameraScope.h"
#include "Ns3DGhostScopeUtils.h"

#include <Ni.h>
#include <NgBodyOp.h>
#include <NgInput.h>

#include <NglShaderProgram.h>
#include <NglVertexAttrib.h>
#include <NglVertexBuffer.h>
#include <NglTexture3D.h>
#include <NglSuperTile.h>
#include <NglSuperTileLayout.h>
#include <NglViewport.h>
#include <NglSlicing.h>

#include <NglUtils.h>

#include <em_mat44_algo.h>

#include <QGLFramebufferObject>

#include <sstream>
#include <limits>
//#include <map>

#if 0
#include <sys/time.h>
#endif

// -----------------------------------------------------------------------------

class Ns3DGhostScope : public Ns3DBodyScope
{
public:

    Ns3DGhostScope()
        : Ns3DBodyScope()
        , _shader1(0)
        , _shader2(0)
        , _shader3(0)
        //, _valRange(0,0)
    {}

    virtual
    ~Ns3DGhostScope() 
    {
        delete _shader1;
        delete _shader2;
        delete _shader3;
    }

    virtual NtString
    typeName() const
    { return "Ghost-Scope"; }

    virtual void
    setContext(const QGLContext* context)
    { ctx = context; }

    virtual void
    reset()
    {
        // Create shaders.        

        _shader1
            = new Ngl::ShaderProgram(NtString(shaderPath() + "ghost-pass1.vs"),
                                     NtString(shaderPath() + "ghost-pass1.fs"));
        _shader2
            = new Ngl::ShaderProgram(NtString(shaderPath() + "ghost-pass2.vs"),
                                     NtString(shaderPath() + "ghost-pass2.fs"));

        _shader3
            = new Ngl::ShaderProgram(NtString(shaderPath() + "ghost-pass3.vs"),
                                     NtString(shaderPath() + "ghost-pass3.fs"));
    }

    virtual bool
    draw(NsBodyObject*          nsBody,
         const Ns3DCameraScope* cam,
         const Ngl::Viewport&   vp)
    {       
        drawGhostSlicedClipBox(
            name(),
            param1s("Ghost Field Channel")->eval(Nb::ZeroTimeBundle),
            nsBody->ns3DBody(),
            _shader1,
            _shader2,
            _shader3,
            vp,
            //_valRange,
            param3f("Translate"),
            param3f("Rotate"),
            param3f("Scale"),
            param1i("Slice Count"),
            param3f("Supersampling"),
            param1i("Light Buffer Size"),
            param3f("Light Direction"),
            param3f("Light Color"),
            param1f("Light Alpha"),
            param3f("Reflective Color"),
            param1f("Reflective Alpha")
            );
        
        ssHud << "Body: '" << fromQStr(nsBody->name()) << "\n";

        return true;
    }

public:

    const QGLContext* ctx;

private:     // Member variables

    Ngl::ShaderProgram* _shader1;   // Pass 1
    Ngl::ShaderProgram* _shader2;   // Pass 2
    Ngl::ShaderProgram* _shader3;   // Pass 3
    //Nb::Vec2f           _valRange;
};

// -----------------------------------------------------------------------------

#endif // NS3D_GHOST_SCOPE_H
