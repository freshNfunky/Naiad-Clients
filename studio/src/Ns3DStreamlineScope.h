// -----------------------------------------------------------------------------
//
// Ns3DStreamlineScope.h
//
// Naiad Studio Streamline scope - renders streamlines through a field-channel.
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

#include <Ni.h>
#include <NgBodyOp.h>
#include <NgInput.h>

#include <NbBody.h>
#include <NbTileLayout.h>

#include "Ns3DCameraScope.h"
#include "Ns3DStreamlineScopeUtils.h"

// -----------------------------------------------------------------------------

class Ns3DStreamlineScope : public Ns3DBodyScope
{
public:
    Ns3DStreamlineScope()
        : Ns3DBodyScope() {}

    virtual NtString
    typeName() const
    { return "Streamline-Scope"; }

    virtual void
    setContext(const QGLContext* context)
    {}

    virtual bool
    draw(NsBodyObject*          nsBody,
         const Ns3DCameraScope* cam,
         const Ngl::Viewport&   vp)
    {        
        drawStreamlines(
            name(),
            param1s("Channel")->eval(Nb::ZeroTimeBundle),
            nsBody->ns3DBody(),
            param3f("Translate"),
            param3f("Rotate"),
            param3f("Scale"),
            param1f("Streamline Spacing"),
            param1f("Streamline Time"),
            param1i("Samples Per Streamline")
            );

        ssHud << "Body: '" << fromQStr(nsBody->name()) << "\n";

        return true;
    }
};

// -----------------------------------------------------------------------------
