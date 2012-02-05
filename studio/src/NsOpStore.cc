// -----------------------------------------------------------------------------
//
// NsOpStore.cc
//
// Naiad Studio Op store, source file.
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

#include "NsOpStore.h"
#include "NsValueObjectHandle.h"
#include "NsOpObject.h"
#include "NsBodyObject.h"
#include "NsValueBaseObject.h"
#include "NsBodyOutputPlugObject.h"
#include "NsBodyInputPlugObject.h"
#include "NsFieldInputPlugObject.h"
#include "NsGraphCallback.h"
#include "NsCmdCentral.h"
#include "NsCmdSetOpState.h"
#include "NsCmdUnselectAndErase.h"  // TMP!
#include "NsQuery.h"
#include "NsStringUtils.h"
#include <NgStore.h>
#include <QAction>

// -----------------------------------------------------------------------------

// instance
// --------
//! Provide access to singleton. [static]

NsOpStore *
NsOpStore::instance()
{
    if (0 == _instance) {
        createInstance();
    }

    return _instance;
}


// createInstance
// --------------
//! Reset singleton. [static]

void
NsOpStore::createInstance()
{
    destroyInstance();
    _instance = new NsOpStore;
}


// destroyInstance
// ---------------
//! Reset singleton. [static]

void
NsOpStore::destroyInstance()
{
    delete _instance;
    _instance = 0;
}


//! Singleton pointer. [static]
NsOpStore *NsOpStore::_instance = 0;

// -----------------------------------------------------------------------------

QList<NsOpObject*>
NsOpStore::mutableOps()
{ return _opObjects.values(); }

QList<const NsOpObject*>
NsOpStore::constOps() const
{ return constPtrList(_opObjects.values()); }


QList<NsOpObject*>
NsOpStore::mutableOpType(const QString &typeName)
{ return _opTypes.value(typeName).toList(); }

QList<const NsOpObject*>
NsOpStore::constOpType(const QString &typeName) const
{ return constPtrList(_opTypes.value(typeName).toList()); }


QList<NsOpObject*>
NsOpStore::mutableOpFamily(const QString &familyName)
{ return _opFamilies.value(familyName).toList(); }

QList<const NsOpObject*>
NsOpStore::constOpFamily(const QString &familyName) const
{ return constPtrList(_opFamilies.value(familyName).toList()); }


QList<NsOpObject*>
NsOpStore::mutableSelectedOps()
{ return _selectedOps.toList(); }

QList<const NsOpObject*>
NsOpStore::constSelectedOps() const
{ return constPtrList(_selectedOps.toList()); }


QList<NsOpObject*>
NsOpStore::mutableUnselectedOps()
{ return _unselectedOps.toList(); }

QList<const NsOpObject*>
NsOpStore::constUnselectedOps() const
{ return constPtrList(_unselectedOps.toList()); }

// -----------------------------------------------------------------------------

// mutableInputs
// -------------
//! DOCS

QList<NsInputPlugObject*>
NsOpStore::mutableInputs()
{
    QList<NsInputPlugObject*> inputs;
    foreach (NsOpObject *op, _opObjects.values()) {
        foreach(NsInputPlugObject *ipo, op->mutableInputs()) {
            inputs.append(ipo);
        }
    }
    return inputs;
}


// constInputPlugs
// ---------------
//! DOCS

QList<const NsInputPlugObject*>
NsOpStore::constInputs() const
{
    QList<const NsInputPlugObject*> inputs;
    foreach (const NsOpObject *op, _opObjects.values()) {
        foreach(const NsInputPlugObject *ipo, op->constInputs()) {
            inputs.append(ipo);
        }
    }
    return inputs;
}


// mutableSelectedInputPlugs
// -------------------------
//! DOCS

QList<NsInputPlugObject*>
NsOpStore::mutableSelectedInputs(const bool selected)
{
    QList<NsInputPlugObject*> inputs;
    foreach (NsOpObject *op, _opObjects.values()) {
        foreach(NsInputPlugObject *ipo, op->mutableInputs()) {
            if (selected == ipo->isSelected()) {
                inputs.append(ipo);
            }
        }
    }
    return inputs;
}


// constSelectedInputs
// -------------------
//! DOCS

QList<const NsInputPlugObject*>
NsOpStore::constSelectedInputs(const bool selected) const
{
    QList<const NsInputPlugObject*> inputs;
    foreach (const NsOpObject *op, _opObjects.values()) {
        foreach(const NsInputPlugObject *ipo, op->constInputs()) {
            if (selected == ipo->isSelected()) {
                inputs.append(ipo);
            }
        }
    }
    return inputs;
}

// -----------------------------------------------------------------------------

// mutableBodies
// -------------
//! DOCS

QList<NsBodyObject*>
NsOpStore::mutableBodies()
{
    QList<NsBodyObject*> bodies;
    foreach (NsOpObject *op, _opObjects.values()) {
        bodies += op->mutableCachedBodies();
        foreach (NsBodyOutputPlugObject *bopo, op->mutableBodyOutputs()) {
            bodies += bopo->mutableLiveBodies();
        }
    }
    return bodies;
}


// constBodies
// -----------
//! DOCS

QList<const NsBodyObject*>
NsOpStore::constBodies() const
{
    QList<const NsBodyObject *> bodies;
    foreach (const NsOpObject *op, _opObjects.values()) {
        bodies += op->constCachedBodies();
        foreach (const NsBodyOutputPlugObject *bopo, op->constBodyOutputs()) {
            bodies += bopo->constLiveBodies();
        }
    }
    return bodies;
}


// mutableSelectedBodies
// ---------------------
//! DOCS

QList<NsBodyObject*>
NsOpStore::mutableSelectedBodies(const bool selected)
{
    QList<NsBodyObject*> bodies;
    foreach (NsOpObject *op, _opObjects.values()) {
        foreach (NsBodyObject *bo, op->mutableCachedBodies()) {
            if (selected == bo->isSelected()) {
                bodies.append(bo);
            }
        }
        foreach (NsBodyOutputPlugObject *bopo, op->mutableBodyOutputs()) {
            foreach (NsBodyObject *bo, bopo->mutableLiveBodies()) {
                if (selected == bo->isSelected()) {
                    bodies.append(bo);
                }
            }
        }
    }
    return bodies;
}


// constSelectedBodies
// -------------------
//! DOCS

QList<const NsBodyObject*>
NsOpStore::constSelectedBodies(const bool selected) const
{
    QList<const NsBodyObject*> bodies;
    foreach (const NsOpObject *op, _opObjects.values()) {
        foreach (const NsBodyObject *bo, op->constCachedBodies()) {
            if (selected == bo->isSelected()) {
                bodies.append(bo);
            }
        }
        foreach (const NsBodyOutputPlugObject *bopo, op->constBodyOutputs()) {
            foreach (const NsBodyObject *bo, bopo->constLiveBodies()) {
                if (selected == bo->isSelected()) {
                    bodies.append(bo);
                }
            }
        }
    }
    return bodies;
}

// -----------------------------------------------------------------------------





// -----------------------------------------------------------------------------

//bool
//NsOpStore::metaValue(const QString &longName,
//                     const QString &valueType,
//                     QString       *value) const
//{
//    Nb::Object *obj(Ng::Store::object(fromQStr(longName)));
//
//    if (0 != obj) {
//        typedef Nb::Object::MetaValueMap::const_iterator IterType;
//        const IterType iter(obj->metaValueMap().find(fromQStr(valueType)));
//
//        if (iter != obj->metaValueMap().end() && 0 != value ) {
//            *value = fromNbStr(iter->second);
//            return true;
//        }
//    }
//
//    return false;
//}


void
NsOpStore::opPositionBounds(QPointF &min, QPointF &max) const
{
    min.setX( (std::numeric_limits<float>::max)());
    min.setY( (std::numeric_limits<float>::max)());
    max.setX(-(std::numeric_limits<float>::max)());
    max.setY(-(std::numeric_limits<float>::max)());

    foreach (const NsOpObject *op, _opObjects.values()) {
        const QPointF pos(op->position());

        min.setX(qMin(min.x(), pos.x()));
        min.setY(qMin(min.y(), pos.y()));
        max.setX(qMax(max.x(), pos.x()));
        max.setY(qMax(max.y(), pos.y()));
    }
}

// -----------------------------------------------------------------------------

// contains
// --------
//! Returns true if the provided name exists in the store.

bool
NsOpStore::contains(const QString &opInstance) const
{ return _opInstances.contains(opInstance); }


// contains
// --------
//! Returns true if the provided handle exists in the store.

bool
NsOpStore::contains(const NsValueObjectHandle &handle) const
{ return _opObjects.contains(handle); }

// -----------------------------------------------------------------------------

// onOpCreated
// -----------
//! An Op has been created with a command. [slot]

void
NsOpStore::onOpCreated(const QString &opInstance, const bool success)
{
    if (success) {
        _createOp(opInstance);
    }
}


// onOpErased
// ----------
//! An Op has been erased with a command. [slot]

void
NsOpStore::onOpErased(const QString &opInstance, const bool success)
{
    if (success) {
        _eraseOp(opInstance);
    }
}


// onOpNameChanged
// ---------------
//! Update map when an Op object changes name. [slot]

void
NsOpStore::onOpNameChanged(const QString &oldOpInstance,
                           const QString &newOpInstance,
                           const bool     success)
{
    if (success) {
        _emitOpNameChanged(oldOpInstance, newOpInstance);
    }
}


// onOpStateChanged
// ----------------
//! DOCS [slot]

void
NsOpStore::onOpStateChanged(const QStringList &opInstances,
                            const bool         success)
{
    if (success) {
        foreach (const QString &opInstance, opInstances) {
            _emitOpStateChanged(opInstance);
        }
    }
}


// onOpSelectionChanged
// --------------------
//! DOCS [slot]

void
NsOpStore::onOpSelectionChanged(const QStringList &opInstances,
                                const bool         success)
{
    qDebug() << "NsOpStore::onOpSelectionChanged";
    if (success) {
        foreach (const QString &opInstance, opInstances) {
            _emitOpSelectionChanged(opInstance);
        }
    }
}


// onOpPositionChanged
// -------------------
//! DOCS [slot]

void
NsOpStore::onOpPositionChanged(const QString &opInstance,
                               const qreal    x,
                               const qreal    y,
                               const bool     success)
{
    if (success) {
        _emitOpPositionChanged(opInstance, x, y);
    }
}

// -----------------------------------------------------------------------------

// onGroupPlugChanged
// ------------------
//! DOCS [slot]

void
NsOpStore::onGroupPlugChanged(const QString &plugLongName,
                              const bool     success)
{
    if (success) {
        _emitGroupPlugChanged(plugLongName);
    }
}


// onSmackChanged
// --------------
//! DOCS [slot]

void
NsOpStore::onSmackChanged(const QStringList &plugLongNames,
                          const bool         success)
{
    if (success) {
        foreach (const QString &plugLongName, plugLongNames) {
            _emitSmackChanged(plugLongName);
        }
    }
}

// -----------------------------------------------------------------------------

// onFeedChanged
// -------------
//! DOCS [slot]

void
NsOpStore::onFeedChanged(const QString &inputLongName,
                         const QString &plugLongName,
                         const bool     success)
{
    if (success) {
        _emitFeedChanged(inputLongName, plugLongName);
    }
}


void
NsOpStore::onFeedSelectionChanged(const QStringList &inputLongNames,
                                  const bool         success)
{
    if (success) {
        foreach (const QString &inputLongName, inputLongNames) {
            _emitFeedSelectionChanged(inputLongName);
        }
    }
}

// -----------------------------------------------------------------------------

void
NsOpStore::onBodySelectionChanged(const QStringList &bodyLongNames,
                                  const bool         success)
{
    if (success) {
        foreach (const QString &bodyLongName, bodyLongNames) {
            _emitBodySelectionChanged(bodyLongName);
        }
    }
}

// -----------------------------------------------------------------------------

//// onValueChanged
//// --------------
////! [slot]
//
//void
//NsOpStore::onValueChanged(const QString &valueLongName,
//                          const QString &expr,
//                          const int      comp,
//                          const bool     success)
//{
//    if (success) {
//        _emitValueChanged(valueLongName, expr, comp);
//    }
//}
//
//
//// onMetaChanged
//// -------------
////! [slot]
//
//void
//NsOpStore::onMetaChanged(const QString &longName,
//                         const QString &valueType,
//                         const QString &value,
//                         const bool     success)
//{
//    if (success) {
//        _emitMetaChanged(longName, valueType, value);
//    }
//}

//void
//NsOpStore::onProjectPathChanged(const QString &path, const bool success)
//{
//    if (success) {
//        _emitProjectPathChanged(path);
//    }
//}

void
NsOpStore::onCurrentVisibleFrameChanged(const int  cvf,
                                        const bool update3DView,
                                        const bool success)
{
    Q_UNUSED(cvf);
    Q_UNUSED(update3DView);

    if (success) {
        _resetOpConditions();
    }
}

//void
//NsOpStore::onFirstVisibleFrameChanged(int fvf, bool success)
//{}
//
//void
//NsOpStore::onLastVisibleFrameChanged(int lvf, bool success)
//{}

// -----------------------------------------------------------------------------

// onGraphCleared
// --------------
//! All graph components have been deleted. [slot]

void
NsOpStore::onGraphCleared(const bool success)
{
    if (success) {
        _clear();
    }
}

// -----------------------------------------------------------------------------

// onBeginTimeStep
// ---------------
//! [slot]

void
NsOpStore::onBeginTimeStep(const NtTimeBundle &tb)
{
    _resetOpConditions();
    _emitBeginTimeStep(tb);
}

// onEndTimeStep
// -------------
//! [slot]

void
NsOpStore::onEndTimeStep(const NtTimeBundle &tb)
{
    _emitEndTimeStep(tb);
}


// onBeginOp
// ---------
//! [slot]

void
NsOpStore::onBeginOp(const NtTimeBundle &tb, const QString &opInstance)
{
    _emitBeginOp(tb, opInstance);
}


// onEndOp
// -------
//! [slot]

void
NsOpStore::onEndOp(const NtTimeBundle &tb, const QString &opInstance)
{
    _emitEndOp(tb, opInstance);
}

void
NsOpStore::onReset()
{
    _reset();
}

// -----------------------------------------------------------------------------

// _createOp
// ---------
//! DOCS

NsOpObject*
NsOpStore::_createOp(const QString &opInstance)
{
    Ng::Op *ngOp(Ng::Store::opInstance(fromQStr(opInstance)));

    if (0 != ngOp && !contains(opInstance)) {
        NsOpObject *op(new NsOpObject(*ngOp, this)); // Child of store.

        // Connect Op object to command central.

        connect(NsCmdCentral::instance(),
                SIGNAL(valueChanged(QString,QString,int,bool)),
                op,
                SLOT(onValueChanged(QString,QString,int,bool)));

        connect(NsCmdCentral::instance(),
                SIGNAL(metaChanged(QString,QString,QString,bool)),
                op,
                SLOT(onMetaChanged(QString,QString,QString,bool)));

        connect(NsCmdCentral::instance(),
                SIGNAL(projectPathChanged(QString,bool)),
                op,
                SLOT(onProjectPathChanged(QString,bool)));

        connect(NsCmdCentral::instance(),
                SIGNAL(currentVisibleFrameChanged(int,bool,bool)),
                op,
                SLOT(onCurrentVisibleFrameChanged(int,bool,bool)));

        //connect(NsCmdCentral::instance(),
        //        SIGNAL(feedChanged(QString,QString,bool)),
        //        op,
        //        SLOT(onFeedChanged(QString,QString,bool)));

        _opInstances.insert(opInstance, op->handle());
        _opObjects.insert(op->handle(), op);
        _opFamilies[op->familyName()] << op;
        _opTypes[op->typeName()] << op;
        (op->isSelected() ? _selectedOps : _unselectedOps) << op;

        //emit opCreated(opInstance);
        emit opObjectCreated(op);

        return op;
    }

    return 0;
}


// _eraseOp
// --------
//! DOCS

void
NsOpStore::_eraseOp(const QString &opInstance)
{
    qDebug() << "NsOpStore::_eraseOp";

    // At this point all resource pointers to server objects are invalid.
    // Don't use them!

    const _OpInstanceMapType::iterator i0(_opInstances.find(opInstance));

    if (_opInstances.end() != i0) {
        // Op instance exists.

        const _OpObjectHashType::iterator i1(_opObjects.find(i0.value()));

        if (_opObjects.end() != i1 && i1.value() != _globalOp) {
            // Op handle exists.

            NsOpObject *op = i1.value();
            const QString familyName = op->familyName();
            const QString typeName = op->typeName();

            _opFamilies[familyName].remove(op);
            _opTypes[typeName].remove(op);
            _selectedOps.remove(op);
            _unselectedOps.remove(op);
            _opInstances.erase(i0);
            _opObjects.erase(i1); // Remove handle association.

            // We mustn't emit until all associations have been removed from
            // the store.

            emit opObjectDestroyed(op);
            delete op;
        }
    }
}


// _renameOp
// ---------
//! DOCS

void
NsOpStore::_emitOpNameChanged(const QString &oldOpInstance, 
                              const QString &newOpInstance)
{
    _OpInstanceMapType::iterator iter(_opInstances.find(oldOpInstance));
    if (_opInstances.end() != iter) {

        NsOpObject *op = _findOp(iter.value());
        EM_ASSERT(0 != op);
        op->emitNameChanged(oldOpInstance, newOpInstance);

        // Reinsert the handle with updated name in map.

        _opInstances.insert(newOpInstance, iter.value());
        _opInstances.erase(iter);
    }
}


// _setOpState
// -----------
//! DOCS

void
NsOpStore::_emitOpStateChanged(const QString &opInstance)
{
    NsOpObject *op = _findOp(opInstance);
    if (0 != op) {
        //op->emitStateChanged(opState);

        if ("CAMERA_SCOPE" == op->familyName() && "ACTIVE" == op->state()) {
            // A camera scope state was set to ACTIVE, deactivate all other
            // camera scopes.

            foreach (NsOpObject *cam, mutableOpFamily("CAMERA_SCOPE")) {
                if (cam != op) {
                    NiSetOpState(fromQStr(cam->longName()), "INACTIVE");
                    //cam->emitStateChanged("INACTIVE");
                }
            }
        }

        foreach (NsOpObject *op1, mutableOps()) {
            op1->emitStateChanged(/*op1->state()*/);
        }
    }

    //const NsOpObject *op(
    //    NsOpStore::instance()->queryConstOp(opInstance));
    //if (0 != op && op != this &&
    //    "ACTIVE" == opState &&
    //    "CAMERA_SCOPE" == familyName() &&
    //    "CAMERA_SCOPE" == op->familyName()) {
    //    // Another camera scope state was set to ACTIVE, deactivate this
    //    // camera scope.

    //    //NsCmdSetOpState::exec(longName(), "INACTIVE");
    //    NiSetOpState(_op->longname(), "INACTIVE");
    //}
    //else {
    //    if (longName() == opInstance) {
    //        emit stateChanged(opState);
    //    }
    //    else {
    //        emit stateChanged(state());
    //    }
    //}
}


// _selectOp
// ---------
//! DOCS

void
NsOpStore::_emitOpSelectionChanged(const QString &opInstance)
{
    qDebug() << "NsOpStore::_emitOpSelectionChanged" << opInstance;
    NsOpObject *op = _findOp(opInstance);
    if (0 != op) {
        const bool selected = op->isSelected();
        if (selected) {
            // The Op became selected, remove from unselected and add
            // to selected.

            _unselectedOps.remove(op);
            _selectedOps.insert(op);
        }
        else {
            // The Op became unselected, add to unselected and remove
            // from selected.

            _unselectedOps.insert(op);
            _selectedOps.remove(op);
        }
        op->emitSelectionChanged();
    }
}


void
NsOpStore::_emitOpPositionChanged(const QString &opInstance, 
                                  const qreal    x, 
                                  const qreal    y)
{
    NsOpObject *op = _findOp(opInstance);
    if (0 != op) {
        op->emitPositionChanged(x, y);
    }
}

// -----------------------------------------------------------------------------

// _emitGroupPlugChanged
// ---------------------
//! DOCS

void
NsOpStore::_emitGroupPlugChanged(const QString &plugLongName)
{
    NsPlugObject *plug = _findPlug<NsPlugObject>(plugLongName);
    if (0 != plug) {
        plug->emitGroupChanged();
    }
}


// _emitSmackChanged
// -----------------
//! DOCS

void
NsOpStore::_emitSmackChanged(const QString &plugLongName)
{
    NsPlugObject *plug = _findPlug<NsPlugObject>(plugLongName);
    if (0 != plug) {
        plug->emitSmackChanged();
    }
}

// -----------------------------------------------------------------------------

void
NsOpStore::_emitFeedChanged(const QString &inputLongName, 
                            const QString &plugLongName)
{

    foreach (NsOpObject *op, mutableOps()) {
        // Any feed change in the graph could possibly affect the
        // state of any Op.

        op->emitStateChanged(/*op->state()*/);
    }

    // Body caching is not affected
    // by removed feeds, so we check to see if the feed change was 
    // actually a feed removal, in which case we do nothing.

    if (!plugLongName.isEmpty()) {
        // A feed was added to the graph.

        const QString inputOpInstance = 
            fromNbStr(fromQStr(inputLongName).parent(":"));
        NsOpObject *inputOp = _findOp(inputOpInstance);
        if (0 != inputOp) {
            inputOp->updateBodyCachePolicy();
        }

        const QString plugOpInstance =
            fromNbStr(fromQStr(plugLongName).parent(":"));
        NsOpObject *plugOp = _findOp(plugOpInstance);
        if (0 != plugOp) {
            plugOp->updateBodyCachePolicy();
        }
    }
}


void
NsOpStore::_emitFeedSelectionChanged(const QString &inputLongName)
{
    NsInputPlugObject *ipo = _findPlug<NsInputPlugObject>(inputLongName);
    if (0 != ipo) {
        ipo->emitSelectionChanged();
    }
}

// -----------------------------------------------------------------------------

void
NsOpStore::_emitBodySelectionChanged(const QString &bodyLongName)
{
    NsBodyObject *body = _findBody(bodyLongName);
    if (0 != body) {
        body->emitSelectionChanged();
    }
}

// -----------------------------------------------------------------------------

//void
//NsOpStore::_emitValueChanged(const QString &valueLongName,
//                             const QString &expr,
//                             const int      comp)
//{
//    const NtString vln = fromQStr(valueLongName);
//    NsOpObject *op = _findOp(fromNbStr(vln.parent(".")));
//    if (0 != op) {
//        op->emitValueChanged(expr, comp);
//
//        NsValueBaseObject *vbo = 
//            op->queryMutableValue(fromNbStr(vln.child(".")));
//        if (0 != vbo) {
//            vbo->emitValueChanged(expr, comp);
//        }
//    }
//}
//
//void
//NsOpStore::_emitMetaChanged(const QString &longName,
//                            const QString &valueType,
//                            const QString &value)
//{
//    QStringList tokens = longName.split(".");
//
//    qDebug() << "Long name: " << longName;
//
//    //const NtString vln = fromQStr(longName);
//    //const QString opInstance = fromNbStr(vln.parent("."));
//
//    NsOpObject *op = queryMutableOp(tokens[0]);
//    if (0 != op) {
//        op->emitMetaChanged(valueType, value);
//    }
//}

//void
//NsOpStore::_emitProjectPathChanged(const QString &path)
//{}

// -----------------------------------------------------------------------------

void
NsOpStore::_emitBeginTimeStep(const NtTimeBundle &tb)
{}

void
NsOpStore::_emitEndTimeStep(const NtTimeBundle &tb)
{}

void
NsOpStore::_emitBeginOp(const NtTimeBundle &tb, const QString &opInstance)
{
    NsOpObject *op = _findOp(opInstance);
    if (0 != op) {
        op->emitStepBegin(tb);
    }
}

void
NsOpStore::_emitEndOp(const NtTimeBundle &tb, const QString &opInstance)
{
    NsOpObject *op = _findOp(opInstance);
    if (0 != op) {
        op->emitStepEnd(tb);
    }
}

void
NsOpStore::_reset()
{
    _resetOpConditions();
}

// -----------------------------------------------------------------------------

// _clear
// ------
//! Free memory and clear associations.

void
NsOpStore::_clear()
{
    qDebug() << "NsOpStore::_clear";

    // At this point all resource pointers to server objects are invalid.
    // Don't use them!

    foreach (NsOpObject *op, _opObjects) {
//        if (op != _globalOp) {
            delete op;
//        }
    }

    _opObjects.clear();
    _opInstances.clear();
    _opFamilies.clear();
    _opTypes.clear();
    _selectedOps.clear();
    _unselectedOps.clear();


    //_opObjects.insert(
    //    _opInstances.insert("Global", _globalOp->handle()).value(), _globalOp);
    _globalOp = _createOp("Global");
}


// _resetOpConditions
// ------------------
//! Reset all Op conditions.

void
NsOpStore::_resetOpConditions()
{
    foreach (NsOpObject *op, _opObjects) {
        op->setCondition(NsOpObject::None);
    }
}

// -----------------------------------------------------------------------------

// _findOp
// -------
//! Returns null if not found.

NsOpObject*
NsOpStore::_findOp(const QString &opInstance) const
{
    const _OpInstanceMapType::const_iterator iter = 
        _opInstances.find(opInstance);
    NsOpObject *op = 0;
    if (_opInstances.end() != iter) {
        op = _findOp(iter.value());
    }
    return op;
}


// _findOp
// -------
//! Returns null if not found.

NsOpObject*
NsOpStore::_findOp(const NsValueObjectHandle &handle) const
{ return _opObjects.value(handle, 0); }

// -----------------------------------------------------------------------------

// _queryMutablePlug
// -----------------
//! [static]

template<>
NsPlugObject* 
NsOpStore::_queryMutablePlug<NsPlugObject>(NsOpObject &op, 
                                           const QString &plugName)
{ return op.queryMutablePlug(plugName); }

template<>
NsInputPlugObject* 
NsOpStore::_queryMutablePlug<NsInputPlugObject>(NsOpObject &op,
                                                const QString &plugName)
{ return op.queryMutableInput(plugName); }

template<>
NsOutputPlugObject* 
NsOpStore::_queryMutablePlug<NsOutputPlugObject>(NsOpObject &op,
                                                 const QString &plugName)
{ return op.queryMutableOutput(plugName); }

template<>
NsBodyInputPlugObject* 
NsOpStore::_queryMutablePlug<NsBodyInputPlugObject>(NsOpObject &op,
                                                    const QString &plugName)
{ return op.queryMutableBodyInput(plugName); }

template<>
NsFieldInputPlugObject* 
NsOpStore::_queryMutablePlug<NsFieldInputPlugObject>(NsOpObject &op,
                                                     const QString &plugName)
{ return op.queryMutableFieldInput(plugName); }

template<>
NsBodyOutputPlugObject* 
NsOpStore::_queryMutablePlug<NsBodyOutputPlugObject>(NsOpObject &op,
                                                     const QString &plugName)
{ return op.queryMutableBodyOutput(plugName); }

template<>
NsFieldOutputPlugObject* 
NsOpStore::_queryMutablePlug<NsFieldOutputPlugObject>(NsOpObject &op,
                                                      const QString &plugName)
{ return op.queryMutableFieldOutput(plugName); }


// _splitPlugLongName
// ------------------
//! [static]

void
NsOpStore::_splitPlugLongName(const QString &plugLongName, 
                              QString       &opInstance, 
                              QString       &plugName)
{
    opInstance = queryPlugOpInstance(plugLongName, &plugName);
}

// -----------------------------------------------------------------------------


NsBodyObject*
NsOpStore::_findBody(const QString &bodyLongName) const
{
    QString opInstance;
    QString plugName;
    QString bodyName;
    _splitBodyLongName(bodyLongName, opInstance, plugName, bodyName);
    NsBodyObject *body = 0;
    if (!plugName.isEmpty()) {
        // Body lives in a plug.

        NsBodyOutputPlugObject *bopo = 
            _findPlug<NsBodyOutputPlugObject>(plugName);

        if (0 != bopo) {
            body = bopo->mutableLiveBodyCache().queryMutableBody(bodyName);
        }
    }
    else {
        NsOpObject *op = _findOp(opInstance);
        if (0 != op) {
            body = op->mutableEmpBodyCache().queryMutableBody(bodyName);
        }
    }
    return body;
}


// _splitBodyLongName
// ------------------
//! [static]

void
NsOpStore::_splitBodyLongName(const QString &bodyLongName,
                              QString       &opInstance,
                              QString       &plugName,
                              QString       &bodyName)
{
    _splitPlugLongName(
        queryPlugLongName(bodyLongName, &bodyName), 
        opInstance, 
        plugName);
}



// -----------------------------------------------------------------------------

// NsOpStore
// ---------
//! CTOR.

NsOpStore::NsOpStore()
    : QObject()
    , _globalOp(_createOp("Global"))
{}


// ~NsOpStore
// ----------
//! DTOR.

NsOpStore::~NsOpStore()
{
    //_clear();
}

// -----------------------------------------------------------------------------


