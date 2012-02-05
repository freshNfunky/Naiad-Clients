// -----------------------------------------------------------------------------
//
// NsParamEnumWidget.cc
//
// Naiad Studio param <enum> widget, source file.
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

#include "NsParamEnumWidget.h"
#include "NsParamWidgetArgs.h"
#include "NsParamWidgetFactory.h"
#include "NsValueWidgetLayout.h"
#include "NsValueLabel.h"
#include "NsValueComboBox.h"
#include "NsOpObject.h"
#include "NsValueBaseObject.h"
#include "NsQuery.h"

#include <NiNb.h>

// -----------------------------------------------------------------------------

//!< Key [static].
const NsValueWidgetKey 
NsParamEnumWidget::key(fromNbStr(Nb::Value1e::staticTypeName()), "");

// -----------------------------------------------------------------------------

// NsParamEnumWidget
// -----------------
//! CTOR.

NsParamEnumWidget::NsParamEnumWidget(const NsParamWidgetArgs &args)
    : NsParamBaseWidget(args)
{
    NsOpObject *o(op());
    NsValueBaseObject *vbo(valueBaseObject());

    if (0 != o && 0 != vbo) {
        NsValueComboBox *vcb(
            new NsValueComboBox(
                *this,
                queryParamEnumItemNames(
                    "INSTANCE",
                    o->longName(),
                    queryParamEnumGroupName(
                        "INSTANCE",
                        o->longName(),
                        vbo->name())))); // Child.

        connect(this, SIGNAL(valueChanged()),
                vcb, SLOT(onValueChanged()));

        connect(vcb, SIGNAL(valueEdited(QString,int)),
                this, SLOT(onValueEdited(QString,int)));

        vcb->onValueChanged();
        valueWidgetLayout()->addWidget(vcb, 1);
    }
}

// -----------------------------------------------------------------------------

//! Static creator instance.
const NsParamEnumWidget::_Creator
    NsParamEnumWidget::_creator(*NsParamWidgetFactory::instance());
