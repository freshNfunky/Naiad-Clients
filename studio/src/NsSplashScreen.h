// -----------------------------------------------------------------------------
//
// NsSplashScreen.h
//
// Naiad Studio splash screen, header file.
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

#ifndef NS_SPLASH_SCREEN_H
#define NS_SPLASH_SCREEN_H

#include <QSplashScreen>

// -----------------------------------------------------------------------------

// NsSplashScreen
// --------------
//! The NsSplashScreen class, representing the Naiad Studio splash screen.

class NsSplashScreen : public QSplashScreen
{
    Q_OBJECT

public:

    explicit
    NsSplashScreen(const QPixmap   &pixmap,
                   int              alignment = Qt::AlignBottom |
                                                Qt::AlignHCenter,
                   const QColor    &color     = Qt::black,
                   Qt::WindowFlags  f         = Qt::WindowStaysOnTopHint |
                                                Qt::FramelessWindowHint);

    //! DTOR.
    virtual
    ~NsSplashScreen()
    {}

public:

    const QColor&
    color() const
    { return _color; }

    int
    alignment() const
    { return _alignment; }

    void
    setColor(const QColor &color)
    { _color = color; }

    void
    setAlignment(const int alignment)
    { _alignment = alignment; }

public slots:

    void
    showMessage(const QString &message);

private:    // Member variables.

    int    _alignment;
    QColor _color;

//public slots:

//    void
//    showMessage(const QString &message,
//                int            alignment = Qt::AlignBottom | Qt::AlignHCenter,
//                const QColor  &color     = Qt::white);
};

#endif // NS_SPLASH_SCREEN_H
