// -----------------------------------------------------------------------------
//
// Ns3DOpBoxItem.cc
//
// Ops which have a box shape.
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

#include "Ns3DOpBoxItem.h"
#include "Ns3DSelectionManager.h"
#include "NsOpObject.h"
#include "NsPreferences.h"

#include <NglUtils.h>
#include <NglState.h>
#include <em_glmat44_algo.h>
#include <QtOpenGL>
#include <limits>

// -----------------------------------------------------------------------------

const Ngl::vec3f Ns3DOpBoxItem::vtx[8] = {
    Ngl::vec3f(-0.5f, -0.5f, -0.5f),
    Ngl::vec3f( 0.5f, -0.5f, -0.5f),
    Ngl::vec3f( 0.5f, -0.5f,  0.5f),
    Ngl::vec3f(-0.5f, -0.5f,  0.5f),
    Ngl::vec3f(-0.5f,  0.5f, -0.5f),
    Ngl::vec3f( 0.5f,  0.5f, -0.5f),
    Ngl::vec3f( 0.5f,  0.5f,  0.5f),
    Ngl::vec3f(-0.5f,  0.5f,  0.5f)
};

const Ngl::vec2i Ns3DOpBoxItem::edge[12] = {
    Ngl::vec2i(0,1), Ngl::vec2i(1,2), Ngl::vec2i(2,3), Ngl::vec2i(3,0),
    Ngl::vec2i(4,5), Ngl::vec2i(5,6), Ngl::vec2i(6,7), Ngl::vec2i(7,4),
    Ngl::vec2i(0,4), Ngl::vec2i(1,5), Ngl::vec2i(2,6), Ngl::vec2i(3,7)
};

const Ngl::vec3i Ns3DOpBoxItem::face[12] = {
    Ngl::vec3i(1,5,6), Ngl::vec3i(6,2,1), Ngl::vec3i(7,4,0), Ngl::vec3i(0,3,7),
    Ngl::vec3i(6,5,4), Ngl::vec3i(4,7,6), Ngl::vec3i(3,0,1), Ngl::vec3i(1,2,3),
    Ngl::vec3i(2,6,7), Ngl::vec3i(7,3,2), Ngl::vec3i(0,4,5), Ngl::vec3i(5,1,0)
};

const Ngl::vec3f Ns3DOpBoxItem::normal[6] = {
    Ngl::vec3f( 1.f,  0.f,  0.f),     // X+
    Ngl::vec3f(-1.f,  0.f,  0.f),     // X-
    Ngl::vec3f( 0.f,  1.f,  0.f),     // Y+
    Ngl::vec3f( 0.f, -1.f,  0.f),     // Y-
    Ngl::vec3f( 0.f,  0.f,  1.f),     // Z+
    Ngl::vec3f( 0.f,  0.f, -1.f)      // Z-
};

const Ngl::vec3f Ns3DOpBoxItem::bbox[8] = {
    Ngl::vec3f(-0.5f, -0.5f, -0.5f),
    Ngl::vec3f( 0.5f, -0.5f, -0.5f),
    Ngl::vec3f( 0.5f, -0.5f,  0.5f),
    Ngl::vec3f(-0.5f, -0.5f,  0.5f),
    Ngl::vec3f(-0.5f,  0.5f, -0.5f),
    Ngl::vec3f( 0.5f,  0.5f, -0.5f),
    Ngl::vec3f( 0.5f,  0.5f,  0.5f),
    Ngl::vec3f(-0.5f,  0.5f,  0.5f)
};

// -----------------------------------------------------------------------------

void
Ns3DOpBoxItem::draw(LabelInfo *label) const
{
    QColor color;
    if (selected()) {
        color = NsPreferences::instance()->scopeViewSelectionColor();
        glLineWidth(_selLineWidth);
    }
    else {
        color = NsPreferences::instance()->scopeViewBoxLineColor();
        glLineWidth(_lineWidth);
    }
    glColor4ub(color.red(), color.green(), color.blue(), 255);
    _draw(label);
}

// -----------------------------------------------------------------------------

void
Ns3DOpBoxItem::selectionDraw() const
{
    Ngl::vec4uc rgba;
    Ns3DSelectionManager::idToRgba(id(), &rgba);
    glColor4ubv(&rgba[0]);
    _draw();
}

// -----------------------------------------------------------------------------

Ns3DBBox
Ns3DOpBoxItem::worldBoundingBox() const
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
Ns3DOpBoxItem::drawFace(const Ngl::vec3f* verts)
{
    glBegin(GL_TRIANGLES);
    for (int f = 0; f < 12; ++f) {
        glNormal3fv(&Ns3DOpBoxItem::normal[f >> 1][0]); // Shift divide-by-two.
        for (int v = 0; v < 3; ++v) {
            glVertex3fv(&verts[Ns3DOpBoxItem::face[f][v]][0]);
        }
    }
    glEnd();
}

// -----------------------------------------------------------------------------

void
Ns3DOpBoxItem::drawEdge(const Ngl::vec3f* verts)
{
    glBegin(GL_LINES);
    for (int e = 0; e < 12; ++e) {
        for (int v = 0; v < 2; ++v) {
            glVertex3fv(&verts[Ns3DOpBoxItem::edge[e][v]][0]);
        }
    }
    glEnd();
}

// -----------------------------------------------------------------------------

void
Ns3DOpBoxItem::_draw(LabelInfo *label) const
{
    glPushMatrix();
        const em::glmat44f xf(
            em::make_transform(translate(),
                               rotate(),
                               scale()));
        glMultMatrixf(&xf[0][0]);

        glBegin(GL_LINES);
        for (int e(0); e < 12; ++e) {
            for (int i(0); i < 2; ++i) {
                glVertex3fv(&Ns3DOpBoxItem::vtx[Ns3DOpBoxItem::edge[e][i]][0]);
            }
        }
        glEnd();

        if (0 != label) {
            QPointF pos;
            if (labelPos(Ns3DOpBoxItem::vtx, 8, pos)) {
                label->setPos(pos);
                label->setText(opObject()->longName() +
                               QString(" [Transform]"));
                label->setColor(NsPreferences::instance()->scopeViewBoxTextColor());
            }
        }

    glPopMatrix();
}

// -----------------------------------------------------------------------------
