// -----------------------------------------------------------------------------
//
// NsValueFloatSlider.h
//
// Naiad Studio float value slider, header file.
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

#ifndef NS_VALUE_FLOAT_SLIDER_H
#define NS_VALUE_FLOAT_SLIDER_H

#include <QSlider>

class NsValueBaseWidget;

// -----------------------------------------------------------------------------

// NsValueFloatSlider
// ------------------
//! A slider for editing float values.

class NsValueFloatSlider : public QSlider
{
    Q_OBJECT

public:

    explicit
    NsValueFloatSlider(NsValueBaseWidget &parent,
                       float              min,
                       float              max,
                       float              softMin,
                       float              softMax,
                       float              hardMin,
                       float              hardMax,
                       float              precision   = 0.1f,
                       Qt::Orientation    orientation = Qt::Horizontal,
                       int                comp        = 0);

    //! DTOR.
    virtual
    ~NsValueFloatSlider()
    {}

    const NsValueBaseWidget&
    parentValueWidget() const;

    int
    comp() const
    { return _comp; }

public slots:

    void
    onValueChanged();

signals:

    void
    valueEdited(const QString &newValue, int comp);

private slots:

    void
    _onValueChanged(int value);

private:

    QString
    _value() const;

    void
    _setValue(const QString &value);

    int
    _range() const;

    void
    _setFloatRange(double floatMin, double floatMax);

    double
    _floatRange() const;

    double
    _floatFromValue() const;

    int
    _valueFromFloat(double f) const;

private:    // Member variables.

    NsValueBaseWidget *_vbw;    //!< Parent.
    int                _comp;   //!< Component.
    double             _floatMin;
    double             _floatMax;
    const double       _softMin;
    const double       _softMax;
    const double       _hardMin;
    const double       _hardMax;
    const double       _precision;
};

#endif // NS_VALUE_FLOAT_SLIDER_H
