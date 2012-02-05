// -----------------------------------------------------------------------------
//
// Ns3DStreamlineScopeUtils.h
//
// Utilities & Tools for streamline scopes.
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

#ifndef NS3D_STREAMLINE_SCOPE_UTILS_H
#define NS3D_STREAMLINE_SCOPE_UTILS_H

#include "Ns3DScopeUtils.h"

#include <NglState.h>

#include <em_mat44_algo.h>

// -----------------------------------------------------------------------------

inline void
drawStreamlines(const NtString&     clientName,
                const NtString&     fieldName,
                Ns3DResourceObject* robject,
                const Nb::Value3f*  translateParam,
                const Nb::Value3f*  rotateParam,
                const Nb::Value3f*  scaleParam,
                const Nb::Value1f*  streamlineSpacingParam,
                const Nb::Value1f*  streamlineTimeParam,
                const Nb::Value1i*  streamlineSamplesParam,
                Nb::Value1f*        minValue=0,
                Nb::Value1f*        maxValue=0)
{       
    const Ngl::vec3f translate(
        translateParam->eval(Nb::ZeroTimeBundle,0),
        translateParam->eval(Nb::ZeroTimeBundle,1),
        translateParam->eval(Nb::ZeroTimeBundle,2)
        );
    const Ngl::vec3f rotate(
        rotateParam->eval(Nb::ZeroTimeBundle,0),
        rotateParam->eval(Nb::ZeroTimeBundle,1),
        rotateParam->eval(Nb::ZeroTimeBundle,2)
        );
    const Ngl::vec3f scale(
        scaleParam->eval(Nb::ZeroTimeBundle,0),
        scaleParam->eval(Nb::ZeroTimeBundle,1),
        scaleParam->eval(Nb::ZeroTimeBundle,2)
        );

    em::quaternionf qr;
    qr.set_euler_angles(rotate);
    const em::mat44f M(em::make_transform(translate, qr, em::vec3f(1)));

    const NtVec3f x0(-0.5f*scale[0],-0.5f*scale[1],-0.5f*scale[2]);
    const NtVec3f x1( 0.5f*scale[0], 0.5f*scale[1], 0.5f*scale[2]);
                                                   
    const float dx=
        streamlineSpacingParam->eval(Nb::ZeroTimeBundle);

    const float timelen=
        streamlineTimeParam->eval(Nb::ZeroTimeBundle);
    const float sampleDt=
        timelen/streamlineSamplesParam->eval(Nb::ZeroTimeBundle);

    const Nb::TileLayout* layout(robject->constLayoutPtr());
    if(!layout) {
        NB_WARNING("NULL layout detected");
        return;
    }

    const Nb::Field1f&    u(robject->constNbField(fieldName, 0));
    const Nb::Field1f&    v(robject->constNbField(fieldName, 1));
    const Nb::Field1f&    w(robject->constNbField(fieldName, 2));
            
    Ngl::ShadeModelState::set(GL_SMOOTH);
    glPointSize(1);

    for(float sx(x0[0]); sx<x1[0]; sx+=dx) {
        for(float sy(x0[1]); sy<x1[1]; sy+=dx) {
            for(float sz(x0[2]); sz<x1[2]; sz+=dx) {
                float time(0);        
                NtVec3f pos(sx,sy,sz);
                pos = pos * M;
                glBegin(GL_LINE_STRIP);        
                glColor3f(0,0,0);
                glVertex3f(pos[0],pos[1],pos[2]);
                while(time<timelen) {
                    const NtVec3f step(
                        Nb::sampleField1f(pos, *layout, u)*sampleDt,
                        Nb::sampleField1f(pos, *layout, v)*sampleDt,
                        Nb::sampleField1f(pos, *layout, w)*sampleDt
                        );
                    pos += step;
                    const float nt(time/timelen);
                    glColor3f(nt,nt,0);
                    glVertex3f(pos[0],pos[1],pos[2]);
                    time+=sampleDt;            
                }   
                glEnd();
            }
        }
    }
}

// -----------------------------------------------------------------------------

#endif // NS3D_STREAMLINE_SCOPE_UTILS_H
