// -----------------------------------------------------------------------------
//
// NsPreferences.h
//
// Naiad Studio preferences, header file.
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

#ifndef NS_PREFERENCES_H
#define NS_PREFERENCES_H

#include <QObject>
#include <QMap>
#include <QColor>

class QFile;
class QXmlStreamReader;
class QXmlStreamWriter;
class QXmlStreamAttributes;

// -----------------------------------------------------------------------------

class NsPreferences : public QObject
{
    Q_OBJECT

public:     // Singleton interface.

    static NsPreferences*
    instance();

    static void
    createInstance();

    static void
    destroyInstance();

private:

    static NsPreferences *_instance;

public:

    bool
    read(QIODevice *device);

    void 
    write(QIODevice *device) const;

    void
    reset();

public:     // General.

    QString
    generalStylePath() const
    { return qgetenv("NAIAD_PATH") + _generalStylePath; }

public:     // Graph View.

    // General.

    const QColor&
    graphViewBackground() const
    { return _graphViewBackground; }

    const QColor&
    graphViewLiveBackground() const
    { return _graphViewLiveBackground; }

    const QColor&
    graphViewSelectionColor() const
    { return _graphViewSelectionColor; }
    

    // Feeds.

    const QColor&
    graphViewFeedLineColor() const
    { return _graphViewFeedLineColor; }

    const QColor&
    graphViewFeedReplacedLineColor() const
    { return _graphViewFeedReplacedLineColor; }

    const QColor&
    graphViewFeedValidLineColor() const
    { return _graphViewFeedValidLineColor; }

    const QColor&
    graphViewFeedInvalidLineColor() const
    { return _graphViewFeedInvalidLineColor; }


    // Plugs.

    const QColor&
    graphViewPlugLineColor() const
    { return _graphViewPlugLineColor; }

    const QColor&
    graphViewPlugBackground() const
    { return _graphViewPlugBackground; }

    const QColor&
    graphViewPlugTextColor() const
    { return _graphViewPlugTextColor; }


    // Bodies.

    const QColor&
    graphViewBodyLineColor() const
    { return _graphViewBodyLineColor; }

    const QColor&
    graphViewBodyBackground() const
    { return _graphViewBodyBackground; }

    const QColor&
    graphViewBodyTextColor() const
    { return _graphViewBodyTextColor; }


    // Ops.

    const QColor&
    graphViewOpLineColor() const
    { return _graphViewOpLineColor; }

    const QColor&
    graphViewOpBackground() const
    { return _graphViewOpBackground; }

    const QColor&
    graphViewOpTextColor() const
    { return _graphViewOpTextColor; }

    const QColor&
    graphViewOpDisabledColor() const
    { return _graphViewOpDisabledColor; }

    // Op state.

    QColor
    graphViewOpStateColor(const QString &state) const
    {
        return _graphViewOpStateColors.value(
            state, _graphViewOpStateDefaultColor);
    }

    QColor
    graphViewOpStateDefaultColor() const
    { return _graphViewOpStateDefaultColor; }

    QStringList
    graphViewOpStateNames() const
    { return _graphViewOpStateColors.keys(); }

    // Op category.

    QColor
    graphViewOpCategoryColor(const QString &categoryName) const
    {
        return _graphViewOpCategoryColors.value(
            categoryName, _graphViewOpCategoryDefaultColor);
    }

    QColor
    graphViewOpCategoryDefaultColor() const
    { return _graphViewOpCategoryDefaultColor; }

    QStringList
    graphViewOpCategoryNames() const
    { return _graphViewOpCategoryColors.keys(); }


    // Misc.

    QColor
    graphViewSignatureColor(const QString &signatureName) const
    {
        return _graphViewSignatureColors.value(
            signatureName, _graphViewSignatureDefaultColor);
    }

    QColor
    graphViewSignatureDefaultColor() const
    { return _graphViewSignatureDefaultColor; }

    QStringList
    graphViewSignatureNames() const
    { return _graphViewSignatureColors.keys(); }

public:     // Scope View.

    const QColor&
    scopeViewBackground() const
    { return _scopeViewBackground; }

    const QColor& 
    scopeViewSelectionColor() const
    { return _scopeViewSelectionColor; }

    const QColor&
    scopeViewConstructionGridColor() const
    { return _scopeViewConstructionGridColor; }

    const QColor& 
    scopeViewAxisLineColor() const
    { return _scopeViewAxisLineColor; }

    const QColor& 
    scopeViewAxisTextColor() const
    { return _scopeViewAxisTextColor; }

    const QColor& 
    scopeViewBoxLineColor() const
    { return _scopeViewBoxLineColor; }

    const QColor& 
    scopeViewBoxTextColor() const
    { return _scopeViewBoxTextColor; }

    const QColor& 
    scopeViewClipBoxLineColor() const
    { return _scopeViewClipBoxLineColor; }

    const QColor& 
    scopeViewClipBoxTextColor() const
    { return _scopeViewClipBoxTextColor; }

    const QColor& 
    scopeViewFrustumLineColor() const
    { return _scopeViewFrustumLineColor; }

    const QColor& 
    scopeViewFrustumTextColor() const
    { return _scopeViewFrustumTextColor; }

    const QColor&
    scopeViewImportTransformLineColor() const
    { return _scopeViewImportTransformLineColor; }

    const QColor&
    scopeViewImportTransformTextColor() const
    { return _scopeViewImportTransformTextColor; }

    const QColor& 
    scopeViewPlaneLineColor() const
    { return _scopeViewPlaneLineColor; }

    const QColor& 
    scopeViewPlaneTextColor() const
    { return _scopeViewPlaneTextColor; }

    const QColor& 
    scopeViewSphereLineColor() const
    { return _scopeViewSphereLineColor; }

    const QColor& 
    scopeViewSphereTextColor() const
    { return _scopeViewSphereTextColor; }

    const QColor& 
    scopeViewHudTextColor() const
    { return _scopeViewHudTextColor; }

    const QColor& 
    scopeViewBodyLabelTextColor() const
    { return _scopeViewBodyLabelTextColor; }

public slots:   // General.

    void
    setGeneralStylePath(const QString &path)
    { _generalStylePath = path; emit changed(); }

public slots:   // Graph View.

    void
    setGraphViewBackground(const QColor &color)
    { _graphViewBackground = color; emit changed(); }

    void
    setGraphViewLiveBackground(const QColor &color)
    { _graphViewLiveBackground = color; emit changed(); }

    void
    setGraphViewSelectionColor(const QColor &color)
    { _graphViewSelectionColor = color; emit changed(); }

    void
    setGraphViewFeedLineColor(const QColor &color)
    { _graphViewFeedLineColor = color; emit changed(); }

    void
    setGraphViewFeedReplacedLineColor(const QColor &color)
    { _graphViewFeedReplacedLineColor = color; emit changed(); }

    void
    setGraphViewFeedValidLineColor(const QColor &color)
    { _graphViewFeedValidLineColor = color; emit changed(); }

    void
    setGraphViewFeedInvalidLineColor(const QColor &color)
    { _graphViewFeedInvalidLineColor = color; emit changed(); }

    void
    setGraphViewPlugLineColor(const QColor &color)
    { _graphViewPlugLineColor = color; emit changed(); }

    void
    setGraphViewPlugBackground(const QColor &color)
    { _graphViewPlugBackground = color; emit changed(); }

    void
    setGraphViewPlugTextColor(const QColor &color)
    { _graphViewPlugTextColor = color; emit changed(); }

    void
    setGraphViewBodyLineColor(const QColor &color)
    { _graphViewBodyLineColor = color; emit changed(); }

    void
    setGraphViewBodyBackground(const QColor &color)
    { _graphViewBodyBackground = color; emit changed(); }

    void
    setGraphViewBodyTextColor(const QColor &color)
    { _graphViewBodyTextColor = color; emit changed(); }

    void
    setGraphViewOpLineColor(const QColor &color)
    { _graphViewOpLineColor = color; emit changed(); }

    void
    setGraphViewOpTextColor(const QColor &color)
    { _graphViewOpTextColor = color; emit changed(); }

    void
    setGraphViewOpBackground(const QColor &color)
    { _graphViewOpBackground = color; emit changed(); }

    void
    setGraphViewOpDisabledColor(const QColor &color)
    { _graphViewOpDisabledColor = color; emit changed(); }

    // Op category.

    void
    setGraphViewOpCategoryColor(const QString &categoryName, 
                                const QColor  &color)
    { _graphViewOpCategoryColors[categoryName] = color; emit changed(); }

    void
    setGraphViewOpCategoryDefaultColor(const QColor &color)
    { _graphViewOpCategoryDefaultColor = color; emit changed(); }

    // Op state.

    void
    setGraphViewOpStateColor(const QString &stateName, 
                             const QColor  &color)
    { _graphViewOpStateColors[stateName] = color; emit changed(); }

    void
    setGraphViewOpStateDefaultColor(const QColor &color)
    { _graphViewOpStateDefaultColor = color; emit changed(); }

    // Signature.

    void
    setGraphViewSignatureColor(const QString &signatureName, 
                               const QColor  &color)
    { _graphViewSignatureColors[signatureName] = color; emit changed(); }

    void
    setGraphViewSignatureDefaultColor(const QColor &color)
    { _graphViewSignatureDefaultColor = color; emit changed(); }

public slots:   // Scope View.

    void
    setScopeViewBackground(const QColor &color)
    { _scopeViewBackground = color; emit changed(); }

    void
    setScopeViewSelectionColor(const QColor &color)
    { _scopeViewSelectionColor = color; emit changed(); }

    void
    setScopeViewConstructionGridColor(const QColor &color)
    { _scopeViewConstructionGridColor = color; emit changed(); }

    void
    setScopeViewAxisLineColor(const QColor &color)
    { _scopeViewAxisLineColor = color; emit changed(); }

    void
    setScopeViewAxisTextColor(const QColor &color)
    { _scopeViewAxisTextColor = color; emit changed(); }

    void
    setScopeViewBoxLineColor(const QColor &color)
    { _scopeViewBoxLineColor = color; emit changed(); }

    void
    setScopeViewBoxTextColor(const QColor &color)
    { _scopeViewBoxTextColor = color; emit changed(); }

    void
    setScopeViewClipBoxLineColor(const QColor &color)
    { _scopeViewClipBoxLineColor = color; emit changed(); }

    void
    setScopeViewClipBoxTextColor(const QColor &color)
    { _scopeViewClipBoxTextColor = color; emit changed(); }

    void
    setScopeViewFrustumLineColor(const QColor &color)
    { _scopeViewFrustumLineColor = color; emit changed(); }

    void
    setScopeViewFrustumTextColor(const QColor &color)
    { _scopeViewFrustumTextColor = color; emit changed(); }

    void
    setScopeViewImportTransformLineColor(const QColor &color)
    { _scopeViewImportTransformLineColor = color; emit changed(); }

    void
    setScopeViewImportTransformTextColor(const QColor &color)
    { _scopeViewImportTransformTextColor = color; emit changed(); }

    void
    setScopeViewPlaneLineColor(const QColor &color)
    { _scopeViewPlaneLineColor = color; emit changed(); }

    void
    setScopeViewPlaneTextColor(const QColor &color)
    { _scopeViewPlaneTextColor = color; emit changed(); }

    void
    setScopeViewSphereLineColor(const QColor &color)
    { _scopeViewSphereLineColor = color; emit changed(); }

    void
    setScopeViewSphereTextColor(const QColor &color)
    { _scopeViewSphereTextColor = color; emit changed(); }

    void
    setScopeViewHudTextColor(const QColor &color)
    { _scopeViewHudTextColor = color; emit changed(); }

    void
    setScopeViewBodyLabelTextColor(const QColor &color)
    { _scopeViewBodyLabelTextColor = color; emit changed(); }

signals:

    void
    changed();

private:

    bool
    _readGeneral(QXmlStreamReader &xml);

    bool
    _readGraphView(QXmlStreamReader &xml);

    bool
    _readScopeView(QXmlStreamReader &xml);

    static QColor
    _parseColorAttributes(const QXmlStreamAttributes &attr);

    static QString 
    _parseStringAttribute(const QXmlStreamAttributes &attr,
                          const QString              &qualifiedName);

    static void
    _writeColorElement(QXmlStreamWriter &xml, 
                       const QString    &qualifiedName, 
                       const QColor     &color);

    static void
    _writeMappedColorElement(QXmlStreamWriter           &xml, 
                             const QString              &qualifiedName, 
                             const QMap<QString,QColor> &colors);

    static void
    _writePathElement(QXmlStreamWriter &xml, 
                      const QString    &qualifiedName, 
                      const QString    &color);

private:    // Member variables.

    QString _generalStylePath;

    typedef QMap<QString, QColor> _OpCategoryColorMapType;
    typedef QMap<QString, QColor> _OpStateColorMapType;
    typedef QMap<QString, QColor> _SignatureColorMapType;

    QColor _graphViewBackground;
    QColor _graphViewLiveBackground;
    QColor _graphViewSelectionColor;

    QColor _graphViewFeedLineColor;
    QColor _graphViewFeedReplacedLineColor;
    QColor _graphViewFeedValidLineColor;
    QColor _graphViewFeedInvalidLineColor;


    QColor _graphViewPlugLineColor; 
    QColor _graphViewPlugTextColor; 
    QColor _graphViewPlugBackground; 

    QColor _graphViewBodyLineColor; 
    QColor _graphViewBodyTextColor; 
    QColor _graphViewBodyBackground; 

    QColor                  _graphViewOpLineColor;
    QColor                  _graphViewOpTextColor;
    QColor                  _graphViewOpBackground;
    QColor                  _graphViewOpDisabledColor;
    _OpStateColorMapType    _graphViewOpStateColors;
    QColor                  _graphViewOpStateDefaultColor;
    _OpCategoryColorMapType _graphViewOpCategoryColors;
    QColor                  _graphViewOpCategoryDefaultColor;
    
    _SignatureColorMapType  _graphViewSignatureColors;
    QColor                  _graphViewSignatureDefaultColor;

    QColor _scopeViewBackground;
    QColor _scopeViewSelectionColor;
    QColor _scopeViewConstructionGridColor;

    QColor _scopeViewAxisLineColor;
    QColor _scopeViewAxisTextColor;
    QColor _scopeViewBoxLineColor;
    QColor _scopeViewBoxTextColor;
    QColor _scopeViewClipBoxLineColor;
    QColor _scopeViewClipBoxTextColor;
    QColor _scopeViewFrustumLineColor;
    QColor _scopeViewFrustumTextColor;
    QColor _scopeViewImportTransformLineColor;
    QColor _scopeViewImportTransformTextColor;
    QColor _scopeViewPlaneLineColor;
    QColor _scopeViewPlaneTextColor;
    QColor _scopeViewSphereLineColor;
    QColor _scopeViewSphereTextColor;
    
    QColor _scopeViewHudTextColor;
    QColor _scopeViewBodyLabelTextColor;

private:

    explicit
    NsPreferences (QObject *parent = 0);

    //! DTOR.
    ~NsPreferences () 
    {}
};

// -----------------------------------------------------------------------------

#endif  // NS_PREFERENCES_H
