// -----------------------------------------------------------------------------
//
// Ns3DOpSphereItem.h
//
// Interface for renderable objects.
//
// Copyright (c) 2011 Exotic Matter AB. All rights reserved.
//
// This file is part of Open Naiad Studio.
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
// -----------------------------------------------------------------------------

#ifndef NS3D_OP_SPHERE_ITEM_H
#define NS3D_OP_SPHERE_ITEM_H

#include "Ns3DOpItem.h"

// -----------------------------------------------------------------------------

class Ns3DOpSphereItem : public Ns3DOpItem
{
public:

    // Object space.

    static const Ngl::vec3f ctr;  // Center.
    static const GLfloat    rad;  // Radius.
    static const Ngl::vec3f bbox[8];

public:

    explicit
    Ns3DOpSphereItem(
           NsOpObject       *opObject,
           const int32_t     selId        = -1,
           const GLfloat     lineWidth    = 2.f,
           const GLfloat     selLineWidth = 2.f);

    //! DTOR.
    virtual
    ~Ns3DOpSphereItem()
    {}

    virtual void
    draw(LabelInfo *label = 0) const;

    virtual void
    selectionDraw() const;

    virtual Ns3DBBox
    worldBoundingBox() const;

public:     // Transformation.

    virtual QString
    translateParamName() const
    { return QString("Position"); }

    virtual QString
    rotateParamName() const
    { return QString(); }

    virtual QString
    scaleParamName() const
    { return QString("Radius"); }

//    virtual void
//    setTranslate(const Ngl::vec3f &t, const bool merge)
//    { setParam3f("Position", t, merge); }

//    //! Do nothing.
//    virtual void
//    setRotate(const Ngl::vec3f &r, const bool merge)
//    {}

//    virtual void
//    setScale(const Ngl::vec3f &s, const bool merge)
//    { setParam1f("Radius", fabs(s[0]), merge); }

    virtual Ngl::vec3f
    translate() const
    { return getParam3f("Position"); }

    virtual Ngl::vec3f
    rotate() const
    { return Ngl::vec3f(0.f, 0.f, 0.f); }

    virtual Ngl::vec3f
    scale() const
    {
        const float r(getParam1f("Radius"));
        return Ngl::vec3f(r, r, r);
    }

protected:  // Member variables. TODO: private.

    std::vector<Ngl::vec3f> _vtx;

    GLfloat    _lineWidth;
    GLfloat    _selLineWidth;

private:

    void
    _draw(LabelInfo *label = 0) const;
};

#endif // NS3D_OP_SPHERE_ITEM_H
