// -----------------------------------------------------------------------------
//
// NsValueBaseObject.h
//
// Naiad Studio class for representing a value base, header file.
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

#ifndef NS_VALUE_BASE_OBJECT_H
#define NS_VALUE_BASE_OBJECT_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QVector>

class NsValueObject;             // Parent parent type.
class NsValueSectionObject;      // Parent type.

namespace Nb {
    class ValueBase;             // Resource type.
}

// -----------------------------------------------------------------------------

// NsValueBaseObject
// -----------------
//! Represents a value base.

class NsValueBaseObject : public QObject
{
    Q_OBJECT

public:

    explicit
    NsValueBaseObject(Nb::ValueBase        &valueBase,
                      NsValueSectionObject &parent);

    //! DTOR.
    virtual
    ~NsValueBaseObject() 
    {}

    const NsValueObject&
    valueObject() const;

    //! Returns the parenting value section object.
    const NsValueSectionObject&
    valueSection() const
    { return *_vs; }

    //! Returns a reference to the value base resource.
    const Nb::ValueBase&
    nbValueBase() const
    { return *_vb; }

    QString
    expr(int component) const;

    int
    componentCount() const;

    QString
    name() const;

    QString
    longName() const;

    QString
    typeName() const;

    QString
    subTypeName() const;

    bool
    hidden() const;

    bool
    readOnly() const;

    bool 
    metaValue(const QString &valueType, QString *value) const;

public:

    void
    emitValueChanged(const QString &expr, const int component)
    { emit valueChanged(expr, component); }

    void
    emitMetaChanged(const QString &valueType, const QString &value)
    { emit metaChanged(valueType, value); }

signals:

    //! Emitted when a component expression was (successfully) changed.
    void 
    valueChanged(const QString &newExpr, int component);

    //! Emitted when a meta value was (successfully) changed.
    void 
    metaChanged(const QString &valueType, const QString &value);

private:

    //! Returns true if the provided component is in valid range for this
    //! value base object.
    bool 
    _validComponent(const int component)
    { return (0 <= component && component < componentCount()); }

private:    // Member variables.

    const NsValueSectionObject *_vs;    //!< "parent".
    Nb::ValueBase              *_vb;    //!< Server resource.
};

#endif // NS_VALUE_BASE_OBJECT_H
