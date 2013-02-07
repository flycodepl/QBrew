/***************************************************************************
  databasetool.h
  -------------------
  Database editor for QBrew
  -------------------
  Copyright 2005-2008, David Johnson
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

#ifndef DATABASETOOL_H
#define DATABASETOOL_H

#include <QMainWindow>

#include "ui_databasetool.h"
#include "ui_ingredientpage.h"

#include "grainmodel.h"
#include "hopmodel.h"
#include "miscmodel.h"
#include "stylemodel.h"

class QTableView;
class Data;

class DatabaseTool : public QMainWindow {
    Q_OBJECT
public:
    // constructor
    DatabaseTool(QWidget* parent=0);
    // destructor
    ~DatabaseTool();

private slots:
    // save the database
    void fileSave();
    // received if data has changed
    void dataModified();

private:
    Ui::DatabaseTool ui;
    Ui::IngredientPage grainpage;
    Ui::IngredientPage hoppage;
    Ui::IngredientPage miscpage;
    Ui::IngredientPage stylepage;

    GrainList grains_;
    HopList hops_;
    MiscList miscs_;
    StyleList styles_;

    GrainModel *grainmodel_;
    HopModel *hopmodel_;
    MiscModel *miscmodel_;
    StyleModel *stylemodel_;

    bool modified_;
};

#endif // DATABASETOOL_H
