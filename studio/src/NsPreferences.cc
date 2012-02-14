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
#include <QCoreApplication>
#include <QSettings>
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

const QString NsPreferences::_settingsGroup = QString("Preferences");

void
NsPreferences::_readSettings()
{
    QSettings s(QSettings::IniFormat,
                QSettings::UserScope,
                QCoreApplication::organizationName(),
                QCoreApplication::applicationName());
    s.beginGroup(_settingsGroup);
    {
        s.beginGroup(_GraphView::settingsGroup);
        {
            _readColorSetting(s, "Background",
                              _gv.background,
               _GraphView::defaultBackground);
            _readColorSetting(s, "LiveBackground",
                              _gv.liveBackground,
               _GraphView::defaultLiveBackground);
            _readColorSetting(s, "SelectionColor",
                              _gv.selectionColor,
               _GraphView::defaultSelectionColor);

            _readColorSetting(s, "FeedLineColor", 
                              _gv.feedLineColor,
               _GraphView::defaultFeedLineColor);
            _readColorSetting(s, "FeedReplacedLineColor", 
                              _gv.feedReplacedLineColor,
               _GraphView::defaultFeedReplacedLineColor);
            _readColorSetting(s, "FeedValidLineColor", 
                              _gv.feedValidLineColor,
               _GraphView::defaultFeedValidLineColor);
            _readColorSetting(s, "FeedInvalidLineColor", 
                              _gv.feedInvalidLineColor,
               _GraphView::defaultFeedInvalidLineColor);

            _readColorSetting(s, "PlugLineColor", 
                              _gv.plugLineColor,
               _GraphView::defaultPlugLineColor);
            _readColorSetting(s, "PlugTextColor", 
                              _gv.plugTextColor,
               _GraphView::defaultPlugTextColor);
            _readColorSetting(s, "PlugBackground", 
                              _gv.plugBackground,
               _GraphView::defaultPlugBackground);

            _readColorSetting(s, "BodyLineColor",
                              _gv.bodyLineColor,
               _GraphView::defaultBodyLineColor);
            _readColorSetting(s, "BodyTextColor",
                              _gv.bodyTextColor,
               _GraphView::defaultBodyTextColor);
            _readColorSetting(s, "BodyBackground",
                              _gv.bodyBackground,
               _GraphView::defaultBodyBackground);

            _readColorSetting(s, "OpLineColor",
                              _gv.opLineColor,
               _GraphView::defaultOpLineColor);
            _readColorSetting(s, "OpTextColor",
                              _gv.opTextColor,
               _GraphView::defaultOpTextColor);
            _readColorSetting(s, "OpBackground",
                              _gv.opBackground,
               _GraphView::defaultOpBackground);
            _readColorSetting(s, "OpDisabledColor",
                              _gv.opDisabledColor,
               _GraphView::defaultOpDisabledColor);

            _readColorSetting(s, "OpStateDefaultColor",
                              _gv.opStateDefaultColor,
               _GraphView::defaultOpStateDefaultColor);
            s.beginGroup("OpStateColors");
            {
                _readMappedColorSetting(s, _gv.opStateColors, 
                            _GraphView::defaultOpStateColors());
            }
            s.endGroup();   // "OpStateColors"

            _readColorSetting(s, "OpCategoryDefaultColor",
                              _gv.opCategoryDefaultColor,
               _GraphView::defaultOpCategoryDefaultColor);
            s.beginGroup("OpCategoryColors");
            {
                _readMappedColorSetting(s, _gv.opCategoryColors, 
                            _GraphView::defaultOpCategoryColors());
            }
            s.endGroup();   // "OpCategoryColors"

            _readColorSetting(s, "SignatureDefaultColor",
                              _gv.signatureDefaultColor,
               _GraphView::defaultSignatureDefaultColor);
            s.beginGroup("SignatureColors");
            {
                _readMappedColorSetting(s, _gv.signatureColors, 
                            _GraphView::defaultSignatureColors());
            }
            s.endGroup();   // "SignatureColors"
        }
        s.endGroup();   // "GraphView"

        s.beginGroup(_ScopeView::settingsGroup);
        {

        }
        s.endGroup();   // "ScopeView"

    }
    s.endGroup();    // "Preferences"
}


//! DOCS [static]
void
NsPreferences::_readColorSetting(const QSettings &s, 
                                 const QString   &k, 
                                 QColor          &c,
                                 const QString   &d)
{
    c = QColor(s.value(k, d).toString());
}


//! DOCS [static]
void
NsPreferences::_readMappedColorSetting(const QSettings            &s,  
                                       QMap<QString,QColor>       &m,
                                       const QMap<QString,QColor> &d)
{
    QColor color;
    foreach (const QString &key, s.allKeys()) {
        _readColorSetting(s, key, color, 
            d.value(key, QColor(QColor::Invalid)).name());
        m.insert(key, color);
    }
}


//! DOCS 
void
NsPreferences::_writeSettings() const
{
    QSettings s(QSettings::IniFormat,
                QSettings::UserScope,
                QCoreApplication::organizationName(),
                QCoreApplication::applicationName());
    s.beginGroup(_settingsGroup);
    {
        s.beginGroup(_GraphView::settingsGroup);
        {
            _writeColorSetting(s, "Background", 
                               _gv.background);
            _writeColorSetting(s, "LiveBackground", 
                               _gv.liveBackground);
            _writeColorSetting(s, "SelectionColor", 
                               _gv.selectionColor);

            _writeColorSetting(s, "FeedLineColor", 
                               _gv.feedLineColor);
            _writeColorSetting(s, "FeedReplacedLineColor", 
                               _gv.feedReplacedLineColor);
            _writeColorSetting(s, "FeedValidLineColor", 
                               _gv.feedValidLineColor);
            _writeColorSetting(s, "FeedInvalidLineColor", 
                               _gv.feedInvalidLineColor);

            _writeColorSetting(s, "PlugLineColor",
                               _gv.plugLineColor);
            _writeColorSetting(s, "PlugTextColor",
                               _gv.plugTextColor);
            _writeColorSetting(s, "PlugBackground",
                               _gv.plugBackground);

            _writeColorSetting(s, "BodyLineColor",
                               _gv.bodyLineColor);
            _writeColorSetting(s, "BodyTextColor",
                               _gv.bodyTextColor);
            _writeColorSetting(s, "BodyBackground",
                               _gv.bodyBackground);

            _writeColorSetting(s, "OpLineColor",
                               _gv.opLineColor);
            _writeColorSetting(s, "OpTextColor",
                               _gv.opTextColor);
            _writeColorSetting(s, "OpBackground",
                               _gv.opBackground);
            _writeColorSetting(s, "OpDisabledColor",
                               _gv.opDisabledColor);
            _writeColorSetting(s, "OpStateDefaultColor",
                               _gv.opStateDefaultColor);
            s.beginGroup("OpStateColors");
                _writeMappedColorSetting(s, _gv.opStateColors);
            s.endGroup();   // "OpStateColors"

            _writeColorSetting(s, "OpCategoryDefaultColor",
                               _gv.opCategoryDefaultColor);
            s.beginGroup("OpCategoryColors");
                _writeMappedColorSetting(s, _gv.opCategoryColors);
            s.endGroup();   // "OpCategoryColors"

            _writeColorSetting(s, "SignatureDefaultColor",
                               _gv.signatureDefaultColor);
            s.beginGroup("SignatureColors");
                _writeMappedColorSetting(s, _gv.signatureColors);
            s.endGroup();   // "SignatureColors"
        }
        s.endGroup();   // "GraphView.

        s.beginGroup(_ScopeView::settingsGroup);
        {
            s.setValue("Background", 
                       _sv.background.name());
            s.setValue("SelectionColor", 
                       _sv.selectionColor.name());
            s.setValue("ConstructionGridColor", 
                       _sv.constructionGridColor.name());

            s.setValue("AxisLineColor", 
                       _sv.axisLineColor.name());
            s.setValue("AxisTextColor", 
                       _sv.axisTextColor.name());
            s.setValue("BoxLineColor", 
                       _sv.boxLineColor.name());
            s.setValue("BoxTextColor", 
                       _sv.boxTextColor.name());
            s.setValue("ClipBoxLineColor", 
                       _sv.clipBoxLineColor.name());
            s.setValue("ClipBoxTextColor", 
                       _sv.clipBoxTextColor.name());
            s.setValue("FrustumLineColor", 
                       _sv.frustumLineColor.name());
            s.setValue("FrustumTextColor", 
                       _sv.frustumTextColor.name());
            s.setValue("ImportTransformLineColor", 
                       _sv.importTransformLineColor.name());
            s.setValue("ImportTransformTextColor", 
                       _sv.importTransformTextColor.name());
            s.setValue("PlaneLineColor", 
                       _sv.planeLineColor.name());
            s.setValue("PlaneTextColor", 
                       _sv.planeTextColor.name());
            s.setValue("SphereLineColor", 
                       _sv.sphereLineColor.name());
            s.setValue("SphereTextColor", 
                       _sv.sphereTextColor.name());

            s.setValue("HudTextColor", 
                       _sv.hudTextColor.name());
            s.setValue("BodyLabelTextColor", 
                       _sv.bodyLabelTextColor.name());
        }
        s.endGroup();   // "ScopeView"
    }
    s.endGroup();   // "Preferences"
}


//! DOCS [static]
void
NsPreferences::_writeColorSetting(QSettings     &s, 
                                  const QString &k, 
                                  const QColor  &c)
{
    s.setValue(k, c.name()); 
}


//! DOCS [static]
void
NsPreferences::_writeMappedColorSetting(QSettings                  &s, 
                                        const QMap<QString,QColor> &map)
{
    QMap<QString,QColor>::const_iterator iter = map.constBegin();
    const QMap<QString,QColor>::const_iterator iend = map.constEnd();
    for (; iter != iend; ++iter) {
        _writeColorSetting(s, iter.key(), iter.value());
    }
}

// -----------------------------------------------------------------------------

bool
NsPreferences::readXml(QIODevice *device)
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
                _readXmlGeneral(xml);
            }
            else if ("graph-view" == name) {
                _readXmlGraphView(xml);
            }
            else if ("scope-view" == name) {
                _readXmlScopeView(xml);
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
NsPreferences::writeXml(QIODevice *device) const
{
    QXmlStreamWriter xml(device);
    xml.setCodec("utf-8");
    xml.setAutoFormatting(true); // Nice indentation!
    xml.writeStartDocument();
    xml.writeStartElement("nstudio");

    xml.writeStartElement("general");
    _writeXmlPathElement(xml, "style", _gen.stylePath);
    xml.writeEndElement();  // general.

    xml.writeStartElement("graph-view");
    _writeXmlColorElement(xml, "background", _gv.background);
    _writeXmlColorElement(xml, "background-live", _gv.liveBackground);
    _writeXmlColorElement(xml, "selection-color", _gv.selectionColor);

    _writeXmlColorElement(xml, "feed-line", _gv.feedLineColor);
    _writeXmlColorElement(xml, "feed-replaced-line", _gv.feedReplacedLineColor);
    _writeXmlColorElement(xml, "feed-valid-line", _gv.feedValidLineColor);
    _writeXmlColorElement(xml, "feed-invalid-line", _gv.feedInvalidLineColor);
    
    _writeXmlColorElement(xml, "plug-line", _gv.plugLineColor);
    _writeXmlColorElement(xml, "plug-text", _gv.plugTextColor);
    _writeXmlColorElement(xml, "plug-background", _gv.plugBackground);

    _writeXmlColorElement(xml, "body-line", _gv.bodyLineColor);
    _writeXmlColorElement(xml, "body-text", _gv.bodyTextColor);
    _writeXmlColorElement(xml, "body-background", _gv.bodyBackground);

    _writeXmlColorElement(xml, "op-line", _gv.opLineColor);
    _writeXmlColorElement(xml, "op-text", _gv.opTextColor);
    _writeXmlColorElement(xml, "op-background", _gv.opBackground);
    _writeXmlColorElement(xml, "op-disabled", _gv.opDisabledColor);
    _writeXmlMappedColorElement(xml, "op-state", _gv.opStateColors);
    _writeXmlColorElement(xml, "op-state-default", _gv.opStateDefaultColor);
    _writeXmlMappedColorElement(xml, "op-category", _gv.opCategoryColors);
    _writeXmlColorElement(xml, "op-category-default", _gv.opCategoryDefaultColor);

    _writeXmlMappedColorElement(xml, "signature", _gv.signatureColors);
    _writeXmlColorElement(xml, "signature-default", _gv.signatureDefaultColor);
    xml.writeEndElement(); // graph-view

    xml.writeStartElement("scope-view");
    _writeXmlColorElement(xml, "background", _sv.background);
    _writeXmlColorElement(xml, "selection-color", _sv.selectionColor);
    _writeXmlColorElement(xml, "construction-grid", _sv.constructionGridColor);

    _writeXmlColorElement(xml, "axis-line", _sv.axisLineColor);
    _writeXmlColorElement(xml, "axis-text", _sv.axisTextColor);
    _writeXmlColorElement(xml, "box-line", _sv.boxLineColor);
    _writeXmlColorElement(xml, "box-text", _sv.boxTextColor);
    _writeXmlColorElement(xml, "clip-box-line", _sv.clipBoxLineColor);
    _writeXmlColorElement(xml, "clip-box-text", _sv.clipBoxTextColor);
    _writeXmlColorElement(xml, "frustum-line", _sv.frustumLineColor);
    _writeXmlColorElement(xml, "frustum-text", _sv.frustumTextColor);
    _writeXmlColorElement(xml, "import-transform-line", _sv.frustumLineColor);
    _writeXmlColorElement(xml, "import-transform-text", _sv.frustumTextColor);
    _writeXmlColorElement(xml, "plane-line", _sv.planeLineColor);
    _writeXmlColorElement(xml, "plane-text", _sv.planeTextColor);
    _writeXmlColorElement(xml, "sphere-line", _sv.sphereLineColor);
    _writeXmlColorElement(xml, "sphere-text", _sv.sphereTextColor);

    _writeXmlColorElement(xml, "hud-text", _sv.hudTextColor);
    _writeXmlColorElement(xml, "body-label-text", _sv.bodyLabelTextColor);
    xml.writeEndElement(); // scope-view
    xml.writeEndElement(); // nstudio
    xml.writeEndDocument();
}


bool
NsPreferences::_readXmlGeneral(QXmlStreamReader &xml)
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
                _gen.stylePath = 
                    _parseXmlStringAttribute(xml.attributes(), "path");
            }
        }
        xml.readNext();
    }
    return true;    // Success!
}

bool
NsPreferences::_readXmlGraphView(QXmlStreamReader &xml)
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
                _gv.background = 
                    _parseXmlColorAttributes(xml.attributes());
            }
            else if ("background-live" == xml.name()) {
                _gv.liveBackground = 
                    _parseXmlColorAttributes(xml.attributes());
            }
            else if ("selection-color" == xml.name()) {
                _gv.selectionColor = 
                    _parseXmlColorAttributes(xml.attributes());                                            
            }
            else if ("feed-line" == xml.name()) {
                _gv.feedLineColor = 
                    _parseXmlColorAttributes(xml.attributes());                                            
            }
            else if ("feed-replaced-line" == xml.name()) {
                _gv.feedReplacedLineColor = 
                    _parseXmlColorAttributes(xml.attributes());                                            
            }
            else if ("feed-valid-line" == xml.name()) {
                _gv.feedValidLineColor = 
                    _parseXmlColorAttributes(xml.attributes());                                            
            }
            else if ("feed-invalid-line" == xml.name()) {
                _gv.feedInvalidLineColor = 
                    _parseXmlColorAttributes(xml.attributes());                                            
            }
            else if ("plug-line" == xml.name()) {
                _gv.plugLineColor = 
                    _parseXmlColorAttributes(xml.attributes());                                            
            }
            else if ("plug-text" == xml.name()) {
                _gv.plugTextColor = 
                    _parseXmlColorAttributes(xml.attributes());                                            
            }
            else if ("plug-background" == xml.name()) {
                _gv.plugBackground = 
                    _parseXmlColorAttributes(xml.attributes());                                            
            }
            else if ("body-line" == xml.name()) {
                _gv.bodyLineColor = 
                    _parseXmlColorAttributes(xml.attributes());                                            
            }
            else if ("body-text" == xml.name()) {
                _gv.bodyTextColor = 
                    _parseXmlColorAttributes(xml.attributes());                                            
            }
            else if ("body-background" == xml.name()) {
                _gv.bodyBackground = 
                    _parseXmlColorAttributes(xml.attributes());                                            
            }
            else if ("op-line" == xml.name()) {
                _gv.opLineColor = 
                    _parseXmlColorAttributes(xml.attributes());                                            
            }
            else if ("op-text" == xml.name()) {
                _gv.opTextColor = 
                    _parseXmlColorAttributes(xml.attributes());                                            
            }
            else if ("op-background" == xml.name()) {
                _gv.opBackground = 
                    _parseXmlColorAttributes(xml.attributes());                                            
            }
            else if ("op-disabled" == xml.name()) {
                _gv.opDisabledColor = 
                    _parseXmlColorAttributes(xml.attributes());                                            
            }
            else if ("op-state" == xml.name()) {
                const QXmlStreamAttributes attr = xml.attributes();
                _gv.opStateColors[_parseXmlStringAttribute(attr,"name")] =
                    _parseXmlColorAttributes(attr);
            }
            else if ("op-state-default" == xml.name()) {
                _gv.opStateDefaultColor = 
                    _parseXmlColorAttributes(xml.attributes());
            }
            else if ("op-category" == xml.name()) {
                const QXmlStreamAttributes attr = xml.attributes();
                _gv.opCategoryColors[_parseXmlStringAttribute(attr,"name")] =
                    _parseXmlColorAttributes(attr);
            }
            else if ("op-category-default" == xml.name()) {
                _gv.opCategoryDefaultColor = 
                    _parseXmlColorAttributes(xml.attributes());
            }
            else if ("signature" == xml.name()) {
                const QXmlStreamAttributes attr = xml.attributes();
                _gv.signatureColors[_parseXmlStringAttribute(attr,"name")] =
                    _parseXmlColorAttributes(attr);
            }
            else if ("signature-default" == xml.name()) {
                _gv.signatureDefaultColor = 
                    _parseXmlColorAttributes(xml.attributes());
            }
        }
        xml.readNext();
    }
    return true;    // Success!
}


bool
NsPreferences::_readXmlScopeView(QXmlStreamReader &xml)
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
                _sv.background = 
                    _parseXmlColorAttributes(xml.attributes());
            }
            else if ("selection-color" == xml.name()) {
                _sv.selectionColor = 
                    _parseXmlColorAttributes(xml.attributes());
            }
            else if ("construction-grid" == xml.name()) {
                _sv.constructionGridColor =
                    _parseXmlColorAttributes(xml.attributes());
            }
            else if ("axis-line" == xml.name()) {
                _sv.axisLineColor = 
                    _parseXmlColorAttributes(xml.attributes());
            }
            else if ("axis-text" == xml.name()) {
                _sv.axisTextColor = 
                    _parseXmlColorAttributes(xml.attributes());
            }
            else if ("box-line" == xml.name()) {
                _sv.boxLineColor = 
                    _parseXmlColorAttributes(xml.attributes());
            }
            else if ("box-text" == xml.name()) {
                _sv.boxTextColor = 
                    _parseXmlColorAttributes(xml.attributes());
            }
            else if ("clip-box-line" == xml.name()) {
                _sv.clipBoxLineColor = 
                    _parseXmlColorAttributes(xml.attributes());
            }
            else if ("clip-box-text" == xml.name()) {
                _sv.clipBoxTextColor = 
                    _parseXmlColorAttributes(xml.attributes());
            }
            else if ("frustum-line" == xml.name()) {
                _sv.frustumLineColor = 
                    _parseXmlColorAttributes(xml.attributes());
            }
            else if ("frustum-text" == xml.name()) {
                _sv.frustumTextColor = 
                    _parseXmlColorAttributes(xml.attributes());
            }
            else if ("import-transform-line" == xml.name()) {
                _sv.importTransformLineColor =
                    _parseXmlColorAttributes(xml.attributes());
            }
            else if ("import-transform-text" == xml.name()) {
                _sv.importTransformTextColor =
                    _parseXmlColorAttributes(xml.attributes());
            }
            else if ("plane-line" == xml.name()) {
                _sv.planeLineColor = 
                    _parseXmlColorAttributes(xml.attributes());
            }
            else if ("plane-text" == xml.name()) {
                _sv.planeTextColor = 
                    _parseXmlColorAttributes(xml.attributes());
            }
            else if ("sphere-line" == xml.name()) {
                _sv.sphereLineColor = 
                    _parseXmlColorAttributes(xml.attributes());
            }
            else if ("sphere-text" == xml.name()) {
                _sv.sphereTextColor = 
                    _parseXmlColorAttributes(xml.attributes());
            }
            else if ("hud-text" == xml.name()) {
                _sv.hudTextColor = 
                    _parseXmlColorAttributes(xml.attributes());
            }
            else if ("body-label-text" == xml.name()) {
                _sv.bodyLabelTextColor = 
                    _parseXmlColorAttributes(xml.attributes());
            }
        }
        xml.readNext();
    }
    return true;    // Success!
}


QColor
NsPreferences::_parseXmlColorAttributes(const QXmlStreamAttributes &attr)
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
NsPreferences::_parseXmlStringAttribute(const QXmlStreamAttributes &attr,
                                        const QString            &qualifiedName)
{
    QString str;
    if (attr.hasAttribute(qualifiedName)) {
        str = attr.value(qualifiedName).toString();
    }
    return str;
}

void
NsPreferences::_writeXmlColorElement(QXmlStreamWriter &xml, 
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
NsPreferences::_writeXmlMappedColorElement(QXmlStreamWriter &xml, 
                                           const QString    &qualifiedName, 
                                           const _ColorMap  &colors)
{
    _ColorMap::ConstIterator iter = colors.constBegin();
    const _ColorMap::ConstIterator iend = colors.constEnd();
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
NsPreferences::_writeXmlPathElement(QXmlStreamWriter &xml, 
                                    const QString    &qualifiedName, 
                                    const QString    &path)
{
    xml.writeStartElement(qualifiedName);
    xml.writeAttribute("path", path);
    xml.writeEndElement();
}

// -----------------------------------------------------------------------------

const QString
NsPreferences::_General::settingsGroup("General");

const QString
NsPreferences::_General::defaultStylePath("/clients/config/nstudio-pro.qss");

// -----------------------------------------------------------------------------

const QString 
NsPreferences::_GraphView::settingsGroup("GraphView");


const QString 
NsPreferences::_GraphView::defaultBackground = 
    QColor(72, 72, 72).name();

const QString 
NsPreferences::_GraphView::defaultLiveBackground = 
    QColor(226, 255, 226).name();

const QString
NsPreferences::_GraphView::defaultSelectionColor = 
    QColor(249, 255, 61).name();


const QString 
NsPreferences::_GraphView::defaultFeedLineColor = 
    QColor(0, 0, 0).name();

const QString 
NsPreferences::_GraphView::defaultFeedReplacedLineColor = 
    QColor(160, 160, 160).name();

const QString
NsPreferences::_GraphView::defaultFeedValidLineColor = 
    QColor(50, 205, 50).name();

const QString
NsPreferences::_GraphView::defaultFeedInvalidLineColor =
    QColor(255, 99, 71).name();


const QString 
NsPreferences::_GraphView::defaultPlugLineColor = 
    QColor(0, 0, 0).name();

const QString 
NsPreferences::_GraphView::defaultPlugTextColor = 
    QColor(0, 0, 0).name();

const QString
NsPreferences::_GraphView::defaultPlugBackground = 
    QColor(255, 255, 255).name();


const QString 
NsPreferences::_GraphView::defaultBodyLineColor = 
    QColor(0, 0, 0).name();

const QString 
NsPreferences::_GraphView::defaultBodyTextColor = 
    QColor(0, 0, 0).name();

const QString
NsPreferences::_GraphView::defaultBodyBackground = 
    QColor(255, 255, 255).name();


const QString 
NsPreferences::_GraphView::defaultOpLineColor = 
    QColor(0, 0, 0).name();

const QString 
NsPreferences::_GraphView::defaultOpTextColor = 
    QColor(0, 0, 0).name();

const QString 
NsPreferences::_GraphView::defaultOpBackground = 
    QColor(255, 255, 255).name();

const QString 
NsPreferences::_GraphView::defaultOpDisabledColor =
    QColor(160, 160, 164).name();


const QString 
NsPreferences::_GraphView::defaultOpStateDefaultColor = 
    QColor(0, 0, 0).name();

const QString 
NsPreferences::_GraphView::defaultOpCategoryDefaultColor = 
    QColor(255, 255, 255).name();

const QString
NsPreferences::_GraphView::defaultSignatureDefaultColor =
    QColor(0, 0, 0).name();


NsPreferences::_ColorMap
NsPreferences::_GraphView::defaultOpStateColors()
{
    _ColorMap m;
    m.insert("ACTIVE", QColor(7, 176, 0));
    m.insert("INACTIVE", QColor(160, 160, 160));
    m.insert("SUB_ACTIVE", QColor(205, 250, 157));
    return m;
}

NsPreferences::_ColorMap
NsPreferences::_GraphView::defaultOpCategoryColors()
{
    _ColorMap m;
    m.insert("Body", QColor(244, 201, 203));
    m.insert("Channel", QColor(207, 244, 234));
    m.insert("Convert", QColor(244, 201, 203));
    m.insert("Distance", QColor(210, 230, 255));
    m.insert("Dynamics", QColor(255, 222, 175));
    m.insert("Emit", QColor(225, 207, 229));
    m.insert("Field", QColor(210, 230, 255));
    m.insert("File", QColor(226, 213, 182));
    m.insert("Global", QColor(255, 255, 255));
    m.insert("Math", QColor(210, 230, 255));
    m.insert("Mesh", QColor(227, 255, 216));
    m.insert("Origin", QColor(255, 255, 217));
    m.insert("Particle", QColor(239, 255, 186));
    m.insert("Point", QColor(227, 255, 216));
    m.insert("Primitive", QColor(255, 255, 217));
    return m;
}

NsPreferences::_ColorMap
NsPreferences::_GraphView::defaultSignatureColors()
{
    _ColorMap m;
    m.insert("Body", QColor(195, 193, 153));
    m.insert("Camera", QColor(96, 199, 232));
    m.insert("Field", QColor(133, 217, 107));
    m.insert("Field-Gas", QColor(195, 193, 153));
    m.insert("Field-Liquid", QColor(96, 199, 232));
    m.insert("Field-Smoke", QColor(179, 17, 17));
    m.insert("FieldOp", QColor(133, 217, 107));
    m.insert("Mesh", QColor(195, 193, 153));
    m.insert("Particle", QColor(195, 193, 153));
    m.insert("Particle-Liquid", QColor(96, 199, 232));
    m.insert("Particle-Smoke", QColor(179, 17, 17));
    m.insert("Volume", QColor(133, 217, 107));
    return m;
}

// -----------------------------------------------------------------------------

const QString 
NsPreferences::_ScopeView::settingsGroup("ScopeView");


const QString 
NsPreferences::_ScopeView::defaultBackground = 
    QColor(176, 181, 184).name();

const QString
NsPreferences::_ScopeView::defaultSelectionColor = 
    QColor(249, 255, 61).name();

const QString
NsPreferences::_ScopeView::defaultConstructionGridColor = 
    QColor(127, 127, 127).name();


const QString
NsPreferences::_ScopeView::defaultAxisLineColor = 
    QColor(255, 255, 255).name();

const QString
NsPreferences::_ScopeView::defaultAxisTextColor = 
    QColor(0, 0, 0).name();

const QString
NsPreferences::_ScopeView::defaultBoxLineColor = 
    QColor(255, 255, 255).name();

const QString
NsPreferences::_ScopeView::defaultBoxTextColor = 
    QColor(0, 0, 0).name();

const QString
NsPreferences::_ScopeView::defaultClipBoxLineColor = 
    QColor(255, 255, 255).name();

const QString
NsPreferences::_ScopeView::defaultClipBoxTextColor = 
    QColor(0, 0, 0).name();

const QString
NsPreferences::_ScopeView::defaultFrustumLineColor = 
    QColor(255, 255, 255).name();

const QString
NsPreferences::_ScopeView::defaultFrustumTextColor = 
    QColor(0, 0, 0).name();

const QString
NsPreferences::_ScopeView::defaultImportTransformLineColor = 
    QColor(255, 255, 255).name();

const QString
NsPreferences::_ScopeView::defaultImportTransformTextColor = 
    QColor(0, 0, 0).name();

const QString
NsPreferences::_ScopeView::defaultPlaneLineColor = 
    QColor(255, 255, 255).name();

const QString
NsPreferences::_ScopeView::defaultPlaneTextColor = 
    QColor(0, 0, 0).name();

const QString
NsPreferences::_ScopeView::defaultSphereLineColor = 
    QColor(255, 255, 255).name();

const QString
NsPreferences::_ScopeView::defaultSphereTextColor = 
    QColor(0, 0, 0).name();


const QString
NsPreferences::_ScopeView::defaultHudTextColor = 
    QColor(223, 223, 185).name();

const QString
NsPreferences::_ScopeView::defaultBodyLabelTextColor = 
    QColor(255, 105, 180).name();

// -----------------------------------------------------------------------------
