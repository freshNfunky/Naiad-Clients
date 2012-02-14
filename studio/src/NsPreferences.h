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
class QSettings;

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
    readXml(QIODevice *device);

    void 
    writeXml(QIODevice *device) const;

    //! Reset to default values.
    void
    reset()
    {
        _gen.reset();
        _gv.reset();
        _sv.reset();
        emit changed();
    }

public:     // General.

    QString
    generalStylePath() const
    { return qgetenv("NAIAD_PATH") + _gen.stylePath; }

public:     // Graph View.

    // General.

    const QColor&
    graphViewBackground() const
    { return _gv.background; }

    const QColor&
    graphViewLiveBackground() const
    { return _gv.liveBackground; }

    const QColor&
    graphViewSelectionColor() const
    { return _gv.selectionColor; }
    

    // Feeds.

    const QColor&
    graphViewFeedLineColor() const
    { return _gv.feedLineColor; }

    const QColor&
    graphViewFeedReplacedLineColor() const
    { return _gv.feedReplacedLineColor; }

    const QColor&
    graphViewFeedValidLineColor() const
    { return _gv.feedValidLineColor; }

    const QColor&
    graphViewFeedInvalidLineColor() const
    { return _gv.feedInvalidLineColor; }


    // Plugs.

    const QColor&
    graphViewPlugLineColor() const
    { return _gv.plugLineColor; }

    const QColor&
    graphViewPlugBackground() const
    { return _gv.plugBackground; }

    const QColor&
    graphViewPlugTextColor() const
    { return _gv.plugTextColor; }


    // Bodies.

    const QColor&
    graphViewBodyLineColor() const
    { return _gv.bodyLineColor; }

    const QColor&
    graphViewBodyBackground() const
    { return _gv.bodyBackground; }

    const QColor&
    graphViewBodyTextColor() const
    { return _gv.bodyTextColor; }


    // Ops.

    const QColor&
    graphViewOpLineColor() const
    { return _gv.opLineColor; }

    const QColor&
    graphViewOpBackground() const
    { return _gv.opBackground; }

    const QColor&
    graphViewOpTextColor() const
    { return _gv.opTextColor; }

    const QColor&
    graphViewOpDisabledColor() const
    { return _gv.opDisabledColor; }

    // Op state.

    QColor
    graphViewOpStateColor(const QString &state) const
    { return _gv.opStateColors.value(state, _gv.opStateDefaultColor); }

    const QColor&
    graphViewOpStateDefaultColor() const
    { return _gv.opStateDefaultColor; }

    QStringList
    graphViewOpStateNames() const
    { return _gv.opStateColors.keys(); }

    // Op category.

    QColor
    graphViewOpCategoryColor(const QString &catName) const
    { return _gv.opCategoryColors.value(catName, _gv.opCategoryDefaultColor); }

    const QColor&
    graphViewOpCategoryDefaultColor() const
    { return _gv.opCategoryDefaultColor; }

    QStringList
    graphViewOpCategoryNames() const
    { return _gv.opCategoryColors.keys(); }


    // Misc.

    QColor
    graphViewSignatureColor(const QString &sigName) const
    { return _gv.signatureColors.value(sigName, _gv.signatureDefaultColor); }

    const QColor&
    graphViewSignatureDefaultColor() const
    { return _gv.signatureDefaultColor; }

    QStringList
    graphViewSignatureNames() const
    { return _gv.signatureColors.keys(); }

public:     // Scope View.

    const QColor&
    scopeViewBackground() const
    { return _sv.background; }

    const QColor& 
    scopeViewSelectionColor() const
    { return _sv.selectionColor; }

    const QColor&
    scopeViewConstructionGridColor() const
    { return _sv.constructionGridColor; }

    const QColor& 
    scopeViewAxisLineColor() const
    { return _sv.axisLineColor; }

    const QColor& 
    scopeViewAxisTextColor() const
    { return _sv.axisTextColor; }

    const QColor& 
    scopeViewBoxLineColor() const
    { return _sv.boxLineColor; }

    const QColor& 
    scopeViewBoxTextColor() const
    { return _sv.boxTextColor; }

    const QColor& 
    scopeViewClipBoxLineColor() const
    { return _sv.clipBoxLineColor; }

    const QColor& 
    scopeViewClipBoxTextColor() const
    { return _sv.clipBoxTextColor; }

    const QColor& 
    scopeViewFrustumLineColor() const
    { return _sv.frustumLineColor; }

    const QColor& 
    scopeViewFrustumTextColor() const
    { return _sv.frustumTextColor; }

    const QColor&
    scopeViewImportTransformLineColor() const
    { return _sv.importTransformLineColor; }

    const QColor&
    scopeViewImportTransformTextColor() const
    { return _sv.importTransformTextColor; }

    const QColor& 
    scopeViewPlaneLineColor() const
    { return _sv.planeLineColor; }

    const QColor& 
    scopeViewPlaneTextColor() const
    { return _sv.planeTextColor; }

    const QColor& 
    scopeViewSphereLineColor() const
    { return _sv.sphereLineColor; }

    const QColor& 
    scopeViewSphereTextColor() const
    { return _sv.sphereTextColor; }

    const QColor& 
    scopeViewHudTextColor() const
    { return _sv.hudTextColor; }

    const QColor& 
    scopeViewBodyLabelTextColor() const
    { return _sv.bodyLabelTextColor; }

public slots:   // General.

    void
    setGeneralStylePath(const QString &path)
    { _gen.stylePath = path; emit changed(); }

public slots:   // Graph View.

    void
    setGraphViewBackground(const QColor &color)
    { _gv.background = color; emit changed(); }

    void
    setGraphViewLiveBackground(const QColor &color)
    { _gv.liveBackground = color; emit changed(); }

    void
    setGraphViewSelectionColor(const QColor &color)
    { _gv.selectionColor = color; emit changed(); }

    void
    setGraphViewFeedLineColor(const QColor &color)
    { _gv.feedLineColor = color; emit changed(); }

    void
    setGraphViewFeedReplacedLineColor(const QColor &color)
    { _gv.feedReplacedLineColor = color; emit changed(); }

    void
    setGraphViewFeedValidLineColor(const QColor &color)
    { _gv.feedValidLineColor = color; emit changed(); }

    void
    setGraphViewFeedInvalidLineColor(const QColor &color)
    { _gv.feedInvalidLineColor = color; emit changed(); }

    void
    setGraphViewPlugLineColor(const QColor &color)
    { _gv.plugLineColor = color; emit changed(); }

    void
    setGraphViewPlugBackground(const QColor &color)
    { _gv.plugBackground = color; emit changed(); }

    void
    setGraphViewPlugTextColor(const QColor &color)
    { _gv.plugTextColor = color; emit changed(); }

    void
    setGraphViewBodyLineColor(const QColor &color)
    { _gv.bodyLineColor = color; emit changed(); }

    void
    setGraphViewBodyBackground(const QColor &color)
    { _gv.bodyBackground = color; emit changed(); }

    void
    setGraphViewBodyTextColor(const QColor &color)
    { _gv.bodyTextColor = color; emit changed(); }

    void
    setGraphViewOpLineColor(const QColor &color)
    { _gv.opLineColor = color; emit changed(); }

    void
    setGraphViewOpTextColor(const QColor &color)
    { _gv.opTextColor = color; emit changed(); }

    void
    setGraphViewOpBackground(const QColor &color)
    { _gv.opBackground = color; emit changed(); }

    void
    setGraphViewOpDisabledColor(const QColor &color)
    { _gv.opDisabledColor = color; emit changed(); }

    // Op category.

    void
    setGraphViewOpCategoryColor(const QString &categoryName, 
                                const QColor  &color)
    { _gv.opCategoryColors[categoryName] = color; emit changed(); }

    void
    setGraphViewOpCategoryDefaultColor(const QColor &color)
    { _gv.opCategoryDefaultColor = color; emit changed(); }

    // Op state.

    void
    setGraphViewOpStateColor(const QString &stateName, 
                             const QColor  &color)
    { _gv.opStateColors[stateName] = color; emit changed(); }

    void
    setGraphViewOpStateDefaultColor(const QColor &color)
    { _gv.opStateDefaultColor = color; emit changed(); }

    // Signature.

    void
    setGraphViewSignatureColor(const QString &signatureName, 
                               const QColor  &color)
    { _gv.signatureColors[signatureName] = color; emit changed(); }

    void
    setGraphViewSignatureDefaultColor(const QColor &color)
    { _gv.signatureDefaultColor = color; emit changed(); }

public slots:   // Scope View.

    void
    setScopeViewBackground(const QColor &color)
    { _sv.background = color; emit changed(); }

    void
    setScopeViewSelectionColor(const QColor &color)
    { _sv.selectionColor = color; emit changed(); }

    void
    setScopeViewConstructionGridColor(const QColor &color)
    { _sv.constructionGridColor = color; emit changed(); }

    void
    setScopeViewAxisLineColor(const QColor &color)
    { _sv.axisLineColor = color; emit changed(); }

    void
    setScopeViewAxisTextColor(const QColor &color)
    { _sv.axisTextColor = color; emit changed(); }

    void
    setScopeViewBoxLineColor(const QColor &color)
    { _sv.boxLineColor = color; emit changed(); }

    void
    setScopeViewBoxTextColor(const QColor &color)
    { _sv.boxTextColor = color; emit changed(); }

    void
    setScopeViewClipBoxLineColor(const QColor &color)
    { _sv.clipBoxLineColor = color; emit changed(); }

    void
    setScopeViewClipBoxTextColor(const QColor &color)
    { _sv.clipBoxTextColor = color; emit changed(); }

    void
    setScopeViewFrustumLineColor(const QColor &color)
    { _sv.frustumLineColor = color; emit changed(); }

    void
    setScopeViewFrustumTextColor(const QColor &color)
    { _sv.frustumTextColor = color; emit changed(); }

    void
    setScopeViewImportTransformLineColor(const QColor &color)
    { _sv.importTransformLineColor = color; emit changed(); }

    void
    setScopeViewImportTransformTextColor(const QColor &color)
    { _sv.importTransformTextColor = color; emit changed(); }

    void
    setScopeViewPlaneLineColor(const QColor &color)
    { _sv.planeLineColor = color; emit changed(); }

    void
    setScopeViewPlaneTextColor(const QColor &color)
    { _sv.planeTextColor = color; emit changed(); }

    void
    setScopeViewSphereLineColor(const QColor &color)
    { _sv.sphereLineColor = color; emit changed(); }

    void
    setScopeViewSphereTextColor(const QColor &color)
    { _sv.sphereTextColor = color; emit changed(); }

    void
    setScopeViewHudTextColor(const QColor &color)
    { _sv.hudTextColor = color; emit changed(); }

    void
    setScopeViewBodyLabelTextColor(const QColor &color)
    { _sv.bodyLabelTextColor = color; emit changed(); }

signals:

    void
    changed();

protected slots:

    void
    onAboutToQuit()
    { _writeSettings(); }

private:    // Settings.

    typedef QMap<QString, QColor> _ColorMap;

    static const QString _settingsGroup;

    void
    _readSettings();

    static void
    _readColorSetting(const QSettings &s,  
                      const QString   &k, 
                      QColor          &c,
                      const QString   &d);

    static void 
    _readMappedColorSetting(const QSettings &s,  
                            _ColorMap       &m,
                            const _ColorMap &d);

    void
    _writeSettings() const;

    static void
    _writeColorSetting(QSettings &s, const QString &k, const QColor &c);

    static void
    _writeMappedColorSetting(QSettings &s, const _ColorMap &map);

private:    // XML

    bool
    _readXmlGeneral(QXmlStreamReader &xml);

    bool
    _readXmlGraphView(QXmlStreamReader &xml);

    bool
    _readXmlScopeView(QXmlStreamReader &xml);

    static QColor
    _parseXmlColorAttributes(const QXmlStreamAttributes &attr);

    static QString 
    _parseXmlStringAttribute(const QXmlStreamAttributes &attr,
                             const QString              &qualifiedName);

    static void
    _writeXmlColorElement(QXmlStreamWriter &xml, 
                          const QString    &qualifiedName, 
                          const QColor     &color);

    static void
    _writeXmlMappedColorElement(QXmlStreamWriter &xml, 
                                const QString    &qualifiedName, 
                                const _ColorMap  &colors);

    static void
    _writeXmlPathElement(QXmlStreamWriter &xml, 
                         const QString    &qualifiedName, 
                         const QString    &path);

private:

    //! DOCS
    struct _General
    {
    public:

        static const QString settingsGroup;

    public:     // Default values.

        static const QString defaultStylePath;

    public:

        //! CTOR.
        _General()
            : stylePath(defaultStylePath)
        {}

    public:     // Values.
        
        QString stylePath;

    public:

        void
        reset()
        {
            stylePath = defaultStylePath;
        }
    };

private:

    //! DOCS
    struct _GraphView
    {
    public:

        static const QString settingsGroup;

    public:     // Default values.

        static const QString defaultBackground;
        static const QString defaultLiveBackground;
        static const QString defaultSelectionColor;

        static const QString defaultFeedLineColor;
        static const QString defaultFeedReplacedLineColor;
        static const QString defaultFeedValidLineColor;
        static const QString defaultFeedInvalidLineColor;

        static const QString defaultPlugLineColor; 
        static const QString defaultPlugTextColor; 
        static const QString defaultPlugBackground; 

        static const QString defaultBodyLineColor; 
        static const QString defaultBodyTextColor; 
        static const QString defaultBodyBackground; 

        static const QString defaultOpLineColor;
        static const QString defaultOpTextColor;
        static const QString defaultOpBackground;
        static const QString defaultOpDisabledColor;

        static const QString  defaultOpStateDefaultColor;
        static const QString  defaultOpCategoryDefaultColor;
        static const QString  defaultSignatureDefaultColor;
        
        static _ColorMap 
        defaultOpStateColors();

        static _ColorMap 
        defaultOpCategoryColors();

        static _ColorMap 
        defaultSignatureColors();

    public: 

        //! CTOR.
        _GraphView()
            : background(defaultBackground)
            , liveBackground(defaultLiveBackground)
            , selectionColor(defaultSelectionColor)
            , feedLineColor(defaultFeedLineColor)
            , feedReplacedLineColor(defaultFeedReplacedLineColor)
            , feedValidLineColor(defaultFeedValidLineColor)
            , feedInvalidLineColor(defaultFeedInvalidLineColor)
            , plugLineColor(defaultPlugLineColor)
            , plugTextColor(defaultPlugTextColor)
            , plugBackground(defaultPlugBackground)
            , bodyLineColor(defaultBodyLineColor)
            , bodyTextColor(defaultBodyTextColor)
            , bodyBackground(defaultBodyBackground)
            , opLineColor(defaultOpLineColor)
            , opTextColor(defaultOpTextColor)
            , opBackground(defaultOpBackground)
            , opDisabledColor(defaultOpDisabledColor)
            , opStateDefaultColor(defaultOpStateDefaultColor)
            , opCategoryDefaultColor(defaultOpCategoryDefaultColor)
            , signatureDefaultColor(defaultSignatureDefaultColor)
            , opStateColors(defaultOpStateColors())
            , opCategoryColors(defaultOpCategoryColors())
            , signatureColors(defaultSignatureColors())
        {}

    public:     // Values.

        QColor background;
        QColor liveBackground;
        QColor selectionColor;

        QColor feedLineColor;
        QColor feedReplacedLineColor;
        QColor feedValidLineColor;
        QColor feedInvalidLineColor;

        QColor plugLineColor; 
        QColor plugTextColor; 
        QColor plugBackground; 

        QColor bodyLineColor; 
        QColor bodyTextColor; 
        QColor bodyBackground; 

        QColor opLineColor;
        QColor opTextColor;
        QColor opBackground;
        QColor opDisabledColor;

        QColor opStateDefaultColor;
        QColor opCategoryDefaultColor;
        QColor signatureDefaultColor;

        _ColorMap opStateColors;
        _ColorMap opCategoryColors;
        _ColorMap signatureColors;

    public:

        void
        reset()
        {
            background             = defaultBackground;
            liveBackground         = defaultLiveBackground;
            selectionColor         = defaultSelectionColor;
            feedLineColor          = defaultFeedLineColor;
            feedReplacedLineColor  = defaultFeedReplacedLineColor;
            feedValidLineColor     = defaultFeedValidLineColor;
            feedInvalidLineColor   = defaultFeedInvalidLineColor;
            plugLineColor          = defaultPlugLineColor;
            plugTextColor          = defaultPlugTextColor;
            plugBackground         = defaultPlugBackground;
            bodyLineColor          = defaultBodyLineColor;
            bodyTextColor          = defaultBodyTextColor;
            bodyBackground         = defaultBodyBackground;
            opLineColor            = defaultOpLineColor;
            opTextColor            = defaultOpTextColor;
            opBackground           = defaultOpBackground;
            opDisabledColor        = defaultOpDisabledColor;
            opStateDefaultColor    = defaultOpStateDefaultColor;
            opCategoryDefaultColor = defaultOpCategoryDefaultColor;
            signatureDefaultColor  = defaultSignatureDefaultColor;
            opStateColors          = defaultOpStateColors();
            opCategoryColors       = defaultOpCategoryColors();
            signatureColors        = defaultSignatureColors();
        }
    };

private:

    //! DOCS
    struct _ScopeView
    {
    public:

        static const QString settingsGroup;

    public:     // Default values.

        static const QString defaultBackground;
        static const QString defaultSelectionColor;
        static const QString defaultConstructionGridColor;

        static const QString defaultAxisLineColor;
        static const QString defaultAxisTextColor;
        static const QString defaultBoxLineColor;
        static const QString defaultBoxTextColor;
        static const QString defaultClipBoxLineColor;
        static const QString defaultClipBoxTextColor;
        static const QString defaultFrustumLineColor;
        static const QString defaultFrustumTextColor;
        static const QString defaultImportTransformLineColor;
        static const QString defaultImportTransformTextColor;
        static const QString defaultPlaneLineColor;
        static const QString defaultPlaneTextColor;
        static const QString defaultSphereLineColor;
        static const QString defaultSphereTextColor;

        static const QString defaultHudTextColor;
        static const QString defaultBodyLabelTextColor;

    public:

        //! CTOR.
        _ScopeView()
            : background(defaultBackground)
            , selectionColor(defaultSelectionColor)
            , constructionGridColor(defaultConstructionGridColor)
            , axisLineColor(defaultAxisLineColor)
            , axisTextColor(defaultAxisTextColor)
            , boxLineColor(defaultBoxLineColor)
            , boxTextColor(defaultBoxTextColor)
            , clipBoxLineColor(defaultClipBoxLineColor)
            , clipBoxTextColor(defaultClipBoxTextColor)
            , frustumLineColor(defaultFrustumLineColor)
            , frustumTextColor(defaultFrustumTextColor)
            , importTransformLineColor(defaultImportTransformLineColor)
            , importTransformTextColor(defaultImportTransformTextColor)
            , planeLineColor(defaultPlaneLineColor)
            , planeTextColor(defaultPlaneTextColor)
            , sphereLineColor(defaultSphereLineColor)
            , sphereTextColor(defaultSphereTextColor)
            , hudTextColor(defaultHudTextColor)
            , bodyLabelTextColor(defaultBodyLabelTextColor)
        {}

    public:     // Values.

        QColor background;
        QColor selectionColor;
        QColor constructionGridColor;

        QColor axisLineColor;
        QColor axisTextColor;
        QColor boxLineColor;
        QColor boxTextColor;
        QColor clipBoxLineColor;
        QColor clipBoxTextColor;
        QColor frustumLineColor;
        QColor frustumTextColor;
        QColor importTransformLineColor;
        QColor importTransformTextColor;
        QColor planeLineColor;
        QColor planeTextColor;
        QColor sphereLineColor;
        QColor sphereTextColor;
    
        QColor hudTextColor;
        QColor bodyLabelTextColor;

    public:

        void
        reset()
        {
            background               = defaultBackground;
            selectionColor           = defaultSelectionColor;
            constructionGridColor    = defaultConstructionGridColor;
            axisLineColor            = defaultAxisLineColor;
            axisTextColor            = defaultAxisTextColor;
            boxLineColor             = defaultBoxLineColor;
            boxTextColor             = defaultBoxTextColor;
            clipBoxLineColor         = defaultClipBoxLineColor;
            clipBoxTextColor         = defaultClipBoxTextColor;
            frustumLineColor         = defaultFrustumLineColor;
            frustumTextColor         = defaultFrustumTextColor;
            importTransformLineColor = defaultImportTransformLineColor;
            importTransformTextColor = defaultImportTransformTextColor;
            planeLineColor           = defaultPlaneLineColor;
            planeTextColor           = defaultPlaneTextColor;
            sphereLineColor          = defaultSphereLineColor;
            sphereTextColor          = defaultSphereTextColor;
            hudTextColor             = defaultHudTextColor;
            bodyLabelTextColor       = defaultBodyLabelTextColor;
        }
    };

private:    // Member variables.

    _General   _gen;
    _GraphView _gv;
    _ScopeView _sv;

private:

    //! CTOR.
    explicit
    NsPreferences (QObject *parent = 0)
        : QObject(parent)
        , _gen()    // Default General prefs.
        , _gv()     // Default Graph View prefs.
        , _sv()     // Default Scope View prefs.
    { _readSettings(); }

    //! DTOR.
    virtual
    ~NsPreferences() 
    {}
};

// -----------------------------------------------------------------------------

#endif  // NS_PREFERENCES_H
