// -----------------------------------------------------------------------------
//
// NsTimeSlider.h
//
// Naiad Studio time slider, header file.
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

#ifndef NS_TIME_SLIDER_H
#define NS_TIME_SLIDER_H

#include <QSlider>

class NsFirstFrameAction;
class NsStepBwdAction;
class NsPlayBwdAction;
class NsPlayFwdAction;
class NsStepFwdAction;
class NsLastFrameAction;
class NsLoopAction;

// -----------------------------------------------------------------------------

// NsTimeSlider
// ------------
//! DOCS!

class NsTimeSlider : public QSlider
{
    Q_OBJECT

public:

    explicit
    NsTimeSlider(QWidget *parent = 0);

    //! DTOR.
    virtual
    ~NsTimeSlider()
    {}

public:

    int
    firstVisibleFrame() const
    { return minimum(); }

    int
    lastVisibleFrame() const
    { return maximum(); }

    int
    currentVisibleFrame() const
    { return value(); }

    void
    setFirstVisibleFrame(const int fvf)
    { _setMinimum(fvf); }

    void
    setLastVisibleFrame(const int lvf)
    { _setMaximum(lvf); }

    void
    setCurrentVisibleFrame(const int cvf)
    { _setValue(cvf); }

public:     // Actions.

    NsFirstFrameAction*
    createFirstVisibleFrameAction(QObject *parent) const;

    NsStepBwdAction*
    createPreviousFrameAction(QObject *parent) const;

    NsPlayBwdAction*
    createPlayBwdAction(QObject *parent) const;

    NsPlayFwdAction*
    createPlayFwdAction(QObject *parent) const;

    NsStepFwdAction*
    createNextFrameAction(QObject *parent) const;

    NsLastFrameAction*
    createLastVisibleFrameAction(QObject *parent) const;

    NsLoopAction*
    createLoopAction(QObject *parent) const;

public slots:

    void
    setFirstVisibleFrame()
    { triggerAction(SliderToMinimum); }

    void
    singleStepBwd()
    { triggerAction(SliderSingleStepSub); }

    void
    playBwd()
    { _isPlayingBwd() ? _stop() : _playBwd(); }

    void
    playFwd()
    { _isPlayingFwd() ? _stop() : _playFwd(); }

    void
    singleStepFwd()
    { triggerAction(SliderSingleStepAdd); }

    void
    setLastVisibleFrame()
    { triggerAction(SliderToMaximum); }

protected slots:

    void
    onValueChanged(const int val)
    { _setValue(val); }

    void
    onMinimumChanged(const int min)
    { _setMinimum(min); }

    void
    onMaximumChanged(const int max)
    { _setMaximum(max); }

signals:

    void
    sliderChanged(int value,int min,int max,bool fwd,bool bwd,bool loop);

    void
    minimumChanged(int min);

    void
    maximumChanged(int max);

private slots:

    void
    _onChange();

    void
    _onValueChanged(int value);

    void
    _onRangeChanged(int min, int max);

    void
    _onLoopChanged(bool checked);

    void
    _onActionTriggered(int action);

private:

    bool
    _isMinimum(const int value) const
    { return (minimum() == value); }

    bool
    _isMaximum(const int value) const
    { return (maximum() == value); }

    //! Returns true if the slider is playing forward.
    bool
    _isPlayingFwd() const
    { return (SliderSingleStepAdd == repeatAction()); }

    //! Returns true if the slider is playing backward.
    bool
    _isPlayingBwd() const
    { return (SliderSingleStepSub == repeatAction()); }

    bool
    _isLooping() const
    { return _looping; }

    void
    _playFwd(int thresholdTime = 0);

    void
    _playBwd(int thresholdTime = 0);

    void
    _stop();

private:    // Implementations.

    void
    _setMinimum(int min);

    void
    _setMaximum(int max);

    void
    _setValue(int v);

private:

    static int
    _playRepeatTime();

private:    // Member variables.

    bool _looping;
};

#endif // NS_TIME_SLIDER_H
