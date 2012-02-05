// -----------------------------------------------------------------------------
//
// NsFocusAction.cc
//
// Naiad Studio focus action source file.
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

#include "NsFocusAction.h"
#include <QtGui>

// -----------------------------------------------------------------------------

// NsFocusAction
// -------------
//! CTOR.

NsFocusAction::NsFocusAction(const QString &text, QObject *parent)
    : QAction(text, parent)
{
    connect(this,
            SIGNAL(triggered()),
            SLOT(onTriggered()));

    connect(qApp,
            SIGNAL(focusChanged(QWidget*, QWidget*)),
            SLOT(onFocusChanged()));
    onFocusChanged();
}


// addAction
// ---------
//! Adds the given action as corresponding action for the given widget.

void
NsFocusAction::addAction(QWidget *widget, QAction *action)
{
    _actions.insert(widget, action);
    connect(action, SIGNAL(changed()), SLOT(onFocusChanged()));
}


// setStatusTip
// ------------
//! Sets the status tip of the action, and stores it so it can be reset to this
//! tip when no widget with a corresponding action has focus.

void
NsFocusAction::setStatusTip(const QString &tip)
{
    QAction::setStatusTip(tip);
    _statusTip = tip;
}

// -----------------------------------------------------------------------------

// onTriggered
// -----------
//! Triggers the action corresponding to the focused widget. [slot]

void
NsFocusAction::onTriggered()
{
    QWidget *widget(QApplication::focusWidget());
    //QWidget *widget = qApp->widgetAt(QCursor::pos());

    if (_actions.contains(widget)) {
        _actions.value(widget)->trigger();
    }
}


// onFocusChanged
// --------------
//! Updates the action so that it corresponds to the corresponding action in
//! the focused widget. [slot]

void
NsFocusAction::onFocusChanged()
{
    QWidget *widget(QApplication::focusWidget());

    if (_actions.contains(widget)) {
        QAction *action(_actions.value(widget));
        setEnabled(action->isEnabled());
        QAction::setStatusTip(action->statusTip());
    }
    else {
        QAction::setStatusTip(_statusTip);
        setEnabled(false);
    }
}
