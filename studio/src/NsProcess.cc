// -----------------------------------------------------------------------------
//
// NsProcess.cc
//
// Naiad Studio progress, source file.
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

#include "NsProcess.h"
#include "NsConsole.h"
#include <QDebug>

// -----------------------------------------------------------------------------

// NsProcess
// ---------
//! CTOR.

NsProcess::NsProcess(NsConsole &parent)
    : QProcess(&parent)
{
    connect(this, 
            SIGNAL(readyReadStandardOutput()), 
            SLOT(onReadyReadStandardOutput()));
    connect(this, 
            SIGNAL(readyReadStandardError()), 
            SLOT(onReadyReadStandardError()));
    connect(this,
            SIGNAL(error(QProcess::ProcessError)),
            SLOT(onError(QProcess::ProcessError)));
    connect(this,
            SIGNAL(finished(int,QProcess::ExitStatus)),
            SLOT(onFinished(int,QProcess::ExitStatus)));
    connect(this,
            SIGNAL(stateChanged(QProcess::ProcessState)),
            SLOT(onStateChanged(QProcess::ProcessState)));
}


// ~NsProcess
// ----------
//! DTOR.

NsProcess::~NsProcess()
{ qDebug() << "~NsProcess"; }

// -----------------------------------------------------------------------------

// _errorMsg
// ---------
//! [static]

QString
NsProcess::_errorMsg(const QProcess::ProcessError error)
{
    QString msg(">> ERROR: ");
    switch (error) {
    case QProcess::FailedToStart:
        msg += QString(
            "The process failed to start. Either the invoked program is "
            "missing, or you may have insufficient permissions to invoke "
            "the program.");
        break;
    case QProcess::Crashed:
        msg += QString(
            "The process crashed some time after starting successfully.");
        break;
    case QProcess::Timedout:
        msg += QString(
            "The process timed out.");
        break;
    case QProcess::WriteError:
        msg += QString(
            "An error occurred when attempting to write to the process. "
            "For example, the process may not be running, or it may have "
            "closed its input channel.");
        break;
    case QProcess::ReadError:
        msg += QString(
            "An error occurred when attempting to read from the process. "
            "For example, the process may not be running.");
        break;
    case QProcess::UnknownError:
    default:
        msg += QString(
            "An unknown error occurred.");
        break;
    }

    return msg;
}


// _exitMsg
// --------
//! [static]

QString
NsProcess::_exitMsg(const QProcess::ExitStatus exitStatus)
{
    QString msg(">> EXIT: ");
    switch (exitStatus) {
    case QProcess::NormalExit:
        msg += QString("The process exited normally.");
        break;
    case QProcess::CrashExit:
    default:
        msg += QString("The process crashed.");
        break;
    }
        
    return msg;
}


// _stateMsg
// ---------
//! [static]

QString
NsProcess::_stateMsg(const QProcess::ProcessState state)
{
    QString msg(">> STATE: ");

    switch (state) {
    case QProcess::NotRunning:
        msg += QString(
            "The process is not running.");
        break;
    case QProcess::Starting:
        msg += QString(
            "The process is starting, "
            "but the program has not yet been invoked.");
        break;
    case QProcess::Running:
        msg += QString(
            "The process is running and is ready for reading and writing.");
        break;
    }
        
    return msg;
}
