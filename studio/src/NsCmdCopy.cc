// -----------------------------------------------------------------------------
//
// NsCmdCopy.cc
//
// Naiad Studio command class for copying to clipboard, source file.
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

#include "NsCmdCopy.h"
#include "NsOpStore.h"
#include "NsOpObject.h"
#include "NsInputPlugObject.h"
#include "NsQuery.h"
#include "NsStringUtils.h"
#include <Ni.h>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QStringList>
#include <QSet>
#include <QDebug>

// -----------------------------------------------------------------------------

// exec
// ----
//! Execute once. [static]

void
NsCmdCopy::exec(const QStringList &opInstances)
{
    _request(opInstances);
}


// exec
// ----
//! Execute as child of a parent command. [static]

void
NsCmdCopy::exec(const QStringList &opInstances,
                QUndoCommand      &parent)
{
    _request(opInstances, &parent);
}

// -----------------------------------------------------------------------------

void
NsCmdCopy::_request(const QStringList &opInstances,
                    QUndoCommand      *parent)
{
    QByteArray mime;

    // Export Ops first.

    foreach (const QString &opInstance, opInstances) {
        const NsOpObject *op = NsOpStore::instance()->queryConstOp(opInstance);
        if (0 != op) {
            const QByteArray niOp = 
                NiExportOp(fromQStr(opInstance), ".ni").c_str();
            mime += niOp;
            mime += "\n";
        }
    }

    // Export Feeds after all Ops.

    QSet<QString> internalFeeds;
    foreach (const QString &opInstance, opInstances) {
        const NsOpObject *op = NsOpStore::instance()->queryConstOp(opInstance);
        if (0 != op) {
            QSet<QString> feeds;
            queryFeeds(opInstance, feeds); // Long names of this Op's feeds.
            QString plugName;
            foreach (const QString &feed, feeds) {
                if (!internalFeeds.contains(feed)) {
                    const QString inputOpInstance = 
                        queryPlugOpInstance(feed, &plugName);
                    const NsOpObject *inputOp = 
                        NsOpStore::instance()->queryConstOp(inputOpInstance);
                    if (0 != inputOp) {
                        const NsInputPlugObject *ipo = 
                            inputOp->queryConstInput(plugName);

                        if (0 != ipo && 0 != ipo->source()) {
                            const QString outputOpInstance = 
                                queryPlugOpInstance(
                                    fromNbStr(ipo->source()->longname()));
                    
                            if (opInstances.contains(inputOpInstance) &&
                                opInstances.contains(outputOpInstance)) {
                                internalFeeds.insert(feed);
                                const QByteArray niFeed = 
                                    NiExportFeed(fromQStr(feed), ".ni").c_str();
                                mime += niFeed;
                                mime += "\n";
                            }
                        }
                    }
                }
            }
        }
    }

    if (!mime.isEmpty()) {
        QMimeData *mimeData = new QMimeData;
        mimeData->setData("text/plain", mime);
        mimeData->setData("application/ni", mime);
        QApplication::clipboard()->setMimeData(mimeData);// Clipboard owns mime.
    }
}

// -----------------------------------------------------------------------------
