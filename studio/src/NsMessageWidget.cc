// -----------------------------------------------------------------------------
//
// NsMessageWidget.cc
//
// Naiad Studio message widget source file.
//
// Copyright (c) 2011 Exotic Matter AB.  All rights reserved.
//
// This file is part of Open Naiad Studio..
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

#include "NsMessageWidget.h"
#include "NsMessageFilterAction.h"
#include "NsOpStore.h"
#include "NsOpObject.h"
#include "NsCmdCentral.h"
#include "NsCmdSelectOp.h"
#include "NsCmdFrameGraphView.h"
#include <QHeaderView>
#include <QApplication>
#include <QClipboard>
#include <QTreeWidgetItemIterator>

// -----------------------------------------------------------------------------

// instance
// --------
//! Returns the singleton instance, creates a new instance if necessary.
//! [static]

NsMessageWidget *
NsMessageWidget::instance()
{
    if (0 == _instance) {
        _instance = new NsMessageWidget;
    }

    return _instance;
}


// destroyInstance
// ---------------
//! Destroy the singleton instance.
//! [static]

void
NsMessageWidget::destroyInstance()
{
    delete _instance;
}


//! Singleton instance. [static]
NsMessageWidget *NsMessageWidget::_instance(0);

// -----------------------------------------------------------------------------

// NsMessageWidget
// ---------------
//! CTOR.

NsMessageWidget::NsMessageWidget(QWidget *parent)
    : QTreeWidget(parent)
{
    setSelectionMode(ExtendedSelection);
    setSelectionBehavior(SelectRows);
    setRootIsDecorated(false);
    setAllColumnsShowFocus(true);

    header()->hide();

    _createActions();
    setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(this,
            SIGNAL(itemSelectionChanged()),
            SLOT(onItemSelectionChanged()));
    onItemSelectionChanged();

    connect(_cb.notifier(), SIGNAL(info()),    SLOT(_pollServerInfo()));
    connect(_cb.notifier(), SIGNAL(warning()), SLOT(_pollServerWarnings()));
    connect(_cb.notifier(), SIGNAL(error()),   SLOT(_pollServerErrors()));
}

// -----------------------------------------------------------------------------

// clientInfo
// ----------
//! Adds an info message with the given text to the bottom of the list.

void
NsMessageWidget::clientInfo(const QString &text, const QString &vobName)
{
    _msg(text, NsMessageItem::Info, vobName);
}


// clientWarning
// -------------
//! Adds a warning with the given text to the bottom of the list.

void
NsMessageWidget::clientWarning(const QString &text, const QString &vobName)
{
    _msg(text, NsMessageItem::Warning, vobName);
}


// clientError
// -----------
//! Adds an error with the given text to the bottom of the list.

void
NsMessageWidget::clientError(const QString &text, const QString &vobName)
{
    _msg(text, NsMessageItem::Error, vobName);
}


// serverInfo
// ----------
//! DOCS

void
NsMessageWidget::serverInfo(const QString &text, const QString &vobName)
{
    _msg(text, NsMessageItem::Info, vobName);
}


// serverWarning
// -------------
//! DOCS

void
NsMessageWidget::serverWarning(const QString &text, const QString &vobName)
{
    _msg(text, NsMessageItem::Warning, vobName);
    NsOpObject *op = NsOpStore::instance()->queryMutableOp(vobName);
    if (0 != op) {
        op->setCondition(NsOpObject::Warning);
        //NsCmdFrameGraphView::exec();
    }
}


// serverError
// -----------
//! DOCS

void
NsMessageWidget::serverError(const QString &text, const QString &vobName)
{
    _msg(text, NsMessageItem::Error, vobName);
    NsOpObject *op = NsOpStore::instance()->queryMutableOp(vobName);
    if (0 != op) {
        op->setCondition(NsOpObject::Error);
        //NsCmdFrameGraphView::exec(false, 200);
    }
}

// -----------------------------------------------------------------------------

// onItemSelectionChanged
// ----------------------
//! [slot]

void
NsMessageWidget::onItemSelectionChanged()
{
    const bool actionsEnabled = !selectedItems().isEmpty();
    _copyAction->setEnabled(actionsEnabled);
    _selectAction->setEnabled(actionsEnabled);
}


// onGraphCleared
// --------------
//! [slot]

void
NsMessageWidget::onGraphCleared(const bool success)
{
    if (success) {
        clear();
    }
}

// -----------------------------------------------------------------------------

// _pollServerInfo
// ---------------
//! Grab info messages from server callback. [slot]

void
NsMessageWidget::_pollServerInfo()
{
    NsMessageCallback::Message msg;
    while (_cb.pollInfo(msg)) {
        serverInfo(msg.text, msg.vobName);
    }
}


// _pollServerWarnings
// -------------------
//! Grab warning messages from server callback. [slot]

void
NsMessageWidget::_pollServerWarnings()
{
    NsMessageCallback::Message msg;
    while (_cb.pollWarning(msg)) {
        serverWarning(msg.text, msg.vobName);
    }
}


// _pollServerErrors
// -----------------
//! Grab error messages from server callback. [slot]

void
NsMessageWidget::_pollServerErrors()
{
    NsMessageCallback::Message msg;
    while (_cb.pollError(msg)) {
        serverError(msg.text, msg.vobName);
    }
}


// _copySelection
// --------------
//! Copies the selected message to the text clipboard. [slot]

void
NsMessageWidget::_onCopySelection()
{
    QString text;
    const QList<QTreeWidgetItem*> selection(selectedItems());

    foreach (const QTreeWidgetItem *item, selection)
    {
        switch (item->type()) {
        case NsMessageItem::Info:
            // TODO!
            break;
        case NsMessageItem::Warning:
            text += "WARNING: ";
            break;
        case NsMessageItem::Error:
            text += "ERROR: ";
            break;
        case NsMessageItem::InternalError:
            text += "INTERNAL ERROR: ";
            break;
        default:
            break;
        }

        text += item->text(0) + "\n";
    }

    QApplication::clipboard()->setText(text);
}


// _onSelectSelection
// ------------------
//! [slot]

void
NsMessageWidget::_onSelectSelection()
{
    const QList<QTreeWidgetItem *> selection(selectedItems());

    foreach (const QTreeWidgetItem *twItem, selection) {
        const NsMessageItem *msgItem(
            dynamic_cast<const NsMessageItem*>(twItem));

        if (0 != msgItem && !msgItem->vobName().isEmpty()) {
            NsCmdSelectOp::exec(
                NsCmdSelectOp::ArgsList() <<
                    NsCmdSelectOp::Args(msgItem->vobName(), true));
        }
    }
}


// _onClear
// --------
//! [slot]

void
NsMessageWidget::_onClear()
{
    clear();
}


// _onFilterChanged
// ----------------
//! [slot]

void
NsMessageWidget::_onFilterChanged(const bool                checked,
                                  const NsMessageItem::Type type)
{
    QTreeWidgetItemIterator iter(this);

    while (*iter) {
        NsMessageItem *msgItem(dynamic_cast<NsMessageItem*>((*iter)));

        if (0 != msgItem && msgItem->type() == type) {
            if (checked) {
                // Show item.

                msgItem->setHidden(false);
            }
            else {
                // Unselect and hide item.

                msgItem->setSelected(false);
                msgItem->setHidden(true);
            }
        }

        ++iter;
    }
}

// -----------------------------------------------------------------------------



// -----------------------------------------------------------------------------

// _msg
// ----
//! Add a message with a suitable icon to the tree view.

void
NsMessageWidget::_msg(const QString             &text,
                      const NsMessageItem::Type  type,
                      const QString             &vobName)
{
    QString itemText;
    if (!vobName.isEmpty()) {
        itemText = vobName + ": " + text;
    }
    else {
        itemText = text;
    }

    NsMessageItem *item(
        new NsMessageItem(QStringList(itemText.replace('\n', ' ')),
                          type,
                          vobName));

    addTopLevelItem(item);  // Widget takes ownership.

    const bool hidden = _isTypeHidden(type);
    if (!hidden) {
        setCurrentItem(item);
        scrollToBottom();
    }
    item->setHidden(hidden);
}


// _msgCount
// ---------
//! Returns the number of messages in the message widget.

int
NsMessageWidget::_msgCount() const
{
    return topLevelItemCount();
}


// _createActions
// --------------
//! Create the actions for this widget.

void
NsMessageWidget::_createActions()
{
    // Copy action.

    _copyAction = new QAction("&Copy", this);   // Child.
    _copyAction->setShortcut(QKeySequence::Copy);
    _copyAction->setShortcutContext(Qt::WidgetShortcut);
    _copyAction->setStatusTip("Copy the selected messages to the clipboard");
    _copyAction->setEnabled(false);
    connect(_copyAction, SIGNAL(triggered()), SLOT(_onCopySelection()));
    addAction(_copyAction);


    // Select action.

    _selectAction = new QAction("&Select", this); // Child.
    // TODO: Shortcuts?
    _selectAction->setStatusTip("Select the object in question");
    _selectAction->setEnabled(false);
    connect(_selectAction, SIGNAL(triggered()), SLOT(_onSelectSelection()));
    addAction(_selectAction);


    // Clear action.

    _clearAction = new QAction("&Clear", this); // Child.
    _clearAction->setStatusTip("Clear messages");
    connect(_clearAction, SIGNAL(triggered()), SLOT(_onClear()));
    addAction(_clearAction);


    // Filter actions.

    _filterInfoAction =
        new NsMessageFilterAction("Show info",
                                   NsMessageItem::Info,
                                   this);   // Child.
    _filterInfoAction->setStatusTip("Show info messages");
    _filterInfoAction->setCheckable(true);
    _filterInfoAction->setChecked(true);
    connect(_filterInfoAction,
            SIGNAL(toggled(bool,NsMessageItem::Type)),
            SLOT(_onFilterChanged(bool,NsMessageItem::Type)));
    addAction(_filterInfoAction);

    _filterWarningsAction =
        new NsMessageFilterAction("Show warnings",
                                  NsMessageItem::Warning,
                                  this);   // Child.
    _filterWarningsAction->setStatusTip("Show warning messages");
    _filterWarningsAction->setCheckable(true);
    _filterWarningsAction->setChecked(true);
    connect(_filterWarningsAction,
            SIGNAL(toggled(bool,NsMessageItem::Type)),
            SLOT(_onFilterChanged(bool,NsMessageItem::Type)));
    addAction(_filterWarningsAction);

    _filterErrorsAction =
        new NsMessageFilterAction("Show errors",
                                  NsMessageItem::Error,
                                  this);   // Child.
    _filterErrorsAction->setStatusTip("Show error messages");
    _filterErrorsAction->setCheckable(true);
    _filterErrorsAction->setChecked(true);
    connect(_filterErrorsAction,
            SIGNAL(toggled(bool,NsMessageItem::Type)),
            SLOT(_onFilterChanged(bool,NsMessageItem::Type)));
    addAction(_filterErrorsAction);
}


// _isTypeHidden
// -------------
//! DOCS

bool
NsMessageWidget::_isTypeHidden(const NsMessageItem::Type type) const
{
    switch (type) {
    case NsMessageItem::Info:
        return _isInfoHidden();
    case NsMessageItem::Warning:
        return _isWarningHidden();
    case NsMessageItem::Error:
        return _isErrorHidden();
//    case NsMessageItem::InternalError:
//        return _isInternalErrorHidden();
    }

    return false;
}

bool
NsMessageWidget::_isInfoHidden() const
{ return !_filterInfoAction->isChecked(); }

bool
NsMessageWidget::_isWarningHidden() const
{ return !_filterWarningsAction->isChecked(); }

bool
NsMessageWidget::_isErrorHidden() const
{ return !_filterErrorsAction->isChecked(); }

// -----------------------------------------------------------------------------
