// -----------------------------------------------------------------------------
//
// NsGraphCallback.cc
//
// Naiad Studio graph callback, source file.
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
// -----------------------------------------------------------------------------

#include "NsGraphCallback.h"
#include "NsCmdSetMeta.h"
#include "NsCmdSelectAllBodies.h"
#include "NsCmdSetCurrentVisibleFrame.h"
#include "NsStringUtils.h"
#include <Ni.h>
#include <QMessageBox>
#include <QApplication>
#include <QDebug>

// -----------------------------------------------------------------------------

// instance
// --------
//! Provide access to singleton. [static]

NsGraphCallback *
NsGraphCallback::instance()
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
NsGraphCallback::createInstance()
{
    destroyInstance();
    _instance = new NsGraphCallback;
}


// destroyInstance
// ---------------
//! Reset singleton. [static]

void
NsGraphCallback::destroyInstance()
{
    delete _instance;
    _instance = 0;
}


//! Singleton pointer. [static]
NsGraphCallback *NsGraphCallback::_instance = 0;

// -----------------------------------------------------------------------------

// NsGraphCallback
// ---------------
//! CTOR.

NsGraphCallback::NsGraphCallback()
    : _cb(*this)
    , _steppingStopped(false)
{}

// -----------------------------------------------------------------------------

// _Callback
// ---------
//! CTOR. Make sure this is called after NiBegin.

NsGraphCallback::_Callback::_Callback(NsGraphCallback &gcb)
    : NtGraphCallback()
    , _gcb(&gcb)
{
    NiRegisterGraphCallback(this);
}


// beginStep
// ----------
//! Called when step begins.

bool
NsGraphCallback::_Callback::beginStep(const NtTimeBundle &tb)
{
    NsCmdSelectAllBodies::exec(false);
    _gcb->emitBeginStep(tb);
    _wakeGui();
    return !_gcb->steppingStopped();
}


// beginFrame
// ----------
//! Called when solve of the given frame begins.

bool
NsGraphCallback::_Callback::beginFrame(const NtTimeBundle &tb)
{
    NsCmdSelectAllBodies::exec(false);
    _gcb->emitBeginFrame(tb);

    if (!_gcb->steppingStopped()) {
        NsCmdSetCurrentVisibleFrame::exec(tb.frame, false);
    }

    _wakeGui();

    return !_gcb->steppingStopped();
}


// beginTimestep
// -------------
//! Called when solve of the given timestep begins.

bool
NsGraphCallback::_Callback::beginTimestep(const NtTimeBundle &tb)
{
    _gcb->emitBeginTimeStep(tb);
    _wakeGui();
    return !_gcb->steppingStopped();
}


// endTimestep
// -----------
//! Called when solve of the given timestep is completed.

bool
NsGraphCallback::_Callback::endTimestep(const NtTimeBundle &tb)
{
    _gcb->emitEndTimestep(tb);
    _wakeGui();
    return !_gcb->steppingStopped();
}


// endFrame
// --------
//! Called when solve of the given frame is completed.

bool
NsGraphCallback::_Callback::endFrame(const NtTimeBundle &tb)
{
    _gcb->emitEndFrame(tb);
    _wakeGui();
    return !_gcb->steppingStopped();
}


// endStep
// -------
//! Called when a step has been completed.

bool
NsGraphCallback::_Callback::endStep(const NtTimeBundle &tb)
{
    _gcb->emitEndStep(tb);
    _wakeGui();
    return !_gcb->steppingStopped();
}


// reset
// -----
//! Called to query if it's OK to reset the solve. If 'false' is returned the
//! solve will not be reset.

bool
NsGraphCallback::_Callback::reset()
{
    const bool ok(
        QMessageBox::Yes ==
            QMessageBox::warning(
                0,
                QObject::tr("Warning"),
                QObject::tr("This will reset the graph. Are you sure?"),
                QMessageBox::Yes | QMessageBox::No));

    if (ok) {
        _gcb->emitReset();
    }

    return ok;
}

// -----------------------------------------------------------------------------

// beginOp
// --------
//! Called when solve of the Op with the given name begins.

void
NsGraphCallback::_Callback::beginOp(const NtTimeBundle &tb,
                                    const NtString     &opInstance)
{
    _gcb->emitBeginOp(tb, fromNbStr(opInstance));
    _wakeGui();
}


// endOp
// ------
//! Called when stepping of the Op with the given name is completed.

void
NsGraphCallback::_Callback::endOp(const NtTimeBundle &tb,
                                  const NtString     &opInstance)
{
    _gcb->emitEndOp(tb, fromNbStr(opInstance));
    _wakeGui();
}

// -----------------------------------------------------------------------------

void
NsGraphCallback::_Callback::_wakeGui()
{
#if 0
    // Flushes the platform specific event queues.
    // If you are doing graphical changes inside a loop that does not return to 
    // the event loop on asynchronous window systems like X11 or double buffered 
    // window systems like Mac OS X, and you want to visualize these changes 
    // immediately (e.g. Splash Screens), call this function.

    QApplication::flush();
#endif

#if 0
    // Immediately dispatches all events which have been previously queued with 
    // QCoreApplication::postEvent() and which are for the object receiver and 
    // have the event type event_type. Events from the window system are not 
    // dispatched by this function, but by processEvents().
    // If receiver is null, the events of event_type are sent for all objects. 
    // If event_type is 0, all the events are sent for receiver.

    QObject *receiver(0);   // Null.
    int eventType(0);
    QApplication::sendPostedEvents(receiver, eventType);
#endif

    // Processes all pending events for the calling thread according to the 
    // specified flags until there are no more events to process.
    // You can call this function occasionally when your program is busy 
    // performing a long operation (e.g. copying a file).

    QApplication::processEvents(QEventLoop::AllEvents);
}


/*
    // cache the bodies for Body Plugs and fields for Field Plugs

#if 0
    // clamp time to a whole frame (so its visible in nstudio)
    const int cacheFrame=
        ::floor((tb.time+tb.dt*0.1)*Ns::framesPerSecond());

    const double cacheTime=
        Ns::time(cacheFrame);
#endif

    const int cacheFrame  = tb.frame;

    // BodyOp: cache the output bodies
    try {
        Ng::Op* op = Ng::Store::opInstance(name);    // NiQueryConstOp
        int outputCount = op->outputCount();
        for (int o = 0; o < outputCount; ++o) {

            // collect a list of DummyOps connect to either an input or
            // output on this op.
            em::array1<Ng::Op*> dummyOps;

            Ng::Output* output = op->output(o);
            for (int i = 0; i < output->dummyInputCount(); ++i)
                dummyOps.push_back(output->dummyInput(i)->op());

            int inputCount = output->downstreamInputCount();
            for (int i = 0; i < inputCount; ++i) {
                Ng::Input* input=output->downstreamInput(i);
                for (int j = 0; j < input->dummyInputCount(); ++j)
                    dummyOps.push_back(input->dummyInput(j)->op());
            }

            // caching the "full" plug data means not just the body names
            // and signatures and other "high level info", but actually
            // caching the shapes, channels, etc - the "full" body data.
            const bool cacheFullPlugData(dummyOps.size()>0);

            // body output? then cache the bodies...
            Ng::BodyOutput* bodyOutput=dynamic_cast<Ng::BodyOutput*>(output);
            if(bodyOutput) {
                // collect the bodies from the output
                QList<const Nb::Body *> bodies;
                const Ng::BodyPlugData *bpd = bodyOutput->bodyPlugData(tb);
                const Ng::BodySet &bodySet = bpd->bodySet();
                for(int b = 0; b < bodySet.size(); ++b) {
                    Nb::Body* body=bodySet.constBody(b)->unsolvedClone();
                    body->prop1e("Motion Type")->setExpr(
                        bodySet.constBody(b)->prop1e("Motion Type")->expr()
                        );
                    bodies += body;
                }

                // store plug data for this frame in the BodyCache

                NsSolveCache::createLiveBodyCache(
                    fromNbStr(name),
                    fromNbStr(output->name()),
                    bodies,
                    cacheFullPlugData,
                    cacheFrame
                    );

                // clean up...
                if(!cacheFullPlugData) {
                    foreach (const Nb::Body *body, bodies)
                        delete body;
                }
                continue;
            }

            // don't cache fields unless we have scopes attached...
            if(!cacheFullPlugData)
                continue;

            // field output? then sample & cache the field...
            Ng::FieldOutput* fieldOutput=dynamic_cast<Ng::FieldOutput*>(output);
            if(fieldOutput) {
                // construct a "virtual" FieldFeed to sample the FieldOp
                Ng::FieldFeed feed(
                    op->fieldPlugData(fieldOutput->name(),tb),
                    NtVec3f(0)
                    );

                // find the smallest Interactive Cell Scale among the attached
                // field-scopes
                bool found=false;
                float minInterCs=1e6;
                for(int i=0; i<dummyOps.size(); ++i) {
                    if(!dummyOps(i)->hasParam("Interactive Cell Scale"))
                        continue;
                    found=true;
                    const Nb::Value1f* interCs=
                        dummyOps(i)->param1f("Interactive Cell Scale");
                    float ics=interCs->eval(Nb::ZeroTimeBundle);
                    if(ics<=0) {
//                        Ns::warning(
//                            "Interactive Cell Scale too low field-scope: "+
//                            QString(dummyOps(i)->name().c_str())
//                            );
                        EM_ASSERT(false);
                        ics=2.f;
                    }
                    minInterCs=em::min(minInterCs,ics);
                }
                if(!found)
                    minInterCs=1; // just keep it at current MCS, if none found

                // temp. disable the callbacks so we don't recurse back into
                // this function when we call "computeLayout"
                //Ns::disableGraphCallback();
                EM_ASSERT(false);
                const Nb::TileLayout* layout=feed.computeLayout3f(tb);
                //Ns::enableGraphCallback();

                // create the tile-layout which is shared by all the scopes
                Nb::TileLayout* fldLayout=new Nb::TileLayout();
                if(layout) {
                    fldLayout->init(
                        layout->cellSize()*minInterCs,layout->tileSize()
                        );
                    fldLayout->refine(*layout);
                } else {
                    fldLayout->init(
                        NiEvalParam1f("Global.Master Cell Size")*minInterCs,5
                        );
                    for(int i=0; i<dummyOps.size(); ++i) {
                        //NtVec3f xmin, xmax;
                        em::glmat44f clipXform;
                        em::glmat44f invClipXform;
                        computeClipBoxXforms(
                            dummyOps(i)->param3f("Translate"),
                            dummyOps(i)->param3f("Rotate"),
                            dummyOps(i)->param3f("Scale"),
                            clipXform,
                            invClipXform
                            );

                        const NtVec3f xmin(clipXform*NtVec3f(-1,-1,-1));
                        const NtVec3f xmax(clipXform*NtVec3f( 1, 1, 1));

                        fldLayout->worldBoxRefine(xmin, xmax);
                    }
                }

                // create a field using the layout, and sample the field feed
                Ns3DField* field=new Ns3DField(name,fldLayout);

                for(int i=0; i<3; ++i)
                    Ng::sampleFieldFeed(
                        &feed,*fldLayout,field->mutableNbField("",i),tb,i
                        );

                // cache the field
                NsSolveCache::createLiveFieldCache(
                    fromNbStr(name),
                    fromNbStr(output->name()),
                    field,
                    cacheFrame
                    );
            }
        }

        // update the graph view to turn off the solving color
//        NsGraph* graph(Ns::graph());
//        NsOpItem *item = graph->findOp(fromNbStr(name));
//        if(item->condition()==NsOpItem::Stepping)
//            item->setCondition(NsOpItem::None);

//        Ns::ns()->emitEndOp();

//        QApplication::processEvents();
//        QApplication::processEvents();
    }
    catch (std::exception &exception) {
//        Ns::internalError
//            (QObject::tr("Failed to get bodies from solved Op: ") +
//             exception.what());
    }
*/
