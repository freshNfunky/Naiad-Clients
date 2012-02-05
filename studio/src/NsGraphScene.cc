// -----------------------------------------------------------------------------
//
// NsGraphScene.cc
//
// Naiad Studio Op graph, source file.
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

#include "NsGraphScene.h"
#include "NsPreferences.h"
#include "NsCmdCentral.h"
#include "NsUndoStack.h"
#include "NsOpStore.h"
#include "NsQuery.h"
#include "NsStringUtils.h"
#include "NsGraphCallback.h"
#include "NsGraphOpItemFactory.h"
#include "NsGraphItem.h"
#include "NsGraphInputPlugItem.h"
#include "NsGraphOutputPlugItem.h"
#include "NsGraphFeedItem.h"
#include "NsMessageWidget.h"
#include <QDebug>

// -----------------------------------------------------------------------------

// ~NsGraphScene
// -------------
//! DTOR.

NsGraphScene::~NsGraphScene()
{ 
    qDebug() << "~NsGraphScene"; 
}

// -----------------------------------------------------------------------------

// NsGraphScene
// ------------
//! CTOR.

NsGraphScene::NsGraphScene(NsUndoStack *undoStack, QObject *parent)
    : NsGraphicsScene(parent)
    , _undoStack(undoStack)
{
    qDebug() << "NsGraphScene";   

    _createOpItem(NsOpStore::instance()->mutableGlobalOp());



/*

QTextEdit *textItem;
textItem=new QTextEdit;
textItem->setReadOnly(true);
textItem->setFont(QFont("sansserif", 12, QFont::DemiBold));
textItem->setTextColor(Qt::black);
textItem->setTextBackgroundColor(Qt::lightGray);
textItem->setLineWrapMode(QTextEdit::NoWrap);
QGraphicsWidget *textEdit = addWidget(textItem);
textEdit->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum,QSizePolicy::DefaultType);
QGraphicsGridLayout *layout = new QGraphicsGridLayout;
layout->addItem(textEdit,0,0);
layout->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum,QSizePolicy::DefaultType);
QGraphicsWidget *textWidget;
textWidget = new QGraphicsWidget;
textWidget->setLayout(layout);
textWidget->setZValue(10);   
textWidget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum,QSizePolicy::DefaultType);
addItem(textWidget);

*/
}


bool
NsGraphScene::containsOpItem(const NsValueObjectHandle &handle) const
{
    return _opItems.contains(handle);
}


NsGraphFeedItem*
NsGraphScene::queryMutableFeedItem(const QString &inputLongName) const
{
    return _findFeedItem(inputLongName);
}


QList<NsGraphOpItem*>
NsGraphScene::selectedOpItems()
{
    QList<NsGraphOpItem*> selOpItems;
    foreach (const NsOpObject *op, NsOpStore::instance()->constSelectedOps()) {
        selOpItems.append(_findOpItem(op->handle()));
    }
    return selOpItems;
}

// -----------------------------------------------------------------------------

// onFeedChanged
// -------------
//! DOCS. [slot]

void
NsGraphScene::onFeedChanged(const QString &inputLongName,
                            const QString &plugLongName,
                            const bool     success)
{
    if (success) {

        if (plugLongName.isEmpty()) {
            _eraseFeedItem(inputLongName);              // Feed was erased.
        }
        else if (!inputLongName.isEmpty() && !plugLongName.isEmpty()){
            // A new feed will be created. Check if we need to destroy
            // an old feed first.

            _eraseFeedItem(inputLongName);
            _createFeedItem(inputLongName, plugLongName);
        }
    }
}


// onGraphCleared
// --------------
//! Clear child items. [slot]

void
NsGraphScene::onGraphCleared(const bool success)
{
    if (success) {
        qDebug() << "NsGraphScene::onGraphCleared";
        //_clear();
    }
}

// -----------------------------------------------------------------------------

void
NsGraphScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    int cvf = 1;
    queryCurrentVisibleFrame(&cvf);
    if (queryIsFrameLive(cvf)) {
        painter->setBrush(QBrush(
            NsPreferences::instance()->graphViewLiveBackground()));
    }
    else {
        painter->setBrush(QBrush(
            NsPreferences::instance()->graphViewBackground()));
    }

    painter->drawRect(rect);

    //// Parent method, fill rect with backgroundBrushColor()

    //NsGraphicsScene::drawBackground(painter, rect);


/*
    painter->save();

    QTransform xf(painter->transform());
    xf.setMatrix(1, xf.m12(), xf.m13(),
                 xf.m21(), 1, xf.m23(),
                 xf.m31(), xf.m32(), 1);
    painter->setTransform(xf);

    painter->setPen(QPen(QColor("#666666"), 5));
    painter->setFont(QFont("Sans", 32));
    painter->setRenderHint(QPainter::TextAntialiasing);

    painter->drawText(rect, "Test");

    painter->restore();
*/
}

void
NsGraphScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //Q_UNUSED(event);
    event->accept();
    //qDebug() << "NsGraphScene::mousePressEvent";
    //NsGraphicsScene::mousePressEvent(event);
}

void
NsGraphScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    //qDebug() << "NsGraphScene::mouseMoveEvent";
    NsGraphicsScene::mouseMoveEvent(event);
}

void
NsGraphScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    //Q_UNUSED(event);
    event->accept();
    //qDebug() << "NsGraphScene::mouseReleaseEvent";
    //NsGraphicsScene::mouseReleaseEvent(event);
}

void
NsGraphScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    //Q_UNUSED(event);
    event->accept();
    //qDebug() << "NsGraphScene::mouseDoubleClickEvent";
    //NsGraphicsScene::mouseDoubleClickEvent(event);
}

// -----------------------------------------------------------------------------

// _createOpItem
// -------------
//! DOCS

void
NsGraphScene::_createOpItem(NsOpObject *op)
{
    if (!containsOpItem(op->handle())) {
        typedef NsGraphOpItemFactory FactoryType;

        NsGraphOpItem *opItem(
            FactoryType::instance()->create(
                FactoryType::KeyType(op->familyName()),
                FactoryType::ArgsType(op, _undoStack, 0, this, 64.)));

        if (0 != opItem) {
            // A valid item was created by the factory.
            // Connect the Op object to the Op item.

            connect(op,     SIGNAL(nameChanged(QString,QString)),
                    opItem, SLOT(onNameChanged(QString,QString)));

            connect(op,     SIGNAL(conditionChanged(NsOpObject::Condition)),
                    opItem, SLOT(onConditionChanged(NsOpObject::Condition)));

            connect(op,     SIGNAL(enabledChanged(bool)),
                    opItem, SLOT(onEnabledChanged(bool)));

            connect(op,     SIGNAL(selectionChanged()),
                    opItem, SLOT(onSelectionChanged()));

            connect(op,     SIGNAL(positionChanged(qreal,qreal)),
                    opItem, SLOT(onPositionChanged(qreal,qreal)));

            addItem(opItem);    // Pass ownership to graph.
            _opItems.insert(op->handle(), opItem);
        }
        else {
            NsMessageWidget::instance()->clientWarning(
                "Unrecognized Op family: " + op->familyName());
        }
    }
    else {
        NsMessageWidget::instance()->clientWarning(
            "Cannot add duplicate Op item");
    }
}


// _eraseOpItem
// ------------
//! DOCS

void
NsGraphScene::_eraseOpItem(NsOpObject *op)
{
    qDebug() << "NsGraphScene::_eraseOpItem";

    _OpItemHashType::iterator iter(_opItems.find(op->handle()));

    if (_opItems.end() != iter) {
        _eraseItem(iter.value());
        _opItems.erase(iter);       // Remove association.
    }
}


// _findOpItem
// -----------
//! Returns null if no matching Op item found.

NsGraphOpItem*
NsGraphScene::_findOpItem(const NsValueObjectHandle &handle) const
{
    return _opItems.value(handle, 0);
}

// -----------------------------------------------------------------------------

void
NsGraphScene::_createFeedItem(const QString &inputLongName,
                              const QString &plugLongName)
{
    EM_ASSERT(queryValidFeed(inputLongName, plugLongName));

    NsGraphInputPlugItem *input(_findInputPlugItem(inputLongName));
    NsGraphPlugItem      *plug(_findPlugItem(plugLongName));

    if (0 != input && 0 != plug) {
        // Both input and plug exist.

        NsGraphFeedItem *feedItem(new NsGraphFeedItem(*input, *plug));

        // Body cache updates.

        connect(plug->plugObject(),
                SIGNAL(liveBodyCacheChanged()),
                feedItem,
                SLOT(onLiveBodyCacheChanged()));

        connect(plug->plugObject()->constOp(),
                SIGNAL(empBodyCacheChanged()),
                feedItem,
                SLOT(onEmpBodyCacheChanged()));

        connect(plug->plugObject()->constOp(),
                SIGNAL(stateChanged()),
                feedItem,
                SLOT(onStateChanged()));

        connect(NsCmdCentral::instance(),
                SIGNAL(currentVisibleFrameChanged(int,bool,bool)),
                feedItem,
                SLOT(onCurrentVisibleFrameChanged(int,bool,bool)));

        // Selection.

        connect(input->plugObject(), SIGNAL(selectionChanged()),
                feedItem,            SLOT(onSelectionChanged()));

        // Path.

        connect(input->opItem(), SIGNAL(xChanged()), 
                feedItem,        SLOT(onPathChanged()));
        connect(input->opItem(), SIGNAL(yChanged()), 
                feedItem,        SLOT(onPathChanged()));
        connect(plug->opItem(),  SIGNAL(xChanged()), 
                feedItem,        SLOT(onPathChanged()));
        connect(plug->opItem(),  SIGNAL(yChanged()), 
                feedItem,        SLOT(onPathChanged()));

        addItem(feedItem);    // Pass ownership to graph.
    }
}

void
NsGraphScene::_eraseFeedItem(const QString &inputLongName)
{
    NsGraphFeedItem *gfi(_findFeedItem(inputLongName));

    if (0 != gfi) {
        _eraseItem(gfi);
    }
}


// _findInputPlugItem
// ------------------
//! Search for the given input plug in Op items. Returns null if the plug
//! cannot be found.

NsGraphInputPlugItem*
NsGraphScene::_findInputPlugItem(const QString &inputLongName) const
{
    const NtString iln(fromQStr(inputLongName));

    //const QString opInstance(fromNbStr(iln.parent(":")));
    //const QString plugName(fromNbStr(iln.child(":")));

    const NsOpObject *op(
        NsOpStore::instance()->queryConstOp(
            fromNbStr(iln.parent(":"))));

    if (0 != op) {
        // Op exists.

        NsGraphOpItem *goi(_findOpItem(op->handle()));

        if (0 != goi) {
            // Op item exists.

            return goi->inputPlugItem(inputLongName);
        }
    }

    //foreach (QGraphicsItem *item, items()) {
    //    NsGraphOpItem *opItem(dynamic_cast<NsGraphOpItem*>(item));

    //    if (0 != opItem) {
    //        // Search for input plugs in Op items.

    //        NsGraphInputPlugItem *plug(opItem->inputPlugItem(inputLongName));

    //        if (0 != plug) {
    //            return plug;    // Plug found, we are done!
    //        }
    //    }
    //}

    return 0;   // No matching plug found, return null.
}


// _findPlugItem
// -------------
//! Search for the given plug in Op items. Returns null if the plug
//! cannot be found.

NsGraphPlugItem*
NsGraphScene::_findPlugItem(const QString &plugLongName) const
{
    const NtString iln(fromQStr(plugLongName));

    //const QString opInstance(fromNbStr(iln.parent(":")));
    //const QString plugName(fromNbStr(iln.child(":")));

    const NsOpObject *op(
        NsOpStore::instance()->queryConstOp(
            fromNbStr(iln.parent(":"))));

    if (0 != op) {
        // Op exists.

        NsGraphOpItem *goi(_findOpItem(op->handle()));

        if (0 != goi) {
            // Op item exists.

            return goi->plugItem(plugLongName);
        }
    }

    //foreach (QGraphicsItem *item, items()) {
    //    NsGraphOpItem *opItem(dynamic_cast<NsGraphOpItem*>(item));

    //    if (0 != opItem) {
    //        // Search for output plugs in Op items.

    //        NsGraphPlugItem *plug(opItem->plugItem(plugLongName));

    //        if (0 != plug) {
    //            return plug;    // Plug found, we are done!
    //        }
    //    }
    //}

    return 0;   // No matching plug found, return null.
}


// _findFeedItem
// -------------
//! DOCS

NsGraphFeedItem*
NsGraphScene::_findFeedItem(const QString &inputLongName) const
{
    foreach (QGraphicsItem *item, items()) {
        NsGraphFeedItem *feedItem(dynamic_cast<NsGraphFeedItem*>(item));

        if (0 != feedItem && feedItem->longName() == inputLongName) {
            return feedItem;
        }
    }

    return 0;   // Not found.
}

// -----------------------------------------------------------------------------

// _eraseItem
// ----------
//! DOCS

void
NsGraphScene::_eraseItem(NsGraphItem *item)
{
    removeItem(item);       // Remove from scene.
    emit itemRemoved(item); // Notify.
    delete item;            // Make sure memory is deallocated.
}


// _topLevelItems
// --------------
//! Returns a list of the top-level items in the scene.

QList<QGraphicsItem*>
NsGraphScene::_topLevelItems() const
{
    QList<QGraphicsItem*> tli;
    foreach (QGraphicsItem *item, items()) {
        if (_isTopLevel(item)) {
            tli.append(item);
        }
    }
    
    return tli;
}

// -----------------------------------------------------------------------------

// _isTopLevel
// -----------
//! Returns true if the provided item is top-level. [static]

bool
NsGraphScene::_isTopLevel(const QGraphicsItem *item)
{
    return (item->topLevelItem() == item);
}
