// -----------------------------------------------------------------------------
//
// NsTimeSlider.cc
//
// Naiad Studio time slider, source file.
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

#include "NsTimeSlider.h"
#include "NsTimeAction.h"
#include "NsQuery.h"

// -----------------------------------------------------------------------------

// NsTimeSlider
// ------------
//! CTOR.

NsTimeSlider::NsTimeSlider(QWidget *parent)
    : QSlider(Qt::Horizontal, parent)
    , _looping(false)
{
    setTickInterval(1);
    setTickPosition(QSlider::TicksBothSides);

    // Internal connections.

    connect(this, SIGNAL(actionTriggered(int)),
            this, SLOT(_onActionTriggered(int)));

    connect(this, SIGNAL(valueChanged(int)),
            this, SLOT(_onValueChanged(int)));

    connect(this, SIGNAL(rangeChanged(int,int)),
            this, SLOT(_onRangeChanged(int,int)));
}

// -----------------------------------------------------------------------------

NsFirstFrameAction*
NsTimeSlider::createFirstVisibleFrameAction(QObject *parent) const
{
    NsFirstFrameAction *action = 
        new NsFirstFrameAction(tr("First Visible Frame"), parent); // Child.
    QIcon icon;
    icon.addFile(
        ":/images/scrub_first-bwd-enabled.png",
        QSize(), QIcon::Normal, QIcon::Off);
    icon.addFile(
        ":/images/scrub_first-bwd-pressed.png",
        QSize(), QIcon::Active, QIcon::Off);
    icon.addFile(
        ":/images/scrub_first-bwd-disabled.png", 
        QSize(), QIcon::Disabled, QIcon::Off);
    action->setIcon(icon);
    action->setCheckable(false);
    action->setShortcuts(
        QList<QKeySequence>() 
            << Qt::CTRL + Qt::Key_Down 
            << Qt::Key_Home);
    action->setStatusTip(
        action->text() + " [" + action->shortcut().toString() + "]");
    connect(action, SIGNAL(triggered()), SLOT(setFirstVisibleFrame()));
    connect(this,   SIGNAL(sliderChanged(int,int,int,bool,bool,bool)),
            action, SLOT(onSliderChanged(int,int,int,bool,bool,bool)));
    return action;
}


NsStepBwdAction*
NsTimeSlider::createPreviousFrameAction(QObject *parent) const
{
    NsStepBwdAction *action = 
        new NsStepBwdAction(tr("Previous Visible Frame"), parent); // Child.
    QIcon icon;
    icon.addFile(
        ":/images/scrub_single-bwd-enabled.png",
        QSize(), QIcon::Normal, QIcon::Off);
    icon.addFile(
        ":/images/scrub_single-bwd-pressed.png",
        QSize(), QIcon::Active, QIcon::Off);
    icon.addFile(
        ":/images/scrub_single-bwd-disabled.png", 
        QSize(), QIcon::Disabled, QIcon::Off);
    action->setIcon(icon);
    action->setCheckable(false);
    action->setShortcuts(
        QList<QKeySequence>() 
            << Qt::Key_Left);
    action->setStatusTip(
        action->text() + " [" + action->shortcut().toString() + "]");
    connect(action, SIGNAL(triggered()), SLOT(singleStepBwd()));
    connect(this,   SIGNAL(sliderChanged(int,int,int,bool,bool,bool)),
            action, SLOT(onSliderChanged(int,int,int,bool,bool,bool)));
    return action;
}

NsPlayBwdAction*
NsTimeSlider::createPlayBwdAction(QObject *parent) const
{
    NsPlayBwdAction *action = 
        new NsPlayBwdAction(tr("Play Backward"), parent);   // Child.
    QIcon icon;
    icon.addFile(
        ":/images/scrub_pause-enabled.png",
        QSize(), QIcon::Normal, QIcon::On);
    icon.addFile(
        ":/images/scrub_play-bwd-enabled.png",
        QSize(), QIcon::Normal, QIcon::Off);
    icon.addFile(
        ":/images/scrub_pause-pressed.png",
        QSize(), QIcon::Active, QIcon::On);
    icon.addFile(
        ":/images/scrub_play-bwd-pressed.png",
        QSize(), QIcon::Active, QIcon::Off);
    icon.addFile(
        ":/images/scrub_pause-disabled.png", 
        QSize(), QIcon::Disabled, QIcon::On);
    icon.addFile(
        ":/images/scrub_play-bwd-disabled.png", 
        QSize(), QIcon::Disabled, QIcon::Off);
    action->setIcon(icon);
    action->setCheckable(true);
    action->setShortcuts(
        QList<QKeySequence>()
            << Qt::Key_Down);
    action->setStatusTip(
        action->text() + " [" + action->shortcut().toString() + "]");
    connect(action, SIGNAL(triggered()), SLOT(playBwd()));
    connect(this,   SIGNAL(sliderChanged(int,int,int,bool,bool,bool)),
            action, SLOT(onSliderChanged(int,int,int,bool,bool,bool)));
    return action;
}

NsPlayFwdAction*
NsTimeSlider::createPlayFwdAction(QObject *parent) const
{
    NsPlayFwdAction *action = 
        new NsPlayFwdAction(tr("Play Forward"), parent);    // Child.
    QIcon icon;
    icon.addFile(
        ":/images/scrub_pause-enabled.png",
        QSize(), QIcon::Normal, QIcon::On);
    icon.addFile(
        ":/images/scrub_play-fwd-enabled.png",
        QSize(), QIcon::Normal, QIcon::Off);
    icon.addFile(
        ":/images/scrub_pause-pressed.png",
        QSize(), QIcon::Active, QIcon::On);
    icon.addFile(
        ":/images/scrub_play-fwd-pressed.png",
        QSize(), QIcon::Active, QIcon::Off);
    icon.addFile(
        ":/images/scrub_pause-disabled.png", 
        QSize(), QIcon::Disabled, QIcon::On);
    icon.addFile(
        ":/images/scrub_play-fwd-disabled.png", 
        QSize(), QIcon::Disabled, QIcon::Off);
    action->setIcon(icon);
    action->setCheckable(true);
    action->setShortcuts(
        QList<QKeySequence>()
            << Qt::Key_Up);
    action->setStatusTip(
        action->text() + " [" + action->shortcut().toString() + "]");
    connect(action, SIGNAL(triggered()), SLOT(playFwd()));
    connect(this,   SIGNAL(sliderChanged(int,int,int,bool,bool,bool)),
            action, SLOT(onSliderChanged(int,int,int,bool,bool,bool)));
    return action;
}

NsStepFwdAction*
NsTimeSlider::createNextFrameAction(QObject *parent) const
{
    NsStepFwdAction *action = 
        new NsStepFwdAction(tr("Next Visible Frame"), parent);  // Child.
    QIcon icon;
    icon.addFile(
        ":/images/scrub_single-fwd-enabled.png",
        QSize(), QIcon::Normal, QIcon::Off);
    icon.addFile(
        ":/images/scrub_single-fwd-pressed.png",
        QSize(), QIcon::Active, QIcon::Off);
    icon.addFile(
        ":/images/scrub_single-fwd-disabled.png", 
        QSize(), QIcon::Disabled, QIcon::Off);
    action->setIcon(icon);
    action->setCheckable(false);
    action->setShortcuts(
        QList<QKeySequence>()
            << Qt::Key_Right);
    action->setStatusTip(
        action->text() + " [" + action->shortcut().toString() + "]");
    connect(action, SIGNAL(triggered()), SLOT(singleStepFwd()));
    connect(this,   SIGNAL(sliderChanged(int,int,int,bool,bool,bool)),
            action, SLOT(onSliderChanged(int,int,int,bool,bool,bool)));
    return action;
}

NsLastFrameAction*
NsTimeSlider::createLastVisibleFrameAction(QObject *parent) const
{
    NsLastFrameAction *action = 
        new NsLastFrameAction(tr("Last Visible Frame"), parent);
    QIcon icon;
    icon.addFile(
        ":/images/scrub_last-fwd-enabled.png",
        QSize(), QIcon::Normal, QIcon::Off);
    icon.addFile(
        ":/images/scrub_last-fwd-pressed.png",
        QSize(), QIcon::Active, QIcon::Off);
    icon.addFile(
        ":/images/scrub_last-fwd-disabled.png", 
        QSize(), QIcon::Disabled, QIcon::Off);
    action->setIcon(icon);
    action->setCheckable(false);
    action->setShortcuts(
        QList<QKeySequence>()
            << Qt::CTRL + Qt::Key_Up 
            << Qt::Key_End);
    action->setStatusTip(
        action->text() + " [" + action->shortcut().toString() + "]");
    connect(action, SIGNAL(triggered()), SLOT(setLastVisibleFrame()));
    connect(this,   SIGNAL(sliderChanged(int,int,int,bool,bool,bool)),
            action, SLOT(onSliderChanged(int,int,int,bool,bool,bool)));
    return action;
}

NsLoopAction*
NsTimeSlider::createLoopAction(QObject *parent) const
{
    NsLoopAction *action =
        new NsLoopAction(tr("Loop"), parent);   // Child.
    QIcon icon;
    icon.addFile(
        ":/images/scrub_loop-enabled.png",
        QSize(), QIcon::Normal, QIcon::On);
    icon.addFile(
        ":/images/scrub_loop-disabled.png",
        QSize(), QIcon::Normal, QIcon::Off);
    icon.addFile(
        ":/images/scrub_loop-pressed.png",
        QSize(), QIcon::Active, QIcon::On);
    icon.addFile(
        ":/images/scrub_loop-pressed.png",
        QSize(), QIcon::Active, QIcon::Off);
    icon.addFile(
        ":/images/scrub_loop-disabled.png", 
        QSize(), QIcon::Disabled, QIcon::On);
    icon.addFile(
        ":/images/scrub_loop-disabled.png", 
        QSize(), QIcon::Disabled, QIcon::Off);
    action->setIcon(icon);
    action->setCheckable(true);
    action->setStatusTip(action->text());
    action->setChecked(_looping); // Current state.
    connect(action, SIGNAL(triggered(bool)), SLOT(_onLoopChanged(bool)));
    return action; 
}

// -----------------------------------------------------------------------------

//! [slot]

void
NsTimeSlider::_onChange()
{
    emit sliderChanged(value(),
                       minimum(),
                       maximum(),
                       _isPlayingFwd(),
                       _isPlayingBwd(),
                       _isLooping());
    update();
}


//! [slot]

void
NsTimeSlider::_onValueChanged(const int value)
{
    if ((_isMinimum(value) || _isMaximum(value)) && !_isLooping()) {
        _stop();
    }

    _onChange();
}


void
NsTimeSlider::_onRangeChanged(const int min, const int max)
{
    emit minimumChanged(min);
    emit maximumChanged(max);
    _onChange();
}


//! [slot]

void
NsTimeSlider::_onLoopChanged(const bool checked)
{
    _looping = checked;
    _onChange();
}


//! [slot]

void
NsTimeSlider::_onActionTriggered(const int action)
{
    // Loop, if enabled.

    switch (action) {
    case SliderSingleStepSub:
        if (_isLooping() && _isPlayingBwd() && _isMinimum(value())) {
            setSliderPosition(maximum());
        }
        break;
    case SliderSingleStepAdd:
        if (_isLooping() && _isPlayingFwd() && _isMaximum(value())) {
            setSliderPosition(minimum());
        }
        break;
    default:
        break;
    };
}

// -----------------------------------------------------------------------------

//! Start playing forward from the current frame with the given interval.

void
NsTimeSlider::_playFwd(const int thresholdTime)
{
    setRepeatAction(SliderSingleStepAdd,
                    thresholdTime,
                    _playRepeatTime());
    _onChange();
}


//! Start playing backward from the current frame with the given interval.

void
NsTimeSlider::_playBwd(const int thresholdTime)
{
    setRepeatAction(SliderSingleStepSub,
                    thresholdTime,
                    _playRepeatTime()); // Repeat time.
    _onChange();
}


//! Stops playing.

void
NsTimeSlider::_stop()
{
    setRepeatAction(SliderNoAction);
    _onChange();
}

// -----------------------------------------------------------------------------

void
NsTimeSlider::_setMinimum(const int min)
{
    if (!_isMinimum(min)) {
        QSlider::setMinimum(min);
    }
}


void
NsTimeSlider::_setMaximum(const int max)
{
    if (!_isMaximum(max)) {
        QSlider::setMaximum(max);
    }
}


void
NsTimeSlider::_setValue(const int val)
{
    if (value() != val) {
        QSlider::setValue(val);
    }
}

// -----------------------------------------------------------------------------

int
NsTimeSlider::_playRepeatTime()
{
    return (1000./evalParam1i("Global.Fps"));
}
