// -----------------------------------------------------------------------------
//
// NsParamBodyNameList.cc
//
// Naiad Studio param <string | body-name-list>, source file.
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

#include "NsParamBodyNameListWidget.h"
#include "NsParamWidgetArgs.h"
#include "NsParamWidgetFactory.h"
#include "NsOpObject.h"
#include "NsOpStore.h"
#include "NsBodyObject.h"
#include "NsBodyOutputPlugObject.h"
#include "NsValueWidgetLayout.h"
#include "NsValueLineEdit.h"
#include "NsValueRegExpValidator.h"
#include "NsQuery.h"
#include "NsStringUtils.h"

#include <NiNb.h>

// -----------------------------------------------------------------------------

//!< Key [static].
const NsValueWidgetKey 
NsParamBodyNameListWidget::key(fromNbStr(Nb::Value1s::staticTypeName()),
                               "body-name-list");

// -----------------------------------------------------------------------------

// NsParamWidgetBodyNameList
// -------------------------
//! CTOR.

NsParamBodyNameListWidget::NsParamBodyNameListWidget(
    const NsParamWidgetArgs &args)
    : NsParamBaseWidget(args)
    , _vle(new NsValueLineEdit(*this))   // Child.
{
    //initMetaMode(*_vle);
    _vle->setValidator(new NsValueRegExpValidator(QRegExp("^[^:\\./]*$"),_vle));
    _vle->onValueChanged();

    connect(this, SIGNAL(valueChanged()),
            _vle, SLOT(onValueChanged()));

    connect(_vle, SIGNAL(valueEdited(QString,int)),
            this, SLOT(onValueEdited(QString,int)));

    valueWidgetLayout()->addWidget(_vle, 1);
}


// addContextActions
// -----------------
//! DOCS

void
NsParamBodyNameListWidget::addContextActions(QMenu &menu, const int comp) const
{
    Q_UNUSED(comp);

    // Build sub-menu.

    menu.addSeparator();
    QMenu *bodyNameMenu(menu.addMenu("Body Names"));

    const QSet<QString> bodyNameSet(upstreamBodyNames());
    foreach (const QString &bodyName, bodyNameSet) {
        _addBodyNameAction(bodyName, *bodyNameMenu);
    }

    _addBodyNameAction("*", *bodyNameMenu); // All bodies.
}

// -----------------------------------------------------------------------------

// onBodyNameActionTriggered
// -------------------------
//! Set the line edit value to senders text value. [slot]

void
NsParamBodyNameListWidget::onBodyNameActionTriggered()
{
    QAction *action(qobject_cast<QAction *>(sender()));

    if (0 != action) {
        if ("*" == action->text() ||
            "*" == _vle->text()) {
            onValueEdited(action->text());   // Replace.
        }
        else {
            onValueEdited(_vle->text() + " " + action->text()); // Append.
        }
    }
}

// -----------------------------------------------------------------------------

void
NsParamBodyNameListWidget::_addBodyNameAction(const QString &text,
                                              QMenu         &menu) const
{
    QAction *action(new QAction(text, &menu)); // Child.
    connect(action,
            SIGNAL(triggered()),
            SLOT(onBodyNameActionTriggered()));
    menu.addAction(action);
}

// -----------------------------------------------------------------------------

//! Static creator instance.
const NsParamBodyNameListWidget::_Creator
    NsParamBodyNameListWidget::_creator(*NsParamWidgetFactory::instance());
