// -----------------------------------------------------------------------------
//
// NsProxyStyle.h
//
// Naiad Studio proxy style header file.
//
// Copyright (c) 2011 Exotic Matter AB.  All rights reserved.
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

#ifndef NS_PROXYSTYLE_H
#define NS_PROXYSTYLE_H

#include <QProxyStyle>

class NsTimeSlider;
class QStyleOptionSlider;

// -----------------------------------------------------------------------------

// NsProxyStyle
// ------------
//! The NsProxyStyle class, used to override some style specific behaviour
//! and/or appearance to make things work the way we want them to, regardless
//! of the style.

class NsProxyStyle : public QProxyStyle
{
public:

    virtual int
    styleHint(StyleHint           hint,
              const QStyleOption *option = 0,
              const QWidget      *widget = 0,
              QStyleHintReturn   *returnData = 0) const;

//    virtual void
//    drawPrimitive(PrimitiveElement    element,
//                  const QStyleOption *option,
//                  QPainter           *painter,
//                  const QWidget      *widget = 0) const;

    virtual void
    drawComplexControl(ComplexControl             control,
                       const QStyleOptionComplex *option,
                       QPainter                  *painter,
                       const QWidget             *widget = 0) const;

private:

    void
    _drawSliderFrames(const QStyleOptionSlider *sos,
                      QPainter                 *painter,
                      const NsTimeSlider       *slider,
                      int                       first,
                      int                       last,
                      const QColor             &color) const;
};

#endif // NS_PROXYSTYLE_H
