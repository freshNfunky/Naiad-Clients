// -----------------------------------------------------------------------------
//
// NsOpStore.h
//
// Naiad Studio Op store, header file.
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

#ifndef NS_OP_STORE_H
#define NS_OP_STORE_H

#include "NsInputPlugObject.h"
#include "NsStringUtils.h"
#include <NiTypes.h>
#include <QObject>  // Base.
#include <QMap>
#include <QHash>
#include <QSet>
#include <QStack>
#include <QString>
#include <QPointF>

class NsValueObjectHandle;
class NsOpObject;
class NsBodyObject;
class NsPlugObject;
//class NsInputPlugObject;
class NsOutputPlugObject;
class NsBodyInputPlugObject;
class NsFieldInputPlugObject;
class NsBodyOutputPlugObject;
class NsFieldOutputPlugObject;

class QAction;

// -----------------------------------------------------------------------------

// NsOpStore
// ---------
//! Singleton.
//! Note that Op objects can only be added/removed through commands.

class NsOpStore : public QObject
{
    Q_OBJECT

public:     // Singleton interface.

    static NsOpStore*
    instance();

    static void
    createInstance();

    static void
    destroyInstance();

private:

    static NsOpStore *_instance;

public:     // Global Op.

    NsOpObject*
    mutableGlobalOp()
    { return _globalOp; }

    const NsOpObject*
    constGlobalOp() const
    { return _globalOp; }

public:     // Ops.

    //! May return null.
    NsOpObject*
    queryMutableOp(const QString &opInstance)
    { return _findOp(opInstance); }

    //! May return null.
    const NsOpObject*
    queryConstOp(const QString &opInstance) const
    { return _findOp(opInstance); }


    QList<NsOpObject*>
    mutableOps();

    QList<const NsOpObject*>
    constOps() const;


    QList<NsOpObject*>
    mutableOpType(const QString &typeName);

    QList<const NsOpObject*>
    constOpType(const QString &typeName) const;


    QList<NsOpObject*>
    mutableOpFamily(const QString &familyName);

    QList<const NsOpObject*>
    constOpFamily(const QString &familyName) const;


    QList<NsOpObject*>
    mutableSelectedOps();

    QList<const NsOpObject*>
    constSelectedOps() const;


    QList<NsOpObject*>
    mutableUnselectedOps();

    QList<const NsOpObject*>
    constUnselectedOps() const;

public:     // Plugs.

    //! May return null.
    NsPlugObject*
    queryMutablePlug(const QString &plugLongName)
    { return _findPlug<NsPlugObject>(plugLongName); }

    //! May return null.
    const NsPlugObject*
    queryConstPlug(const QString &plugLongName) const
    { return _findPlug<NsPlugObject>(plugLongName); }


    //! May return null.
    NsInputPlugObject*
    queryMutableInput(const QString &plugLongName)
    { return _findPlug<NsInputPlugObject>(plugLongName); }

    //! May return null.
    const NsInputPlugObject*
    queryConstInput(const QString &plugLongName) const
    { return _findPlug<NsInputPlugObject>(plugLongName); }


    //! May return null.
    NsOutputPlugObject*
    queryMutableOutput(const QString &plugLongName)
    { return _findPlug<NsOutputPlugObject>(plugLongName); }

    //! May return null.
    const NsOutputPlugObject*
    queryConstOutput(const QString &plugLongName) const
    { return _findPlug<NsOutputPlugObject>(plugLongName); }


    //! May return null.
    NsBodyInputPlugObject*
    queryMutableBodyInput(const QString &plugLongName)
    { return _findPlug<NsBodyInputPlugObject>(plugLongName); }

    //! May return null.
    const NsBodyInputPlugObject*
    queryConstBodyInput(const QString &plugLongName) const
    { return _findPlug<NsBodyInputPlugObject>(plugLongName); }


    //! May return null.
    NsFieldInputPlugObject*
    queryMutableFieldInput(const QString &plugLongName)
    { return _findPlug<NsFieldInputPlugObject>(plugLongName); }

    //! May return null.
    const NsFieldInputPlugObject*
    queryConstFieldInput(const QString &plugLongName) const
    { return _findPlug<NsFieldInputPlugObject>(plugLongName); }


    //! May return null.
    NsBodyOutputPlugObject*
    queryMutableBodyOutput(const QString &plugLongName)
    { return _findPlug<NsBodyOutputPlugObject>(plugLongName); }

    //! May return null.
    const NsBodyOutputPlugObject*
    queryConstBodyOutput(const QString &plugLongName) const
    { return _findPlug<NsBodyOutputPlugObject>(plugLongName); }


    //! May return null.
    NsFieldOutputPlugObject*
    queryMutableFieldOutput(const QString &plugLongName)
    { return _findPlug<NsFieldOutputPlugObject>(plugLongName); }

    //! May return null.
    const NsFieldOutputPlugObject*
    queryConstFieldOutput(const QString &plugLongName) const
    { return _findPlug<NsFieldOutputPlugObject>(plugLongName); }


    QList<NsInputPlugObject*>
    mutableInputs();

    QList<const NsInputPlugObject*>
    constInputs() const;

    // TODO: Cache selected set?
    QList<NsInputPlugObject*>
    mutableSelectedInputs(bool selected = true);

    QList<const NsInputPlugObject*>
    constSelectedInputs(bool selected = true) const;

public:     // Bodies.

    NsBodyObject*
    queryMutableBody(const QString &bodyLongName) const
    { return _findBody(bodyLongName); }

    const NsBodyObject*
    queryConstBody(const QString &bodyLongName) const
    { return _findBody(bodyLongName); }


    QList<NsBodyObject*>
    mutableBodies();

    QList<const NsBodyObject*>
    constBodies() const;

    // TODO: Cache selected set?
    QList<NsBodyObject*>
    mutableSelectedBodies(bool selected = true);

    QList<const NsBodyObject*>
    constSelectedBodies(bool selected = true) const;

    // TODO: Values.
    // TODO: Feeds.

public:

    void
    opPositionBounds(QPointF &min, QPointF &max) const;

    bool
    contains(const QString &opInstance) const;

    bool
    contains(const NsValueObjectHandle &handle) const;

protected slots:    // Op slots.

    void
    onOpCreated( const QString &opInstance, bool success);

    void
    onOpErased(const QString &opInstance, bool success);

    void
    onOpNameChanged(const QString &oldOpInstance,
                    const QString &newOpInstance,
                    bool           success);

    void
    onOpStateChanged(const QStringList &opInstances,
                     bool               success);

    void
    onOpSelectionChanged(const QStringList &opInstances,
                         bool               success);

    void
    onOpPositionChanged(const QString &opInstance,
                        qreal          x,
                        qreal          y,
                        bool           success);

protected slots:    // Plug slots.

    void
    onGroupPlugChanged(const QString &plugLongName,
                       bool           success);

    void
    onSmackChanged(const QStringList &plugLongNames,
                   bool               success);

protected slots:    // Feed slots.

    void
    onFeedChanged(const QString &inputLongName,
                  const QString &plugLongName,
                  bool           success);

    void
    onFeedSelectionChanged(const QStringList &inputLongNames,
                           bool               success);

protected slots:    // Body slots.

    void
    onBodySelectionChanged(const QStringList &bodyLongNames,
                           bool               success);

protected slots:    // Value slots.

    //void
    //onValueChanged(const QString &valueLongName,
    //               const QString &expr,
    //               int            comp,
    //               bool           success);

    //void
    //onMetaChanged(const QString &longName,
    //              const QString &valueType,
    //              const QString &value,
    //              bool           success);

    //void
    //onProjectPathChanged(const QString &path, bool success);

    void
    onCurrentVisibleFrameChanged(int cvf, bool update3DView, bool success);

    //void
    //onFirstVisibleFrameChanged(int fvf, bool success);

    //void
    //onLastVisibleFrameChanged(int lvf, bool success);

protected slots:    // Misc.

    void
    onGraphCleared(bool success);

    //void
    //onGraphViewFramed(bool, int, bool)
    //{}

protected slots:    // Stepping slots.

    void
    onBeginTimeStep(const NtTimeBundle &tb);

    void
    onEndTimeStep(const NtTimeBundle &tb);

    void
    onBeginOp(const NtTimeBundle &tb, const QString &opInstance);

    void
    onEndOp(const NtTimeBundle &tb, const QString &opInstance);

    void
    onReset();

signals:

    //! Emitted when an Op object has been added to the store.
    void
    opCreated(const QString &opInstance);

    void
    opCreated(const NsValueObjectHandle &handle);

    //! Emitted when an Op object has been added to the store.
    void
    opObjectCreated(NsOpObject *op);

    //! Emitted when an Op object has been removed from the store.
    void
    opObjectDestroyed(NsOpObject *op);

//    //! Emitted when an Op object has been removed from the store.
//    void
//    opErased(const QString &opInstance);

//    void
//    opErased(const NsValueObjectHandle &handle);

    ////! Emitted when the Op store (i.e. the graph) has been cleared.
    //void
    //graphCleared();

private:    // Op editing.

    NsOpObject*
    _createOp(const QString &opInstance);

    void
    _eraseOp(const QString &opInstance);

    void
    _emitOpNameChanged(const QString &oldOpInstance, 
                       const QString &newOpInstance);

    void
    _emitOpStateChanged(const QString &opInstance);

    void
    _emitOpSelectionChanged(const QString &opInstance);

    void
    _emitOpPositionChanged(const QString &opInstance, qreal x, qreal y);

private:    // Plug editing.

    void
    _emitGroupPlugChanged(const QString &plugLongName);

    void
    _emitSmackChanged(const QString &inputLongName);

private:    // Feed editing.

    void
    _emitFeedChanged(const QString &inputLongName, const QString &plugLongName);

    void
    _emitFeedSelectionChanged(const QString &inputLongName);

private:    // Body "editing".

    void
    _emitBodySelectionChanged(const QString &bodyLongName);

private:    // Value editing.

    //void
    //_emitValueChanged(const QString &valueLongName,
    //                  const QString &expr,
    //                  int            comp);

    //void
    //_emitMetaChanged(const QString &longName,
    //                 const QString &valueType,
    //                 const QString &value);

    //void
    //_emitProjectPathChanged(const QString &path);

private:    // Stepping.

    void
    _emitBeginTimeStep(const NtTimeBundle &tb);

    void
    _emitEndTimeStep(const NtTimeBundle &tb);

    void
    _emitBeginOp(const NtTimeBundle &tb, const QString &opInstance);

    void
    _emitEndOp(const NtTimeBundle &tb, const QString &opInstance);

    void
    _reset();

private:

    void
    _clear();

    void
    _resetOpConditions();

private:    // Find Op.

    NsOpObject*
    _findOp(const QString &opInstance) const;

    NsOpObject*
    _findOp(const NsValueObjectHandle &handle) const;

private:    // Find plug.

    template<class P>
    P*
    _findPlug(const QString &plugLongName) const
    {
        QString opInstance;
        QString plugName;
        _splitPlugLongName(plugLongName, opInstance, plugName);
        NsOpObject *op = _findOp(opInstance);
        P *plug = 0;
        if (0 != op) {
            plug = _queryMutablePlug<P>(*op, plugName);
        }
        return plug;
    }

    
    template<class P>
    static P* 
    _queryMutablePlug(NsOpObject &op, const QString &plugName);

    static void
    _splitPlugLongName(const QString &plugLongName, 
                       QString       &opInstance, 
                       QString       &plugName);

private:    // Find body.

    NsBodyObject*
    _findBody(const QString &bodyLongName) const;

    static void
    _splitBodyLongName(const QString &bodyLongName,
                       QString       &opInstance,
                       QString       &plugName,
                       QString       &bodyName);

private:

    NsOpStore();

    virtual
    ~NsOpStore();

private:    // Member variables.

    typedef QMap<QString, NsValueObjectHandle>      _OpInstanceMapType;
    typedef QHash<NsValueObjectHandle, NsOpObject*> _OpObjectHashType;
    typedef QSet<NsOpObject*>                       _OpSetType;
    typedef QMap<QString, _OpSetType>               _OpFamilyMapType;
    typedef QMap<QString, _OpSetType>               _OpTypeMapType;

    _OpInstanceMapType   _opInstances;
    _OpObjectHashType    _opObjects;
    _OpFamilyMapType     _opFamilies;
    _OpTypeMapType       _opTypes;
    _OpSetType           _selectedOps;
    _OpSetType           _unselectedOps;

    NsOpObject *_globalOp;
};

#endif // NS_OP_STORE_H
