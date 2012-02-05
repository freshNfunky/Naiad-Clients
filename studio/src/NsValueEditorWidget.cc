// -----------------------------------------------------------------------------
//
// NsValueEditorWidget.cc
//
// Naiad Studio value editor widget, source file.
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

#include "NsValueEditorWidget.h"
#include "NsOpStore.h"
#include "NsOpObjectBox.h"
#include "NsBodyObjectBox.h"
#include "NsQuery.h"
#include "NsValueObject.h"
#include "NsOpObject.h"
#include "NsBodyObject.h"
#include <QString>
#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include <QVBoxLayout>
#include <QDebug>

// -----------------------------------------------------------------------------

// NsValueEditorWidget
// -------------------
//! CTOR.

NsValueEditorWidget::NsValueEditorWidget(NsUndoStack *undoStack,
                                         QWidget     *parent)
    : QWidget(parent),
      _undoStack(undoStack),
      _layout(0)
{
    _layout = new QVBoxLayout;
    _layout->addStretch();
    setLayout(_layout);     // Pass ownership to this widget.
}

// -----------------------------------------------------------------------------

// paintEvent
// ----------
//! Must be overriden for stylesheets to work.

void
NsValueEditorWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

// -----------------------------------------------------------------------------

// onOpSelectionChanged
// --------------------
//! DOCS. [slot]

void
NsValueEditorWidget::onOpSelectionChanged(const QStringList &opInstances,
                                          const bool         success)
{
    if (success && !opInstances.isEmpty()) {
        // Check if any Ops where selected.

        foreach (const QString &opInstance, opInstances) {
            NsOpObject *op = 
                NsOpStore::instance()->queryMutableOp(opInstance);

            if (0 != op) {
                if (op->isSelected()) {
                    // Found a newly selected Op. Show it!

                    _clear();
                    _insertValueObjectBox(op);
                    break;  // Done!
                }
                else {
                    // Try removing Op in case the Op 
                    // being shown was unselected.

                    _removeValueObjectBox(op);
                }
            }
        }

        if (_valueObjectBoxes.isEmpty()) {
            // Any other selected Ops?

            const QList<NsOpObject*> selOps = 
                NsOpStore::instance()->mutableSelectedOps();
            if (!selOps.isEmpty()) {
                _insertValueObjectBox(selOps.first());
            }
        }

        if (_valueObjectBoxes.isEmpty()) {
            // Still empty, any other selected bodies?

            const QList<NsBodyObject*> selBodies = 
                NsOpStore::instance()->mutableSelectedBodies();
            if (!selBodies.isEmpty()) {
                _insertValueObjectBox(selBodies.first());
            }
        }
    }
}


// onBodySelectionChanged
// ----------------------
//! DOCS. [slot]

void
NsValueEditorWidget::onBodySelectionChanged(const QStringList &bodyLongNames,
                                            const bool         success)
{
    if (success && !bodyLongNames.isEmpty()) {
        // Check if any Bodies where selected.

        foreach (const QString &bodyLongName, bodyLongNames) {
            NsBodyObject *body = 
                NsOpStore::instance()->queryMutableBody(bodyLongName);

            if (0 != body) {
                if (body->isSelected()) {
                    // Found a newly selected body. Show it!

                    _clear();
                    _insertValueObjectBox(body);
                    break;  // Done!
                }
                else {
                    // Try removing Op in case the Op 
                    // being shown was unselected.

                    _removeValueObjectBox(body);
                }
            }
        }

        if (_valueObjectBoxes.isEmpty()) {
            // Any other selected bodies?

            const QList<NsBodyObject*> selBodies = 
                NsOpStore::instance()->mutableSelectedBodies();
            if (!selBodies.isEmpty()) {
                _insertValueObjectBox(selBodies.first());
            }
        }

        if (_valueObjectBoxes.isEmpty()) {
            // Still empty, any other selected Ops?

            const QList<NsOpObject*> selOps = 
                NsOpStore::instance()->mutableSelectedOps();
            if (!selOps.isEmpty()) {
                _insertValueObjectBox(selOps.first());
            }
        }
    }
}


// onGraphCleared
// --------------
//! [slot]

void
NsValueEditorWidget::onGraphCleared(const bool success)
{
    qDebug() << "NsValueEditorWidget::onGraphCleared";
    if (success) {
        _clear();
    }
}


// onValueObjectDestroyed
// ----------------------
//! [slot]

void
NsValueEditorWidget::onValueObjectDestroyed(NsValueObject *vo)
{
    _removeValueObjectBox(vo);
}

// -----------------------------------------------------------------------------

template<>
NsValueObjectBox*
NsValueEditorWidget::_createValueObjectBox<NsOpObject>(NsOpObject *op)
{ return new NsOpObjectBox(op, this); } // Child.

template<>
NsValueObjectBox*
NsValueEditorWidget::_createValueObjectBox<NsBodyObject>(NsBodyObject *body)
{ return new NsBodyObjectBox(body, this); } // Child. 

// -----------------------------------------------------------------------------

// _removeValueObjectBox
// ---------------------
//! Remove all value object boxes whose name matches the provided long name,
//! there should be max one.

void
NsValueEditorWidget::_removeValueObjectBox(NsValueObject *vo)
{
    _ValueObjectBoxHashType::iterator iter = 
        _valueObjectBoxes.find(vo->handle());

    if (_valueObjectBoxes.end() != iter) {
        delete iter.value();            // Remove box.
        _valueObjectBoxes.erase(iter);  // Remove association.
    }
}


// _clear
// ------
//! DOCS

void
NsValueEditorWidget::_clear()
{
    foreach (NsValueObjectBox *vob, _valueObjectBoxes) {
        delete vob;             // Remove boxes.
    }
    _valueObjectBoxes.clear();  // Remove associations.
}
