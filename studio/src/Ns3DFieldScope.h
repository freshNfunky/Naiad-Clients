// -----------------------------------------------------------------------------
//
// Ns3DFieldScope.h
//
// Naiad Studio base class for scopes which draw field data in the 3D View.
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

#ifndef NS3D_FIELD_SCOPE_H
#define NS3D_FIELD_SCOPE_H

#include <NgDummyFieldOp.h>
#include <NgBodyInput.h>
#include <NgNelContext.h>

#include <NglViewport.h>
#include <NglShaderProgram.h>

#include "NsStringUtils.h"
#include "Ns3DField.h"
#include "Ns3DScopeUtils.h"
#include "NsQuery.h"

class Ns3DCameraScope;

// -----------------------------------------------------------------------------

class Ns3DFieldScope : public Ng::DummyFieldOp
{
public:
    Ns3DFieldScope()
        : Ng::DummyFieldOp("You Have Not overridden typeName()"),
        _bmin(std::numeric_limits<float>::max(),
              std::numeric_limits<float>::max(),
              std::numeric_limits<float>::max()),
        _bmax(-std::numeric_limits<float>::max(),
              -std::numeric_limits<float>::max(),
              -std::numeric_limits<float>::max())
    {}

    virtual void
    drawField(const Ns3DCameraScope* cam,
              const Ngl::Viewport&   vp,
              const int              frame)
    {
        ssHud.str("");

        _bmin[0] = std::numeric_limits<float>::max();
        _bmin[1] = std::numeric_limits<float>::max();
        _bmin[2] = std::numeric_limits<float>::max();
        _bmax[0] = -std::numeric_limits<float>::max();
        _bmax[1] = -std::numeric_limits<float>::max();
        _bmax[2] = -std::numeric_limits<float>::max();


        if (queryOpState(fromNbStr(name())) ==
            fromNbStr(NI_INACTIVE)) {
            return;
        }

        //if(Ns::opState(QString(name().c_str()))==Ns::ExplicitOff)
        //    return;

        if(input("scope-input")->source() == 0)
            return;

        const QString inputLongname(
            fromNbStr(input("scope-input")->longname()));

        Ns3DField* fld = 0;
            //NsSolveCache::field(1/*Ns::currentFrame()*/, inputLongname);

        if(fld) {
            Nb::TimeBundle tb(Nb::ZeroTimeBundle);
            Ng::NelContext nelContext(0,0,"",tb);
            draw(fld,cam,vp);        

            // Query drawn bodies bounds.

            NtVec3f fldMin(std::numeric_limits<float>::max(),
                           std::numeric_limits<float>::max(),
                           std::numeric_limits<float>::max());
            NtVec3f fldMax(-std::numeric_limits<float>::max(),
                           -std::numeric_limits<float>::max(),
                           -std::numeric_limits<float>::max());

            fld->bounds(fldMin, fldMax);

            // Update bounds for all bodies.

            if (fldMin[0] < fldMax[0] &&
                fldMin[1] < fldMax[1] &&
                fldMin[2] < fldMax[2] ) {
                _bmin[0] = std::min<float>(_bmin[0], fldMin[0]);
                _bmin[1] = std::min<float>(_bmin[1], fldMin[1]);
                _bmin[2] = std::min<float>(_bmin[2], fldMin[2]);
                _bmax[0] = std::max<float>(_bmax[0], fldMax[0]);
                _bmax[1] = std::max<float>(_bmax[1], fldMax[1]);
                _bmax[2] = std::max<float>(_bmax[2], fldMax[2]);
            }


            if(hasParam("Display Cell Shadow") &&
               param1e("Display Cell Shadow")->eval(Nb::ZeroTimeBundle)=="On")
                drawCellShadow(fld,param1e("Shadow Cell Size"));
        }
    }

    virtual void
    setContext(const QGLContext* context) {}

    virtual bool
    draw(Ns3DField*             fld,
         const Ns3DCameraScope* cam,
         const Ngl::Viewport&   vp) = 0;

    virtual void
    hudInfo(std::stringstream *ss)
    {
        *ss << typeName() << ": '" << name() << "'\n";

        if (!ssHud.str().empty()) {
            *ss << ssHud.str() << "\n";
        }
        else {
            *ss << "No fields\n\n";
        }
    }

    virtual void
    bounds(NtVec3f &bmin, NtVec3f &bmax)
    {
        bmin = _bmin;
        bmax = _bmax;
    }


    GLuint envMapHandle;

protected:

    virtual
    bool hudAddField(Ns3DField* fld)
    {
        NtVec3f bmin(std::numeric_limits<float>::max(),
                     std::numeric_limits<float>::max(),
                     std::numeric_limits<float>::max());

        NtVec3f bmax(-std::numeric_limits<float>::max(),
                     -std::numeric_limits<float>::max(),
                     -std::numeric_limits<float>::max());
        fld->bounds(bmin, bmax);

        if (bmin[0] < bmax[0] && bmin[1] < bmax[1] && bmin[2] < bmax[2] ) {

            const NtVec3f dims(bmax - bmin);
            const float invDx(1.f/fld->constLayoutPtr()->cellSize());

            ssHud
                << "Field: '" << fld->name() << "': "
                << fld->constLayoutPtr()->cellCount()
                << " cells ["
                << dims[0]*invDx << "x"
                << dims[1]*invDx << "x"
                << dims[2]*invDx << "]\n";

            return true;
        }

        return false;   // Strange bounds.
    }

protected:  // Member variables.

    std::stringstream ssHud;

    //! Bounds of the bodies most recently drawn.
    NtVec3f _bmin;
    NtVec3f _bmax;
};

// -----------------------------------------------------------------------------

#endif // NS3D_FIELD_SCOPE_H
