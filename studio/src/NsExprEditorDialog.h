// -----------------------------------------------------------------------------
//
// NsParamBaseWidget.cc
//
// Naiad Studio param base widget, source file.
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

#ifndef NS_EXPR_EDITOR_DIALOG_H
#define NS_EXPR_EDITOR_DIALOG_H

#include "NsValueBaseWidget.h"
#include "NsValueBaseObject.h"
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QKeySequence>
#include <QGridLayout>

// -----------------------------------------------------------------------------

// NsExprEditorDialog
// ------------------
//! DOCS

class NsExprEditorDialog : public QDialog
{
    Q_OBJECT

public:

    NsExprEditorDialog(const int          comp,
                       NsValueBaseWidget &parent)
        : QDialog(&parent)
        , _vbw(&parent)
        , _comp(comp)
        , _te(new QTextEdit(parent.valueBaseObject()->expr(comp), this))
    {
        setWindowTitle(tr("Expression Editor"));
        setSizeGripEnabled(true);
        setModal(false);

        QGridLayout *gridLayout(new QGridLayout);

        QLabel *vbLabel(new QLabel(this));  // Child.

        NsValueBaseObject *vbo(_vbw->valueBaseObject());

        if (0 != vbo) {
            vbLabel->setText(
                tr("Value") + ": " +
                vbo->longName() + "[" + QString::number(_comp) + "]");

            _te->setPlainText(vbo->expr(comp));
        }

        gridLayout->addWidget(vbLabel);
        gridLayout->addWidget(_te);

        QPushButton *acceptButton(new QPushButton(tr("&Done"), this));
        acceptButton->setAutoDefault(true);
        acceptButton->setToolTip(tr("Set expression and close dialog"));
        connect(acceptButton, SIGNAL(clicked()), SLOT(accept()));

        QPushButton *applyButton(new QPushButton(tr("&Apply"), this));
        applyButton->setToolTip(tr("Set expression"));
        connect(applyButton, SIGNAL(clicked()), SLOT(_onApply()));

        QPushButton *cancelButton(new QPushButton(tr("&Cancel"), this));
        cancelButton->setToolTip(tr("Close dialog without setting expression"));
        connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));

        QDialogButtonBox *dbb(new QDialogButtonBox(this));
        dbb->addButton(acceptButton, QDialogButtonBox::AcceptRole);
        dbb->addButton(applyButton, QDialogButtonBox::ApplyRole);
        dbb->addButton(cancelButton, QDialogButtonBox::RejectRole);
        gridLayout->addWidget(dbb);

        setLayout(gridLayout);
    }

    virtual ~NsExprEditorDialog() {}    //!< DTOR.

    virtual void
    accept()
    {
        _onApply();
        QDialog::accept();
    }

public slots:

    //! DOCS [slot]

    void
    onValueChanged(const QString &valueLongName,
                   const QString &expr,
                   const int      comp,
                   const bool     success)
    {
        if (success &&
            _comp == comp &&
            _vbw->valueBaseObject()->longName() == valueLongName) {
            _te->setPlainText(expr);
        }
    }

signals:

    void valueEdited(const QString &newValue, int comp);

private slots:

    //! DOCS [slot]
    void
    _onApply()
    {
        emit valueEdited(_te->toPlainText(), _comp);
    }

private:    // Member variables.

    NsValueBaseWidget *_vbw;    //!< Parent.
    int                _comp;   //!< Component.
    QTextEdit         *_te;
};

#endif // NS_EXPR_EDITOR_DIALOG_H
