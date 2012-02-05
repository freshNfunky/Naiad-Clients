// -----------------------------------------------------------------------------
//
// NsPreferencesDialog.h
//
// Naiad Studio preferences dialog, header file.
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

#ifndef NS_PREFERENCES_DIALOG_H
#define NS_PREFERENCES_DIALOG_H

#include <QMainWindow>

#include <QPushButton>  // TMP!!
#include <QColorDialog>

class NsPreferences;
class QAction;
class QWidget;

// -----------------------------------------------------------------------------

// NsPreferencesDialog
// -------------------
//! The NsPreferencesDialog class, representing the dialog in which preferences
//! are set.

class NsPreferencesDialog : public QMainWindow
{
    Q_OBJECT

public:

    explicit
    NsPreferencesDialog(NsPreferences &prefs, QWidget *parent = 0);

    //! DTOR.
    virtual
    ~NsPreferencesDialog()
    {}

protected slots:

    void
    onReset()
    { _reset(); }

    void
    onImport()
    { _import(); }

    void
    onExport()
    { _export(); }

private:

    void
    _createActions();

    void
    _createMenus();

    void
    _createStatusBar();

    QWidget*
    _createGeneralPage();

    QWidget*
    _createGraphViewPage();

    QWidget*
    _createScopeViewPage();

    void
    _createCentralWidget();

    void
    _rebuild();

private:

    void
    _reset();

    void
    _load();

    void
    _import();

    void
    _export();

    void
    _save();

    void
    _save(const QString &fileName);

    void
    _saveAs();

    void
    _setWindowTitle();

private:

    static QString
    _getImportFileName(QWidget       *parent,
                       const QString &caption, 
                       const QString &path);

    static QString
    _getExportFileName(QWidget       *parent, 
                       const QString &caption, 
                       const QString &path);

    static QString
    _getBackupFileName(const QString &fileName);

private:    // Member variables.

    NsPreferences *_preferences;

    QAction *_resetAction;
    QAction *_importAction;
    QAction *_exportAction;
    QAction *_closeAction;
};

// -----------------------------------------------------------------------------

class NsColorButton : public QPushButton
{
public:

    Q_OBJECT

public:

    //! CTOR.
    explicit
    NsColorButton(const QColor &color, QWidget *parent = 0)
        : QPushButton(parent)
        , _color(color)
    {
        _setStyle(_color);
        connect(this, SIGNAL(clicked()), SLOT(onClicked()));
    }

    //! DTOR.
    virtual
    ~NsColorButton()
    {}

    const QColor&
    color() const
    { return _color; }

public slots:

    void
    setColor(const QColor &color)
    { _setColor(color); }

protected slots:

    void
    onClicked()
    { 
        const QColor color = QColorDialog::getColor(_color/*, this*/);
        if (color.isValid()) {
            _setColor(color); 
        }
    }

signals:

    void
    colorChanged(const QColor &newColor);

private:

    void
    _setColor(const QColor &color)
    {
        if (color != _color) {
            _color = color;
            _setStyle(_color);
            emit colorChanged(_color);
        }
    }

    void
    _setStyle(const QColor &color)
    {
        setStyleSheet(
            "background-color: " + color.name() + ";"
            "border-style solid;"
            "border-width: 1px;"
            "border-color: #000000;"
            "border-radius: 0px;"
            "min-width: 0px;"
            "margin: 0px;"
            "padding: 0px;");
    }

private:    // Member variables.

    QColor _color;
};

// -----------------------------------------------------------------------------

class NsNamedColorButton : public NsColorButton
{
public:

    Q_OBJECT

public:

    //! CTOR.
    explicit
    NsNamedColorButton(const QString &name, 
                       const QColor  &color, 
                       QWidget       *parent = 0)
        : NsColorButton(color, parent)
        , _name(name)
    {
        connect(this, 
                SIGNAL(colorChanged(QColor)), 
                SLOT(onColorChanged(QColor)));
    }

    //! DTOR.
    virtual
    ~NsNamedColorButton()
    {}

    const QString&
    name() const
    { return _name; }

protected slots:

    void
    onColorChanged(const QColor &c)
    { 
        Q_UNUSED(c);
        emit colorChanged(_name, color());
    }

signals:

    void
    colorChanged(const QString &name, const QColor &newColor);

private:    // Member variables.

    QString _name;
};

// -----------------------------------------------------------------------------

#endif  // NS_PREFERENCES_DIALOG_H
