/***************************************************************************
  qbrew.h
  -------------------
  Controller class for QBrew
  -------------------
  Copyright 2006-2008 David Johnson
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
***************************************************************************/

#ifndef QBREW_H
#define QBREW_H

#include <QMainWindow>

#include "ui_mainwindow.h"
#include "configstate.h"

class QCloseEvent;
class QTimer;

class AlcoholTool;
class Configure;
class Data;
class DatabaseTool;
class HelpViewer;
class HydrometerTool;
class Recipe;
class TextPrinter;
class View;

class QBrew : public QMainWindow {
    Q_OBJECT
 public:
    ~QBrew();
    // return pointer to the singleton
    static QBrew *instance();
    // initialize qbrew
    void initialize(const QString &filename = "");

    // return out the base dir for the help documents
    QString docBase();
    // return the base dir for the data files
    QString dataBase();

 private slots:
    // generate a new recipe
    void fileNew();
    // open a recipe
    void fileOpen(const QString &filename = "");
    // open a recent recipe
    void fileRecent();
    // save a recipe
    void fileSave();
    // save a recipe under a different filename
    void fileSaveAs();
    // export the recipe to non-native format
    void fileExport();
    // print preview
    void filePrintPreview();
    // print the recipe
    void filePrint();

    // show the alcohol tool
    void toolsAlcohol();
    // show the hydrometer tool
    void toolsHydrometer();
    // show the database tool
    void toolsDatabase();

    // toggle the statusbar
    void optionsStatusbar(bool on);
    // display the setup dialog
    void optionsConfigure();

    // displays help contents
    void helpContents();
    // displays primer
    void helpPrimer();
    // displays context help cursor
    void helpContext();
    // shows an about dialog for QBrew
    void helpAbout();

    // received if view widgets have changed
    void recipeModified();

    // setup recent file menu
    void recentMenuShow();

    // apply states from config dialog
    void applyGeneralState(const GenConfigState &state);
    void applyRecipeState(const RecipeConfigState &state);
    void applyCalcState(const CalcConfigState &state);
    void saveConfig();

    // autosave timer
    void autoSave();

 private:
    // singleton
    QBrew();
    QBrew(const QBrew&);
    QBrew &operator=(const QBrew&);

    // filter events
    bool eventFilter(QObject *obj, QEvent *event);

    // initialize actions
    void initActions();
    // initialize autosave
    void initAutoSave();

    // catch the close event
    void closeEvent(QCloseEvent *e);

    // query user to save the current file
    int querySave();
    // query to overwrite existing file
    int queryOverwrite(const QString filename);
    // add a file to recent file menu
    void addRecent(const QString &filename);
    // open a named file
    bool openFile(const QString &filename);
    // save a file
    bool saveFile(const QString &filename);
    // backup the file
    bool backupFile();

    // set window caption from file name
    void setFileCaption(const QString &filename);

    // read in configuration
    void readConfig();
    // write out configuration
    void writeConfig();
    // restore application state
    void restoreState();
    // save application state
    void saveState();

private:
    static QBrew *instance_;

    Ui::MainWindow ui;

    Data *data_;
    Recipe *recipe_;
    View *view_;

    AlcoholTool* alcoholtool_;
    DatabaseTool* databasetool_;
    HydrometerTool* hydrometertool_;
    Configure* configure_;
    HelpViewer* handbook_;
    HelpViewer* primer_;

    ConfigState state_;
    QString filename_;
    bool newflag_;
    bool backed_;

    QTimer *autosave_;
    QString autosavename_;
    TextPrinter *textprinter_;
};

#endif // QBREW_H
