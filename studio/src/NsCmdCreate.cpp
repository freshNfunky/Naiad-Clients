// -----------------------------------------------------------------------------
//
// NsCmdCreate.cc
//
// Naiad Studio command class for creating an Op source file.
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

#include "NsCmdCreate.h"
#include "NsCmdUndoStack.h"
#include <QtDebug>


// run
// ---
//! Executes the request and places a command instance on the undostack if
//! successful.

void
NsCmdCreate::run(const QString     &opClass,
                 const QString     &opType,
                 const QString     &opInstance,
                 const bool         merge,
                 const QString     &text,
                 QUndoCommand      *parent)
{
    qDebug() << "NsCmdCreate::run";

    // True = Create command instance on success.

    _createRequest(NtString(opClass.toStdString()),
                   NtString(opType.toStdString()),
                   NtString(opInstance.toStdString()),
                   true, merge, text, parent);
}


// ~NsCmdCreate
// ------------
//! DTOR, destroy the command instance.

NsCmdCreate::~NsCmdCreate()
{
    qDebug() << "Destroy NsCmdCreate: " << this;
}


// undo
// ----
//! This method is called to undo the command.

void
NsCmdCreate::undo()
{
    // Do not create a new command instance.

    qDebug() << "NsCmdCreate::undo()";
    _eraseRequest(_cb.opInstance);
}


// mergeWith
// ---------
//! ...

bool
NsCmdCreate::mergeWith(const QUndoCommand *command)
{
    // Check if argument has same type as this object using RTTI.

    //const NsSetParamCmd *other =
    //    dynamic_cast<const NsSetParamCmd *>(command);

    // Make sure the two commands refer to the same parameter of
    // the same Op.

//    if (0 != other &&
//        other->_merge &&
//        other->_opName    == _opName &&
//        other->_paramName == _paramName &&
//        other->_component == _component) {
//        newValue = other->_newValue;
//        //oldValue = other->_oldValue;
//        return true;
//    }

    return false;
}


// id
// --
//! Returns the id for the NsCmdCreate class.

int
NsCmdCreate::id() const
{
    return -1;
}


// delayedRedo
// -----------
//! This method is called by an undo stack in order to redo the command.

void
NsCmdCreate::delayedRedo()
{
    // Do not create a new command instance.

    qDebug() << "NsCmdCreate::delayedRedo()";
    _createRequest(_cb.opClass, _cb.opType, _cb.opInstance, false);
}


// _CreateCallback
// ---------------
//! CTOR.

NsCmdCreate::_CreateCallback::_CreateCallback(const bool     createCmdInstance,
                                              const bool     merge,
                                              const QString &text,
                                              QUndoCommand  *parent)
    : NtCreateCallback(),
      _createCmdInstance(createCmdInstance),
      _merge(merge),
      _text(text),
      _parent(parent)
{}


// ~_CreateCallback
// ----------------
//! DTOR.

NsCmdCreate::_CreateCallback::~_CreateCallback()
{}


// success
// -------
//! Called from successful NI request.

void
NsCmdCreate::_CreateCallback::success()
{
    qDebug() << "NsCmdCreate::_CreateCallback::success()";

    // Notify the undo stack that a parameter has been changed.

    NsCmdUndoStack::instance()->onOpCreated(
        QString(this->opClass.str().c_str()),
        QString(this->opType.str().c_str()),
        QString(this->opInstance.str().c_str()));

    if (_createCmdInstance) {

        // Create a command instance and push it onto the undo stack.

        const NtCreateCallback *base(
            static_cast<const NtCreateCallback *>(this));

        NsCmdCreate* cmd = new NsCmdCreate(*base, _merge, _text, _parent);
        NsCmdUndoStack::instance()->push(cmd);
    }
}


// failure
// -------
//! Called from unsuccessful NI request. Must never throw.

void
NsCmdCreate::_CreateCallback::failure()
{
    qDebug() << "NsCmdCreate::_CreateCallback::failure()";
}


// _EraseCallback
// --------------
//! CTOR.

NsCmdCreate::_EraseCallback::_EraseCallback()
    : NtEraseCallback()
{}


// ~_EraseCallback
// ---------------
//! DTOR.

NsCmdCreate::_EraseCallback::~_EraseCallback()
{}


// success
// -------
//! Called from successful NI request.

void
NsCmdCreate::_EraseCallback::success()
{
    qDebug() << "NsCmdCreate::_EraseCallback::success()";

    // Notify the undo stack that an Op has been erased.

    NsCmdUndoStack::instance()->onOpErased(
        QString(this->opInstance.str().c_str()));
}


// failure
// -------
//! Called from unsuccessful NI request. Must never throw.

void
NsCmdCreate::_EraseCallback::failure()
{
    qDebug() << "NsCmdCreate::_EraseCallback::failure()";
}


// _createRequest
// --------------
//! Send request to NI.

void
NsCmdCreate::_createRequest(const NtString    &opClass,
                            const NtString    &opType,
                            const NtString    &opInstance,
                            const bool         createCmdInstance,
                            const bool         merge,
                            const QString     &text,
                            QUndoCommand      *parent)
{
    qDebug() << "NiCreate(" << opClass.str().c_str() << ","
                            << opType.str().c_str() << ","
                            << opInstance.str().c_str() << ")";

    // Create a callback object and provide it to NI.

    NsCmdCreate::_CreateCallback cb(createCmdInstance, merge, text, parent);
    NiPushRequestCallback(&cb);
    NiCreate(opClass, opType, opInstance);
}


// _eraseRequest
// --------------
//! Send request to NI.

void
NsCmdCreate::_eraseRequest(const NtString &opInstance)
{
    qDebug() << "NiErase(" << opInstance.str().c_str() << ")";

    // Create a callback object and provide it to NI.

    NsCmdCreate::_EraseCallback cb;
    NiPushRequestCallback(&cb);
    NiErase(opInstance);
}


// NsCmdCreate
// -----------
//! CTOR.

NsCmdCreate::NsCmdCreate(const NtCreateCallback &cb,
                         const bool              merge,
                         const QString          &text,
                         QUndoCommand           *parent)
    : NsCmdBase(text, parent),
      _cb(cb),
      _merge(merge)
{
    qDebug() << "Create NsCmdCreate: " << this;
}
