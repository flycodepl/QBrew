/***************************************************************************
  recipe.cpp
  -------------------
  Recipe (document) class
  -------------------
  Copyright 2001-2008, David Johnson
  Please see the header file for copyright and license information
 ***************************************************************************/

#include <cmath>

#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QTextDocument>
#include <QTextStream>

#include "beerxmlreader.h"
#include "data.h"
#include "recipereader.h"
#include "resource.h"
#include "textprinter.h"

#include "recipe.h"

using namespace Resource;

const QByteArray Recipe::EXTRACT_STRING = QT_TRANSLATE_NOOP("recipe", "Extract");
const QByteArray Recipe::PARTIAL_STRING = QT_TRANSLATE_NOOP("recipe", "Partial Mash");
const QByteArray Recipe::ALLGRAIN_STRING = QT_TRANSLATE_NOOP("recipe", "All Grain");

//////////////////////////////////////////////////////////////////////////////
// Construction, destruction                                                //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Recipe()
// --------
// Default constructor

Recipe::Recipe(QObject *parent)
        : QObject(parent), modified_(false), title_(), brewer_(),
          size_(5.0, Volume::gallon), style_(), grains_(),  hops_(), miscs_(),
          recipenotes_(), batchnotes_(), og_(0.0), ibu_(0), srm_(0)
{ ; }

//////////////////////////////////////////////////////////////////////////////
// Recipe()
// --------
// Copy constructor

Recipe::Recipe(const Recipe &r)
    : QObject(0), modified_(r.modified_), title_(r.title_), brewer_(r.brewer_),
      size_(r.size_), style_(r.style_), grains_(r.grains_), hops_(r.hops_),
      miscs_(r.miscs_), recipenotes_(r.recipenotes_),
      batchnotes_(r.batchnotes_), og_(r.og_), ibu_(r.ibu_), srm_(r.srm_)
{
}

//////////////////////////////////////////////////////////////////////////////
// operator=()
// -----------
// Assignment operator
Recipe Recipe::operator=(const Recipe &r)
{
    if (this != &r) {
        modified_ = r.modified_;
        title_ = r.title_;
        brewer_ = r.brewer_;
        size_ = r.size_;
        style_ = r.style_;
        grains_ = r.grains_;
        hops_ = r.hops_;
        miscs_ = r.miscs_;
        recipenotes_ = r.recipenotes_;
        batchnotes_ = r.batchnotes_;
        og_ = r.og_;
        ibu_ = r.ibu_;
        srm_ = r.srm_;
    }
    return *this;
}

Recipe::~Recipe()
{ ; }

//////////////////////////////////////////////////////////////////////////////
// Serialization                                                            //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// newRecipe()
// -------------
// Clears recipe (new document)

void Recipe::newRecipe()
{
    title_.clear();
    brewer_.clear();
    size_ = Data::instance()->defaultSize();
    style_ = Data::instance()->defaultStyle();
    grains_.clear();
    hops_.clear();
    miscs_.clear();
    recipenotes_.clear();
    batchnotes_.clear();

    og_ = 0.0;
    ibu_ = 0;
    srm_ = 0;

    setModified(false); // new documents are not in a modified state
    emit recipeChanged();
}

/////////////////////////////////////////////////////////////////////////////
// nativeFormat()
// -------------
// Is the recipe in native format?

bool Recipe::nativeFormat(const QString &filename)
{
    QFile datafile(filename);
    if (!datafile.open(QFile::ReadOnly | QFile::Text)) {
        // error opening file
        qWarning() << "Error: Cannot open" << filename;
        return false;
    }

    RecipeReader reader(&datafile);
    return reader.isRecipeFormat();
}

/////////////////////////////////////////////////////////////////////////////
// beerXmlFormat()
// ---------------
// Is the recipe in BeerXML format?

bool Recipe::beerXmlFormat(const QString &filename)
{
    QFile datafile(filename);
    if (!datafile.open(QFile::ReadOnly | QFile::Text)) {
        // error opening file
        qWarning() << "Error: Cannot open" << filename;
        return false;
    }

    BeerXmlReader reader(&datafile);
    return reader.isBeerXmlFormat();
}

//////////////////////////////////////////////////////////////////////////////
// loadRecipe()
// --------------
// Load a recipe. Assumes file has been checked with nativeFormat()

bool Recipe::loadRecipe(const QString &filename)
{
    // open file
    QFile datafile(filename);
    if (!datafile.open(QFile::ReadOnly | QFile::Text)) {
        // error opening file
        qWarning() << "Error: Cannot open" << filename;
        QMessageBox::warning(0, TITLE,
                             tr("Cannot read file %1:\n%2")
                             .arg(filename)
                             .arg(datafile.errorString()));
        datafile.close();
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    title_.clear();
    brewer_.clear();
    size_ = Data::instance()->defaultSize();
    style_ = Data::instance()->defaultStyle();
    grains_.clear();
    hops_.clear();
    miscs_.clear();
    recipenotes_.clear();
    batchnotes_.clear();

    // parse file
    RecipeReader reader(&datafile);
    bool status = reader.readRecipe(this);
    datafile.close();

    if (!status) {
        qWarning() << "Error: Problem reading file" << filename;
        qWarning() << reader.errorString();
        QMessageBox::warning(0, TITLE,
                             tr("Error reading file %1").arg(filename));
        QApplication::restoreOverrideCursor();
        return false;
    }

    // calculate the numbers
    recalc();

    // just loaded recipes  are not modified
    setModified(false);
    emit recipeChanged();

    QApplication::restoreOverrideCursor();
    return true;
}

//////////////////////////////////////////////////////////////////////////////
// saveRecipe()
// ---------------
// Save a recipe

bool Recipe::saveRecipe(const QString &filename)
{
    // open file
    QFile datafile(filename);
    if (!datafile.open(QFile::WriteOnly | QFile::Text)) {
        // error opening file
        qWarning() << "Error: Cannot open file" << filename;
        QMessageBox::warning(0, TITLE,
                             tr("Cannot write file %1:\n%2")
                             .arg(filename)
                             .arg(datafile.errorString()));
        datafile.close();
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    // write out xml
    RecipeWriter writer(&datafile);
    writer.writeRecipe(this);
    datafile.close();

    // recipe is saved, so set flags accordingly
    setModified(false);
    QApplication::restoreOverrideCursor();
    return true;
}

//////////////////////////////////////////////////////////////////////////////
// previewRecipe()
// ---------------
// Preview the recipe (assumes textprinter has been setup)

void Recipe::previewRecipe(TextPrinter *textprinter)
{
    if (!textprinter) return;
    QTextDocument document;
    document.setHtml(recipeHTML());
    textprinter->preview(&document);
}

    void previewRecipe(TextPrinter *textprinter, QWidget *wparent);
//////////////////////////////////////////////////////////////////////////////
// printRecipe()
// ---------------
// Print the recipe (assumes textprinter has been setup)

void Recipe::printRecipe(TextPrinter *textprinter)
{
    if (!textprinter) return;
    QTextDocument document;
    document.setHtml(recipeHTML());
    textprinter->print(&document);
}

//////////////////////////////////////////////////////////////////////////////
// Miscellaneous                                                            //
//////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// setStyle()
// ----------
// Set style from string

void Recipe::setStyle(const QString &s)
{
    if (Data::instance()->hasStyle(s))
        style_ = Data::instance()->style(s);
    else
        style_ = Style();
    setModified(true);
}

//////////////////////////////////////////////////////////////////////////////
// addGrain()
// ----------
// Add a grain ingredient to the recipe

void Recipe::addGrain(const Grain &g)
{
    grains_.append(g);
    recalc();
    setModified(true);
}

//////////////////////////////////////////////////////////////////////////////
// addHop()
// ----------
// Add a hop ingredient to the recipe

void Recipe::addHop(const Hop &h)
{
    hops_.append(h);
    recalc();
    setModified(true);
}

//////////////////////////////////////////////////////////////////////////////
// addMisc()
// ----------
// Add a misc ingredient to the recipe

void Recipe::addMisc(const Misc &m)
{
    miscs_.append(m);
    recalc();
    setModified(true);
}

//////////////////////////////////////////////////////////////////////////////
// recipeType()
// ------------
// Return type of recipe

QString Recipe::method()
{
    int extract = 0;
    int mash = 0;

    foreach(Grain grain, grains_) {
        if (grain.use().toLower() == Grain::MASHED_STRING.toLower()) mash++;
        else if (grain.use().toLower() == Grain::EXTRACT_STRING.toLower()) extract++;
    }

    if (mash > 0) {
        if (extract > 0) return PARTIAL_STRING;
        else             return ALLGRAIN_STRING;
    }
    return EXTRACT_STRING;
}

//////////////////////////////////////////////////////////////////////////////
// Calculations                                                             //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// recalc()
// -------
// Recalculate recipe values

void Recipe::recalc()
{
    og_ = calcOG();
    ibu_ = calcIBU();
    srm_ = calcSRM();
}

//////////////////////////////////////////////////////////////////////////////
// calcOG()
// --------
// Calculate the original gravity

double Recipe::calcOG()
{
    double yield;
    double est = 0.0;
    foreach(Grain grain, grains_) {
        yield = grain.yield();
        if (grain.use().toLower() == Grain::MASHED_STRING.toLower()) {
            // adjust for mash efficiency
            yield *= Data::instance()->efficiency();
        } else if (grain.use().toLower() == Grain::STEEPED_STRING.toLower()) {
                // steeped grains don't yield nearly as much as mashed grains
                yield *= Data::instance()->steepYield();
        }
        est += yield;
    }
    if (size_.amount()) {
        est /= size_.amount(Volume::gallon);
    } else {
        est = 0.0;
    }
    return est + 1.0;
}

//////////////////////////////////////////////////////////////////////////////
// calcIBU()
// ---------
// Calculate the bitterness

int Recipe::calcIBU()
{
    // switch between two possible calculations
    if (Data::instance()->tinseth())
        return calcTinsethIBU();
    else
        return calcRagerIBU();
}

//////////////////////////////////////////////////////////////////////////////
// calcRagerIBU()
// --------------
// Calculate the bitterness based on Rager's method (table method)

int Recipe::calcRagerIBU()
{
// TODO: update this (and other) hop calculations
    double bitterness = 0.0;
    foreach(Hop hop, hops_) {
        bitterness += hop.HBU() * Data::instance()->utilization(hop.time());
        // TODO: we should also correct for hop type
    }
    if (size_.amount()) {
        bitterness /= size_.amount(Volume::gallon);
    } else {
        bitterness = 0.0;
    }
    // correct for boil gravity
    if (og_ > 1.050) bitterness /= 1.0 + ((og_ - 1.050) / 0.2);
    return qRound(bitterness);
}

//////////////////////////////////////////////////////////////////////////////
// calcTinsethIBU()
// ----------------
// Calculate the bitterness based on Tinseth's method (formula method)
// The formula used is:
// (1.65*0.000125^(gravity-1))*(1-EXP(-0.04*time))*alpha*mass*1000
// ---------------------------------------------------------------
// (volume*4.15)

// TODO: recheck this formula

int Recipe::calcTinsethIBU()
{
    const double GPO = 28.3495; // grams per ounce
    const double LPG = 3.785;   // liters per gallon

    const double COEFF1 = 1.65;
    const double COEFF2 = 0.000125;
    const double COEFF3 = 0.04;
    const double COEFF4 = 4.15;

    double ibu;
    double bitterness = 0.0;
    foreach(Hop hop, hops_) {
        ibu = (COEFF1 * pow(COEFF2, (og_ - 1.0))) *
            (1.0 - exp(-COEFF3 * hop.time())) *
            (hop.alpha()) * hop.weight().amount(Weight::ounce) * 1000.0;
        if (size_.amount()) {
            ibu /= (size_.amount(Volume::gallon) * COEFF4);
        } else {
            ibu = 0.0;
        }
        bitterness += ibu;
    }
    bitterness *= (GPO / LPG) / 100.0;
    return qRound(bitterness);
}

//////////////////////////////////////////////////////////////////////////////
// calcSRM()
// ---------
// Calculate the color

int Recipe::calcSRM()
{
    double srm = 0.0;
    foreach(Grain grain, grains_) {
        srm += grain.HCU();
    }
    if (size_.amount()) {
        srm /= size_.amount(Volume::gallon);
    } else {
        srm = 0.0;
    }

    // switch between two possible calculations
    if (Data::instance()->morey()) {
        // power model (morey) [courtesy Rob Hudson <rob@tastybrew.com>]
        srm = (pow(srm, 0.6859)) * 1.4922;
        if (srm > 50) srm = 50;
    } else {
        // linear model (daniels)
        if (srm > 8.0) {
            srm *= 0.2;
            srm += 8.4;
        }
    }
    return qRound(srm);
}

// TODO: following formulas need to use constants

//////////////////////////////////////////////////////////////////////////////
// FGEstimate()
// ------------
// Return estimated final gravity

double Recipe::FGEstimate()
{
    if (og_ <= 0.0) return 0.0;
    return (((og_ - 1.0) * 0.25) + 1.0);
}

//////////////////////////////////////////////////////////////////////////////
// ABV()
// -----
// Calculate alcohol by volume

double Recipe::ABV() // recipe version
{
    return (ABV(og_, FGEstimate()));
}

double Recipe::ABV(double og, double fg) // static version
{ 
    return (og - fg) * 1.29;
}

//////////////////////////////////////////////////////////////////////////////
// ABW()
// -----
// Calculate alcohol by weight
// NOTE: Calculations were taken from http://hbd.org/ensmingr/

double Recipe::ABW() // recipe version
{
    return (ABW(og_, FGEstimate()));
}
 
double Recipe::ABW(double og, double fg)  // static version
{
    return (ABV(og, fg) * 0.785);
}

//////////////////////////////////////////////////////////////////////////////
// SgToP()
// -------
// Convert specific gravity to degrees plato

double Recipe::SgToP(double sg)
{
    return ((-463.37) + (668.72*sg) - (205.35 * sg * sg));
}

//////////////////////////////////////////////////////////////////////////////
// extractToYield()
// ----------------
// Convert extract potential to percent yield

// TODO: need to double check this, as well as terms
double Recipe::extractToYield(double extract)
{
    const double SUCROSE = 46.21415;
    return (((extract-1.0)*1000.0) / SUCROSE);
}

//////////////////////////////////////////////////////////////////////////////
// yieldToExtract()
// ----------------
// Convert percent yield to extract potential

double Recipe::yieldToExtract(double yield)
{
    const double SUCROSE = 46.21415;
    return ((yield*SUCROSE)/1000.0)+1.0;
}

//////////////////////////////////////////////////////////////////////////////
// Miscellaneous                                                            //
//////////////////////////////////////////////////////////////////////////////

void Recipe::setModified(bool mod)
{
    modified_ = mod;
    if (mod) emit recipeModified();
}

bool Recipe::modified() const
{
    return modified_;
}
