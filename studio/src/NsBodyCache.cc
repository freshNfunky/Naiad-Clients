// -----------------------------------------------------------------------------
//
// NsBodyCache.cc
//
// Class for caching bodies, header file.
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

#include "NsBodyCache.h"
#include "NsBodyObject.h"
#include "NsOpObject.h"
#include "NsCmdCentral.h"
#include "NsStringUtils.h"
#include <NbBody.h>
#include <QDebug>

// -----------------------------------------------------------------------------

// NsBodyCache
// -----------
//! CTOR.

NsBodyCache::NsBodyCache()
{}


// ~BodyCache
// ----------
//! DTOR.

NsBodyCache::~NsBodyCache()
{
    qDebug() << "~NsBodyCache";
    clear();
}

// -----------------------------------------------------------------------------

NsBodyObject*
NsBodyCache::addBody(Nb::Body               &nbb,
                     NsOpObject             &op,
                     NsBodyOutputPlugObject *bopo,
                     const bool              live)
{   
    NsBodyObject *body(new NsBodyObject(nbb, op, bopo, live));
    _bodies.insert(body->name(), body); // Cache owns memory.
    return body;

    //// Connect Body object to command central.

    //QObject::connect(NsCmdCentral::instance(),
    //                 SIGNAL(bodySelectionChanged(QString,bool,bool)),
    //                 body,
    //                 SLOT(onBodySelectionChanged(QString,bool,bool)));

    //QObject::connect(NsCmdCentral::instance(),
    //                 SIGNAL(metaChanged(QString,QString,QString,bool)),
    //                 body,
    //                 SLOT(onMetaChanged(QString,QString,QString,bool)));

    QObject::connect(&op,
                     SIGNAL(nameChanged(QString,QString)),
                     body,
                     SLOT(onOpNameChanged(QString,QString)));
}


// clear
// -----
//! Remove all bodies from the cache.

void
NsBodyCache::clear()
{
    qDebug() << "NsBodyCache::clear";

    foreach (NsBodyObject *body, _bodies) {
        delete body;    // Free memory.
    }
    _bodies.clear();    // Remove associations.
}

// -----------------------------------------------------------------------------

// constBodies
// -----------
//! DOCS.

QList<const NsBodyObject*>
NsBodyCache::constBodies() const
{ return constPtrList(_bodies.values()); }
