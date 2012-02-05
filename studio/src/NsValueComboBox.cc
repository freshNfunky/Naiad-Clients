// -----------------------------------------------------------------------------
//
// NsValueComboBox.cc
//
// Naiad Studio param combo box, source file.
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

#include "NsValueComboBox.h"
#include "NsValueBaseWidget.h"
#include "NsQuery.h"
#include <QtGui>

// -----------------------------------------------------------------------------

// NsValueComboBox
// ---------------
//! CTOR.

NsValueComboBox::NsValueComboBox(NsValueBaseWidget &parent,
                                 const QStringList &texts,
                                 const int          comp)
    : QComboBox(&parent)
    , _vbw(&parent)
    , _comp(comp)
{
    addItems(texts);
    _init();
}


// NsValueComboBox
// ---------------
//! CTOR.

NsValueComboBox::NsValueComboBox(NsValueBaseWidget &parent,
                                 const int          comp)
    : QComboBox(&parent)
    , _vbw(&parent)
    , _comp(comp)
{
    // NB: No items added!

    _init();
}


// parentValueWidget
// -----------------
//! DOCS

const NsValueBaseWidget &
NsValueComboBox::parentValueWidget() const
{
    return *_vbw;
}

// -----------------------------------------------------------------------------

// onValueChanged
// --------------
//! NB: Combo box value is always based on evaluated expression! [slot]

void
NsValueComboBox::onValueChanged()
{
    int cvf(0);
    queryCurrentVisibleFrame(&cvf); // Ignore success.
    _setValue(_vbw->stringEval(queryFrameTimeBundle(cvf), comp()));
}

// -----------------------------------------------------------------------------

// contextMenuEvent
// ----------------
//! DOCS

void
NsValueComboBox::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;

    // Let parent widget add stuff to the menu.

    parentValueWidget().addContextActions(menu, comp());

    if (!menu.isEmpty()) {
        menu.exec(event->globalPos());
    }
}

// -----------------------------------------------------------------------------

// _onCurrentIndexChanged
// ----------------------
//! [slot]

void
NsValueComboBox::_onCurrentIndexChanged(const QString &text)
{
    emit valueEdited(text, comp());
}

// -----------------------------------------------------------------------------

// _setValue
// ---------
//! Set the state of the combo box from a string.

void
NsValueComboBox::_setValue(const QString &value)
{
    if (currentText() != value) {
        const int idx(findText(value)); // TODO: Check (idx == -1)?

        if (idx != -1) {
            setCurrentIndex(idx);
        }
        else {
            setCurrentIndex(0);
        }
    }
}


// _init
// -----
//! DOCS

void
NsValueComboBox::_init()
{
    connect(this, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(_onCurrentIndexChanged(QString)));
}
