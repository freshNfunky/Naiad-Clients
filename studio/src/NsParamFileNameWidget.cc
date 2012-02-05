// -----------------------------------------------------------------------------
//
// NsParamFileNameWidget.cc
//
// Naiad Studio file name param, source file.
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

#include "NsParamFileNameWidget.h"
#include "NsParamWidgetArgs.h"
#include "NsParamWidgetFactory.h"
#include "NsValueBaseObject.h"
#include "NsValueWidgetLayout.h"
#include "NsValueLineEdit.h"
#include "NsValueButton.h"
#include "NsQuery.h"
#include "NsPath.h"

#include <NiNb.h>

// -----------------------------------------------------------------------------

//! Key [static].
const NsValueWidgetKey 
NsParamFileNameWidget::key(fromNbStr(Nb::Value1s::staticTypeName()), 
                           "file-name");

// -----------------------------------------------------------------------------

// NsParamFileNameWidget
// ---------------------
//! CTOR.

NsParamFileNameWidget::NsParamFileNameWidget(const NsParamWidgetArgs &args)
    : NsParamBaseWidget(args)
    , _vle(new NsValueLineEdit(*this))                        // Child.
    , _vb(new NsValueButton(*this))                           // Child.
{
    // TODO: validator?

    //initMetaMode(*_vle);
    _vle->onValueChanged();

    connect(this, SIGNAL(valueChanged()),
            _vle, SLOT(onValueChanged()));

    connect(_vle, SIGNAL(valueEdited(QString,int)),
            this, SLOT(onValueEdited(QString,int)));

    valueWidgetLayout()->addWidget(_vle, 1);

    _vb->setIcon(QIcon(":/images/folder-16.png"));
    _vb->setIconSize(QSize(16, 16));
    _vb->setToolTip(tr("Browse..."));
    connect(_vb, SIGNAL(clicked()), SLOT(onFileButtonClicked()));
    valueWidgetLayout()->addWidget(_vb);
}

// -----------------------------------------------------------------------------

// onFileButtonClicked
// -------------------
//! Opens a file dialog in which the user can select a file name for the given
//! parameter, and returns the selected file name, normalized wrt the project
//! path.

void
NsParamFileNameWidget::onFileButtonClicked()
{
    const QString empFilter("EMP Files (*.emp)");
    const QString allFilter("All Files (*.*)");
    QString filter(allFilter + ";;" + empFilter);

    if (valueBaseObject()->name().startsWith("EMP")) {
        filter = empFilter + ";;" + allFilter;
    }

    const QString projectPath(evalParam1s("Global.Project Path"));

    const QString fileName( 
        NsPath::getFileName(
            0, 
            tr("Set ") + valueBaseObject()->name(),
            NsPath::makeAbsolute(
                stringEval(queryCurrentVisibleFrameTimeBundle(), 0),
                projectPath),
            filter));

    if (!fileName.isEmpty()) {
        onValueEdited(NsPath::makeRelative(fileName, projectPath));
    }
}

// -----------------------------------------------------------------------------

//! Static creator instance.
const NsParamFileNameWidget::_Creator
    NsParamFileNameWidget::_creator(*NsParamWidgetFactory::instance());

// -----------------------------------------------------------------------------
