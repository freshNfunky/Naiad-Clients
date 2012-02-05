// -----------------------------------------------------------------------------
//
// NsTimeToolBar.cc
//
// Naiad Studio time tool bar, source file.
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

#include "NsTimeToolBar.h"
#include "NsTimeSlider.h"
#include "NsTimeLineEdit.h"
#include "NsTimeToolBar.h"
#include "NsTimeAction.h"
#include "NsGraphCallback.h"
#include "NsCmdCentral.h"
#include "NsCmdSetCurrentVisibleFrame.h"
#include "NsCmdSetFirstVisibleFrame.h"
#include "NsCmdSetLastVisibleFrame.h"
#include "NsCmdSelectAll.h"
#include "NsCmdSelectAllBodies.h"
#include "NsMessageWidget.h"
#include "NsOpStore.h"
#include "NsOpObject.h"
#include "NsQuery.h"
#include <NiQuery.h>
#include <NiStep.h>
#include <QtGui>

// -----------------------------------------------------------------------------

//! [static]
const QString
NsTimeToolBar::_currentVisibleFrameMetaValueType("Current Visible Frame");

//! [static]
const QString
NsTimeToolBar::_firstVisibleFrameMetaValueType("First Visible Frame");

//! [static]
const QString
NsTimeToolBar::_lastVisibleFrameMetaValueType("Last Visible Frame");

// -----------------------------------------------------------------------------

// NsTimeToolBar
// -------------
//! CTOR.

NsTimeToolBar::NsTimeToolBar(QAction     *naiadAction, 
                             NsUndoStack *undoStack, 
                             QWidget     *parent)
    : QToolBar(tr("Time"), parent)
    , _undoStack(undoStack)
    , _timeSlider(new NsTimeSlider(this)) // Child.
    , _firstVisibleFrameAction(_timeSlider->createFirstVisibleFrameAction(this))
    , _previousFrameAction(_timeSlider->createPreviousFrameAction(this))
    , _playBwdAction(_timeSlider->createPlayBwdAction(this))
    , _playFwdAction(_timeSlider->createPlayFwdAction(this))
    , _nextFrameAction(_timeSlider->createNextFrameAction(this))
    , _lastVisibleFrameAction(_timeSlider->createLastVisibleFrameAction(this))
    , _loopAction(_timeSlider->createLoopAction(this))
    , _resetAction(new NsStepAction(tr("Reset Stepping"), this))
          //new NsStepAction(
          //    tr("Reset Stepping"),
          //    ":/images/reset-solver-18.png",
          //    ":/images/reset-solver-18.png",
          //    QSize(18, 18),
          //    QKeySequence(Qt::Key_F2),
          //    false,
          //    this))
    , _stepToLastAction(new NsStepAction(tr("Step to last frame"), this))
          //new NsStepAction(
          //    tr("Step to last frame"),
          //    ":/images/solve-from-18.png",
          //    ":/images/solve-from-18.png",
          //    QSize(18, 18),
          //    QKeySequence(Qt::Key_F6),
          //    false,
          //    this))
    , _stepSingleAction(new NsStepAction(tr("Step frame"), this))
              //tr("Step frame"),
              //":/images/solve-18.png",
              //":/images/solve-18.png",
              //QSize(18, 18),
              //QKeySequence(Qt::Key_F3),
              //false,
              //this))
    , _restepAction(new NsStepAction(tr("Restep frame"), this))
          //new NsStepAction(
          //    tr("Restep frame"),
          //    ":/images/restep-18.png",
          //    ":/images/restep-18.png",
          //    QSize(18, 18),
          //    QKeySequence(Qt::Key_F4),
          //    false,
          //    this))
    , _stepVisibleAction(new NsStepAction(tr("Step visible frame"), this))
          //new NsStepAction(
          //    tr("Step visible frame"),
          //    ":/images/step-visible-18.png",
          //    ":/images/step-visible-18.png",
          //    QSize(18, 18),
          //    QKeySequence(Qt::Key_F5),
          //    false,
          //    this))
    , _stopAction(new NsStepAction(tr("Stop stepping"), this)) // Child.
          //new NsStepAction(
          //    tr("Stop stepping"),
          //    ":/images/stop-18.png",
          //    ":/images/stop-18.png",
          //    QSize(18, 18),
          //    QKeySequence(Qt::Key_Escape),
          //    false,
          //    this))
{

    {
    QIcon icon;
    icon.addFile(
        ":/images/step_reset-enabled.png",
        QSize(), QIcon::Normal, QIcon::Off);
    //icon.addFile(
    //    ":/images/step_reset-pressed.png",
    //    QSize(), QIcon::Active, QIcon::Off);
    icon.addFile(
        ":/images/step_reset-disabled.png", 
        QSize(), QIcon::Disabled, QIcon::Off);
    _resetAction->setIcon(icon);
    _resetAction->setCheckable(false);
    _resetAction->setShortcuts(QList<QKeySequence>() << Qt::Key_F2);
    _resetAction->setStatusTip(
       _resetAction->text() + " [" + _resetAction->shortcut().toString() + "]");
    }

    {
    QIcon icon;
    icon.addFile(
        ":/images/step_play-enabled.png",
        QSize(), QIcon::Normal, QIcon::Off);
    //icon.addFile(
    //    ":/images/step_reset-pressed.png",
    //    QSize(), QIcon::Active, QIcon::Off);
    icon.addFile(
        ":/images/step_play-disabled.png", 
        QSize(), QIcon::Disabled, QIcon::Off);
    _stepToLastAction->setIcon(icon);
    _stepToLastAction->setCheckable(false);
    _stepToLastAction->setShortcuts(QList<QKeySequence>() << Qt::Key_F6);
    _stepToLastAction->setStatusTip(
       _stepToLastAction->text() + " [" + 
       _stepToLastAction->shortcut().toString() + "]");
    }

    {
    QIcon icon;
    icon.addFile(
        ":/images/step_single-enabled.png",
        QSize(), QIcon::Normal, QIcon::Off);
    //icon.addFile(
    //    ":/images/step_reset-pressed.png",
    //    QSize(), QIcon::Active, QIcon::Off);
    icon.addFile(
        ":/images/step_single-disabled.png", 
        QSize(), QIcon::Disabled, QIcon::Off);
    _stepSingleAction->setIcon(icon);
    _stepSingleAction->setCheckable(false);
    _stepSingleAction->setShortcuts(QList<QKeySequence>() << Qt::Key_F3);
    _stepSingleAction->setStatusTip(
       _stepSingleAction->text() + " [" + 
       _stepSingleAction->shortcut().toString() + "]");
    }

    {
    QIcon icon;
    icon.addFile(
        ":/images/step_refresh-enabled.png",
        QSize(), QIcon::Normal, QIcon::Off);
    //icon.addFile(
    //    ":/images/step_reset-pressed.png",
    //    QSize(), QIcon::Active, QIcon::Off);
    icon.addFile(
        ":/images/step_refresh-disabled.png", 
        QSize(), QIcon::Disabled, QIcon::Off);
    _restepAction->setIcon(icon);
    _restepAction->setCheckable(false);
    _restepAction->setShortcuts(QList<QKeySequence>() << Qt::Key_F4);
    _restepAction->setStatusTip(
       _restepAction->text() + " [" + 
       _restepAction->shortcut().toString() + "]");
    }

    {
    QIcon icon;
    icon.addFile(
        ":/images/step_visible-enabled.png",
        QSize(), QIcon::Normal, QIcon::Off);
    //icon.addFile(
    //    ":/images/step_reset-pressed.png",
    //    QSize(), QIcon::Active, QIcon::Off);
    icon.addFile(
        ":/images/step_visible-disabled.png", 
        QSize(), QIcon::Disabled, QIcon::Off);
    _stepVisibleAction->setIcon(icon);
    _stepVisibleAction->setCheckable(false);
    _stepVisibleAction->setShortcuts(QList<QKeySequence>() << Qt::Key_F5);
    _stepVisibleAction->setStatusTip(
       _stepVisibleAction->text() + " [" + 
       _stepVisibleAction->shortcut().toString() + "]");
    }

    {
    QIcon icon;
    icon.addFile(
        ":/images/step_stop-enabled.png",
        QSize(), QIcon::Normal, QIcon::Off);
    //icon.addFile(
    //    ":/images/step_reset-pressed.png",
    //    QSize(), QIcon::Active, QIcon::Off);
    icon.addFile(
        ":/images/step_stop-disabled.png", 
        QSize(), QIcon::Disabled, QIcon::Off);
    _stopAction->setIcon(icon);
    _stopAction->setCheckable(false);
    _stopAction->setShortcuts(QList<QKeySequence>() << Qt::Key_Escape);
    _stopAction->setStatusTip(
       _stopAction->text() + " [" + 
       _stopAction->shortcut().toString() + "]");
    }


    // Connect slider.

    connect(_timeSlider, SIGNAL(valueChanged(int)),
            this,        SLOT(_onSliderValueChanged(int)));
    connect(_timeSlider, SIGNAL(minimumChanged(int)),
            this,        SLOT(_onSliderMinimumChanged(int)));
    connect(_timeSlider, SIGNAL(maximumChanged(int)),
            this,        SLOT(_onSliderMaximumChanged(int)));

    // Create and connect line edits to slider.

    NsTimeLineEdit *firstVisibleFrame(new NsTimeLineEdit(this)); // Child.
    connect(firstVisibleFrame, SIGNAL(valueChanged(int)),
            _timeSlider,       SLOT(onMinimumChanged(int)));
    connect(_timeSlider,       SIGNAL(minimumChanged(int)),
            firstVisibleFrame, SLOT(onValueChanged(int)));
    firstVisibleFrame->onValueChanged(_timeSlider->minimum());

    NsTimeLineEdit *lastVisibleFrame(new NsTimeLineEdit(this));  // Child.
    connect(lastVisibleFrame,  SIGNAL(valueChanged(int)),
            _timeSlider,       SLOT(onMaximumChanged(int)));
    connect(_timeSlider,       SIGNAL(maximumChanged(int)),
            lastVisibleFrame,  SLOT(onValueChanged(int)));
    lastVisibleFrame->onValueChanged(_timeSlider->maximum());

    NsTimeLineEdit *currFrame(new NsTimeLineEdit(this));  // Child.
    connect(currFrame,   SIGNAL(valueChanged(int)),
            _timeSlider, SLOT(onValueChanged(int)));
    connect(_timeSlider, SIGNAL(valueChanged(int)),
            currFrame,   SLOT(onValueChanged(int)));
    currFrame->onValueChanged(_timeSlider->value());


    // Connect step actions.

    connect(_resetAction,      SIGNAL(triggered()),
            this,              SLOT(_reset()));
    connect(NsGraphCallback::instance(),
            SIGNAL(beginStep(NtTimeBundle)),
            _resetAction,
            SLOT(onBeginStep(NtTimeBundle)));
    connect(NsGraphCallback::instance(),
            SIGNAL(endStep(NtTimeBundle)),
            _resetAction,
            SLOT(onEndStep(NtTimeBundle)));

    connect(_stepToLastAction, SIGNAL(triggered()),
            this,              SLOT(_stepToLast()));
    connect(NsGraphCallback::instance(),
            SIGNAL(beginStep(NtTimeBundle)),
            _stepToLastAction,
            SLOT(onBeginStep(NtTimeBundle)));
    connect(NsGraphCallback::instance(),
            SIGNAL(endStep(NtTimeBundle)),
            _stepToLastAction,
            SLOT(onEndStep(NtTimeBundle)));

    connect(_stepSingleAction, SIGNAL(triggered()),
            this,              SLOT(_stepSingle()));
    connect(NsGraphCallback::instance(),
            SIGNAL(beginStep(NtTimeBundle)),
            _stepSingleAction,
            SLOT(onBeginStep(NtTimeBundle)));
    connect(NsGraphCallback::instance(),
            SIGNAL(endStep(NtTimeBundle)),
            _stepSingleAction,
            SLOT(onEndStep(NtTimeBundle)));

    connect(_restepAction,     SIGNAL(triggered()),
            this,              SLOT(_restep()));
    connect(NsGraphCallback::instance(),
            SIGNAL(beginStep(NtTimeBundle)),
            _restepAction,
            SLOT(onBeginStep(NtTimeBundle)));
    connect(NsGraphCallback::instance(),
            SIGNAL(endStep(NtTimeBundle)),
            _restepAction,
            SLOT(onEndStep(NtTimeBundle)));

    connect(_stepVisibleAction, SIGNAL(triggered()),
            this,               SLOT(_stepVisible()));
    connect(NsGraphCallback::instance(), SIGNAL(beginStep(NtTimeBundle)),
            _stepVisibleAction,          SLOT(onBeginStep(NtTimeBundle)));
    connect(NsGraphCallback::instance(), SIGNAL(endStep(NtTimeBundle)),
            _stepVisibleAction,          SLOT(onEndStep(NtTimeBundle)));

    _stopAction->setEnabled(false);
    connect(_stopAction, SIGNAL(triggered()),
            this,        SLOT(_stopStepping()));
    connect(NsGraphCallback::instance(), SIGNAL(beginStep(NtTimeBundle)),
            _stopAction,                 SLOT(onBeginStep(NtTimeBundle)));
    connect(NsGraphCallback::instance(), SIGNAL(endStep(NtTimeBundle)),
            _stopAction,                 SLOT(onEndStep(NtTimeBundle)));


    // Add widgets to tool bar.

    addAction(_firstVisibleFrameAction);
    addAction(_previousFrameAction);
    addAction(_playBwdAction);
    addWidget(currFrame);
    addAction(_playFwdAction);
    addAction(_nextFrameAction);
    addAction(_lastVisibleFrameAction);
    addAction(_loopAction);
    //addSeparator(); // ----------
    addWidget(firstVisibleFrame);
    //addSeparator(); // ----------
    addWidget(_timeSlider);
    //addSeparator(); // ----------
    addWidget(lastVisibleFrame);
    //addSeparator(); // ----------
    addAction(naiadAction);
    //addSeparator(); // ----------
    addAction(_resetAction);
    addAction(_stepToLastAction);
    addAction(_stepSingleAction);
    addAction(_restepAction);
    addAction(_stepVisibleAction);
    addAction(_stopAction);
}

// -----------------------------------------------------------------------------

// paintEvent
// ----------
//! Must be overriden for stylesheets (CSS) to work.

void
NsTimeToolBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


void
NsTimeToolBar::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "NsTimeToolBar::keyPressEvent";
    QToolBar::keyPressEvent(event);
}

// -----------------------------------------------------------------------------

// onValueChanged
// --------------
//! [slot]

void
NsTimeToolBar::onValueChanged(const QString &paramLongName,
                              const QString &expr,
                              const int      component,
                              const bool     success)
{
    if (success) {
        update();
    }
}


// onMetaChanged
// -------------
//! [slot]

void
NsTimeToolBar::onMetaChanged(const QString &longName,
                             const QString &valueType,
                             const QString &value,
                             const bool     success)
{
    if (success && "Global" == longName) {
        bool ok(false);

        if (firstVisibleFrameMetaValueType() == valueType) {
            const int fvf(value.toInt(&ok));

            if (ok && _timeSlider->firstVisibleFrame() != fvf) {
                _timeSlider->setFirstVisibleFrame(fvf);
                update();
            }
        }
        else if (lastVisibleFrameMetaValueType() == valueType) {
            const int lvf(value.toInt(&ok));

            if (ok && _timeSlider->lastVisibleFrame() != lvf) {
                _timeSlider->setLastVisibleFrame(lvf);
                update();
            }
        }
        else if (currentVisibleFrameMetaValueType() == valueType) {
            const int cvf(value.toInt(&ok));

            if (ok && _timeSlider->currentVisibleFrame() != cvf) {
                _timeSlider->setCurrentVisibleFrame(cvf);
                update();
            }
        }
    }
}


// onFirstVisibleFrameChanged
// --------------------------
//! [slot]

void
NsTimeToolBar::onFirstVisibleFrameChanged(const int fvf, const bool success)
{
    if (success && _timeSlider->firstVisibleFrame() != fvf) {
        _timeSlider->setFirstVisibleFrame(fvf);
        update();
    }
}


// onLastVisibleFrameChanged
// -------------------------
//! [slot]

void
NsTimeToolBar::onLastVisibleFrameChanged(const int lvf, const bool success)
{
    if (success && _timeSlider->lastVisibleFrame() != lvf) {
        _timeSlider->setLastVisibleFrame(lvf);
        update();
    }
}


// onCurrentVisibleFrameChanged
// ----------------------------
//! [slot]

void
NsTimeToolBar::onCurrentVisibleFrameChanged(const int cvf, 
                                            const bool update3DView,
                                            const bool success)
{
    Q_UNUSED(update3DView);

    if (success && _timeSlider->currentVisibleFrame() != cvf) {
        _timeSlider->setCurrentVisibleFrame(cvf);
        update();
    }
}

// -----------------------------------------------------------------------------

// _reset
// ------
//! [slot]

void
NsTimeToolBar::_reset(const bool force)
{
    // TODO: eval time == 0?

    if (NI_TRUE == NiReset(evalParam1i("Global.First Frame"), force)) {
        qDebug() << "reset";

        NsCmdSelectAll::exec(false);

        //NsOpStore::instance()->sync();

        const NtTimeBundle cstb(NiQueryTimeBundle());
        const int cvf(cstb.frame + 1); // pad forward, since server will be at First Frame - 1

        qDebug() << "CVF:" << cvf;

        // Adjust first visible frame if necessary.

        const QString fvfMeta(queryMeta("Global", "First Visible Frame"));
        bool fvfOk(false);
        const int fvf(fvfMeta.toInt(&fvfOk));
        qDebug() << "FVF:" << fvf;

        if (fvfOk && cvf < fvf) {
            NsCmdSetFirstVisibleFrame::exec(cvf);
        }

        // Adjust last visible frame if necessary.

        const QString lvfMeta(queryMeta("Global", "Last Visible Frame"));
        bool lvfOk(false);
        const int lvf(lvfMeta.toInt(&lvfOk));
        qDebug() << "LVF:" << lvf;

        if (lvfOk && cvf > lvf) {
            NsCmdSetLastVisibleFrame::exec(cvf + 1);
        }

        NsCmdSetCurrentVisibleFrame::exec(cvf);
    }
}


// _stepTo
// -------
//! [slot]

void
NsTimeToolBar::_stepToLast()
{
    qDebug() << "step to";

    NsGraphCallback::instance()->setSteppingStopped(false);
    NiStepTo(evalParam1i("Global.Last Frame"));
    update();
}


// _stepFrame
// ----------
//! [slot]

void
NsTimeToolBar::_stepSingle()
{
    const NtTimeBundle cstb0(NiQueryTimeBundle());
#if 0
    qDebug() << "Pre Server:" << cstb0.frame << "|" << cstb0.timestep;
    qDebug() << "Pre Client:" << _frameSlider->currentFrame() << "|" << 0;
    qDebug() << "Pre Slider:" << _frameSlider->currentFrame();
#endif

    qDebug() << "step frame:" << cstb0.frame + 1;

    NsGraphCallback::instance()->setSteppingStopped(false);
    NiStepTo(cstb0.frame + 1);
    update();

#if 0
    const NtTimeBundle cstb1(NiQueryTimeBundle());
    qDebug() << "Post Server:" << cstb1.frame << "|" << cstb1.timestep;
    qDebug() << "Post Client:" << _frameSlider->currentFrame() << "|" << 0;
    qDebug() << "Post Slider:" << _frameSlider->currentFrame();
#endif
}


// _restep
// -------
//! Re-step the current frame. [slot]

void
NsTimeToolBar::_restep()
{
#if 0
    const NtTimeBundle cstb0(NiQueryTimeBundle());
    qDebug() << "Pre Server:" << cstb0.frame << "|" << cstb0.timestep;
    qDebug() << "Pre Client:" << _frameSlider->currentFrame() << "|" << 0;
    qDebug() << "Pre Slider:" << _frameSlider->currentFrame();
#endif

    qDebug() << "restep";

    NsGraphCallback::instance()->setSteppingStopped(false);
    NiRestep();
    update();

#if 0
    const NtTimeBundle cstb1(NiQueryTimeBundle());
    qDebug() << "Post Server:" << cstb1.frame << "|" << cstb1.timestep;
    qDebug() << "Post Client:" << _frameSlider->currentFrame() << "|" << 0;
    qDebug() << "Post Slider:" << _frameSlider->sliderPosition();
#endif
}


// _stepVisible
// ------------
//! Step the current visible frame. [slot]

void
NsTimeToolBar::_stepVisible(const bool force)
{
    // TODO: eval time == 0?

    int cvf(0);
    if (queryCurrentVisibleFrame(&cvf)) {
        if (NI_TRUE == NiReset(cvf, force)) {
            NsCmdSelectAll::exec(false);

            NsGraphCallback::instance()->setSteppingStopped(false);
            NiStepTo(cvf, NI_TRUE);
            update();
        }
    }
}


void
NsTimeToolBar::_stopStepping()
{
    if (!NsGraphCallback::instance()->steppingStopped()) {
        // TODO: Use time toolbar actions to stop solve!

        NsGraphCallback::instance()->setSteppingStopped(true);
        update();
    }
}


//! [slot]

void
NsTimeToolBar::_onSliderValueChanged(const int value)
{
    int cvf(0);
    if (queryCurrentVisibleFrame(&cvf) && value != cvf) {
        NsCmdSelectAllBodies::exec(false);

        if (0 != undoStack()) {
            NsCmdSetCurrentVisibleFrame::exec(value, *undoStack(), true);
        }
        else {
            NsCmdSetCurrentVisibleFrame::exec(value);
        }
    }
}


void
NsTimeToolBar::_onSliderMinimumChanged(const int min)
{
    int fvf(0);
    if (queryFirstVisibleFrame(&fvf) && min != fvf) {
        NsCmdSelectAllBodies::exec(false);

        if (0 != undoStack()) {
            NsCmdSetFirstVisibleFrame::exec(min, *undoStack(), true);
        }
        else {
            NsCmdSetFirstVisibleFrame::exec(min);
        }
    }
}


void
NsTimeToolBar::_onSliderMaximumChanged(const int max)
{
    int lvf(0);
    if (queryLastVisibleFrame(&lvf) && max != lvf) {
        NsCmdSelectAllBodies::exec(false);

        if (0 != undoStack()) {
            NsCmdSetLastVisibleFrame::exec(max, *undoStack(), true);
        }
        else {
            NsCmdSetLastVisibleFrame::exec(max);
        }
    }
}

// -----------------------------------------------------------------------------
