// -----------------------------------------------------------------------------
//
// Ns3DOpFrustumItem.h
//
// Ops with a "View Frustum" parameter section
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

#ifndef NS3D_OPFRUSTUMITEM_H
#define NS3D_OPFRUSTUMITEM_H

#include "Ns3DOpBoxItem.h"
#include <NglTypes.h>
#include <QString>

class NsOpObject;

// -----------------------------------------------------------------------------

class Ns3DOpFrustumItem : public Ns3DOpBoxItem
{
public:

    explicit
    Ns3DOpFrustumItem(
        NsOpObject       *opObject,
        const int32_t     selId        = -1,
        const Ngl::vec4f& matAmb       = Ngl::vec4f(0.2f, 0.2f, 0.2f, 1.f),
        const Ngl::vec4f& matDiff      = Ngl::vec4f(0.5f, 0.5f, 0.5f, 1.f),
        const Ngl::vec4f& matSpec      = Ngl::vec4f(0.f , 0.f , 0.f , 1.f),
        const Ngl::vec4f& selMatAmb    = Ngl::vec4f(0.2f, 0.2f, 0.2f, 1.f),
        const Ngl::vec4f& selMatDiff   = Ngl::vec4f(0.8f, 0.8f, 0.2f, 1.f),
        const Ngl::vec4f& selMatSpec   = Ngl::vec4f(0.f , 0.f , 0.f , 1.f),
        const GLfloat lineWidth        = 2.f,
        const GLfloat selLineWidth     = 2.f);

    virtual void
    draw(LabelInfo *label) const;

    virtual void
    selectionDraw() const;

private:

    static const Ngl::vec4f _ndc[];

    void
    _updateFrustum(Ngl::vec3f* verts) const;
};

#endif // NS3D_OPFRUSTUMITEM_H
