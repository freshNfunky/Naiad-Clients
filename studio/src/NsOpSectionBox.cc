// -----------------------------------------------------------------------------
//
// NsOpSectionBox.cc
//
// Naiad Studio Op section box source file.
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

#include "NsOpSectionBox.h"
#include "NsOpObject.h"
#include "NsOpObjectBox.h"
#include "NsValueSectionObject.h"
#include "NsValueBaseObject.h"
#include "NsParamBaseWidget.h"
#include "NsParamWidgetFactory.h"
#include "NsCmdCentral.h"
#include "NsStringUtils.h"
#include <QVBoxLayout>
#include <QDebug>

#include "NsAbstractMetaWidget.h"
#include "NsMetaCheckBox.h"
#include "NsMetaLabel.h"
#include <string>   // TMP, for child()

// -----------------------------------------------------------------------------

namespace {

    std::string
    child(const std::string &str, const char separator = ':')
    {
        const std::size_t pos(str.find_first_of(separator));

        if (pos == std::string::npos) {
            // Separator not found, return entire string.

            return str;
        }

        return str.substr(pos + 1, str.size());
    }
}

// -----------------------------------------------------------------------------

// NsOpSectionBox
// --------------
//! CTOR.

NsOpSectionBox::NsOpSectionBox(const NsValueSectionObject &vso,
                               NsUndoStack             *undoStack,
                               NsOpObjectBox              &parent)
    : NsValueSectionBox(vso, parent)
{
    QVBoxLayout *layout(new QVBoxLayout);    
    const bool isOp = (dynamic_cast<const NsOpObject*>(&vso.valueObject())!=0);

    if (isOp && 
        ("Transform"         == vso.name() ||
         "Initial Transform" == vso.name() ||
         "Clip Box"          == vso.name() ||
         "View Frustum"      == vso.name() ||
         "Plane"             == vso.name() ||
         "Sphere"            == vso.name() ||
         "Axis"              == vso.name() ||
         "Vector"            == vso.name() ||
         "Import Transform"  == vso.name())) {
        NsMetaCheckBoxWidget *mcbw(
            new NsMetaCheckBoxWidget(
                NsMetaWidgetArgs(parent.op(),
                                 0,
                                 "Visible in 3D",
                                 undoStack,
                                 this)));

        connect(NsCmdCentral::instance(),
                SIGNAL(metaChanged(QString,QString,QString,bool)),
                mcbw,
                SLOT(onMetaChanged(QString,QString,QString,bool)));

        layout->addWidget(mcbw);
    }


    NsParamBaseWidget *pbw(0);
    foreach (NsValueBaseObject *vbo, vso.constValues()) {
        if (!vbo->hidden()) {

            pbw =
                NsParamWidgetFactory::instance()->create(
                    NsParamWidgetFactory::KeyType(
                        vbo->typeName(),
                        fromNbStr(child(fromQStr(vbo->subTypeName())))),
                    NsParamWidgetFactory::ArgsType(
                        parent.op(), vbo, undoStack, this));

            if (0 != pbw) {
                // Factory produced a valid widget, add it to the layout.

                if (vbo->readOnly()) {
                    pbw->setReadOnly(true);
                }

                connect(NsCmdCentral::instance(),
                        SIGNAL(metaChanged(QString,QString,QString,bool)),
                        pbw,
                        SLOT(onMetaChanged(QString,QString,QString,bool)));

                connect(NsCmdCentral::instance(),
                        SIGNAL(valueChanged(QString,QString,int,bool)),
                        pbw,
                        SLOT(onValueChanged(QString,QString,int,bool)));

                connect(NsCmdCentral::instance(),
                        SIGNAL(currentVisibleFrameChanged(int,bool,bool)),
                        pbw,
                        SLOT(onCurrentVisibleFrameChanged(int,bool,bool)));

                layout->addWidget(pbw);
            }
            else {
                qDebug() << "Unrecognized param type: <"
                         << vbo->typeName() << " | " << vbo->subTypeName()
                         << ">";
            }
        }
    }

    setLayout(layout); // Pass ownership of layout to box.
}

// -----------------------------------------------------------------------------
