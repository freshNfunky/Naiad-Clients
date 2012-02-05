// -----------------------------------------------------------------------------
//
// NsOpHeaderBox.cc
//
// Naiad Studio Op header box, source file.
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

#include "NsOpHeaderBox.h"
#include "NsOpNameLineEdit.h"
#include "NsOpObject.h"
#include "NsValueWidgetLayout.h"
#include "NsValueLabel.h"
#include "NsValueLineEdit.h"
#include "NsValueRegExpValidator.h"
#include "NsCmdRename.h"
#include <QVBoxLayout>

// -----------------------------------------------------------------------------

// NsOpHeaderBox
// -------------
//! CTOR.

NsOpHeaderBox::NsOpHeaderBox(NsOpObject     *op,
                             const QString  &title,
                             NsUndoStack *undoStack,
                             QWidget        *parent)
    : NsValueHeaderBox(title, parent)
    , _op(op)
    , _undoStack(undoStack)
{
    QVBoxLayout *vlayout(new QVBoxLayout);

    // Op Type.

    NsValueLabel *typeLabel0(new NsValueLabel("Type:"));
    typeLabel0->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    NsValueLabel *typeLabel1(new NsValueLabel(_op->typeName()));
    typeLabel1->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    NsValueWidgetLayout *vwl(new NsValueWidgetLayout);
    vwl->addWidget(typeLabel0, 0, Qt::AlignLeft | Qt::AlignVCenter);
    vwl->addWidget(typeLabel1, 1, Qt::AlignLeft | Qt::AlignVCenter);

    vlayout->addLayout(vwl);

    // Op Family.

    NsValueLabel *familyLabel0(new NsValueLabel("Family:"));
    familyLabel0->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    NsValueLabel *familyLabel1(new NsValueLabel(op->familyName()));
    familyLabel1->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    NsValueWidgetLayout *vwl1(new NsValueWidgetLayout);

    vwl1->addWidget(familyLabel0, 0, Qt::AlignLeft | Qt::AlignVCenter);
    vwl1->addWidget(familyLabel1, 1, Qt::AlignLeft | Qt::AlignVCenter);

    vlayout->addLayout(vwl1);

    // Op Name.

    NsValueLabel *nameLabel0(new NsValueLabel("Name:"));
    nameLabel0->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    NsOpNameLineEdit *onle(new NsOpNameLineEdit(op->longName()));
    onle->setValidator(new NsValueRegExpValidator(QRegExp("^[^:\\.\\s/]+$"),
                                                  onle));
    connect(_op,  SIGNAL(nameChanged(QString, QString)),
            onle, SLOT(onOpNameChanged(QString, QString)));
    connect(onle, SIGNAL(opNameEdited(QString)),
            this, SLOT(_onOpNameEdited(QString)));

    if ("Global" == _op->typeName()) {
        onle->setDisabled(true);
    }

    NsValueWidgetLayout *vwl2(new NsValueWidgetLayout);

    vwl2->addWidget(nameLabel0, 0, Qt::AlignLeft | Qt::AlignVCenter);
    vwl2->addWidget(onle, 1);

    vlayout->addLayout(vwl2);

    setLayout(vlayout);
}

// -----------------------------------------------------------------------------

// _onOpNameEdited
// ---------------
//! [slot]

void
NsOpHeaderBox::_onOpNameEdited(const QString &newOpInstance)
{
    const QString oldOpInstance(_op->longName());
    if (oldOpInstance != newOpInstance) {
        if (0 != _undoStack) {
            NsCmdRename::exec(oldOpInstance, newOpInstance, *_undoStack);
        }
        else {
            NsCmdRename::exec(oldOpInstance, newOpInstance);
        }
    }
}

// -----------------------------------------------------------------------------
