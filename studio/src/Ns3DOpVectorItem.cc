// -----------------------------------------------------------------------------
//
// Ns3DOpVectorItem.h
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

#include "Ns3DOpVectorItem.h"
#include "Ns3DSelectionManager.h"
#include "NsOpObject.h"
#include "NsPreferences.h"

#include <NglUtils.h>
#include <NglState.h>
#include <em_glmat44_algo.h>
#include <QtOpenGL>

// -----------------------------------------------------------------------------

const Ngl::vec3f Ns3DOpVectorItem::axis(0.f, 0.f, 1.f);
const Ngl::vec3f Ns3DOpVectorItem::tangent0(1.f, 0.f, 0.f);
const Ngl::vec3f Ns3DOpVectorItem::tangent1(0.f, 1.f, 0.f);
const GLfloat Ns3DOpVectorItem::length(1.f);
const GLfloat Ns3DOpVectorItem::rad(0.1f);

//! NB: Assumes
const Ngl::vec3f Ns3DOpVectorItem::bbox[8] = {
    Ngl::vec3f(-.1f, -0.1f, 0.f),
    Ngl::vec3f(-.1f, -0.1f, 1.f),
    Ngl::vec3f(-.1f,  0.1f, 1.f),
    Ngl::vec3f(-.1f,  0.1f, 0.f),
    Ngl::vec3f(0.1f, -0.1f, 0.f),
    Ngl::vec3f(0.1f, -0.1f, 1.f),
    Ngl::vec3f(0.1f,  0.1f, 0.f),
    Ngl::vec3f(0.1f,  0.1f, 1.f)
};

// -----------------------------------------------------------------------------

Ns3DOpVectorItem::Ns3DOpVectorItem(NsOpObject    *opObject,
                                   const int32_t  selId,
                                   const GLfloat  lineWidth,
                                   const GLfloat  selLineWidth)
    : Ns3DOpItem(opObject, selId)
    //, _tube(axis, 0.5f*rad, 0.8f*length, 0.f)
    //, _cone(axis, tangent0, tangent1, 0.8f*length, length, rad)
    , _lineWidth(lineWidth)
    , _selLineWidth(selLineWidth)
    , _rollDeg(0.f)
{}

// -----------------------------------------------------------------------------

void
Ns3DOpVectorItem::draw(LabelInfo *label) const
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
Ns3DOpVectorItem::selectionDraw() const
{
    Ngl::vec4uc rgba;
    Ns3DSelectionManager::idToRgba(id(), &rgba);
    glColor4ubv(&rgba[0]);
    _draw();
}

// -----------------------------------------------------------------------------

Ns3DBBox
Ns3DOpVectorItem::worldBoundingBox() const
{
    Ngl::vec3f wsxMin(
        (std::numeric_limits<GLfloat>::max)(),
        (std::numeric_limits<GLfloat>::max)(),
        (std::numeric_limits<GLfloat>::max)());
    Ngl::vec3f wsxMax(
        -(std::numeric_limits<GLfloat>::max)(),
        -(std::numeric_limits<GLfloat>::max)(),
        -(std::numeric_limits<GLfloat>::max)());

    const em::glmat44f xf = 
        em::make_transform(
            translate(), 
            rotate(),
            Ngl::vec3f(1.f, 1.f, 1.f));

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
Ns3DOpVectorItem::_draw(LabelInfo *label) const
{
    glPushMatrix();

    const em::glmat44f xf =
        em::make_transform(
            translate(),
            rotate(),
            Ngl::vec3f(1.f, 1.f, 1.f));
    glMultMatrixf(&xf[0][0]);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    const Ngl::vec3f v = getParam3f("Vector");
    const GLfloat len = em::mag(v);
    const Ngl::Cuboid tube(axis, 0.5f*rad, 0.8f*len, 0.f);
    const Ngl::Cone cone(axis, tangent0, tangent1, 0.8f*len, len, rad);
    tube.draw();
    cone.draw();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Default.

    if (0 != label) {
        const Ngl::vec3f vtx(0.5f, 0.f, 0.f);

        QPointF pos;
        if (labelPos(&vtx, 1, pos)) {
            label->setPos(pos);
            label->setText(opObject()->longName() + QString(" [Vector]"));
            label->setColor(NsPreferences::instance()->scopeViewAxisTextColor());
        }
    }

    glPopMatrix();
}


Ngl::vec3f 
Ns3DOpVectorItem::_eulerAnglesDeg() const
{
    const Ngl::vec3f b = em::normalized(getParam3f("Vector")); // "View".
    const float dotx = fabs(em::dot(b, Ngl::vec3f(1.f, 0.f, 0.f)));
    const float doty = fabs(em::dot(b, Ngl::vec3f(0.f, 1.f, 0.f)));
    const float dotz = fabs(em::dot(b, Ngl::vec3f(0.f, 0.f, 1.f)));
    Ngl::vec3f r; // Right dir.
    Ngl::vec3f u; // Up dir.
    if (dotx < doty) {
        if (dotx < dotz) {
            r = em::normalized(em::cross(Ngl::vec3f(1.f, 0.f, 0.f), b));
        }
        else {
            r = em::normalized(em::cross(Ngl::vec3f(0.f, 0.f, 1.f), b));
        }
    }
    else {
        if (doty < dotz) {
            r = em::normalized(em::cross(Ngl::vec3f(0.f, 1.f, 0.f), b));
        }
        else {
            r = em::normalized(em::cross(Ngl::vec3f(0.f, 0.f, 1.f), b));
        }
    }
    u = em::cross(b, r);
    em::mat33f m;
    m(0,0) = r[0]; m(0,1) = u[0]; m(0,2) = b[0];
    m(1,0) = r[1]; m(1,1) = u[1]; m(1,2) = b[1];
    m(2,0) = r[2]; m(2,1) = u[2]; m(2,2) = b[2];
    return Ngl::vec3f(
        (180.f/M_PI)*(atan2(-m(1,2), m(2,2))),
        (180.f/M_PI)*(asin(m(0,2))),
        _rollDeg); //(180.f/M_PI)*(atan2(-m(0,1), m(0,0))));
#if 0
            std::cerr << "v: [" << x << ", " << y << ", " << z <<"]\n";
            std::cerr << "Pitch: " << _item->rotate()[0] << "\n";
            std::cerr << "Yaw: " << _item->rotate()[1] << "\n";
            std::cerr << "Roll: " << _item->rotate()[2] << "\n";
#endif
}

// -----------------------------------------------------------------------------
