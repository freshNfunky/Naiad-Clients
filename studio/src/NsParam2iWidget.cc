// -----------------------------------------------------------------------------
//
// NsParam2iWidget.cc
//
// Naiad Studio param <vec2i>, source file.
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

#include "NsParam2iWidget.h"
#include "NsParamWidgetArgs.h"
#include "NsParamWidgetFactory.h"
#include "NsValueWidgetLayout.h"
#include "NsValueLineEdit.h"

#include <NbValue.h>

// -----------------------------------------------------------------------------

//!< Key [static].
const NsValueWidgetKey 
NsParam2iWidget::key(fromNbStr(Nb::Value2i::staticTypeName()), "");

// -----------------------------------------------------------------------------

// NsParam2iWidget
// ---------------
//! CTOR.

NsParam2iWidget::NsParam2iWidget(const NsParamWidgetArgs &args)
    : NsParamBaseWidget(args)
{
    NsValueLineEdit *vle0(new NsValueLineEdit(*this));   // Child.
    NsValueLineEdit *vle1(new NsValueLineEdit(*this));   // Child.
    ////initMetaMode(*vle0, *vle1);
    vle0->onValueChanged();
    vle1->onValueChanged();

    connect(this, SIGNAL(valueChanged()),
            vle0, SLOT(onValueChanged()));
    connect(this, SIGNAL(valueChanged()),
            vle1, SLOT(onValueChanged()));

    connect(vle0, SIGNAL(valueEdited(QString,int)),
            this, SLOT(onValueEdited(QString,int)));
    connect(vle1, SIGNAL(valueEdited(QString,int)),
            this, SLOT(onValueEdited(QString,int)));

    valueWidgetLayout()->addWidget(vle0, 1);
    valueWidgetLayout()->addWidget(vle1, 1);
}

// -----------------------------------------------------------------------------

//! Static creator instance.
const NsParam2iWidget::_Creator
    NsParam2iWidget::_creator(*NsParamWidgetFactory::instance());
