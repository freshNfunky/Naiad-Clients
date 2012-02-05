// -----------------------------------------------------------------------------
//
// NsValueLineEdit.h
//
// Naiad Studio value expression line edit, header file.
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

#ifndef NS_VALUE_LINE_EDIT_H
#define NS_VALUE_LINE_EDIT_H

#include "NsLineEdit.h"

class NsValueBaseWidget;

// -----------------------------------------------------------------------------

// NsValueLineEdit
// ---------------
//! DOCS

class NsValueLineEdit : public NsLineEdit
{
    Q_OBJECT
    Q_PROPERTY(QString modeName READ modeName);

public:

    static const QString modeValueType;

    enum Mode { expression = 0, evaluation, numModes };

    explicit
    NsValueLineEdit(NsValueBaseWidget &parent, int comp = 0);

    //! DTOR.
    virtual
    ~NsValueLineEdit() 
    {}

    // Copy and assign disabled in base class.

    const NsValueBaseWidget&
    parentValueWidget() const
    { return *_vbw; }

    int
    comp() const
    { return _comp; }

    Mode
    mode() const
    { return _mode; }

    const QString&
    modeName() const
    { return modeName(mode()); }

    const QString&
    modeName(Mode mode) const;

    void
    setMode(const Mode newMode)
    { _setMode(newMode); }

    void
    setMode(const QString &name);

public slots:

    void
    onValueChanged();

signals:

    void 
    valueEdited(const QString &newValue, int comp);

protected:

    virtual void
    returnPressed();

protected:      // Event handlers.

    virtual void
    focusInEvent(QFocusEvent *event);

    virtual void
    focusOutEvent(QFocusEvent *event);

    virtual void
    contextMenuEvent(QContextMenuEvent *event);

private slots:

    void
    _onEditingFinished();

    void
    _onMetaChanged(const QString &longName,
                   const QString &valueType,
                   const QString &value,
                   bool           success);

private:

    void
    _setMode(Mode mode);

    void
    _updateStyle();

    static const QString _modeNames[];

private:    // Member variables.

    NsValueBaseWidget  *_vbw;    //!< Parent.
    int                 _comp;   //!< Component.
    Mode                _mode;
    bool                _mouseStoleFocus;

};

#endif // NS_VALUE_LINE_EDIT_H
