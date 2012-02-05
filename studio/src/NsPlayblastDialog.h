// -----------------------------------------------------------------------------
//
// NsPlayblastDialog.h
//
// Naiad Studio playblast dialog header file.
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

#ifndef NS_PLAYBLAST_DIALOG_H
#define NS_PLAYBLAST_DIALOG_H

#include <QDialog>

class QLineEdit;
class QComboBox;
class QSlider;

// -----------------------------------------------------------------------------

// NsPlayblastDialog
// -----------------
//! The NsPlayblastDialog class, representing the dialog in which playblast
//! options are set before a playblast is performed.

class NsPlayblastDialog : public QDialog
{
    Q_OBJECT

public:

    explicit
    NsPlayblastDialog(const QString &path,
                      const QString &fileName,
                      int            firstFrame,
                      int            lastFrame,
                      int            width,
                      int            height,
                      const QString &defaultFormat = "jpeg",
                      int            qualityMin    = 0,
                      int            qualityMax    = 100,
                      int            qualityValue  = 80,
                      QWidget       *parent        = 0);

    //! DTOR.
    virtual
    ~NsPlayblastDialog()
    {}

public:

    QString
    path() const;

    QString
    fileName() const;

    int
    firstFrame() const;

    int
    lastFrame() const;

    int
    width() const;
    
    int
    height() const;

    QString
    format() const;

    int
    quality() const;

private slots:

    void _onFileButtonClicked();

private:    // Member variables.

    QLineEdit *_pathLineEdit;
    QLineEdit *_fileNameLineEdit;
    QLineEdit *_firstFrameLineEdit;
    QLineEdit *_lastFrameLineEdit;
    QLineEdit *_widthLineEdit;
    QLineEdit *_heightLineEdit;
    QComboBox *_formatComboBox;
    QSlider   *_qualitySlider;
};

#endif // NS_PLAYBLAST_DIALOG_H
