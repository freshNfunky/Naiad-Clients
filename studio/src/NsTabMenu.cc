// -----------------------------------------------------------------------------
//
// NsTabMenu.cc
//
// Naiad Studio tab creation menu, source file.
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

#include "NsTabMenu.h"
#include "NsTabCategoryMenu.h"
#include "NsTabMenuAction.h"
#include "NsTabMenuLineEdit.h"
#include "NsCmdCreateAndSelect.h"
#include "NsCmdSetOpPosition.h"
#include "NsUndoStack.h"
#include "NsGraphView.h"
#include "NsQuery.h"
#include <QWidgetAction>
#include <QLabel>
#include <QLineEdit>
#include <QCompleter>
#include <QSet>
#include <QMap>
#include <QDebug>

// -----------------------------------------------------------------------------

// NsTabMenu
// ---------
//! CTOR.

NsTabMenu::NsTabMenu(NsGraphView &graphView,
                     NsUndoStack *undoStack,
                     QWidget     *parent)
    : QMenu(parent)
    , _graphView(&graphView)
    , _undoStack(undoStack)
{
    // Label.

    QWidgetAction *waLabel(new QWidgetAction(this));    // Child.
    QLabel *label(new QLabel("TAB Menu", this));        // Child.
    label->setAlignment(Qt::AlignHCenter);
    waLabel->setDefaultWidget(label);
    addAction(waLabel);

    //addSeparator(); // ------------------------

    const QStringList opTypes(queryOpTypeNames(false));

    typedef QMap<QString, QStringList> CategoryMapType;
    CategoryMapType categories;    

    foreach (const QString &opType, opTypes) {
        const QStringList typeCategories(queryOpTypeCategories(opType));
        foreach (const QString &category, typeCategories) {
            CategoryMapType::iterator iter(categories.find(category));
            if (categories.end() != iter) {
                // Existing category.

                iter.value().append(opType);
            }
            else {
                // New category.

                categories.insert(category, QStringList(opType));
            }
        }
    }

    // Line edit.

    QWidgetAction *waLineEdit(new QWidgetAction(this));         // Child.
    NsTabMenuLineEdit *lineEdit(new NsTabMenuLineEdit(this));   // Child.
    // lineEdit->setPlaceholderText(tr("(type to search)")); // In Qt 4.7
    QCompleter *completer(new QCompleter(opTypes, lineEdit));
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::InlineCompletion);
    //completer->setCompletionMode(QCompleter::PopupCompletion);
    //completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    lineEdit->setCompleter(completer);
    connect(lineEdit, SIGNAL(returnPressed(QString)),
            this,     SLOT(onReturnPressed(QString)));
    waLineEdit->setDefaultWidget(lineEdit);
    addAction(waLineEdit);

    //addSeparator(); // ------------------------

    const CategoryMapType::iterator iend(categories.end());
    for (CategoryMapType::iterator iter(categories.begin());
         iter != iend;
         ++iter) {
        // Create a separate menu for each category.

        iter.value().sort();
        addMenu(_createCategoryMenu(iter.key(), iter.value()));
    }

    lineEdit->setFocus(); // NB: Must do this after widgets have been created!
}

// -----------------------------------------------------------------------------

// onReturnPressed
// ---------------
//! Function called when an Op type has been typed. [slot]

void
NsTabMenu::onReturnPressed(const QString &text)
{
    _createOp(text);
    hide(); // Hide the menu.
    //close();  // TODO: Better?
}

// -----------------------------------------------------------------------------

// _createCategoryMenu
// -------------------
//! ...

NsTabCategoryMenu*
NsTabMenu::_createCategoryMenu(const QString     &category, 
                               const QStringList &opTypes)
{
    NsTabCategoryMenu *menu(new NsTabCategoryMenu(category, this)); // Child.

    foreach (const QString &opType, opTypes) {
        menu->addAction(_createAction(opType, menu));
    }

    return menu;
}


// _createAction
// -------------
//! Returns an action suitable for an Op family menu.

NsTabMenuAction*
NsTabMenu::_createAction(const QString &opType, QMenu *menu)
{
    NsTabMenuAction *action(new NsTabMenuAction(opType, menu)); // Child.
    connect(action, 
            SIGNAL(triggered(QString)), 
            SLOT(_onActionTriggered(QString)));
    return action;
}


// _createOp
// ---------
//! Create an Op with an undoable command. The command generates a
//! suitable Op name.

void
NsTabMenu::_createOp(const QString &opType)
{
    if (0 != _undoStack) {
        NsCmdCreateAndSelect::exec(
            opType,
            opType,
            *_undoStack,
            _graphView->mapToSceneClamped(
               _graphView->viewport()->mapFromGlobal(QCursor::pos()),
               _graphView->rect()));
    }
    else {
        NsCmdCreateAndSelect::exec(
            opType,
            opType,
            _graphView->mapToSceneClamped(
                _graphView->viewport()->mapFromGlobal(QCursor::pos()),
                _graphView->rect()));
    }
}
