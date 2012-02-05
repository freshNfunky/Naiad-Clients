// -----------------------------------------------------------------------------
//
// Ns3DOpAxisItem.h
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

#include "Ns3DOpAxisItem.h"
#include "Ns3DSelectionManager.h"
#include "NsOpObject.h"
#include "NsPreferences.h"

#include <NglUtils.h>
#include <NglState.h>
#include <em_glmat44_algo.h>
#include <QtOpenGL>

// -----------------------------------------------------------------------------

const Ngl::vec3f Ns3DOpAxisItem::axis(1.f, 0.f, 0.f);
const Ngl::vec3f Ns3DOpAxisItem::tangent0(0.f, 1.f, 0.f);
const Ngl::vec3f Ns3DOpAxisItem::tangent1(0.f, 0.f, 1.f);
const GLfloat Ns3DOpAxisItem::length(1.f);
const GLfloat Ns3DOpAxisItem::rad(0.1f);

//! NB: Assumes
const Ngl::vec3f Ns3DOpAxisItem::bbox[8] = {
    Ngl::vec3f(0.f, -.1f, -0.1f),
    Ngl::vec3f(1.f, -.1f, -0.1f),
    Ngl::vec3f(1.f, -.1f,  0.1f),
    Ngl::vec3f(0.f, -.1f,  0.1f),
    Ngl::vec3f(0.f, 0.1f, -0.1f),
    Ngl::vec3f(1.f, 0.1f, -0.1f),
    Ngl::vec3f(0.f, 0.1f,  0.1f),
    Ngl::vec3f(1.f, 0.1f,  0.1f)
};

// -----------------------------------------------------------------------------

// GLfloat      tubeLen        = 1.f,
// GLfloat      tubeThickness  = 0.05f,
// GLfloat      coneHeight     = 0.2f,
// GLfloat      coneRadius     = 0.1f,
// GLint        coneSubd       = 8

Ns3DOpAxisItem::Ns3DOpAxisItem(NsOpObject       *opObject,
                               const int32_t     selId,
                               const GLfloat     lineWidth,
                               const GLfloat     selLineWidth)
    : Ns3DOpItem(opObject, selId)
    , _tube(axis, 0.5f*rad, 0.8f*length, 0.f)
    , _cone(axis, tangent0, tangent1, 0.8f*length, length, rad)
    , _lineWidth(lineWidth)
    , _selLineWidth(selLineWidth)
{}

// -----------------------------------------------------------------------------

void
Ns3DOpAxisItem::draw(LabelInfo *label) const
{
    QColor color;
    if (selected()) {
        color = NsPreferences::instance()->scopeViewSelectionColor();
        glLineWidth(_selLineWidth);
    }
    else {
        color = NsPreferences::instance()->scopeViewAxisLineColor();
        glLineWidth(_lineWidth);
    }
    glColor4ub(color.red(), color.green(), color.blue(), 255);
    _draw(label);
}

// -----------------------------------------------------------------------------

void
Ns3DOpAxisItem::selectionDraw() const
{
    Ngl::vec4uc rgba;
    Ns3DSelectionManager::idToRgba(id(), &rgba);
    glColor4ubv(&rgba[0]);
    _draw();
}

// -----------------------------------------------------------------------------

Ns3DBBox
Ns3DOpAxisItem::worldBoundingBox() const
{
    Ngl::vec3f wsxMin(
        (std::numeric_limits<GLfloat>::max)(),
        (std::numeric_limits<GLfloat>::max)(),
        (std::numeric_limits<GLfloat>::max)());
    Ngl::vec3f wsxMax(
        -(std::numeric_limits<GLfloat>::max)(),
        -(std::numeric_limits<GLfloat>::max)(),
        -(std::numeric_limits<GLfloat>::max)());

    const em::glmat44f xf = em::make_transform(translate(), rotate(), scale());

    for (int i = 0; i < 8; ++i) {
        const Ngl::vec3f wsx = xf*bbox[i];
        wsxMin[0] = std::min<GLfloat>(wsxMin[0], wsx[0]);
        wsxMin[1] = std::min<GLfloat>(wsxMin[1], wsx[1]);
        wsxMin[2] = std::min<GLfloat>(wsxMin[2], wsx[2]);
        wsxMax[0] = std::max<GLfloat>(wsxMax[0], wsx[0]);
        wsxMax[1] = std::max<GLfloat>(wsxMax[1], wsx[1]);
        wsxMax[2] = std::max<GLfloat>(wsxMax[2], wsx[2]);
    }

    return Ns3DBBox(
        wsxMin[0], wsxMax[0], 
        wsxMin[1], wsxMax[1], 
        wsxMin[2], wsxMax[2]);
}

// -----------------------------------------------------------------------------

void
Ns3DOpAxisItem::_draw(LabelInfo *label) const
{
    glPushMatrix();

    const em::glmat44f xf(
        em::make_transform(translate(),
                           rotate(),
                           scale()));
    glMultMatrixf(&xf[0][0]);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    _tube.draw();
    _cone.draw();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Default.

    if (0 != label) {
        const Ngl::vec3f vtx(0.5f, 0.f, 0.f);

        QPointF pos;
        if (labelPos(&vtx, 1, pos)) {
            label->setPos(pos);
            label->setText(opObject()->longName() + QString(" [Axis]"));
            label->setColor(NsPreferences::instance()->scopeViewAxisTextColor());
        }
    }

    glPopMatrix();
}

// -----------------------------------------------------------------------------
