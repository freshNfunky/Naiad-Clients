// -----------------------------------------------------------------------------
//
// NsValueBaseObject.cc
//
// Naiad Studio class for representing a value base, source file.
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

#include "NsValueBaseObject.h"
#include "NsValueObject.h"
#include "NsValueSectionObject.h"
#include "NsCmdCentral.h"
#include "NsStringUtils.h"
#include <NbObject.h>
#include <NbValueBase.h>

// -----------------------------------------------------------------------------

// NsValueBaseObject
// -----------------
//! CTOR.

NsValueBaseObject::NsValueBaseObject(Nb::ValueBase        &valueBase,
                                     NsValueSectionObject &parent)
    : QObject(&parent)
    , _vs(&parent)
    , _vb(&valueBase)
{
//    _sync();

    //_createMetaValues();
}


// valueObject
// ------------
//! Returns the parenting value section object's parent.

const NsValueObject&
NsValueBaseObject::valueObject() const
{
    return valueSection().valueObject();
}


// expr
// ----
//! Return a component expression.

QString
NsValueBaseObject::expr(const int component) const
{
    return fromNbStr(_vb->expr(component));
}


// componentCount
//! Return number of components.

int
NsValueBaseObject::componentCount() const
{
    return _vb->componentCount();
}


// name
// ----
//! Returns the name of the value base object.

QString
NsValueBaseObject::name() const
{
    return fromNbStr(_vb->name());
}


// longName
// --------
//! Returns the long name of the value base object.

QString
NsValueBaseObject::longName() const
{
    return fromNbStr(_vb->longname());
}


// typeName
// --------
//! Returns the value's type.

QString
NsValueBaseObject::typeName() const
{
    return fromNbStr(_vb->typeName());
}


// subTypeName
// -----------
//! Returns the value's sub-type.

QString
NsValueBaseObject::subTypeName() const
{
    return fromNbStr(_vb->subTypeName());
}


// hidden
// ------
//! Returns true if the value is hidden.

bool
NsValueBaseObject::hidden() const
{
    return _vb->hidden();
}


// readOnly
// --------
//! Returns true if the value is read-only.

bool
NsValueBaseObject::readOnly() const
{
    return _vb->readOnly();
}

// -----------------------------------------------------------------------------

bool
NsValueBaseObject::metaValue(const QString &valueType, QString *value) const
{
    const Nb::Object::MetaValueMap::const_iterator iter(
        nbValueBase().metaValueMap().find(
            fromQStr(valueType)));

    const bool found(iter != nbValueBase().metaValueMap().end());

    if ((0 != value) && found) {
        *value = fromNbStr(iter->second);
    }

    return found;
}

// -----------------------------------------------------------------------------
