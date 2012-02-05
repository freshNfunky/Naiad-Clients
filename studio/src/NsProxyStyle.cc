// -----------------------------------------------------------------------------
//
// NsProxyStyle.cc
//
// Naiad Studio proxy style source file.
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

#include "NsProxyStyle.h"
//#include "NsGroupBox.h"
#include "NsTimeSlider.h"
#include "NsQuery.h"
#include <QtGui>

// -----------------------------------------------------------------------------

// styleHint
// ---------
//! Overridden to make sliders always go to clicked position on left button
//! mouse clicks

int
NsProxyStyle::styleHint(StyleHint           hint,
                        const QStyleOption *option,
                        const QWidget      *widget,
                        QStyleHintReturn   *returnData) const
{
    int value(QProxyStyle::styleHint(hint, option, widget, returnData));

    if (hint == QStyle::SH_Slider_AbsoluteSetButtons) {
        value |= Qt::LeftButton;
    }

    return value;
}

// -----------------------------------------------------------------------------

//// drawPrimitive
//// -------------
////! Overridden to draw collapsible group box check boxes as arrows.

//void
//NsProxyStyle::drawPrimitive(PrimitiveElement element,
//                            const QStyleOption *option,
//                            QPainter *painter,
//                            const QWidget *widget) const
//{
//    if (element == PE_IndicatorCheckBox &&
//        qobject_cast<const NsGroupBox *>(widget)) {
//        if (option->state & State_On)
//            element = PE_IndicatorArrowDown;
//        else
//            element = PE_IndicatorArrowUp;
//    }

//    QProxyStyle::drawPrimitive(element, option, painter, widget);
//}

// -----------------------------------------------------------------------------

// drawComplexControl
// ------------------
//! Overridden to show valid frames in green behind timeline slider.

void
NsProxyStyle::drawComplexControl(ComplexControl             control,
                                 const QStyleOptionComplex *option,
                                 QPainter                  *painter,
                                 const QWidget             *widget) const
{
    const NsTimeSlider *slider(qobject_cast<const NsTimeSlider *>(widget));

    if (control == CC_Slider && 0 != slider) {
        const QStyleOptionSlider *sos(
            qstyleoption_cast<const QStyleOptionSlider *>(option));

        if (0 != sos) {
            // Draw range.
            // TODO: Eval times for first and last step frames?!
            //       Should it even be possible for these to be expressions?!

            _drawSliderFrames(sos,
                              painter,
                              slider,
                              evalParam1i("Global.First Frame"),
                              evalParam1i("Global.Last Frame"),
                              //slider->firstStepFrame(),
                              //slider->lastStepFrame(),
                              QColor(255, 215, 0));


            if (queryStepped()) {
                // Draw live frame if any stepping has occured.

                const int liveFrame(queryTimeBundle().frame);

                _drawSliderFrames(sos,
                                  painter,
                                  slider,
                                  liveFrame, //slider->liveFrame()
                                  liveFrame, //slider->liveFrame()
                                  QColor(173, 255, 47));
            }
        }
    }

    QProxyStyle::drawComplexControl(control, option, painter, widget);
}


// drawSliderFrames
// ----------------
//! Draws the given frames on the given slider in the given color.

void
NsProxyStyle::_drawSliderFrames(const QStyleOptionSlider *sos,
                                QPainter                 *painter,
                                const NsTimeSlider       *slider,
                                const int                 firstFrame,
                                const int                 lastFrame,
                                const QColor             &color) const
{
    const int first(qMax(firstFrame, slider->firstVisibleFrame()));
    const int last(qMin(lastFrame, slider->lastVisibleFrame() - 1));

    if (first <= last) {
        const int length(
            proxy()->pixelMetric(PM_SliderLength,
                                 sos,
                                 slider));

        const int space(sos->rect.width() - length);
        const int halfLength(length/2);

        const int start(
            sliderPositionFromValue(sos->minimum,
                                    sos->maximum,
                                    first,
                                    space) + halfLength);

        const int end(
            sliderPositionFromValue(sos->minimum,
                                    sos->maximum,
                                    last + 1,
                                    space) + halfLength);

        painter->fillRect(start,
                          sos->rect.top(),
                          end - start,
                          sos->rect.bottom() - sos->rect.top() + 1,
                          color);
    }
}
