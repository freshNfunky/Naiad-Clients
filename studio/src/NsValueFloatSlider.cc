// -----------------------------------------------------------------------------
//
// NsValueFloatSlider.cc
//
// Naiad Studio float value slider, source file.
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

#include "NsValueFloatSlider.h"
#include "NsValueBaseWidget.h"
#include "NsValueBaseObject.h"
#include "NsCmdSetMeta.h"
#include "NsQuery.h"
#include <QtGui>

// -----------------------------------------------------------------------------

// NsValueFloatSlider
// ----------------
//! CTOR.

NsValueFloatSlider::NsValueFloatSlider(NsValueBaseWidget     &parent,
                                       const float            floatMin,
                                       const float            floatMax,
                                       const float            softMin,
                                       const float            softMax,
                                       const float            hardMin,
                                       const float            hardMax,
                                       const float            precision,
                                       const Qt::Orientation  orientation,
                                       const int              comp)
    : QSlider(orientation, &parent)
    , _vbw(&parent)
    , _comp(comp)
    , _floatMin(qMin(floatMin, floatMax))
    , _floatMax(qMax(floatMin, floatMax))
    , _softMin(qMin(softMin, softMax))
    , _softMax(qMax(softMin, softMax))
    , _hardMin(qMin(hardMin, hardMax))
    , _hardMax(qMax(hardMin, hardMax))
    , _precision(precision)
{
    _setFloatRange(_floatMin, _floatMax);
    connect(this, SIGNAL(valueChanged(int)), SLOT(_onValueChanged(int)));
}


// parentValueWidget
// -----------------
//! DOCS

const NsValueBaseWidget &
NsValueFloatSlider::parentValueWidget() const
{
    return *_vbw;
}

// -----------------------------------------------------------------------------

// onValueChanged
// --------------
//! NB: Slider value is always based on evaluated expression! [slot]

void
NsValueFloatSlider::onValueChanged()
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
NsValueFloatSlider::_onValueChanged(const int value)
{
    emit valueEdited(_value(), comp());
}

// -----------------------------------------------------------------------------

// _value
// ------
//! Returns the state of the slider as a string.

QString
NsValueFloatSlider::_value() const
{
    return QString::number(_floatFromValue());
}


// _setValue
// ---------
//! Set the state of the slider from a string.

void
NsValueFloatSlider::_setValue(const QString &value)
{
    bool ok = false;
    const double fval = value.toDouble(&ok);
    if (ok && fval != _floatFromValue()) {
        // Adjust range, if necessary, before setting value.
        // Check if the value we are trying to set is outside the current
        // range, if so update the range using clamped limits.

        if (fval < _floatMin) {
            const double newFloatMin = qMax(_hardMin, qMin(fval, _softMin));
            if (_floatMin != newFloatMin) {
                _setFloatRange(newFloatMin, _floatMax);
            }
        }
        else if (fval > _floatMax) {
            const double newFloatMax = qMax(_softMax, qMin(fval, _hardMax));
            if (_floatMax != newFloatMax) {
                _setFloatRange(_floatMin, newFloatMax);
            }
#if 0
            qDebug() << "Value: '" << value << "'";
            qDebug() << "Float Max: " << _floatMax << "|" << newFloatMax;
#endif
        }
        QSlider::setValue(_valueFromFloat(fval));
    }
}


int
NsValueFloatSlider::_range() const
{
    return (maximum() - minimum());
}


void
NsValueFloatSlider::_setFloatRange(const double floatMin, const double floatMax)
{
    _floatMin = qMin(floatMin, floatMax);
    _floatMax = qMax(floatMin, floatMax);
    QSlider::setMinimum(0);
    QSlider::setMaximum(_floatRange()/_precision + 1);

    NsCmdSetMeta::exec(parentValueWidget().valueBaseObject()->longName(),
                       "Slider Minimum",
                       QString::number(_floatMin));
    NsCmdSetMeta::exec(parentValueWidget().valueBaseObject()->longName(),
                       "Slider Maximum",
                       QString::number(_floatMax));

#if 0
    setToolTip("Range: [" + QString::number(minimum()) + "," +
                            QString::number(maximum()) + "]\n" +
               "Float: [" + QString::number(_floatMin) + "," +
                            QString::number(_floatMax) + "]");
#endif
}


// _floatRange
// -----------
//! Assumes _floatMax > _floatMin.

double
NsValueFloatSlider::_floatRange() const
{
    return (_floatMax - _floatMin);
}


// _floatFromValue
// ---------------
//! LERP.

double
NsValueFloatSlider::_floatFromValue() const
{
    const double alpha =
        static_cast<double>(value() - minimum())/qMax(1, _range());
    return _floatMin + alpha*_floatRange();
}


// _valueFromFloat
// ---------------
//! LERP.

int
NsValueFloatSlider::_valueFromFloat(const double f) const
{
    const double alpha = (f - _floatMin)/_floatRange();
    return minimum() + static_cast<int>(alpha*_range() + 0.499999);
}
