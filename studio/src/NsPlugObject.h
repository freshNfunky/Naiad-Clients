// -----------------------------------------------------------------------------
//
// NsPlugObject.h
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

#ifndef NS_PLUG_OBJECT_H
#define NS_PLUG_OBJECT_H

#include <NiTypes.h>
#include <QObject>
#include <QList>
#include <QString>

class NsOpObject;               // Parent type.
class NsBodyObject;
namespace Ng { class Plug; }    // Resource type.

// -----------------------------------------------------------------------------

// NsPlugObject
// ------------
//! Abstract base class for objects representing plugs.

class NsPlugObject : public QObject
{
    Q_OBJECT

public:

    static const QString&
    smackMetaValueType()
    { return _smackMetaValueType; }

    static const QString&
    smackedMetaValue(const bool smacked)
    { return (smacked ? _smackedMetaValue : _unsmackedMetaValue); }

    static bool
    isSmacked(const QString &metaValue)
    { return (metaValue == _smackedMetaValue); }

private:

    static const QString _smackMetaValueType;
    static const QString _smackedMetaValue;
    static const QString _unsmackedMetaValue;

public:     // Interface.

    virtual
    ~NsPlugObject() = 0;    // Force abstract.

    virtual const Ng::Plug*
    constNgPlug() const
    { return _plug; }

    virtual Ng::Plug*
    mutableNgPlug() 
    { return _plug; }

    //! By default returns an empty list.
    virtual QList<const NsBodyObject*>
    constLiveBodies() const
    { return QList<const NsBodyObject*>(); }

    //! By default returns an empty list.
    virtual QList<NsBodyObject*>
    mutableLiveBodies()
    { return QList<NsBodyObject*>(); }

public:     // Op.

    const NsOpObject*
    constOp() const
    { return _op; }

    NsOpObject*
    mutableOp()
    { return _op; }

public:     // Plug attributes.

    QString
    name() const;

    QString
    longName() const;

    QString
    sigName() const;

    bool
    isGroup() const;

    bool
    isDummy() const;

    bool
    isSmacked() const
    {
        QString value;
        metaValue(smackMetaValueType(), &value);    // Ignore success.
        return isSmacked(value);
    }

    bool
    metaValue(const QString &valueType, QString *value) const;

public:

    void
    emitGroupChanged(/*const bool group*/)
    {
        groupPlugChanged(); // Call hook.
        emit groupChanged(/*group*/);
    }

    void
    emitSmackChanged()
    { emit smackChanged(); }

protected slots:

    void
    onMetaChanged(const QString &longName,
                  const QString &valueType,
                  const QString &value,
                  const bool     success)
    {
        if (success) {
            metaChanged(longName, valueType, value); // Call hook.
        }
    }

protected:  // Hooks.

    virtual void
    metaChanged(const QString &longName,
                const QString &valueType,
                const QString &value);

    virtual void
    groupPlugChanged() = 0;

signals:

    void
    groupChanged(/*bool group*/) const;

    void
    liveBodyCacheChanged();

    void
    metaChanged(const QString &valueType, const QString &value) const;

    void
    smackChanged();

protected:

    explicit
    NsPlugObject(Ng::Plug   &plug,   // TODO: const.
                 NsOpObject &parent);

private:    // Member variables.

    NsOpObject *_op;      //!< "parent".
    Ng::Plug   *_plug;    //!< Server resource. TODO: const?
    QString     _name;
};

// -----------------------------------------------------------------------------

#endif // NS_PLUG_OBJECT_H
