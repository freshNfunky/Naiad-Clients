// -----------------------------------------------------------------------------
//
// NsPlayblastProgressWidget.cc
//
// Naiad Studio playblast progress widget, header file.
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

#include "NsPlayblastProgressWidget.h"
#include <QProgressDialog>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QVBoxLayout>

// -----------------------------------------------------------------------------

// NsPlayblastProgressWidget
// -------------------------
//! CTOR.

NsPlayblastProgressWidget::NsPlayblastProgressWidget(const int      min,
                                                     const int      max,
                                                     const QString &title,
                                                     QWidget       *parent)
    : QWidget(parent)
    , _progressDialog(new QProgressDialog)
    , _treeWidget(new QTreeWidget)
{
    setWindowTitle(title);
    setWindowModality(Qt::WindowModal);

    QVBoxLayout *layout = new QVBoxLayout;
    //QGridLayout *layout = new QGridLayout;

    // Progress dialog.

    _progressDialog->setMinimum(min);
    _progressDialog->setMaximum(max);
    _progressDialog->setCancelButtonText("Cancel");
    _progressDialog->setOrientation(Qt::Horizontal);
    _progressDialog->setWindowModality(Qt::WindowModal);
    layout->addWidget(_progressDialog, Qt::AlignCenter);

    // Tree widget.

    _treeWidget->setSelectionMode(QTreeView::ExtendedSelection);
    _treeWidget->setSelectionBehavior(QTreeView::SelectRows);
    _treeWidget->setRootIsDecorated(false);
    _treeWidget->setAllColumnsShowFocus(true);
    _treeWidget->header()->hide();

    layout->addWidget(_treeWidget);

    setLayout(layout);

    //resize(500, 0);
}


bool
NsPlayblastProgressWidget::wasCanceled() const
{
    return _progressDialog->wasCanceled();
}


void
NsPlayblastProgressWidget::setValue(const int progress)
{
    _progressDialog->setValue(progress);
}


void
NsPlayblastProgressWidget::addFileName(const QString &fileName)
{
    QString itemText(fileName);
    QTreeWidgetItem *item(
        new QTreeWidgetItem(QStringList(itemText.replace('\n', ' '))));

    _treeWidget->addTopLevelItem(item);  // Widget takes ownership.
    _treeWidget->setCurrentItem(item);
    _treeWidget->scrollToBottom();
}
