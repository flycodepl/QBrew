/***************************************************************************
  alcoholtool.cpp
  -------------------
  An Alcohol Percentage Calculator utility for QBrew
  -------------------
  Copyright 2004-2008, David Johnson <david@usermode.org>
  Based on code Copyright 2004, Michal Palczewski <michalp@gmail.com>
  Please see the header file for copyright and license information
 ***************************************************************************/

#include <QLabel>

#include "recipe.h"
#include "resource.h"
#include "alcoholtool.h"

//////////////////////////////////////////////////////////////////////////////
// AbvcalcTool()
// ----------------
// Constructor

AlcoholTool::AlcoholTool(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(Resource::TITLE + tr(" - Alcohol Tool"));

    ui.setupUi(this);

    // connections
    connect(ui.og, SIGNAL(valueChanged(double)), this, SLOT(recalc()));
    connect(ui.fg, SIGNAL(valueChanged(double)), this, SLOT(recalc()));

    recalc();
}

//////////////////////////////////////////////////////////////////////////////
// recalc()
// --------
// the signal to calculate the Alcohol percentage

void AlcoholTool::recalc()
{
    ui.abw->setText(QString::number(Recipe::ABW(ui.og->value(),
                                                ui.fg->value()) * 100.0,
                                    'f', 1) + "%");
    ui.abv->setText(QString::number(Recipe::ABV(ui.og->value(),
                                                ui.fg->value()) * 100.0,
                                    'f', 1) + "%");
}
