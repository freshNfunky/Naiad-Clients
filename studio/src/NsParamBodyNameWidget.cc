// -----------------------------------------------------------------------------
//
// NsParamBodyNameWidget.cc
//
// Naiad Studio param <string | body-name>, source file.
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

#include "NsParamBodyNameWidget.h"
#include "NsParamWidgetArgs.h"
#include "NsParamWidgetFactory.h"
#include "NsOpObject.h"
#include "NsOpStore.h"
#include "NsBodyObject.h"
#include "NsBodyOutputPlugObject.h"
#include "NsValueBaseObject.h"
#include "NsValueWidgetLayout.h"
#include "NsValueLineEdit.h"
#include "NsValueRegExpValidator.h"
#include "NsQuery.h"
#include "NsStringUtils.h"

#include <NiNb.h>

// -----------------------------------------------------------------------------

//! Key [static].
const NsValueWidgetKey 
NsParamBodyNameWidget::key(fromNbStr(Nb::Value1s::staticTypeName()), 
                           "body-name");

// -----------------------------------------------------------------------------

// NsParamBodyNameWidget
// ---------------------
//! CTOR.

NsParamBodyNameWidget::NsParamBodyNameWidget(const NsParamWidgetArgs &args)
    : NsParamBaseWidget(args)
    , _vle(new NsValueLineEdit(*this))  // Child.
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
NsParamBodyNameWidget::addContextActions(QMenu &menu, const int comp) const
{
    Q_UNUSED(comp);

    // Build sub-menu.

    menu.addSeparator();
    QMenu *bodyNameMenu(menu.addMenu("Body Names"));

    const QSet<QString> bodyNameSet(upstreamBodyNames());
    foreach (const QString &bodyName, bodyNameSet) {
        QAction *action(new QAction(bodyName, bodyNameMenu)); // Child.
        connect(action,
                SIGNAL(triggered()),
                SLOT(onBodyNameActionTriggered()));
        bodyNameMenu->addAction(action);
    }
}

// -----------------------------------------------------------------------------

// onBodyNameActionTriggered
// -------------------------
//! Set the line edit value to senders text value. [slot]

void
NsParamBodyNameWidget::onBodyNameActionTriggered()
{
    QAction *action(qobject_cast<QAction *>(sender()));

    if (0 != action) {
        onValueEdited(action->text());  // Replace.
    }
}

// -----------------------------------------------------------------------------

//! Static creator instance.
const NsParamBodyNameWidget::_Creator
    NsParamBodyNameWidget::_creator(*NsParamWidgetFactory::instance());
