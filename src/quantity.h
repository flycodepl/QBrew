/***************************************************************************
  quantity.h
  -------------------
  Generic quantity class
  -------------------
  Copyright 2001-2008, David Johnson
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

#ifndef QUANTITY_H
#define QUANTITY_H

#include <QMap>
#include <QString>

typedef double (*Conversion)(double);

/**
 * A class to encapsulate the idea of a unit of measure.
 * This is a slight variation of the Unit from Fowler's
 * Quantity analysis pattern.
 */
class Unit {
 public:
    // Constructor
    Unit(const QString &name, const QString &symbol);
    // Destructor
    ~Unit();

    // Return the full name of Unit
    QString name() const;
    // Return the symbol of Unit
    QString symbol() const;

    // Add a conversion function for this Unit
    void addConversion(const Unit &to, const Conversion c);
    // Can we convert to Unit?
    bool convertable(const Unit &u) const;
    // Convert amount to Unit
    double convert(double amount, const Unit &u) const;

    // Return pointer to Unit if it exists
    static Unit* unit(const QString &symbol);

    // equivalence operator
    bool operator==(const Unit &u) const;

 private:
    Unit(const Unit&);
    Unit operator=(const Unit&);

 private:
    QString name_;
    QString symbol_;
    QMap<QString,Conversion> conversions_;
    static QMap<QString,Unit*> units_;
};

/**
 * A class to encapsulate the idea of a measurable quantity.
 * This is a slight variation of Fowler's Quantity analysis pattern.
 */
class Quantity {
  public:
    // Constructor
    Quantity(double amount, const Unit &unit);
    // Default constructor
    Quantity();
    // Construct from string
    Quantity(const QString &from, const Unit &defaultunit = generic);
    // Copy constructor
    Quantity(const Quantity &q);
    // Destructor
    virtual ~Quantity();

    // Return quantity amount
    double amount() const;
    // Return converted amount
    double amount(const Unit &u) const;
    // Return quantity unit
    const Unit& unit() const;
    // Convert quantity to unit
    void convert(const Unit &u);
    // output quantity as a string
    QString toString(int prec=2) const;
    // set quantity from string
    void fromQString(const QString &from, const Unit &defaultunit = generic);

    // equivalence operator
    bool operator==(const Quantity &q) const;

    // assignment operators
    Quantity &operator=(const Quantity &q);
    Quantity &operator+=(const Quantity &q);
    Quantity &operator+=(const double d);
    Quantity &operator-=(const Quantity &q);
    Quantity &operator-=(const double d);

    // arithmetic operators
    friend Quantity operator+(const Quantity &q1, const Quantity &q2);
    friend Quantity operator+(const Quantity &q, const double d);
    friend Quantity operator+(const double d, const Quantity &q);

    friend Quantity operator-(const Quantity &q1, const Quantity &q2);
    friend Quantity operator-(const Quantity &q, const double d);
    friend Quantity operator-(const double d, const Quantity &q);

    friend Quantity operator-(const Quantity &q);

 public:
    static Unit generic;

 protected:
    double amount_;
    Unit const* unit_;
};

class Weight : public Quantity
{
 public:
    // Constructor
    Weight(double amount, const Unit &unit);
    /// Default constructor
    Weight();
    /// Construct from string
    Weight(const QString &from, const Unit &defaultunit = pound);
    // Copy constructor
    Weight(const Weight &w);
    // Conversion constructor
    Weight(const Quantity &q);
    // Destructor
    ~Weight();

 public:
    static Unit gram;
    static Unit kilogram;
    static Unit ounce;
    static Unit pound;

 private:
    // initialize the weight units
    static void initialize();

 private:
    static bool initialized_;
};

class Volume : public Quantity
{
 public:
    // Constructor
    Volume(double amount, const Unit &unit);
    /// Default constructor
    Volume();
    /// Construct from string
    Volume(const QString &from, const Unit &defaultunit = gallon);
    // Copy constructor
    Volume(const Volume &v);
    // Conversion constructor
    Volume(const Quantity &q);
    // destructor
    ~Volume();

 public:
    static Unit barrel;
    static Unit fluidounce;
    static Unit gallon;
    static Unit hectoliter;
    static Unit liter;
    static Unit milliliter;

 private:
    // initialize the weight units
    static void initialize();

 private:
    static bool initialized_;
};

class Temperature : public Quantity
{
 public:
    // Constructor
    Temperature(double amount, const Unit &unit);
    /// Default constructor
    Temperature();
    /// Construct from string
    Temperature(const QString &from, const Unit &defaultunit = fahrenheit);
    // Copy constructor
    Temperature(const Temperature &t);
    // Conversion constructor
    Temperature(const Quantity &q);
    // destructor
    ~Temperature();

 public:
    static Unit fahrenheit;
    static Unit celsius;

 private:
    // initialize the temperature units
    static void initialize();

 private:
    static bool initialized_;
};

// Conversion Prototypes ////////////////////////////////////////////////////

double gram2kilogram(double value);
double gram2ounce(double value);
double gram2pound(double value);
double kilogram2gram(double value);
double kilogram2ounce(double value);
double kilogram2pound(double value);
double ounce2gram(double value);
double ounce2kilogram(double value);
double ounce2pound(double value);
double pound2gram(double value);
double pound2kilogram(double value);
double pound2ounce(double value);

double barrel2fluidounce(double value);
double barrel2gallon(double value);
double barrel2hectoliter(double value);
double barrel2liter(double value);
double barrel2milliliter(double value);
double fluidounce2barrel(double value);
double fluidounce2gallon(double value);
double fluidounce2hectoliter(double value);
double fluidounce2liter(double value);
double fluidounce2milliliter(double value);
double gallon2barrel(double value);
double gallon2fluidounce(double value);
double gallon2hectoliter(double value);
double gallon2liter(double value);
double gallon2milliliter(double value);
double hectoliter2barrel(double value);
double hectoliter2fluidounce(double value);
double hectoliter2gallon(double value);
double hectoliter2liter(double value);
double hectoliter2milliliter(double value);
double liter2barrel(double value);
double liter2fluidounce(double value);
double liter2gallon(double value);
double liter2hectoliter(double value);
double liter2milliliter(double value);
double milliliter2barrel(double value);
double milliliter2fluidounce(double value);
double milliliter2gallon(double value);
double milliliter2hectoliter(double value);
double milliliter2liter(double value);

double fahrenheit2celsius(double value);
double celsius2fahrenheit(double value);

// Inlined Methods //////////////////////////////////////////////////////////

inline QString Unit::name() const
    { return name_; }

inline QString Unit::symbol() const
    { return symbol_; }

inline bool Unit::convertable(const Unit &u) const
    { return (conversions_.contains(u.name())); }

inline Quantity::Quantity(double amount, const Unit &unit)
    : amount_(amount), unit_(&unit) { ; }

inline Quantity::Quantity()
    : amount_(0.0), unit_(&generic) { ; }

inline Quantity::Quantity(const QString &from, const Unit &defaultunit)
    : amount_(0.0), unit_(&defaultunit) { fromQString(from, defaultunit); }

inline Quantity::Quantity(const Quantity &q)
    : amount_(q.amount_), unit_(q.unit_) { ; }

inline Quantity::~Quantity()
    { ; }

inline Quantity &Quantity::operator+=(const double d)
    { amount_ += d; return *this; }

inline Quantity operator+(const double d, const Quantity &q)
    { return (q + d); }

inline Quantity &Quantity::operator-=(const double d)
    { amount_ -= d;  return *this; }

inline double Quantity::amount() const
    { return amount_; }

inline double Quantity::amount(const Unit &u) const
    { return (unit_->convert(amount_, u)); }

inline const Unit& Quantity::unit() const
    { return *unit_; }

inline Weight::Weight(const Weight &w)
    : Quantity(w) { ; }

inline Weight::Weight()
    : Quantity(0.0, pound) { ; }

inline Weight::Weight(const QString &from, const Unit &defaultunit)
    : Quantity(from, defaultunit) { ; }

inline Weight::~Weight()
    { ; }

inline Volume::Volume(const Volume &v)
    : Quantity(v) { ; }

inline Volume::Volume()
    : Quantity(0.0, gallon) { ; }

inline Volume::Volume(const QString &from, const Unit &defaultunit)
    : Quantity(from, defaultunit) { ; }

inline Volume::~Volume()
    { ; }

inline Temperature::Temperature(const Temperature &t)
    : Quantity(t) { ; }

inline Temperature::Temperature()
    : Quantity(0.0, fahrenheit) { ; }

inline Temperature::Temperature(const QString &from, const Unit &defaultunit)
    : Quantity(from, defaultunit) { ; }

inline Temperature::~Temperature()
    { ; }

#endif // QUANTITY_H
