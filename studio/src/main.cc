// -----------------------------------------------------------------------------
//
// main.cc
//
// Naiad Studio main file.
//
// Naiad Studio is Exotic Matter's "freeware" reference implementation of a
// graphical front-end for creating, editing and visualizing Naiad Operator
// Graphs.
//
// Copyright (c) 2011 Exotic Matter AB.  All rights reserved. 
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
//
// -----------------------------------------------------------------------------

#include "NsPlatform.h"
#include "NsApplication.h"
#include "NsMainWindow.h"
#include "NsQuery.h"
#include "NsSplashScreen.h"
#include "NsPreferences.h"
#include <QMessageBox>
#include <cstdlib>
//#include <NbFilename.h>

//#define NS_NO_SPLASH

// -----------------------------------------------------------------------------

// main
// ----
//! The Open Naiad Studio main function.
//! Sets things up and starts the Open Naiad Studio GUI.

int
main(int argc, char *argv[])
{
    try {
        // Create application.

        // NB: Must be called before application is constructed.
        //QApplication::setGraphicsSystem("opengl");

        NsApplication app(argc, argv);
        if (app.help()) {
            return 0;   // Early exit.
        }

        NsSplashScreen *splash(0);

#ifndef NS_NO_SPLASH
        // Create and show splash screen.
        splash = new NsSplashScreen(QPixmap(":/images/splash.png"));
        splash->showMessage("Starting up Naiad Studio...");
        if (!app.playblast()) {
            splash->show();
        }
        splash->showMessage("Creating main window...");
#endif  // NS_NO_SPLASH

        // Create main window.

        NsMainWindow mainWindow(splash);
        mainWindow.setTabShape(QTabWidget::Triangular);

        // Show main window.

#ifndef NS_NO_SPLASH
        splash->showMessage(QObject::tr("Opening main window..."));
#endif  // NS_NO_SPLASH

        mainWindow.show();
        mainWindow.newGraph(QObject::tr("New Graph"));

#ifndef NS_NO_SPLASH
        splash->finish(&mainWindow);
#endif  // NS_NO_SPLASH

        // Open given graph, if any.

        if (!app.fileName().isEmpty()) {
#ifndef NS_NO_SPLASH
            splash->showMessage(
                QObject::tr("Opening graph from ") + app.fileName() + "...");
#endif  // NS_NO_SPLASH

            mainWindow.openGraph(app.fileName());
        }

        if (app.playblast()/* && app.validPlayblastFrames()*/) {
            // Go into playblast mode. Don't read window settings!
            // We don't read settings in playblast mode because the
            // 3D View might be closed. By default it is open.

            int firstFrame = 1;
            int lastFrame = 1;
            queryFirstVisibleFrame(&firstFrame);
            queryLastVisibleFrame(&lastFrame);
            if (app.validPlayblastFrames()) {
                // Valid frames from command line args.

                firstFrame = app.playblastFirstFrame();
                lastFrame = app.playblastLastFrame();
            }

            mainWindow.playblast(app.playblastPath(),
                                 app.playblastFileName(),
                                 firstFrame,//app.playblastFirstFrame(),
                                 lastFrame,//app.playblastLastFrame(),
                                 app.playblastWidth(),
                                 app.playblastHeight(),
                                 app.playblastFormat(),
                                 app.playblastQuality());
            
            return 0;   // Exit before event loop starts.
        }
        else {
            mainWindow.readSettings();
        }

        const int exitCode = app.exec(); // Start GUI event loop.

        return exitCode;
    }
    catch (const std::exception &ex) {
        std::cerr << "nstudio: Unhandled exception in main(): "
                  << ex.what() << "\n";
        std::abort();
    }
}


/*!

\file main.cc

\mainpage Naiad Studio Developer's Manual
  
This document is the Naiad Studio Developer's Manual.
The document is automatically generated from the source code and its
documentation using the Doxygen tool. It describes all the
<A CLASS="el" HREF="annotated.html">Naiad Studio classes</A> and the global
<A CLASS="el" HREF="main_8cc.html">main function</A>.

Naiad Studio is implemented in C++ with Qt 4.6.
For documentation of the Qt classes, please refer to the <A CLASS="el"
HREF="http://doc.trolltech.com/4.6">Qt 4.6 Reference Documentation</A>.

*/
