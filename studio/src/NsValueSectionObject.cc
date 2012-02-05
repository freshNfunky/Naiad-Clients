// -----------------------------------------------------------------------------
//
// NsValueSectionObject.cc
//
// Naiad Studio class for representing a value section, source file.
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

#include "NsValueSectionObject.h"
#include "NsValueObject.h"
#include "NsValueBaseObject.h"
#include "NsCmdCentral.h"
#include "NsStringUtils.h"
#include <NbValueSection.h>
#include <NbValueBase.h>

// -----------------------------------------------------------------------------

// NsValueSectionObject
// --------------------
//! CTOR.

NsValueSectionObject::NsValueSectionObject(Nb::ValueSection &valueSection,
                                           NsValueObject    &parent)
    : QObject(&parent)
    , _vo(&parent)
    , _vs(&valueSection)
{
    _createValues();
}



//// values
//// ------
////! Returns a list of pointers to the values of this section.
//
//QList<NsValueBaseObject*>
//NsValueSectionObject::values() const
//{
//    return _values.values();
//}


// name
// ----
//! Returns the value section's name.

QString
NsValueSectionObject::name() const
{
    return fromNbStr(_vs->name);
}

// -----------------------------------------------------------------------------

void
NsValueSectionObject::_createValues()
{
    const unsigned int count(_vs->valueCount());
    for (unsigned int v(0); v < count; ++v) {
        _addValue(*_vs->value(v));
    }
}


void
NsValueSectionObject::_addValue(Nb::ValueBase &vb)
{
    NsValueBaseObject *vbo = new NsValueBaseObject(vb, *this); // Child.

    //connect(NsCmdCentral::instance(),
    //        SIGNAL(valueChanged(QString,QString,int,bool)),
    //        vbo,
    //        SLOT(onValueChanged(QString,QString,int,bool)));

    //connect(NsCmdCentral::instance(),
    //        SIGNAL(metaChanged(QString,QString,QString,bool)),
    //        vbo,
    //        SLOT(onMetaChanged(QString,QString,QString,bool)));

    _values.insert(vbo->name(), vbo);
    _orderedValues.append(vbo);
    //_values.append(vbo);
}
