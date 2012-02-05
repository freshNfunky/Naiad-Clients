// -----------------------------------------------------------------------------
//
// NsValueEditorWidget.h
//
// Naiad Studio value editor widget, header file.
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

#ifndef NS_VALUE_EDITOR_WIDGET_H
#define NS_VALUE_EDITOR_WIDGET_H

#include "NsValueObjectHandle.h"
#include "NsValueObjectBox.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QHash>
#include <QString>

class NsUndoStack;
class NsValueObject;
//class NsValueObjectBox;
//class QVBoxLayout;

// -----------------------------------------------------------------------------

// NsValueEditorWidget
// --------------------
//! The NsValueEditorWidget class.

class NsValueEditorWidget : public QWidget
{
    Q_OBJECT

public:

    explicit
    NsValueEditorWidget(NsUndoStack *undoStack, QWidget *parent = 0);

    //! DTOR.
    virtual
    ~NsValueEditorWidget()
    {}

    NsUndoStack*
    undoStack()
    { return _undoStack; }

protected:  // Events

    virtual void
    paintEvent(QPaintEvent *event);

protected slots:

    //void
    //onOpSelectionChanged(const QString &longName,
    //                     bool           selected,
    //                     bool           success);

    void
    onOpSelectionChanged(const QStringList &opInstances,
                         bool               success);            

    void
    onBodySelectionChanged(const QStringList &bodyLongNames,
                           bool               success);

    void
    onGraphCleared(bool success);

    void
    onValueObjectDestroyed(NsValueObject *vo);

private:

    template<class V>
    NsValueObjectBox*
    _createValueObjectBox(V *vo);

    //! Insert a value object box and setup necessary structures.
    template<class V>
    void
    _insertValueObjectBox(V *vo)
    { 
        if (!_valueObjectBoxes.contains(vo->handle())) {
            // Make sure box is removed when value object is destroyed.

            NsValueObjectBox *vob = _createValueObjectBox(vo);
            connect(vo,   SIGNAL(valueObjectDestroyed(NsValueObject*)),
                    this, SLOT(onValueObjectDestroyed(NsValueObject*)));
            _layout->insertWidget(_layout->count() - 1, vob);
            _valueObjectBoxes.insert(vo->handle(), vob);
        }
    }

    void
    _removeValueObjectBox(NsValueObject *vo);

    void
    _clear();

private:    // Member variables.

    typedef QHash<NsValueObjectHandle,NsValueObjectBox*>
        _ValueObjectBoxHashType;
    _ValueObjectBoxHashType _valueObjectBoxes;

    NsUndoStack *_undoStack;  //!< May be null.
    QVBoxLayout *_layout;     //!< The widget's layout.
};

// -----------------------------------------------------------------------------

#endif // NS_VALUE_EDITOR_WIDGET_H
