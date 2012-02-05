// -----------------------------------------------------------------------------
//
// NsValueBaseWidget.h
//
// Naiad Studio value base widget, header file.
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

#ifndef NS_VALUE_BASE_WIDGET_H
#define NS_VALUE_BASE_WIDGET_H

#include <NiTypes.h>
#include <QRegExp>
#include <QWidget>

class NsValueWidgetArgs;
class NsValueObject;
class NsValueBaseObject;
class NsValueWidgetLayout;
class NsValueLineEdit;
class QMenu;

// -----------------------------------------------------------------------------

// NsValueBaseWidget
// -----------------
//! Abstract base for classes representing values visually in the value editor.

class NsValueBaseWidget : public QWidget
{
    Q_OBJECT

public:

    virtual
    ~NsValueBaseWidget() = 0;     // Force class to be abstract.

    virtual QString
    stringEval(const NtTimeBundle &tb,
               int                 comp     = 0,
               int                 threadId = 0) const = 0;

    virtual NsValueObject*
    valueObject() const;

    virtual void
    addContextActions(QMenu &menu, int comp) const;

public:

    NsValueBaseObject*
    valueBaseObject() const;

protected slots:

    virtual void
    onModeActionTriggered(QAction *action) = 0;

protected:

    explicit
    NsValueBaseWidget(const NsValueWidgetArgs &args);

    NsValueWidgetLayout*
    valueWidgetLayout() const
    { return _vwl; }

private:    // Member variables.

    NsValueObject       *_vo;   //!< Resource.
    NsValueBaseObject   *_vbo;  //!< Resource.
    NsValueWidgetLayout *_vwl;  //!< Layout.

private:

    NsValueBaseWidget();                                    //!< Disabled.
    NsValueBaseWidget(const NsValueBaseWidget&);            //!< Disabled.
    NsValueBaseWidget& operator=(const NsValueBaseWidget&); //!< Disabled.
};

// -----------------------------------------------------------------------------

#endif // NS_VALUE_BASE_WIDGET_H
