// -----------------------------------------------------------------------------
//
// NsDockWidget.h
//
// Naiad Studio class for dock widgets, header file.
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

#ifndef NS_DOCK_WIDGET_H
#define NS_DOCK_WIDGET_H

#include <QDockWidget>
#include <QString>
#include <QWidget>

// -----------------------------------------------------------------------------

// NsDockWidget
// ------------
//! Class for Open Naiad Studio dock widgets. The window title
//! gets set to a translated string, while the object name remains the
//! untranslated title provided.

class NsDockWidget : public QDockWidget
{
    Q_OBJECT

public:

    //! CTOR.
    NsDockWidget(const QString   &title,
                 QWidget         *parent = 0,
                 Qt::WindowFlags  flags  = 0)
        : QDockWidget(tr(title.toAscii().data()), parent, flags)
    {
        setObjectName(title);   // Not translated.
        //setMinimumWidth(400);
        setSizePolicy(QSizePolicy::MinimumExpanding,
                      QSizePolicy::MinimumExpanding);

        //setStyleSheet("{ background-color: #123522; }");
    }

    //! DTOR
    virtual 
    ~NsDockWidget() 
    {}
};

// -----------------------------------------------------------------------------



// -----------------------------------------------------------------------------

//// NsCmdViewDockWidget
//// -------------------
////! Dock widget for command views.

//class NsCmdViewDockWidget : public NsDockWidgetBase
//{
//    Q_OBJECT

//public:

//    NsCmdViewDockWidget(const QString   &title,
//                        QWidget         *parent = 0,
//                        Qt::WindowFlags  flags  = 0)
//        : NsDockWidgetBase(title, parent, flags)
//    {
////        commandWindow->hide();
////        undoView = new QUndoView(Ns::undoStack(), this);
////        undoView->setEmptyLabel(tr("New Scene"));
////        // TODO: Remove this if we can make arrow key navigation work
////        undoView->setFocusPolicy(Qt::NoFocus);
////        commandWindow->setWidget(undoView);
//    }

//    //! DTOR.
//    virtual ~NsCmdViewDockWidget() {}
//};

// -----------------------------------------------------------------------------


#endif // NS_DOCK_WIDGET_H
