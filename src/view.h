/***************************************************************************
  view.h
  -------------------
  View class for application
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

#ifndef VIEW_H
#define VIEW_H

#include <QWidget>
#include "ui_view.h"
#include "ui_ingredientpage.h"

class QTableView;
class GrainModel;
class HopModel;
class MiscModel;
class NotePage;
class Recipe;

class View : public QWidget {
    Q_OBJECT
public:
    // constructor
    View(QWidget *parent, Recipe *recipe);
    // destructor
    ~View();

public slots:
    // set the recipe title
    void setTitle(const QString &title);
    // set the recipe style
    void setStyle(const QString &style);
    // set the brewer name
    void setBrewer(const QString &brewer);
    // set the recipe size
    void setSize(double size);

    // model has been modified
    void modelModified();
    // flush/reset the view
    void flush();
    // refresh just the characteristics
    void refresh();

private:
    Ui::View ui;
    Ui::IngredientPage grainpage;
    Ui::IngredientPage hoppage;
    Ui::IngredientPage miscpage;

    Recipe *recipe_;
    GrainModel *grainmodel_;
    HopModel *hopmodel_;
    MiscModel *miscmodel_;

    NotePage *notepage_;
};

#endif // VIEW_H

