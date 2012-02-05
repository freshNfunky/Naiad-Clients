// -----------------------------------------------------------------------------
//
// Ns3DConstructionGridItem.h
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

#ifndef NS3D_CONSTRUCTION_GRID_ITEM_H
#define NS3D_CONSTRUCTION_GRID_ITEM_H

#include "Ns3DGraphicsItem.h"

// -----------------------------------------------------------------------------

class Ns3DConstructionGridItem : public Ns3DGraphicsItem
{
public:

    explicit
    Ns3DConstructionGridItem(const int count = 21)
        : Ns3DGraphicsItem(-1, false)
        , _count(count)
        , _visible(true)
    {}

    //! DTOR.
    virtual
    ~Ns3DConstructionGridItem()
    {}

    virtual void
    draw(LabelInfo *label = 0) const;

    // Don't override selection draw, since the construction grid isn't
    // selectable.

public:

    virtual QString
    translateParamName() const
    { return QString(); }

    virtual QString
    rotateParamName() const
    { return QString(); }

    virtual QString
    scaleParamName() const
    { return QString(); }

    virtual Ngl::vec3f
    translate() const
    { return Ngl::vec3f(0.f, 0.f, 0.f); }

    virtual Ngl::vec3f
    rotate() const
    { return Ngl::vec3f(0.f, 0.f, 0.f); }

    virtual Ngl::vec3f
    scale() const
    { return Ngl::vec3f(1.f, 1.f, 1.f); }

    virtual bool
    isVisible() const
    { return _visible; }

public:

    void
    setVisible(const bool visible)
    { _visible = visible; }

    virtual Ns3DBBox
    worldBoundingBox() const;

private:    // Member variables.

    int  _count;
    bool _visible;
};

// -----------------------------------------------------------------------------

#endif  // NS3D_CONSTRUCTION_GRID_ITEM_H
