// -----------------------------------------------------------------------------
//
// NsMainWindow.cc
//
// Naiad Studio main window source file.
//
// Copyright (c) 2011 Exotic Matter AB.  All rights reserved.
//
// This file is part of Open Naiad Studio..
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

#include "NsMainWindow.h"
#include "NsConsole.h"
#include "NsProcess.h"
#include "NsProcessDialog.h"
#include "NsPreferences.h"
#include "NsPath.h"
#include "NsApplication.h"
#include "NsDockWidget.h"
#include "NsSplashScreen.h"
#include "NsParserCallback.h"
#include "NsGraphCallback.h"

#include "NsUndoStack.h"
#include "NsUndoView.h"
#include "NsGraphScene.h"
#include "NsGraphView.h"
#include "Ns3DScene.h"
#include "Ns3DView.h"
#include "NsHelpView.h"
#include "NsValueEditorWidget.h"
#include "NsMessageWidget.h"

#include "NsTimeToolBar.h"

#include "NsAboutDialog.h"
#include "NsPlayblastDialog.h"
#include "NsPreferences.h"
#include "NsPreferencesDialog.h"

#include "NsValueBaseObject.h"

#include "NsSelectAction.h"
#include "NsFocusAction.h"
#include "NsHoverAction.h"

#include "NsCmdCentral.h"
#include "NsCmdSelectAll.h"
#include "NsCmdSelectAllOps.h"
#include "NsCmdSelectAllBodies.h"
#include "NsCmdSelectAllFeeds.h"
#include "NsCmdSelectOp.h"
#include "NsCmdFrameGraphView.h"
#include "NsCmdClearGraph.h"
#include "NsCmdCreate.h"
#include "NsCmdUnselectAndErase.h"
#include "NsCmdCopy.h"
#include "NsCmdCut.h"
#include "NsCmdPaste.h"
#include "NsCmdFeed.h"
#include "NsCmdSetFirstVisibleFrame.h"
#include "NsCmdSetLastVisibleFrame.h"
#include "NsCmdSetCurrentVisibleFrame.h"
#include "NsCmdSetOpPosition.h"
#include "NsCmdSetOpState.h"
#include "NsCmdSetParam.h"

#include <QProcess>
#include <QPlainTextEdit>
#include <QtGui>

//#include <Ni.h>
//#include "Ns.h"
//#include "NsIconStore.h"

// -----------------------------------------------------------------------------

// NsMainWindow
// ------------
//! CTOR, creates the NsMainWindow and sets everything up, in the
//! correct order.

NsMainWindow::NsMainWindow(NsSplashScreen *splash)
    : QMainWindow()
    , _undoStack(_createUndoStack(this))
    , _undoView(_createUndoView(_undoStack, this))
    , _graphScene(_createGraphScene(_undoStack, this))
    , _graphView(_createGraphView(_graphScene, _undoStack, this))
    , _3DScene(_create3DScene(_undoStack, this))
    , _3DView(_create3DView(_3DScene, this))
    , _helpView(_createHelpView(this))
    , _valueEditor(_createValueEditor(_undoStack, this))
    , _graphFileName("")
{    
    setWindowIcon(QIcon(":/images/nstudio-window-icon.png"));
    setAnimated(false);
    setUnifiedTitleAndToolBarOnMac(true);

    _splashMessage(splash, tr("Creating graph view..."));
    setCentralWidget(_graphView);

    _splashMessage(splash, tr("Creating dock widgets..."));
    _createDockWidgets();

    _splashMessage(splash, tr("Creating actions..."));
    _createActions();

    _splashMessage(splash, tr("Creating toolbars..."));
    _createToolBars();

    _splashMessage(splash, tr("Creating menus..."));
    _createMenus();

    _splashMessage(splash, tr("Creating status bar..."));
    _createStatusBar();

    _writeDefaultSettings(); // Before reading settings...
    //_splashMessage(splash, tr("Reading settings..."));
    //_readSettings();

    //_3DView->activate();
    _3DView->raise();
    _3DView->show();
    //_3DView->popup();

    connect(QApplication::clipboard(),
            SIGNAL(dataChanged()),
            SLOT(_onClipboardChanged()));

    // Internal connections.

    connect(_undoStack,  
            SIGNAL(cleanChanged(bool)),
            SLOT(_onCleanChanged(bool)));

    // Connect to command central.

    connect(NsCmdCentral::instance(),
            SIGNAL(opSelectionChanged(QStringList,bool)),
            SLOT(_onOpSelectionChanged(QStringList,bool)));
    connect(NsCmdCentral::instance(),
            SIGNAL(feedSelectionChanged(QStringList,bool)),
            SLOT(_onFeedSelectionChanged(QStringList,bool)));
    connect(NsCmdCentral::instance(),
            SIGNAL(graphCleared(bool)),
            SLOT(onGraphCleared(bool)));

    connect(NsPreferences::instance(), SIGNAL(changed()), SLOT(update()));
            

    // NB: Call after menus have been created.

    //_newGraph(tr("New Graph"), false);
    //_setGraphFileName(_defaultGraphFileName());

    //NsPreferences::instance()->
}


// ~NsMainWindow
// -------------
//! DTOR.

NsMainWindow::~NsMainWindow()
{}


// playblast
// ---------
//! Exports the given frame range as a sequence of image files in the given
//! image format.

bool
NsMainWindow::playblast(const QString &path,
                        const QString &fileName,
                        const int      firstFrame,
                        const int      lastFrame,
                        const int      width,
                        const int      height,
                        const QString &format,
                        const int      quality)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    const QString message(tr("Exporting image sequence to ") + path + "...");
    statusBar()->showMessage(message);

    const bool success = 
        _3DView->playblast(path,
                           fileName,
                           firstFrame,
                           lastFrame,
                           width,
                           height,
                           format,
                           quality);

    statusBar()->clearMessage();
    QApplication::restoreOverrideCursor();

    return success;
}

// -----------------------------------------------------------------------------

// onGraphCleared
// --------------
//! [slot]

void
NsMainWindow::onGraphCleared(const bool success)
{
    if (success) {
        qDebug() << "NsMainWindow::onGraphCleared";

        _graphView->setUpdatesEnabled(false);
        delete _graphScene;
        _graphScene = _createGraphScene(_undoStack, this);

        connect(_graphScene, SIGNAL(itemRemoved(NsGraphItem*)),   
                _graphView,  SLOT(onItemRemoved(NsGraphItem*)));

        _graphView->setScene(_graphScene);
        _graphView->setUpdatesEnabled(true);

        _3DView->setUpdatesEnabled(false);
        delete _3DScene;
        _3DScene = _create3DScene(_undoStack, this);
        _3DView->setScene(_3DScene);
        _3DView->setUpdatesEnabled(true);
    }
}

// -----------------------------------------------------------------------------

// closeEvent
// ----------
//! Overridden closeEvent method, saves settings and checks that graph is saved
//! before main windows is closed.

void
NsMainWindow::closeEvent(QCloseEvent *event)
{
    if (_saveGraphIfModified()) {
        _writeSettings();
        //event->accept();

        QMainWindow::closeEvent(event);
    }
    else {
        event->ignore();
    }
}


// keyPressEvent
// -------------
//! Overridden keyPressEvent method. Stops solve if <Esc> is pressed.

void
NsMainWindow::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "NsMainWindow::keyPressEvent";

//    switch (event->key()) {
//    case Qt::Key_Escape:
//        if (!NsGraphCallback::instance()->steppingStopped()) {
//            // TODO: Use time toolbar actions to stop solve!

//            NsGraphCallback::instance()->setSteppingStopped(true);
//            statusBar()->showMessage(tr("Stepping stopped"), 5000);
//        }
//        break;
//    default:
//        break;
//    }

    QMainWindow::keyPressEvent(event);
}

// -----------------------------------------------------------------------------

// newScene
// --------
//! Closes the current graph and creates a new empty graph. [slot]

void
NsMainWindow::_onNewGraph()
{
    if (_saveGraphIfModified()) {
        _newGraph(tr("New Graph"));
        _setGraphFileName(_defaultGraphFileName());
    }
}


// openScene
// ---------
//! Opens file dialog in which the user can select a graph file to open. [slot]

void
NsMainWindow::_onOpenGraph()
{
    if (_saveGraphIfModified()) {
        // Let user select a file name.

        const QString fileName(
            NsPath::getOpenFileName(
                this, 
                tr("Open Graph"),
                QFileInfo(_graphFileName).absolutePath()));

        if (!fileName.isEmpty()) {
            _openGraph(fileName);
        }
    }
}


// openRecent
// ----------
//! Opens a recently opened graph. [slot]

void
NsMainWindow::_onOpenRecent()
{
    if (_saveGraphIfModified()) {
        QAction *action = qobject_cast<QAction*>(sender());

        if (action && !action->text().isEmpty()) {
            _openGraph(action->text());
        }
    }
}


// saveScene
// ---------
//! Saves the current graph on its graph file.
//! Returns false if the save was cancelled or unsuccessful. [slot]

bool
NsMainWindow::_onSaveGraph()
{
    if (_graphFileName.isEmpty()) {
        return _onSaveGraphAs(); // Early exit.
    }

    return _saveGraph(_graphFileName, _defaultGraphFileNameSuffix);
}


// saveGraphAs
// -----------
//! Saves the current graph on a graph file selected by the user.
//! Returns false if the save was cancelled or unsuccessful. [slot]

bool
NsMainWindow::_onSaveGraphAs()
{
    const QString fileName(
        _getSaveFileName(
            tr("Save Graph As"), 
            QFileInfo(_graphFileName).absolutePath()));

    if (fileName.isEmpty()) {
        return false;
    }

    return _saveGraph(fileName, _defaultGraphFileNameSuffix);
}


void
NsMainWindow::_onCmdLine()
{
    if (_onSaveGraph()) {
        NsProcessDialog pd("naiad",
                           QFileInfo(_graphFileName).absoluteFilePath(),
                           evalParam1i("Global.First Frame"),
                           evalParam1i("Global.Last Frame"),
                           evalParam1i("Global.Thread Count"),
                           this);    // Child.

        if (QDialog::Accepted == pd.exec()) {
            // Execute process with arguments set in the dialog.

            NsConsole *console(new NsConsole);
            console->setReadOnly(true);
            //console->setStyleSheet("background-color: lightgray");
            console->setWindowTitle(tr("Naiad Console"));
            console->setMinimumHeight(640);
            console->setMinimumWidth(480);
            console->show();

            NsProcess *proc(new NsProcess(*console)); // Child.
            proc->setProcessChannelMode(QProcess::MergedChannels); // Merge cout & cerr.

            connect(proc,    SIGNAL(output(QString)),
                    console, SLOT(appendPlainText(QString)));

            connect(console, SIGNAL(destroyed()),
                    proc,    SLOT(terminate()));

            connect(console, SIGNAL(closed(NsConsole*)),
                    proc,    SLOT(terminate()));

            connect(console, SIGNAL(closed(NsConsole*)),
                    this,    SLOT(_onConsoleClosed(NsConsole*)));

            QString cmd;

#ifdef WIN32
            QString arguments(
                pd.fileNames() + " " +
                "--frames " + pd.firstFrame() + " " + pd.lastFrame() + " " +
                (pd.verbose() ? "--verbose " : "") +
                "--threads " + pd.threads() + " " +
                pd.optionalArgs() + " " +
                (pd.restartEmpSequence()!="" ? "--restart " : "") +
                (pd.restartEmpSequence()!="" ? pd.restartFrame() : "") + " " +
                (pd.restartEmpSequence()!="" ? pd.restartEmpSequence() : ""));

            cmd = ">> PROGRAM: " + pd.program() + "\n" +
                  ">> ARGUMENTS: " + arguments;
            proc->setNativeArguments(arguments);
#else
            QStringList arguments;
            arguments
                << pd.fileNames() << " "
                << "--frames" << pd.firstFrame() << pd.lastFrame()
                << (pd.verbose() ? "--verbose" : "")
                << "--threads" << pd.threads()
                << pd.optionalArgs()
                << (pd.restartEmpSequence()!="" ? "--restart" : "")
                << (pd.restartEmpSequence()!="" ? pd.restartFrame() : "")
                << (pd.restartEmpSequence()!="" ? pd.restartEmpSequence() : "");

            cmd = ">> PROGRAM: " + pd.program() + "\n";
            cmd += ">> ARGUMENTS: ";
            foreach (const QString &str, arguments) {
                cmd += str + " ";
            }
#endif  // WIN32

            console->appendPlainText(cmd);

#ifdef WIN32
            proc->start(pd.program());
#else
            // must set argument on LINUX/MAC OSX here
            // since setNativeArguments is Windows only (and not just Qt 4.7)
            proc->start(pd.program(), arguments);
#endif  // WIN32
        }
    }
}


//! [slot]

void
NsMainWindow::_onSubmitToDeadline()
{
    const QString cmd(
        "python $NAIAD_PATH/clients/nstudio/bin/SubmitToDeadline.py " +
        QFileInfo(_graphFileName).absoluteFilePath());
    qDebug() << cmd;
    system(cmd.toAscii().data());
}


// exportScene
// -----------
//! Exports the current Graph using some custom format.
//! Returns false if the export was cancelled or unsuccessful. [slot]

bool
NsMainWindow::_onExportGraph()
{
    QString format;
    const QString fileName(
        _getExportFileName(
            tr("Export Graph"),
            QFileInfo(_graphFileName).absolutePath(),
            format));

    if (fileName.isEmpty()) {
        return false;
    }

    return _saveGraph(fileName, format);
}

// -----------------------------------------------------------------------------

// _onCutSelection
// ---------------
//! [slot]

void
NsMainWindow::_onCutSelection()
{
    const QList<const NsOpObject *> selOps(
        NsOpStore::instance()->constSelectedOps());

    QStringList opInstances;
    foreach (const NsOpObject *op, selOps) {
        opInstances.append(op->longName());
    }

    if (0 != undoStack()) {
        NsCmdCut::exec(opInstances, *undoStack());
    }
    else {
        NsCmdCut::exec(opInstances);
    }
}


// _onCopySelection
// ----------------
//! [slot]

void
NsMainWindow::_onCopySelection()
{
    qDebug() << "NsMainWindow::_copySelection";
    const QList<const NsOpObject *> selOps =
        NsOpStore::instance()->constSelectedOps();
    QStringList opInstances;
    foreach (const NsOpObject *op, selOps) {
        opInstances.append(op->longName());
    }
    NsCmdCopy::exec(opInstances);
}


// _onPaste
// --------
//! [slot]

void
NsMainWindow::_onPaste()
{
    //_3DView->setUpdatesEnabled(false);

    if (0 != undoStack()) {
        NsCmdPaste::exec(
            _graphView->mapToSceneClamped(
                _graphView->viewport()->mapFromGlobal(QCursor::pos()),
                _graphView->rect()),
            *undoStack());
    }
    else {
        NsCmdPaste::exec(
            _graphView->mapToSceneClamped(
                _graphView->viewport()->mapFromGlobal(QCursor::pos()),
                _graphView->rect()));
    }

    //_3DView->setUpdatesEnabled(true);
}


// _onEraseSelection
// -----------------
//! [slot]

void
NsMainWindow::_onEraseSelection()
{
    const QList<const NsOpObject *> selOps(
        NsOpStore::instance()->constSelectedOps());

    QStringList opInstances;
    foreach (const NsOpObject *op, selOps) {
        opInstances.append(op->longName());
    }

    if (0 != undoStack()) {
        NsCmdUnselectAndErase::exec(opInstances, *undoStack());
    }
    else {
        NsCmdUnselectAndErase::exec(opInstances);
    }

    // TODO: Erase Ops and feeds, even if those feeds are not internal to
    //       the selected Ops!

    if (selOps.empty()) {
        // If no Ops were selected, erase feeds here. Otherwise, erase feeds
        // as part of Op erasing.

        const QList<const NsInputPlugObject *> selInputs(
            NsOpStore::instance()->constSelectedInputs(true));

        foreach (const NsInputPlugObject *ipo, selInputs) {
            if (0 != undoStack()) {
                NsCmdFeed::exec(ipo->longName(), "", *undoStack());
            }
            else {
                NsCmdFeed::exec(ipo->longName(), "");
            }
        }
    }
}


// _onSelectAll
// ------------
//! [slot]

void
NsMainWindow::_onSelectAll(const bool selected)
{
    if (0 != undoStack()) {
        NsCmdSelectAll::exec(selected, *undoStack());
    }
    else {
        NsCmdSelectAll::exec(selected);
    }
}


// _onSelectAllOps
// ---------------
//! [slot]

void
NsMainWindow::_onSelectAllOps(const bool selected)
{
    if (0 != undoStack()) {
        NsCmdSelectAllOps::exec(selected, *undoStack());
    }
    else {
        NsCmdSelectAllOps::exec(selected);
    }
}


// _onSelectAllBodies
// ------------------
//! [slot]

void
NsMainWindow::_onSelectAllBodies(const bool selected)
{
    if (0 != undoStack()) {
        NsCmdSelectAllBodies::exec(selected, *undoStack());
    }
    else {
        NsCmdSelectAllBodies::exec(selected);
    }
}


// _onSelectAllFeeds
// -----------------
//! [slot]

void
NsMainWindow::_onSelectAllFeeds(const bool selected)
{
    if (0 != undoStack()) {
        NsCmdSelectAllFeeds::exec(selected, *undoStack());
    }
    else {
        NsCmdSelectAllFeeds::exec(selected);
    }
}


// _onPreferences
// --------------
//! [slot]

void
NsMainWindow::_onPreferences()
{
    NsPreferencesDialog *pd = 
        new NsPreferencesDialog(*NsPreferences::instance(), this);    // Child.
    //pd->setWindowModality(Qt::WindowModal); // Block other windows.
    pd->setAttribute(Qt::WA_DeleteOnClose);
    pd->show();

    // TODO: If dialog was saved... update stuff!?
}

// -----------------------------------------------------------------------------

// _onPlayblast
// ------------
//! Exports frame range as a sequence of image files. [slot]

void
NsMainWindow::_onPlayblast()
{
    int fvf(1);
    queryFirstVisibleFrame(&fvf);   // Ignore success.

    int lvf(100);
    queryLastVisibleFrame(&lvf);    //Ignore success.

    // Set initial values in playblast dialog.

    QString path = _graphFileName;
    if (path.endsWith(".ni")) {
        path = path.left(path.length() - 3);
    }
    else if (!path.endsWith("/")) {
        path += "/";
    }

    path += ".playblast";

    NsPlayblastDialog pbd(path,
                          QFileInfo(_graphFileName).completeBaseName(),
                          fvf,
                          lvf,
                          _3DView->size().width(),
                          _3DView->size().height(),
                          "jpeg",
                          0,
                          100,
                          80,
                          this);    // Child.

    if (QDialog::Accepted == pbd.exec()) {
        // Execute playblast with parameters set in the dialog.

        playblast(pbd.path(),
                  pbd.fileName(),
                  pbd.firstFrame(),
                  pbd.lastFrame(),
                  pbd.width(),
                  pbd.height(),  
                  pbd.format(),
                  pbd.quality());
    }
}

void
NsMainWindow::_onToggleSelectedOpState()
{
    QString state = fromNbStr(NI_ACTIVE);
    foreach (const NsOpObject *op, NsOpStore::instance()->constSelectedOps()) {
        if (op->state() == fromNbStr(NI_ACTIVE)) {
            state = fromNbStr(NI_INACTIVE);
            break;
        }
    }

    NsCmdSetOpState::ArgsList argsList;
    foreach (const NsOpObject *op, NsOpStore::instance()->constSelectedOps()) {
        argsList +=
            NsCmdSetOpState::Args(op->longName(), state);
    }

    if (0 != undoStack()) {
        NsCmdSetOpState::exec(argsList, *undoStack());
    }
    else {
        NsCmdSetOpState::exec(argsList);
    }
}

void
NsMainWindow::_onToggleSelectedOpEnabled()
{
    bool enable = true;
    foreach (const NsOpObject *op, NsOpStore::instance()->constSelectedOps()) {
        if (op->hasParam("Enabled") && op->isEnabled()) {
            // If at least one of the selected Ops is enabled we are going
            // to disable them all. We also skip Ops that are not "enableable".

            enable = false;
            break;
        }
    }

    const QString enablePrefix("1 || ");
    const QString disablePrefix("0 && ");

    NsCmdSetParam::ArgsList argsList;
    foreach (const NsOpObject *op, NsOpStore::instance()->constSelectedOps()) {
        const NsValueBaseObject *vbo = op->queryConstValue(QString("Enabled"));
        if (0 != vbo) {
            const QString expr = vbo->expr(0);
            const bool exprEnablePrefix = expr.startsWith(enablePrefix);
            const bool exprDisablePrefix = expr.startsWith(disablePrefix);
            if (enable && !exprEnablePrefix) {
                // Enabling an Op that already has the enabled prefix is a 
                // no-op.

                QString newExpr = expr;
                if (exprDisablePrefix) {
                    newExpr.replace(0, enablePrefix.length(), enablePrefix);
                }
                else {
                    newExpr.prepend(enablePrefix);
                }
                argsList += NsCmdSetParam::Args(vbo->longName(), newExpr, 0);
            }
            else if (!enable && !exprDisablePrefix) {
                // Disabling an Op that already has the enabled prefix is a 
                // no-op.

                QString newExpr = expr;
                if (exprEnablePrefix) {
                    newExpr.replace(0, disablePrefix.length(), disablePrefix);
                }
                else {
                    newExpr.prepend(disablePrefix);
                }
                argsList += NsCmdSetParam::Args(vbo->longName(), newExpr, 0);
            }
        }
    }

    if (0 != undoStack()) {
        NsCmdSetParam::exec(argsList, *undoStack());
    }
    else {
        NsCmdSetParam::exec(argsList);
    }
}

// -----------------------------------------------------------------------------

// showFullScreen
// --------------
//! Shows the main window in full screen mode, or not.

void
NsMainWindow::_onShowFullScreen(bool fullScreen)
{
    if (fullScreen) {
        showFullScreen();
    }
    else {
        showNormal();
    }
}


// resetLayout
// -----------
//! Resets the layout of dock widgets and toolbars.

void
NsMainWindow::_onResetLayout()
{
    _readDefaultSettings();
}

// -----------------------------------------------------------------------------

// _about
// ------
//! Opens the about dialog.

void
NsMainWindow::_onAbout()
{
    NsAboutDialog about(this);
    about.exec();
}

// -----------------------------------------------------------------------------

// _onCleanChanged
// ---------------
//! Called when the undo stack's clean status is changed. Updates the modified
//! status of the main window and the state of the save action. [slot]

void
NsMainWindow::_onCleanChanged(const bool clean)
{
    setWindowModified(!clean);
    _saveGraphAction->setEnabled(!clean);
}


// _onClipboardChanged
// -------------------
//! Updates the enabled status of the Paste action. [slot]

void
NsMainWindow::_onClipboardChanged()
{
    _pasteAction->setEnabled(
        QApplication::clipboard()->mimeData()->hasFormat(
            "application/ni"));
}


void
NsMainWindow::_onConsoleClosed(NsConsole *console)
{
    delete console;
}

// _onOpSelectionChanged
// ---------------------
//! [slot]

void
NsMainWindow::_onOpSelectionChanged(const QStringList &opInstances,
                                    const bool         success)
{
    Q_UNUSED(opInstances);

    if (success) {
        // TODO: NsOpStore could have a function to determine if
        // there is at least one selected Op. This function would be able to
        // terminate as soon as a selected Op was found.

        QList<const NsOpObject*> selOps = 
            NsOpStore::instance()->constSelectedOps();

        _cutAction->setEnabled(!selOps.isEmpty());
        _copyAction->setEnabled(!selOps.isEmpty());
        _eraseAction->setEnabled(!selOps.isEmpty());

        _graphSelectedOpStateToggleAction->setEnabled(!selOps.isEmpty());
        _graphSelectedOpEnabledToggleAction->setEnabled(!selOps.isEmpty());
    }
}


// _onFeedSelectionChanged
// -----------------------
//! [slot]

void
NsMainWindow::_onFeedSelectionChanged(const QStringList &inputLongNames,
                                      const bool         success)
{
    Q_UNUSED(inputLongNames);

    if (success) {
        QList<const NsInputPlugObject *> selInputs(
            NsOpStore::instance()->constSelectedInputs(true));

        _eraseAction->setEnabled(!selInputs.empty());
    }
}


// -----------------------------------------------------------------------------

// _createActions
// --------------
//! Creates the main window's actions.

void
NsMainWindow::_createActions()
{
    // File menu actions.

    _newGraphAction = new QAction(tr("New Graph"), this); // Child.
    _newGraphAction->setShortcut(QKeySequence::New);
    _newGraphAction->setStatusTip(
        tr("Close the current graph and create a new empty graph"));
    connect(_newGraphAction, SIGNAL(triggered()), SLOT(_onNewGraph()));

    _openGraphAction = new QAction(tr("Open Graph..."), this); // Child.
    _openGraphAction->setShortcut(QKeySequence::Open);
    _openGraphAction->setStatusTip(
        tr("Open a graph from an existing Naiad Interface file"));
    connect(_openGraphAction, SIGNAL(triggered()), SLOT(_onOpenGraph()));

    _saveGraphAction = new QAction(tr("Save Graph"), this); // Child.
    _saveGraphAction->setShortcut(QKeySequence::Save);
    _saveGraphAction->setStatusTip(tr("Save the current graph"));
    _saveGraphAction->setEnabled(false);
    connect(_saveGraphAction, SIGNAL(triggered()), SLOT(_onSaveGraph()));

    _saveGraphAsAction = new QAction(tr("Save Graph As..."), this); // Child.
    _saveGraphAsAction->setShortcut(QKeySequence::SaveAs);
    _saveGraphAsAction->setStatusTip(
        tr("Save the current graph with a specified file name"));
    connect(_saveGraphAsAction, SIGNAL(triggered()), SLOT(_onSaveGraphAs()));

    _exportGraphAction = new QAction(tr("Export Graph..."), this); // Child.
    //_exportGraphAction->setShortcut(QKeySequence("Ctrl+E"));
    _exportGraphAction->setStatusTip(tr("Export the current graph"));
    connect(_exportGraphAction, SIGNAL(triggered()), SLOT(_onExportGraph()));

    _cmdLineAction = new QAction(tr("Launch Naiad"), this);  // Child.
    _cmdLineAction->setStatusTip(tr(""));
    _cmdLineAction->setCheckable(false);
    _cmdLineAction->setIcon(QIcon(":/images/launch.png"));
    connect(_cmdLineAction, SIGNAL(triggered()), SLOT(_onCmdLine()));

    _deadlineAction = new QAction(tr("Submit to Deadline"), this);  // Child.
    _deadlineAction->setStatusTip(tr("Submit the current graph to Deadline"));
    connect(_deadlineAction, SIGNAL(triggered()), SLOT(_onSubmitToDeadline()));

    _quitAction = new QAction(tr("Quit"), this); // Child.
    _quitAction->setShortcut(QKeySequence::Quit);   // TODO: "Ctrl+Q"?
    _quitAction->setStatusTip(tr("Quit Naiad Studio"));
    connect(_quitAction, SIGNAL(triggered()), SLOT(close()));


    // Edit menu actions.

    // Children.
    _undoAction = undoStack()->createUndoAction(this, 
                                                QKeySequence::Undo); // Child.
    _redoAction = undoStack()->createRedoAction(this, 
                                                QKeySequence::Redo); // Child.

    _cutAction = new QAction(tr("Cut"), this);  // Child.
    _cutAction->setStatusTip("Cut selection");
    _cutAction->setShortcuts(QKeySequence::Cut);
    connect(_cutAction, SIGNAL(triggered()), SLOT(_onCutSelection()));

    _copyAction = new QAction(tr("Copy"), this);     // Child.
    _copyAction->setStatusTip("Copy selection");
    _copyAction->setShortcuts(QKeySequence::Copy);
    connect(_copyAction, SIGNAL(triggered()), SLOT(_onCopySelection()));

    _pasteAction = new QAction(tr("Paste"), this);   // Child.
    _pasteAction->setStatusTip("Paste");
    _pasteAction->setShortcuts(QKeySequence::Paste);
    connect(_pasteAction, SIGNAL(triggered()), SLOT(_onPaste()));
    _onClipboardChanged();

    QList<QKeySequence> eraseShortcuts;
    eraseShortcuts << QKeySequence::Delete << Qt::Key_Backspace;
    _eraseAction = new QAction(tr("Erase"), this);    // Child.
    _eraseAction->setStatusTip("Erase selection");
    _eraseAction->setShortcuts(eraseShortcuts);
    connect(_eraseAction, SIGNAL(triggered()), SLOT(_onEraseSelection()));

    _frameAction = new NsHoverAction(this); // Child.
    _frameAction->setShortcut(Qt::Key_F);
    _frameAction->addAction(_graphView->viewport(),
                            _graphView->createFrameSelectedGraphAction(this));
    _frameAction->addAction(_3DView,
                            _3DView->frameSceneAction());

    // We don't want the frame-action in any menu, so we add it to the main
    // window instead.

    addAction(_frameAction);

    _selectAllAction =
        new NsSelectAction(tr("Select All"), true, this); //Child.
    _selectAllAction->setStatusTip(tr("Select everything"));
    _selectAllAction->setShortcuts(QKeySequence::SelectAll);
    connect(_selectAllAction, SIGNAL(select(bool)),
            this,             SLOT(_onSelectAll(bool)));

    _selectAllOpsAction =
        new NsSelectAction(tr("Select All &Ops"), true, this); //Child.
    _selectAllOpsAction->setStatusTip(tr("Select all Ops"));
    connect(_selectAllOpsAction, SIGNAL(select(bool)),
            this,                SLOT(_onSelectAllOps(bool)));

    _selectAllBodiesAction =
        new NsSelectAction(tr("Select All &Bodies"), true, this);
    _selectAllBodiesAction->setStatusTip(tr("Select all Bodies"));
    connect(_selectAllBodiesAction, SIGNAL(select(bool)),
            this,                   SLOT(_onSelectAllBodies(bool)));

    _selectAllFeedsAction =
        new NsSelectAction(tr("Select All &Feeds"), true, this); // Child.
    _selectAllFeedsAction->setStatusTip(tr("Select all Feeds"));
    connect(_selectAllFeedsAction, SIGNAL(select(bool)),
            this,                  SLOT(_onSelectAllFeeds(bool)));

    _unselectAllAction =
        new NsSelectAction(tr("Unselect All"), false, this); // Child.
    _unselectAllAction->setStatusTip(tr("Unselect everything"));
    connect(_unselectAllAction, SIGNAL(select(bool)),
            this,               SLOT(_onSelectAll(bool)));

    _unselectAllOpsAction =
        new NsSelectAction(tr("Unselect All &Ops"), false, this); // Child.
    _unselectAllOpsAction->setStatusTip(tr("Unselect all Ops"));
    connect(_unselectAllOpsAction, SIGNAL(select(bool)),
            this,                  SLOT(_onSelectAllOps(bool)));

    _unselectAllBodiesAction =
        new NsSelectAction(tr("Unselect All &Bodies"), false, this); // Child.
    _unselectAllBodiesAction->setStatusTip(tr("Unselect all Bodies"));
    connect(_unselectAllBodiesAction, SIGNAL(select(bool)),
            this,                     SLOT(_onSelectAllBodies(bool)));

    _unselectAllFeedsAction =
        new NsSelectAction(tr("Unselect All &Feeds"), false, this); // Child.
    _unselectAllFeedsAction->setStatusTip(tr("Unselect all Feeds"));
    connect(_unselectAllFeedsAction, SIGNAL(select(bool)),
            this,                    SLOT(_onSelectAllFeeds(bool)));

    //    _editMenu->addAction(_graphView->createCutAction(this));
    //    _editMenu->addAction(_graphView->createCopyAction(this));
    //    _editMenu->addAction(_graphView->createPasteAction(this));
    //    _editMenu->addAction(_graphView->createDeleteAction(this));


    _preferencesAction = new QAction(tr("Preferences..."), this); // Child.
    _preferencesAction->setStatusTip(tr("Open preferences dialog"));
    connect(_preferencesAction, SIGNAL(triggered()),
            this,               SLOT(_onPreferences()));


    // 3D View actions.

    _drawOriginGridAction = _3DView->drawOriginGridAction();
    _drawHudAction = _3DView->drawHudAction();
    _drawAxesAction = _3DView->drawAxesAction();
    //_3DViewFrameAction = _3DView->frameSceneAction();

    _playblastAction = _3DView->playblastAction();
    _playblastAction->setStatusTip(tr("Export frame range as image sequence"));
    connect(_playblastAction, SIGNAL(triggered()),
            this,             SLOT(_onPlayblast()));


    //    _playblastAction = new QAction(tr("&Playblast..."), this);
    //    _playblastAction->setStatusTip(tr("Export frame range as image sequence"));
    //    connect(_playblastAction, SIGNAL(triggered()), SLOT(_playblast()));


    // Graph actions.

    _graphZoomInAction = new NsFocusAction("Zoom In", this);    // Child.
    _graphZoomInAction->setStatusTip("Zoom In");
    _graphZoomInAction->setShortcut(QKeySequence::ZoomIn);
    _graphZoomInAction->addAction(_graphView,
                                  _graphView->createZoomInAction(this));

    _graphZoomOutAction = new NsFocusAction("Zoom Out", this);    // Child.
    _graphZoomOutAction->setStatusTip("Zoom Out");
    _graphZoomOutAction->setShortcut(QKeySequence::ZoomOut);
    _graphZoomOutAction->addAction(_graphView,
                                   _graphView->createZoomOutAction(this));

//    _graphFrameAction = new NsFocusAction(tr("&Frame Graph"), this); // Child.
//    _graphFrameAction->setStatusTip(tr("Frame Graph View"));
//    _graphFrameAction->setShortcut(Qt::Key_F);
//    _graphFrameAction->addAction(_graphView,
//                                 _graphView->createFrameGraphAction(this));

    for (int zoomScale(20); zoomScale <= 200; zoomScale += 20) {
        _graphZoomActions.append(_graphView->createZoomAction(this, zoomScale));
    }

    _graphSelectedOpStateToggleAction =
        new QAction(tr("Activate/Deactivate Ops"), this);   // Child.
    _graphSelectedOpStateToggleAction->setStatusTip(
        tr("Toggle the state of selected operators."));
    _graphSelectedOpStateToggleAction->setShortcut(Qt::Key_I);
    connect(_graphSelectedOpStateToggleAction, SIGNAL(triggered()),
            this,                            SLOT(_onToggleSelectedOpState()));

    _graphSelectedOpEnabledToggleAction = 
        new QAction(tr("Enable/Disable Ops"), this);        // Child.
    _graphSelectedOpEnabledToggleAction->setStatusTip(
        tr("Toggle the enabled parameter of selected operators."));
    _graphSelectedOpEnabledToggleAction->setShortcut(Qt::Key_D);
    connect(_graphSelectedOpEnabledToggleAction, SIGNAL(triggered()),
        this,                                 SLOT(_onToggleSelectedOpEnabled()));


    // Time actions.

    //_playFwdAction = _timeToolBar->playFwdAction();
    //_playBwdAction = _timeToolBar->playBwdAction();
    //_loopAction = _timeToolBar->loopAction();
    //_stepBwdAction = _timeToolBar->previousFrameAction();
    //_stepFwdAction = _timeToolBar->nextFrameAction();
    //_firstFrameAction = _timeToolBar->firstVisibleFrameAction();
    //_lastFrameAction = _timeToolBar->lastVisibleFrameAction();
    //_resetAction = _timeToolBar->resetAction();
    //_stepToLastAction = _timeToolBar->stepToLastAction();
    //_stepSingleAction = _timeToolBar->stepSingleAction();
    //_restepAction = _timeToolBar->restepAction();

    // Window actions.

    _fullScreenAction = new QAction(tr("Full Screen"), this); // Child.
    _fullScreenAction->setStatusTip(tr("Toggle full screen mode"));
    _fullScreenAction->setCheckable(true);
    _fullScreenAction->setChecked(isFullScreen());
    connect(_fullScreenAction, SIGNAL(toggled(bool)),
            this,              SLOT(_onShowFullScreen(bool)));

    _resetLayoutAction = new QAction(tr("&Reset Layout"), this); // Child.
    _resetLayoutAction->setStatusTip(tr("Reset toolbars and dock windows"));
    connect(_resetLayoutAction, SIGNAL(triggered()), SLOT(_onResetLayout()));


    // Help actions.

    _aboutAction = new QAction(tr("About Naiad Studio"), this); // Child.
    _aboutAction->setStatusTip(tr("Show information about Naiad Studio"));
    connect(_aboutAction, SIGNAL(triggered()), SLOT(_onAbout()));

    _onOpSelectionChanged(QStringList(), true); // Update cut/copy/erase actions.
}


// _createMenus
// ------------
//! Creates the main window's menus. Assumes that actions have been created
//! already.

void
NsMainWindow::_createMenus()
{
    // File menu.

    _fileMenu = menuBar()->addMenu(tr("&File"));
    _fileMenu->setObjectName("File menu");
    _fileMenu->addAction(_newGraphAction);
    _fileMenu->addAction(_openGraphAction);
    _fileRecentGraphsMenu = _fileMenu->addMenu(tr("&Recent Graphs"));
    _fileRecentGraphsMenu->setObjectName("Recent graphs");
    _fileMenu->addSeparator();
    _fileMenu->addAction(_saveGraphAction);
    _fileMenu->addAction(_saveGraphAsAction);
    _fileMenu->addAction(_exportGraphAction);
    _fileMenu->addSeparator();
    _fileMenu->addAction(_cmdLineAction);
    _fileMenu->addSeparator();
//    _fileMenu->addAction(_deadlineAction);
//    _fileMenu->addSeparator();
    _fileMenu->addAction(_quitAction);


    // Edit menu.

    _editMenu = menuBar()->addMenu(tr("&Edit"));
    _editMenu->addAction(_undoAction);
    _editMenu->addAction(_redoAction);
    _editMenu->addSeparator();
    _editMenu->addAction(_cutAction);
    _editMenu->addAction(_copyAction);
    _editMenu->addAction(_pasteAction);
    _editMenu->addAction(_eraseAction);
    //_editMenu->addSeparator();
    //_editMenu->addAction(_frameAction); // Invisible.
    _editMenu->addSeparator();
    _editMenu->addAction(_selectAllAction);
    _editMenu->addAction(_selectAllOpsAction);
    _editMenu->addAction(_selectAllBodiesAction);
    _editMenu->addAction(_selectAllFeedsAction);
    _editMenu->addSeparator();
    _editMenu->addAction(_unselectAllAction);
    _editMenu->addAction(_unselectAllOpsAction);
    _editMenu->addAction(_unselectAllBodiesAction);
    _editMenu->addAction(_unselectAllFeedsAction);
    _editMenu->addSeparator();
    _editMenu->addAction(_preferencesAction);


    //centralWidget()
    //this->addAction(_frameAction);

    // 3D View menu.

    _3DViewMenu = menuBar()->addMenu(tr("&3D View"));
    _3DViewMenu->addAction(_drawOriginGridAction);
    _3DViewMenu->addAction(_drawHudAction);
    _3DViewMenu->addAction(_drawAxesAction);
    _3DViewMenu->addAction(_3DView->drawItemLabelsAction());
    _3DViewMenu->addAction(_3DView->drawBodyLabelsAction());
    _3DViewMenu->addSeparator();
    _3DViewMenu->addAction(_3DView->nullManipAction());
    _3DViewMenu->addAction(_3DView->translateManipAction());
    _3DViewMenu->addAction(_3DView->rotateManipAction());
    _3DViewMenu->addAction(_3DView->scaleManipAction());

    //_3DViewMenu->addSeparator();
    //_3DViewMenu->addAction(_3DViewFrameAction);
    _3DViewMenu->addSeparator();
    _3DViewMenu->addAction(_playblastAction);
    _3DViewMenu->addAction(_3DView->stepPlayblastAction());


    // Graph menu.

    _graphMenu = menuBar()->addMenu(tr("&Graph"));
    _graphMenu->addAction(_graphView->createFrameAllGraphAction(this));
    _graphMenu->addAction(_graphZoomInAction);
    _graphMenu->addAction(_graphZoomOutAction);
    _graphZoomMenu = _graphMenu->addMenu(tr("Set &Zoom"));
    _graphZoomMenu->addActions(_graphZoomActions);
    //_graphMenu->addAction(_graphFrameAction);
    _graphMenu->addSeparator();
    _graphMenu->addAction(_graphSelectedOpStateToggleAction);
    _graphMenu->addAction(_graphSelectedOpEnabledToggleAction);



    // Time menu.

    _timeMenu = menuBar()->addMenu(tr("&Time"));
    _timeMenu->addAction(_timeToolBar->playFwdAction());
    _timeMenu->addAction(_timeToolBar->playBwdAction());
    _timeMenu->addAction(_timeToolBar->loopAction());
    _timeMenu->addSeparator();
    _timeMenu->addAction(_timeToolBar->previousFrameAction());
    _timeMenu->addAction(_timeToolBar->nextFrameAction());
    _timeMenu->addAction(_timeToolBar->firstVisibleFrameAction());
    _timeMenu->addAction(_timeToolBar->lastVisibleFrameAction());
    _timeMenu->addSeparator();
    _timeMenu->addAction(_timeToolBar->resetAction());
    _timeMenu->addAction(_timeToolBar->stepSingleAction());
    _timeMenu->addAction(_timeToolBar->restepAction());
    _timeMenu->addAction(_timeToolBar->stepVisibleAction());
    _timeMenu->addAction(_timeToolBar->stepToLastAction());
    _timeMenu->addAction(_timeToolBar->stopAction());


    // Window menu.

    _windowMenu = createPopupMenu();
    _windowMenu->insertSeparator(_windowMenu->actions().first());
    _windowMenu->insertAction(_windowMenu->actions().first(),
                              _fullScreenAction);
    _windowMenu->addSeparator();
    _windowMenu->addAction(_resetLayoutAction);
    _windowMenu->setTitle(tr("&Window"));
    menuBar()->addMenu(_windowMenu);

    menuBar()->addSeparator();


    // Help menu.

    _helpMenu = menuBar()->addMenu(tr("&Help"));
    _helpMenu->addAction(_aboutAction);
}


// _createToolBars
// ---------------
//! Creates the main window's tool bars.

void
NsMainWindow::_createToolBars()
{
    _timeToolBar = new NsTimeToolBar(_cmdLineAction, undoStack(), this);
    _timeToolBar->setObjectName("Time Toolbar");
    _timeToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

    // Connect to command central.

    connect(NsCmdCentral::instance(),
            SIGNAL(valueChanged(QString,QString,int,bool)),
            _timeToolBar,
            SLOT(onValueChanged(QString,QString,int,bool)));

    connect(NsCmdCentral::instance(),
            SIGNAL(metaChanged(QString,QString,QString,bool)),
            _timeToolBar,
            SLOT(onMetaChanged(QString,QString,QString,bool)));

    connect(NsCmdCentral::instance(),
            SIGNAL(firstVisibleFrameChanged(int,bool)),
            _timeToolBar,
            SLOT(onFirstVisibleFrameChanged(int,bool)));

    connect(NsCmdCentral::instance(),
            SIGNAL(lastVisibleFrameChanged(int,bool)),
            _timeToolBar,
            SLOT(onLastVisibleFrameChanged(int,bool)));

    connect(NsCmdCentral::instance(),
            SIGNAL(currentVisibleFrameChanged(int,bool,bool)),
            _timeToolBar,
            SLOT(onCurrentVisibleFrameChanged(int,bool,bool)));

    addToolBar(Qt::BottomToolBarArea, _timeToolBar);
}


// _createStatusBar
// ----------------
//! Creates the main window's status bar.

void NsMainWindow::_createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}


// _createDockWidgets
// ------------------
//! Creates the main window's dock widgets.

void
NsMainWindow::_createDockWidgets()
{
    // Create dock widgets and add them to the main window.
    // NB: Dock widgets should be parented by the main window.

    // 3D View.

    NsDockWidget *dock3DView(
        _createDockWidget(
            "Scope View", 
            this, 
            0, 
            _3DView,
            QDockWidget::DockWidgetClosable |
            QDockWidget::DockWidgetMovable  |
            QDockWidget::DockWidgetFloatable,
            Qt::AllDockWidgetAreas));
    addDockWidget(Qt::LeftDockWidgetArea, dock3DView);


    // Help View.

    QMainWindow* helpMain(new QMainWindow);  // TODO: memory leak?
    QToolBar* toolBar(helpMain->addToolBar(tr("Navigation")));
    toolBar->addAction(_helpView->pageAction(QWebPage::Back));
    toolBar->addAction(_helpView->pageAction(QWebPage::Forward));
    toolBar->addAction(_helpView->pageAction(QWebPage::Reload));
    toolBar->addAction(_helpView->pageAction(QWebPage::Stop));
    helpMain->setCentralWidget(_helpView);

    NsDockWidget *dockHelpView(
        _createDockWidget(
            "Help View", 
            this, 
            0, 
            helpMain,
            QDockWidget::DockWidgetClosable |
            QDockWidget::DockWidgetMovable  |
            QDockWidget::DockWidgetFloatable,
            Qt::AllDockWidgetAreas));
    addDockWidget(Qt::LeftDockWidgetArea, dockHelpView);

    tabifyDockWidget(dockHelpView, dock3DView); // Place 3D View on top.

       

    // Value Editor.

    QScrollArea *scrollArea(new QScrollArea);   // TODO: memory leak?
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidget(_valueEditor); // Takes ownership.

    NsDockWidget *dockValueEditor(
        _createDockWidget(
            "Value Editor", 
            this, 
            0, 
            scrollArea,//_valueEditor,
            QDockWidget::DockWidgetClosable |
            QDockWidget::DockWidgetMovable  |
            QDockWidget::DockWidgetFloatable,
            Qt::LeftDockWidgetArea | 
            Qt::RightDockWidgetArea));
    //dockValueEditor->setMinimumWidth(400);
    addDockWidget(Qt::RightDockWidgetArea, dockValueEditor);


    // Message view.

    NsDockWidget *dockMsg(
        _createDockWidget(
            "Messages", 
            this, 
            0, 
            NsMessageWidget::instance(),
            QDockWidget::DockWidgetClosable |
            QDockWidget::DockWidgetMovable  |
            QDockWidget::DockWidgetFloatable,
            Qt::AllDockWidgetAreas));
    addDockWidget(Qt::BottomDockWidgetArea, dockMsg);

    // Undo view.

    NsDockWidget *dockHistory(
        _createDockWidget(
            "History", 
            this, 
            0, 
            _undoView,
            QDockWidget::DockWidgetClosable |
            QDockWidget::DockWidgetMovable  |
            QDockWidget::DockWidgetFloatable,
            Qt::AllDockWidgetAreas));
    addDockWidget(Qt::BottomDockWidgetArea, dockHistory);

    tabifyDockWidget(dockHistory, dockMsg);
}

// -----------------------------------------------------------------------------

// _addRecentGraphAction
// ---------------------
//! Adds an action with the given name to the top of the recent Graphs menu,
//! or moves it to the top if it's already in the menu.

void
NsMainWindow::_addRecentGraphAction(const QString &graphFileName)
{
    //  Never add empty file names.

    if (!graphFileName.isEmpty()) {
        // Move file name to beginning of recent files menu
        // if not already there.

        QList<QAction*> actions(_fileRecentGraphsMenu->actions());
        const int numActions(actions.size());
        foreach (QAction *action, actions) {
            if (action->text() == graphFileName) {
                // An existing action matches the provided string.

                if (numActions > 1 && action != actions.first()) {
                    // The matching action is not the first in the list.
                    // (1) - Remove action from non-first position.
                    // (2) - Insert action at first position in the list.

                    _fileRecentGraphsMenu->removeAction(action);
                    _fileRecentGraphsMenu->insertAction(actions.first(), action);
                }
                return; // Early exit, work is done.
            }
        }

        // The provided string does not exist in the menu, create new action
        // and add as the first item in the menu.

        if (actions.isEmpty()) {
            // Add action to empty list.

            _fileRecentGraphsMenu->addAction(
                _createRecentGraphAction(graphFileName));
        }
        else {
            // Insert before first action.

            _fileRecentGraphsMenu->insertAction(
                actions.first(),
                _createRecentGraphAction(graphFileName));
        }

        // Remove last actions, if too many.

        while (_fileRecentGraphsMenu->actions().size() > _maxRecentGraphs) {
            _fileRecentGraphsMenu->removeAction(
                _fileRecentGraphsMenu->actions().last());
        }
    }
}


// _createRecentGraphAction
// ------------------------
//! Create an action that can be added to the recent Graphs menu.
//! Assumes that the provided file name is not empty.

QAction*
NsMainWindow::_createRecentGraphAction(const QString &graphFileName)
{
    // The action's text corresponds to the file that the action will open.

    QAction *action(new QAction(graphFileName, this));  // Child.
    action->setStatusTip(tr("Open the graph ") + action->text());
    connect(action, SIGNAL(triggered()), SLOT(_onOpenRecent()));
    return action;
}

// -----------------------------------------------------------------------------

// _readSettings
// -------------
//! Reads the main window settings.

void
NsMainWindow::_readSettings()
{
    QSettings settings;
    settings.beginGroup(_settingsGroup);

    restoreGeometry(settings.value("Geometry").toByteArray());
    restoreState(settings.value("State").toByteArray());
    //filePath = settings.value("FilePath", QDir::currentPath()).toString();

    // Insert recent graphs appears in reverse order.

    const QStringList recentGraphs(
        settings.value("RecentGraphs").toStringList());

    for (int i(recentGraphs.size() - 1); i >= 0; --i) {
        _addRecentGraphAction(recentGraphs[i]);
    }

    settings.endGroup();
}


// _readDefaultSettings
// --------------------
//! Reads the main window default settings.

void
NsMainWindow::_readDefaultSettings()
{
    QSettings settings;
    settings.beginGroup(_settingsGroup);
    restoreGeometry(settings.value("DefaultGeometry").toByteArray());
    restoreState(settings.value("DefaultState").toByteArray());
    settings.endGroup();
}


// _writeSettings
// --------------
//! Writes the main window settings.

void
NsMainWindow::_writeSettings()
{
    QSettings settings;
    settings.beginGroup(_settingsGroup);

    settings.setValue("Geometry", saveGeometry());
    settings.setValue("State", saveState());

    QStringList recentGraphs;
    QList<QAction *> actions = _fileRecentGraphsMenu->actions();
    foreach (QAction *action, actions) {
        recentGraphs += action->text();
    }

    settings.setValue("RecentGraphs", recentGraphs);

    settings.endGroup();
}


// _writeDefaultSettings
// ---------------------
//! Writes the main window default settings which are used by the
//! Reset Layout action.

void
NsMainWindow::_writeDefaultSettings()
{
    QSettings settings;
    settings.beginGroup(_settingsGroup);
    settings.setValue("DefaultGeometry", saveGeometry());
    settings.setValue("DefaultState", saveState());
    settings.endGroup();
}

// -----------------------------------------------------------------------------

void
NsMainWindow::_newGraph(const QString &emptyLabel, 
                        const bool     clearGraph,
                        const bool     createCamera)
{
    if (clearGraph) {
        qDebug() << "New Graph - unselect all";
        NsCmdSelectAll::exec(false);

        qDebug() << "New Graph - clear graph";
        NsCmdClearGraph::exec();
    }

    _undoView->setEmptyLabel(emptyLabel);
    _setFirstVisibleFrame();
    _setLastVisibleFrame();
    _setCurrentVisibleFrame();

    if (createCamera && _3DView->isValid()) {    
        qDebug() << "New Graph - create camera";
        QString createdOpInstance;
        NsCmdCreate::exec("Camera-Scope", "Camera-Scope", &createdOpInstance);
        NsCmdSetOpPosition::exec(
            NsCmdSetOpPosition::ArgsList() <<
                NsCmdSetOpPosition::Args(createdOpInstance, 0., 100.));
        NsCmdSetOpState::exec(
            NsCmdSetOpState::ArgsList() <<
                NsCmdSetOpState::Args(createdOpInstance, fromNbStr(NI_ACTIVE)));
    }

    NsCmdFrameGraphView::exec(false, 100);
}


// _saveGraph
// ----------
//! Make a backup copy of the existing file and write a new file.

bool
NsMainWindow::_saveGraph(const QString &fileName, const QString &format)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    bool saved(false);

    const QString msg0(tr("Saving graph on '") + fileName + "'...");
    NsMessageWidget::instance()->clientInfo(msg0);
    statusBar()->showMessage(msg0);

    // Back up existing Graph file.

    const QString backupFileName(_backupFileName(fileName));

    QFile saveFile(fileName);
    QFile backupFile(backupFileName);

    // Remove existing backup file.

    if (saveFile.exists() && backupFile.exists()) {
        if (!backupFile.remove()) {
            NsMessageWidget::instance()->clientWarning(
                tr("Failed to remove existing backup file") +
                   "'" + backupFileName + "'");
        }
    }

    // Copy existing file to backup file.

    if (saveFile.exists() && !backupFile.exists()) {
        if (!saveFile.copy(backupFileName)) {
            NsMessageWidget::instance()->clientWarning(
                tr("Failed to copy existing file to backup file") +
                   "'" + backupFileName + "'");
        }
    }

    NsCmdSelectAll::exec(false);

    // Write Graph file, assume success.

    const QString msg("This " + format + " was generated by Naiad Studio");

    NiExportGraph(fromQStr(QFileInfo(fileName).absoluteFilePath()),
                  fromQStr(msg),
                  fromQStr(format));
    saved = true;

    NsMessageWidget::instance()->clientInfo(
        tr("Successfully saved graph as") + "'" + fileName + "'");

    _undoStack->setClean();

    if (".ni" == format) {
        _setGraphFileName(fileName);
    }

    statusBar()->clearMessage();
    QApplication::restoreOverrideCursor();

    return saved;
}


// _saveGraphIfModified
// --------------------
//! Let's the user save the graph if it has been modified.
//! Returns false if the user presses Cancel.

bool
NsMainWindow::_saveGraphIfModified()
{
    if (isWindowModified()) {
        const QMessageBox::StandardButton ans =
            QMessageBox::warning(
                this,
                tr("Warning - Unsaved changes"),
                tr("The current graph has been modified\n"
                   "since it was last saved on disk.\n"
                   "Do you want to save your changes?"),
                QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel);

        switch (ans) {
        case QMessageBox::Save:
            return _onSaveGraph();
        case QMessageBox::Discard:
            return true;
        default:
        case QMessageBox::Cancel:
            return false;   // Modified but not saved.
        }
    }

    return true;
}


// _openGraph
// ----------
//! Opens the Graph on the given file.

void
NsMainWindow::_openGraph(const QString &fileName)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    const QString msg0(tr("Opening graph from") + "'" + fileName + "'...");
    NsMessageWidget::instance()->clientInfo(msg0);
    statusBar()->showMessage(msg0);

    QFileInfo info(fileName);

    _newGraph(tr("Open Graph"), true, false);
    _setGraphFileName(fileName);
    _addRecentGraphAction(_graphFileName);

    setUpdatesEnabled(false);   
    _graphView->setUpdatesEnabled(false);
    _graphView->viewport()->setUpdatesEnabled(false);
    //_graphView->setScene(0);
    //_graphScene->blockSignals(true);
    //_graphScene->setBspTreeDepth(1);
    //_graphScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    _3DView->setUpdatesEnabled(false);

    NsParserCallback pcb(false);
    NiParseFile(fromQStr(info.absoluteFilePath()), &pcb);

    _3DView->setUpdatesEnabled(true);
    //_graphScene->blockSignals(false);
    //_graphView->setScene(_graphScene);
    _graphView->viewport()->setUpdatesEnabled(true);
    _graphView->setUpdatesEnabled(true);
    setUpdatesEnabled(true);

    NsCmdFrameGraphView::exec(false, 20);

    statusBar()->clearMessage();
    NsMessageWidget::instance()->clientInfo(
        tr("Opened graph") +"'"+ fileName +"'");
    QApplication::restoreOverrideCursor();
}

// -----------------------------------------------------------------------------

//// _getOpenFileName
//// ----------------
////! Opens a file dialog with the given caption and returns the chosen file.
//
//QString
//NsMainWindow::_getOpenFileName(const QString &caption, const QString &path)
//{
//    const QString fileName(
//        QFileDialog::getOpenFileName(
//            this,
//            caption,
//            path,
//            tr("Naiad Interface Files (*.ni);;All Files (*.*)")));
//
//    return fileName;
//}


// _getSaveFileName
// ----------------
//! Opens a file dialog with the given caption and returns the chosen file.

QString
NsMainWindow::_getSaveFileName(const QString &caption, const QString &path)
{
    QString fileName(
        NsPath::getSaveFileName(
            this,
            caption,
            path,
            tr("Naiad Interface Files (*.ni);;All Files (*.*)")));

    if (!fileName.isEmpty() && QFileInfo(fileName).suffix().isEmpty()) {
        // Add default suffix if necessary.

        fileName += _defaultGraphFileNameSuffix;
    }

    return fileName;
}


// _getExportFileName
// ------------------
//! Opens a file dialog with the given caption and returns the chosen file.

QString
NsMainWindow::_getExportFileName(const QString &caption,
                                 const QString &path,
                                 QString       &format)
{
    QString selectedFilter;
    QString fileName(
        NsPath::getSaveFileName(
            this,
            caption,
            path,
            tr("Naiad Python Script (*.py);;Naiad Interface File (*.ni)"),
            &selectedFilter));

    if ("Naiad Python Script (*.py)" == selectedFilter) {
        format = ".py";
    }
    else if ("Naiad Interface File (*.ni)" == selectedFilter) {
        format = ".ni";
    }
    else {
        format = _defaultGraphFileNameSuffix;
    }

    if (!fileName.isEmpty() && QFileInfo(fileName).suffix().isEmpty()) {
        fileName += format;
    }

    return fileName;
}


// _setGraphFileName
// -----------------
//! Set the current graph file name.

void
NsMainWindow::_setGraphFileName(const QString &fileName)
{
    _graphFileName = fileName;

    setWindowTitle(
        "Naiad Studio "
#ifdef DEBUG
        "[DEBUG] "
#endif  // DEBUG
        "(" + queryVersionString() + ")"
        " - " + _graphFileName + "[*]");
}

// -----------------------------------------------------------------------------

//! Default graph file name. [static]
const QString NsMainWindow::_defaultGraphFileNameBase("untitled");

//! Default format. [static]
const QString NsMainWindow::_defaultGraphFileNameSuffix(".ni");

//! Name of settings for main windows. [static]
const QString NsMainWindow::_settingsGroup("NsMainWindow");

//! [static]
const int NsMainWindow::_defaultFirstVisibleFrame(1);

//! [static]
const int NsMainWindow::_defaultLastVisibleFrame(100);

// -----------------------------------------------------------------------------

// _defaultGraphFileName
// ---------------------
//! Returns the default graph file name. [static]

QString
NsMainWindow::_defaultGraphFileName()
{
    return _defaultGraphFileNameBase + _defaultGraphFileNameSuffix;
}


// _backupFileName
// ---------------
//! Returns a suitable backup file name. [static]

QString
NsMainWindow::_backupFileName(const QString &fileName)
{
    return fileName + "~";
}


// _setFirstVisibleFrame
// ---------------------
//! [static]

void
NsMainWindow::_setFirstVisibleFrame()
{
    const QString metaValue(
        queryMeta("Global", NsTimeToolBar::firstVisibleFrameMetaValueType()));

    bool ok(false);
    const int fvf(metaValue.toInt(&ok));

    if (ok) {
        NsCmdSetFirstVisibleFrame::exec(fvf);
    }
    else {
        NsCmdSetFirstVisibleFrame::exec(_defaultFirstVisibleFrame);
    }
}


// _setLastVisibleFrame
// --------------------
//! [static]

void
NsMainWindow::_setLastVisibleFrame()
{
    const QString metaValue(
        queryMeta("Global", NsTimeToolBar::lastVisibleFrameMetaValueType()));

    bool ok(false);
    const int lvf(metaValue.toInt(&ok));

    if (ok) {
        NsCmdSetLastVisibleFrame::exec(lvf);
    }
    else {
        NsCmdSetLastVisibleFrame::exec(_defaultLastVisibleFrame);
    }
}


// _setCurrentVisibleFrame
// -----------------------
//! [static]

void
NsMainWindow::_setCurrentVisibleFrame()
{
    const QString metaValue(
        queryMeta("Global", NsTimeToolBar::currentVisibleFrameMetaValueType()));

    bool ok(false);
    const int cvf(metaValue.toInt((&ok)));

    if (ok) {
        NsCmdSetCurrentVisibleFrame::exec(cvf);
    }
    else {
        NsCmdSetCurrentVisibleFrame::exec(_defaultFirstVisibleFrame);
    }
}

// -----------------------------------------------------------------------------

NsUndoStack*
NsMainWindow::_createUndoStack(QObject *parent)
{
    NsUndoStack *stack(new NsUndoStack(parent));    // Child.
    
    connect(NsCmdCentral::instance(), SIGNAL(graphCleared(bool)), 
            stack,                    SLOT(onGraphCleared(bool)));

    return stack;
}

NsUndoView*
NsMainWindow::_createUndoView(NsUndoStack *undoStack, QWidget *parent)
{
    NsUndoView *view(new NsUndoView(undoStack, parent));    // Child.
    return view;
}


NsGraphScene*
NsMainWindow::_createGraphScene(NsUndoStack *undoStack, QObject *parent)
{
    // Create graph scene.

    NsGraphScene *scene(new NsGraphScene(undoStack, parent)); // Child.
    scene->setSceneRect(-1e9, -1e9, 2*1e9, 2*1e9);

    // Connect Graph Scene to Op Store.

    connect(NsOpStore::instance(), SIGNAL(opObjectCreated(NsOpObject*)),
            scene,                 SLOT(onOpObjectCreated(NsOpObject*)));

    connect(NsOpStore::instance(), SIGNAL(opObjectDestroyed(NsOpObject*)),
            scene,                 SLOT(onOpObjectDestroyed(NsOpObject*)));

    // Connect Graph Scene to Command Central.

    connect(NsCmdCentral::instance(),SIGNAL(feedChanged(QString,QString,bool)),
            scene,                   SLOT(onFeedChanged(QString,QString,bool)));

    connect(NsCmdCentral::instance(), SIGNAL(graphCleared(bool)),
            scene,                    SLOT(onGraphCleared(bool)));
    
    return scene;
}

NsGraphView*
NsMainWindow::_createGraphView(NsGraphScene   *scene, 
                               NsUndoStack *undoStack, 
                               QWidget        *parent)
{
    NsGraphView *view(new NsGraphView(scene, undoStack, parent)); // Child.
    //view->setViewport(new QGLWidget); // Not working!
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setMouseTracking(false);
    view->setRenderHint(QPainter::Antialiasing);
    view->setRenderHint(QPainter::HighQualityAntialiasing);
    view->setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    //setCacheMode(QGraphicsView::CacheBackground);
    //view->setDragMode(QGraphicsView::NoDrag);
    view->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    //view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    //view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    //view->viewport()->setAutoFillBackground(false);

    // Connect Graph View to Graph Scene.

    connect(scene, SIGNAL(itemRemoved(NsGraphItem*)),
            view,  SLOT(onItemRemoved(NsGraphItem*)));

    // Connect Graph View to Command Central.

    connect(NsCmdCentral::instance(), SIGNAL(graphViewFramed(bool,int,bool)),
            view,                     SLOT(onFramed(bool,int,bool)));
    connect(NsCmdCentral::instance(),
            SIGNAL(currentVisibleFrameChanged(int,bool,bool)),
            view,
            SLOT(onCurrentVisibleFrameChanged(int,bool,bool)));

    // Connect Graph View to Graph Callback.
    
    connect(NsGraphCallback::instance(), SIGNAL(beginTimeStep(NtTimeBundle)),
            view,                        SLOT(onBeginTimeStep(NtTimeBundle)));

    connect(NsGraphCallback::instance(), SIGNAL(endStep(NtTimeBundle)),
            view,                        SLOT(onEndStep(NtTimeBundle)));

    // Connect Graph View to preferences.

    connect(NsPreferences::instance(), SIGNAL(changed()), 
            view->viewport(),          SLOT(update()));

    return view;
}


Ns3DScene*
NsMainWindow::_create3DScene(NsUndoStack *undoStack, QObject *parent)
{
    Ns3DScene *scene(new Ns3DScene(undoStack, parent));    // Child.

    // Connect scene to Op store.

    connect(NsOpStore::instance(), SIGNAL(opObjectCreated(NsOpObject*)),
            scene,                 SLOT(onOpObjectCreated(NsOpObject*)));

    connect(NsOpStore::instance(), SIGNAL(opObjectDestroyed(NsOpObject*)),
            scene,                 SLOT(onOpObjectDestroyed(NsOpObject*)));

    // Connect scene to Command Central.

    //connect(NsCmdCentral::instance(),
    //        SIGNAL(graphCleared(bool)),
    //        scene,
    //        SLOT(onGraphCleared(bool)));

    connect(NsCmdCentral::instance(),
            SIGNAL(opSelectionChanged(QStringList,bool)),
            scene,
            SLOT(onOpSelectionChanged(QStringList,bool)));

    connect(NsCmdCentral::instance(),
            SIGNAL(metaChanged(QString,QString,QString,bool)),
            scene,
            SLOT(onMetaChanged(QString,QString,QString,bool)));

    return scene;
}


Ns3DView*
NsMainWindow::_create3DView(Ns3DScene *scene, QWidget *parent)
{
    // Create 3D View.

    Ns3DView *view(new Ns3DView(scene, parent));    // Child.

    // Connect 3D View to Command Central.

    connect(NsCmdCentral::instance(), SIGNAL(opCreated(QString,bool)),
            view,                     SLOT(update()));
    connect(NsCmdCentral::instance(), SIGNAL(opErased(QString,bool)),
            view,                     SLOT(update()));

    connect(NsCmdCentral::instance(), 
            SIGNAL(opStateChanged(QStringList,bool)),
            view,
            SLOT(update()));

    connect(NsCmdCentral::instance(),
            SIGNAL(valueChanged(QString,QString,int,bool)),
            view,
            SLOT(_onValueChanged(QString,QString,int,bool)));

    connect(NsCmdCentral::instance(),
            SIGNAL(metaChanged(QString,QString,QString,bool)),
            view,
            SLOT(update()));

    connect(NsCmdCentral::instance(),
            SIGNAL(opSelectionChanged(QStringList,bool)),
            view,  
            SLOT(update()));
    
    connect(NsCmdCentral::instance(),
            SIGNAL(feedChanged(QString,QString,bool)),
            view,
            SLOT(update()));

    connect(NsCmdCentral::instance(),
            SIGNAL(currentVisibleFrameChanged(int,bool,bool)),
            view,
            SLOT(onCurrentVisibleFrameChanged(int,bool,bool)));

    connect(NsCmdCentral::instance(),
            SIGNAL(graphCleared(bool)),
            view,
            SLOT(update()));

    // Connect 3D View to Graph Callback.

    connect(NsGraphCallback::instance(), SIGNAL(endFrame(NtTimeBundle)),
            view,                        SLOT(onEndFrame(NtTimeBundle)));
    connect(NsGraphCallback::instance(), SIGNAL(endTimeStep(NtTimeBundle)),
            view,                        SLOT(onEndTimeStep(NtTimeBundle)));

    // Connect Graph View to preferences.

    connect(NsPreferences::instance(), SIGNAL(changed()), 
            view,                      SLOT(update()));

    return view;
}

NsHelpView*
NsMainWindow::_createHelpView(QWidget *parent)
{
    NsHelpView::createOpHtmlCache();

    NsHelpView *view = new NsHelpView(parent);   // Child.
    connect(NsCmdCentral::instance(),
            SIGNAL(opSelectionChanged(QStringList,bool)),
            view,
            SLOT(onOpSelectionChanged(QStringList,bool)));
    connect(NsCmdCentral::instance(), SIGNAL(graphCleared(bool)),
            view,                     SLOT(onGraphCleared(bool)));
    return view;
}


// _createValueEditor
// ------------------
//! [static]

NsValueEditorWidget*
NsMainWindow::_createValueEditor(NsUndoStack *undoStack, QWidget *parent)
{
    NsValueEditorWidget *editor(new NsValueEditorWidget(undoStack, parent));
    //editor->setBackgroundRole(QPalette::Dark);
    //connect(NsCmdCentral::instance(),
    //        SIGNAL(opSelectionChanged(QString,bool,bool)),
    //        editor,
    //        SLOT(onOpSelectionChanged(QString,bool,bool)));
    connect(NsCmdCentral::instance(),
            SIGNAL(opSelectionChanged(QStringList,bool)),
            editor,
            SLOT(onOpSelectionChanged(QStringList,bool)));

    connect(NsCmdCentral::instance(),
            SIGNAL(bodySelectionChanged(QStringList,bool)),
            editor,
            SLOT(onBodySelectionChanged(QStringList,bool)));

    connect(NsCmdCentral::instance(),  
            SIGNAL(graphCleared(bool)),
            editor,
            SLOT(onGraphCleared(bool)));

    return editor;
}


// _createDockWidget
// -----------------
//! [static]

NsDockWidget*
NsMainWindow::_createDockWidget(const QString                         &title, 
                                QWidget                               *parent, 
                                const Qt::WindowFlags                  flags, 
                                QWidget                               *widget,
                                const QDockWidget::DockWidgetFeatures  features,
                                const Qt::DockWidgetAreas              areas)
{
    NsDockWidget *dw = new NsDockWidget(title, parent, flags); // Child.
    dw->setFeatures(features);
    dw->setAllowedAreas(areas);
    dw->setWidget(widget);
    return dw;
}


// _splashMessage
// --------------
//! [static]

void
NsMainWindow::_splashMessage(NsSplashScreen *splash, const QString &message)
{
    if (0 != splash) {
        splash->showMessage(message);
    }   
}



//// onAlarmMessage
//// --------------
////! Makes sure the message widget is visible and on top of any widgets tabbed
////! with it, and shows message in status bar for 5 seconds.

//void
//NsMainWindow::onAlarmMessage(const QString &text)
//{
//    statusBar()->showMessage(text, 5000);

//    messageWindow->show();

//    QList<QDockWidget *> dockWidgets = tabifiedDockWidgets(messageWindow);
//    foreach (QDockWidget *dockWidget, dockWidgets)
//        tabifyDockWidget(dockWidget, messageWindow);
//}


//// updateActions
//// -------------
////! Updates the enabled/disabled state of the main window's actions.

//void
//NsMainWindow::updateActions()
//{
//    newSceneAction->setEnabled(!Ns::isSolving());
//    openSceneAction->setEnabled(!Ns::isSolving());
//    recentScenesMenu->setEnabled(!Ns::isSolving());
//}
