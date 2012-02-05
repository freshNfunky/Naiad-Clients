// -----------------------------------------------------------------------------
//
// NsOpObject.cc
//
// Naiad Studio class for representing an Op, source file.
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

#include "NsOpObject.h"
#include "NsOpStore.h"
#include "NsBodyObject.h"
#include "NsValueSectionObject.h"
#include "NsValueBaseObject.h"
#include "NsGraphCallback.h"
#include "NsQuery.h"
#include "NsStringUtils.h"
#include "NsCmdCentral.h"
#include "NsCmdSetOpState.h"
#include "NsBodyInputPlugObject.h"
#include "NsFieldInputPlugObject.h"
#include "NsBodyOutputPlugObject.h"
#include "NsFieldOutputPlugObject.h"
#include <NiNb.h>
#include <NiQuery.h>
#include <NgBodyOp.h>
#include <NgProjectPath.h>
#include <NbEmpSequenceReader.h>
//#include <NgDummyBodyOp.h>
//#include <NgDummyFieldOp.h>
#include <QDebug>
#include <vector>

// -----------------------------------------------------------------------------

// ~NsOpObject
// -----------
//! DTOR.

NsOpObject::~NsOpObject()
{
    qDebug() << "~NsOpObject";

    emit opObjectDestroyed(this);
}


// valueObject
// -----------
//! Returns a (const) reference to the underlying Ng::Op.

const Ng::Op&
NsOpObject::constNbValueObject() const
{
    return *_op;
}


Ng::Op&
NsOpObject::mutableNbValueObject()
{
    return *_op;
}

// -----------------------------------------------------------------------------

// NsOpObject
// ----------
//! CTOR.

NsOpObject::NsOpObject(Ng::Op                    &op,
                       QObject                   *parent)
    : NsValueObject(op, parent)
    , _op(&op)
    , _condition(None)
    , _cachePolicy(NoCache)
    , _hasEmpCacheParam(_op->hasParam("EMP Cache") ||
                        _op->hasParam("Geometry Cache") ||
                        _op->hasParam("Particle Cache"))
    , _hasEnabledParam(_op->hasParam("Enabled"))
    , _familyName(queryOpInstanceFamilyName(fromNbStr(op.longname())))
    , _typeName(queryOpTypeName(longName()))
    //, _nelContext(&op, 0, "")
{
    setObjectName(fromNbStr(op.longname()));

    _createPlugs();
}


// state
// -----
//! Return state as a string.

QString
NsOpObject::state() const
{
    //return QString("INACTIVE");
    return queryOpState(longName());
}


bool
NsOpObject::hasParam(const QString &paramName) const
{
    return _op->hasParam(fromQStr(paramName));
}


//// typeName
//// ----------
////! Returns the Op type name.
//
//QString
//NsOpObject::typeName() const
//{
//    return _typeName;
//    //return queryOpTypeName(longName());
//}
//
//
//// familyName
//// ----------
////! Returns the Op family name.
//
//QString
//NsOpObject::familyName() const
//{
//    return _familyName;
//    //return queryOpInstanceFamilyName(longName());
//}


// categoryNames
// -------------
//! Returns a list of the categories to which the Op type belongs.

QStringList
NsOpObject::categoryNames() const
{
    return queryOpInstanceCategories(longName());
}


NsOpObject::Condition
NsOpObject::condition() const
{
    return _condition;
}


void
NsOpObject::setCondition(Condition cond)
{
    _setCondition(cond);
}

// -----------------------------------------------------------------------------

// emitStepBegin
// -------------
//! Handle graph callback.

void
NsOpObject::emitStepBegin(const NtTimeBundle &tb)
{
    emit stepBegin(tb);
    _setCondition(Stepping);
}


// emitStepEnd
// -----------
//! Handle graph callback.

void
NsOpObject::emitStepEnd(const NtTimeBundle &tb)
{
    emit stepEnd(tb);

    if (Stepping == condition()) {
        _setCondition(None);
    }
}

// -----------------------------------------------------------------------------

//void
//NsOpObject::bindNelContext(const NtTimeBundle &tb) const
//{
    //Ng::NelContext nelContext(_op, 0, "");
    //ngOp().bindNelContext(&_nelContext, tb);
//}

//void
//NsOpObject::unbindNelContext() const
//{
    //ngOp().unbindNelContext();
//}

// -----------------------------------------------------------------------------

bool
NsOpObject::isEnabled() const
{
    bool enabled(true);
    //const NsValueSectionObject *vso(queryConstSection("Stepping"));

    if (_hasEnabledParam) {
        const NsValueBaseObject *vbo(queryConstValue("Enabled"));

        // TODO: bindNelContext(tb);

        const Nb::Value<int,1> *val1i(
            static_cast<const Nb::Value<int,1>*>(
                &vbo->nbValueBase()));

        enabled = (0 != val1i->eval(queryCurrentVisibleFrameTimeBundle()));
    }

    return enabled; // Assume true if no enabled parameter.
}


//QList<const NsBodyObject*>
//NsOpObject::constBodies() const
//{
//    return _empBodyCache.constBodies();
//}
//
//
//QList<NsBodyObject*>
//NsOpObject::mutableBodies() const
//{
//    return _empBodyCache.mutableBodies();
//}

// -----------------------------------------------------------------------------

//// onOpNameChanged
//// ---------------
////! Update cached name if it corresponds to the Op name that was changed. [slot]
//
//void
//NsOpObject::onOpNameChanged(const QString &oldOpInstance,
//                            const QString &newOpInstance,
//                            const bool     success)
//{
//    if (success && longName() == newOpInstance) {
//        // NB: Must check long name against new Op instance because
//        // the change has already been made on the server.
//
//        setObjectName(newOpInstance);
//        emit nameChanged(oldOpInstance, newOpInstance);
//    }
//}


//// onOpStateChanged
//// ----------------
////! Update cached state if the change corresponds to
////! the long name of this Op. [slot]
//
//void
//NsOpObject::onOpStateChanged(const QString &opInstance,
//                             const QString &opState,
//                             const bool     success)
//{
//    if (success) {
//        const NsOpObject *op(
//            NsOpStore::instance()->queryConstOp(opInstance));
//
//        if (0 != op && op != this &&
//            "ACTIVE" == opState &&
//            "CAMERA_SCOPE" == familyName() &&
//            "CAMERA_SCOPE" == op->familyName()) {
//            // Another camera scope state was set to ACTIVE, deactivate this
//            // camera scope.
//
//            //NsCmdSetOpState::exec(longName(), "INACTIVE");
//            NiSetOpState(_op->longname(), "INACTIVE");
//        }
//        else {
//            if (longName() == opInstance) {
//                emit stateChanged(opState);
//            }
//            else {
//                emit stateChanged(state());
//            }
//        }
//    }
//}


//void
//NsOpObject::onFeedChanged(const QString &inputLongName,
//                          const QString &plugLongName,
//                          const bool     success)
//{
//    if (success) {
//        // Any feed change in the graph could possibly affect the
//        // state of any Op.
//
//        emit stateChanged(state());
//
//        if (_hasEmpCacheParam && !plugLongName.isEmpty()) {
//            // This Op has an EMP cache. Also, body caching is not affected
//            // by removed feeds, so we check to see if the feed change was 
//            // actually a feed removal, in which case we do nothing.            
//
//            const QString inputOpInstance(
//                fromNbStr(fromQStr(inputLongName).parent(":")));
//            const QString plugOpInstance(
//                fromNbStr(fromQStr(plugLongName).parent(":")));
//
//            if (longName() == inputOpInstance ||
//                longName() == plugOpInstance) {
//                // A new feed has been added to the graph and one of the plugs
//                // involved belongs to this Op. Note that adding a feed to an
//                // Op only triggers a body cache update IF the feed requires
//                // the cache policy to change.
//
//                int dsic(0);
//                int dic(0);
//                _feedConfig(dsic, dic);
//                const _BodyCachePolicy newCachePolicy(
//                    _bodyCachePolicy(dsic, dic));
//    
//                qDebug() << "NsOpObject::onFeedChanged -"
//                         << _cachePolicy << " :" << newCachePolicy;
//
//                if (_cachePolicy != newCachePolicy) {
//                    // Switching cache policy.
//
//                    _cachePolicy = newCachePolicy;
//                    
//                    switch (_cachePolicy) {
//                    default:
//                    case NoCache:
//                        // Note that we don't clear the cache if the new cache policy
//                        // is NoCache. 
//                        break;
//                    case StatCache:
//                    case FullCache:
//                        _updateEmpBodyCache(queryCurrentVisibleFrameTimeBundle());
//                        break;
//                    }
//                }
//    
//                
//                //// Compute the current feed configuration for this Op. We need
//                //// to count the total number of regular and dummy feeds 
//                //// connected to the plugs of this Op. The feed configuration
//                //// determines which cache mode to use.
//
//                //int dic;
//                //int dsic;
//                //_feedCount(dsic, dic);
//
//                //// The body cache is not empty, but we may need to
//                //// rebuild the cache if the cache mode needs to be changed.
//
//                //const NsBodyCache::CacheMode empCacheMode(
//                //    _empBodyCache.cacheMode());
//
//                //if (0 < dic && NsBodyCache::StatCache==empCacheMode) {
//                //    // There are dummy feeds, so we need to switch to 
//                //    // full caching. Switching cache mode will clear the 
//                //    // cache, so we must update the cache afterward.
//                //    
//                //    _empBodyCache.setCacheMode(NsBodyCache::FullCache);
//                //    _updateEmpBodyCache(queryCurrentVisibleFrameTimeBundle());
//                //} 
//                ////else if (dic <= 0 && NsBodyCache::FullCache==empCacheMode) {
//                ////    // There are no dummy feeds, so we need to switch to 
//                ////    // stat caching.
//                ////    
//                ////    _empBodyCache.setCacheMode(NsBodyCache::StatCache);
//                ////    _updateEmpBodyCache(queryCurrentVisibleFrameTimeBundle());
//                ////}
//            }
//        }
//    }
//}

void
NsOpObject::updateBodyCachePolicy()
{
    if (_hasEmpCacheParam) {
        // A new feed has been added to the graph and one of the plugs
        // involved belongs to this Op. Note that adding a feed to an
        // Op only triggers a body cache update IF the feed requires
        // the cache policy to change.

        int dsic = 0;
        int dic = 0;
        _feedConfig(dsic, dic);
        const _BodyCachePolicy newCachePolicy = _bodyCachePolicy(dsic, dic);
    
        if (_cachePolicy != newCachePolicy) {
            // Switching cache policy.

            _cachePolicy = newCachePolicy;
                    
            switch (_cachePolicy) {
            default:
            case NoCache:
                // Note that we don't clear the cache if the new cache policy
                // is NoCache. 
                break;
            case StatCache:
            case FullCache:
                _updateEmpBodyCache(queryCurrentVisibleFrameTimeBundle());
                break;
            }
        }
    }
}


void
NsOpObject::updateEmpBodyCache()
{
    if (_hasEmpCacheParam) {
        const NtTimeBundle cvftb = queryCurrentVisibleFrameTimeBundle();
        NtBool ok = NI_TRUE;
        if (NiInitCachedBodies(fromQStr(longName()), cvftb, NI_TRUE, &ok)) {
            _updateEmpBodyCache(cvftb);
        }
        else if (!ok) {
            _empBodyCache.clear();
            emit empBodyCacheChanged();
        }
    }
}


// onValueChanged
// --------------
//! [slot]

void
NsOpObject::onValueChanged(const QString &valueLongName,
                           const QString &expr,
                           int            comp,
                           bool           success)
{    
    if (success &&         
        NiQueryParamAffectsOp(fromQStr(valueLongName), 
                              fromQStr(longName()))) {
        emit enabledChanged(isEnabled());

        // Changing a value does not affect an Op's feed configuration,
        // which means that the cache mode remains constant. We simply check if 
        // the Op has an EMP cache and if this cache needs to be updated. 

        if (_hasEmpCacheParam) {
            //qDebug() << "NsOpObject::onValueChanged";
            //qDebug() << valueLongName << ":" << expr;
            //qDebug() << NiInitCachedBodies(fromQStr(longName()), cvftb);
        
            switch (_cachePolicy) {
            default:
            case NoCache:   // Do nothing.
                break;
            case StatCache:
            case FullCache:
                const NtTimeBundle cvftb(queryCurrentVisibleFrameTimeBundle());
                bool ok(true);
                const bool update(initCachedBodies(longName(),cvftb,true,&ok));

                qDebug() << "NsOpObject::onValueChanged - "
                         << "Update: " << update << "| Ok: " << ok;

                if (update) {
                    _updateEmpBodyCache(cvftb);
                } 
                else if (!ok) {
                    _empBodyCache.clear();
                    emit empBodyCacheChanged(); 
                }
                break;
            }
        }
    }
}


// onProjectPathChanged
// --------------------
//! [slot]

void
NsOpObject::onProjectPathChanged(const QString &path,
                                 const bool     success)
{
    if (success) {
        // clear all conditions first
        _setCondition(None);

        // Changing the project path does not affect an Op's feed configuration,
        // which means that the cache mode remains constant. We simply check if 
        // the Op has an EMP cache and if this cache needs to be updated. 

        if (_hasEmpCacheParam) {
            switch (_cachePolicy) {
            default:
            case NoCache:   // Do nothing.
                break;
            case StatCache:
            case FullCache:
                const NtTimeBundle cvftb(queryCurrentVisibleFrameTimeBundle());
                bool ok(true);
                const bool update(initCachedBodies(longName(),cvftb,true,&ok));

                qDebug() << "NsOpObject::onProjectPathChanged - "
                         << "Update: " << update << "| Ok: " << ok;

                if (update) {
                    _updateEmpBodyCache(cvftb);
                } 
                else if (!ok) {
                    _empBodyCache.clear();
                    emit empBodyCacheChanged();
                }
                break;
            }
        }
    }
}


void
NsOpObject::onCurrentVisibleFrameChanged(const int  cvf,    
                                         const bool update3DView,
                                         const bool success)
{
    Q_UNUSED(update3DView);

    if (success) {
        emit enabledChanged(isEnabled());

        // "Scrubbing" does not affect an Op's feed configuration, which means 
        // that the cache mode remains contant. We simply check if the Op has
        // an EMP cache and if this cache needs to be updated.

        if (_hasEmpCacheParam) {
            //qDebug() << "NsOpObject::onCurrentVisibleFrameChanged";
            //qDebug() << cvf;
            //qDebug() << NiInitCachedBodies(fromQStr(longName()), cvftb);

            switch (_cachePolicy) {
            default:
            case NoCache:   // Do nothing.
                break;
            case StatCache:
            case FullCache:
                const NtTimeBundle cvftb(queryCurrentVisibleFrameTimeBundle());
                bool ok(true);
                const bool update(initCachedBodies(longName(),cvftb,true,&ok));

                qDebug() << "NsOpObject::onCurrentVisibleFrameChanged - "
                         << "Update: " << update << "| Ok: " << ok;

                if (update) {
                    _updateEmpBodyCache(cvftb);
                }
                else if (!ok) {
                    _empBodyCache.clear();
                    emit empBodyCacheChanged();
                }
                break;
            }
        }
    }
}


//// onBeginOp
//// ---------
////! Handle graph callback.
//
//void
//NsOpObject::onBeginOp(const NtTimeBundle &tb, const QString &opInstance)
//{
//    if (longName() == opInstance) {
//        qDebug() << "NsOpObject::onBeginOp";
//        emit stepBegin(tb);
//        _setCondition(Stepping);
//    }
//}
//
//
//// onEndOp
//// -------
////! Handle graph callback.
//
//void
//NsOpObject::onEndOp(const NtTimeBundle &tb, const QString &opInstance)
//{
//    if (longName() == opInstance) {
//        qDebug() << "NsOpObject::onEndOp";
//        emit stepEnd(tb);
//
//        if (Stepping == condition()) {
//            _setCondition(None);
//        }
//    }
//}

// -----------------------------------------------------------------------------

// _createPlugs
// ------------
//! Create plug object children.

void
NsOpObject::_createPlugs()
{
    _destroyPlugs();

    // Create new plugs.

    // Body inputs.

    for (int in(0); in < _op->inputCount(); ++in) {
        try {
            Ng::BodyInput *bi(_op->bodyInput(in));  // May throw.

            NsBodyInputPlugObject *bipo(
                new NsBodyInputPlugObject(*bi, *this)); // Child.

            connect(NsCmdCentral::instance(),
                    SIGNAL(metaChanged(QString,QString,QString,bool)),
                    bipo,
                    SLOT(onMetaChanged(QString,QString,QString,bool)));

            const QString name = bipo->name();
            _plugs.insert(name, bipo);
            _inputs.insert(name, bipo);
            _bodyInputs.insert(name, bipo);
            _orderedPlugs.append(bipo);
            _orderedInputs.append(bipo);
            _orderedBodyInputs.append(bipo);
        }
        catch (...) {
            // Do nothing.
        }
    }

    // Field inputs.

    for (int in(0); in < _op->inputCount(); ++in) {
        try {
            Ng::FieldInput *fi(_op->fieldInput(in));  // May throw.

            NsFieldInputPlugObject *fipo(
                new NsFieldInputPlugObject(*fi, *this)); // Child.

            connect(NsCmdCentral::instance(),
                    SIGNAL(metaChanged(QString,QString,QString,bool)),
                    fipo,
                    SLOT(onMetaChanged(QString,QString,QString,bool)));

            const QString name = fipo->name();
            _plugs.insert(name, fipo);
            _inputs.insert(name, fipo);
            _fieldInputs.insert(name, fipo);
            _orderedPlugs.append(fipo);
            _orderedInputs.append(fipo);
            _orderedFieldInputs.append(fipo);
        }
        catch (...) {
            // Do nothing.
        }
    }

    // Body outputs.

    for (int out(0); out < _op->outputCount(); ++out) {
        try {
            Ng::BodyOutput *bo(_op->bodyOutput(out));  // May throw.

            NsBodyOutputPlugObject *bopo(
                new NsBodyOutputPlugObject(*bo, *this)); // Child.

            // Connect plug to Command Central.

            //connect(NsCmdCentral::instance(),
            //        SIGNAL(feedChanged(QString,QString,bool)),
            //        SLOT(onFeedChanged(QString,QString,bool)));

            connect(NsCmdCentral::instance(),
                    SIGNAL(metaChanged(QString,QString,QString,bool)),
                    bopo,
                    SLOT(onMetaChanged(QString,QString,QString,bool)));

            // Connect plug to parent Op.

            connect(this,
                    SIGNAL(stepBegin(NtTimeBundle)),
                    bopo,
                    SLOT(onBeginOp(NtTimeBundle)));

            connect(this,
                    SIGNAL(stepEnd(NtTimeBundle)),
                    bopo,
                    SLOT(onEndOp(NtTimeBundle)));

            const QString name = bopo->name();
            _plugs.insert(name, bopo);
            _outputs.insert(name, bopo);
            _bodyOutputs.insert(name, bopo);
            _orderedPlugs.append(bopo);
            _orderedOutputs.append(bopo);
            _orderedBodyOutputs.append(bopo);
        }
        catch (...) {
            // Do nothing.
        }
    }

    // Field outputs.

    for (int out(0); out < _op->outputCount(); ++out) {
        try {
            Ng::FieldOutput *fo(_op->fieldOutput(out)); // May throw.

            NsFieldOutputPlugObject *fopo(
                new NsFieldOutputPlugObject(*fo, *this));   // Child.

            connect(NsCmdCentral::instance(),
                    SIGNAL(metaChanged(QString,QString,QString,bool)),
                    fopo,
                    SLOT(onMetaChanged(QString,QString,QString,bool)));

            const QString name = fopo->name();
            _plugs.insert(name, fopo);
            _outputs.insert(name, fopo);
            _fieldOutputs.insert(name, fopo);
            _orderedPlugs.append(fopo);
            _orderedOutputs.append(fopo);
            _orderedFieldOutputs.append(fopo);
        }
        catch (...) {
            // Do nothing.
        }
    }
}


void
NsOpObject::_destroyPlugs()
{
    // Remove any existing plugs.

    foreach (NsPlugObject *po, _orderedPlugs) {
        delete po;
    }

    // Clear associations.

    _plugs.clear();
    _inputs.clear();
    _outputs.clear();
    _bodyInputs.clear();
    _fieldInputs.clear();
    _bodyOutputs.clear();
    _fieldOutputs.clear();

    _orderedPlugs.clear();
    _orderedInputs.clear();
    _orderedOutputs.clear();
    _orderedBodyInputs.clear();
    _orderedFieldInputs.clear();
    _orderedBodyOutputs.clear();
    _orderedFieldOutputs.clear();
}


// _setCondition
// -------------
//! DOCS

void
NsOpObject::_setCondition(Condition c)
{
    if (condition() != c) {
        _condition = c;
        emit conditionChanged(_condition);
    }
}

// -----------------------------------------------------------------------------

//bool
//NsOpObject::_hasEmpCache() const
//{
//    return _op->hasParam("EMP Cache");
//}


void
NsOpObject::_feedConfig(int &downstreamInputCount, int &dummyInputCount) const
{
    downstreamInputCount = 0;
    dummyInputCount = 0;

    foreach (const NsBodyInputPlugObject *bipo, _orderedBodyInputs) {
        dummyInputCount += bipo->constNgPlug()->dummyInputCount();
    }

    foreach (const NsFieldInputPlugObject *fipo, _orderedFieldInputs) {
        // TODO: Necessary?
    
        dummyInputCount += fipo->constNgPlug()->dummyInputCount();
    }

    foreach (const NsBodyOutputPlugObject *bopo, _orderedBodyOutputs) {
        dummyInputCount      += bopo->constNgPlug()->dummyInputCount();
        downstreamInputCount += bopo->constNgPlug()->downstreamInputCount();
    }

    foreach (const NsFieldOutputPlugObject *fopo, _orderedFieldOutputs) {
        // TODO: Necessary?

        dummyInputCount      += fopo->constNgPlug()->dummyInputCount();
        downstreamInputCount += fopo->constNgPlug()->downstreamInputCount();
    }
}


// _updateEmpBodyCache
// -------------------
//! DOCS

void
NsOpObject::_updateEmpBodyCache(const NtTimeBundle &tb)
{
    //typedef std::vector<Nb::Body*> BodyVectorType;
    //typedef BodyVectorType::const_iterator BodyIterType;

    qDebug() << "NsOpObject::_updateEmpBodyCache -"
             << longName() << ":" << _cachePolicy;

    _empBodyCache.clear();      // Clear existing bodies no matter what.

    const QList<Nb::Body*> bodies(queryCachedBodies(longName(), tb, true));

    foreach (Nb::Body *nbBody, bodies) {
        NsBodyObject *body(_empBodyCache.addBody(*nbBody, *this, 0, false));

        switch (_cachePolicy) {
        case StatCache:
            body->eraseShapes();
            break;
        case FullCache: 
            body->init3DBody();
            break;
        }
    }

    emit empBodyCacheChanged();
}

NsOpObject::_BodyCachePolicy
NsOpObject::_bodyCachePolicy(const int downstreamInputCount, 
                             const int dummyInputCount)
{
    return (0 < dummyInputCount ? FullCache : 
           (0 < downstreamInputCount ? StatCache : NoCache));
}
