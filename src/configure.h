/***************************************************************************
  configure.h
  -------------------
  Application configuration dialog
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
****************************************************************************/

#ifndef CONFIGURE_H
#define CONFIGURE_H

#include <QDialog>
#include "ui_calcconfig.h"
#include "ui_generalconfig.h"
#include "ui_recipeconfig.h"
#include "configstate.h"

class QDialogButtonBox;
class QTabWidget;

class Configure : public QDialog
{
    Q_OBJECT
public:
    // constructor
    Configure(QWidget* parent);
    // destructor
    virtual ~Configure();

    // read in config to set states
    void setState(const ConfigState &config);

signals:
    // send on apply button press
    void calcApply(const CalcConfigState &state);
    void generalApply(const GenConfigState &state);
    void recipeApply(const RecipeConfigState &state);
    // OK button pressed
    void configureAccept();

private:
    // set the state for the pages
    void setCalcState(const CalcConfigState &state);
    void setGeneralState(const GenConfigState &state);
    void setRecipeState(const RecipeConfigState &state);
    void defaults();
    void apply();

private slots:
    // standard modeless dialog slots
    void accept();
    void slotClicked(QAbstractButton *button);
    // convert the batch size when units have changed
    void convertBatchSpin(const QString &selection);

private:
    QTabWidget *tabwidget_;
    QDialogButtonBox *buttonbox_;
    Ui::CalcConfig *calc_;
    Ui::GeneralConfig *general_;
    Ui::RecipeConfig *recipe_;
    ConfigState state_;
};

#endif // CONFIGURE_H
