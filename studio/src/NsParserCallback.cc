// -----------------------------------------------------------------------------
//
// NsParserCallback.cc
//
// Naiad Studio parser callback source file.
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
//
// -----------------------------------------------------------------------------

#include "NsParserCallback.h"
#include "NsQuery.h"
#include "NsStringUtils.h"
#include "NsOpObject.h"
#include "NsOpStore.h"
#include "NsCmdCreate.h"
#include "NsCmdFeed.h"
#include "NsCmdSetMeta.h"
#include "NsCmdSetParam.h"
#include "NsCmdSetOpState.h"
#include "NsCmdSetOpPosition.h"
#include "NsCmdSetGroupPlug.h"
#include <QString>
#include <QUndoCommand>
#include <QDebug>

// -----------------------------------------------------------------------------

// NsParserCallback
// ----------------
//! CTOR.

NsParserCallback::NsParserCallback(const bool     bodyNameOverride,
                                   QUndoCommand  *parent)
    : NtParserCallback()
    , _bodyNameOverride(bodyNameOverride)
    //, _opPositionOverride(opPositionOverride)
    , _parent(parent)
{}

// -----------------------------------------------------------------------------

// create
// ------
//! Creates an Op of the given class and type with the given name.

void
NsParserCallback::create(const NtString &className,
                         const NtString &type,
                         const NtString &name)
{
    const QString suggestedOpInstance(fromNbStr(name));
    QString createdOpInstance;
    
    if (0 != _parent) {
        NsCmdCreate::exec(fromNbStr(type),
                          suggestedOpInstance,
                          *_parent,
                          &createdOpInstance);
    }
    else {
        NsCmdCreate::exec(fromNbStr(type),
                          suggestedOpInstance,
                          &createdOpInstance);
    }
    
    _createdOpInstances.insert(suggestedOpInstance, createdOpInstance);
}


// setGlobalParam
// --------------
//! Sets the value of the given global parameter.

void
NsParserCallback::setGlobalParam(const NtString    &paramName,
                                 const NtString    &expr,
                                 const NtComponent  comp)
{
    if (0 != _parent) {
        NsCmdSetParam::exec(
            NsCmdSetParam::ArgsList() <<
                NsCmdSetParam::Args(
                    "Global",
                    fromNbStr(paramName),
                    fromNbStr(expr),
                    comp),
            *_parent);
    }
    else {
        NsCmdSetParam::exec(
            NsCmdSetParam::ArgsList() <<
                NsCmdSetParam::Args(
                    "Global",
                    fromNbStr(paramName),
                    fromNbStr(expr),
                    comp));
    }
}

// setGroupPlug
// --------------
//! Sets the a plug to group or single

void
NsParserCallback::setGroupPlug(const NtString& plugLongName,
                               const NtBool groupEnabled)
{
    const QString opInstance(
        _createdOpInstance(
            fromNbStr(plugLongName.parent(":"))));
    const QString plugName(fromNbStr(plugLongName.child(":")));
    const QString renamedPlugLongName = 
        opInstance + QString(":") + plugName;

    if (0 != _parent) {
        NsCmdSetGroupPlug::exec(renamedPlugLongName,
                                groupEnabled,
                                *_parent);
    } else {
        NsCmdSetGroupPlug::exec(renamedPlugLongName,
                                groupEnabled);
    }
}


// setGlobalMeta
// -------------
//! Sets the given global meta data.

void
NsParserCallback::setGlobalMeta(const NtString &paramName,
                                const NtString &type,
                                const NtString &value)
{
    _setMeta(fromNbStr("Global." + paramName),
             fromNbStr(type),
             fromNbStr(value));
}


// setParam
// --------
//! Sets the value of the given parameter.

void
NsParserCallback::setParam(const NtString    &longName,
                           const NtString    &expr,
                           const NtComponent  comp)
{
    const QString opInstance(
        _createdOpInstance(
            fromNbStr(longName.parent())));

    const QString paramName(fromNbStr(longName.child()));

    // HACK: Make sure that the "Body Name" parameter has a unique value.

    QString qExpr(fromNbStr(expr));

    if (_bodyNameOverride && ("Body Name" == paramName)) {

        const QString subTypeName(
            queryParamSubTypeName(opInstance, paramName));

        if ("cached-body-name"      != subTypeName &&
            "cached-body-name-list" != subTypeName) {
            // Body name is same as Op name, guaranteed to be unique.

            qExpr = opInstance;
        }
    }

    if (0 != _parent) {
        NsCmdSetParam::exec(
            NsCmdSetParam::ArgsList() <<
                NsCmdSetParam::Args(
                    opInstance,
                    paramName,
                    qExpr,
                    comp),
            *_parent);
    }
    else {
        NsCmdSetParam::exec(
            NsCmdSetParam::ArgsList() <<
                NsCmdSetParam::Args(
                    opInstance,
                    paramName,
                    qExpr,
                    comp));
    }
}


// setMeta
// -------
//! Sets the given Op meta data.

void
NsParserCallback::setMeta(const NtString &longName,
                          const NtString &type,
                          const NtString &value)
{
    const QString suggestedOpInstance(fromNbStr(longName.parent()));

    const QString qLongName(fromNbStr(longName));

    QStringList sl = qLongName.split(".");

    //qDebug() << longName.c_str();

    //qDebug() << "Suggested:" << suggestedOpInstance << sl[0];
    //qDebug() << "Created:" << _createdOpInstance(suggestedOpInstance) << _createdOpInstance(sl[0]);
    //qDebug() << "Meta long name 0: " << fromNbStr(longName);

    const QString metaLongName(
        fromNbStr(longName).replace(
            sl[0], _createdOpInstance(sl[0])));
    //        suggestedOpInstance, _createdOpInstance(suggestedOpInstance)));

    //qDebug() << "Meta long name 1: " << metaLongName;

    _setMeta(metaLongName,
             fromNbStr(type),
             fromNbStr(value));
}


// feed
// ----
//! Connects the plugs with the given names.

void
NsParserCallback::feed(const NtString &terminal,
                       const NtString &source)
{
    // Fix input name.

    QString inputPlugName;
    const QString inputPlugLongName =
        queryPlugLongName(
            _createdOpInstance(
                queryPlugOpInstance(
                    fromNbStr(terminal.str()), &inputPlugName)),
                inputPlugName);

    // Fix output name.

    QString outputPlugName;
    const QString outputPlugLongName =
        queryPlugLongName(
            _createdOpInstance(
                queryPlugOpInstance(
                    fromNbStr(source.str()), &outputPlugName)),
                outputPlugName);

    if (queryValidFeed(inputPlugLongName, outputPlugLongName)) {
        if (0 != _parent) {
            NsCmdFeed::exec(inputPlugLongName,
                            outputPlugLongName,
                            *_parent);
        }
        else {
            NsCmdFeed::exec(inputPlugLongName,
                            outputPlugLongName);
        }
    }
}


// setOpState
// ----------
//! Sets the state of the Op with the given name.

void
NsParserCallback::setOpState(const NtString &name,
                             const NtString &state)
{
    const QString opInstance(_createdOpInstance(fromNbStr(name)));

    QString opState = fromNbStr(state);

    if (fromNbStr(name.str()) != opInstance) {
        const QString opFamily(queryOpInstanceFamilyName(opInstance));
        opState = (opFamily == "CAMERA_SCOPE" ? "INACTIVE" : fromNbStr(state));
    }

    NiSetOpState(fromQStr(opInstance), fromQStr(opState));
    
    //if (0 != _parent) {
    //    NsCmdSetOpState::exec(
    //        opInstance,
    //        opState,
    //        *_parent);
    //}
    //else {
    //    NsCmdSetOpState::exec(
    //        opInstance,
    //        opState);
    //}
}

// -----------------------------------------------------------------------------

// createdOpPosition
// -----------------
//! DOCS

QPointF
NsParserCallback::createdOpPosition(const QString &opInstance,
                                    const QPointF &defaultValue) const
{
    return _createdOpPositions.value(opInstance, defaultValue);
}


// createdOpPositionBounds
// -----------------------
//! DOCS

void
NsParserCallback::createdOpPositionBounds(QPointF &min, QPointF &max) const
{
    min.setX( (std::numeric_limits<float>::max)());
    min.setY( (std::numeric_limits<float>::max)());
    max.setX(-(std::numeric_limits<float>::max)());
    max.setY(-(std::numeric_limits<float>::max)());

    foreach (const QPointF &pos, _createdOpPositions.values()) {
        min.setX(qMin(min.x(), pos.x()));
        min.setY(qMin(min.y(), pos.y()));
        max.setX(qMax(max.x(), pos.x()));
        max.setY(qMax(max.y(), pos.y()));
    }
}

// -----------------------------------------------------------------------------

// _isCreated
// ----------
//! Returns true if the provided Op instance exists in the map of
//! created Op instances.

bool
NsParserCallback::_isCreated(const QString &opInstance) const
{
    return _createdOpInstances.contains(opInstance);
}


// _createdOpInstance
// ------------------
//! If given Op instance does not exist in map return the provided Op instance.

QString
NsParserCallback::_createdOpInstance(const QString &opInstance) const
{
    return _createdOpInstances.value(opInstance, opInstance);
}


// _setMeta
// --------
//! Set a meta value.

void
NsParserCallback::_setMeta(const QString &metaLongName,
                           const QString &valueType,
                           const QString &value)
{
    // HACK: Backwards compatibility for the old separate x & y meta positions.

    static QString _cachedx;

    QString pos;

    if ("x" == valueType) {
        _cachedx = value;
    }
    else if ("y" == valueType) {
        std::stringstream ss;
        ss << fromQStr(_cachedx) << "," << fromQStr(value);
        pos = fromNbStr(ss.str());
        
        if (0 != _parent) {
            NsCmdSetMeta::exec(metaLongName, "Pos", pos, *_parent);
        }
        else {
            NsCmdSetMeta::exec(metaLongName, "Pos", pos);
        }
    }
    else {
        if ("Pos" == valueType) {  
            pos = value;
        }

        if (0 != _parent) {
            NsCmdSetMeta::exec(metaLongName,
                               valueType,
                               value,
                               *_parent);
        }
        else {
            NsCmdSetMeta::exec(metaLongName,
                               valueType,
                               value);
        }
    }

    if (!pos.isEmpty()) {
        qreal x;
        qreal y;
        if (NsValueObject::positionCoordinates(pos, x, y)) {
            _createdOpPositions.insert(metaLongName, QPointF(x, y));
        }
    }
}
