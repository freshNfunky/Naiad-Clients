// -----------------------------------------------------------------------------
//
// NsBodyObjectBox.cc
//
// Naiad Studio body object box, source file.
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

#include "NsBodyObjectBox.h"
#include "NsValueEditorWidget.h"
#include "NsBodyHeaderBox.h"
#include "NsBodySectionBox.h"
#include "NsValueSectionObject.h"
#include <QVBoxLayout>

// -----------------------------------------------------------------------------

// valueObject
// -----------
//! Covariant return type.

NsBodyObject*
NsBodyObjectBox::valueObject() const
{
    return _body;
}

// -----------------------------------------------------------------------------

// NsBodyObjectBox
// ---------------
//! CTOR.

NsBodyObjectBox::NsBodyObjectBox(NsBodyObject        *body,
                                 NsValueEditorWidget *parent)
    : NsValueObjectBox(body, parent)
    , _body(body)
    , _vew(parent)
{
    connect(_body, SIGNAL(nameChanged(QString)),
            this, SLOT(onNameChanged(QString)));
    onNameChanged(_body->longName());

    QVBoxLayout *layout(new QVBoxLayout);

    // Add header box.

    layout->addWidget(new NsBodyHeaderBox(_body, "Header"));

    // Add section boxes.

    foreach (const NsValueSectionObject *vso, _body->constSections()) {
        layout->addWidget(new NsBodySectionBox(*vso, *this));
    }

    setLayout(layout);
}


// body
// ----
//! DOCS

NsBodyObject*
NsBodyObjectBox::body() const
{
    return _body;
}

// -----------------------------------------------------------------------------

// onNameChanged
// -------------
//! [slot]

void
NsBodyObjectBox::onNameChanged(const QString &newName)
{
    setTitle(newName);
}
