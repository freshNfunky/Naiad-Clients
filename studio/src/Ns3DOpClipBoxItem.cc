// -----------------------------------------------------------------------------
//
// Ns3DOpClipBoxItem.cc
//
// Ops with a "Clip Box" parameter section
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
// -----------------------------------------------------------------------------

#include "Ns3DOpClipBoxItem.h"
#include "NsOpObject.h"
#include "Ns3DSelectionManager.h"
#include "Ns3DCameraScope.h"
#include "NsPreferences.h"

#include <Ni.h>
#include <NiNg.h>

#include <em_glmat44_algo.h>

#include <NglUtils.h>
#include <NglState.h>
#include <QtOpenGL>

// -----------------------------------------------------------------------------

Ns3DOpClipBoxItem::Ns3DOpClipBoxItem(NsOpObject       *opObject,
                                     const int32_t     selId,
                                     const Ngl::vec4f& matAmb,
                                     const Ngl::vec4f& matDiff,
                                     const Ngl::vec4f& matSpec,
                                     const Ngl::vec4f& selMatAmb,
                                     const Ngl::vec4f& selMatDiff,
                                     const Ngl::vec4f& selMatSpec,
                                     const GLfloat lineWidth,
                                     const GLfloat selLineWidth)
    : Ns3DOpBoxItem(opObject, selId,
                    matAmb, matDiff, matSpec,
                    selMatAmb, selMatDiff, selMatSpec,
                    lineWidth, selLineWidth)
{}

// -----------------------------------------------------------------------------

void
Ns3DOpClipBoxItem::draw(LabelInfo *label) const
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
Ns3DOpClipBoxItem::selectionDraw() const
{
    Ngl::vec4uc rgba;
    Ns3DSelectionManager::idToRgba(id(), &rgba);
    glColor4ubv(&rgba[0]);
    _draw();
}

// -----------------------------------------------------------------------------

void
Ns3DOpClipBoxItem::_draw(LabelInfo *label) const
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
                label->setText(opObject()->longName() + QString(" [Clip Box]"));
                label->setColor(
                    NsPreferences::instance()->scopeViewClipBoxTextColor());
            }
        }

    glPopMatrix();
}

// -----------------------------------------------------------------------------
