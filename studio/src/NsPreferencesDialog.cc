// -----------------------------------------------------------------------------
//
// NsPreferencesDialog.cc
//
// Naiad Studio preferences dialog, source file.
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

#include "NsPreferencesDialog.h"
#include "NsPreferences.h"
#include "NsPath.h"
#include "NsMessageWidget.h"
#include <QApplication>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QTabWidget>
#include <QScrollArea>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QColorDialog>
#include <QPushButton>
#include <QLineEdit>

// -----------------------------------------------------------------------------

// NsPreferencesDialog
// -------------------
//! CTOR.

NsPreferencesDialog::NsPreferencesDialog(NsPreferences &preferences, 
                                         QWidget       *parent)
    : QMainWindow(parent, Qt::Dialog)
    , _preferences(&preferences)
{
    setWindowTitle("Naiad Studio Preferences");
    _createActions();
    _createMenus();
    _rebuild();
    resize(640, 480);
}

// -----------------------------------------------------------------------------

// _createActions
// --------------
//! DOCS

void
NsPreferencesDialog::_createActions()
{
    _resetAction = new QAction(tr("Reset"), this); // Child.
    _resetAction->setStatusTip(tr("Reset to default preferences."));
    connect(_resetAction, SIGNAL(triggered()), SLOT(onReset()));

    _importAction = new QAction(tr("Import..."), this); // Child.
    _importAction->setShortcut(QKeySequence::Open);
    _importAction->setStatusTip(tr("Import preferences from disk."));
    connect(_importAction, SIGNAL(triggered()), SLOT(onImport()));

    _exportAction = new QAction(tr("Export..."), this); // Child.
    _exportAction->setShortcut(QKeySequence::SaveAs);
    _exportAction->setStatusTip(tr("Export preferences to disk."));
    connect(_exportAction, SIGNAL(triggered()), SLOT(onExport()));

    _closeAction = new QAction(tr("Close"), this); // Child.
    _closeAction->setShortcuts(QKeySequence::Close);
    _closeAction->setStatusTip(tr("Close preferences dialog."));
    connect(_closeAction, SIGNAL(triggered()), SLOT(close()));
}


// _createMenus
// ------------
//! Assumes actions were created already!

void
NsPreferencesDialog::_createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(_resetAction);
    fileMenu->addAction(_importAction);
    fileMenu->addAction(_exportAction);
    fileMenu->addSeparator();
    fileMenu->addAction(_closeAction);
}


QWidget*
NsPreferencesDialog::_createGeneralPage()
{
    QWidget *page = new QWidget(this);   // Child.

    // Create layout.
    
    const Qt::Alignment headerAlignment = Qt::AlignLeft | Qt::AlignVCenter;
    const Qt::Alignment labelAlignment = Qt::AlignRight | Qt::AlignVCenter;
    const Qt::Alignment buttonAlignment = Qt::AlignRight | Qt::AlignVCenter;
    const QFont headerFont(QFont("sansserif", 12, QFont::Bold));
    QGridLayout *grid = new QGridLayout;
    grid->setHorizontalSpacing(40);
    grid->setVerticalSpacing(20);

    {
    int row = 0;
    QGridLayout *layout = new QGridLayout;
    QLabel *headerLabel = new QLabel(tr("General"));
    headerLabel->setFont(headerFont);
    layout->addWidget(headerLabel, 0, 0, 1, 3, headerAlignment);
    ++row;

    QLabel *stylePathTextLabel = new QLabel(tr("Style Path"));
    QLineEdit *stylePathLineEdit =
        new QLineEdit(_preferences->generalStylePath());
    stylePathLineEdit->setEnabled(false);
//    connect(bgColorButton, SIGNAL(colorChanged(QColor)),
//            _preferences,  SLOT(setGraphViewBackground(QColor)));
    layout->addWidget(stylePathTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(stylePathLineEdit, row, 2, buttonAlignment);
    ++row;

    layout->addItem(new QSpacerItem(0, 20), row, 0, 1, 3);
    ++row;

    grid->addLayout(layout, 0, 0, 1, 1, Qt::AlignTop);
    }

    page->setLayout(grid);
    return page;
}

QWidget*
NsPreferencesDialog::_createGraphViewPage()
{
    QWidget *page = new QWidget(this);   // Child.

    // Create layout.

    const Qt::Alignment headerAlignment = Qt::AlignLeft | Qt::AlignVCenter;
    const Qt::Alignment labelAlignment = Qt::AlignRight | Qt::AlignVCenter;
    const Qt::Alignment buttonAlignment = Qt::AlignRight | Qt::AlignVCenter;
    const QFont headerFont(QFont("sansserif", 12, QFont::Bold));
    QGridLayout *grid = new QGridLayout;
    grid->setHorizontalSpacing(40);
    grid->setVerticalSpacing(20);

    {
    int row = 0;
    QGridLayout *layout = new QGridLayout;
    QLabel *headerLabel = new QLabel(tr("General"));
    headerLabel->setFont(headerFont); 
    layout->addWidget(headerLabel, 0, 0, 1, 3, headerAlignment);
    ++row;

    QLabel *bgTextLabel = new QLabel(tr("Background"));
    NsColorButton *bgColorButton = 
        new NsColorButton(_preferences->graphViewBackground());
    connect(bgColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,  SLOT(setGraphViewBackground(QColor)));
    layout->addWidget(bgTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(bgColorButton, row, 2, buttonAlignment);
    ++row;

    QLabel *liveBgTextLabel = new QLabel(tr("Live Background"));
    NsColorButton *liveBgColorButton = 
        new NsColorButton(_preferences->graphViewLiveBackground());
    connect(liveBgColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,      SLOT(setGraphViewLiveBackground(QColor)));
    layout->addWidget(liveBgTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(liveBgColorButton, row, 2, buttonAlignment);
    ++row;

    QLabel *selectionColorTextLabel = new QLabel(tr("Selection Color"));
    NsColorButton *selectionColorButton = 
        new NsColorButton(_preferences->graphViewSelectionColor());
    connect(selectionColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,         SLOT(setGraphViewSelectionColor(QColor)));
    layout->addWidget(selectionColorTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(selectionColorButton, row, 2, buttonAlignment);
    ++row;

    layout->addItem(new QSpacerItem(0, 20), row, 0, 1, 3);
    ++row;

    // Ops.

    QLabel *opHeaderLabel = new QLabel(tr("Ops"));
    opHeaderLabel->setFont(headerFont); 
    layout->addWidget(opHeaderLabel, row, 0, 1, 3, headerAlignment);
    ++row;

    QLabel *opLineColorTextLabel = new QLabel(tr("Line Color"));
    NsColorButton *opLineColorButton = 
        new NsColorButton(_preferences->graphViewOpLineColor());
    connect(opLineColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,        SLOT(setGraphViewOpLineColor(QColor)));
    layout->addWidget(opLineColorTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(opLineColorButton, row, 2, buttonAlignment);
    ++row;

    QLabel *opTextLabel = new QLabel(tr("Text Color"));
    NsColorButton *opTextColorButton = 
        new NsColorButton(_preferences->graphViewOpTextColor());
    connect(opTextColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,      SLOT(setGraphViewOpTextColor(QColor)));
    layout->addWidget(opTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(opTextColorButton, row, 2, buttonAlignment);
    ++row;

    QLabel *opBgTextLabel = new QLabel(tr("Background"));
    NsColorButton *opBgColorButton = 
        new NsColorButton(_preferences->graphViewOpBackground());
    connect(opBgColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,      SLOT(setGraphViewOpBackground(QColor)));
    layout->addWidget(opBgTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(opBgColorButton, row, 2, buttonAlignment);
    ++row;

    QLabel *opDisabledTextLabel = new QLabel(tr("Disabled"));
    NsColorButton *opDisabledColorButton = 
        new NsColorButton(_preferences->graphViewOpDisabledColor());
    connect(opDisabledColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,      SLOT(setGraphViewOpDisabledColor(QColor)));
    layout->addWidget(opDisabledTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(opDisabledColorButton, row, 2, buttonAlignment);
    ++row;

    layout->addItem(new QSpacerItem(0, 20), row, 0, 1, 3);
    ++row;

    // Bodies.

    QLabel *bodyHeaderLabel = new QLabel(tr("Bodies"));
    bodyHeaderLabel->setFont(headerFont); 
    layout->addWidget(bodyHeaderLabel, row, 0, 1, 3, headerAlignment);
    ++row;

    QLabel *bodyLineColorTextLabel = new QLabel(tr("Line Color"));
    NsColorButton *bodyLineColorButton = 
        new NsColorButton(_preferences->graphViewBodyLineColor());
    connect(bodyLineColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,        SLOT(setGraphViewBodyLineColor(QColor)));
    layout->addWidget(bodyLineColorTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(bodyLineColorButton, row, 2, buttonAlignment);
    ++row;

    QLabel *bodyTextLabel = new QLabel(tr("Text Color"));
    NsColorButton *bodyTextColorButton = 
        new NsColorButton(_preferences->graphViewBodyTextColor());
    connect(bodyTextColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,      SLOT(setGraphViewBodyTextColor(QColor)));
    layout->addWidget(bodyTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(bodyTextColorButton, row, 2, buttonAlignment);
    ++row;

    QLabel *bodyBgTextLabel = new QLabel(tr("Background"));
    NsColorButton *bodyBgColorButton = 
        new NsColorButton(_preferences->graphViewBodyBackground());
    connect(bodyBgColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,      SLOT(setGraphViewBodyBackground(QColor)));
    layout->addWidget(bodyBgTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(bodyBgColorButton, row, 2, buttonAlignment);
    ++row;

    layout->addItem(new QSpacerItem(0, 20), row, 0, 1, 3);
    ++row;

    // Feeds.

    QLabel *feedHeaderLabel = new QLabel(tr("Feeds"));
    feedHeaderLabel->setFont(headerFont); 
    layout->addWidget(feedHeaderLabel, row, 0, 1, 3, headerAlignment);
    ++row;

    QLabel *feedLineColorTextLabel = new QLabel(tr("Line Color"));
    NsColorButton *feedLineColorButton = 
        new NsColorButton(_preferences->graphViewFeedLineColor());
    connect(feedLineColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,        SLOT(setGraphViewFeedLineColor(QColor)));
    layout->addWidget(feedLineColorTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(feedLineColorButton, row, 2, buttonAlignment);
    ++row;

    QLabel *feedReplacedLineTextLabel = new QLabel(tr("Replaced Line Color"));
    NsColorButton *feedReplacedLineColorButton = 
        new NsColorButton(_preferences->graphViewFeedReplacedLineColor());
    connect(feedReplacedLineColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,      SLOT(setGraphViewFeedReplacedLineColor(QColor)));
    layout->addWidget(feedReplacedLineTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(feedReplacedLineColorButton, row, 2, buttonAlignment);
    ++row;
    
    QLabel *feedValidLineTextLabel = new QLabel(tr("Valid Line Color"));
    NsColorButton *feedValidLineColorButton = 
        new NsColorButton(_preferences->graphViewFeedValidLineColor());
    connect(feedReplacedLineColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,      SLOT(setGraphViewFeedValidLineColor(QColor)));
    layout->addWidget(feedValidLineTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(feedValidLineColorButton, row, 2, buttonAlignment);
    ++row;

    QLabel *feedInvalidLineTextLabel = new QLabel(tr("Invalid Line Color"));
    NsColorButton *feedInvalidLineColorButton = 
        new NsColorButton(_preferences->graphViewFeedInvalidLineColor());
    connect(feedInvalidLineColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,      SLOT(setGraphViewFeedInvalidLineColor(QColor)));
    layout->addWidget(feedInvalidLineTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(feedInvalidLineColorButton, row, 2, buttonAlignment);
    ++row;

    layout->addItem(new QSpacerItem(0, 20), row, 0, 1, 3);
    ++row;

    // Plugs.

    QLabel *plugHeaderLabel = new QLabel(tr("Plugs"));
    plugHeaderLabel->setFont(headerFont); 
    layout->addWidget(plugHeaderLabel, row, 0, 1, 3, headerAlignment);
    ++row;

    QLabel *plugLineColorTextLabel = new QLabel(tr("Line Color"));
    NsColorButton *plugLineColorButton = 
        new NsColorButton(_preferences->graphViewPlugLineColor());
    connect(plugLineColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,        SLOT(setGraphViewPlugLineColor(QColor)));
    layout->addWidget(plugLineColorTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(plugLineColorButton, row, 2, buttonAlignment);
    ++row;

    QLabel *plugTextLabel = new QLabel(tr("Text Color"));
    NsColorButton *plugTextColorButton = 
        new NsColorButton(_preferences->graphViewPlugTextColor());
    connect(plugTextColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,      SLOT(setGraphViewPlugTextColor(QColor)));
    layout->addWidget(plugTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(plugTextColorButton, row, 2, buttonAlignment);
    ++row;

    QLabel *plugBgTextLabel = new QLabel(tr("Background"));
    NsColorButton *plugBgColorButton = 
        new NsColorButton(_preferences->graphViewPlugBackground());
    connect(plugBgColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,      SLOT(setGraphViewPlugBackground(QColor)));
    layout->addWidget(plugBgTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(plugBgColorButton, row, 2, buttonAlignment);
    ++row;

    layout->addItem(new QSpacerItem(0, 20), row, 0, 1, 3);
    ++row;

    grid->addLayout(layout, 0, 0, 1, 1, Qt::AlignTop);
    }

    // Second column.

    {
    int row = 0;
    QGridLayout *layout = new QGridLayout;
    QLabel *opCategoryHeaderLabel = new QLabel(tr("Op Categories"));
    opCategoryHeaderLabel->setFont(headerFont); 
    layout->addWidget(opCategoryHeaderLabel, row, 0, 1, 3, headerAlignment);
    ++row;

    foreach (const QString &name, _preferences->graphViewOpCategoryNames()) {
        QLabel *nameLabel = new QLabel(name);
        NsNamedColorButton *colorButton = 
            new NsNamedColorButton(
                name, _preferences->graphViewOpCategoryColor(name));
        connect(colorButton,  SIGNAL(colorChanged(QString,QColor)),
                _preferences, SLOT(setGraphViewOpCategoryColor(QString,QColor)));
        layout->addWidget(nameLabel, row, 0, labelAlignment);
        layout->addItem(new QSpacerItem(10, 0), row, 1);
        layout->addWidget(colorButton, row, 2, buttonAlignment);
        ++row;
    }

    QLabel *defaultCategoryLabel = new QLabel(tr("<default>"));
    NsColorButton *defaultCategoryColorButton = 
        new NsColorButton(_preferences->graphViewOpCategoryDefaultColor());
    connect(defaultCategoryColorButton,  SIGNAL(colorChanged(QColor)),
            _preferences, SLOT(setGraphViewOpCategoryDefaultColor(QColor)));
    layout->addWidget(defaultCategoryLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(defaultCategoryColorButton, row, 2, buttonAlignment);
    ++row;

    layout->addItem(new QSpacerItem(0, 20), row, 0, 1, 3);
    ++row;

    // Op states.

    QLabel *opStatesHeaderLabel = new QLabel(tr("Op States"));
    opStatesHeaderLabel->setFont(headerFont); 
    layout->addWidget(opStatesHeaderLabel, row, 0, 1, 2, headerAlignment);
    ++row;

    foreach (const QString &name, _preferences->graphViewOpStateNames()) {
        QLabel *nameLabel = new QLabel(name);
        NsNamedColorButton *colorButton = 
            new NsNamedColorButton(
                name, _preferences->graphViewOpStateColor(name));
        connect(colorButton,  SIGNAL(colorChanged(QString,QColor)),
                _preferences, SLOT(setGraphViewOpStateColor(QString,QColor)));
        layout->addWidget(nameLabel, row, 0, labelAlignment);
        layout->addItem(new QSpacerItem(10, 0), row, 1);
        layout->addWidget(colorButton, row, 2, buttonAlignment);
        ++row;
    }

    QLabel *defaultStateLabel = new QLabel(tr("<default>"));
    NsColorButton *defaultStateColorButton = 
        new NsColorButton(_preferences->graphViewOpStateDefaultColor());
    connect(defaultStateColorButton,  SIGNAL(colorChanged(QColor)),
            _preferences, SLOT(setGraphViewOpStateDefaultColor(QColor)));
    layout->addWidget(defaultStateLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(defaultStateColorButton, row, 2, buttonAlignment);
    ++row;

    layout->addItem(new QSpacerItem(0, 20), row, 0, 1, 3);
    ++row;

    grid->addLayout(layout, 0, 1, 1, 1, Qt::AlignTop);
    }

    // Third column.

    {
    int row = 0;
    QGridLayout *layout = new QGridLayout;

    // Signatures.

    QLabel *sigHeaderLabel = new QLabel(tr("Signatures"));
    sigHeaderLabel->setFont(headerFont); 
    layout->addWidget(sigHeaderLabel, row, 0, 1, 3, headerAlignment);
    ++row;

    foreach (const QString &name, _preferences->graphViewSignatureNames()) {
        QLabel *nameLabel = new QLabel(name);
        NsNamedColorButton *colorButton = 
            new NsNamedColorButton(
                name, _preferences->graphViewSignatureColor(name));
        connect(colorButton,  SIGNAL(colorChanged(QString,QColor)),
                _preferences, SLOT(setGraphViewSignatureColor(QString,QColor)));
        layout->addWidget(nameLabel, row, 0, labelAlignment);
        layout->addItem(new QSpacerItem(10, 0), row, 1);
        layout->addWidget(colorButton, row, 2, buttonAlignment);
        ++row;
    }

    QLabel *defaultSigLabel = new QLabel(tr("<default>"));
    NsColorButton *defaultSigColorButton = 
        new NsColorButton(_preferences->graphViewSignatureDefaultColor());
    connect(defaultSigColorButton,  SIGNAL(colorChanged(QColor)),
            _preferences, SLOT(setGraphViewOpStateDefaultColor(QColor)));
    layout->addWidget(defaultSigLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(defaultSigColorButton, row, 2, buttonAlignment);
    ++row;

    layout->addItem(new QSpacerItem(0, 20), row, 0, 1, 3);
    ++row;

    grid->addLayout(layout, 0, 2, 1, 1, Qt::AlignTop);    
    }

    page->setLayout(grid);
    return page;
}

QWidget*
NsPreferencesDialog::_createScopeViewPage()
{
    QWidget *page = new QWidget(this);   // Child.

    // Create layout.

    const Qt::Alignment headerAlignment = Qt::AlignLeft | Qt::AlignVCenter;
    const Qt::Alignment labelAlignment = Qt::AlignRight | Qt::AlignVCenter;
    const Qt::Alignment buttonAlignment = Qt::AlignRight | Qt::AlignVCenter;
    const QFont headerFont(QFont("sansserif", 12, QFont::Bold));
    int gridRow = 0;
    QGridLayout *grid = new QGridLayout;
    grid->setHorizontalSpacing(40);
    grid->setVerticalSpacing(20);

    {
    int row = 0;
    QGridLayout *layout = new QGridLayout;
    QLabel *headerLabel = new QLabel(tr("General"));
    headerLabel->setFont(headerFont); 
    layout->addWidget(headerLabel, 0, 0, 1, 3, headerAlignment);
    ++row;

    QLabel *bgTextLabel = new QLabel(tr("Background"));
    NsColorButton *bgColorButton = 
        new NsColorButton(_preferences->scopeViewBackground());
    connect(bgColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,  SLOT(setScopeViewBackground(QColor)));
    layout->addWidget(bgTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(bgColorButton, row, 2, buttonAlignment);
    ++row;

    QLabel *selectionColorTextLabel = new QLabel(tr("Selection Color"));
    NsColorButton *selectionColorButton = 
        new NsColorButton(_preferences->scopeViewSelectionColor());
    connect(selectionColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,         SLOT(setScopeViewSelectionColor(QColor)));
    layout->addWidget(selectionColorTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(selectionColorButton, row, 2, buttonAlignment);
    ++row;

    QLabel *constructionGridTextLabel = new QLabel(tr("Construction Grid"));
    NsColorButton *constructionGridButton =
        new NsColorButton(_preferences->scopeViewConstructionGridColor());
    connect(constructionGridButton, SIGNAL(colorChanged(QColor)),
            _preferences,      SLOT(setScopeViewConstructionGridColor(QColor)));
    layout->addWidget(constructionGridTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(constructionGridButton, row, 2, buttonAlignment);
    ++row;

    QLabel *hudTextTextLabel = new QLabel(tr("HUD Text"));
    NsColorButton *hudTextColorButton = 
        new NsColorButton(_preferences->scopeViewHudTextColor());
    connect(hudTextColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,        SLOT(setScopeViewHudTextColor(QColor)));
    layout->addWidget(hudTextTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(hudTextColorButton, row, 2);
    ++row;

    QLabel *bodyLabelTextLabel = new QLabel(tr("Body Label Text"));
    NsColorButton *bodyLabelTextColorButton = 
        new NsColorButton(_preferences->scopeViewBodyLabelTextColor());
    connect(bodyLabelTextColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,        SLOT(setScopeViewBodyLabelTextColor(QColor)));
    layout->addWidget(bodyLabelTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(bodyLabelTextColorButton, row, 2);
    ++row;

    layout->addItem(new QSpacerItem(0, 20), row, 0, 1, 3);
    ++row;

    grid->addLayout(layout, 0, 0, 1, 1, Qt::AlignTop);
    }

    {
    int row = 0;
    QGridLayout *layout = new QGridLayout;
    QLabel *headerLabel = new QLabel(tr("Op Items"));
    headerLabel->setFont(headerFont); 
    layout->addWidget(headerLabel, 0, 0, 1, 3, headerAlignment);
    ++row;

    QLabel *axisLineTextLabel = new QLabel(tr("Axis Line"));
    NsColorButton *axisLineColorButton = 
        new NsColorButton(_preferences->scopeViewAxisLineColor());
    connect(axisLineColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,        SLOT(setScopeViewAxisLineColor(QColor)));
    layout->addWidget(axisLineTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(axisLineColorButton, row, 2, buttonAlignment);
    ++row;

    QLabel *axisTextTextLabel = new QLabel(tr("Axis Text"));
    NsColorButton *axisTextColorButton = 
        new NsColorButton(_preferences->scopeViewAxisTextColor());
    connect(axisTextColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,        SLOT(setScopeViewAxisTextColor(QColor)));
    layout->addWidget(axisTextTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(axisTextColorButton, row, 2, buttonAlignment);
    ++row;

    QLabel *boxLineTextLabel = new QLabel(tr("Box Line"));
    NsColorButton *boxLineColorButton = 
        new NsColorButton(_preferences->scopeViewBoxLineColor());
    connect(boxLineColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,        SLOT(setScopeViewBoxLineColor(QColor)));
    layout->addWidget(boxLineTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(boxLineColorButton, row, 2);
    ++row;

    QLabel *boxTextTextLabel = new QLabel(tr("Box Text"));
    NsColorButton *boxTextColorButton = 
        new NsColorButton(_preferences->scopeViewBoxTextColor());
    connect(boxTextColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,        SLOT(setScopeViewBoxTextColor(QColor)));
    layout->addWidget(boxTextTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1); // Layout owns.
    layout->addWidget(boxTextColorButton, row, 2, buttonAlignment);
    ++row;

    QLabel *clipBoxLineTextLabel = new QLabel(tr("Clip Box Line"));
    NsColorButton *clipBoxLineColorButton = 
        new NsColorButton(_preferences->scopeViewClipBoxLineColor());
    connect(clipBoxLineColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,        SLOT(setScopeViewClipBoxLineColor(QColor)));
    layout->addWidget(clipBoxLineTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(clipBoxLineColorButton, row, 2, buttonAlignment);
    ++row;

    QLabel *clipBoxTextTextLabel = new QLabel(tr("Clip Box Text"));
    NsColorButton *clipBoxTextColorButton = 
        new NsColorButton(_preferences->scopeViewClipBoxTextColor());
    connect(clipBoxTextColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,        SLOT(setScopeViewClipBoxTextColor(QColor)));
    layout->addWidget(clipBoxTextTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(clipBoxTextColorButton, row, 2, buttonAlignment);
    ++row;

    QLabel *frustumLineTextLabel = new QLabel(tr("Frustum Line"));
    NsColorButton *frustumLineColorButton = 
        new NsColorButton(_preferences->scopeViewFrustumLineColor());
    connect(frustumLineColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,        SLOT(setScopeViewFrustumLineColor(QColor)));
    layout->addWidget(frustumLineTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(frustumLineColorButton, row, 2, buttonAlignment);
    ++row;

    QLabel *frustumTextTextLabel = new QLabel(tr("Frustum Text"));
    NsColorButton *frustumTextColorButton = 
        new NsColorButton(_preferences->scopeViewFrustumTextColor());
    connect(frustumTextColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,        SLOT(setScopeViewFrustumTextColor(QColor)));
    layout->addWidget(frustumTextTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(frustumTextColorButton, row, 2, buttonAlignment);
    ++row;

    QLabel *importXfLineTextLabel = new QLabel(tr("Import Transform Line"));
    NsColorButton *importXfLineColorButton =
        new NsColorButton(_preferences->scopeViewImportTransformLineColor());
    connect(importXfLineColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,   SLOT(setScopeViewImportTransformLineColor(QColor)));
    layout->addWidget(importXfLineTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(importXfLineColorButton, row, 2, buttonAlignment);
    ++row;

    QLabel *importXfTextTextLabel = new QLabel(tr("Import Transform Text"));
    NsColorButton *importXfTextColorButton =
        new NsColorButton(_preferences->scopeViewImportTransformTextColor());
    connect(importXfTextColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,   SLOT(setScopeViewImportTransformTextColor(QColor)));
    layout->addWidget(importXfTextTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(importXfTextColorButton, row, 2, buttonAlignment);
    ++row;

    QLabel *planeLineTextLabel = new QLabel(tr("Plane Line"));
    NsColorButton *planeLineColorButton = 
        new NsColorButton(_preferences->scopeViewPlaneLineColor());
    connect(planeLineColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,        SLOT(setScopeViewPlaneLineColor(QColor)));
    layout->addWidget(planeLineTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(planeLineColorButton, row, 2, buttonAlignment);
    ++row;

    QLabel *planeTextTextLabel = new QLabel(tr("Plane Text"));
    NsColorButton *planeTextColorButton = 
        new NsColorButton(_preferences->scopeViewPlaneTextColor());
    connect(planeTextColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,        SLOT(setScopeViewPlaneTextColor(QColor)));
    layout->addWidget(planeTextTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(planeTextColorButton, row, 2, buttonAlignment);
    ++row;

    QLabel *sphereLineTextLabel = new QLabel(tr("Sphere Line"));
    NsColorButton *sphereLineColorButton = 
        new NsColorButton(_preferences->scopeViewSphereLineColor());
    connect(sphereLineColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,        SLOT(setScopeViewSphereLineColor(QColor)));
    layout->addWidget(sphereLineTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(sphereLineColorButton, row, 2, buttonAlignment);
    ++row;

    QLabel *sphereTextTextLabel = new QLabel(tr("Sphere Text"));
    NsColorButton *sphereTextColorButton = 
        new NsColorButton(_preferences->scopeViewSphereTextColor());
    connect(sphereTextColorButton, SIGNAL(colorChanged(QColor)),
            _preferences,        SLOT(setScopeViewSphereTextColor(QColor)));
    layout->addWidget(sphereTextTextLabel, row, 0, labelAlignment);
    layout->addItem(new QSpacerItem(10, 0), row, 1);
    layout->addWidget(sphereTextColorButton, row, 2, buttonAlignment);
    ++row;

    layout->addItem(new QSpacerItem(0, 20), row, 0, 1, 3);
    ++row;

    grid->addLayout(layout, 0, 1, 1, 1, Qt::AlignTop);
    }

    page->setLayout(grid);
    return page;
}


void
NsPreferencesDialog::_rebuild()
{
    delete centralWidget();                         // Let's start over...
    QTabWidget *tabWidget = new QTabWidget(this);   // Child.

    QWidget *generalPage = _createGeneralPage();
    QScrollArea *generalScrollArea = new QScrollArea;
    generalScrollArea->setWidget(generalPage); // Takes ownership.

    QWidget *graphViewPage = _createGraphViewPage();
    QScrollArea *graphViewScrollArea = new QScrollArea;
    graphViewScrollArea->setWidget(graphViewPage); // Takes ownership.

    QWidget *scopeViewPage = _createScopeViewPage();
    QScrollArea *scopeViewScrollArea = new QScrollArea;
    scopeViewScrollArea->setWidget(scopeViewPage); // Takes ownership.

    tabWidget->addTab(generalScrollArea,   tr("General"));
    tabWidget->addTab(graphViewScrollArea, tr("Graph View"));
    tabWidget->addTab(scopeViewScrollArea, tr("Scope View"));
    setCentralWidget(tabWidget);
}

// -----------------------------------------------------------------------------

// _reset
// ------
//! DOCS

void
NsPreferencesDialog::_reset()
{ 
    _preferences->reset(); 
    _rebuild();
}


// _import
// -------
//! DOCS

void
NsPreferencesDialog::_import()
{
    const QString fileName = 
        NsPath::getOpenFileName(
            this, 
            tr("Import Preferences"), 
            "../..", 
            tr("XML Files (*.xml);;All Files (*.*)"));

    if (!fileName.isEmpty()) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        QFile importFile(fileName);
        if (importFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            if (_preferences->readXml(&importFile)) {
                _rebuild();
                NsMessageWidget::instance()->clientInfo(
                    tr("Imported preferences from ") + 
                    "'" + importFile.fileName() + "'");
            }
            else {
                NsMessageWidget::instance()->clientError(
                    tr("Failed to import preferences from ") + 
                    "'" + importFile.fileName() + "'");
            }
            importFile.close();
        }
        else {
            NsMessageWidget::instance()->clientError(
                tr("Failed to open file ") + 
                "'" + importFile.fileName() + "'");
        }
        QApplication::restoreOverrideCursor();
    }
}

// _export
// -------
//! DOCS

void
NsPreferencesDialog::_export()
{
    QString fileName = 
        NsPath::getSaveFileName(
            this,
            tr("Export Preferences"),
            "../..",
            tr("XML Files (*.xml);;All Files (*.*)"));

    if (!fileName.isEmpty()) {
        if (QFileInfo(fileName).suffix().isEmpty()) {
            fileName += ".xml"; // Add default suffix if necessary.
        }

        QApplication::setOverrideCursor(Qt::WaitCursor);
        QFile exportFile(fileName);
        if (exportFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            _preferences->writeXml(&exportFile);
            NsMessageWidget::instance()->clientInfo(
                tr("Exported preferences to ") + 
                "'" + exportFile.fileName() + "'");
            exportFile.close();
        }
        else {
            NsMessageWidget::instance()->clientError(
                tr("Failed to open file ") + 
                "'" + exportFile.fileName() + "'");
        }
        QApplication::restoreOverrideCursor();
    }
}

// -----------------------------------------------------------------------------
