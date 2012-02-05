// -----------------------------------------------------------------------------
//
// NsBodyObject.cc
//
// Naiad Studio class for representing a body, source file.
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

#include "NsBodyObject.h"
#include "NsOpObject.h"
#include "NsBodyOutputPlugObject.h"
#include "NsQuery.h"
#include "NsStringUtils.h"
#include <NbShape.h>
#include <NbChannel.h>

// -----------------------------------------------------------------------------

// ~NsBodyObject
// -------------
//! DTOR.

NsBodyObject::~NsBodyObject()
{
    emit bodyObjectDestroyed(this);

    qDebug() << "~NsBodyObject :" << fromNbStr(_body->longname());
    delete _body;
    delete _ns3DBody;
}

// -----------------------------------------------------------------------------

// void
// NsBodyObject::bindNelContext(const NtTimeBundle &tb) const
// {
//     nbBody().bindNelContext(&_nelContext, tb);
// }

// void
// NsBodyObject::unbindNelContext() const
// {
//     nbBody().unbindNelContext();
// }

// -----------------------------------------------------------------------------

QString
NsBodyObject::longName() const
{
    return queryBodyLongName(_op->longName(),
                             (0 != _bopo) ? _bopo->longName() : QString(),
                             name());

    //if (0 != _bopo) {
    //    return _bopo->longName() + ":" + name();
    //}
    //else {
    //    return _op->longName() + "::" + name();
    //}
}

// -----------------------------------------------------------------------------

// NsBodyObject
// ------------
//! CTOR.

NsBodyObject::NsBodyObject(Nb::Body                  &body,
                           NsOpObject                &op,
                           NsBodyOutputPlugObject    *bopo,
                           //const bool                 cache,
                           const bool                 live,
                           QObject                   *parent)
    : NsValueObject(body, parent)
    , _body(&body)
    , _op(&op)
    , _bopo(bopo)
    , _ns3DBody(0)  // Null.
    , _live(live)
{

    for (Nb::Body::ConstShapeMapIter shit(body.beginShapes());
         shit != body.endShapes(); 
         ++shit) {
        // Create info for each shape and then destroy the shape
        // if it is not needed for further queries.

        _shapes.append(ShapeInfo(*shit->second(),fromNbStr(shit->first),live));
    }
}


// nbBody
// ------
//! ...

const Nb::Body&
NsBodyObject::nbBody() const
{
    return *_body;
}


Nb::Body&
NsBodyObject::nbBody()
{
    return *_body;
}


// name
// ----
//! Returns the body name.

QString
NsBodyObject::name() const
{
    return fromNbStr(_body->name());
}


// sigName
// -------
//! Returns the body signature name.

QString
NsBodyObject::sigName() const
{
    return fromNbStr(_body->sig());
}


// isEvolving
// ----------
//! Returns true if dynamics computations are performed on the body.

bool
NsBodyObject::isEvolving() const
{
    return _body->evolving();
}


// isLive
// ------
//! Returns true if body is provided from the current server frame.

bool
NsBodyObject::isLive() const
{
    return _live;
}

void
NsBodyObject::eraseShapes()
{
    std::vector<Nb::String> shapeNames;

    for (Nb::Body::ConstShapeMapIter shit(_body->beginShapes());
         shit != _body->endShapes(); 
         ++shit) {
        // Erasing while iterating over the shape map doesn't seem to work
        // properly. Therefore, collect names of all shapes in a first pass.

        shapeNames.push_back(shit->first);
    }

    for (std::vector<Nb::String>::const_iterator iter(shapeNames.begin());
         iter != shapeNames.end();
         ++iter) {
        _body->eraseShape(*iter);
    }
}

void
NsBodyObject::init3DBody() 
{
    _ns3DBody = new Ns3DBody(_body); // This object owns this memory.
}

// -----------------------------------------------------------------------------

// ChannelInfo
// -----------
//! CTOR.

NsBodyObject::ShapeInfo::ChannelInfo::ChannelInfo(const Nb::Channel &channel,
                                                  const bool         live)
    : _name(fromNbStr(channel.name()))
    , _typeName(fromNbStr(channel.typeName()))
{
    // Size.

    std::stringstream ss;
    ss.precision(3);
    ss << (live ? channel.size() : channel.cachedSize());
    _size = fromNbStr(ss.str());

    // Min.
    
    Nb::Vec3f v;
    v = (live ? channel.min() : channel.cachedMin());
    ss.str(""); // Clear.
    ss << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
    _minValue = fromNbStr(ss.str());

    // Max.

    v = (live ? channel.max() : channel.cachedMax());
    ss.str(""); // Clear.
    ss << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
    _maxValue = fromNbStr(ss.str());

    // Avg.

    v = (live ? channel.avg() : channel.cachedAvg());
    ss.str(""); // Clear.
    ss << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
    _avgValue = fromNbStr(ss.str());
}

// -----------------------------------------------------------------------------

// ShapeInfo
// ---------
//! CTOR.

NsBodyObject::ShapeInfo::ShapeInfo(const Nb::Shape &shape,
                                   const QString   &name,
                                   const bool       live)
    : _name(name)
{
    for(int i(0); i < shape.channelCount(); ++i) {
        _channels.append(ChannelInfo(*shape.channel(i)(), live));
    }
}

// -----------------------------------------------------------------------------

//// onBodySelectionChanged
//// ----------------------
////! [slot]
//
//void
//NsBodyObject::onBodySelectionChanged(const QString &bodyLongName,
//                                     const bool     selected,
//                                     const bool     success)
//{
//    if (success && longName() == bodyLongName) {
//        emit selectionChanged(selected);
//    }
//}



void
NsBodyObject::onOpNameChanged(const QString &oldOpInstance,
                              const QString &newOpInstance)
{
    Q_UNUSED(oldOpInstance);
    Q_UNUSED(newOpInstance);

    emit nameChanged(longName());
}
