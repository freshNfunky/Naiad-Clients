// -----------------------------------------------------------------------------
//
// NsSignatureStore.h
//
// Class for managing signature appearance, header file.
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

#ifndef NS_SIGNATURE_STORE_H
#define NS_SIGNATURE_STORE_H

#include <QMap>
#include <QString>
#include <QPainterPath>
#include <QColor>

// -----------------------------------------------------------------------------

// NsSignatureStore
// ----------------
//! DOCS

class NsSignatureStore
{
public:     // Singleton interface.

    static NsSignatureStore*
    instance();

    static void
    createInstance();

    static void
    destroyInstance();

private:

    static NsSignatureStore *_instance;

public:

    QPainterPath
    shape(const QString &signature,
          qreal          scale);

    //QColor
    //color(const QString &signature,
    //      const QColor  &defaultColor = QColor("#f69050"));

private:

    //void _initColors();
    void _initShapes();

private:

    NsSignatureStore();

    //! DTOR.
    ~NsSignatureStore() 
    {}  

    NsSignatureStore(const NsSignatureStore&);            //!< Disabled.
    NsSignatureStore &operator=(const NsSignatureStore&); //!< Disabled.

private:    // Member variables.

    //typedef QMap<QString, QColor>       _ColorMapType;
    typedef QMap<QString, QPainterPath> _ShapeMapType;

    //_ColorMapType _colors;
    _ShapeMapType _shapes;
};

#endif // NS_SIGNATURE_STORE_H
