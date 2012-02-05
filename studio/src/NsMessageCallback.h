// -----------------------------------------------------------------------------
//
// NsMessageCallback.h
//
// Naiad Studio solve callback, header file.
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

#ifndef NS_MESSAGECALLBACK_H
#define NS_MESSAGECALLBACK_H

#include <QObject>
#include <QQueue>
#include <QString>
#include <NiTypes.h>

// -----------------------------------------------------------------------------

class NsMessageCallbackNotifier : public QObject
{
    Q_OBJECT

public:

    //! CTOR.
    explicit
    NsMessageCallbackNotifier(QObject *parent = 0)
        : QObject(parent)
    {}

    //! DTOR.
    virtual
    ~NsMessageCallbackNotifier()
    {}

    void
    emitInfo()
    { emit info(); }

    void
    emitWarning()
    { emit warning(); }

    void
    emitError()
    { emit error(); }

signals:

    void
    info();

    void
    warning();

    void
    error();
};

// -----------------------------------------------------------------------------

// NsMessageCallback
// ---------------
//! The NsMessageCallback class, representing the Naiad Interface message 
//! callback. Updates the information displayed in the user interface.

class NsMessageCallback : public NtMessageCallback
{
public:     // NtMessageCallback interface.

    virtual
    ~NsMessageCallback();

    virtual void
    info(const NtString &text);

    virtual void
    warning(const NtString &text);

    virtual void
    error(const NtString &text);

public:

    struct Message
    {
        QString text;
        QString vobName;
    };

public:

    bool
    pollInfo(Message &msg);

    bool
    pollWarning(Message &msg);

    bool
    pollError(Message &msg);

    bool
    hasInfo() const
    { return !_infoQueue.isEmpty(); }

    bool
    hasWarning() const
    { return !_warningQueue.isEmpty(); }

    bool
    hasError() const
    { return !_errorQueue.isEmpty(); }

public:

    explicit
    NsMessageCallback();

    const NsMessageCallbackNotifier*
    notifier() const
    { return &_notifier; }

    //    void enableGuiUpdates(const bool guiUpdates);

private:

    void
    _createMessage(const QString         &text,
                   const Nb::ValueObject *vob,
                   Message               &msg);

private:    // Member variables.

    NsMessageCallbackNotifier _notifier;

    typedef QQueue<Message> _MessageQueueType;

    _MessageQueueType _infoQueue;
    _MessageQueueType _warningQueue;
    _MessageQueueType _errorQueue;
    //    bool _guiUpdates;

private:

    NsMessageCallback(const NsMessageCallback&);            //!< Disabled.
    NsMessageCallback& operator=(const NsMessageCallback&); //!< Disabled.
};

#endif  // NS_MESSAGECALLBACK_H
