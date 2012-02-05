// -----------------------------------------------------------------------------
//
// Ns3DConstructionGridItem.cc
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

#include "Ns3DConstructionGridItem.h"
#include "NsPreferences.h"
#include "NsQuery.h"

// -----------------------------------------------------------------------------

void
Ns3DConstructionGridItem::draw(LabelInfo *label) const
{
    const NtTimeBundle cvftb = queryCurrentVisibleFrameTimeBundle();
    const float mvs = evalParam1f("Global.Master Voxel Size", cvftb);
    const int halfCount = _count/2;
    glLineWidth(1.f);
    glBegin(GL_LINES);
    const QColor color =
        NsPreferences::instance()->scopeViewConstructionGridColor();
    glColor3ub(color.red(), color.green(), color.blue());
    for (int k = 0; k < _count; ++k) {
        const float z = (k - halfCount)*mvs;
        glVertex3f(-halfCount*mvs, 0.f, z);
        glVertex3f( halfCount*mvs, 0.f, z);

        glVertex3f(z, 0.f, -halfCount*mvs);
        glVertex3f(z, 0.f,  halfCount*mvs);
    }
    glEnd();
}

// -----------------------------------------------------------------------------

Ns3DBBox
Ns3DConstructionGridItem::worldBoundingBox() const
{
    const NtTimeBundle cvftb = queryCurrentVisibleFrameTimeBundle();
    const float mvs = evalParam1f("Global.Master Voxel Size", cvftb);
    const int halfCount = _count/2;

    return Ns3DBBox(-halfCount*mvs, halfCount*mvs,
                    -mvs, mvs,
                    -halfCount*mvs, halfCount*mvs);
}
