// -----------------------------------------------------------------------------
//
// NsApplication.cc
//
// Naiad Studio application source file.
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

#include "NsApplication.h"
#include "NsProxyStyle.h"
#include "NsQuery.h"
#include "NsCmdCentral.h"
#include "NsOpStore.h"
#include "NsGraphCallback.h"
#include "NsGraphOpItemFactory.h"
//#include "NsGraphInputPlugItemFactory.h"
//#include "NsGraphOutputPlugItemFactory.h"

#include <Ni.h>
#include <NgFactory.h>
#include <NbObject.h>

#include <QFile>
#include <QDebug>
#include <QResource>
#include <QMessageBox>

// Naiad Studio Body Scopes
#include "Ns3DIsoScope.h"
#include "Ns3DGhostScope.h"
#include "Ns3DTileScope.h"
#include "Ns3DParticleScope.h"
#include "Ns3DMeshScope.h"
#include "Ns3DStreamlineScope.h"
#include "Ns3DCameraScope.h"
#include "Ns3DOrthoCameraScope.h"
#include "Ns3DPerspectiveCameraScope.h"
//#include "NsGraphNoteOp.h"

// Naiad Studio Field Scopes
#include "Ns3DFieldIsoScope.h"
#include "Ns3DFieldScalarScope.h"
#include "Ns3DFieldStreamlineScope.h"
#include "Ns3DFieldTileScope.h"

#include <iostream>

// -----------------------------------------------------------------------------

// NsApplication
// -------------
//! CTOR.

NsApplication::NsApplication(int &argc, char **argv)
    : QApplication(argc, argv)
    , _fileName("")
    , _playblast(false)
    , _playblastFirstFrame(1)  // Initially invalid range.
    , _playblastLastFrame(0)
    , _playblastWidth(_defaultPlayblastWidth)
    , _playblastHeight(_defaultPlayblastHeight)
    , _playblastFormat(_defaultPlayblastFormat)
    , _playblastQuality(_defaultPlayblastQuality)
    , _help(false)
{
    connect(this, SIGNAL(aboutToQuit()), SLOT(onAboutToQuit()));

    _parseArgs(QApplication::arguments());

    if (!_help) {
        if (NI_FALSE == NiBegin()) {
            QMessageBox::critical(0,
                                  tr("Naiad Studio - Error"),
                                  tr("NiBegin failed\n"),
                                  QMessageBox::Ok,
                                  QMessageBox::Ok);
        }

        QCoreApplication::setOrganizationName("Exotic Matter");
        QCoreApplication::setOrganizationDomain("exoticmatter.com");
        QCoreApplication::setApplicationName("Naiad Studio");
        QCoreApplication::setApplicationVersion(queryVersion());

        connect(this,                      SIGNAL(aboutToQuit()), 
                NsPreferences::instance(), SLOT(onAboutToQuit()));

        _readSettings();


    //    NiSetVerboseLevel(NI_VERBOSE);

        setStyle(new NsProxyStyle); // Application takes ownership.

        //_parseArgs(QApplication::arguments());
        //_loadStyleSheet(":/styles/default.qss");
        _loadStyleSheet(NsPreferences::instance()->generalStylePath());
        //"../../config/nstudio.qss"


        // Op store must be instantiated early on, before any Ops are
        // created/erased/renamed.

        //NsGraphCallback::createInstance();
        //NsOpStore::createInstance();

        _registerScopes();


        NsCmdCentral    *cc = NsCmdCentral::instance();
        NsOpStore       *os = NsOpStore::instance();
        NsGraphCallback *gc = NsGraphCallback::instance();
        NsMessageWidget *mw = NsMessageWidget::instance();


        // Connect Command Central [signals] to Op Store [slots].

        // Op editing.

        connect(cc, SIGNAL(opCreated(QString,bool)),
                os, SLOT(onOpCreated(QString,bool)));
        connect(cc, SIGNAL(opErased(QString,bool)),
                os, SLOT(onOpErased(QString,bool)));
        connect(cc, SIGNAL(opNameChanged(QString,QString,bool)),
                os, SLOT(onOpNameChanged(QString,QString,bool)));
        connect(cc, SIGNAL(opStateChanged(QStringList,bool)),
                os, SLOT(onOpStateChanged(QStringList,bool)));
        connect(cc, SIGNAL(opSelectionChanged(QStringList,bool)),
                os, SLOT(onOpSelectionChanged(QStringList,bool)));
        connect(cc, SIGNAL(opPositionChanged(QString,qreal,qreal,bool)),
                os, SLOT(onOpPositionChanged(QString,qreal,qreal,bool)));

        // Plug editing.

        connect(cc, SIGNAL(groupPlugChanged(QString,bool)),
                os, SLOT(onGroupPlugChanged(QString,bool)));
        connect(cc, SIGNAL(smackChanged(QStringList,bool)),
                os, SLOT(onSmackChanged(QStringList,bool)));

        // Feed editing.

        connect(cc, SIGNAL(feedChanged(QString,QString,bool)),
                os, SLOT(onFeedChanged(QString,QString,bool)));
        connect(cc, SIGNAL(feedSelectionChanged(QStringList,bool)),
                os, SLOT(onFeedSelectionChanged(QStringList,bool)));

        // Body "editing"

        connect(cc, SIGNAL(bodySelectionChanged(QStringList,bool)),
                os, SLOT(onBodySelectionChanged(QStringList,bool)));

        // Value editing.

        //connect(cc, SIGNAL(valueChanged(QString,QString,int,bool)),
        //        os, SLOT(onValueChanged(QString,QString,int,bool)));
        //connect(cc, SIGNAL(metaChanged(QString,QString,QString,bool)),
        //        os, SLOT(onMetaChanged(QString,QString,QString,bool)));
        //connect(cc, SIGNAL(projectPathChanged(QString,bool)),
        //        os, SLOT(onProjectPathChanged(QString,bool)));
        connect(cc, SIGNAL(currentVisibleFrameChanged(int,bool,bool)),
                os, SLOT(onCurrentVisibleFrameChanged(int,bool,bool)));
        //connect(cc, SIGNAL(firstVisibleFrameChanged(int,bool)),
        //        os, SLOT(onFirstVisibleFrameChanged(int,bool)));
        //connect(cc, SIGNAL(lastVisibleFrameChanged(int,bool)),
        //        os, SLOT(onLastVisibleFrameChanged(int,bool)));

        // Misc.

        connect(cc, SIGNAL(graphCleared(bool)),
                os, SLOT(onGraphCleared(bool)));

        //connect(cc, SIGNAL(graphViewFramed(bool,int,bool)),
        //        os, SLOT(onGraphViewFramed(bool,int,bool)));


        // Connect Command Central [signals] to Message Widget [slots].

        connect(cc, SIGNAL(graphCleared(bool)),
                mw, SLOT(onGraphCleared(bool)));


        // Connect Graph Callback [signals] to Op Store [slots].

        connect(gc, SIGNAL(beginTimeStep(NtTimeBundle)),
                os, SLOT(onBeginTimeStep(NtTimeBundle)));
        connect(gc, SIGNAL(endTimeStep(NtTimeBundle)),
                os, SLOT(onEndTimeStep(NtTimeBundle)));
        connect(gc, SIGNAL(beginOp(NtTimeBundle,QString)),
                os, SLOT(onBeginOp(NtTimeBundle,QString)));
        connect(gc, SIGNAL(endOp(NtTimeBundle,QString)),
                os, SLOT(onEndOp(NtTimeBundle,QString)));
        connect(gc, SIGNAL(reset()),
                os, SLOT(onReset()));

        // 
    }
}


// ~NsApplication
// --------------
//! DTOR.

NsApplication::~NsApplication()
{
    if (!_help) {
        // Destroy singletons.

        NsGraphOpItemFactory::destroy();

        //NsParamWidgetFactory::destroy();
        //NsPropWidgetFactory::destroy();

        //NsGraphInputPlugItemFactory::destroy();
        //NsGraphOutputPlugItemFactory::destroy();

        NsOpStore::destroyInstance();
        NsGraphCallback::destroyInstance();
        //NsPreferences::destroyInstance();

        NiEnd();
    }
}


// notify
// ------
//! Overridden to catch unexpected exceptions.

bool
NsApplication::notify(QObject *receiver, QEvent *event)
{
    try {
        return QApplication::notify(receiver, event);
    }
    catch (const std::exception &ex) {
        QApplication::restoreOverrideCursor();
        //Ns::internalError(tr("Unhandled exception: ") + exception.what());
        qFatal("Unhandled exception: ", ex.what());
    }
    catch (...) {
        QApplication::restoreOverrideCursor();
        //Ns::internalError(tr("Unhandled exception."));
        qDebug() << receiver;
        qDebug() << "Type: " << event->type();
        qDebug() << "Spontaneous: " << event->spontaneous();

        QMouseEvent *me(dynamic_cast<QMouseEvent*>(event));
        if (0 != me) {
            qDebug() << me;
        }

        qFatal("Unhandled exception.");
    }

    return false;
}


// onAboutToQuit
// -------------
//! DOCS [slot]

void
NsApplication::onAboutToQuit()
{
    _writeSettings();
}

// -----------------------------------------------------------------------------

// _parseArgs
// ----------
//! Very basic parsing of command line arguments.

void
NsApplication::_parseArgs(const QStringList &args)
{
    for (int i(1); i < args.size(); ++i) {
        if (args.at(i) == "--playblast") {
            _playblast = true;
        }
        else if (args.at(i) == "--frames") {
            if ((i + 2) < args.size()) {
                _playblastFirstFrame = args.at(++i).toInt();
                _playblastLastFrame = args.at(++i).toInt();
            }
            else {
                qDebug()
                    << tr("Ignoring command line option ") << args.at(i)
                    << tr(" with no parameter");
            }
        }
        else if (args.at(i) == "--size" || 
                 args.at(i) == "--rez"  || 
                 args.at(i) == "--pbrez") {
            if ((i + 2) < args.size()) {
                _playblastWidth = args.at(++i).toInt();
                _playblastHeight = args.at(++i).toInt();
            }
            else {
                qDebug()
                    << tr("Ignoring command line option ") << args.at(i)
                    << tr(" with no parameter");
            }
        }
        else if (args.at(i) == "--fmt" ||
                 args.at(i) == "--format") {
            if (i + 1 < args.size()) {
                _playblastFormat = args.at(++i);
            }
            else {
                qDebug()
                    << tr("Ignoring command line option ") << args.at(i)
                    << tr(" with no parameter");
            }
        }
        else if (args.at(i) == "--quality") {
            if (i + 1 < args.size()) {
                _playblastQuality = args.at(++i).toInt();
            }
            else {
                qDebug()
                    << tr("Ignoring command line option ") << args.at(i)
                    << tr(" with no parameter");
            }
        }
        else if (args.at(i) == "--help" ||
                 args.at(i) == "--usage") {
            using std::cout;
            using std::endl;
            cout 
            << endl
            //<< "----- BEGIN HELP -----" <<  endl
            << endl
            << "Usage: nstudio filename.ni [options]" << endl
            << endl
            << "If no filename is provided Naiad Studio will start with an empty graph." << endl
            << endl
            << "Options may be any of the following:" 
            << endl
            << "--playblast         * Start Naiad Studio in playblast mode." << endl
            << endl
            << "--frames first last * Optionally specify first and last frame." << endl
            << "         <int> <int>  It is strictly required that first <= last." << endl
            << "                      Default frames: Visible frame range"
            << endl
            << "--size width height * Optionally specify resolution of playblast images" << endl
            << "       <int> <int>    You can also use --pbrez or --rez." << endl
            << "                      Default width: " << 640 << endl
            << "                      Default height: " << 480 << endl
            << endl
            << "--format extension  * Optionally specify the format of playblast images" << endl
            << "         <string>     You can also use --fmt." << endl
            << "                      Default format: " << "jpeg" << endl
            << endl
            << "--quality value     * Optionally specify image quality." << endl
            << "          <int>       Valid range is [0, 100]." << endl
            << "                      Default quality: " << 80 << endl
            << endl
            << "Example:" << endl
            << "nstudio mygraph.ni --playblast --frames 31 42 --size 480 320 --format jpeg --quality 100" << endl
            << endl
            << "The command above will create an image sequence from frames 31 to 42" << endl
            << "in the file 'mygraph.ni'. Image resolution will be 480x320 and images" << endl
            << "will be stored in jpeg format using the highest quality." << endl
            << endl
            //<< "----- END HELP -----" << endl
            << endl;
            _help = true;
        }
        else if (_fileName.isEmpty() /*&& args.at(i).endsWith(".ni")*/) {
            _fileName = args.at(i);
        }
        else {
            qDebug()
                << tr("Ignoring additional command line argument ")
                << args.at(i);
        }
    }
}


// _loadStyleSheet
// ---------------
//! Load a style sheet from file and set it.

void
NsApplication::_loadStyleSheet(const QString &fileName)
{
    QString style;
    QFile file(fileName);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream styleIn(&file);
        style = styleIn.readAll();  // Read file to string.
        file.close();
    }
    else {
        file.setFileName(":/styles/nstudio-pro.qss"); // From resource.
        file.open(QFile::ReadOnly | QFile::Text);     // Should be fine!
        QTextStream styleIn(&file);
        style = styleIn.readAll();  // Read file to string.
        file.close();
    }
    setStyleSheet(style);
}

template<typename T>
class NsScopeAllocator
{
public:
    static Nb::Object*
    alloc(const char* type, const char* name) { return new T(); }
};

void
NsApplication::_registerScopes()
{
    // Register body scope allocators

    // Cameras.

    // NB: Perspective camera!
    Ng::Factory::instance()->registerOpAllocFunc(
        "Camera-Scope",
        NsScopeAllocator<Ns3DPerspectiveCameraScope>::alloc);
    // NB: Ortho camera!
    Ng::Factory::instance()->registerOpAllocFunc(
        "Ortho-Camera-Scope",
        NsScopeAllocator<Ns3DOrthoCameraScope>::alloc);
    
    Ng::Factory::instance()->registerOpAllocFunc(
        "Iso-Scope",NsScopeAllocator<Ns3DIsoScope>::alloc);
    Ng::Factory::instance()->registerOpAllocFunc(
        "Ghost-Scope",NsScopeAllocator<Ns3DGhostScope>::alloc);
    Ng::Factory::instance()->registerOpAllocFunc(
        "Particle-Scope",NsScopeAllocator<Ns3DParticleScope>::alloc);
    Ng::Factory::instance()->registerOpAllocFunc(
        "Mesh-Scope",NsScopeAllocator<Ns3DMeshScope>::alloc);
    Ng::Factory::instance()->registerOpAllocFunc(
        "Tile-Scope",NsScopeAllocator<Ns3DTileScope>::alloc);
    Ng::Factory::instance()->registerOpAllocFunc(
        "Streamline-Scope",NsScopeAllocator<Ns3DStreamlineScope>::alloc);

    // Graph notes.

    //Ng::Factory::instance()->registerOpAllocFunc(
    //    "Graph-Note", allocGraphNote);

    // Register field scope allocators
/*
    Ng::Factory::instance()->registerOpAllocFunc(
        "Field-Iso-Scope", allocFieldIsoScope);
    Ng::Factory::instance()->registerOpAllocFunc(
        "Field-Scalar-Scope", allocFieldScalarScope);
    Ng::Factory::instance()->registerOpAllocFunc(
        "Field-Streamline-Scope", allocFieldStreamlineScope);
    Ng::Factory::instance()->registerOpAllocFunc(
        "Field-Tile-Scope", allocFieldTileScope);
*/
}

// -----------------------------------------------------------------------------

// Default playblast settings.

const int     NsApplication::_defaultPlayblastWidth   = 640;
const int     NsApplication::_defaultPlayblastHeight  = 480;
const QString NsApplication::_defaultPlayblastFormat  = QString("jpeg");
const int     NsApplication::_defaultPlayblastQuality = 80;

// -----------------------------------------------------------------------------

//! DOCS [static]
const QString NsApplication::_settingsGroup = QString("Application");


//! DOCS
void
NsApplication::_readSettings()
{
    QSettings settings(QSettings::IniFormat,
                       QSettings::UserScope,
                       QCoreApplication::organizationName(),
                       QCoreApplication::applicationName());
    settings.beginGroup(_settingsGroup);
    _playblastWidth = 
        settings.value(
            "PlayblastWidth", QVariant(_defaultPlayblastWidth)).toInt();
    _playblastHeight = 
        settings.value(
            "PlayblastHeight", QVariant(_defaultPlayblastHeight)).toInt();
    _playblastFormat = 
        settings.value(
            "PlayblastFormat", QVariant(_defaultPlayblastFormat)).toString();
    _playblastQuality = 
        settings.value(
            "PlayblastQuality", QVariant(_defaultPlayblastQuality)).toInt();
    settings.endGroup();
}


//! DOCS
void
NsApplication::_writeSettings()
{
    QSettings settings(QSettings::IniFormat,
                       QSettings::UserScope,
                       QCoreApplication::organizationName(),
                       QCoreApplication::applicationName());
    settings.beginGroup(_settingsGroup);
    settings.setValue("PlayblastWidth", QVariant(_playblastWidth));
    settings.setValue("PlayblastHeight", QVariant(_playblastHeight));
    settings.setValue("PlayblastFormat", QVariant(_playblastFormat));
    settings.setValue("PlayblastQuality", QVariant(_playblastQuality));
    settings.endGroup();
}

// -----------------------------------------------------------------------------



//Ns::internalError(const QString &text)
//{
//    if (ns()->_internalError) {
//        qFatal("Internal error during handling of internal error, giving up...");
//        NiEnd();
//    }

//    if (internalErrorsAsErrors()) {
//        error(text);
//        return;
//    }

//    ns()->_internalError = true;

//    QMessageBox::StandardButton answer = QMessageBox::critical
//        (0, tr("Naiad Studio - Internal Error"),
//         tr("An internal error has occured:\n") + text + "\n\n" +
//         tr("The continued operation of the program cannot be "
//            "guaranteed, and it is therefore recommended that "
//            "you save the current scene on a backup file "
//            "(nstudio_internal_error.ni) and restart the program.\n\n"
//            "Please make sure you report the error and the circumstances "
//            "under which it occurred to Exotic Matter so that we can make "
//            "sure it does not happen in the future."),
//         QMessageBox::Save | QMessageBox::Discard | QMessageBox::Ignore);

//    if (answer == QMessageBox::Save) {
//        NiExportGraph("nstudio_internal_error.ni",
//                      "This NI file was generated from Naiad Studio "
//                      "as a result of an internal error",
//                      ".ni");
//        NiEnd();
//        exit(EXIT_FAILURE);
//    } else if (answer == QMessageBox::Discard) {
//        NiEnd();
//        exit(EXIT_FAILURE);
//    }

//    NsMessageWidget::internalError(text);
//    ns()->_internalError = false;
//}

