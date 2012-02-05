// -----------------------------------------------------------------------------
//
// NsPlayblastDialog.cc
//
// Naiad Studio playblast dialog source file.
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

#include "NsPlayblastDialog.h"
#include "NsIntValidator.h"
#include "NsQuery.h"
#include <QtGui>
//#include "NsParameterFileDialog.h"
//#include "NsIconStore.h"

// -----------------------------------------------------------------------------

// NsPlayblastDialog
// -----------------
//! CTOR.

NsPlayblastDialog::NsPlayblastDialog(const QString &path,
                                     const QString &fileName,
                                     const int      firstFrame,
                                     const int      lastFrame,
                                     const int      width,
                                     const int      height,
                                     const QString &defaultFormat,
                                     const int      qualityMin,
                                     const int      qualityMax,
                                     const int      qualityValue,
                                     QWidget       *parent)
    : QDialog(parent)
    , _pathLineEdit(new QLineEdit)
    , _fileNameLineEdit(new QLineEdit)
    , _firstFrameLineEdit(new QLineEdit)
    , _lastFrameLineEdit(new QLineEdit)
    , _widthLineEdit(new QLineEdit)
    , _heightLineEdit(new QLineEdit)
    , _formatComboBox(new QComboBox)
    , _qualitySlider(new QSlider)
{
    setWindowTitle("Playblast Options");

    // Create layout.

    QGridLayout *layout = new QGridLayout;
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 1);
    layout->setColumnStretch(2, 0);
    for (int i = 0; i < 7; ++i) {
        layout->setRowStretch(i, 0);
    }
    layout->setRowStretch(5, 1);

    const Qt::Alignment labelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // Path.

    int row = 0;

    QLabel *pathLabel = new QLabel(tr("Path"));

    _pathLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    _pathLineEdit->setText(path);

    layout->addWidget(pathLabel, row, 0, labelAlignment);
    layout->addWidget(_pathLineEdit, row, 1);

    QToolButton *fileButton = new QToolButton;
    fileButton->setIcon(QIcon(":/images/folder-16.png"));
    fileButton->setIconSize(QSize(16, 16));
    fileButton->setToolTip(tr("Browse..."));
    layout->addWidget(fileButton, row, 2);
    connect(fileButton,
            SIGNAL(clicked()),
            SLOT(_onFileButtonClicked()));

    // File name.

    ++row;

    QLabel *fileNameLabel = new QLabel(tr("File name"));
    layout->addWidget(fileNameLabel, row, 0, labelAlignment);

    _fileNameLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    _fileNameLineEdit->setText(fileName);
    layout->addWidget(_fileNameLineEdit, row, 1);

    // First frame.

    ++row;

    QLabel *firstFrameLabel(new QLabel(tr("First Frame")));
    layout->addWidget(firstFrameLabel, row, 0, labelAlignment);

    _firstFrameLineEdit->setText(QString::number(firstFrame));
    _firstFrameLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    _firstFrameLineEdit->setValidator(new NsIntValidator(_lastFrameLineEdit));

    layout->addWidget(_firstFrameLineEdit, row, 1);

    // Last frame.

    ++row;

    QLabel *lastFrameLabel(new QLabel(tr("Last Frame")));
    layout->addWidget(lastFrameLabel, row, 0, labelAlignment);

    _lastFrameLineEdit->setText(QString::number(lastFrame));
    _lastFrameLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    _lastFrameLineEdit->setValidator(new NsIntValidator(_lastFrameLineEdit));
    layout->addWidget(_lastFrameLineEdit, row, 1);

    // Width.

    ++row;

    QLabel *widthLabel(new QLabel(tr("Width")));
    layout->addWidget(widthLabel, row, 0, labelAlignment);

    _widthLineEdit->setText(QString::number(width));
    _widthLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    _widthLineEdit->setValidator(new NsIntValidator(_widthLineEdit));
    layout->addWidget(_widthLineEdit, row, 1);

    // Height.

    ++row;

    QLabel *heightLabel(new QLabel(tr("Height")));
    layout->addWidget(heightLabel, row, 0, labelAlignment);

    _heightLineEdit->setText(QString::number(height));
    _heightLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    _heightLineEdit->setValidator(new NsIntValidator(_heightLineEdit));
    layout->addWidget(_heightLineEdit, row, 1);

    // Format.

    ++row;

    QLabel *formatLabel = new QLabel(tr("Format"));
    layout->addWidget(formatLabel, row, 0, labelAlignment);

    foreach (const QByteArray &ba, QImageWriter::supportedImageFormats()) {
        _formatComboBox->addItem(QString(ba));
    }

    const int defaultFormatIndex(_formatComboBox->findText(defaultFormat));
    if (-1 != defaultFormatIndex) {
        // Default format is one of the supported formats, use as default.

        _formatComboBox->setCurrentIndex(defaultFormatIndex);
    }

    layout->addWidget(_formatComboBox, row, 1);

    // Quality.

    ++row;

    QLabel *qualityLabel = new QLabel(tr("Quality"));
    layout->addWidget(qualityLabel, row, 0, labelAlignment);

    QSpinBox *qualitySpinBox = new QSpinBox;
    qualitySpinBox->setRange(qualityMin, qualityMax);
    qualitySpinBox->setContextMenuPolicy(Qt::NoContextMenu);

    _qualitySlider->setOrientation(Qt::Horizontal);
    _qualitySlider->setRange(qualityMin, qualityMax);

    // Synchronize slider and spin box.

    connect(qualitySpinBox, SIGNAL(valueChanged(int)),
            _qualitySlider, SLOT(setValue(int)));
    connect(_qualitySlider, SIGNAL(valueChanged(int)),
            qualitySpinBox, SLOT(setValue(int)));

    // Sets value of both slider and spin box.

    _qualitySlider->setValue(qualityValue);

    QHBoxLayout *hlayout = new QHBoxLayout;

    hlayout->addWidget(qualitySpinBox, 0);
    hlayout->addWidget(_qualitySlider, 0);

    layout->addLayout(hlayout, row, 1);

    // Buttons

    ++row;

    QDialogButtonBox *dbb(
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel));
    connect(dbb, SIGNAL(accepted()), SLOT(accept()));
    connect(dbb, SIGNAL(rejected()), SLOT(reject()));
    layout->addWidget(dbb, row, 0, 1, 3);

    setLayout(layout);
    resize(480, 0);
}


// path
// ----
//! Returns the chosen playblast path.

QString
NsPlayblastDialog::path() const
{
    return _pathLineEdit->text();
}


// fileName
// --------
//! Returns the chosen file name.

QString
NsPlayblastDialog::fileName() const
{
    return _fileNameLineEdit->text();
}


// firstFrame
// ----------
//! Returns the chosen first frame.

int
NsPlayblastDialog::firstFrame() const
{
    bool ok(false);

    int frame(1);
    const int i(_firstFrameLineEdit->text().toInt(&ok));

    if (ok) {
        frame = i;
    }
    else {
        queryFirstVisibleFrame(&frame); // Ignore success.
    }

    return frame;
}


// lastFrame
// ---------
//! Returns the chosen last frame.

int
NsPlayblastDialog::lastFrame() const
{
    bool ok(false);

    int frame(1);
    const int i(_lastFrameLineEdit->text().toInt(&ok));

    if (ok) {
        frame = i;
    }
    else {
        queryLastVisibleFrame(&frame); // Ignore success.
    }

    return frame;
}


// width
// -----
//! Returns the chosen width.

int
NsPlayblastDialog::width() const
{
    bool ok(false);
    int w(640);

    const int i(_widthLineEdit->text().toInt(&ok));
    if (ok) {
        w = i;
    }

    return w;
}


// height
// ------
//! Returns the chosen height.

int
NsPlayblastDialog::height() const
{
    bool ok(false);
    int h(480);

    const int i(_heightLineEdit->text().toInt(&ok));
    if (ok) {
        h = i;
    }

    return h;
}


// format
// ------
//! Returns the chosen format.

QString
NsPlayblastDialog::format() const
{
    return _formatComboBox->currentText();
}


// quality
// -------
//! Returns the chosen quality.

int
NsPlayblastDialog::quality() const
{
    return _qualitySlider->value();
}

// -----------------------------------------------------------------------------

// _onFileButtonClicked
// --------------------
//! Opens file dialog where playblast path can be chosen. [slot]

void
NsPlayblastDialog::_onFileButtonClicked()
{
    const QString path(
        QFileDialog::getExistingDirectory(
            this,
            tr("Playblast Path")));

    if (!path.isEmpty()) {
        _pathLineEdit->setText(path);
    }
}
