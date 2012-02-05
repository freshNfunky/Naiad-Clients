// -----------------------------------------------------------------------------
//
// NsBodySectionBox.cc
//
// Naiad Studio body section box, source file.
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

#include "NsBodySectionBox.h"
#include "NsBodyObjectBox.h"
#include "NsValueSectionObject.h"
#include "NsValueBaseObject.h"
#include "NsPropBaseWidget.h"
#include "NsPropWidgetFactory.h"
#include <QVBoxLayout>
#include <QDebug>

// -----------------------------------------------------------------------------

// NsBodySectionBox
// ----------------
//! CTOR.

NsBodySectionBox::NsBodySectionBox(const NsValueSectionObject &vso,
                                   NsBodyObjectBox            &parent)
    : NsValueSectionBox(vso, parent)
{
    QVBoxLayout *layout(new QVBoxLayout);

    NsPropBaseWidget *pbw(0);
    foreach (NsValueBaseObject *vbo, vso.constValues()) {
        if (!vbo->hidden()) {
            // Create a property widget using a factory.

            pbw =
                NsPropWidgetFactory::instance()->create(
                    NsPropWidgetFactory::KeyType(vbo->typeName(),
                                                 vbo->subTypeName()),
                    NsPropWidgetFactory::ArgsType(parent.body(), vbo, this));

            if (0 != pbw) {
                // Valid property widget.

                layout->addWidget(pbw);
            }
            else {
                qDebug() << "Unrecognized property type: <"
                         << vbo->typeName() << "|" << vbo->subTypeName() << ">";
            }
        }
    }

    setLayout(layout); // Pass ownership of layout to box.
}

// -----------------------------------------------------------------------------
