// -----------------------------------------------------------------------------
//
// NsSignatureStore.cc
//
// Class for managing signature appearance, source file.
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

#include "NsSignatureStore.h"
#include <QMatrix>

#include <math.h>

// -----------------------------------------------------------------------------

// instance
// --------
//! Provide access to singleton. [static]

NsSignatureStore *
NsSignatureStore::instance()
{
    if (0 == _instance) {
        createInstance();
    }

    return _instance;
}


// createInstance
// --------------
//! Reset singleton. [static]

void
NsSignatureStore::createInstance()
{
    destroyInstance();
    _instance = new NsSignatureStore;
}


// destroyInstance
// ---------------
//! Reset singleton. [static]

void
NsSignatureStore::destroyInstance()
{
    delete _instance;
    _instance = 0;
}


//! Singleton pointer. [static]
NsSignatureStore *NsSignatureStore::_instance = 0;

// -----------------------------------------------------------------------------

QPainterPath
NsSignatureStore::shape(const QString &signature, const qreal scale)
{
    if (_shapes.isEmpty()) {
        // Initialize map if it is empty, as it will be the first time
        // this function is called.

        _initShapes();
    }

    QPainterPath defaultPath;
    defaultPath.addEllipse(-0.25, -0.25, 0.5, 0.5);

    QMatrix xform;
    xform.scale(scale, scale);
    return _shapes.value(signature, defaultPath)*xform;
}


//QColor
//NsSignatureStore::color(const QString &signature, const QColor &defaultColor)
//{
//    if (_colors.isEmpty()) {
//        // Initialize map if it is empty, as it will be the first time
//        // this function is called.
//
//        _initColors();
//    }
//
//    return _colors.value(signature, defaultColor);
//}

// -----------------------------------------------------------------------------

//void
//NsSignatureStore::_initColors()
//{
//    // TODO: Make this less hardcoded, maybe even user customizable!
//    //
//    // #c3c199 : beige.
//    // #626775 : gray.
//    // #60a7e8 : blue.
//    // #b31111 : red.
//    // #85d96b : orange.
//
//    _colors.clear();
//
//    _colors.insert("Body",            QColor("#c3c199"));
//    _colors.insert("Particle",        QColor("#c3c199"));
//    _colors.insert("Mesh",            QColor("#c3c199"));
//    _colors.insert("Field-Gas",       QColor("#c3c199"));
//    _colors.insert("Camera",          QColor("#60c7e8"));
//    _colors.insert("Particle-Liquid", QColor("#60a7e8"));
//    _colors.insert("Field-Liquid",    QColor("#60a7e8"));
//    _colors.insert("Particle-Smoke",  QColor("#b31111"));
//    _colors.insert("Field-Smoke",     QColor("#b31111"));
//    _colors.insert("Volume",          QColor("#85d96b"));
//    _colors.insert("Field",           QColor("#85d96b"));
//    _colors.insert("FieldOp",         QColor("#85d96b"));
//}


void
NsSignatureStore::_initShapes()
{
    // TODO: Make this less hardcoded, maybe even user customizable

    _shapes.clear();

    QPainterPath bodyPath;
    bodyPath.addEllipse(-0.3, -0.3, 0.6, 0.6);
    _shapes.insert("Body", bodyPath);

    QPainterPath particlePath;
    particlePath.addEllipse(-0.2, -0.2, 0.4, 0.4);
    _shapes.insert("Particle",        particlePath);
    _shapes.insert("Particle-Liquid", particlePath);
    _shapes.insert("Particle-Smoke",  particlePath);
    _shapes.insert("Particle-Fire",   particlePath);


    QPainterPath fieldPath;
    fieldPath.addRect(-0.25, -0.25, 0.5, 0.5);
    _shapes.insert("Field",        fieldPath);
    _shapes.insert("Field-Liquid", fieldPath);
    _shapes.insert("Field-Smoke",  fieldPath);
    _shapes.insert("Field-Fire",   fieldPath);
    _shapes.insert("Field-Gas",    fieldPath);
    _shapes.insert("Volume",       fieldPath);
    _shapes.insert("FieldOp",      fieldPath);

    QPainterPath meshPath;
    meshPath.moveTo( 0.,  -0.35);
    meshPath.lineTo( 0.3,  0.2);
    meshPath.lineTo(-0.3,  0.2);
    meshPath.closeSubpath();
    _shapes.insert("Mesh",       meshPath);
    _shapes.insert("Rigid-Body", meshPath);

    //QPainterPath camSubPath0;
    //camSubPath0.addRoundedRect(-0.15, -0.2, 0.45, 0.4, 1., 1.);
    //QPainterPath camSubPath1;
    //camSubPath1.moveTo(-0.3, -0.15);
    //camSubPath1.lineTo(-0.1,  0.);
    //camSubPath1.lineTo(-0.3,  0.15);
    //camSubPath1.closeSubpath();
    //const QPainterPath camPath(camSubPath0.united(camSubPath1));

    const qreal lensAngleRad(15.*(3.1415/180.));

    QPainterPath bodyPath1;
    bodyPath1.addRect(-0.2, -0.15, 0.4, 0.3);

    QPainterPath filmPath;
    filmPath.addEllipse(QPointF(-0.1, -0.2), 0.1, 0.1);          
    filmPath.addEllipse(QPointF(0.15, -0.2), 0.1, 0.1);          

    QPolygonF tri;
    tri << QPointF(0.f, 0.f)
        << QPointF(-0.3*cos(lensAngleRad),  0.3*sin(lensAngleRad))
        << QPointF(-0.3*cos(lensAngleRad), -0.3*sin(lensAngleRad));
    QPainterPath lensPath;
    lensPath.addPolygon(tri);
    lensPath.closeSubpath();

    QPainterPath camPath = bodyPath1.united(lensPath).united(filmPath);

    _shapes.insert("Camera", camPath);
}

// -----------------------------------------------------------------------------

// NsSignatureStore
// ----------------
//! CTOR.

NsSignatureStore::NsSignatureStore()
{
    //_initColors();
    _initShapes();
}
