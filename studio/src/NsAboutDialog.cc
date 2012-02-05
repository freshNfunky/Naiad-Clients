// -----------------------------------------------------------------------------
//
// NsAboutDialog.cc
//
// Naiad Studio about dialog, source file.
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

#include <QtGui>

#include "NsAboutDialog.h"


// NsAboutDialog
// -------------
//! Constructor, creates a new NsAboutDialog.

NsAboutDialog::NsAboutDialog(QWidget *parent)
    : QDialog(parent, Qt::FramelessWindowHint)
{
    QLabel *background = new QLabel(this);
    background->setStyleSheet(
        "padding: 0px;"
        "margin: 0px;"
        "border: 0px solid transparent;");
    QPixmap pixmap(":/images/about.png");
    background->setPixmap(pixmap);
    setFixedSize(pixmap.width(), pixmap.height());

    QLabel *version = new QLabel(this);
    version->setStyleSheet(
        "background-color: transparent;"
        "color: #000000;");
    version->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    version->setGeometry(20, pixmap.height() - 80, pixmap.width() - 20, 40);
    version->setText(QApplication::applicationName() + " " +
                     QApplication::applicationVersion());

    QLabel *copyright = new QLabel(this);
    copyright->setStyleSheet(
        "background-color: transparent;"
        "color: #000000;");
    copyright->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    copyright->setGeometry(20, pixmap.height() - 40, pixmap.width() - 20, 40);
    copyright->setText("Copyright \xa9 2009-2011 Exotic Matter AB");
}
