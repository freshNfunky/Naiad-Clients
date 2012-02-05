// -----------------------------------------------------------------------------
//
// NsValueBaseWidget.cc
//
// Naiad Studio value base widget, source file.
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

#include "NsValueBaseWidget.h"
#include "NsValueWidgetArgs.h"
#include "NsValueBaseObject.h"
#include "NsValueWidgetLayout.h"
#include "NsValueLabel.h"
#include "NsValueLineEdit.h"
#include "NsCmdSetMeta.h"
#include "NsQuery.h"
#include <NbValue.h>
#include <QMenu>

// -----------------------------------------------------------------------------

// ~NsValueBaseWidget
// ------------------
//! DTOR.

NsValueBaseWidget::~NsValueBaseWidget()
{}


// valueObject
// -----------
//! DOCS

NsValueObject*
NsValueBaseWidget::valueObject() const
{
    return _vo;
}


// addContextActions
// -----------------
//! By default do nothing.

void
NsValueBaseWidget::addContextActions(QMenu &menu, const int comp) const
{}

// -----------------------------------------------------------------------------

// NsValueBaseWidget
// -----------------
//! CTOR.

NsValueBaseWidget::NsValueBaseWidget(const NsValueWidgetArgs &args)
    : QWidget(args.parent())
    , _vo(args.valueObject())
    , _vbo(args.valueBaseObject())
    , _vwl(new NsValueWidgetLayout)
{
    setToolTip(
        QString(_vbo->name() + " <" +
                _vbo->typeName() + " | " +
                _vbo->subTypeName()
                + ">"));

    //_vbo->nbValueBase().

    NsValueLabel *vl(new NsValueLabel(_vbo->name(), this)); // Child.
    _vwl->addWidget(vl, 0, Qt::AlignRight);

    setLayout(_vwl);
}


// valueBaseObject
// ---------------
//! Returns parameter info object.

NsValueBaseObject*
NsValueBaseWidget::valueBaseObject() const
{
    return _vbo;
}

// -----------------------------------------------------------------------------
