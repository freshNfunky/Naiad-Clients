// -----------------------------------------------------------------------------
//
// NsTimeToolBar.h
//
// Naiad Studio time tool bar, header file.
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

#ifndef NS_TIME_TOOLBAR_H
#define NS_TIME_TOOLBAR_H

#include "NsTimeAction.h"
#include <QToolBar>

class NsUndoStack;
class NsTimeSlider;

// -----------------------------------------------------------------------------

// NsTimeToolBar
// -------------
//! The NsTimeToolBar class, representing the tool bar containing the
//! timeline and the timeline controls and actions.

class NsTimeToolBar : public QToolBar
{
    Q_OBJECT

public:

    static const QString&
    currentVisibleFrameMetaValueType()
    { return _currentVisibleFrameMetaValueType; }

    static const QString&
    firstVisibleFrameMetaValueType()
    { return _firstVisibleFrameMetaValueType; }

    static const QString&
    lastVisibleFrameMetaValueType()
    { return _lastVisibleFrameMetaValueType; }

private:

    static const QString _currentVisibleFrameMetaValueType;
    static const QString _firstVisibleFrameMetaValueType;
    static const QString _lastVisibleFrameMetaValueType;

public:

    explicit
    NsTimeToolBar(QAction *naiadAction,
                  NsUndoStack *undoStack, 
                  QWidget *parent = 0);

    // Expose actions so that they can be used elsewhere (e.g. in the main
    // window).

    NsTimeSlider*
    timeSlider() const
    { return _timeSlider; }

    NsFirstFrameAction*
    firstVisibleFrameAction() const
    { return _firstVisibleFrameAction; }

    NsStepBwdAction*
    previousFrameAction() const
    { return _previousFrameAction; }

    NsPlayBwdAction*
    playBwdAction() const
    { return _playBwdAction; }

    NsPlayFwdAction*
    playFwdAction() const
    { return _playFwdAction; }

    NsStepFwdAction*
    nextFrameAction() const
    { return _nextFrameAction; }

    NsLastFrameAction*
    lastVisibleFrameAction() const
    { return _lastVisibleFrameAction; }

    NsLoopAction*
    loopAction() const
    { return _loopAction; }


    // Step actions.

    NsStepAction*
    resetAction() const 
    { return _resetAction; }

    NsStepAction*
    stepToLastAction() const 
    { return _stepToLastAction; }

    NsStepAction*
    stepSingleAction() const 
    { return _stepSingleAction; }

    NsStepAction*
    restepAction() const 
    { return _restepAction; }

    NsStepAction*
    stepVisibleAction() const 
    { return _stepVisibleAction; }

    NsStepAction*
    stopAction() const
    { return _stopAction; }

protected:  // Events

    virtual void
    paintEvent(QPaintEvent *event);

    virtual void
    keyPressEvent(QKeyEvent *event);

protected:

    NsUndoStack*
    undoStack() const
    { return _undoStack; }

protected slots:

    void
    onValueChanged(const QString &paramLongName,
                   const QString &expr,
                   int            component,
                   bool           success);

    void
    onMetaChanged(const QString &longName,
                  const QString &valueType,
                  const QString &value,
                  bool           success);

    void
    onCurrentVisibleFrameChanged(int cvf, bool update3DView, bool success);

    void
    onFirstVisibleFrameChanged(int fvf, bool success);

    void
    onLastVisibleFrameChanged(int lvf, bool success);

private slots:

    void
    _reset(bool force = false);

    void
    _stepToLast();

    void
    _stepSingle();

    void
    _restep();

    void
    _stepVisible(bool force = false);

    void
    _stopStepping();

    void
    _onSliderValueChanged(int value);

    void
    _onSliderMinimumChanged(int min);

    void
    _onSliderMaximumChanged(int max);

private:    // Member variables.

    NsUndoStack *_undoStack;

    NsTimeSlider *_timeSlider;

    // Actions.

    NsFirstFrameAction *_firstVisibleFrameAction;   //!< First frame action.
    NsStepBwdAction    *_previousFrameAction;       //!< Step backward action.
    NsPlayBwdAction    *_playBwdAction;             //!< Play backward action.
    NsPlayFwdAction    *_playFwdAction;             //!< Play forward action.
    NsStepFwdAction    *_nextFrameAction;          //!< Step forward action.
    NsLastFrameAction  *_lastVisibleFrameAction;    //!< Last frame action.
    NsLoopAction       *_loopAction;                //!< Loop play action.

    NsStepAction *_resetAction;             //!< Reset solver action.
    NsStepAction *_stepToLastAction;        //!< Step from current action.
    NsStepAction *_stepSingleAction;        //!< Step frame action.
    NsStepAction *_restepAction;            //!< Re-Step action.
    NsStepAction *_stepVisibleAction;       //!< Step visible action.
    NsStepAction *_stopAction;

};

// -----------------------------------------------------------------------------

#endif // NS_TIME_TOOLBAR_H
