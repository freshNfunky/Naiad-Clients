// -----------------------------------------------------------------------------
//
// NsBodyInputPlugObject.cc
//
// Naiad Studio class for representing a Ng::BodyInput, source file.
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

#include "NsBodyInputPlugObject.h"
#include "NsBodyOutputPlugObject.h"
#include "NsOpObject.h"
#include "NsOpStore.h"
#include "NsStringUtils.h"
#include "NsQuery.h"
#include "NsStringUtils.h"

// -----------------------------------------------------------------------------

// constLiveBodies
// ---------------
//! Return body list from output, if any.

QList<const NsBodyObject*>
NsBodyInputPlugObject::constLiveBodies() const
{
    const NsBodyOutputPlugObject *bopo(
        NsOpStore::instance()->queryConstBodyOutput(
            queryFeedingBodyOutput(longName())));
    QList<const NsBodyObject*> bodies;

    if (0 != bopo) {
        bodies = bopo->constLiveBodies();
    }

    return bodies;
}


// mutableLiveBodies
// -----------------
//! Return body list from output, if any.

QList<NsBodyObject*>
NsBodyInputPlugObject::mutableLiveBodies()
{
    NsBodyOutputPlugObject *bopo(
        NsOpStore::instance()->queryMutableBodyOutput(
            queryFeedingBodyOutput(longName())));
    QList<NsBodyObject*> bodies;  

    if (0 != bopo) {
        bodies = bopo->mutableLiveBodies();
    }

    return bodies;
}

// -----------------------------------------------------------------------------

// NsBodyInputPlugObject
// ---------------------
//! CTOR.

NsBodyInputPlugObject::NsBodyInputPlugObject(Ng::BodyInput &bodyInput,
                                             NsOpObject    &parent)
    : NsInputPlugObject(bodyInput, parent)
    , _bodyInput(&bodyInput)
{}

// -----------------------------------------------------------------------------

void
NsBodyInputPlugObject::groupPlugChanged()
{
    NsInputPlugObject::groupPlugChanged();   // Parent method.
    _bodyInput = 
        mutableOp()->mutableNgOp().bodyInput(fromQStr(name())); // Update.
}

// -----------------------------------------------------------------------------

//// onFeedChanged
//// -------------
////! DOCS. [slot]
//
//void
//NsBodyInputPlugObject::onFeedChanged(const QString &inputLongName,
//                                     const QString &plugLongName,
//                                     const  bool    success)
//{
//    //if (success && 
//    //    (longName() == plugLongName)) {
//
//    //    Ng::Plug* ngSource = source();
//    //    if (0 != ngSource) {
//    //        NsBodyOutputPlugObject* bopo = 
//    //            NsOpStore::instance()->mutableBodyOutputPlug(
//    //                fromNbStr(ngSource->longname()));
//
//    //        if (0 != bopo) {
//    //            bopo->computeLiveBodyCache();
//    //        }
//    //    }           
//    //}
//}
