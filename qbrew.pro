
# Copyright 2007-2008 David Johnson
# This project file is free software; the author gives unlimited
# permission to copy, distribute and modify it.

TARGET = qbrew
TEMPLATE = app
CONFIG += qt warn_on

MOC_DIR = build
OBJECTS_DIR = build
RCC_DIR = build
UI_DIR = build

INCLUDEPATH += src
VPATH = src

QMAKE_DISTCLEAN = config.log

win32 {
    RC_FILE = win\icon.rc
    CONFIG -= console

    target.path = release

    trans.path = translations
    trans.files = translations/*.qm

    data.path = release
    data.files = data/* pics/splash.png win/qbrew.ico README LICENSE

    doc.path = release/doc
    doc.files = docs/book/*.html docs/primer/*.html

    INSTALLS += target trans data doc 
}

macx {
    CONFIG += x86 ppc
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4
    QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.4u.sdk

    DEFINES += HAVE_ROUND
    LIBS += -dead-strip
    QMAKE_POST_LINK=strip qbrew.app/Contents/MacOS/qbrew

    # install into app bundle
    trans.path = qbrew.app/Contents/Resources/translations
    trans.files = translations/*.qm

    data.path = qbrew.app/Contents/Resources
    data.files = data/* pics/splash.png mac/application.icns mac/document.icns

    doc.path = qbrew.app/Contents/Resources/en.lproj
    doc.files = docs/book/*.html docs/primer/*.html

    misc.path = qbrew.app/Contents
    misc.files = mac/Info.plist mac/PkgInfo

    INSTALLS += trans data doc misc
}

unix:!macx {
    configure {
        DEFINES += $$(HAVEDEFS)
        target.path = $$(BINDIR)
        trans.path = $$(DATADIR)/translations
        data.path = $$(DATADIR)
        doc.path = $$(DOCDIR)
    } else {
        target.path = /usr/local/bin
        trans.path = /usr/local/share/qbrew/translations
        data.path = /usr/local/share/qbrew
        doc.path = /usr/local/share/doc/qbrew
    }

    trans.files = translations/*.qm
    data.files = data/* pics/splash.png
    # The handbook and the primer are installed in a different
    # location. The files README and LICENSE are copied by the
    # dh_installdocs script
    #doc.files = docs/book/*.html docs/primer/*.html README LICENSE
    #INSTALLS += target trans data doc
    INSTALLS += target trans data
}

RESOURCES = qbrew.qrc

HEADERS = alcoholtool.h \
        beerxmlreader.h \
	configstate.h \
	configure.h \
	data.h \
	databasetool.h \
        datareader.h \
	grain.h \
	graindelegate.h \
	grainmodel.h \
	helpviewer.h \
	hop.h \
	hopdelegate.h \
	hopmodel.h \
	hydrometertool.h \
        ingredientview.h \
	misc.h \
	miscdelegate.h \
	miscmodel.h \
	notepage.h \
	qbrew.h \
	quantity.h \
	recipe.h \
        recipereader.h \
	resource.h \
	style.h \
	styledelegate.h \
	stylemodel.h \
	textprinter.h \
	view.h

SOURCES = alcoholtool.cpp \
        beerxmlreader.cpp \
	configure.cpp \
	data.cpp \
	databasetool.cpp \
        datareader.cpp \
	export.cpp \
	grain.cpp \
	graindelegate.cpp \
	grainmodel.cpp \
	helpviewer.cpp \
	hop.cpp \
	hopdelegate.cpp \
	hopmodel.cpp \
	hydrometertool.cpp \
        ingredientview.cpp \
	main.cpp \
	misc.cpp \
	miscdelegate.cpp \
	miscmodel.cpp \
	notepage.cpp \
	qbrew.cpp \
	quantity.cpp \
	recipe.cpp \
        recipereader.cpp \
	style.cpp \
	styledelegate.cpp \
	stylemodel.cpp \
	textprinter.cpp \
	view.cpp

FORMS = alcoholtool.ui \
	calcconfig.ui \
	databasetool.ui \
	generalconfig.ui \
	helpviewer.ui \
	hydrometertool.ui \
        ingredientpage.ui \
	mainwindow.ui \
	noteview.ui \
	recipeconfig.ui \
	view.ui

TRANSLATIONS = translations/qbrew_de.ts
