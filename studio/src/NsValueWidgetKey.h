// -----------------------------------------------------------------------------
//
// NsValueWidgetKey.h
//
// Naiad Studio value widget key, header file.
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

#ifndef NS_VALUE_WIDGET_KEY_H
#define NS_VALUE_WIDGET_KEY_H

#include <QString>
#include <QHash>

// -----------------------------------------------------------------------------

// NsValueWidgetKey
// ----------------
//! Simple class for identifying value widget types.

class NsValueWidgetKey
{
public:

    //! CTOR.
    explicit 
    NsValueWidgetKey(const QString &typeName,
                     const QString &subTypeName = "")
        : _typeName(typeName)
        , _subTypeName(subTypeName)
    {}

    //! Copy CTOR.
    NsValueWidgetKey(const NsValueWidgetKey &rhs)
        : _typeName(rhs._typeName)
        , _subTypeName(rhs._subTypeName)
    {}

    //! DTOR.
    virtual
    ~NsValueWidgetKey()
    {}    

    //! Assignment operator.
    NsValueWidgetKey&
    operator=(const NsValueWidgetKey &rhs)
    {
        _typeName = rhs._typeName;
        _subTypeName = rhs._subTypeName;
        return *this;
    }

public:

    const QString&
    typeName() const
    { return _typeName; }

    const QString&
    subTypeName() const
    { return _subTypeName; }

private:    // Member variables.

    QString _typeName;      //!< Type string.
    QString _subTypeName;   //!< Sub-type string.

private:

    //! Empty CTOR - disabled.
    NsValueWidgetKey(); 
};

// -----------------------------------------------------------------------------

// Functions required to use NsValueWidgetKey with QHash structure.

uint 
qHash(const NsValueWidgetKey &key);

bool 
operator==(const NsValueWidgetKey &lhs, const NsValueWidgetKey &rhs);

#endif // NS_VALUE_WIDGET_KEY_H
