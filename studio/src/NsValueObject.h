// -----------------------------------------------------------------------------
//
// NsValueObject.h
//
// Naiad Studio class for representing a Nb::ValueObject, header file.
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

#ifndef NS_VALUE_OBJECT_H
#define NS_VALUE_OBJECT_H

#include "NsValueObjectHandle.h"
#include "NsStringUtils.h"
#include <NiTypes.h>
#include <QPointF>
#include <QObject>
#include <QMap>
#include <QHash>
#include <QList>
#include <QString>
#include <QStringList>
#include <QDebug>

class NsValueSectionObject;
class NsValueBaseObject;

namespace Nb {
    class ValueObject;
    class ValueSection;
    class ValueBaseObject;
}

// -----------------------------------------------------------------------------

// NsValueObject
// -------------
//! Abstract base class used to represent Nb::ValueObject instances, such as
//! Ops or bodies, in Open Naiad Studio.

class NsValueObject : public QObject
{
    Q_OBJECT

public:

    static const QString&
    selectionMetaValueType()
    { return _selectionMetaValueType; }

    static const QString&
    selectedMetaValue(const bool selected)
    { return (selected ? _selectedMetaValue : _unselectedMetaValue); }

    static bool
    isSelected(const QString &metaValue)
    { return (metaValue == _selectedMetaValue); }


    static const QString&
    positionMetaValueType()
    { return _positionMetaValueType; }

    static QString
    positionString(const qreal x, const qreal y);

    static bool
    positionCoordinates(const QString &str, qreal &x, qreal &y);

private:

    static const QString _selectionMetaValueType;
    static const QString _selectedMetaValue;
    static const QString _unselectedMetaValue;

    static const QString _positionMetaValueType;
    static const QString _positionCoordinateSeparator;

public:

    virtual
    ~NsValueObject() = 0;   // Force abstract.

    virtual const Nb::ValueObject&
    constNbValueObject() const;

    virtual Nb::ValueObject&
    mutableNbValueObject();

    // virtual void
    // bindNelContext(const NtTimeBundle &tb) const = 0;

    // virtual void
    // unbindNelContext() const = 0;

    virtual bool
    isEnabled() const;

    virtual QString
    longName() const;

public:

    const NsValueObjectHandle&
    handle() const
    { return _handle; }

public:     // Sections.

    QList<const NsValueSectionObject*>
    constSections() const
    { return constPtrList(_orderedSections); }

    //! Returns a list of pointers to the sections of this value object.
    const QList<NsValueSectionObject*>&
    mutableSections() const
    { return _orderedSections; }

    //! Returns null if section not found.
    const NsValueSectionObject*
    queryConstSection(const QString &name) const
    { return _sections.value(name, 0); }

    //! Returns null if section not found.
    NsValueSectionObject*
    queryMutableSection(const QString &name)
    { return _sections.value(name, 0); }

    bool
    hasSection(const QString &name) const
    { return _sections.contains(name); }

public:     // Values.

    QList<const NsValueBaseObject*>
    constValues() const
    { return constPtrList(_orderedValues); }

    const QList<NsValueBaseObject*>&
    mutableValues()
    { return _orderedValues; }


    //! Null if not found.
    const NsValueBaseObject*
    queryConstValue(const QString &name) const
    { return _values.value(name, 0); }

    //! Null if not found.
    NsValueBaseObject*
    queryMutableValue(const QString &name)
    { return _values.value(name, 0); }

public:    // Meta.

    bool
    metaValue(const QString &valueType, QString *value) const;

    QStringList
    metaValueTypes() const;

public:     // TODO: Enum groups!

public:    // Selection.

    bool
    isSelected() const
    { return _isSelected(); }

public:    // Position.

    QPointF
    position() const;

public:
    
    //void
    //emitMetaChanged(const QString &valueType, const QString &value);

    void
    emitSelectionChanged()            
    {
        qDebug() << "NsValueObject::emitSelectionChanged";
        //_selected = _isSelected();
        emit selectionChanged(); 
    }

    void
    emitPositionChanged(const qreal x, const qreal y)
    { emit positionChanged(x, y); }

protected slots:

    void
    onMetaChanged(const QString &longName, 
                  const QString &valueType, 
                  const QString &value, 
                  bool           success);

signals:

    void
    metaChanged(const QString &valueType, const QString &value);

    void
    selectionChanged();

    void
    positionChanged(qreal x, qreal y);

    void
    valueObjectDestroyed(NsValueObject *vo);

protected:

    explicit
    NsValueObject(Nb::ValueObject &vo,
                  QObject         *parent = 0);

private:

    void
    _createSections();

    void
    _addSection(Nb::ValueSection &vs);

    bool
    _isSelected() const;

private:    // Member variables.

    Nb::ValueObject *_vo; //!< Server resource. TODO: const?

    NsValueObjectHandle _handle;

    //bool _selected;

    typedef QHash<QString,NsValueSectionObject*> _SectionHashType;
    typedef QHash<QString,NsValueBaseObject*>    _ValueBaseHashType;
    typedef QList<NsValueSectionObject*>         _SectionListType;
    typedef QList<NsValueBaseObject*>            _ValueBaseListType;

    _SectionHashType   _sections;
    _ValueBaseHashType _values;      //!< Associated values.

    _SectionListType _orderedSections;
    _ValueBaseListType _orderedValues;
};

#endif // NS_VALUE_OBJECT_H
