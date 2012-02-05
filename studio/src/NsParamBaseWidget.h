// -----------------------------------------------------------------------------
//
// NsParamBaseWidget.h
//
// Naiad Studio parameter base widget, header file.
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

#ifndef NS_PARAM_BASE_WIDGET_H
#define NS_PARAM_BASE_WIDGET_H

#include "NsValueBaseWidget.h"  // Base.
#include "NsOpObject.h"
#include <QAction>

class NsParamWidgetArgs;
class NsValueLineEdit;
class NsUndoStack;
class NsMetaObject;
class NsExprEditorDialog;

// -----------------------------------------------------------------------------

// NsParamBaseWidget
// -----------------
//! Abstract base for classes representing params visually in the value editor.

class NsParamBaseWidget : public NsValueBaseWidget
{
    Q_OBJECT

public:

    virtual
    ~NsParamBaseWidget() = 0;     // Force abstract.

    virtual void
    setReadOnly(bool readOnly);

    virtual void
    addContextActions(QMenu &menu, int comp) const;

    virtual QString
    stringEval(const NtTimeBundle &tb,
               int                 comp     = 0,
               int                 threadId = 0) const;

    //! Covariant return type.
    virtual NsOpObject* 
    valueObject() const
    { return _op; }

public slots:

    void
    onValueChanged(const QString &longName,
                   const QString &expr,
                   int            comp,
                   bool           success);

    void
    onMetaChanged(const QString &longName,
                  const QString &valueType,
                  const QString &value,
                  bool           success);

    void
    onCurrentVisibleFrameChanged(int cvf,
                                 bool update3DView,
                                 bool success);

signals:

    void
    valueChanged();

protected slots:

    virtual void
    onModeActionTriggered(QAction *action);

protected:

    explicit
    NsParamBaseWidget(const NsParamWidgetArgs &args);

    NsOpObject*
    op() const
    { return _op; }

    NsUndoStack*
    undoStack() const
    { return _undoStack; }

    QSet<QString>
    upstreamBodyNames(bool cached = true, bool live = true) const;

protected slots:

    virtual void
    onValueEdited(const QString &newValue, int comp = 0);

    virtual void
    onExprEditorActionTriggered(int comp);

private:    // Member variables.

    NsOpObject         *_op;        //!< Resource.
    NsUndoStack        *_undoStack; //!< Undo stack used with commands.
    //NsExprEditorDialog *_exprEditor;
};

// -----------------------------------------------------------------------------

class NsExprEditorAction : public QAction
{
    Q_OBJECT

public:

    explicit
    NsExprEditorAction(const QString &text, const int comp, QObject *parent)
        : QAction(text, parent)
        , _comp(comp)
    { connect(this, SIGNAL(triggered()), SLOT(_onTriggered())); }

signals:

    void 
    triggered(int comp);

private slots:

    void
    _onTriggered()
    { emit triggered(_comp); }

private:    // Member variables.

    int _comp;
};

// -----------------------------------------------------------------------------

#endif // NS_PARAM_BASE_WIDGET_H
