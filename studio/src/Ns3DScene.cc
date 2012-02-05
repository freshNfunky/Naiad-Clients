// -----------------------------------------------------------------------------
//
// Ns3DScene.cc
//
// Naiad Studio 3D graph source file.
//
// Copyright (c) 2011 Exotic Matter AB.  All rights reserved. 
//
// This file is part of Open Naiad Studio..
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

#include "Ns3DScene.h"
#include "Ns3DGraphicsItem.h"
#include "Ns3DManipulator.h"
#include "Ns3DOpAxisItem.h"
#include "Ns3DOpVectorItem.h"
#include "Ns3DOpBoxItem.h"
#include "Ns3DOpClipBoxItem.h"
#include "Ns3DOpPlaneItem.h"
#include "Ns3DOpFrustumItem.h"
#include "Ns3DOpImportTransformItem.h"
#include "Ns3DOpSphereItem.h"
#include "Ns3DConstructionGridItem.h"
#include "NsOpStore.h"
#include "NsOpObject.h"
#include "NsCmdCentral.h"
#include "NsCmdSelectOp.h"
#include <QDebug>

// -----------------------------------------------------------------------------

// Ns3DScene
// ---------
//! Create instance and connect to naiad.

Ns3DScene::Ns3DScene(NsUndoStack *undoStack, QObject *parent)
    : QObject(parent)
    , _manip(0)
    , _cgItem(new Ns3DConstructionGridItem)
    , _undoStack(undoStack)
{
    qDebug() << "Ns3DScene";
}


// ~Ns3DScene
// ----------
//! Destroy instance and free item memory.

Ns3DScene::~Ns3DScene()
{
    qDebug() << "~Ns3DScene";
    _clear();
    delete _cgItem;
}

// -----------------------------------------------------------------------------

QList<Ns3DGraphicsItem*>
Ns3DScene::selectedItems(const bool selected) const
{
    QList<Ns3DGraphicsItem*> selItems;
    foreach (Ns3DGraphicsItem *item, items()) {
        if (item->selected() == selected) {
            selItems.append(item);
        }
    }
    return selItems;
}


bool
Ns3DScene::itemsWorldBoundingBox(Ns3DBBox &bbox) const
{
    int numValid = 0;
    foreach (Ns3DGraphicsItem *item, items()) {
        Ns3DOpItem *opItem = dynamic_cast<Ns3DOpItem*>(item);
        if (0 != opItem && opItem->isVisible()) {
            // Only use visible items.

            Ns3DOpFrustumItem *frustumItem = 
                dynamic_cast<Ns3DOpFrustumItem*>(item);
            if (0 == frustumItem) {
                // Don't include camera frustums, they are very large...

                if (0 == numValid) {
                    bbox = opItem->worldBoundingBox(); // First valid item.
                }
                else {
                    bbox.unite(opItem->worldBoundingBox());
                }
                ++numValid;
            }
        }
    }
    return (0 < numValid);
}


bool
Ns3DScene::selectedItemsWorldBoundingBox(Ns3DBBox &bbox, 
                                         const bool selected) const
{
    int numValid = 0;
    foreach (Ns3DGraphicsItem *item, items()) {
        Ns3DOpItem *opItem = dynamic_cast<Ns3DOpItem*>(item);
        if (0 != opItem && opItem->isVisible() && 
            opItem->selected() == selected) {
            Ns3DOpFrustumItem *frustumItem = 
                dynamic_cast<Ns3DOpFrustumItem*>(item);
            if (0 == frustumItem) {
                // Don't include camera frustums, they are very large...

                if (0 == numValid) {
                    bbox = opItem->worldBoundingBox(); // First valid item.
                }
                else {
                    bbox.unite(opItem->worldBoundingBox());
                }
                ++numValid;
            }
        }
    }
    return (0 < numValid);
}


// findItem
// --------
//! Find an item given an Id. Returns null if no match for Id.

Ns3DGraphicsItem*
Ns3DScene::findItem(const int32_t id)
{
    return _items.value(id, 0);
}


// selectItem
// ----------
//! Select a (selectable) item by Id.

void
Ns3DScene::selectItem(const int32_t id)
{
    Ns3DGraphicsItem* item = findItem(id);

    if (0 != item && item->selectable()) {
        Ns3DOpItem* opItem(dynamic_cast<Ns3DOpItem*>(item));

        if (0 != opItem) {
            NsCmdSelectOp::exec(
                NsCmdSelectOp::ArgsList() <<
                    NsCmdSelectOp::Args(opItem->opObject()->longName(), true));
        }
        else {
            _selectItem(item);
        }
    }
}


// setManip
// --------
//! Set the current manipulator. Pass '0' to disable manipulation.
//! Attaches selected item to the provided manipulator.

void
Ns3DScene::setManip(Ns3DManipulator *manip)
{
    delete _manip;
    _manip = manip;

    // Attach selected items to manipulator

    if (hasManip()) {
        _manip->clearItems();

        foreach (Ns3DGraphicsItem* item, _items) {
            if (item->selected() && item->isVisible()) {
                _manip->attachItem(item);
            }
        }
    }
}

// -----------------------------------------------------------------------------

void
Ns3DScene::onOpObjectCreated(NsOpObject *op)
{
    if (op->hasSection("View Frustum")) {
        _createOpItem(op, Ns3DGraphicsItem::FrustumFlag);
    }
    else if(op->hasSection("Transform") ||
            op->hasSection("Initial Transform")) {
        _createOpItem(op, Ns3DGraphicsItem::TransformFlag);
        //_createOpItem(op, Ns3DGraphicsItem::ImportTransformFlag); // TMP!!!
    }
    else if(op->hasSection("Clip Box")) {
        _createOpItem(op, Ns3DGraphicsItem::ClipBoxFlag);
    }
    else if(op->hasSection("Plane")) {
        _createOpItem(op, Ns3DGraphicsItem::PlaneFlag);
    }
    else if(op->hasSection("Sphere")) {
        _createOpItem(op, Ns3DGraphicsItem::SphereFlag);
    }
    else if(op->hasSection("Axis")) {
        _createOpItem(op, Ns3DGraphicsItem::AxisFlag);
    }
    else if(op->hasSection("Vector")) {
        _createOpItem(op, Ns3DGraphicsItem::VectorFlag);
    }
    else if(op->hasSection("Import Transform")) {
        _createOpItem(op, Ns3DGraphicsItem::ImportTransformFlag);
    }
}


void
Ns3DScene::onGraphCleared(const bool success)
{
    if (success) {
        qDebug() << "Ns3DScene::onGraphCleared";
        _clear();
    }
}


void
Ns3DScene::onOpSelectionChanged(const QStringList &opInstances,
                                const bool         success)
{
    if (success) {
        foreach (const QString &opInstance, opInstances) {
            const NsOpObject *op = 
                NsOpStore::instance()->queryConstOp(opInstance);

            if (0 != op) {
                Ns3DGraphicsItem *item(_findItem(op->handle()));

                if (0 != item) {
                    if (op->isSelected()) {
                        _selectItem(item);
                    }
                    else {
                        _unselectItem(item);
                    }
                }
            }
        }
    }
}


void
Ns3DScene::onMetaChanged(const QString &longName,
                         const QString &valueType,
                         const QString &value,
                         const bool     success)
{
    if (success) {
        const NsOpObject *opObject(
            NsOpStore::instance()->queryConstOp(longName));

        if (0 != opObject) {
            Ns3DGraphicsItem *item(_findItem(opObject->handle()));

            if (0 != item) {
                if ("Visible in 3D" == valueType) {
                    if ("On" == value) {
                        _showItem(item);
                    }
                    else {
                        _hideItem(item);
                    }
                }
            }
        }
    }
}

// -----------------------------------------------------------------------------

void
Ns3DScene::_createOpItem(NsOpObject *op, const int sectionFlags)
{
    if (!_ids.contains(op->handle())) {
        int32_t id = _items.size() + 1;  // Non-zero id;

        if (!_usedIds.empty()) {
            id = _usedIds.pop();      // Reuse Id's from removed items, if any.
        }

        if (!_items.contains(id)) {

            Ns3DGraphicsItem *item(0);

            if (sectionFlags & Ns3DGraphicsItem::FrustumFlag) {
                item = new Ns3DOpFrustumItem(op, id);
            }

            if (sectionFlags & Ns3DGraphicsItem::TransformFlag) {
                item = new Ns3DOpBoxItem(op, id);
            }

            if (sectionFlags & Ns3DGraphicsItem::ClipBoxFlag) {
                item = new Ns3DOpClipBoxItem(op, id);
            }

            if (sectionFlags & Ns3DGraphicsItem::PlaneFlag) {
                item = new Ns3DOpPlaneItem(op, id);
            }

            if (sectionFlags & Ns3DGraphicsItem::SphereFlag) {
                item = new Ns3DOpSphereItem(op, id);
            }

            if (sectionFlags & Ns3DGraphicsItem::AxisFlag) {
                item = new Ns3DOpAxisItem(op, id);
            }

            if (sectionFlags & Ns3DGraphicsItem::VectorFlag) {
                item = new Ns3DOpVectorItem(op, id);
            }

            if (sectionFlags & Ns3DGraphicsItem::ImportTransformFlag) {
                item = new Ns3DOpImportTransformItem(op, id);
            }

            if (item) {
                // Update associations.

                _ids.insert(op->handle(), id);
                _items.insert(id, item);

                // Make sure item is erased when Op is destroyed.

                /*
                connect(op,
                        SIGNAL(opObjectDestroyed(NsOpObject*)),
                        SLOT(onOpObjectDestroyed(NsOpObject*)));
                        */
            }
        }
    }
}


void
Ns3DScene::_eraseOpItem(NsOpObject *op)
{
    // At this point all resource pointers to server objects are invalid.
    // Don't use them!

    const _IdHashType::iterator i0(_ids.find(op->handle()));

    if (_ids.end() != i0) {
        // Handle exists.

        const _ItemHashType::iterator i1(_items.find(i0.value()));

        if (_items.end() != i1) {
            // Item exists.

            if (hasManip()) {
                _manip->removeItem(i1.value());
            }

            delete i1.value();
            _usedIds.push(i1.key());
            _items.erase(i1); // Remove handle association.
            _ids.erase(i0);
        }
    }
}


void
Ns3DScene::_clear()
{
    qDebug() << "Ns3DScene::_clear";

    setManip(0);

    foreach (Ns3DGraphicsItem *item, _items) {
        delete item;
    }

    _usedIds.clear();
    _items.clear();
    _ids.clear();
}



Ns3DGraphicsItem*
Ns3DScene::_findItem(const NsValueObjectHandle &handle) const
{
    const _IdHashType::const_iterator iter(_ids.find(handle));

    if (_ids.end() != iter) {
        return _findItem(iter.value());
    }

    return 0;   // Null.
}


Ns3DGraphicsItem*
Ns3DScene::_findItem(const int32_t id) const
{
    return _items.value(id, 0);
}

// -----------------------------------------------------------------------------

void
Ns3DScene::_showItem(Ns3DGraphicsItem* item)
{
    if (hasManip() && item->selected()) {
        _manip->attachItem(item);
    }
}


void
Ns3DScene::_hideItem(Ns3DGraphicsItem* item)
{
    if (hasManip()) {
        _manip->removeItem(item);
    }
}


// _selectItem
// -----------
//! Select a (selectable) item

void
Ns3DScene::_selectItem(Ns3DGraphicsItem* item)
{
    if (item->selectable()) {
        item->setSelected(true);       
        if (hasManip() && item->isVisible()) {
            _manip->attachItem(item);
        }
    }
}


// unselectItem
// ------------
//! Unselect a (selectable) item.

void
Ns3DScene::_unselectItem(Ns3DGraphicsItem* item)
{
    if (item->selectable()) {
        item->setSelected(false);
        if (hasManip()) {
            _manip->removeItem(item);
        }
    }
}

// -----------------------------------------------------------------------------
