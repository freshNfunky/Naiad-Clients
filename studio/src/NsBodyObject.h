// -----------------------------------------------------------------------------
//
// NsBodyObject.h
//
// Naiad Studio class for representing a body, header file.
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

#ifndef NS_BODY_OBJECT_H
#define NS_BODY_OBJECT_H

#include "NsValueObject.h"
#include "Ns3DBody.h"
#include <NbBody.h>   // Full include required for covariant return types.
#include <NbNelContext.h>
#include <QString>
#include <QStringList>

class NsOpObject;
class NsBodyOutputPlugObject;

namespace Nb {
    class Shape;
    class Channel;
}

// -----------------------------------------------------------------------------

// NsBodyObject
// ------------
//! TODO

class NsBodyObject : public NsValueObject
{
    Q_OBJECT

public: // NsValueObject interface.

    virtual
    ~NsBodyObject();

    virtual const Nb::Body&
    constNbValueObject() const // Covariant return type.
    { return *_body; }

    virtual Nb::Body&
    mutableNbValueObject()
    { return *_body; }

//    virtual void
//   bindNelContext(const NtTimeBundle &tb) const;

    // virtual void
    // unbindNelContext() const;

    virtual QString
    longName() const;

public:

    explicit
    NsBodyObject(Nb::Body                  &body,
                 NsOpObject                &op,
                 NsBodyOutputPlugObject    *bopo,
                 //bool                       cache,
                 bool                       live,
                 QObject                   *parent = 0);

    const Nb::Body&
    nbBody() const;

    Nb::Body&
    nbBody();

    Ns3DBody*
    ns3DBody()
    { return _ns3DBody; }


    QString
    name() const;

    QString
    sigName() const;

    bool
    isEvolving() const;

    bool
    isLive() const;

    void
    eraseShapes();

    void
    init3DBody();

public: // Shapes.

    class ShapeInfo
    {
    public:

        class ChannelInfo
        {
        public:

            explicit
            ChannelInfo(const Nb::Channel &channel,
                        bool               live);

            const QString&
            name() const
            { return _name; }

            const QString&
            typeName() const
            { return _typeName; }

            const QString&
            size() const
            { return _size; }

            const QString&
            minValue() const
            { return _minValue; }

            const QString&
            maxValue() const
            { return _maxValue; }

            const QString&
            avgValue() const
            { return _avgValue; }

        private:    // Member variables.

            QString _name;
            QString _typeName;
            QString _size;      //!< Count.
            QString _minValue;
            QString _maxValue;
            QString _avgValue;
        };

    public:

        explicit
        ShapeInfo(const Nb::Shape &shape,
                  const QString   &name,
                  const bool       live);

        const QString&
        name() const
        { return _name; }

        const QList<ChannelInfo>&
        channels() const
        { return _channels; }

    private:    // Member variables.

        QString            _name;
        QList<ChannelInfo> _channels;
    };

    const QList<ShapeInfo>&
    shapes() const
    { return _shapes; }

signals:

    void
    nameChanged(const QString &newName);

    void
    bodyObjectDestroyed(NsBodyObject *bo);    

protected slots:

    //void
    //onBodySelectionChanged(const QString &bodyLongName,
    //                       bool           selected,
    //                       bool           success);

    void
    onOpNameChanged(const QString &oldOpInstance,
                    const QString &newOpInstance);

private:    // Member variables.

    Nb::Body               *_body;  //!< Resource object.
    NsOpObject             *_op;
    NsBodyOutputPlugObject *_bopo;
    Ns3DBody               *_ns3DBody;

    bool _live;
    QList<ShapeInfo> _shapes;
};

#endif // NS_BODY_OBJECT_H
