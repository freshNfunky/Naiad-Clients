// -----------------------------------------------------------------------------
//
// NsPropBaseWidget.cc
//
// Naiad Studio property base widget, source file.
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

#include "NsPropBaseWidget.h"
#include "NsPropWidgetArgs.h"
#include "NsValueBaseObject.h"
#include "NsStringUtils.h"
#include <QtGui>

// -----------------------------------------------------------------------------

// ~NsPropBaseWidget
// -----------------
//! DTOR.

NsPropBaseWidget::~NsPropBaseWidget()
{}

// -----------------------------------------------------------------------------

// NsPropBaseWidget
// ----------------
//! CTOR.

NsPropBaseWidget::NsPropBaseWidget(const NsPropWidgetArgs &args)
    : NsValueBaseWidget(args)
    , _body(args.body())
{}


// body
// ----
//! DOCS

NsBodyObject*
NsPropBaseWidget::body() const
{
    return _body;
}


// stringEval
// ----------
//! TODO

QString
NsPropBaseWidget::stringEval(const NtTimeBundle &tb,
                             const int           comp,
                             const int           threadId) const
{
    try {
        Nb::NelContext nelContext(&body()->nbBody(),"",tb);

        const QString eval(
            fromNbStr(
                valueBaseObject()->nbValueBase().stringEval(
                    tb, comp, threadId)));

        return eval;
    }
    catch (std::exception &ex) {
        qDebug() << ex.what();   // TODO: error message!
        return QString("");
    }
}


// valueObject
// -----------
//! Covariant return type.

NsBodyObject*
NsPropBaseWidget::valueObject() const
{
    return _body;
}

// -----------------------------------------------------------------------------
