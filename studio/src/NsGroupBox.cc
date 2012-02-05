// -----------------------------------------------------------------------------
//
// NsGroupBox.cc
//
// Naiad Studio group box source file.
//
// Copyright (c) 2011 Exotic Matter AB.  All rights reserved.
//
// This file is part of Open Naiad Studio..
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
// POSSIBILITY OF SUCH DAMAGE.//
// -----------------------------------------------------------------------------

#include "NsGroupBox.h"
#include <QtGui>


// ~NsGroupBox
// -----------
//! DTOR.

NsGroupBox::~NsGroupBox()
{}


// NsGroupBox
// ----------
//! CTOR.

NsGroupBox::NsGroupBox(QWidget *parent)
    : QGroupBox(parent)
{
    _init();
}


// NsGroupBox
// ----------
//! CTOR.

NsGroupBox::NsGroupBox(const QString &title, QWidget *parent)
    : QGroupBox(title, parent)
{
    _init();
}


// _showContents
// -------------
//! Shows or hides the contents of the box.

void NsGroupBox::_showContents(const bool show)
{
    foreach(QObject *object, children()) {
        QWidget *widget(qobject_cast<QWidget *>(object));
        if (0 != widget) {
            widget->setVisible(show);
        }
    }
}



// _init
// -----
//! By default, the group box is checked, i.e. not collapsed.

void
NsGroupBox::_init()
{
    setCheckable(true);
    setChecked(true);

    connect(this, SIGNAL(toggled(bool)), SLOT(_showContents(bool)));
}
