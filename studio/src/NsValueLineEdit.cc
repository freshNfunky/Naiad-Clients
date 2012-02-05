// -----------------------------------------------------------------------------
//
// NsValueLineEdit.cc
//
// Naiad Studio value expression line edit, source file.
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

#include "NsValueLineEdit.h"
#include "NsValueBaseWidget.h"
#include "NsValueBaseObject.h"
#include "NsQuery.h"
#include "NsCmdCentral.h"
#include "NsCmdSetMeta.h"
#include <QtGui>

// -----------------------------------------------------------------------------

const QString NsValueLineEdit::modeValueType("Mode");  //!< [static]

// -----------------------------------------------------------------------------

// NsValueLineEdit
// ---------------
//! CTOR.

NsValueLineEdit::NsValueLineEdit(NsValueBaseWidget &parent,
                                 const int          comp)
    : NsLineEdit(&parent)
    , _vbw(&parent)
    , _comp(comp)
    , _mode(expression) // Default.
    , _mouseStoleFocus(false) 
{
    // Check for existing meta value regarding mode.

    QString metaMode(_modeNames[0]);
    if (parentValueWidget().valueBaseObject()->metaValue(modeValueType,
                                                         &metaMode)) {
        setMode(metaMode);
    }
    else {
        _setMode(mode());   // Set default.
    }

    connect(this,
            SIGNAL(editingFinished()),
            SLOT(_onEditingFinished()));

    connect(NsCmdCentral::instance(),
            SIGNAL(metaChanged(QString,QString,QString,bool)),
            this,
            SLOT(_onMetaChanged(QString,QString,QString,bool)));
}


// modeName
// --------
//! DOCS

const QString&
NsValueLineEdit::modeName(const Mode mode) const
{
    EM_ASSERT(mode != numModes);
    return _modeNames[mode];
}


// setMode
// -------
//! DOCS

void
NsValueLineEdit::setMode(const QString &name)
{
    qDebug() << "NsValueLineEdit::setMode:" << name;
    for (int i(0); i < static_cast<int>(numModes); ++i) {
        if (name == _modeNames[i]) {
            _setMode(static_cast<Mode>(i));
            break;
        }
    }
}

// -----------------------------------------------------------------------------

// onValueChanged
// --------------
//! DOCS [slot]

void
NsValueLineEdit::onValueChanged()
{
    switch (mode()) {
    case expression:
        setText(_vbw->valueBaseObject()->expr(comp()));
        //clearFocus();   // Loose focus.
        break;
    case evaluation:
        {
        int cvf(0);
        queryCurrentVisibleFrame(&cvf); // Ignore success.
        setText(_vbw->stringEval(queryFrameTimeBundle(cvf), comp()));
        //clearFocus();   // Loose focus.
        break;
        }
    case numModes:
    default:
        break;
    }
}

// -----------------------------------------------------------------------------

void
NsValueLineEdit::returnPressed()
{
    emit valueEdited(text(), comp());
    NsLineEdit::returnPressed();    // Parent method.
}

// -----------------------------------------------------------------------------

void
NsValueLineEdit::focusInEvent(QFocusEvent *event)
{
    qDebug() << "NsValueLineEdit::focusInEvent";       
    NsLineEdit::focusInEvent(event); 
}

void
NsValueLineEdit::focusOutEvent(QFocusEvent *event)
{
    qDebug() << "NsValueLineEdit::focusOutEvent";
    Qt::FocusReason reason = event->reason();
    if(reason == Qt::PopupFocusReason) {
        qDebug() << "mouseStoleFocus=TRUE";
        _mouseStoleFocus = true;
    }
    NsLineEdit::focusOutEvent(event); 
}


// contextMenuEvent
// ----------------
//! DOCS

void
NsValueLineEdit::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;
    QAction editAction(tr("Show &Expression"), &menu);
    editAction.setObjectName(modeName(expression));
    editAction.setCheckable(true);
    editAction.setChecked(mode() == expression);

    QAction evalAction(tr("E&valuate"), &menu);
    evalAction.setObjectName(modeName(evaluation));
    evalAction.setCheckable(true);
    evalAction.setChecked(mode() == evaluation);

    QActionGroup modeGroup(&menu);
    modeGroup.addAction(&editAction);
    modeGroup.addAction(&evalAction);
    modeGroup.setExclusive(true);
    connect(&modeGroup,           SIGNAL(triggered(QAction *)),
            &parentValueWidget(), SLOT(onModeActionTriggered(QAction *)));

    menu.addActions(modeGroup.actions());
    menu.addSeparator();

    // Let parent widget add stuff to the menu.

    parentValueWidget().addContextActions(menu, comp());

    if (!menu.isEmpty()) {
        menu.exec(event->globalPos());
    }
}

// -----------------------------------------------------------------------------

// _onEditingFinished
// ------------------
//! DOCS [slot]

void
NsValueLineEdit::_onEditingFinished()
{
    qDebug() << "NsValueLineEdit::_onEditingFinished";
    
    if(!_mouseStoleFocus) {
        qDebug() << "emitting ValueEdited";
        emit valueEdited(text(), comp());
    } else {
        qDebug() << "mouseStoleFocus=TRUE";
    }
    
    _mouseStoleFocus = false;
}


// _onMetaChanged
// --------------
//! DOCS [slot]

void
NsValueLineEdit::_onMetaChanged(const QString &longName,
                                const QString &valueType,
                                const QString &value,
                                const bool     success)
{
    if (success &&
        modeValueType == valueType &&
        parentValueWidget().valueBaseObject()->longName() == longName) {
        setMode(value);
    }
}

// -----------------------------------------------------------------------------

// _setMode
// --------
//! DOCS

void
NsValueLineEdit::_setMode(const Mode mode)
{
    qDebug() << "NsValueLineEdit::_setMode:" << static_cast<int>(mode);
    switch (mode) {
    case expression:
        setReadOnly(false);                 // Enable editing.
        setFocusPolicy(Qt::StrongFocus);    // Allow focusing.
        _mode = mode;
        _updateStyle();
        onValueChanged();
        break;
    case evaluation:
        setReadOnly(true);                  // Disable editing.
        clearFocus();                       // Loose focus.
        setFocusPolicy(Qt::NoFocus);        // Disable focusing.
        _mode = mode;
        _updateStyle();
        onValueChanged();
        break;
    case numModes:
    default:
        break;
    }
}


// _updateStyle
// ------------
//! Make sure the appearance of this widget can be control through CSS
//! even when properties change dynamically. It is necessary to call this
//! function when a property (e.g. "mode") is changed. Hopefully future
//! versions of Qt will handle this better.

void
NsValueLineEdit::_updateStyle()
{
#if 1
    style()->unpolish(this);
    style()->polish(this);
    ensurePolished();
#else
    setStyleSheet(styleSheet());
#endif
}

// -----------------------------------------------------------------------------

//! [static]
const QString NsValueLineEdit::_modeNames[] = {
    QString("expression"),
    QString("evaluation")
};
