// -----------------------------------------------------------------------------
//
// NsValueObject.cc
//
// Naiad Studio class for representing a Nb::ValueObject, source file.
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

#include "NsValueObject.h"
#include "NsValueSectionObject.h"
#include "NsValueBaseObject.h"
#include "NsCmdCentral.h"
#include "NsCmdSetMeta.h"
#include "NsStringUtils.h"
#include <NbObject.h>
#include <NbValueObject.h>
#include <NbValueSection.h>
#include <NgOp.h>
#include <QDebug>

// -----------------------------------------------------------------------------

const QString NsValueObject::_selectionMetaValueType("Selected"); //!< [static]
const QString NsValueObject::_selectedMetaValue("True");          //!< [static]
const QString NsValueObject::_unselectedMetaValue("False");       //!< [static]

const QString NsValueObject::_positionMetaValueType("Pos");       //!< [static]
const QString NsValueObject::_positionCoordinateSeparator(",");   //!< [static]

// -----------------------------------------------------------------------------

// positionString
// --------------
//! [static]

QString
NsValueObject::positionString(const qreal x, const qreal y)
{
    return QString(QString::number(x) +
                   _positionCoordinateSeparator +
                   QString::number(y));
}


// positionCoordinates
// -------------------
//! [static]

bool
NsValueObject::positionCoordinates(const QString &str, qreal &x, qreal &y)
{
    bool success(false);
    const int index(str.indexOf(_positionCoordinateSeparator));

    if (-1 != index) {
        // Separator found. There is at least one separator.

        bool xOk(false);
        const qreal tx(str.left(index).toDouble(&xOk));
        if (xOk) {
            bool yOk(false);
            const qreal ty(str.right(str.length() - index - 1).toDouble(&yOk));
            if (yOk) {
                x = tx;
                y = ty;
                success = true;
            }
        }
    }

    return success;
}

// -----------------------------------------------------------------------------

// ~NsValueObject
// --------------
//! DTOR.

NsValueObject::~NsValueObject()
{
    qDebug() << "~NsValueObject";
    emit valueObjectDestroyed(this);
}


// valueObject
// -----------
//! Returns a const reference to the underlying value object.

const Nb::ValueObject&
NsValueObject::constNbValueObject() const
{    
    return *_vo;
}


// valueObject
// -----------
//! Returns a const reference to the underlying value object.

Nb::ValueObject&
NsValueObject::mutableNbValueObject()
{
    return *_vo;
}


bool
NsValueObject::isEnabled() const
{
    return true;
}


//// valueObject
//// -----------
////! Returns a non-const reference to the underlying value object.

//Nb::ValueObject &
//NsValueObject::nbValueObject()
//{
//    return *_vo;
//}


// longName
// --------
//! Returns the value object's long name.

QString
NsValueObject::longName() const
{
    return fromNbStr(_vo->longname());
}

// -----------------------------------------------------------------------------

// metaValue
// ---------
//! Returns true of meta value type exists for this object.

bool
NsValueObject::metaValue(const QString &valueType, QString *value) const
{
    const Nb::Object::MetaValueMap::const_iterator iter(
        _vo->metaValueMap().find(fromQStr(valueType)));
    const bool found(iter != _vo->metaValueMap().end());
    if ((0 != value) && found) {
        *value = fromNbStr(iter->second);
    }
    return found;
}


QStringList
NsValueObject::metaValueTypes() const
{
    QStringList mvt;
    Nb::Object::MetaValueMap::const_iterator iter(_vo->metaValueMap().begin());
    Nb::Object::MetaValueMap::const_iterator iend(_vo->metaValueMap().end());
    for (; iter != iend; ++iter) {
        mvt.push_back(fromNbStr(iter->first));
    }
    return mvt;
}

// -----------------------------------------------------------------------------

// position
// --------
//! Return the position of the Op, or (0,0) if the Op has no position.

QPointF
NsValueObject::position() const
{
    qreal x(0.);
    qreal y(0.);
    QString value;
    if (metaValue(positionMetaValueType(), &value)) {
        positionCoordinates(value, x, y); // Ignore success.
    }
    return QPointF(x, y);
}

// -----------------------------------------------------------------------------

//void
//NsValueObject::emitMetaChanged(const QString &valueType, const QString &value)
//{ 
//    emit metaChanged(valueType, value); 
//
//    if (selectionMetaValueType() == valueType) {
//        _selected = isSelected(value);
//        emitSelectionChanged(_selected);
//    }
//    else if (positionMetaValueType() == valueType) {
//        qreal x = 0;
//        qreal y = 0;
//        if (positionCoordinates(value, x, y)) {
//            emitPositionChanged(x, y);
//        }
//    }
//}

// -----------------------------------------------------------------------------

void
NsValueObject::onMetaChanged(const QString &longName, 
                             const QString &valueType, 
                             const QString &value, 
                             const bool     success)
{
    if (success && (longName == this->longName())) {
        emit metaChanged(valueType, value); 

        if (selectionMetaValueType() == valueType) {
            //_selected = isSelected(value);
            emitSelectionChanged();
        }
        else if (positionMetaValueType() == valueType) {
            qreal x = 0;
            qreal y = 0;
            if (positionCoordinates(value, x, y)) {
                emitPositionChanged(x, y);
            }
        }
    }
}

// -----------------------------------------------------------------------------

// NsValueObject
// -------------
//! CTOR.

NsValueObject::NsValueObject(Nb::ValueObject           &vo,
                             QObject                   *parent)
    : QObject(parent)
    , _vo(&vo)
    //, _selected(_isSelected())
{
    _createSections();
}

// -----------------------------------------------------------------------------

// _createSections
// ---------------
//! Create objects representing the sections of this value object.

void
NsValueObject::_createSections()
{
    const int64_t count(_vo->sectionCount());

    for (int64_t s(0); s < count; ++s) {
        _addSection(*_vo->mutableSection(_vo->section(s)->name));
    }
}


// _addSection
// -----------
//! Add a section to map.

void
NsValueObject::_addSection(Nb::ValueSection& vs)
{
    // Assume value sections are unique, no checking for existing sections
    // in map.

    EM_ASSERT(!hasSection(fromNbStr(vs.name)));
    const bool isOp = (dynamic_cast<Ng::Op*>(vs.vo) != 0);

    // HACK: trigger creation of 3D section item!

    if (isOp &&
        ("Transform"         == vs.name ||
         "Initial Transform" == vs.name ||
         "Clip Box"          == vs.name ||
         "View Frustum"      == vs.name ||
         "Plane"             == vs.name ||
         "Sphere"            == vs.name ||
         "Axis"              == vs.name ||
         "Vector"            == vs.name ||
         "Import Transform"  == vs.name)) {
        // Don't show frustums by default.

        NsCmdSetMeta::exec(
            longName(),
            "Visible in 3D",
            vs.name == "View Frustum" ? "Off" : "On");
    }

    NsValueSectionObject *vso = new NsValueSectionObject(vs, *this); // Child.
    _sections.insert(vso->name(), vso);
    _orderedSections.append(vso);

    foreach (NsValueBaseObject *vbo, vso->constValues()) {
        _values.insert(vbo->name(), vbo);
        _orderedValues.append(vbo);
    }
}


// _isSelected
// -----------
//! Returns true if the Op is selected.

bool
NsValueObject::_isSelected() const
{
    bool selected(false);
    QString value;
    if (metaValue(selectionMetaValueType(), &value)) {
        selected = isSelected(value);
    }
    return selected;
}
