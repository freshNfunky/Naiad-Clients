// -----------------------------------------------------------------------------
//
// NsPreferences.cc
//
// Naiad Studio preferences, source file.
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

#include "NsPreferences.h"
#include "NsMessageWidget.h"
#include <QFile>
#include <QStack>
#include <QIODevice>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

// -----------------------------------------------------------------------------

// instance
// --------
//! Provide access to singleton. [static]

NsPreferences*
NsPreferences::instance()
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
NsPreferences::createInstance()
{
    destroyInstance();
    _instance = new NsPreferences;
}


// destroyInstance
// ---------------
//! Reset singleton. [static]

void
NsPreferences::destroyInstance()
{
    delete _instance;
    _instance = 0;
}


//! Singleton pointer. [static]
NsPreferences *NsPreferences::_instance = 0;

// -----------------------------------------------------------------------------

bool
NsPreferences::read(QIODevice *device)
{
    QXmlStreamReader xml(device);
    while (!xml.atEnd() && !xml.hasError()) {
        const QXmlStreamReader::TokenType token = xml.readNext();

        // If token is just StartDocument, we'll go to next.
        if (token != QXmlStreamReader::StartDocument &&
            token == QXmlStreamReader::StartElement) {
            // If token is StartElement, we'll see if we can read it.

            const QStringRef name = xml.name();
            if ("nstudio" == name) {
                continue;
            }
            else if ("general" == name) {
                _readGeneral(xml);
            }
            else if ("graph-view" == name) {
                _readGraphView(xml);
            }
            else if ("scope-view" == name) {
                _readScopeView(xml);
            }
        }
    }

    if (xml.hasError()) {
        NsMessageWidget::instance()->clientError(
            xml.errorString()); // Report error.
    }

    // Removes any device() or data from the reader
    // and resets its internal state to the initial state.

    xml.clear();
    emit changed();

    return !xml.error();
}


void 
NsPreferences::write(QIODevice *device) const
{
    QXmlStreamWriter xml(device);
    xml.setCodec("utf-8");
    xml.setAutoFormatting(true); // Nice indentation!
    xml.writeStartDocument();
    xml.writeStartElement("nstudio");

    xml.writeStartElement("general");
    _writePathElement(xml, "style", _generalStylePath);
    xml.writeEndElement();  // general.

    xml.writeStartElement("graph-view");
    _writeColorElement(xml, "background", _graphViewBackground);
    _writeColorElement(xml, "background-live", _graphViewLiveBackground);
    _writeColorElement(xml, "selection-color", _graphViewSelectionColor);

    _writeColorElement(xml, "feed-line", _graphViewFeedLineColor);
    _writeColorElement(xml, "feed-replaced-line", _graphViewFeedReplacedLineColor);
    _writeColorElement(xml, "feed-valid-line", _graphViewFeedValidLineColor);
    _writeColorElement(xml, "feed-invalid-line", _graphViewFeedInvalidLineColor);
    
    _writeColorElement(xml, "plug-line", _graphViewPlugLineColor);
    _writeColorElement(xml, "plug-text", _graphViewPlugTextColor);
    _writeColorElement(xml, "plug-background", _graphViewPlugBackground);

    _writeColorElement(xml, "body-line", _graphViewBodyLineColor);
    _writeColorElement(xml, "body-text", _graphViewBodyTextColor);
    _writeColorElement(xml, "body-background", _graphViewBodyBackground);

    _writeColorElement(xml, "op-line", _graphViewOpLineColor);
    _writeColorElement(xml, "op-text", _graphViewOpTextColor);
    _writeColorElement(xml, "op-background", _graphViewOpBackground);
    _writeColorElement(xml, "op-disabled", _graphViewOpDisabledColor);
    _writeMappedColorElement(xml, "op-state", _graphViewOpStateColors);
    _writeColorElement(xml, "op-state-default", _graphViewOpStateDefaultColor);
    _writeMappedColorElement(xml, "op-category", _graphViewOpCategoryColors);
    _writeColorElement(xml, "op-category-default", _graphViewOpCategoryDefaultColor);

    _writeMappedColorElement(xml, "signature", _graphViewSignatureColors);
    _writeColorElement(xml, "signature-default", _graphViewSignatureDefaultColor);
    xml.writeEndElement(); // graph-view

    xml.writeStartElement("scope-view");
    _writeColorElement(xml, "background", _scopeViewBackground);
    _writeColorElement(xml, "selection-color", _scopeViewSelectionColor);
    _writeColorElement(xml, "construction-grid", _scopeViewConstructionGridColor);

    _writeColorElement(xml, "axis-line", _scopeViewAxisLineColor);
    _writeColorElement(xml, "axis-text", _scopeViewAxisTextColor);
    _writeColorElement(xml, "box-line", _scopeViewBoxLineColor);
    _writeColorElement(xml, "box-text", _scopeViewBoxTextColor);
    _writeColorElement(xml, "clip-box-line", _scopeViewClipBoxLineColor);
    _writeColorElement(xml, "clip-box-text", _scopeViewClipBoxTextColor);
    _writeColorElement(xml, "frustum-line", _scopeViewFrustumLineColor);
    _writeColorElement(xml, "frustum-text", _scopeViewFrustumTextColor);
    _writeColorElement(xml, "import-transform-line", _scopeViewFrustumLineColor);
    _writeColorElement(xml, "import-transform-text", _scopeViewFrustumTextColor);
    _writeColorElement(xml, "plane-line", _scopeViewPlaneLineColor);
    _writeColorElement(xml, "plane-text", _scopeViewPlaneTextColor);
    _writeColorElement(xml, "sphere-line", _scopeViewSphereLineColor);
    _writeColorElement(xml, "sphere-text", _scopeViewSphereTextColor);

    _writeColorElement(xml, "hud-text", _scopeViewHudTextColor);
    _writeColorElement(xml, "body-label-text", _scopeViewBodyLabelTextColor);
    xml.writeEndElement(); // scope-view
    xml.writeEndElement(); // nstudio
    xml.writeEndDocument();
}


void
NsPreferences::reset()
{
    _graphViewOpCategoryColors.clear();
    _graphViewOpStateColors.clear();
    _graphViewSignatureColors.clear();

    QFile file(":/styles/default-pref.xml");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        read(&file);
        file.close();
    }
}

// -----------------------------------------------------------------------------

bool
NsPreferences::_readGeneral(QXmlStreamReader &xml)
{
    // Check that we are in the right element.

    if (xml.tokenType() != QXmlStreamReader::StartElement &&
        xml.name() == "general") {
        return false;
    }
    xml.readNext();

    // Read until we find </graph-view>.

    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "general")) {
        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            if ("style" == xml.name()) {
                _generalStylePath = 
                    _parseStringAttribute(xml.attributes(), "path");
            }
        }
        xml.readNext();
    }
    return true;    // Success!
}

bool
NsPreferences::_readGraphView(QXmlStreamReader &xml)
{
    // Check that we are in the right element.

    if (xml.tokenType() != QXmlStreamReader::StartElement &&
        xml.name() == "graph-view") {
        return false;
    }
    xml.readNext();

    // Read until we find </graph-view>.

    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
             xml.name() == "graph-view")) {
        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            if ("background" == xml.name()) {
                _graphViewBackground = 
                    _parseColorAttributes(xml.attributes());
            }
            else if ("background-live" == xml.name()) {
                _graphViewLiveBackground = 
                    _parseColorAttributes(xml.attributes());
            }
            else if ("selection-color" == xml.name()) {
                _graphViewSelectionColor = 
                    _parseColorAttributes(xml.attributes());                                            
            }
            else if ("feed-line" == xml.name()) {
                _graphViewFeedLineColor = 
                    _parseColorAttributes(xml.attributes());                                            
            }
            else if ("feed-replaced-line" == xml.name()) {
                _graphViewFeedReplacedLineColor = 
                    _parseColorAttributes(xml.attributes());                                            
            }
            else if ("feed-valid-line" == xml.name()) {
                _graphViewFeedValidLineColor = 
                    _parseColorAttributes(xml.attributes());                                            
            }
            else if ("feed-invalid-line" == xml.name()) {
                _graphViewFeedInvalidLineColor = 
                    _parseColorAttributes(xml.attributes());                                            
            }
            else if ("plug-line" == xml.name()) {
                _graphViewPlugLineColor = 
                    _parseColorAttributes(xml.attributes());                                            
            }
            else if ("plug-text" == xml.name()) {
                _graphViewPlugTextColor = 
                    _parseColorAttributes(xml.attributes());                                            
            }
            else if ("plug-background" == xml.name()) {
                _graphViewPlugBackground = 
                    _parseColorAttributes(xml.attributes());                                            
            }
            else if ("body-line" == xml.name()) {
                _graphViewBodyLineColor = 
                    _parseColorAttributes(xml.attributes());                                            
            }
            else if ("body-text" == xml.name()) {
                _graphViewBodyTextColor = 
                    _parseColorAttributes(xml.attributes());                                            
            }
            else if ("body-background" == xml.name()) {
                _graphViewBodyBackground = 
                    _parseColorAttributes(xml.attributes());                                            
            }
            else if ("op-line" == xml.name()) {
                _graphViewOpLineColor = 
                    _parseColorAttributes(xml.attributes());                                            
            }
            else if ("op-text" == xml.name()) {
                _graphViewOpTextColor = 
                    _parseColorAttributes(xml.attributes());                                            
            }
            else if ("op-background" == xml.name()) {
                _graphViewOpBackground = 
                    _parseColorAttributes(xml.attributes());                                            
            }
            else if ("op-disabled" == xml.name()) {
                _graphViewOpDisabledColor = 
                    _parseColorAttributes(xml.attributes());                                            
            }
            else if ("op-state" == xml.name()) {
                const QXmlStreamAttributes attr = xml.attributes();
                _graphViewOpStateColors[_parseStringAttribute(attr,"name")]=
                    _parseColorAttributes(attr);
            }
            else if ("op-state-default" == xml.name()) {
                _graphViewOpStateDefaultColor = 
                    _parseColorAttributes(xml.attributes());
            }
            else if ("op-category" == xml.name()) {
                const QXmlStreamAttributes attr = xml.attributes();
                _graphViewOpCategoryColors[_parseStringAttribute(attr,"name")]=
                    _parseColorAttributes(attr);
            }
            else if ("op-category-default" == xml.name()) {
                _graphViewOpCategoryDefaultColor = 
                    _parseColorAttributes(xml.attributes());
            }
            else if ("signature" == xml.name()) {
                const QXmlStreamAttributes attr = xml.attributes();
                _graphViewSignatureColors[_parseStringAttribute(attr,"name")] =
                    _parseColorAttributes(attr);
            }
            else if ("signature-default" == xml.name()) {
                _graphViewSignatureDefaultColor = 
                    _parseColorAttributes(xml.attributes());
            }
        }
        xml.readNext();
    }
    return true;    // Success!
}


bool
NsPreferences::_readScopeView(QXmlStreamReader &xml)
{
    // Check that we are in the right element.

    if (xml.tokenType() != QXmlStreamReader::StartElement &&
        xml.name() == "scope-view") {
        return false;
    }
    xml.readNext();

    // Read until we find </graph-view>.

    while (!(xml.tokenType() == QXmlStreamReader::EndElement &&
            xml.name() == "scope-view")) {
        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            if ("background" == xml.name()) {
                _scopeViewBackground = 
                    _parseColorAttributes(xml.attributes());
            }
            else if ("selection-color" == xml.name()) {
                _scopeViewSelectionColor = 
                    _parseColorAttributes(xml.attributes());
            }
            else if ("construction-grid" == xml.name()) {
                _scopeViewConstructionGridColor =
                    _parseColorAttributes(xml.attributes());
            }
            else if ("axis-line" == xml.name()) {
                _scopeViewAxisLineColor = 
                    _parseColorAttributes(xml.attributes());
            }
            else if ("axis-text" == xml.name()) {
                _scopeViewAxisTextColor = 
                    _parseColorAttributes(xml.attributes());
            }
            else if ("box-line" == xml.name()) {
                _scopeViewBoxLineColor = 
                    _parseColorAttributes(xml.attributes());
            }
            else if ("box-text" == xml.name()) {
                _scopeViewBoxTextColor = 
                    _parseColorAttributes(xml.attributes());
            }
            else if ("clip-box-line" == xml.name()) {
                _scopeViewClipBoxLineColor = 
                    _parseColorAttributes(xml.attributes());
            }
            else if ("clip-box-text" == xml.name()) {
                _scopeViewClipBoxTextColor = 
                    _parseColorAttributes(xml.attributes());
            }
            else if ("frustum-line" == xml.name()) {
                _scopeViewFrustumLineColor = 
                    _parseColorAttributes(xml.attributes());
            }
            else if ("frustum-text" == xml.name()) {
                _scopeViewFrustumTextColor = 
                    _parseColorAttributes(xml.attributes());
            }
            else if ("import-transform-line" == xml.name()) {
                _scopeViewImportTransformLineColor =
                    _parseColorAttributes(xml.attributes());
            }
            else if ("import-transform-text" == xml.name()) {
                _scopeViewImportTransformTextColor =
                    _parseColorAttributes(xml.attributes());
            }
            else if ("plane-line" == xml.name()) {
                _scopeViewPlaneLineColor = 
                    _parseColorAttributes(xml.attributes());
            }
            else if ("plane-text" == xml.name()) {
                _scopeViewPlaneTextColor = 
                    _parseColorAttributes(xml.attributes());
            }
            else if ("sphere-line" == xml.name()) {
                _scopeViewSphereLineColor = 
                    _parseColorAttributes(xml.attributes());
            }
            else if ("sphere-text" == xml.name()) {
                _scopeViewSphereTextColor = 
                    _parseColorAttributes(xml.attributes());
            }
            else if ("hud-text" == xml.name()) {
                _scopeViewHudTextColor = 
                    _parseColorAttributes(xml.attributes());
            }
            else if ("body-label-text" == xml.name()) {
                _scopeViewBodyLabelTextColor = 
                    _parseColorAttributes(xml.attributes());
            }
        }
        xml.readNext();
    }
    return true;    // Success!
}


QColor
NsPreferences::_parseColorAttributes(const QXmlStreamAttributes &attr)
{
    QColor color(QColor::Invalid);
    if (attr.hasAttribute("r") && 
        attr.hasAttribute("g") &&
        attr.hasAttribute("b")) {
        bool rOk = false;
        bool gOk = false;
        bool bOk = false;
        const int r = attr.value("r").toString().toInt(&rOk);
        const int g = attr.value("g").toString().toInt(&gOk);
        const int b = attr.value("b").toString().toInt(&bOk);
        if (rOk && gOk && bOk) {
            color = QColor(r, g, b);
        }
    }
    return color;
}

QString
NsPreferences::_parseStringAttribute(const QXmlStreamAttributes &attr,
                                     const QString              &qualifiedName)
{
    QString str;
    if (attr.hasAttribute(qualifiedName)) {
        str = attr.value(qualifiedName).toString();
    }
    return str;
}

void
NsPreferences::_writeColorElement(QXmlStreamWriter &xml, 
                                  const QString    &qualifiedName, 
                                  const QColor     &color)
{
    xml.writeStartElement(qualifiedName);
    xml.writeAttribute("r", QString::number(color.red()));
    xml.writeAttribute("g", QString::number(color.green()));
    xml.writeAttribute("b", QString::number(color.blue()));
    xml.writeEndElement();
}

void
NsPreferences::_writeMappedColorElement(QXmlStreamWriter           &xml, 
                                        const QString            &qualifiedName, 
                                        const QMap<QString,QColor> &colors)
{
    const QMap<QString,QColor>::ConstIterator iend = colors.constEnd();
    QMap<QString,QColor>::ConstIterator iter = colors.constBegin();
    for (; iter != iend; ++iter) {
        xml.writeStartElement(qualifiedName);
        xml.writeAttribute("name", iter.key());
        xml.writeAttribute("r", QString::number(iter.value().red()));
        xml.writeAttribute("g", QString::number(iter.value().green()));
        xml.writeAttribute("b", QString::number(iter.value().blue()));
        xml.writeEndElement();
    }
}

void
NsPreferences::_writePathElement(QXmlStreamWriter &xml, 
                                 const QString    &qualifiedName, 
                                 const QString    &path)
{
    xml.writeStartElement(qualifiedName);
    xml.writeAttribute("path", path);
    xml.writeEndElement();
}

// -----------------------------------------------------------------------------

// NsPreferences
// -------------
//! CTOR.

NsPreferences::NsPreferences(QObject *parent)
    : QObject(parent)
{
    reset();    // Set default values.
}
