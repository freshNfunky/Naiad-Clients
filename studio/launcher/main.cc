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

#ifdef WIN32
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif  // WIN32

#include "NsConsole.h"
#include "NsProcess.h"
#include <QtGui/QApplication>
#include <QFileInfo>
#include <QDebug>
#include <cstdlib>
#include <iostream>

// -----------------------------------------------------------------------------

int
main(int argc, char *argv[])
{
    const QString program("nstudio-app");

#ifdef WIN32
    QApplication app(argc, argv);

    // Use fixed-width font. This is the same font that MSVC uses by default.

    NsConsole console;
    console.setFont(QFont("Consolas")); 
    console.setReadOnly(true);
    console.setStyleSheet("background-color: lightgray");
    console.setWindowTitle(QObject::tr("Naiad Studio Console"));
    console.setMinimumHeight(640);
    console.setMinimumWidth(480);
    console.show();

    NsProcess proc(&console); // Child. Console owns process.
    proc.setProcessChannelMode(QProcess::MergedChannels); // Merge cout & cerr. 

    QString arguments;
    for (int i = 1; i < argc; ++i) {
        // Skip first arg.
        arguments += QString(argv[i]) + " ";
    }

    console.setPlainText(
        ">> PROGRAM: " + program + "\n" + 
        ">> ARGUMENTS: " + arguments);

    // Connect Process to Console to catch output.

    QObject::connect(&proc,    SIGNAL(output(QString)), 
                     &console, SLOT(onOutput(QString)));

    QObject::connect(&proc,    SIGNAL(finished(int,QProcess::ExitStatus)),
                     &console, SLOT(close(int,QProcess::ExitStatus)));

    // Connect Console to Process.
    // Kill process when console is closed.

    QObject::connect(&console, SIGNAL(closed(NsConsole*)),
                     &proc,    SLOT(kill()/*terminate()*/));

    QObject::connect(&console, SIGNAL(destroyed()),
                     &proc,    SLOT(kill()/*terminate()*/));

    proc.setNativeArguments(arguments);
    proc.start(program);

    const int timeOut(30000);   // Milliseconds.

    // Wait for process to start before entering event loop.

    if (!proc.waitForStarted(timeOut)) {
        std::abort();
    }

    const int exitStatus = app.exec(); // Start event loop.

    // Wait for process to finish before exiting.

    if (QProcess::Running == proc.state() &&
        !proc.waitForFinished(timeOut)) {
        std::abort();
    }

    return exitStatus;
#else // Linux and MAC
    QStringList arguments;
    for (int i = 1; i < argc; ++i) {
        arguments += QString(argv[i]);
    }

    // Execute nstudio-app as an attached process so that we can kill
    // it using <ctrl>-C in the terminal.

    const int exitCode = QProcess::execute(program, arguments);

    switch (exitCode) {
    case -1:
        std::cerr << program.toAscii().data() << " crashed\n";
        std::abort();
    case -2:
        std::cerr << program.toAscii().data() << " failed to start\n";
        std::abort();
    default:
        break;
    }

    return exitCode;

//    if (!QProcess::startDetached(program, arguments)) {
//        std::cerr << "Failure starting program: "
//                  << program.toAscii().data() << "\n";
//        std::abort();
//    }
//    return 0;
#endif  // WIN32
}
