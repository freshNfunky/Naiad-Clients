// -----------------------------------------------------------------------------
//
// NsBodyHeaderBox.cc
//
// Naiad Studio body header box, source file.
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

#include "NsBodyHeaderBox.h"
#include "NsBodyObject.h"
#include "NsValueWidgetLayout.h"
#include "NsValueLabel.h"
#include <QVBoxLayout>
#include <NbValueType.h>

// -----------------------------------------------------------------------------

// NsBodyHeaderBox
// ---------------
//! CTOR.

NsBodyHeaderBox::NsBodyHeaderBox(NsBodyObject  *body,
                                 const QString &title,
                                 QWidget       *parent)
    : NsValueHeaderBox(title, parent)
{
    QVBoxLayout *vlayout(new QVBoxLayout);

    // Body Name.

    NsValueLabel *nameLabel0(new NsValueLabel("Name:"));
    nameLabel0->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    NsValueLabel *nameLabel1(new NsValueLabel(body->name()));
    nameLabel1->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    NsValueWidgetLayout *vwl0(new NsValueWidgetLayout);
    vwl0->addWidget(nameLabel0, 0, Qt::AlignLeft | Qt::AlignVCenter);
    vwl0->addWidget(nameLabel1, 1, Qt::AlignLeft | Qt::AlignVCenter);

    vlayout->addLayout(vwl0);

    // Body Signature.

    NsValueLabel *sigLabel0(new NsValueLabel("Signature:"));
    sigLabel0->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    NsValueLabel *sigLabel1(new NsValueLabel(body->sigName()));
    sigLabel1->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    NsValueWidgetLayout *vwl1(new NsValueWidgetLayout);
    vwl1->addWidget(sigLabel0, 0, Qt::AlignLeft | Qt::AlignVCenter);
    vwl1->addWidget(sigLabel1, 1, Qt::AlignLeft | Qt::AlignVCenter);

    vlayout->addLayout(vwl1);

    // Body cache mode.

    NsValueLabel *cacheLabel0(new NsValueLabel("Caching:"));
    cacheLabel0->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    NsValueLabel *cacheLabel1(new NsValueLabel(body->isLive() ? "Live":"EMP"));
    cacheLabel1->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    NsValueWidgetLayout *vwl2(new NsValueWidgetLayout);
    vwl2->addWidget(cacheLabel0, 0, Qt::AlignLeft | Qt::AlignVCenter);
    vwl2->addWidget(cacheLabel1, 1, Qt::AlignLeft | Qt::AlignVCenter);

    vlayout->addLayout(vwl2);

    // Evolving.

    NsValueLabel *evolveLabel0(new NsValueLabel("Evolving:"));
    evolveLabel0->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    NsValueLabel *evolveLabel1(new NsValueLabel(body->isEvolving()?"Yes":"No"));
    evolveLabel1->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    NsValueWidgetLayout *vwl3(new NsValueWidgetLayout);
    vwl3->addWidget(evolveLabel0, 0, Qt::AlignLeft | Qt::AlignVCenter);
    vwl3->addWidget(evolveLabel1, 1, Qt::AlignLeft | Qt::AlignVCenter);

    vlayout->addLayout(vwl3);

    vlayout->addWidget(new NsTileLayoutBox(body->nbBody().constLayout(),
                                           *this));

    // Create widgets showing shape information.

    foreach (const NsBodyObject::ShapeInfo &si, body->shapes()) {
        vlayout->addWidget(new NsShapeBox(si, *this));
    }

    setLayout(vlayout);
}

// -----------------------------------------------------------------------------

NsTileLayoutBox::NsTileLayoutBox(const Nb::TileLayout &tl,
                                 NsBodyHeaderBox &parent)
    : NsGroupBox("Tile Layout", &parent)
{
    QGridLayout *gl(new QGridLayout);
    //gl->setColumnStretch(0, 0);
    //gl->setColumnStretch(1, 1);

    NsValueLabel *ctcVl0(new NsValueLabel("Coarse Tile Count:"));
    NsValueLabel *ctcVl1(
        new NsValueLabel(QString::number(tl.coarseTileCount())));
    ctcVl0->setAlignment(Qt::AlignLeft);
    ctcVl1->setAlignment(Qt::AlignLeft);
    gl->addWidget(ctcVl0, 0, 0, Qt::AlignLeft);
    gl->addWidget(ctcVl1, 0, 1, Qt::AlignLeft);

    NsValueLabel *ftcVl0(new NsValueLabel("Fine Tile Count:"));
    NsValueLabel *ftcVl1(new NsValueLabel(QString::number(tl.fineTileCount())));
    ftcVl0->setAlignment(Qt::AlignLeft);
    ftcVl1->setAlignment(Qt::AlignLeft);
    gl->addWidget(ftcVl0, 1, 0, Qt::AlignLeft);
    gl->addWidget(ftcVl1, 1, 1, Qt::AlignLeft);

    NsValueLabel *ccVl0(new NsValueLabel("Voxel Count:"));
    NsValueLabel *ccVl1(new NsValueLabel(QString::number(tl.cellCount())));
    ccVl0->setAlignment(Qt::AlignLeft);
    ccVl1->setAlignment(Qt::AlignLeft);
    gl->addWidget(ccVl0, 2, 0, Qt::AlignLeft);
    gl->addWidget(ccVl1, 2, 1, Qt::AlignLeft);

    NsValueLabel *bcsVl0(new NsValueLabel("BVS:"));
    NsValueLabel *bcsVl1(new NsValueLabel(QString::number(tl.cellSize())));
    bcsVl0->setAlignment(Qt::AlignLeft);
    bcsVl1->setAlignment(Qt::AlignLeft);
    gl->addWidget(bcsVl0, 3, 0, Qt::AlignLeft);
    gl->addWidget(bcsVl1, 3, 1, Qt::AlignLeft);

    setLayout(gl);
    setChecked(false);
}

// -----------------------------------------------------------------------------

// NsShapeBox
// ----------
//! CTOR.

NsShapeBox::NsShapeBox(const NsBodyObject::ShapeInfo &si,
                       NsBodyHeaderBox               &parent)
    : NsGroupBox("Shape: \"" + si.name() + "\"", &parent)
{
    QVBoxLayout *vlayout(new QVBoxLayout);

    foreach (const NsBodyObject::ShapeInfo::ChannelInfo &ci, si.channels()) {
        vlayout->addWidget(new NsChannelBox(ci, *this));
    }

    setLayout(vlayout);
    setChecked(false);
}

// -----------------------------------------------------------------------------

NsChannelBox::NsChannelBox(const NsBodyObject::ShapeInfo::ChannelInfo &ci,
                           NsShapeBox                                 &parent)
    : NsGroupBox("Channel: \"" + ci.name() + "\" <" + ci.typeName() + ">",
                 &parent)
{
    QGridLayout *gl(new QGridLayout);
    //gl->setColumnStretch(0, 0);
    //gl->setColumnStretch(1, 1);

    NsValueLabel *countVl0(new NsValueLabel("Count:"));
    NsValueLabel *countVl1(new NsValueLabel(ci.size()));
    countVl0->setAlignment(Qt::AlignLeft);
    countVl1->setAlignment(Qt::AlignLeft);
    gl->addWidget(countVl0, 0, 0, Qt::AlignLeft);
    gl->addWidget(countVl1, 0, 1, Qt::AlignLeft);

    NsValueLabel *minVl0(new NsValueLabel("Min:"));
    NsValueLabel *minVl1(new NsValueLabel(ci.minValue()));
    minVl0->setAlignment(Qt::AlignLeft);
    minVl1->setAlignment(Qt::AlignLeft);
    gl->addWidget(minVl0, 1, 0, Qt::AlignLeft);
    gl->addWidget(minVl1, 1, 1, Qt::AlignLeft);

    NsValueLabel *maxVl0(new NsValueLabel("Max:"));
    NsValueLabel *maxVl1(new NsValueLabel(ci.maxValue()));
    maxVl0->setAlignment(Qt::AlignLeft);
    maxVl1->setAlignment(Qt::AlignLeft);
    gl->addWidget(maxVl0, 2, 0, Qt::AlignLeft);
    gl->addWidget(maxVl1, 2, 1, Qt::AlignLeft);

    NsValueLabel *avgVl0(new NsValueLabel("Avg:"));
    NsValueLabel *avgVl1(new NsValueLabel(ci.avgValue()));
    avgVl0->setAlignment(Qt::AlignLeft);
    avgVl1->setAlignment(Qt::AlignLeft);
    gl->addWidget(avgVl0, 3, 0, Qt::AlignLeft);
    gl->addWidget(avgVl1, 3, 1, Qt::AlignLeft);

    setLayout(gl);
    setChecked(false);
}
