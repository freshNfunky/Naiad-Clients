// -----------------------------------------------------------------------------
//
// NsMainWindow.h
//
// Naiad Studio main window header file.
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

#ifndef NS_MAIN_WINDOW_H
#define NS_MAIN_WINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QAction>
#include <QFileInfo>
#include <QMenu>
#include <QString>

class NsConsole;
class NsSplashScreen;
class NsUndoStack;
class NsUndoView;
class NsGraphScene;
class NsGraphView;
class Ns3DView;
class Ns3DScene;
class NsHelpView;
class NsValueEditorWidget;
class NsDockWidget;
class NsTimeToolBar;
class NsPlayblastDialog;

class NsSelectAction;
class NsFocusAction;
class NsHoverAction;

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QToolBar;
QT_END_NAMESPACE

// -----------------------------------------------------------------------------

// NsMainWindow
// ------------
//! The NsMainWindow class, representing the Naiad Studio main window.

class NsMainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit
    NsMainWindow(NsSplashScreen *splash = 0);

    virtual
    ~NsMainWindow();

    void
    newGraph(const QString &emptyLabel, 
             const bool clearGraph = true,
             const bool createCamera = true)
    { 
        _newGraph(emptyLabel, clearGraph, createCamera); 
        _setGraphFileName(_defaultGraphFileName());
    }

    void
    openGraph(const QString &fileName)
    { _openGraph(fileName); }

    bool
    playblast(const QString &path,
              const QString &fileName,
              int            firstFrame,
              int            lastFrame,
              int            width,
              int            height,
              const QString &format,
              int            quality);

    void
    readSettings()
    { _readSettings(); }

    const QString&
    graphFileName() const
    { return _graphFileName; }

protected slots:

    void
    onGraphCleared(bool success);

protected:      // Events.

    virtual void
    closeEvent(QCloseEvent *event);

    virtual void
    keyPressEvent(QKeyEvent *event);

protected:

    NsUndoStack*
    undoStack()
    { return _undoStack; }

private slots:  // Action slots.

    // File menu action slots.

    void
    _onNewGraph();

    void
    _onOpenGraph();

    void
    _onOpenRecent();

    bool
    _onSaveGraph();

    bool
    _onSaveGraphAs();

    void
    _onCmdLine();

    void
    _onSubmitToDeadline();

    bool
    _onExportGraph();


    // Edit menu action slots.

    void
    _onCutSelection();

    void
    _onCopySelection();

    void
    _onPaste();

    void
    _onEraseSelection();

    void
    _onSelectAll(bool selected);

    void
    _onSelectAllOps(bool selected);

    void
    _onSelectAllBodies(bool selected);

    void
    _onSelectAllFeeds(bool selected);

    void
    _onPreferences();


    // 3D View menu action slots.

    void
    _onPlayblast();

    // Graph View menu action slots.

    void
    _onToggleSelectedOpState();

    void
    _onToggleSelectedOpEnabled();

    // Window menu action slots.

    void
    _onShowFullScreen(bool fullScreen);

    void
    _onResetLayout();


    // Help menu action slots.

    void
    _onAbout();

private slots:  // Slots.

    void
    _onCleanChanged(bool clean);


    void
    _onClipboardChanged();

    void
    _onConsoleClosed(NsConsole *console);

    void
    _onOpSelectionChanged(const QStringList &opInstances, bool success);

    void
    _onFeedSelectionChanged(const QStringList &inputLongNames, bool success);

    //void _onSceneFileNameChanged(const QString &sceneFileName);

    //void _updateWindowTitle();

    //    void updateActions();
    //    void onGraphChanged(NsGraph *graph);
    //    void onAlarmMessage(const QString &text);

private:    // GUI creation.

    void
    _createDockWidgets();

    void
    _createActions();

    void
    _createMenus();

    void
    _createToolBars();

    void
    _createStatusBar();

private:

    void
    _addRecentGraphAction(const QString &graphFileName);

    QAction*
    _createRecentGraphAction(const QString &graphFileName);

private:    // Settings.

    void
    _readSettings();

    void
    _readDefaultSettings();

    void
    _writeSettings();

    void
    _writeDefaultSettings();

private:    // Implementations.

    void
    _newGraph(const QString &emptyLabel, 
              const bool clearGraph = true,
              const bool createCamera = true );

    bool
    _saveGraph(const QString &fileName, const QString &format);

    bool
    _saveGraphIfModified();

    void
    _openGraph(const QString &fileName);

private:    // File name dialogs.

    //QString
    //_getOpenFileName(const QString &caption,
    //                 const QString &path);

    QString
    _getSaveFileName(const QString &caption,
                     const QString &path);

    QString
    _getExportFileName(const QString &caption,
                       const QString &path,
                       QString       &format);

    void
    _setGraphFileName(const QString &fileName);

private:

    static const int _maxRecentGraphs = 5; //!< Max number of recent graphs.

    static const QString _defaultGraphFileNameBase;
    static const QString _defaultGraphFileNameSuffix;
    static const QString _settingsGroup;

    static const int _defaultFirstVisibleFrame;
    static const int _defaultLastVisibleFrame;

    static QString
    _defaultGraphFileName();

    static QString
    _backupFileName(const QString &fileName);

    static void
    _setFirstVisibleFrame();

    static void
    _setLastVisibleFrame();

    static void
    _setCurrentVisibleFrame();

private:

    static NsUndoStack*
    _createUndoStack(QObject *parent);

    static NsUndoView*
    _createUndoView(NsUndoStack *undoStack, QWidget *parent);

    static NsGraphScene*
    _createGraphScene(NsUndoStack *undoStack, QObject *parent);

    static NsGraphView*
    _createGraphView(NsGraphScene   *scene, 
                     NsUndoStack *undoStack, 
                     QWidget        *parent);

    static Ns3DScene*
    _create3DScene(NsUndoStack *undoStack, QObject *parent);

    static Ns3DView*
    _create3DView(Ns3DScene *scene, QWidget *parent);

    static NsHelpView*
    _createHelpView(QWidget *parent);

    static NsValueEditorWidget*
    _createValueEditor(NsUndoStack *undoStack, QWidget *parent);

    static NsDockWidget*
    _createDockWidget(const QString                   &title, 
                      QWidget                         *parent, 
                      const Qt::WindowFlags            flags, 
                      QWidget                         *widget,
                      QDockWidget::DockWidgetFeatures  features = 
                          QDockWidget::DockWidgetClosable |
                          QDockWidget::DockWidgetMovable  |
                          QDockWidget::DockWidgetFloatable,
                      Qt::DockWidgetAreas              areas = 
                          Qt::AllDockWidgetAreas);

    static void
    _splashMessage(NsSplashScreen *splash, const QString &message);

private:    // Member variables.

    NsUndoStack         *_undoStack;
    NsUndoView          *_undoView;
    NsGraphScene        *_graphScene;
    NsGraphView         *_graphView;
    Ns3DScene           *_3DScene;
    Ns3DView            *_3DView;
    NsHelpView          *_helpView;
    NsValueEditorWidget *_valueEditor;

    QString _graphFileName;     //!< Current graph file name.


    // Toolbars.

    NsTimeToolBar *_timeToolBar;          //!< The Timeline tool bar.

    // Menus.

    QMenu *_fileMenu;
    QMenu *_fileRecentGraphsMenu;       //!< The File -> Recent graphs submenu.
    QMenu *_editMenu;
    QMenu *_3DViewMenu;
    QMenu *_graphMenu;
    QMenu *_graphZoomMenu;              //!< The Graph -> Zoom submenu.
    QMenu *_timeMenu;
    QMenu *_windowMenu;
    QMenu *_helpMenu;

    // Actions.

    QAction *_newGraphAction;             //!< The New Graph action.
    QAction *_openGraphAction;            //!< The Open Graph... action.
    QAction *_saveGraphAction;            //!< The Save Graph action.
    QAction *_saveGraphAsAction;          //!< The Save Graph As... action.
    QAction *_exportGraphAction;          //!< The Export Graph... action.
    QAction *_cmdLineAction;    
    QAction *_deadlineAction;             //!< The submit to deadline action.
    QAction *_quitAction;                 //!< The Quit action.

    // TODO: NsFocusAction's??
//    QAction *_undoAction;               //!< The Undo action.
//    QAction *_redoAction;               //!< The Redo action.
//    QAction *_cutAction;                //!< The Cut action.
//    QAction *_copyAction;               //!< The Copy action.
//    QAction *_pasteAction;              //!< The Paste action.
//    QAction *_eraseAction;              //!< The Erase action.

    QAction *_undoAction;               //!< The Undo action.
    QAction *_redoAction;               //!< The Redo action.

    QAction *_cutAction;                //!< The Cut action.
    QAction *_copyAction;               //!< The Copy action.
    QAction *_pasteAction;              //!< The Paste action.
    QAction *_eraseAction;              //!< The Erase action.

    //NsFocusAction *_frameAction;
    NsHoverAction *_frameAction;
    QAction *_frameAllAction;

    NsSelectAction *_selectAllAction;         //!< The Select All action.
    NsSelectAction *_selectAllOpsAction;      //!< The Select All Ops action.
    NsSelectAction *_selectAllBodiesAction;   //!< The Select All Bodies action.
    NsSelectAction *_selectAllFeedsAction;    //!< The Select All Feeds action.
    NsSelectAction *_unselectAllAction;       //!< The Unselect All action.
    NsSelectAction *_unselectAllOpsAction;    //!< The Unselect Ops action.
    NsSelectAction *_unselectAllBodiesAction; //!< The Unselect Bodies action.
    NsSelectAction *_unselectAllFeedsAction;  //!< The UnSelect Feeds action.

    QAction *_preferencesAction;

    QAction *_drawOriginGridAction;
    QAction *_drawHudAction;
    QAction *_drawAxesAction;
    //QAction *_3DViewFrameAction;
    QAction *_playblastAction;

    NsFocusAction   *_graphZoomInAction;
    NsFocusAction   *_graphZoomOutAction;
    //NsFocusAction   *_graphFrameAction;
    QList<QAction*>  _graphZoomActions;
    //    NsFocusAction *_zoomInAction;         //!< The Zoom In action.
    //    NsFocusAction *_zoomOutAction;        //!< The Zoom Out action.
    //    NsFocusAction *_frameGraphAction;     //!< The Frame Graph action.
    //    NsFocusAction *_resetViewAction;      //!< The Reset action.
    QAction *_graphSelectedOpStateToggleAction;
    QAction *_graphSelectedOpEnabledToggleAction;

    //QAction *_playFwdAction;
    //QAction *_playBwdAction;
    //QAction *_loopAction;
    //QAction *_stepBwdAction;
    //QAction *_stepFwdAction;
    //QAction *_firstFrameAction;
    //QAction *_lastFrameAction;
    //QAction *_resetAction;
    //QAction *_stepToLastAction;
    //QAction *_stepSingleAction;
    //QAction *_restepAction;

    QAction *_fullScreenAction;           //!< The Full Screen action.
    QAction *_resetLayoutAction;          //!< The Reset Layout action.

    QAction *_aboutAction;                //!< The About action.


    //NsGraphView *_graphView;              //!< The Op graph view.
    //NsPlayblastDialog *playblastDialog;  //!< The playblast dialog.
    //QString filePath;                    //!< The path used for file choosers.
};

#endif // NS_MAIN_WINDOW_H


//// NsRecentSceneAction
//// -------------------
////! Class for storing actions that open recently used files in
////! Open Naiad Studio. The text() shows only the file name, while the
////! action stores the full path to recently opened files.

//class NsRecentSceneAction : public QAction
//{
//    Q_OBJECT

//public:

//    //! CTOR:
//    NsRecentSceneAction(const QString &sceneFileName, NsMainWindow *parent)
//        : QAction(_extractFileName(sceneFileName), parent), // May throw.
//          _sceneFileName(sceneFileName)
//    {
//        EM_ASSERT(0 != parent);

//        setStatusTip(tr("Open graph from ") + _sceneFileName);
//        connect(this, SIGNAL(triggered()), parent, SLOT(openRecent()));
//    }


//    //! DTOR.
//    virtual ~NsRecentSceneAction() {}

//    //! Full path to graph file.
//    const QString &sceneFileName() const { return _sceneFileName; }

//private:

//    static QString
//    _extractFileName(const QString &sceneFileName)
//    {
//        QFileInfo info(sceneFileName);
//        const QString fileName(info.fileName());
//        EM_ASSERT(!fileName.isEmpty());

////        if (fileName.isEmpty()) {
////            NB_THROW("Empty file name!");
////        }

//        return fileName;
//    }

//private:    // Member variables.

//    QString _sceneFileName; //!< Full path to graph file.
//};


//class NsRecentSceneMenu
//{
//public:

//    //! CTOR.
//    NsRecentSceneMenu(const int maxSize = 5)
//        : _menu(0)
//    {}


//    void addAction(const QString &sceneFileName,
//                   NsMainWindow *receiver,
//                   const char *member)
//    {
//        _menu->addAction(SLOT(openRecent()))

//    }


//    QMenu *menu() { return _menu; }
//    void setMenu(QMenu *menu) { _menu = menu; }


//private:    // Member variables.

//    QMenu *_menu;   //!< Menu implmentation.
//    int _maxSize;   //!< Max number of recent scenes.

//private:    // Disabled.

//    NsRecentSceneMenu(const NsRecentSceneMenu&);
//    NsRecentSceneMenu& operator=(const NsRecentSceneMenu&);
//};






//// openScene
//// ---------
////! Opens the graph on the given file.

//bool
//Ns::openScene(const QString &fileName)
//{
//    message(tr("Opening graph from ") + fileName + "...");

//    bool opened = false;
//    QFileInfo info(fileName);

//    newGraph();
//    graph()->enableUpdateBodies(false);
//    setInternalErrorsAsErrors(true);
//    int errors = _errors;

//    try {
//        NsParserCallback callback;
//        NiParseFile(info.absoluteFilePath().toStdString(), &callback);
//        opened = true;
//    }
//    catch (std::exception &exception) {
//        error(tr("Failed to open graph from ") +
//              fileName + ": " + exception.what());
//    }

//    setInternalErrorsAsErrors(true);
//    errors = _errors - errors;

//    if (opened) {
//        if (errors)
//            message(tr("Opened graph from ") + fileName +
//                    tr(" with ") + QString::number(errors) + tr(" errors"));
//        else
//            message(tr("Successfully opened graph from ") + fileName);
//        _sceneFileName = fileName;
//    } else
//        _sceneFileName = QString();

//    readSettings();
//    emit sceneFileNameChanged(_sceneFileName);

//    graph()->enableUpdateBodies(true);
//    graph()->updateBodies();

//    return opened;
//}



//// newScene
//// --------
////! Closes the current graph and creates a new empty graph.

//void
//Ns::newScene()
//{
//    newGraph();
//    _sceneFileName = QString();
//    emit sceneFileNameChanged(_sceneFileName);
//}

