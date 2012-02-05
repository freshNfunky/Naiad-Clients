// -----------------------------------------------------------------------------
//
// NsPlugObject.cc
//
// Naiad Studio class for representing a Ng::Plug, header file.
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

#include "NsPlugObject.h"
#include "NsOpObject.h"
#include "NsQuery.h"
#include "NsStringUtils.h"
#include <NiNb.h>
#include <NgPlug.h>
#include <QDebug>

// -----------------------------------------------------------------------------

const QString NsPlugObject::_smackMetaValueType("Smacked");
const QString NsPlugObject::_smackedMetaValue("True");
const QString NsPlugObject::_unsmackedMetaValue("False");

// -----------------------------------------------------------------------------

// ~NsPlugObject
// -------------
//! DTOR.

NsPlugObject::~NsPlugObject()
{ qDebug() << "~NsPlugObject"; }

// -----------------------------------------------------------------------------

// name
// ----
//! Returns the plug name.

QString
NsPlugObject::name() const
{
    return _name;
    //return fromNbStr(ngPlug().name());
}


// longName
// --------
//! Returns the plug long name.

QString
NsPlugObject::longName() const
{
    //return op().longName() + ":" + name();
    //return fromNbStr(ngPlug().longname());
    return queryPlugLongName(_op->longName(), name());
}


// sigName
// -------
//! Returns the plug signature name.

QString
NsPlugObject::sigName() const
{
    return fromNbStr(_plug->sigName());
    //return queryPlugSignatureName()
}


// isGroup
// -------
//! Returns true if plug is a group-plug.

bool
NsPlugObject::isGroup() const
{
    return queryOpInstanceIsGroupPlug(_op->longName(), name());
}


// isDummy
// -------
//! Returns true if plug is on a Dummy Op (e.g. scope).

bool
NsPlugObject::isDummy() const
{
    return _plug->dummy();
}


// metaValue
// ---------
//! Returns true if the meta value type exists. Writes an existing value
//! to the provided string, if valid.

bool
NsPlugObject::metaValue(const QString &valueType, QString *value) const
{
    const Nb::Object::MetaValueMap::const_iterator iter(
        _plug->metaValueMap().find(fromQStr(valueType)));
    const bool found = (_plug->metaValueMap().end() != iter);
    if (0 != value && found) {
        *value = fromNbStr(iter->second);
    }
    return found;
}

// -----------------------------------------------------------------------------

// metaChanged
// -----------
//! Hook.

void
NsPlugObject::metaChanged(const QString &longName,
                          const QString &valueType,
                          const QString &value)
{
    if (longName == this->longName()) {
        // TODO: Check if the provided values are actually true on server!
        emit metaChanged(valueType, value);

        if (smackMetaValueType() == valueType) {
            emitSmackChanged();
        }
#if 0
    qDebug() << "NsPlugObject::metaChanged: " << valueType << "|" << value;
#endif
    }
}

// -----------------------------------------------------------------------------

// NsPlugObject
// ------------
//! CTOR.

NsPlugObject::NsPlugObject(Ng::Plug   &plug,
                           NsOpObject &parent)
    : QObject(&parent)
    , _op(&parent)
    , _plug(&plug)
    , _name(fromNbStr(plug.name()))
{}

// -----------------------------------------------------------------------------

void
NsPlugObject::groupPlugChanged()
{
    qDebug() << "NsPlugObject::groupPlugChanged";
    _plug = _op->mutableNgOp().plug(fromQStr(name())); // Update resource.
}
