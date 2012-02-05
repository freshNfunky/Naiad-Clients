// -----------------------------------------------------------------------------
//
// NsParam1fWidget.cc
//
// Naiad Studio param <float>, source file.
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

#include "NsParam1fWidget.h"
#include "NsParamWidgetArgs.h"
#include "NsParamWidgetFactory.h"
#include "NsValueWidgetLayout.h"
#include "NsValueLineEdit.h"
#include "NsValueFloatSlider.h"
#include "NsValueBaseObject.h"
#include "NsOpObject.h"
#include "NsQuery.h"
#include <QDebug>

#include <NbValue.h>

// -----------------------------------------------------------------------------

//!< Key [static].
const NsValueWidgetKey 
NsParam1fWidget::key(fromNbStr(Nb::Value1f::staticTypeName()),"");

// -----------------------------------------------------------------------------

// NsParam1fWidget
// ---------------
//! CTOR.

NsParam1fWidget::NsParam1fWidget(const NsParamWidgetArgs &args)
    : NsParamBaseWidget(args)
{
    QGridLayout *layout = new QGridLayout;
    NsValueLineEdit *vle = new NsValueLineEdit(*this);   // Child.
    vle->onValueChanged();

    layout->addWidget(vle, 0, 0);

    connect(this, SIGNAL(valueChanged()),
            vle,  SLOT(onValueChanged()));
    connect(vle,  SIGNAL(valueEdited(QString,int)),
            this, SLOT(onValueEdited(QString,int)));

    const float softMin = queryParamSoftMin1f(valueBaseObject()->longName());
    const float softMax = queryParamSoftMax1f(valueBaseObject()->longName());

    if (_hasSlider(softMin, softMax)) {
        // If param has a soft range provide a slider widget.

        layout->setColumnStretch(0, 1);
        layout->setColumnStretch(1, 1);

        const float hardMin =
            queryParamHardMin1f(valueBaseObject()->longName());
        const float hardMax =
            queryParamHardMax1f(valueBaseObject()->longName());

        // Child.

        NsValueFloatSlider *vfs =
            new NsValueFloatSlider(*this,
                                    _sliderMin(softMin, hardMin),
                                    _sliderMax(softMax, hardMax),
                                    softMin,
                                    softMax,
                                    hardMin,
                                    hardMax); // Child.
        vfs->onValueChanged();

        connect(this, SIGNAL(valueChanged()),
                vfs,  SLOT(onValueChanged()));
        connect(vfs,  SIGNAL(valueEdited(QString,int)),
                this, SLOT(onValueEdited(QString,int)));

        layout->addWidget(vfs, 0, 1);

        QString tt(toolTip());
        tt += "\nSoft range: (" + QString::number(softMin) + ", " + 
                                  QString::number(softMax) + ")";
        tt += "\nHard range: [" + QString::number(hardMin) + ", " + 
                                  QString::number(hardMax) + "]";
        setToolTip(tt);
    }

    valueWidgetLayout()->addLayout(layout);
}

// -----------------------------------------------------------------------------

// _hasSlider
// ----------
//! [static]

bool
NsParam1fWidget::_hasSlider(const float softMin, const float softMax)
{
    EM_ASSERT(softMin < softMax);
    return (NI_PARAM_RANGE_MIN < softMin &&
            NI_PARAM_RANGE_MAX > softMax);
}


// _sliderMin
// ----------
//! Returns the slider minimum value, using meta information if available.

float
NsParam1fWidget::_sliderMin(const float softMin, const float hardMin) const
{
    EM_ASSERT(hardMin <= softMin);

    float sliderMin(softMin);
    QString metaValue;
    if (op()->metaValue("Slider Minimum", &metaValue)) {
        // Check if there is a meta value regarding this parameters range.

        bool ok(false);
        const float metaMin(metaValue.toFloat(&ok));
        if (ok) {
            sliderMin = qMax(hardMin, qMin(metaMin, softMin)); // Clamp.
        }
    }
    return sliderMin;
}


// _sliderMax
// ----------
//! Returns the slider maximum value, using meta information if available.

float
NsParam1fWidget::_sliderMax(const float softMax, const float hardMax) const
{
    EM_ASSERT(hardMax >= softMax);

    float sliderMax(softMax);

    QString metaValue;
    if (op()->metaValue("Slider Maximum", &metaValue)) {
        // Check if there is a meta value regarding this parameters range.

        bool ok(false);
        const float metaMax(metaValue.toFloat(&ok));
        if (ok) {
            sliderMax = qMax(softMax, qMin(metaMax, hardMax));  // Clamp.
        }
    }
    return sliderMax;
}

// -----------------------------------------------------------------------------

//! Static creator instance.
const NsParam1fWidget::_Creator
        NsParam1fWidget::_creator(*NsParamWidgetFactory::instance());
