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

#include "NsParamBaseWidget.h"
#include "NsParamWidgetArgs.h"
#include "NsValueBaseObject.h"
#include "NsValueObject.h"
#include "NsCmdCentral.h"
#include "NsCmdSetParam.h"
#include "NsCmdSetMeta.h"
#include "NsOpStore.h"
#include "NsBodyObject.h"
#include "NsBodyOutputPlugObject.h"
#include "NsValueLineEdit.h"
#include "NsTimeToolBar.h"
#include "NsQuery.h"
#include "NsStringUtils.h"
#include "NsExprEditorDialog.h"
#include <NgNelContext.h>
#include <QtGui>

// -----------------------------------------------------------------------------

// ~NsParamBaseWidget
// ------------------
//! DTOR.

NsParamBaseWidget::~NsParamBaseWidget()
{}


void
NsParamBaseWidget::setReadOnly(const bool readOnly)
{
    setEnabled(readOnly);
}

// -----------------------------------------------------------------------------

// onValueChanged
// --------------
//! DOCS [slot]

void
NsParamBaseWidget::onValueChanged(const QString &longName,
                                  const QString &expr,
                                  const int      comp,
                                  const bool     success)
{
    emit valueChanged();
}


// onMetaChanged
// -------------
//! DOCS [slot]

void
NsParamBaseWidget::onMetaChanged(const QString &longName,
                                 const QString &valueType,
                                 const QString &value,
                                 const bool     success)
{
    emit valueChanged();
}


void
NsParamBaseWidget::onCurrentVisibleFrameChanged(const int  cvf,
                                                const bool update3DView,
                                                const bool success)
{
    Q_UNUSED(update3DView);

    emit valueChanged();
}


// onModeActionTriggered
// ---------------------
//! DOCS [slot]
void
NsParamBaseWidget::onModeActionTriggered(QAction *action)
{
    NsCmdSetMeta::exec(valueBaseObject()->longName(),
                       NsValueLineEdit::modeValueType,
                       action->objectName());
}

// -----------------------------------------------------------------------------

// NsParamBaseWidget
// -----------------
//! CTOR.

NsParamBaseWidget::NsParamBaseWidget(const NsParamWidgetArgs &args)
    : NsValueBaseWidget(args)
    , _op(args.op())
    , _undoStack(args.undoStack())
    //, _exprEditor(0)    // Null.
{}


QSet<QString>
NsParamBaseWidget::upstreamBodyNames(const bool cached, const bool live) const
{
    QSet<QString> bodyNameSet;

    const NtTimeBundle cvftb(queryCurrentVisibleFrameTimeBundle());
    const QStringList upstreamOpNames(
        queryUpstreamOpNames(op()->longName(), true));

    foreach (const QString &opInstance, upstreamOpNames) {
        const NsOpObject *usOp(NsOpStore::instance()->queryConstOp(opInstance));

        if (0 != usOp) {
            if (cached) {
                // Check upstream Ops for cached bodies.

                const QStringList cachedBodyNames(
                    queryCachedBodyNames(usOp->longName(), cvftb));

                foreach (const QString &bodyName, cachedBodyNames) {
                    bodyNameSet.insert(bodyName);
                }
            }

            if (live) {
                // Check upstream Ops for live bodies.

                foreach (const NsBodyOutputPlugObject *bopo,
                         usOp->constBodyOutputs()) {
                    // Check body output plugs.

                    foreach (const NsBodyObject *body, 
                             bopo->constLiveBodies()) {
                        bodyNameSet.insert(body->name());
                    }
                }
            }
        }
    }

    return bodyNameSet;
}


// addContextActions
// -----------------
//! DOCS

void
NsParamBaseWidget::addContextActions(QMenu &menu, const int comp) const
{
    menu.addSeparator();
    NsExprEditorAction *action(
        new NsExprEditorAction("&Open Expression Editor", comp, &menu));
    connect(action, SIGNAL(triggered(int)),
            this,   SLOT(onExprEditorActionTriggered(int)));
    menu.addAction(action);
    menu.addSeparator();
}


// stringEval
// ----------
//! DOCS

QString
NsParamBaseWidget::stringEval(const NtTimeBundle &tb,
                              const int           comp,
                              const int           threadId) const
{
    QString eval("");    

    try {
        Ng::NelContext nelContext(&op()->mutableNgOp(),0,"",tb);
        eval = 
            fromNbStr(
                valueBaseObject()->nbValueBase().stringEval(tb,comp,threadId));
        return eval;
    }
    catch (std::exception &ex) {
        qDebug() << ex.what();   // TODO: error message!
        return eval;
    }
}

// -----------------------------------------------------------------------------

// onValueEdited
// -------------
//! DOCS [slot]

void
NsParamBaseWidget::onValueEdited(const QString &newValue,
                                 const int      comp)
{
    if (newValue != valueBaseObject()->expr(comp)) {
        if (0 != undoStack()) {
            NsCmdSetParam::exec(
                NsCmdSetParam::ArgsList() <<
                    NsCmdSetParam::Args(
                        valueBaseObject()->longName(),
                        newValue,
                        comp),
                *undoStack());
        }
        else {
            NsCmdSetParam::exec(
                NsCmdSetParam::ArgsList() <<
                    NsCmdSetParam::Args(
                        valueBaseObject()->longName(),
                        newValue,
                        comp));
        }
    }
}


// onExprEditorActionTriggered
// ---------------------------
//! DOCS [slot]

void
NsParamBaseWidget::onExprEditorActionTriggered(const int comp)
{
    //if (0 == _exprEditor) {
    NsExprEditorDialog *exprEditor(new NsExprEditorDialog(comp, *this));// Child
    connect(exprEditor, SIGNAL(valueEdited(QString,int)),
            this,       SLOT(onValueEdited(QString,int)));
    connect(NsCmdCentral::instance(),
            SIGNAL(valueChanged(QString,QString,int,bool)),
            exprEditor,
            SLOT(onValueChanged(QString,QString,int,bool)));
    //}

    exprEditor->show();
    exprEditor->raise();
    exprEditor->activateWindow();
}
