// -----------------------------------------------------------------------------
//
// NsParam1i64Widget.cc
//
// Naiad Studio Param <int64>, source file.
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

#include "NsParam1i64Widget.h"
#include "NsParamWidgetArgs.h"
#include "NsParamWidgetFactory.h"
#include "NsValueWidgetLayout.h"
#include "NsValueLineEdit.h"
#include "NsValueIntSlider.h"
#include "NsValueBaseObject.h"
#include "NsOpObject.h"
#include "NsQuery.h"

// -----------------------------------------------------------------------------

//!< Key [static].
const NsValueWidgetKey 
NsParam1i64Widget::key(fromNbStr(Nb::Value1i64::staticTypeName()), "");

// -----------------------------------------------------------------------------

// NsParam1i64Widget
// -----------------
//! CTOR.

NsParam1i64Widget::NsParam1i64Widget(const NsParamWidgetArgs &args)
    : NsParamBaseWidget(args)
{
    QGridLayout *layout = new QGridLayout;

    NsValueLineEdit *vle(new NsValueLineEdit(*this));   // Child.
    vle->onValueChanged();

    layout->addWidget(vle, 0, 0);

    connect(this, SIGNAL(valueChanged()),
            vle,  SLOT(onValueChanged()));

    connect(vle,  SIGNAL(valueEdited(QString,int)),
            this, SLOT(onValueEdited(QString,int)));

    NsValueBaseObject *vbo(valueBaseObject());

    if (0 != vbo) {
        const int softMin(queryParamSoftMin1i(vbo->longName()));
        const int softMax(queryParamSoftMax1i(vbo->longName()));

        if (_hasSlider(softMin, softMax)) {
            // If param has a soft range provide a slider widget.

            layout->setColumnStretch(0, 1);
            layout->setColumnStretch(1, 1);

            const int hardMin(queryParamHardMin1i(vbo->longName()));
            const int hardMax(queryParamHardMax1i(vbo->longName()));

            NsValueIntSlider *vis(
                new NsValueIntSlider(*this,
                                     _sliderMin(softMin, hardMin),
                                     _sliderMax(softMax, hardMax),
                                     softMin,
                                     softMax,
                                     hardMin,
                                     hardMax)); // Child.
            vis->onValueChanged();

            connect(this, SIGNAL(valueChanged()),
                    vis,  SLOT(onValueChanged()));

            connect(vis,  SIGNAL(valueEdited(QString,int)),
                    this, SLOT(onValueEdited(QString,int)));

            layout->addWidget(vis, 0, 1);
        }
    }

    valueWidgetLayout()->addLayout(layout);
}

// -----------------------------------------------------------------------------

// _hasSlider
// ----------
//! [static]

bool
NsParam1i64Widget::_hasSlider(const int softMin, const int softMax)
{
    EM_ASSERT(softMin < softMax);
    return (NI_PARAM_RANGE_MIN < softMin &&
            NI_PARAM_RANGE_MAX > softMax);
}


// _validSliderMin
// ---------------
//! Clamp to soft and hard min. [static]

int
NsParam1i64Widget::_validSliderMin(int min, int hardMin, int softMin)
{
    EM_ASSERT(hardMin <= softMin);
    return qMax(hardMin, qMin(min, softMin));
}


// _validSliderMax
// ---------------
//! Clamp to soft and hard max. [static]

int
NsParam1i64Widget::_validSliderMax(int max, int softMax, int hardMax)
{
    EM_ASSERT(hardMax >= softMax);
    return qMax(softMax, qMin(max, hardMax));
}


int
NsParam1i64Widget::_sliderMin(const int softMin, const int hardMin) const
{
    int sliderMin(softMin);

    NsOpObject *o(op());
    QString metaValue;

    if (0 != o && o->metaValue("Slider Minimum", &metaValue)) {
        // Check if there is a meta value regarding this parameters range.

        bool ok(false);
        const int metaMin(metaValue.toInt(&ok));

        if (ok) {
            sliderMin = _validSliderMin(metaMin, hardMin, softMin);
        }
    }

    return sliderMin;
}


int
NsParam1i64Widget::_sliderMax(const int softMax, const int hardMax) const
{
    int sliderMax(softMax);

    NsOpObject *o(op());
    QString metaValue;

    if (0 != o && o->metaValue("Slider Maximum", &metaValue)) {
        bool ok(false);
        const int metaMax(metaValue.toInt(&ok));

        if (ok) {
            sliderMax = _validSliderMax(metaMax, softMax, hardMax);
        }
    }

    return sliderMax;
}

// -----------------------------------------------------------------------------

//! Static creator instance.
const NsParam1i64Widget::_Creator
    NsParam1i64Widget::_creator(*NsParamWidgetFactory::instance());
