// -----------------------------------------------------------------------------
//
// NsValueSectionObject.h
//
// Naiad Studio class for representing a value section, header file.
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

#ifndef NS_VALUE_SECTION_OBJECT_H
#define NS_VALUE_SECTION_OBJECT_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QString>

class NsValueObject;        // Parent type.
class NsValueBaseObject;    // Child type.

namespace Nb {
    class ValueSection;     // Resource type.
    class ValueBase;        // Child resource type.
}

// -----------------------------------------------------------------------------

// NsValueSectionObject
// -----------------
//! Simple representation of a value section.

class NsValueSectionObject : public QObject
{
    Q_OBJECT

public:

    explicit
    NsValueSectionObject(Nb::ValueSection &valueSection,
                         NsValueObject    &parent);

    //! DTOR.
    virtual
    ~NsValueSectionObject() 
    {}  

    //! Returns the value object that the value section belongs to.
    virtual const NsValueObject&
    valueObject() const
    { return *_vo; }

    //! Returns the value section resource.
    virtual const Nb::ValueSection&
    valueSection() const
    { return *_vs; }

public:

    QString
    name() const;

public:

    // TODO: Fix const-ness
    const QList<NsValueBaseObject*>&
    constValues() const
    { return _orderedValues; }


    const NsValueBaseObject*
    queryConstValue(const QString &name) const
    { return _values.value(name, 0); }

    NsValueBaseObject*
    queryMutableValue(const QString &name)
    { return _values.value(name, 0); }

private:

    void
    _createValues();

    void
    _addValue(Nb::ValueBase &vb);

private:    // Member variables.

    const NsValueObject *_vo;    //!< "parent".
    Nb::ValueSection    *_vs;    //!< Server resource.

    typedef QHash<QString,NsValueBaseObject*> _ValueBaseHashType;
    typedef QList<NsValueBaseObject*>         _ValueBaseListType;

    _ValueBaseHashType _values;      //!< Associated values.
    _ValueBaseListType _orderedValues;
};

// -----------------------------------------------------------------------------

#endif // NS_VALUE_SECTION_OBJECT_H
