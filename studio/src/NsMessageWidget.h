// -----------------------------------------------------------------------------
//
// NsMessageWidget.h
//
// Naiad Studio message widget, header file.
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

#ifndef NS_MESSAGE_WIDGET_H
#define NS_MESSAGE_WIDGET_H

#include "NsMessageCallback.h"
#include "NsMessageItem.h"
#include <QTreeWidget>
#include <QIcon>

class NsMessageFilterAction;

QT_BEGIN_NAMESPACE
class QAction;
class QString;
QT_END_NAMESPACE

// -----------------------------------------------------------------------------

// NsMessageWidget
// ---------------
//! Shows a list of messages, both from Open Naiad Studio and Naiad.

class NsMessageWidget : public QTreeWidget
{
    Q_OBJECT

public:     // Singleton interface.

    static NsMessageWidget*
    instance();

    static void
    destroyInstance();

private:    // Singleton instance.

    static NsMessageWidget *_instance;

public:

    explicit
    NsMessageWidget(QWidget *parent = 0);

    //! DTOR.
    virtual
    ~NsMessageWidget()
    {}

public:

    void
    clientInfo(const QString &text, const QString &vobName = QString());

    void
    clientWarning(const QString &text, const QString &vobName = QString());

    void
    clientError(const QString &text, const QString &vobName = QString());

    void
    serverInfo(const QString &text, const QString &vobName = QString());

    void
    serverWarning(const QString &text, const QString &vobName = QString());

    void
    serverError(const QString &text, const QString &vobName = QString());

protected slots:

    void
    onItemSelectionChanged();

    void
    onGraphCleared(bool success);

private slots:

    void
    _pollServerInfo();

    void
    _pollServerWarnings();

    void
    _pollServerErrors();

    void
    _onCopySelection();

    void
    _onSelectSelection();

    void
    _onClear();

    void
    _onFilterChanged(bool checked, NsMessageItem::Type type);

private:


private:

    void
    _msg(const QString       &text,
         NsMessageItem::Type  type    = NsMessageItem::Info,
         const QString       &vobName = QString());

    int
    _msgCount() const;

    void
    _createActions();

    bool
    _isTypeHidden(NsMessageItem::Type type) const;

    bool
    _isInfoHidden() const;

    bool
    _isWarningHidden() const;

    bool
    _isErrorHidden() const;

private:    // Member variables.

    NsMessageCallback _cb;  //!< Receive messages from Ni through callbacks.

    QAction *_copyAction;   //!< The Copy action.
    QAction *_selectAction; //!< The Select action.
    QAction *_clearAction;  //!< The Clear action.

    NsMessageFilterAction *_filterInfoAction;    //!< The info filter action.
    NsMessageFilterAction *_filterWarningsAction;//!< The warning filter action.
    NsMessageFilterAction *_filterErrorsAction;  //!< The error filter action.
};

#endif // NS_MESSAGE_WIDGET_H
