// -----------------------------------------------------------------------------
//
// NsInputPlugObject.h
//
// Naiad Studio class for representing a Ng::Input, header file.
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

#ifndef NS_INPUT_PLUG_OBJECT_H
#define NS_INPUT_PLUG_OBJECT_H

#include "NsPlugObject.h"   // Base.
#include <NgInput.h>        // Resource type.

class NsOpObject;           // Parent type.
class NsBodyObject;

// -----------------------------------------------------------------------------

// NsInputPlugObject
// -----------------
//! Abstract base class for objects representing input plugs.

class NsInputPlugObject : public NsPlugObject
{
    Q_OBJECT

public:

    static const QString&
    selectionMetaValueType()
    { return _selectionMetaValueType; }

    static const QString&
    selectedMetaValue(const bool selected)
    { return (selected ? _selectedMetaValue : _unselectedMetaValue); }

    static bool
    isSelected(const QString &metaValue)
    { return (metaValue == _selectedMetaValue); }

private:

    static const QString _selectionMetaValueType;
    static const QString _selectedMetaValue;
    static const QString _unselectedMetaValue;

public:     // Plug object interface.

    virtual
    ~NsInputPlugObject() = 0;    // Force abstract.

    //! Covariant return type.
    virtual const Ng::Input*
    constNgPlug() const
    { return _input; }

    //! Covariant return type.
    virtual Ng::Input*
    mutableNgPlug() 
    { return _input; }

public:     // Input.

    const Ng::Input*
    constInput() const
    { return _input; }

    Ng::Input*
    mutableInput()
    { return _input; }

public:

    Ng::Plug*
    source() const;

public:

    bool
    isSelected() const;

    void
    emitSelectionChanged() const
    { emit selectionChanged(); }

protected:      // Hooks.

    virtual void
    metaChanged(const QString &longName,
                const QString &valueType,
                const QString &value);

signals:

    void
    selectionChanged() const;

protected:

    explicit
    NsInputPlugObject(Ng::Input &input, NsOpObject &parent);

protected:

    virtual void
    groupPlugChanged() = 0;

private:    // Member variables.

    Ng::Input *_input;  //!< Server resource.
};

// -----------------------------------------------------------------------------

#endif // NS_INPUT_PLUG_OBJECT_H
