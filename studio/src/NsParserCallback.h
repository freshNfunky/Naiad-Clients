// -----------------------------------------------------------------------------
//
// NsParserCallback.h
//
// Naiad Studio parser callback header file.
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

#ifndef NS_PARSER_CALLBACK_H
#define NS_PARSER_CALLBACK_H

#include <Ni.h>
#include <QMap>
#include <QString>
#include <QPointF>
#include <QStringList>

class QUndoCommand;

// -----------------------------------------------------------------------------

// NsParserCallback
// ----------------
//! The NsParserCallback class, representing the Naiad Interface file parser
//! callback. Creates objects in the Naiad graph, as well as in the Naiad
//! Studio graph. Basically a translator between NI-commands and Naiad Studio
//! commands.

class NsParserCallback : public NtParserCallback
{
public:

    explicit
    NsParserCallback(bool          bodyNameOverride,
                     QUndoCommand *parent                 = 0);

    //! DTOR.
    virtual
    ~NsParserCallback()
    {}

    virtual void
    create(const NtString &className,
           const NtString &type,
           const NtString &name);

    virtual void
    setGlobalParam(const NtString    &paramName,
                   const NtString    &expr,
                   const NtComponent  comp);

    virtual void
    setGlobalMeta(const NtString &paramName,
                  const NtString &type,
                  const NtString &value);

    virtual void
    setParam(const NtString &longName,
             const NtString &expr,
             const NtComponent component);

    virtual void
    setMeta(const NtString &longName,
            const NtString &type,
            const NtString &value);

    virtual void
    feed(const NtString &terminal,
         const NtString &source);

    virtual void
    setOpState(const NtString& name,
               const NtString& state);

    virtual void
    setGroupPlug(const NtString& plugLongName,
                 const NtBool groupEnabled);

    //! DEPRECATED
    virtual void
    setEnabled(const NtString &name,
               const NtBool enabled)
    {}

public:

    QStringList
    createdOpInstances() const
    { return _createdOpInstances.values(); }

    //const QMap<QString,QPointF>&
    //createdOpPositions() const
    //{ return _createdOpPositions; }
    
    QPointF
    createdOpPosition(const QString &opInstance,
                      const QPointF &defaultValue = QPointF(0., 0.)) const;

    void
    createdOpPositionBounds(QPointF &min, QPointF &max) const;

private:

    bool
    _isCreated(const QString &opInstance) const;

    QString
    _createdOpInstance(const QString &opInstance) const;

    void
    _setMeta(const QString &metaLongName,
             const QString &valueType,
             const QString &value);

private:    // Member variables.

    QUndoCommand *_parent;

    bool _bodyNameOverride;
    bool _opPositionOverride;

    //! Map suggested Op instances to the actually created ones.
    QMap<QString,QString>  _createdOpInstances;

    //! Position of created Ops.
    QMap<QString,QPointF>  _createdOpPositions;
};

#endif  // NS_PARSER_CALLBACK_H
