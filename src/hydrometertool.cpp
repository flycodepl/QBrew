/***************************************************************************
  hydrometertool.h
  -------------------
  A hydrometer correction utility for QBrew
  -------------------
  Copyright 2004-2008, David Johnson <david@usermode.org>
  Based on code Copyright 2001, Abe Kabakoff <abe_kabakoff@gmx.de>
  Please see the header file for copyright and license information
 ***************************************************************************/

#include <QLabel>

#include "resource.h"
#include "data.h"
#include "hydrometertool.h"

using Resource::TITLE;

//////////////////////////////////////////////////////////////////////////////
// HydrometerTool()
// ----------------
// Constructor

HydrometerTool:: HydrometerTool(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    QString SUFFIX = QString(" %1%2")
        .arg(Resource::DEGREE)
        .arg(Data::instance()->defaultTempUnit().symbol());

    setWindowTitle(TITLE + tr(" - Hydrometer Tool"));

    // additional setup
    QFont fnt(font());
    fnt.setBold(true);
    ui.corrected->setFont(fnt);

    ui.calibrated->setSuffix(SUFFIX);
    ui.sample->setSuffix(SUFFIX);

    // convert UI properties of celsius
    if (Data::instance()->defaultTempUnit() == Temperature::celsius) {
        ui.calibrated->setMinimum(0.0);
        ui.calibrated->setMaximum(100.0);
        ui.calibrated->setValue(15.0);
        ui.sample->setMinimum(0.0);
        ui.sample->setMaximum(100.0);
        ui.sample->setValue(15.0);
    }

    // connections
    connect(ui.sample, SIGNAL(valueChanged(double)), this, SLOT(recalc()));
    connect(ui.calibrated, SIGNAL(valueChanged(double)), this, SLOT(recalc()));
    connect(ui.reading, SIGNAL(valueChanged(double)), this, SLOT(recalc()));

    recalc();
}

//////////////////////////////////////////////////////////////////////////////
// recalc()
// --------
// the signal to calculate the actual SG

void HydrometerTool::recalc()
{
    // see http://www.hbd.org/brewery/library/HydromCorr0992.html

    const double COEFF1 = 1.313454;
    const double COEFF2 = 0.132674;
    const double COEFF3 = 2.057793E-3;
    const double COEFF4 = 2.627634E-6;

    double calterm;   // E is the correction factor for the calibration temp
    double corr;      // the corrected reading

    double samp = ui.sample->value();
    double cal = ui.calibrated->value();
    double read = ui.reading->value();

    // calc needs to be in fahrenheit
    if (Data::instance()->defaultTempUnit() == Temperature::celsius) {
        samp = celsius2fahrenheit(samp);
        cal = celsius2fahrenheit(cal);
    }

    // Correction(@59F) = 1.313454 - 0.132674*T + 2.057793e-3*T**2 - 2.627634e-6*T**3
    // the equation is set up for 59F (15C)
    calterm = (COEFF1
               - (COEFF2 * cal)
               + (COEFF3 * (cal*cal))
               - (COEFF4 * (cal*cal*cal)));
    corr = (COEFF1
            - (COEFF2 * samp)
            + (COEFF3 * (samp*samp))
            - (COEFF4 * (samp*samp*samp))) - calterm;

    corr /= 1000.0;
    corr = 1.0 - corr;
    corr = read / corr;

    // now that we have the value change the reading
    ui.corrected->setText(QString::number(corr, 'f', 3));
}
