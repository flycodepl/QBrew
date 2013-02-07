/***************************************************************************
  grain.cpp
  -------------------
  A grain class
  -------------------
  Copyright 1999-2008, David Johnson
  Please see the header file for copyright and license information
 ***************************************************************************/

#include "resource.h"
#include "grain.h"

using namespace Resource;

const QByteArray Grain::EXTRACT_STRING = QT_TRANSLATE_NOOP("grain", "Extract");
const QByteArray Grain::MASHED_STRING  = QT_TRANSLATE_NOOP("grain", "Mashed");
const QByteArray Grain::STEEPED_STRING = QT_TRANSLATE_NOOP("grain", "Steeped");
const QByteArray Grain::GRAIN_STRING   = QT_TRANSLATE_NOOP("grain", "Grain");
const QByteArray Grain::ADJUNCT_STRING = QT_TRANSLATE_NOOP("grain", "Adjunct");
const QByteArray Grain::SUGAR_STRING   = QT_TRANSLATE_NOOP("grain", "Sugar");
const QByteArray Grain::OTHER_STRING   = QT_TRANSLATE_NOOP("grain", "Other");

//////////////////////////////////////////////////////////////////////////////
// Grain()
// -------
// Constructor

Grain::Grain()
    : name_(QObject::tr("Generic")), weight_(), extract_(1.025), color_(1.0),
      type_(GRAIN_STRING), use_(OTHER_STRING), yield_(0.0), hcu_(0.0)
{ ; }

Grain::Grain(const QString &name, const Weight &weight, const double &extract,
             const double &color, const QString &typ, const QString &use)
    : name_(name), weight_(weight), extract_(extract), color_(color),
      type_(typ), use_(use)
{
    recalc();
}

Grain::~Grain() { ; }

//////////////////////////////////////////////////////////////////////////////
// Grain(const Grain&)
// --------------------
// Copy Constructor

Grain::Grain(const Grain &g)
    : name_(g.name_), weight_(g.weight_), extract_(g.extract_),
      color_(g.color_), type_(g.type_), use_(g.use_), yield_(g.yield_),
      hcu_(g.hcu_)
{ ; }

//////////////////////////////////////////////////////////////////////////////
// operator=()
// -----------
// Assignment operator

Grain Grain::operator=(const Grain &g)
{
    if (this != &g) {
        name_ = g.name_;
        weight_ = g.weight_;
        extract_ = g.extract_;
        color_ = g.color_;
        type_ = g.type_;
        use_ = g.use_;
        yield_ = g.yield_;
        hcu_ = g.hcu_;
    }
    return *this;
}

//////////////////////////////////////////////////////////////////////////////
// operator==()
// ------------
// Equivalence operator

bool Grain::operator==(const Grain &g) const
{
    return (name_ == g.name_)
        && (weight_ == g.weight_)
        && (extract_ == g.extract_)
        && (color_ == g.color_)
        && (type_ == g.type_)
        && (use_ == g.use_);
}

///////////////////////////////////////////////////////////////////////////////
// typeStringList()
// ----------------
// Return string list of grain types

QStringList Grain::typeStringList()
{
    return (QStringList()
            << GRAIN_STRING
            << EXTRACT_STRING
            << ADJUNCT_STRING
            << SUGAR_STRING
            << OTHER_STRING);
}

///////////////////////////////////////////////////////////////////////////////
// useStringList()
// -----------
// Return string list of usage types

QStringList Grain::useStringList()
{
    return (QStringList()
            << EXTRACT_STRING
            << MASHED_STRING
            << STEEPED_STRING
            << OTHER_STRING);
}

///////////////////////////////////////////////////////////////////////////////
// recalc()
// --------
// Recalculate internal values

void Grain::recalc()
{
    yield_ = weight_.amount(Weight::pound) * (extract_ - 1.000);
    hcu_ = weight_.amount(Weight::pound) * color_;
}
