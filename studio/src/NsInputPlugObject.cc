// -----------------------------------------------------------------------------
//
// NsInputPlugObject.cc
//
// Naiad Studio class for representing a Ng::Input, source file.
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

#include "NsInputPlugObject.h"
#include "NsOpObject.h"
#include "NsStringUtils.h"
#include <QDebug>

// -----------------------------------------------------------------------------

const QString NsInputPlugObject::_selectionMetaValueType("Selected");
const QString NsInputPlugObject::_selectedMetaValue("True");
const QString NsInputPlugObject::_unselectedMetaValue("False");

// -----------------------------------------------------------------------------

// NsInputPlugObject
// -----------------
//! DTOR.

NsInputPlugObject::~NsInputPlugObject()
{ qDebug() << "~NsInputPlugObject"; }

// -----------------------------------------------------------------------------

Ng::Plug*
NsInputPlugObject::source() const
{
    return _input->source();
}


// isSelected
// ----------
//! DOCS

bool
NsInputPlugObject::isSelected() const
{
    QString value;
    metaValue(selectionMetaValueType(), &value);    // Ignore success.
    return isSelected(value);
}

// -----------------------------------------------------------------------------

//void
//NsInputPlugObject::emitMetaChanged(const QString &valueType, 
//                                   const QString &value) const
//{
//    // TODO: Check if the provided values are actually true on server!
//    emit metaChanged(valueType, value); 
//
//    if (selectionMetaValueType() == valueType) {
//        emitSelectionChanged(isSelected(value));
//    }
//    else if (modeMetaValueType() == valueType) {
//        emitModeChanged(metaValueMode(value));
//    }
//}


//void
//NsInputPlugObject::emitModeChanged(const NsInputPlugObject::Mode mode) const
//{
//    //EM_ASSERT(mode == this->mode());
//    emit modeChanged(mode);
//}

// -----------------------------------------------------------------------------

// metaChanged
// -----------
//! Hook.

void
NsInputPlugObject::metaChanged(const QString &longName,
                               const QString &valueType,
                               const QString &value)
{
    NsPlugObject::metaChanged(longName, valueType, value); // Parent method.
    if (longName == this->longName()) {
        // TODO: Check if the provided values are actually true on server!
        if (selectionMetaValueType() == valueType) {
            emitSelectionChanged();
        }
    }
}


//void
//NsInputPlugObject::onMetaChanged(const QString &longName,
//                                 const QString &valueType,
//                                 const QString &value,
//                                 const bool     success)
//{
//    if (success && (longName == this->longName())) {
//        // TODO: Check if the provided values are actually true on server!
//        emit metaChanged(valueType, value);

//        if (selectionMetaValueType() == valueType) {
//            emitSelectionChanged();
//        }
////        else if (modeMetaValueType() == valueType) {
////            emitModeChanged(metaValueMode(value));
////        }
//    }
//}

// -----------------------------------------------------------------------------

// NsInputPlugObject
// -----------------
//! CTOR.

NsInputPlugObject::NsInputPlugObject(Ng::Input  &input,
                                     NsOpObject &parent)
    : NsPlugObject(input, parent)
    , _input(&input)
{}

// -----------------------------------------------------------------------------

void
NsInputPlugObject::groupPlugChanged()
{
    qDebug() << "NsInputPlugObject::groupPlugChanged";
    NsPlugObject::groupPlugChanged();   // Parent method.
    _input = mutableOp()->mutableNgOp().input(fromQStr(name())); // Update rc.
}
