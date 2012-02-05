// -----------------------------------------------------------------------------
//
// NsPath.cc
//
// Naiad Studio path, source file.
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

#include "NsPath.h"
#include "NsQuery.h"
#include <QDir>
#include <QFileInfo>

// -----------------------------------------------------------------------------

// makeRelative
// ------------
//! Returns 'path' with 'relativeTo' removed if 'path' starts with 'relativeTo'.
//! [static]

QString
NsPath::makeRelative(const QString &path, const QString &relativeTo)
{
    if (relativeTo.isEmpty()) {
        return path;
    }

    QString fnsrt(QDir::fromNativeSeparators(relativeTo));
    if (!fnsrt.endsWith("/")) {
        fnsrt += "/";    
    }        

    const QString fnsp(QDir::fromNativeSeparators(path));

    return fnsp.startsWith(fnsrt) ? 
        fnsp.right(fnsp.length() - fnsrt.length()) : 
        fnsp;  
}

// -----------------------------------------------------------------------------

// makeAbsolute
// ------------
//! Returns 'path' with 'base' added if 'path' is not an absolute path. Note 
//! that base does not have to be an absolute path. [static]

QString
NsPath::makeAbsolute(const QString &path, const QString &base)
{
    if (QFileInfo(path).isAbsolute()) {
        return path;
    }

    QString fnsb(QDir::fromNativeSeparators(base));
    if (!fnsb.endsWith("/")) {
        fnsb += "/";
    }

    return fnsb + path;
}

// -----------------------------------------------------------------------------

QString	
NsPath::getExistingDirectory(QWidget				    *parent, 
                             const QString		        &caption, 
                             const QString		        &dir, 
                             const QFileDialog::Options  options)
{ 
    return QFileDialog::getExistingDirectory(parent, 
                                             caption, 
                                             dir.isEmpty() ? _defaultDir():dir, 
                                             options); 
}

// -----------------------------------------------------------------------------

QString
NsPath::getOpenFileName(QWidget                    *parent, 
                        const QString              &caption, 
                        const QString              &dir, 
                        const QString              &filter, 
                        QString                    *selectedFilter, 
                        const QFileDialog::Options  options)
{
    return QFileDialog::getOpenFileName(parent, 
                                        caption, 
                                        dir.isEmpty() ? _defaultDir() : dir, 
                                        filter, 
                                        selectedFilter, 
                                        options);
}

// -----------------------------------------------------------------------------

QStringList	
NsPath::getOpenFileNames(QWidget                    *parent, 
                         const QString              &caption, 
                         const QString              &dir, 
                         const QString              &filter, 
                         QString                    *selectedFilter, 
                         const QFileDialog::Options  options)
{
    return QFileDialog::getOpenFileNames(parent, 
                                         caption, 
                                         dir.isEmpty() ? _defaultDir() : dir, 
                                         filter, 
                                         selectedFilter, 
                                         options);
}

// -----------------------------------------------------------------------------

QString	
NsPath::getSaveFileName(QWidget                    *parent, 
                        const QString              &caption, 
                        const QString              &dir, 
                        const QString              &filter, 
                        QString                    *selectedFilter, 
                        const QFileDialog::Options  options)
{
    return QFileDialog::getSaveFileName(parent, 
                                        caption, 
                                        dir.isEmpty() ? _defaultDir() : dir, 
                                        filter, 
                                        selectedFilter, 
                                        options);
}


QString	
NsPath::getFileName(QWidget                    *parent, 
                    const QString              &caption, 
                    const QString              &dir, 
                    const QString              &filter, 
                    QString                    *selectedFilter, 
                    const QFileDialog::Options  options)
{
    QFileDialog fileDialog(parent, caption, dir);
    fileDialog.setLabelText(QFileDialog::Accept, QObject::tr("&Choose"));
    fileDialog.setNameFilter(filter);

    return (QDialog::Accepted != fileDialog.exec()) ?
                QString() :     // User pressed cancel-button.
                fileDialog.selectedFiles().value(0);
}


// -----------------------------------------------------------------------------

QString
NsPath::_defaultDir()
{
    return evalParam1s("Global.Project Path");
}
