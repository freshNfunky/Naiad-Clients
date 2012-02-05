// -----------------------------------------------------------------------------
//
// NsCmdCentral.h
//
// Naiad Studio command central, header file.
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

#ifndef NS_CMD_CENTRAL_H
#define NS_CMD_CENTRAL_H

#include "NsInputPlugObject.h"
#include <QObject>
#include <QDebug>

// -----------------------------------------------------------------------------

class NsCmdCentral : public QObject
{
    Q_OBJECT

public:     // Singleton interface.

    static NsCmdCentral*
    instance();

    static void
    createInstance();

    static void
    destroyInstance();

private:    // Singleton implementation.

    static NsCmdCentral *_instance;

public slots:   // Op-related notification.

    void
    onOpCreated(const QString &opInstance, const bool success)
    { emit opCreated(opInstance, success); }

    void
    onOpErased(const QString &opInstance, const bool success)
    { emit opErased(opInstance, success); }

    void
    onOpNameChanged(const QString &oldOpInstance,
                    const QString &newOpInstance,
                    const bool     success)
    { emit opNameChanged(oldOpInstance, newOpInstance, success); }

    void
    onOpStateChanged(const QStringList &opInstances,
                     const bool         success)
    { emit opStateChanged(opInstances, success); }

    void
    onValueChanged(const QString &valueLongName,
                   const QString &expr,
                   const int      comp,
                   const bool     success)
    {
        if ("Global.Project Path" == valueLongName) {
            emit projectPathChanged(expr, success);
        }

        emit valueChanged(valueLongName, expr, comp, success);
    }

    void
    onMetaChanged(const QString &longName,
                  const QString &valueType,
                  const QString &value,
                  const bool     success)
    { emit metaChanged(longName, valueType, value, success); }

public slots:   // Plugs

    void
    onSmackChanged(const QStringList &plugLongNames, const bool success)
    { emit smackChanged(plugLongNames, success); }

    void
    onGroupPlugChanged(const QString &plugLongName,
                       const bool     success)
    { emit groupPlugChanged(plugLongName, success); }

public slots:   // Selection notification.

    //void
    //onOpSelectionChanged(const QString &opInstance,
    //                     const bool     selected,
    //                     const bool     success)
    //{ emit opSelectionChanged(opInstance, selected, success); }

    void
    onOpSelectionChanged(const QStringList &opInstances,
                         const bool         success)
    { emit opSelectionChanged(opInstances, success); }

    void
    onBodySelectionChanged(const QStringList &bodyLongNames,
                           const bool         success)
    { emit bodySelectionChanged(bodyLongNames, success); }

    void
    onFeedSelectionChanged(const QStringList &inputLongNames,
                           const bool         success)
    { emit feedSelectionChanged(inputLongNames, success); }

public slots:   // Feed-related notification.

    void
    onFeedChanged(const QString &inputLongName,
                  const QString &plugLongName,
                  const bool     success)
    { emit feedChanged(inputLongName, plugLongName, success); }

public slots:   // Misc. notification.

    void
    onGraphCleared(const bool success)
    { emit graphCleared(success); }

    void
    onCurrentVisibleFrameChanged(const int  cvf, 
                                 const bool update3DView, 
                                 const bool success)
    { emit currentVisibleFrameChanged(cvf, update3DView, success); }

    void
    onFirstVisibleFrameChanged(const int fvf, const bool success)
    { emit firstVisibleFrameChanged(fvf, success); }

    void
    onLastVisibleFrameChanged(const int lvf, const bool success)
    { emit lastVisibleFrameChanged(lvf, success); }

    void
    onGraphViewFramed(const bool selected, const int pad, const bool success)
    { emit graphViewFramed(selected, pad, success); }

    void
    onOpPositionChanged(const QString &opInstance,
                        const qreal    x,
                        const qreal    y,
                        const bool     success)
    { emit opPositionChanged(opInstance, x, y, success); }

signals:

    // Op signals.

    //! Signal emitted when an Op has been created.
    void
    opCreated(const QString &opInstance,
              bool           success);

    //! Signal emitted when an Op has been erased.
    void
    opErased(const QString &opInstance,
             bool           success);

    //! Signal emitted when an Op has been renamed.
    void
    opNameChanged(const QString &oldOpInstance,
                  const QString &newOpInstance,
                  bool           success);

    //! Signal emitted when an Op state has changed.
    void
    opStateChanged(const QStringList &opInstance,
                   bool               success);

    //! Signal emitted when a value (i.e. param) has changed.
    void
    valueChanged(const QString &longName,
                 const QString &expr,
                 int            comp,
                 bool           success);

    //! Signal emitted when the project path has changed.
    void
    projectPathChanged(const QString &path,
                       bool           success);

    //! Signal emitted when a meta value has changed.
    void
    metaChanged(const QString &longName,
                const QString &valueType,
                const QString &value,
                bool           success);

    // Plug signals.

//    //! Signal emitted when an input plug mode has been changed.
//    void
//    inputPlugModeChanged(const QString           &inputLongName,
//                         NsInputPlugObject::Mode  mode,
//                         bool                     success);

    //! Signal emitted when a plug has been smacked/unsmacked.
    void
    smackChanged(const QStringList &plugLongNames, bool success);

    //! Signal emitted when the group flag of a plug has been changed.
    void
    groupPlugChanged(const QString &inputLongName,
                     bool           success);

    // Selection signals.

    void
    opSelectionChanged(const QStringList &opInstances,
                       bool               success);

    //! Signal emitted when a body has been selected.
    void
    bodySelectionChanged(const QStringList &bodyLongName,
                         bool               success);

    //! Signal emitted when a feed has been selected.
    void
    feedSelectionChanged(const QStringList &inputLongName,
                         bool               success);


    // Feed signals.

    //! Signal emitted when a feed has changed.
    void
    feedChanged(const QString &inputLongName,
                const QString &plugLongName,
                bool           success);


    // Misc. signals.

    //! Signal emitted when the graph has been cleared.
    void
    graphCleared(bool success);

    //! Signal emitted when the current visible frame has changed.
    void
    currentVisibleFrameChanged(int cvf, bool update3DView, bool success);

    //! Signal emitted when the first visible frame has changed.
    void
    firstVisibleFrameChanged(int fvf, bool success);

    //! Signal emitted when the last visible frame has changed.
    void
    lastVisibleFrameChanged(int lvf, bool success);

    //! Signal emitted when the graph view has been framed.
    void
    graphViewFramed(bool selected, int pad, bool success);

    //! Signal emitted when the position of an Op has changed.
    void
    opPositionChanged(const QString &opInstance,
                      qreal          x,
                      qreal          y,
                      bool           success);

private:

    //! CTOR.
    explicit
    NsCmdCentral()
        : QObject()
    {}

    //! DTOR.
    virtual
    ~NsCmdCentral()
    {}

    NsCmdCentral(const NsCmdCentral&);              //!< Disabled.
    NsCmdCentral& operator=(const NsCmdCentral&);   //!< Disabled.
};

// -----------------------------------------------------------------------------

#endif // NS_CMD_CENTRAL_H
