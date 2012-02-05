// -----------------------------------------------------------------------------
//
// NsProp1iWidget.cc
//
// Naiad Studio property <int>, source file.
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

#include "NsProp1iWidget.h"
#include "NsPropWidgetArgs.h"
#include "NsPropWidgetFactory.h"
#include "NsValueBaseObject.h"
#include "NsValueWidgetLayout.h"
#include "NsValueLabel.h"
#include "NsValueLineEdit.h"
#include "NsCmdCentral.h"
#include "NsCmdSetMeta.h"
#include <QAction>

// -----------------------------------------------------------------------------

//!< Key [static].
const NsValueWidgetKey 
NsProp1iWidget::key(fromNbStr(Nb::Value1i::staticTypeName()), "");

// -----------------------------------------------------------------------------

// NsProp1iWidget
// --------------
//! CTOR.

NsProp1iWidget::NsProp1iWidget(const NsPropWidgetArgs &args)
    : NsPropBaseWidget(args)
    , _vle(new NsValueLineEdit(*this))  // Child.
{
    _vle->setMode(NsValueLineEdit::evaluation);
    _vle->setReadOnly(true);
    _vle->onValueChanged();

    valueWidgetLayout()->addWidget(_vle, 1);
}

// -----------------------------------------------------------------------------

// onModeActionTriggered
// ----------------------
//! Set line edit meta mode. [slot]

void
NsProp1iWidget::onModeActionTriggered(QAction *action)
{
    _vle->setMode(action->objectName());
    _vle->setReadOnly(true);
}

// -----------------------------------------------------------------------------

//! Static creator instance.
const NsProp1iWidget::_Creator
    NsProp1iWidget::_creator(*NsPropWidgetFactory::instance());
