// -----------------------------------------------------------------------------
//
// NsApplication.h
//
// Naiad Studio application header file.
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

#ifndef NS_APPLICATION_H
#define NS_APPLICATION_H

#include <QApplication>
#include <QString>
#include <QFileInfo>

// -----------------------------------------------------------------------------

// NsApplication
// -------------
//! The NsApplication class, used to override QApplication::notify() to catch
//! exceptions that for some reason are not caught elsewhere.

class NsApplication : public QApplication
{
    Q_OBJECT

public:

    NsApplication(int &argc, char **argv);

    virtual 
    ~NsApplication();

    //! Overridden to catch unexpected exceptions.
    virtual bool 
    notify(QObject *receiver, QEvent *event);



    const QString&
    fileName() const
    { return _fileName; }

    bool 
    playblast() const
    { return _playblast; }

    int 
    playblastFirstFrame() const
    { return _playblastFirstFrame; }

    int 
    playblastLastFrame() const
    { return _playblastLastFrame; }

    int 
    playblastWidth() const
    { return _playblastWidth; }

    int 
    playblastHeight() const
    { return _playblastHeight; }

    const QString&
    playblastFormat() const
    { return _playblastFormat; }

    int
    playblastQuality() const
    { return _playblastQuality; }

    bool 
    validPlayblastFrames() const
    { return (_playblastFirstFrame <= _playblastLastFrame); }


    QString
    playblastPath() const
    {
        QString path = _fileName;
        if (path.endsWith(".ni")) {
            path = path.left(path.length() - 3);
        }
        return path + ".playblast";
    }

    QString 
    playblastFileName() const
    { return QFileInfo(_fileName).completeBaseName(); }


    bool
    help() const
    { return _help; }

protected slots:

    void
    onAboutToQuit();

private:

    void 
    _parseArgs(const QStringList &args);

    void 
    _loadStyleSheet(const QString &fileName);
    
    void 
    _registerScopes();

private:

    static const QString _settingsGroup;

    void
    _readSettings();

    void
    _writeSettings();

private:

    static const int     _defaultPlayblastWidth;
    static const int     _defaultPlayblastHeight;
    static const QString _defaultPlayblastFormat;
    static const int     _defaultPlayblastQuality;

private:    // Member variables.

    QString _fileName;

    bool    _playblast;
    int     _playblastFirstFrame;
    int     _playblastLastFrame;
    int     _playblastWidth;
    int     _playblastHeight;
    QString _playblastFormat;
    int     _playblastQuality;

    bool _help;

    //bool frameOverride = false;
};

#endif // NS_APPLICATION_H
