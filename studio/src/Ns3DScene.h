// -----------------------------------------------------------------------------
//
// Ns3DScene.h
//
// Naiad Studio 3D graph header file.
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

#ifndef NS3D_SCENE_H
#define NS3D_SCENE_H

#include "NsValueObjectHandle.h"
#include "Ns3DBBox.h"
#include <QObject>
#include <QHash>
#include <QStack>
#include <stdint.h> // Note: <cstdint> requires C++0x support on g++.

class Ns3DGraphicsItem;
class Ns3DConstructionGridItem;
class Ns3DManipulator;
class NsOpObject;
class NsUndoStack;

// -----------------------------------------------------------------------------

class Ns3DScene : public QObject
{
    Q_OBJECT

public:

    explicit
    Ns3DScene(NsUndoStack *undoStack, QObject *parent = 0);

    virtual
    ~Ns3DScene();

public:

    QList<Ns3DGraphicsItem*>
    items() const
    { return _items.values(); }

    QList<Ns3DGraphicsItem*>
    selectedItems(bool selected = true) const;

    bool
    itemsWorldBoundingBox(Ns3DBBox &bbox) const;

    bool
    selectedItemsWorldBoundingBox(Ns3DBBox &bbox, bool selected = true) const;

    Ns3DGraphicsItem*
    findItem(int32_t id);

    void
    selectItem(int32_t id);

public:

    Ns3DConstructionGridItem*
    constructionGridItem() const
    { return _cgItem; }

public:

    Ns3DManipulator*
    manip()
    { return _manip; }

    const Ns3DManipulator*
    manip() const
    { return _manip; }

    bool
    hasManip() const
    { return (0 != _manip); }

    void
    setManip(Ns3DManipulator *manip);

    NsUndoStack*
    undoStack() const
    { return _undoStack; }

protected slots:

    void
    onOpObjectCreated(NsOpObject *op);

    void
    onOpObjectDestroyed(NsOpObject *op)
    { _eraseOpItem(op); }

    void
    onGraphCleared(bool success);

    void
    onOpSelectionChanged(const QStringList &opInstances,
                         bool               success);

    void
    onMetaChanged(const QString &longName,
                  const QString &valueType,
                  const QString &value,
                  bool           success);

private:

    void
    _createOpItem(NsOpObject *op, int sectionFlags);

    void
    _eraseOpItem(NsOpObject *op);

    void
    _clear();

    Ns3DGraphicsItem*
    _findItem(const NsValueObjectHandle &handle) const;

    Ns3DGraphicsItem*
    _findItem(int32_t id) const;

private:

    void
    _showItem(Ns3DGraphicsItem* item);

    void
    _hideItem(Ns3DGraphicsItem* item);

    void
    _selectItem(Ns3DGraphicsItem* item);

    void
    _unselectItem(Ns3DGraphicsItem* item);

private:    // Member variables.

    typedef QHash<NsValueObjectHandle,int32_t> _IdHashType;
    typedef QHash<int32_t,Ns3DGraphicsItem*> _ItemHashType;

    _IdHashType     _ids;
    _ItemHashType   _items;
    QStack<int32_t> _usedIds;

    Ns3DManipulator* _manip;
    Ns3DConstructionGridItem *_cgItem;

    NsUndoStack *_undoStack;
};

#endif // NS3D_SCENE_H
