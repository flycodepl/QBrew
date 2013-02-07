/***************************************************************************
  quantity.h
  -------------------
  Generic quantity class
  -------------------
  Copyright 2001-2008, David Johnson
  Please see the header file for copyright and license information
 ***************************************************************************/

#include "quantity.h"

// Unit Class ///////////////////////////////////////////////////////////////

// our static map units
QMap<QString,Unit*> Unit::units_;

/**
 * Full constructor. Construct a Unit from name and symbol
 * and add to map.
 */
Unit::Unit(const QString &name, const QString &symbol)
    : name_(name), symbol_(symbol), conversions_()
{
    units_.insert(symbol_, this);
}

/**
 * Destructor. Removes Unit from map.
 */
inline Unit::~Unit()
{
    units_.remove(symbol_);
}

/**
 * Add a conversion function for this Unit.
 */
void Unit::addConversion(const Unit &u, const Conversion c)
{
    conversions_.insert(u.name(), c);
}

/**
 * Convert to amount of other units to this Unit.
 */
double Unit::convert(double amount, const Unit &other) const
{
    // if converting from self...
    if (other.name_ == name_) return amount;
    
    if (conversions_.contains(other.name())) {
        return (conversions_.value(other.name()))(amount);
    }
    // TODO: throw exception instead of returning 0.0
    return 0.0;
}

/**
 * Return Unit associated with symbol.
 */
Unit* Unit::unit(const QString &symbol)
{
    if (units_.contains(symbol))
        return units_.value(symbol);
    else
        return 0;
}

/**
 * Equivalence operator.
 */
bool Unit::operator==(const Unit &u) const
{
    return ((name_ == u.name_) && (symbol_ == u.symbol_));
}

// Quantity Class ///////////////////////////////////////////////////////////

Unit Quantity::generic("unit", "unit");

/**
 * Assignment operator.
 */
Quantity &Quantity::operator=(const Quantity &q)
{
    if (this != &q) {
        amount_ = q.amount_;
        unit_ = q.unit_;
    }
    return *this;
}

/**
 * Equivalence operator.
 */
bool Quantity::operator==(const Quantity &q) const
{
    return ((amount_ == q.amount_) &&
            (unit_->symbol() == q.unit_->symbol()));
}

/**
 * Convert Quantity to unit.
 */
void Quantity::convert(const Unit &u)
{
    amount_ = unit_->convert(amount_, u);
    unit_ = &u;
    // TODO: change to bool return, and return false if not convertable
}

/**
 * Output quantity as a string, output is "xx.xx symbol".
 */
QString Quantity::toString(int prec) const
{
    return (QString::number(amount_, 'f', prec) + " " + unit_->symbol());
}

/**
 * Set quantity from string (assumes format of "%f %s")
 */
void Quantity::fromQString(const QString &from, const Unit &defaultunit)
{
    QString str = from.section(' ', 1, -1);
    unit_ =  Unit::unit(str);
    if (!unit_) unit_ = &defaultunit;

    str = from.section(' ', 0, 0);
    amount_ = str.toDouble();
}

// Addition operators /////////////////

Quantity &Quantity::operator+=(const Quantity &q)
{
    if (unit_->convertable(*q.unit_)) {
        amount_ += q.unit_->convert(q.amount_, *unit_);
    }
    return *this;
}

Quantity operator+(const Quantity &q1, const Quantity &q2)
{
    Quantity temp(q1);
    if (temp.unit_->convertable(*q2.unit_)) {
        temp.amount_ += q2.unit_->convert(q2.amount_, *temp.unit_);
    }
    return temp;
}

Quantity operator+(const Quantity &q, const double d)
{
    Quantity temp(q);
    temp.amount_ += d;
    return temp;
}

// Subtraction operators //////////////

Quantity &Quantity::operator-=(const Quantity &q)
{
    if (unit_->convertable(*q.unit_)) {
        amount_ -= q.unit_->convert(q.amount_, *unit_);
    }
    return *this;
}

Quantity operator-(const Quantity &q1, const Quantity &q2)
{
    Quantity temp(q1);
    if (temp.unit_->convertable(*q2.unit_)) {
        temp.amount_ += q2.unit_->convert(q2.amount_, *temp.unit_);
    }
    return temp;
}

Quantity operator-(const Quantity &q, const double d)
{
    Quantity temp(q);
    temp.amount_ -= d;
    return temp;
}

Quantity operator-(const double d, const Quantity &q)
{
    Quantity temp(q);
    temp.amount_ = d - temp.amount_;
    return temp;
}

// Misc operators /////////////////////

Quantity operator-(const Quantity &q)
{
    Quantity temp(q);
    temp.amount_ *= -1.0;
    return temp;
}

// Weight Class //////////////////////////////////////////////////////////

Unit Weight::gram("gram", "g");
Unit Weight::kilogram("kilogram", "kg");
Unit Weight::ounce("ounce", "oz");
Unit Weight::pound("pound", "lb");

bool Weight::initialized_ = false;

/**
 * Constructor.
 */
Weight::Weight(double amount, const Unit &unit) 
    : Quantity(amount, unit)
{
    if (!initialized_) initialize();
}

/**
 * Conversion constructor. Converts from base Quantity.
 */
Weight::Weight(const Quantity &q)
    : Quantity(q)
{
    if (!initialized_) initialize();
    // TODO: throw if nonconvertable
}

/**
 * Initialize the class units.
 */
void Weight::initialize()
{
    gram.addConversion(kilogram, gram2kilogram);
    gram.addConversion(ounce, gram2ounce);
    gram.addConversion(pound, gram2pound);
    kilogram.addConversion(gram, kilogram2gram);
    kilogram.addConversion(ounce, kilogram2ounce);
    kilogram.addConversion(pound, kilogram2pound);
    ounce.addConversion(gram, ounce2gram);
    ounce.addConversion(kilogram, ounce2kilogram);
    ounce.addConversion(pound, ounce2pound);
    pound.addConversion(gram, pound2gram);
    pound.addConversion(kilogram, pound2kilogram);
    pound.addConversion(ounce, pound2ounce);
    initialized_ = true;
}

// Volume Class //////////////////////////////////////////////////////////

Unit Volume::barrel("barrel", "bbl");
Unit Volume::gallon("gallon", "gal");
Unit Volume::hectoliter("hectoliter", "hL");
Unit Volume::liter("liter", "L");
Unit Volume::milliliter("milliliter", "mL");
Unit Volume::fluidounce("fluid ounce", "fl oz");

bool Volume::initialized_ = false;

/**
 * Constructor.
 */
Volume::Volume(double amount, const Unit &unit) 
    : Quantity(amount, unit)
{
    if (!initialized_) initialize();
}

/**
 * Conversion constructor. Converts from base Quantity.
 */
Volume::Volume(const Quantity &q)
    : Quantity(q)
{
    if (!initialized_) initialize();
    // TODO: throw if nonconvertable
}

/**
 * Initialize the class units.
 */
void Volume::initialize()
{
    barrel.addConversion(fluidounce, barrel2fluidounce);
    barrel.addConversion(gallon, barrel2gallon);
    barrel.addConversion(hectoliter, barrel2hectoliter);
    barrel.addConversion(liter, barrel2liter);
    barrel.addConversion(milliliter, barrel2milliliter);
    fluidounce.addConversion(barrel, fluidounce2barrel);
    fluidounce.addConversion(gallon, fluidounce2gallon);
    fluidounce.addConversion(hectoliter, fluidounce2hectoliter);
    fluidounce.addConversion(liter, fluidounce2liter);
    fluidounce.addConversion(milliliter, fluidounce2milliliter);
    gallon.addConversion(barrel, gallon2barrel);
    gallon.addConversion(fluidounce, gallon2fluidounce);
    gallon.addConversion(hectoliter, gallon2hectoliter);
    gallon.addConversion(liter, gallon2liter);
    gallon.addConversion(milliliter, gallon2milliliter);
    hectoliter.addConversion(barrel, hectoliter2barrel);
    hectoliter.addConversion(fluidounce, hectoliter2fluidounce);
    hectoliter.addConversion(gallon, hectoliter2gallon);
    hectoliter.addConversion(liter, hectoliter2liter);
    hectoliter.addConversion(milliliter, hectoliter2milliliter);
    liter.addConversion(barrel, liter2barrel);
    liter.addConversion(fluidounce, liter2fluidounce);
    liter.addConversion(gallon, liter2gallon);
    liter.addConversion(hectoliter, liter2hectoliter);
    liter.addConversion(milliliter, liter2milliliter);
    milliliter.addConversion(barrel, milliliter2barrel);
    milliliter.addConversion(fluidounce, milliliter2fluidounce);
    milliliter.addConversion(gallon, milliliter2gallon);
    milliliter.addConversion(hectoliter, milliliter2hectoliter);
    milliliter.addConversion(liter, milliliter2liter);
    initialized_ = true;
}

// Temperature Class //////////////////////////////////////////////////////////

Unit Temperature::fahrenheit("fahrenheit", "F");
Unit Temperature::celsius("celsius", "C");

bool Temperature::initialized_ = false;

/**
 * Constructor
 */
Temperature::Temperature(double amount, const Unit &unit) 
    : Quantity(amount, unit)
{
    if (!initialized_) initialize();
}

/**
 * Conversion constructor. Converts from base Quantity.
 */
Temperature::Temperature(const Quantity &q)
    : Quantity(q)
{
    if (!initialized_) initialize();
    // TODO: throw if nonconvertable
}

/**
 * Initialize the class units.
 */
void Temperature::initialize()
{
    fahrenheit.addConversion(celsius, fahrenheit2celsius);
    celsius.addConversion(fahrenheit, celsius2fahrenheit);
}

// Conversion Functions //////////////////////////////////////////////////

// TODO: double check formulas

double gram2kilogram(double value)
    { return value / 1000.0; }

double gram2ounce(double value)
    { return value / 28.349523125; }

double gram2pound(double value)
    { return value / 453.59237; }

double kilogram2gram(double value)
    { return value * 1000.0 ; }

double kilogram2ounce(double value)
    { return value * 35.27396; }

double kilogram2pound(double value)
    { return value * 2.2046226; }

double ounce2gram(double value)
    { return value * 28.349523125; }

double ounce2kilogram(double value)
    { return value / 35.27396; }

double ounce2pound(double value)
    { return value / 16.0; }

double pound2gram(double value)
    { return value * 453.59237; }

double pound2kilogram(double value)
    { return value / 2.2046226; }

double pound2ounce(double value)
    { return value * 16.0; }

double barrel2fluidounce(double value)
    { return value * 3968.0; }

double barrel2gallon(double value)
    { return value * 31.0; }

double barrel2hectoliter(double value)
    { return value * 1.173477658; }

double barrel2liter(double value)
    { return value * 117.3477658; }

double barrel2milliliter(double value)
    { return value * 117347.7658; }

double fluidounce2barrel(double value)
    { return value / 3968.0; }

double fluidounce2gallon(double value)
    { return value / 128.0; }

double fluidounce2hectoliter(double value)
    { return value / 3381.4016; }

double fluidounce2liter(double value)
    { return value / 33.814016; }

double fluidounce2milliliter(double value)
    { return value * 29.5735; }

double gallon2barrel(double value)
    { return value / 31.0; }

double gallon2fluidounce(double value)
    { return value * 128.0; }

double gallon2hectoliter(double value)
    { return value / 26.4172; }

double gallon2liter(double value)
    { return value * 3.7854118; }

double gallon2milliliter(double value)
    { return value * 3785.4118; }

double hectoliter2barrel(double value)
    { return value / 1.173477658; }

double hectoliter2fluidounce(double value)
    { return value * 3381.4016; }

double hectoliter2gallon(double value)
    { return value * 26.4172; }

double hectoliter2liter(double value)
    { return value * 100.0; }

double hectoliter2milliliter(double value)
    { return value * 100000.0; }

double liter2barrel(double value)
    { return value / 117.3477658; }

double liter2fluidounce(double value)
    { return value * 33.814016; }

double liter2gallon(double value)
    { return value / 3.7854118; }

double liter2hectoliter(double value)
    { return value / 100.0; }

double liter2milliliter(double value)
    { return value * 1000.0; }

double milliliter2barrel(double value)
    { return value / 117347.7658; }

double milliliter2fluidounce(double value)
    { return value / 29.5735; }

double milliliter2gallon(double value)
    { return value / 3785.4118; }

double milliliter2hectoliter(double value)
    { return value / 100000.0; }

double milliliter2liter(double value)
    { return value / 1000.0; }

double fahrenheit2celsius(double value)
    { return (value - 32.0) / 1.8; }

double celsius2fahrenheit(double value)
    { return (value * 1.8) + 32.0; }
