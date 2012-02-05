// -----------------------------------------------------------------------------
//
// NsCmdPaste.cc
//
// Naiad Studio command class for pasting, source file.
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

#include "NsCmdPaste.h"
#include "NsCmdCopy.h"
#include "NsUndoStack.h"
#include "NsCmdSelectOp.h"
#include "NsCmdSelectAll.h"
#include "NsCmdSetOpPosition.h"
#include "NsOpStore.h"
#include "NsParserCallback.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QDebug>

// -----------------------------------------------------------------------------

// exec
// ----
//! Execute once.

void
NsCmdPaste::exec(const QPointF &mouseScenePos)
{
    _request(mouseScenePos, 0);
}


// exec
// ----
//! Execute as child of a parent command.

void
NsCmdPaste::exec(const QPointF &mouseScenePos,
                 QUndoCommand  &parent)
{
    _request(mouseScenePos, &parent);
}


void
NsCmdPaste::exec(const QPointF  &mouseScenePos,
                 NsUndoStack &stack,
                 const bool      merge)
{
    _request(mouseScenePos, stack, merge, "Paste");
}

// -----------------------------------------------------------------------------

void
NsCmdPaste::_request(const QPointF &mouseScenePos,
                     NsUndoStack   &stack,
                     const bool     merge,
                     const QString &text)
{
    const QMimeData *mimeData(QApplication::clipboard()->mimeData());

    if (mimeData->hasFormat("application/ni")) {
        const QByteArray niOps(mimeData->data("application/ni"));

        NsCmdPaste *cmd(new NsCmdPaste(merge, text));

        // Execute the paste.

        NsParserCallback pcb(true, cmd);
        NiParseBuffer(niOps, &pcb);

        // Unselect All

        if (cmd) {
            NsCmdSelectAll::exec(false, *cmd);
        }
        else {
            NsCmdSelectAll::exec(false);
        }

        // Selected all freshly made ops.

        foreach (const QString &opInstance, pcb.createdOpInstances()) {
            NsCmdSelectOp::exec(
                NsCmdSelectOp::ArgsList() <<
                    NsCmdSelectOp::Args(opInstance, true), *cmd);
        }

        QPointF min0;
        QPointF max0;
        pcb.createdOpPositionBounds(min0, max0);
        const QPointF ctr0(0.5f*(max0 + min0));

        foreach (const QString &opInstance, pcb.createdOpInstances()) {
            const QPointF p0(pcb.createdOpPosition(opInstance) - ctr0);
            const QPointF p1(mouseScenePos + p0);
            NsCmdSetOpPosition::exec(
                NsCmdSetOpPosition::ArgsList() <<
                    NsCmdSetOpPosition::Args(opInstance, p1.x(), p1.y()),
                *cmd);
        }

        stack.push(cmd);
    }
}


void
NsCmdPaste::_request(const QPointF &mouseScenePos,
                     QUndoCommand  *parent)
{
    const QMimeData *mimeData(QApplication::clipboard()->mimeData());

    if (mimeData->hasFormat("application/ni")) {
        const QByteArray niOps(mimeData->data("application/ni"));

        NsCmdPaste *cmd(0);

        if (0 != parent) {
            cmd = new NsCmdPaste(parent);   // Child.
        }

        // Execute the paste.

        NsParserCallback pcb(true, cmd);
        NiParseBuffer(niOps, &pcb);

        // Unselect All

        if(cmd) {
            NsCmdSelectAll::exec(false, *cmd);
        }
        else {
            NsCmdSelectAll::exec(false);
        }

        // Selected all freshly made ops.

        foreach(const QString& opInstance, pcb.createdOpInstances()) {
            if(cmd) {
                NsCmdSelectOp::exec(
                    NsCmdSelectOp::ArgsList() <<
                        NsCmdSelectOp::Args(opInstance, true), *cmd);
            }
            else {
                NsCmdSelectOp::exec(
                    NsCmdSelectOp::ArgsList() <<
                        NsCmdSelectOp::Args(opInstance, true));
            }
        }

        QPointF min0;
        QPointF max0;
        pcb.createdOpPositionBounds(min0, max0);
        const QPointF ctr0(0.5f*(max0 + min0));

        foreach (const QString &opInstance, pcb.createdOpInstances()) {
            const QPointF p0(pcb.createdOpPosition(opInstance) - ctr0);
            const QPointF p1(mouseScenePos + p0);

            if (0 != cmd) {
                NsCmdSetOpPosition::exec(
                    NsCmdSetOpPosition::ArgsList() <<
                        NsCmdSetOpPosition::Args(opInstance, p1.x(), p1.y()),
                    *cmd);
            }
            else {
                NsCmdSetOpPosition::exec(
                    NsCmdSetOpPosition::ArgsList() <<
                        NsCmdSetOpPosition::Args(opInstance, p1.x(), p1.y()));
            }
        }
    }
}
