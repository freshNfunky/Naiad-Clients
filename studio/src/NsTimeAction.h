// -----------------------------------------------------------------------------
//
// NsTimeAction.h
//
// Naiad Studio time action header file.
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

#ifndef NS_TIME_ACTION_H
#define NS_TIME_ACTION_H

#include <NiTypes.h>

#include <QAction>
#include <QIcon>
#include <QKeySequence>
#include <QString>
#include <QObject>

// -----------------------------------------------------------------------------

// Classes:
// --------
// NsTimeAction [abstract]
//     NsFrameAction [abstract]
//         NsFirstFrameAction
//         NsStepBwdAction
//         NsPlayBwdAction
//         NsPlayFwdAction
//         NsStepFwdAction
//         NsLastFrameAction
//         NsLoopAction
//     NsSolveAction

// -----------------------------------------------------------------------------

// NsTimeAction
// ------------
//! Abstract base class for time-related actions.

class NsTimeAction : public QAction
{
    Q_OBJECT

public:

    explicit
    NsTimeAction(const QString      &text,
                 //const QString      &onIcon,
                 //const QString      &offIcon,
                 //const QSize        &iconSize,
                 //const QKeySequence &shortCut,
                 //const bool          checkable = false,
                 QObject            *parent    = 0)
        : QAction(text, parent)
    {
        //setStatusTip(text, shortCut);
        //setShortcut(shortCut);
        //setCheckable(checkable);
        //setOnOffIcons(onIcon, offIcon, iconSize);
    }

    virtual 
    ~NsTimeAction() = 0;    // Abstract.

//protected:
//
//    void
//    setOnOffIcons(const QString &onIcon,
//                  const QString &offIcon,
//                  const QSize   &iconSize)
//    {
//        QIcon icon;
//        icon.addFile(onIcon, iconSize, QIcon::Normal, QIcon::On);
//        icon.addFile(offIcon, iconSize, QIcon::Normal, QIcon::Off);
//        setIcon(icon);
//    }
//
//    void
//    setStatusTip(const QString &text, const QKeySequence &shortCut)
//    {
//        if (shortCut.isEmpty()) {
//            QAction::setStatusTip(text);
//        }
//        else {
//            QAction::setStatusTip(text + " [" + shortCut.toString() + "]");
//        }
//    }
};

// -----------------------------------------------------------------------------

// NsFrameAction
// -------------
//! Abstract base class for actions that manipulate the current frame.

class NsFrameAction : public NsTimeAction
{
    Q_OBJECT

public:

    explicit
    NsFrameAction(const QString      &text,
                  //const QString      &onIcon,
                  //const QString      &offIcon,
                  //const QSize        &iconSize,
                  //const QKeySequence &shortCut,
                  //const bool          checkable = false,
                  QObject            *parent    = 0)
         : NsTimeAction(text, parent)/*onIcon, offIcon, iconSize,
                        shortCut, checkable, parent)*/
    {}

    virtual 
    ~NsFrameAction() = 0;   // Force abstract.

protected:

    virtual void
    onSliderChanged(const int  frame,
                    const int  firstFrame,
                    const int  lastFrame,
                    const bool playingFwd,
                    const bool playingBwd,
                    const bool looping)
    {}  // Do nothing by default.
};

// -----------------------------------------------------------------------------

// NsFirstFrameAction
// ------------------
//! TODO

class NsFirstFrameAction : public NsFrameAction
{
    Q_OBJECT

public:

    explicit
    NsFirstFrameAction(
            const QString      &text,
            //const QString      &onIcon,
            //const QString      &offIcon,
            //const QSize        &iconSize,
            //const QKeySequence &shortCut,
            //const bool          checkable = false,
            QObject            *parent   = 0)
        : NsFrameAction(text, parent)/*onIcon, offIcon, iconSize,
                        shortCut, checkable, parent)*/
    {}

    //! DTOR.
    virtual 
    ~NsFirstFrameAction() 
    {}    

protected slots:

    virtual void
    onSliderChanged(const int  frame,
                    const int  firstFrame,
                    const int  lastFrame,
                    const bool playingFwd,
                    const bool playingBwd,
                    const bool looping)
    {
        if (playingFwd || playingBwd) {
            setEnabled(false);
        }
        else {
            setEnabled(frame != firstFrame);
        }
    }
};

// -----------------------------------------------------------------------------

// NsStepBwdAction
// ---------------
//! TODO

class NsStepBwdAction : public NsFrameAction
{
    Q_OBJECT

public:

    explicit
    NsStepBwdAction(const QString      &text,
                    //const QString      &onIcon,
                    //const QString      &offIcon,
                    //const QSize        &iconSize,
                    //const QKeySequence &shortCut,
                    //const bool          checkable = false,
                    QObject            *parent   = 0)
        : NsFrameAction(text, parent)/*onIcon, offIcon, iconSize,
                        shortCut, checkable, parent)*/
    {}

    //! DTOR.
    virtual 
    ~NsStepBwdAction() {}   

protected slots:

    virtual void
    onSliderChanged(const int  frame,
                    const int  firstFrame,
                    const int  lastFrame,
                    const bool playingFwd,
                    const bool playingBwd,
                    const bool looping)
    {
        if (playingFwd || playingBwd) {
            setEnabled(false);
        }
        else {
            setEnabled((firstFrame < frame && frame <= lastFrame));
        }
    }
};

// -----------------------------------------------------------------------------

// NsPlayBwdAction
// ---------------
//! TODO

class NsPlayBwdAction : public NsFrameAction
{
    Q_OBJECT

public:

    explicit
    NsPlayBwdAction(
            const QString      &text,
            //const QString      &onIcon,
            //const QString      &offIcon,
            //const QSize        &iconSize,
            //const QKeySequence &shortCut,
            //const bool          checkable = true,
            QObject            *parent   = 0)
        : NsFrameAction(text, parent)/*onIcon, offIcon, iconSize,
                           shortCut, checkable, parent)*/
    {}

    //! DTOR.
    virtual 
    ~NsPlayBwdAction() 
    {}   

protected slots:

    virtual void
    onSliderChanged(const int frame,
                    const int firstFrame,
                    const int lastFrame,
                    const bool playingFwd,
                    const bool playingBwd,
                    const bool looping)
    {
        if (!playingBwd && !playingFwd) {
            setEnabled(true);
            setChecked(false);
        }

        if (playingFwd) {
            setEnabled(false);
        }
        else {
            setEnabled((firstFrame < frame && frame <= lastFrame) || looping);
        }
    }
};

// -----------------------------------------------------------------------------

// NsPlayFwdAction
// ---------------
//! TODO

class NsPlayFwdAction : public NsFrameAction
{
    Q_OBJECT

public:

    //! CTOR.
    explicit
    NsPlayFwdAction(const QString      &text,
                    //const QString      &onIcon,
                    //const QString      &offIcon,
                    //const QSize        &iconSize,
                    //const QKeySequence &shortCut,
                    //const bool          checkable = true,
                    QObject            *parent   = 0)
        : NsFrameAction(text, parent)/*onIcon, offIcon, iconSize,
                        shortCut, checkable, parent)*/
    {}

    //! DTOR.
    virtual 
    ~NsPlayFwdAction() 
    {}   

protected slots:

    virtual void
    onSliderChanged(const int  frame,
                    const int  firstFrame,
                    const int  lastFrame,
                    const bool playingFwd,
                    const bool playingBwd,
                    const bool looping)
    {
        if (!playingBwd && !playingFwd) {
            setEnabled(true);
            setChecked(false);
        }

        if (playingBwd) {
            setEnabled(false);
        }
        else {
            setEnabled((firstFrame <= frame && frame < lastFrame) || looping);
        }
    }
};

// -----------------------------------------------------------------------------

// NsStepFwdAction
// ---------------
//! TODO

class NsStepFwdAction : public NsFrameAction
{
    Q_OBJECT

public:

    explicit
    NsStepFwdAction(const QString      &text,
                    //const QString      &onIcon,
                    //const QString      &offIcon,
                    //const QSize        &iconSize,
                    //const QKeySequence &shortCut,
                    //const bool          checkable = false,
                    QObject            *parent   = 0)
        : NsFrameAction(text, parent)/*onIcon, offIcon, iconSize,
                        shortCut, checkable, parent)*/
    {}

    //! DTOR.
    virtual 
    ~NsStepFwdAction() 
    {}

protected slots:

    virtual void
    onSliderChanged(const int  frame,
                    const int  firstFrame,
                    const int  lastFrame,
                    const bool playingFwd,
                    const bool playingBwd,
                    const bool looping)
    {
        if (playingFwd || playingBwd) {
            setEnabled(false);
        }
        else {
            setEnabled((firstFrame <= frame && frame < lastFrame));
        }
    }
};


// -----------------------------------------------------------------------------

// NsLastFrameAction
// -----------------
//! TODO

class NsLastFrameAction : public NsFrameAction
{
    Q_OBJECT

public:

    explicit
    NsLastFrameAction(const QString      &text,
                      //const QString      &onIcon,
                      //const QString      &offIcon,
                      //const QSize        &iconSize,
                      //const QKeySequence &shortCut,
                      //const bool          checkable = false,
                      QObject            *parent   = 0)
        : NsFrameAction(text, parent)/*onIcon, offIcon, iconSize,
                        shortCut, checkable, parent)*/
    {}

    //! DTOR.
    virtual 
    ~NsLastFrameAction() 
    {}

protected slots:

    virtual void
    onSliderChanged(const int  frame,
                    const int  firstFrame,
                    const int  lastFrame,
                    const bool playingFwd,
                    const bool playingBwd,
                    const bool looping)
    {
        if (playingFwd || playingBwd) {
            setEnabled(false);
        }
        else {
            setEnabled(frame != lastFrame);
        }
    }
};

// -----------------------------------------------------------------------------

// NsLoopAction
// -----------------
//! TODO

class NsLoopAction : public NsFrameAction
{
    Q_OBJECT

public:

    //! CTOR.
    explicit
    NsLoopAction(const QString      &text,
                 //const QString      &onIcon,
                 //const QString      &offIcon,
                 //const QSize        &iconSize,
                 //const QKeySequence &shortCut,
                 //const bool          checkable = true,
                 QObject            *parent    = 0)
        : NsFrameAction(text, parent)/*onIcon, offIcon, iconSize,
                        shortCut, checkable, parent)*/
    {}

    //! DTOR.
    virtual
    ~NsLoopAction()
    {}
};

// -----------------------------------------------------------------------------

class NsStepAction : public NsTimeAction
{
    Q_OBJECT

public:

    //! CTOR.
    explicit
    NsStepAction(const QString      &text,
                 //const QString      &onIcon,
                 //const QString      &offIcon,
                 //const QSize        &iconSize,
                 //const QKeySequence &shortCut,
                 //const bool          checkable,
                 QObject            *parent = 0)
         : NsTimeAction(text, parent)/*onIcon, offIcon, iconSize,
                        shortCut, checkable, parent)*/
    {}

    //! DTOR.
    virtual
    ~NsStepAction()
    {}

protected slots:

    void
    onBeginStep(const NtTimeBundle &tb)
    {
        //setEnabled(false);
        setEnabled(!isEnabled());
    }

    void
    onEndStep(const NtTimeBundle &tb)
    {
        setEnabled(!isEnabled());
        //setEnabled(true);
    }
};

// -----------------------------------------------------------------------------

#endif // NS_TIME_ACTION_H
