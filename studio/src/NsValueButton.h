// -----------------------------------------------------------------------------
//
// NsValueButton.h
//
// Naiad Studio value button, header file.
//
// Copyright (c) 2011 Exotic Matter AB.  All rights reserved.
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

#ifndef NS_VALUE_BUTTON_H
#define NS_VALUE_BUTTON_H

#include <QToolButton>

class NsValueBaseWidget;

// -----------------------------------------------------------------------------

// NsValueButton
// -------------
//! A button connected to a value.

class NsValueButton : public QToolButton
{
    Q_OBJECT

public:

    explicit NsValueButton(NsValueBaseWidget &parent);
    virtual ~NsValueButton() {}     //!< DTOR.

    // Copy and assign disabled in base class.

    const NsValueBaseWidget &
    parentValueWidget() const;

    int
    comp() const
    { return _comp; }

    const QString &
    checkedValue() const
    { return _checkedValue;   }

    const QString &
    uncheckedValue() const
    { return _uncheckedValue; }

    void
    setCheckedValue(const QString &cv)
    { _checkedValue = cv; }

    void
    setUncheckedValue(const QString &ucv)
    { _uncheckedValue = ucv; }

public slots:

    void
    onValueChanged();

signals:

    void valueEdited(const QString &newValue, int comp);

private slots:

    void
    _onClicked(const bool checked);

private:

    //! Returns the state of the button as a string.
    const QString &
    _value(const bool checked) const
    { return (checked ? _checkedValue : _uncheckedValue); }

    void
    _setValue(const QString &value);

private:    // Member variables.

    NsValueBaseWidget *_vbw;    //!< Parent.
    int                _comp;   //!< Component.
    QString            _checkedValue;
    QString            _uncheckedValue;
};

#endif  // NS_VALUE_BUTTON_H
