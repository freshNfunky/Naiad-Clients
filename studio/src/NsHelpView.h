// -----------------------------------------------------------------------------
//
// NsHelpView.h
//
// Naiad Studio help view, header file.
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

#ifndef NS_HELP_VIEW_H
#define NS_HELP_VIEW_H

#include <QWebView>

// -----------------------------------------------------------------------------

// NsHelpView
// ----------
//! The NsHelpView class, representing the help view.

class NsHelpView : public QWebView
{
    Q_OBJECT

public:

    static bool 
    createOpHtmlCache();

private:

    static QString
    _opHtmlPath();

    static QString
    _opTypeHtmlPath(const QString &opType);

    static QString
    _defaultPath();

public:

    explicit 
    NsHelpView(QWidget *parent = 0);

    //! DTOR.
    virtual 
    ~NsHelpView() 
    {}    

protected slots:

    void
    onOpSelectionChanged(const QStringList &opInstances,
                         bool               success);

    void
    onGraphCleared(bool success);

private:

    static QUrl
    _createUrl(const QString &path, const QString &scheme = QString("file://"));

    void
    _loadOpType(const QString &typeName);

    void
    _loadDefault();

    void
    _load(const QUrl &url);

private:    // Member variables.

    bool _opPage;
    QString _loadedOpType;
};

#endif // NS_HELP_VIEW_H
