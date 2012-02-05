// -----------------------------------------------------------------------------
//
// NsTabCategoryMenu.h
//
// Naiad Studio tab creation category menu, header file.
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

#ifndef NS_TAB_MENU_LINE_EDIT_H
#define NS_TAB_MENU_LINE_EDIT_H

#include <QLineEdit>
#include <QEvent>
#include <QKeyEvent>
#include <QDebug>

// -----------------------------------------------------------------------------

// NsTabMenuLineEdit
// -----------------
//! For CSS.

class NsTabMenuLineEdit : public QLineEdit
{
    Q_OBJECT

public:

    //! CTOR.
    explicit
    NsTabMenuLineEdit(QWidget *parent = 0)
        : QLineEdit(parent)
    {
        connect(this, 
                SIGNAL(returnPressed()),
                SLOT(_onReturnPressed()));
    }

    //! DTOR.
    virtual 
    ~NsTabMenuLineEdit() 
    {}     

signals:

    void
    returnPressed(const QString &text);

protected:

    virtual bool
    event(QEvent *event)
    {
        if (QEvent::KeyPress == event->type()) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);   // Safe.

            if (Qt::Key_Tab == keyEvent->key()) {
                qDebug() << "NsTabMenuLineEdit::event - tab";
                QKeyEvent ke(keyEvent->type(),
                             Qt::Key_Right,
                             keyEvent->modifiers());
                keyPressEvent(&ke);
                return true;    // Event was processed.
            }
        }

        return QLineEdit::event(event);
    }

    virtual void
    keyPressEvent(QKeyEvent *event)
    { QLineEdit::keyPressEvent(event); }

private slots:

    void
    _onReturnPressed()
    { emit returnPressed(text()); }
};

#endif // NS_TAB_MENU_LINE_EDIT_H
