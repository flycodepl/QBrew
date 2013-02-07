/***************************************************************************
  data.cpp
  -------------------
  Brewing data
  -------------------
  Copyright 2001-2008 David Johnson
  Please see the header file for copyright and license information.
 ***************************************************************************/

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QMutex>
#include <QStringList>
#include <QTextStream>

#include "datareader.h"
#include "qbrew.h"
#include "recipe.h"
#include "resource.h"

#include "data.h"

using namespace Resource;

Data *Data::instance_ = 0;
static QMutex instancelock;

// Construction, Destruction /////////////////////////////////////////////////

// Private constructor
Data::Data()
    : defaultsize_(Volume(5.0, Volume::gallon)), defaultstyle_(),
      defaulthoptype_(Hop::PELLET_STRING), defaultgrainunit_(&Weight::pound),
      defaulthopunit_(&Weight::ounce),  defaultmiscunit_(&Quantity::generic),
      grainmap_(), hopmap_(), miscmap_(), stylemap_(), utable_(),
      steepyield_(0.5), efficiency_(0.75), tinseth_(true), morey_(true)
{ ; }

// Private destructor
Data::~Data() { ; }

// Return pointer to the data
Data *Data::instance()
{
    if (!instance_) {
        QMutexLocker lock(&instancelock);
        if (!instance_) instance_ = new Data();
    }
    return instance_;
}

// Initialize
void Data::initialize(const ConfigState &state)
{
    // set defaults
    if (state.calc.units == UNIT_METRIC) {
        setDefaultSize(Volume(state.recipe.batch, Volume::liter));
        setDefaultGrainUnit(Weight::kilogram);
        setDefaultHopUnit(Weight::gram);
        setDefaultTempUnit(Temperature::celsius);
    } else if (state.calc.units == UNIT_US) {
        setDefaultSize(Volume(state.recipe.batch, Volume::gallon));
        setDefaultGrainUnit(Weight::pound);
        setDefaultHopUnit(Weight::ounce);
        setDefaultTempUnit(Temperature::fahrenheit);
    }
    setDefaultStyle(state.recipe.style);
    setDefaultHopType(state.recipe.hoptype);
    setDefaultMiscUnit(Quantity::generic);
    setEfficiency(state.calc.efficiency);

    // load data file - look in standard locations
    // TODO: use QDesktopServices in next non-bugfix release (0.5.0)
    bool status = false;
    // try home directory first
    if (!status) status = loadData(QDIR_HOME + "/." + DATA_FILE, true);
    // then try system data directory
    if (!status) status = loadData(QBrew::instance()->dataBase() + DATA_FILE);

    if (!status) {
        qWarning() << "Warning: could not open data file";
        QMessageBox::warning(0, TITLE, QObject::tr("Could not open data file"));
    }
}

//////////////////////////////////////////////////////////////////////////////
// Data Access                                                              //
//////////////////////////////////////////////////////////////////////////////

void Data::setDefaultStyle(const QString &style)
{
    if (stylemap_.contains(style)) defaultstyle_ = stylemap_.value(style);
}

//////////////////////////////////////////////////////////////////////////////
// style()
// -------
// Return a style given its name

Style Data::style(const QString &name)
{
    if (stylemap_.contains(name)) return stylemap_.value(name);
    return Style();
}

//////////////////////////////////////////////////////////////////////////////
// grain()
// -------
// Return grain given its name

Grain Data::grain(const QString &name)
{
    Grain grain;
    if (grainmap_.contains(name)) grain = grainmap_.value(name);
    grain.setWeight(Weight(1.0, *defaultgrainunit_));
    return grain;
}

//////////////////////////////////////////////////////////////////////////////
// hop()
// -----
// Return hop given its name

Hop Data::hop(const QString &name)
{
    Hop hop;
    if (hopmap_.contains(name)) hop = hopmap_.value(name);
    hop.setWeight(Weight(1.0, *defaulthopunit_));
    hop.setType(defaulthoptype_);
    return hop;
}

//////////////////////////////////////////////////////////////////////////////
// misc()
// ------
// Return misc ingredient given its name

Misc Data::misc(const QString &name)
{
    Misc misc;
    if (miscmap_.contains(name)) misc = miscmap_.value(name);
    misc.setQuantity(Quantity(1.0, *defaultmiscunit_));
    return misc;
}

//////////////////////////////////////////////////////////////////////////////
// utilization
// -----------
// Look up the utilization for the given time

double Data::utilization(unsigned time)
{
    foreach(UEntry entry, utable_) {
        if (time >= entry.time) return (double(entry.utilization));
    }
    return 0.0;
}

//////////////////////////////////////////////////////////////////////////////
// addUEntry()
// -----------
// Add an entry to the utilization table [static]

void Data::addUEntry(const UEntry &entry)
{
    // keep the list sorted from highest time to lowest
    QList<UEntry>::Iterator it;
    if (utable_.isEmpty()) {
        utable_.append(entry);
    } else {
        for (it=utable_.begin(); it != utable_.end(); ++it) {
            if ((*it).time < entry.time) break;
        }
        utable_.insert(it, entry);
    }
}

//////////////////////////////////////////////////////////////////////////////
// Serialization                                                            //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// loadData()
// ----------
// Load the data

bool Data::loadData(const QString &filename, bool quiet)
{
    // open file
    QFile datafile(filename);
    if (!datafile.open(QFile::ReadOnly | QFile::Text)) {
        // error opening file
        if (!quiet) {
            qWarning() << "Error: Cannot read" << filename;
            QMessageBox::warning(0, TITLE,
                                 QObject::tr("Cannot read file %1:\n%2")
                                 .arg(filename)
                                 .arg(datafile.errorString()));
        }
        datafile.close();
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    // parse file
    DataReader reader(&datafile);
    bool status = reader.readData(this);
    datafile.close();

    if (!status) {
        qWarning() << "Error: Problem reading file" << filename;
        qWarning() << reader.errorString();
        QMessageBox::warning(0, TITLE,
                             QObject::tr("Error reading file %1").arg(filename));
        QApplication::restoreOverrideCursor();
        return false;
    }

    QApplication::restoreOverrideCursor();
    return true;
}

//////////////////////////////////////////////////////////////////////////////
// saveData()
// ------------
// Save info to data file

bool Data::saveData(const QString &filename)
{
    // open file
    QFile datafile(filename);
    if (!datafile.open(QFile::WriteOnly | QFile::Text)) {
        // error opening file
        qWarning() << "Error: Cannot open file" << filename;
        QMessageBox::warning(0, TITLE,
                             QObject::tr("Cannot write file %1:\n%2")
                             .arg(filename)
                             .arg(datafile.errorString()));
        datafile.close();
        QApplication::restoreOverrideCursor();
        return false;
    }

    // write it out
    DataWriter writer(&datafile);
    writer.writeData(this);
    datafile.close();

    QApplication::restoreOverrideCursor();
    return true;
}
