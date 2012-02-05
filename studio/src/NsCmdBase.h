// -----------------------------------------------------------------------------
//
// NsCmdBase.h
//
// Naiad Studio command base class header file.
//
// Copyright (c) 2011 Exotic Matter AB.  All rights reserved.
//
// This file is part of Open Naiad Studio..
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

#ifndef NS_CMD_BASE_H
#define NS_CMD_BASE_H

#include <NiTypes.h>
#include <QObject>
#include <QString>
#include <QUndoCommand>


// NsCmdBase
// ---------
//! Abstract base class for Naiad Studio commands. This class inherits both
//! from QUndoCommand and QObject. The reason for inheriting from QUndoCommand
//! is so that derived classes will work the QUndoStack. Inheritance from
//! QObject is motivated by the fact that derived classes use signals to
//! communicate with the undostack.

class NsCmdBase : public QUndoCommand
{
public:

    //! DTOR.
    virtual ~NsCmdBase() {}

    // NsCmdBase provides an implementation of redo that is suitable for
    // all derived classes, which should NOT implement this function.

    virtual void redo();

    // Derived classes must implement the following pure virtual functions,
    // which were inherited directly from QUndoCommand.

    virtual void undo() = 0;
    virtual bool mergeWith(const QUndoCommand *command) = 0;

    //! Provide an id that is not -1.
    virtual int
    id() const { return 0; }

protected:

    // Constructors are only available to derived classes.

    //! CTOR.
    explicit
    NsCmdBase(const QString &text, QUndoCommand *parent = 0)
        : QUndoCommand(text, parent),
          _firstRedo(true)
    {}


    //! Derived classes should implement this function to perform some
    //! command task. This function gets called by redo(), except the first
    //! time. This is because the first call to redo() typically corresponds
    //! to the command being pushed on a QUndoStack. Since we wish to evaluate
    //! the success of commands before pushing them, we must decouple such
    //! pushing from actually performing the command task.

    virtual void
    delayedRedo() const = 0;

private:    // Member variables.

    bool _firstRedo;    //!< Flag for ensuring correct pushing onto undostack.

private:    // Not defined, disabled.

    NsCmdBase(const NsCmdBase&);            //!< Disabled.
    NsCmdBase& operator=(const NsCmdBase&); //!< Disabled.
};

#endif // NS_CMD_BASE_H





// TODO: Ugly for base class to have information regarding
// derived classes.
// Enum containing the ids of all mergeable commands.
//    enum {
//        NsCommandId = 0,
//        NsSetOpParameterCommandId,
//        NsSetOpVectorParameterCommandId,
//        NsMoveOpsCommandId,
//        NsSelectCommandId
//    };
