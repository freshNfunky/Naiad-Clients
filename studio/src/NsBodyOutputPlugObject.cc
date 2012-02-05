// -----------------------------------------------------------------------------
//
// NsBodyOutputPlugObject.cc
//
// Naiad Studio class for representing a Ng::BodyOutput, source file.
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

#include "NsBodyOutputPlugObject.h"
#include "NsBodyInputPlugObject.h"
#include "NsOpStore.h"
#include "NsOpObject.h"
#include "NsBodyObject.h"
#include "NsQuery.h"
#include "NsStringUtils.h"
#include <NiNb.h>
#include <NgInput.h>
#include <NgBodyOp.h>
#include <NgBodyPlugData.h>
#include <QDebug>

// -----------------------------------------------------------------------------

// NsBodyOutputPlugObject
// ----------------------
//! CTOR.

NsBodyOutputPlugObject::NsBodyOutputPlugObject(Ng::BodyOutput &bodyOutput,
                                               NsOpObject     &parent)
    : NsOutputPlugObject(bodyOutput, parent)
    , _bodyOutput(&bodyOutput)
{}

// -----------------------------------------------------------------------------

void
NsBodyOutputPlugObject::emitLiveBodyCacheChanged()
{
    emit liveBodyCacheChanged();

    foreach (Ng::Input *input, downstreamInputs()) {
        NsBodyInputPlugObject *bipo =
            NsOpStore::instance()->queryMutableBodyInput(
                fromNbStr(input->longname()));
        if (0 != bipo) {
            bipo->emitLiveBodyCacheChanged();
        }
    }
}

// -----------------------------------------------------------------------------

void
NsBodyOutputPlugObject::groupPlugChanged()
{
    qDebug() << "NsBodyOutputPlugObject::groupPlugChanged";
    NsOutputPlugObject::groupPlugChanged();   // Parent method.
    _bodyOutput = 
        mutableOp()->mutableNgOp().bodyOutput(fromQStr(name())); // Update.
}

//// updateCache
//// -----------
////! TODO

//void
//NsBodyOutputPlugObject::updateCache(const NtTimeBundle &tb)
//{
//    qDebug() << "NsBodyOutputPlugObject::updateCache";

//    const NtTimeBundle cstb(NiQueryTimeBundle());    // Current server time.

//    qDebug() << "Server:" << cstb.frame << "|" << cstb.timestep;
//    qDebug() << "Client:" << tb.frame << "|" << tb.timestep;

//    if (_isFrameLive(tb)) {
//        // Cache update is happening at the current server time, try
//        // to load live bodies.

//        _updateLiveCache(cstb);
//        emit liveBodyCacheChanged();
//    }
//    else if ("body-output" == name() || 1 == op().outputCount()) {
//        // there is only one output plug for this Op.

//        _updateEmpCache(tb);
//        emit empBodyCacheChanged();
//    }
//}

// -----------------------------------------------------------------------------

//// onFeedChanged
//// -------------
////! DOCS. [slot]
//
//void
//NsBodyOutputPlugObject::onFeedChanged(const QString &inputLongName,
//                                      const QString &plugLongName,
//                                      const  bool    success)
//{
//    //if (success && 
//    //    (longName() == inputLongName || 
//    //     longName() == plugLongName)) {
//    //    computeLiveBodyCache();        
//    //}
//}


// onBeginOp
// ---------
//! DOCS [slot]

void
NsBodyOutputPlugObject::onBeginOp(const NtTimeBundle &tb)
{}


// onEndOp
// -------
//! DOCS [slot]

void
NsBodyOutputPlugObject::onEndOp(const NtTimeBundle &tb)
{

    int dsic = 0;
    int dic = 0;
    _feedConfig(dsic, dic);
    const _BodyCachePolicy bcp = _bodyCachePolicy(dsic, dic);

    switch (bcp) {
    default:
    case NoCache:   
        // Do nothing.
        break;
    case StatCache:
    case FullCache:
        _updateLiveBodyCache(bcp);
        break;
    }
}

// -----------------------------------------------------------------------------

// _feedConfig
// -----------
//! DOCS

void
NsBodyOutputPlugObject::_feedConfig(int &downstreamInputCount, 
                                    int &dummyInputCount) const
{
    downstreamInputCount = _bodyOutput->downstreamInputCount();
    dummyInputCount = _bodyOutput->dummyInputCount();

    // Count dummy inputs on downstream inputs.

    for (int i = 0; i < _bodyOutput->downstreamInputCount(); ++i) {
        dummyInputCount += _bodyOutput->downstreamInput(i)->dummyInputCount();
    }
}


// _bodyCachePolicy
// ----------------
//! DOCS [static]

NsBodyOutputPlugObject::_BodyCachePolicy
NsBodyOutputPlugObject::_bodyCachePolicy(const int downstreamInputCount, 
                                         const int dummyInputCount)
{
    return (0 < dummyInputCount ? FullCache : 
           (0 < downstreamInputCount ? StatCache : NoCache));
}


// _updateBodyCachePolicy
// ----------------------
//! DOCS

void
NsBodyOutputPlugObject::_updateBodyCachePolicy()
{
    int dsic = 0;
    int dic = 0;
    _feedConfig(dsic, dic);
    const _BodyCachePolicy bcp = _bodyCachePolicy(dsic, dic);

    switch (bcp) {
    default:
    case NoCache:   
        // Do nothing.
        break;
    case StatCache:
    case FullCache:
        _updateLiveBodyCache(bcp);
        break;
    }
}


// _updateLiveBodyCache
// --------------------
//! DOCS

void
NsBodyOutputPlugObject::_updateLiveBodyCache(const _BodyCachePolicy bcp)
{
    typedef std::vector<Nb::Body*> BodyVectorType;
    typedef BodyVectorType::const_iterator BodyIterType;

    _liveBodyCache.clear(); // Clear the current cache no matter what.
    const BodyVectorType bodies = NiCloneLiveBodies(fromQStr(longName()));
    const BodyIterType iend = bodies.end();
    for (BodyIterType iter = bodies.begin(); iter != iend; ++iter) {
        NsBodyObject *body =
            _liveBodyCache.addBody(*(*iter), *mutableOp(), this, true);

        switch (bcp) {
        case StatCache:
            body->eraseShapes();
            break;
        case FullCache: 
            body->init3DBody();
            break;
        }
    }

    emitLiveBodyCacheChanged();
}





// -----------------------------------------------------------------------------

//void
//NsBodyOutputPlugObject::computeLiveBodyCache()
//{
/*
    if (_liveBodyCache.empty()) {
        // The body cache is empty. and
        
        if (0 < _bodyOutput->downstreamInputCount()) {
            // There are feeds connected to this plug.
            
            _updateLiveBodyCache();
        }
    } else {
*/
        // The body cache is not empty, but the feed change may require
        // the body cache mode to be updated.
        
        //const int dfc(queryDummyFeedCount(longName()));
        //const NsBodyCache::CacheMode liveCacheMode(
        //    _liveBodyCache.cacheMode());
        //        
        //if (dfc > 0 && NsBodyCache::StatCache == liveCacheMode) {
        //    // There are dummy feeds, so we need to switch to full caching.
        //    // Switching cache mode will clear the cache, so we must update
        //    // the cache afterward.
        //    
        //    _liveBodyCache.setCacheMode(NsBodyCache::FullCache);
        //    _updateLiveBodyCache();

        //} else if (dfc <= 0 && NsBodyCache::FullCache == liveCacheMode) {
        //    // There are no dummy feeds, so we need to switch 
        //    // to stat caching.
        //    
        //    _liveBodyCache.setCacheMode(NsBodyCache::StatCache);
        //        _updateLiveBodyCache();
        //}

//    }
//}

// -----------------------------------------------------------------------------
