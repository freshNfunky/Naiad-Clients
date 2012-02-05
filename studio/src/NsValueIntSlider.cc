// -----------------------------------------------------------------------------
//
// NsValueIntSlider.cc
//
// Naiad Studio integer value slider, source file.
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

#include "NsValueIntSlider.h"
#include "NsValueBaseWidget.h"
#include "NsValueBaseObject.h"
#include "NsCmdSetMeta.h"
#include "NsQuery.h"
#include <QtGui>

// -----------------------------------------------------------------------------

// NsValueIntSlider
// ----------------
//! CTOR.

NsValueIntSlider::NsValueIntSlider(NsValueBaseWidget     &parent,
                                   const int              min,
                                   const int              max,
                                   const int              softMin,
                                   const int              softMax,
                                   const int              hardMin,
                                   const int              hardMax,
                                   const Qt::Orientation  orientation,
                                   const int              comp)
    : QSlider(orientation, &parent)
    , _vbw(&parent)
    , _comp(comp)
    , _softMin(softMin)
    , _softMax(softMax)
    , _hardMin(hardMin)
    , _hardMax(hardMax)
{
    setRange(min, max); // Before connections.

    // Internal connections.

    connect(this,
            SIGNAL(valueChanged(int)),
            SLOT(_onValueChanged(int)));

    connect(this,
            SIGNAL(rangeChanged(int,int)),
            SLOT(_onRangeChanged(int,int)));
}

// -----------------------------------------------------------------------------

// onValueChanged
// --------------
//! NB: Slider value is always based on evaluated expression! [slot]

void
NsValueIntSlider::onValueChanged()
{
    if (queryNumericConstantParam(_vbw->valueBaseObject()->longName(), comp())){
        setEnabled(true);
        show();
        _setValue(
            _vbw->stringEval(queryCurrentVisibleFrameTimeBundle(),
                             comp()));
    }
    else {
        setEnabled(false);
        hide();
    }
}

// -----------------------------------------------------------------------------

// _onValueChanged
// ---------------
//! [slot]

void
NsValueIntSlider::_onValueChanged(const int value)
{
    emit valueEdited(_value(), comp());
}


// _onRangeChanged
// ---------------
//! [slot]

void
NsValueIntSlider::_onRangeChanged(const int min, const int max)
{
    NsCmdSetMeta::exec(_vbw->valueBaseObject()->longName(),
                       "Slider Minimum",
                       QString::number(min));

    NsCmdSetMeta::exec(_vbw->valueBaseObject()->longName(),
                       "Slider Maximum",
                       QString::number(max));
}

// -----------------------------------------------------------------------------

// _setValue
// ---------
//! Set the state of the slider from a string.

void
NsValueIntSlider::_setValue(const QString &value)
{
    bool ok(false);
    const int ival(value.toInt(&ok));

    if (ok && ival != this->value()) {
        // Adjust range before setting value.

        if (minimum() > ival || ival > maximum()) {
            // The value we are trying to set is outside the current
            // range, update the range using clamped limits.

            const int newMin(qMax(_hardMin, qMin(ival, _softMin)));
            const int newMax(qMax(_softMax, qMin(ival, _hardMax)));

            if (minimum() != newMin || maximum() != newMax) {
                setRange(newMin, newMax);
            }
        }

        QSlider::setValue(ival);
    }
}
