// -----------------------------------------------------------------------------
//
// NsProcessDialog.cc
//
// Naiad Studio process dialog, source file.
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

#include "NsProcessDialog.h"
#include "NsLineEdit.h"
#include "NsIntValidator.h"
#include <QPlainTextEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>
#include <QGridLayout>
#include <QDialogButtonBox>

// -----------------------------------------------------------------------------

NsProcessDialog::NsProcessDialog(const QString &program,
                                 const QString &fileNames,
                                 const int      firstFrame,
                                 const int      lastFrame,
                                 const int      threads,
                                 QWidget       *parent)
    : QDialog(parent)
    , _programLineEdit(new NsLineEdit)
    , _fileNamesLineEdit(new NsLineEdit)
    , _firstFrameLineEdit(new NsLineEdit)
    , _lastFrameLineEdit(new NsLineEdit)
    , _verboseCheckBox(new QCheckBox)
    , _threadsSpinBox(new QSpinBox)
    , _optionalArgsTextEdit(new QPlainTextEdit)
    , _restartFrameLineEdit(new NsLineEdit)
    , _restartEmpSequenceLineEdit(new NsLineEdit)
{
    setWindowTitle(program + " options");

    // Create layout.

    QGridLayout *layout = new QGridLayout;
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 1);
    layout->setColumnStretch(2, 0);
    //for (int i = 0; i < 7; ++i) {
    //    layout->setRowStretch(i, 0);
    //}
    //layout->setRowStretch(5, 1);

    const Qt::Alignment labelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // Program.

    int row = 0;

    QLabel *programLabel(new QLabel(tr("Program")));
    _programLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    _programLineEdit->setText(program);

    layout->addWidget(programLabel, row, 0, labelAlignment);
    layout->addWidget(_programLineEdit, row, 1, 1, 2);

    // File names.

    row = 1;

    QLabel *fileNamesLabel(new QLabel(tr("File names")));
    _fileNamesLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
#ifdef WIN32
    _fileNamesLineEdit->setText("\"" + fileNames + "\"");
#else
    _fileNamesLineEdit->setText(fileNames);
#endif // WIN32

    layout->addWidget(fileNamesLabel, row, 0, labelAlignment);
    layout->addWidget(_fileNamesLineEdit, row, 1, 1, 2);

    // Frames.

    row = 2;

    QLabel *framesLabel(new QLabel(tr("Frames")));
    _firstFrameLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    _firstFrameLineEdit->setValidator(new NsIntValidator(_firstFrameLineEdit));
    _firstFrameLineEdit->setText(QString::number(firstFrame));
    
    _lastFrameLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    _lastFrameLineEdit->setValidator(new NsIntValidator(_lastFrameLineEdit));
    _lastFrameLineEdit->setText(QString::number(lastFrame));

    layout->addWidget(framesLabel, row, 0, labelAlignment);
    layout->addWidget(_firstFrameLineEdit, row, 1);
    layout->addWidget(_lastFrameLineEdit, row, 2);
        
    // Verbose.
    
    row = 3;

    QLabel *verboseLabel(new QLabel(tr("Verbose")));
    _verboseCheckBox->setContextMenuPolicy(Qt::NoContextMenu);
    _verboseCheckBox->setCheckable(true);
    _verboseCheckBox->setChecked(false);

    layout->addWidget(verboseLabel, row, 0, labelAlignment);
    layout->addWidget(_verboseCheckBox, row, 1, 1, 2);

    // Threads.

    row = 4;

    QLabel *threadsLabel(new QLabel(tr("Threads")));
    _threadsSpinBox->setContextMenuPolicy(Qt::NoContextMenu);
    _threadsSpinBox->setMinimum(1);
    _threadsSpinBox->setValue(threads);

    layout->addWidget(threadsLabel, row, 0, labelAlignment);
    layout->addWidget(_threadsSpinBox, row, 1, 1, 2);

    // Optional args.

    row = 5;
    QLabel *optionalArgsLabel(new QLabel(tr("Optional arguments")));
    layout->addWidget(optionalArgsLabel, row, 0, labelAlignment);

    _optionalArgsTextEdit->setContextMenuPolicy(Qt::NoContextMenu);
    _optionalArgsTextEdit->setPlainText("");

    layout->addWidget(_optionalArgsTextEdit, row, 1, 1, 2);
    
    // Restart sequence.

    row = 6;

    QLabel *restartFrameLabel(new QLabel(tr("Restart Frame")));
    _restartFrameLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    _restartFrameLineEdit->setText("");

    layout->addWidget(restartFrameLabel, row, 0, labelAlignment);
    layout->addWidget(_restartFrameLineEdit, row, 1, 1, 2);

    row = 7;

    QLabel *restartEmpSequenceLabel(new QLabel(tr("Restart EMP sequence")));
    _restartEmpSequenceLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    _restartEmpSequenceLineEdit->setText("");

    layout->addWidget(restartEmpSequenceLabel, row, 0, labelAlignment);
    layout->addWidget(_restartEmpSequenceLineEdit, row, 1, 1, 2);

    // Buttons

    row = 8;

    QDialogButtonBox *dbb(
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel));
    connect(dbb, SIGNAL(accepted()), SLOT(accept()));
    connect(dbb, SIGNAL(rejected()), SLOT(reject()));
    layout->addWidget(dbb, row, 0, 1, 3);

    setLayout(layout);
    resize(480, 0);
}

// -----------------------------------------------------------------------------

QString
NsProcessDialog::program() const
{ return _programLineEdit->text(); }

QString 
NsProcessDialog::fileNames() const
{ return _fileNamesLineEdit->text(); }

QString
NsProcessDialog::firstFrame() const
{ return _firstFrameLineEdit->text(); }

QString
NsProcessDialog::lastFrame() const
{ return _lastFrameLineEdit->text(); }

bool
NsProcessDialog::verbose() const
{ return _verboseCheckBox->isChecked(); }
    
QString
NsProcessDialog::threads() const
{ return QString::number(_threadsSpinBox->value()); }

QString
NsProcessDialog::optionalArgs() const
{ return _optionalArgsTextEdit->toPlainText(); }

QString
NsProcessDialog::restartFrame() const
{ return _restartFrameLineEdit->text(); }

QString
NsProcessDialog::restartEmpSequence() const
{ return _restartEmpSequenceLineEdit->text(); }
