// -----------------------------------------------------------------------------
//
// Ns3DOpImportTransformItem.h
//
// DOCS
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

#ifndef NS3D_OP_IMPORT_TRANSFORM_ITEM_H
#define NS3D_OP_IMPORT_TRANSFORM_ITEM_H

#include "Ns3DOpItem.h"
#include <NglUtils.h>
#include <QList>

class NsBodyObject;

// -----------------------------------------------------------------------------

class Ns3DOpImportTransformItem : public Ns3DOpItem
{
public:

    // Object space.

    static const Ngl::vec3f ctr;  // Center.
    static const GLfloat    rad;  // Radius.
    static const Ngl::vec3f bbox[8];

public:

    //! CTOR.
    explicit
    Ns3DOpImportTransformItem(
        NsOpObject    *opObject,
        const int32_t  selId = -1);

    //! DTOR.
    virtual
    ~Ns3DOpImportTransformItem()
    {}

    virtual void
    draw(LabelInfo *label = 0) const;

    virtual void
    selectionDraw() const;

    virtual Ns3DBBox
    worldBoundingBox() const;

//    virtual Ngl::vec3f
//    translate() const;

    Ngl::vec3f
    position() const;

private:

    void
    _draw(const Ngl::mat44f &xf, LabelInfo *label = 0) const;

private:

    static bool
    _bodyBBox(const QList<const NsBodyObject*> &bodies,
              Ns3DBBox                         &bb);

    static void
    _makeXForm(const Ns3DBBox &bb, Ngl::mat44f &xf);

private:    // Member variables.

    //std::vector<Ngl::vec3f> _vtx;
    Ngl::Sphere _sphere;
};

// -----------------------------------------------------------------------------

#endif  // NS3D_OP_IMPORT_TRANSFORM_ITEM_H
