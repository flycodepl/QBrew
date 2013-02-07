/***************************************************************************
  main.cpp
  -------------------
  A brewing recipe calculator
  -------------------
  Copyright 1999-2008 David Johnson <david@usermode.org>
  All rights reserved.
 
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
 
  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
 
  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
 
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
****************************************************************************/

#include <cstdlib>

#include <QApplication>
#include <QLibraryInfo>
#include <QLocale>
#include <QMainWindow>
#include <QStringList>
#include <QTextStream>
#include <QTranslator>

#if defined(Q_WS_MACX)
extern void qt_mac_set_menubar_icons(bool enable);
#endif

#include "qbrew.h"
#include "resource.h"

using namespace std;
using namespace Resource;

//////////////////////////////////////////////////////////////////////////////
// doversion()
// -----------
// Print out copyright and version info to stdout

void doversion(QTextStream &stream)
{
    stream << PACKAGE << ' ' << VERSION << '\n';
    stream << QObject::tr(DESCRIPTION) << '\n';
    stream << QObject::tr(COPYRIGHT);
    stream << ' ' << AUTHOR;
    stream << " <" << AUTHOR_EMAIL << ">";
    stream << endl;
}

//////////////////////////////////////////////////////////////////////////////
// dohelp()
// --------
// Print out help info to stdout

void dohelp(QTextStream &stream)
{
    stream << QObject::tr("Usage: %1 [options] [file]\n").arg(PACKAGE);
    stream << QObject::tr(DESCRIPTION) << "\n\n";

    // arguments
    stream << QObject::tr("Arguments\n");
    stream << QObject::tr("    file                  File to open") << "\n\n";

    // general options
    stream << QObject::tr("Options\n");
    stream << QObject::tr("    --help                Print the command line options.\n");
    stream << QObject::tr("    --version             Print the application version.\n");
    stream << endl;
}

//////////////////////////////////////////////////////////////////////////////
// doargs()
// --------
// Process the arguments that QApplication doesn't take care of

QString doargs(QStringList arguments, QTextStream &stream)
{
    QString arg;
    for (int n=1; n<arguments.count(); ++n) {
        arg = arguments.at(n);
        if ((arg == "-h")  || (arg == "-help") ||
            (arg == "--help")) {
            dohelp(stream);
            exit(0);
        } else if ((arg == "-v") || (arg == "-version") ||
                   (arg == "--version")) {
            doversion(stream);
            exit(0);
        } else if (arg[0] == '-') {
            // no other valid options
            stream << QObject::tr("Invalid parameter \"%1\"\n") .arg(arg);
            dohelp(stream);
            exit(1);
        } else {
            // it must be a filename
            return arg;
        }
    }
    return (QObject::tr(DEFAULT_FILE));
}

//////////////////////////////////////////////////////////////////////////////
// main()
// ------
// Entry point of application

int main(int argc, char **argv)
{
    Q_INIT_RESOURCE(qbrew);

    QApplication app(argc, argv);
    app.setApplicationName(PACKAGE);
    app.setOrganizationName(TITLE);

#if defined(Q_WS_MACX)
    // disable icons on Mac menus (as is customary)
    qt_mac_set_menubar_icons(false);
#endif

    // load qbrew translations
    QString transdir = QBrew::instance()->dataBase() + "translations";
    QTranslator translator;
    if (translator.load("qbrew_" + QLocale::system().name(), transdir)) {
        app.installTranslator(&translator);
    }
    // load qt translations
    QString qttransdir = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    QTranslator qttranslator;
    if (qttranslator.load("qt_" + QLocale::system().name(), qttransdir)) {
        app.installTranslator(&qttranslator);
    }

    // check for additional command line arguments
    QTextStream stream(stdout);
    QString filename = doargs(app.arguments(), stream);

    QBrew* mainwindow = QBrew::instance();
    mainwindow->initialize(filename);
    mainwindow->show();

    app.installEventFilter(mainwindow); // for handling file open events

    return app.exec();
}
