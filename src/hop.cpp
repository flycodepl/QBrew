/***************************************************************************
  hop.cpp
  -------------------
  A hop class
  -------------------
  Copyright 1999-2008, David Johnson
  Please see the header file for copyright and license information
 ***************************************************************************/

#include <QStringList>
#include "resource.h"
#include "hop.h"

const QByteArray Hop::PELLET_STRING = QT_TRANSLATE_NOOP("Hop", "Pellet");
const QByteArray Hop::PLUG_STRING   = QT_TRANSLATE_NOOP("Hop", "Plug");
const QByteArray Hop::WHOLE_STRING  = QT_TRANSLATE_NOOP("Hop", "Whole");

//////////////////////////////////////////////////////////////////////////////
// Hop()
// -----
// constructor

Hop::Hop()
    : name_(QObject::tr("Generic")), weight_(), type_(PELLET_STRING),
      alpha_(10.0),  time_(0)
{ ; }

Hop::Hop(const QString &name, const Weight &weight, const QString &typ,
         const double &alpha, const unsigned &time)
    : name_(name), weight_(weight), type_(typ), alpha_(alpha), time_(time)
      
{
    recalc();
}

Hop::~Hop() { ; }

//////////////////////////////////////////////////////////////////////////////
// Hop(const Hop &)
// ----------------
// Copy constructor

Hop::Hop(const Hop &h)
    : name_(h.name_), weight_(h.weight_), type_(h.type_), alpha_(h.alpha_),
      time_(h.time_), hbu_(h.hbu_)
{ ; }

//////////////////////////////////////////////////////////////////////////////
// operator=()
// -----------
// Assignment operator

Hop Hop::operator=(const Hop &h)
{
    if (this != &h) {
        name_ = h.name_;
        weight_ = h.weight_;
        type_ = h.type_;
        alpha_ = h.alpha_;
        time_ = h.time_;
        hbu_ = h.hbu_;
    }
    return *this;
}

//////////////////////////////////////////////////////////////////////////////
// operator==()
// ------------
// Equivalence operator

bool Hop::operator==(const Hop &h) const
{
    return (name_ == h.name_)
        && (weight_ == h.weight_)
        && (type_ == h.type_)
        && (alpha_ == h.alpha_)
        && (time_ == h.time_);
}

//////////////////////////////////////////////////////////////////////////////
// typeStringList()
// ----------------
// Return string list of available hop types

QStringList Hop::typeStringList()
{
    return (QStringList() << PELLET_STRING << PLUG_STRING << WHOLE_STRING);
}

//////////////////////////////////////////////////////////3/////////////////////
// recalc()
// --------
// Recalculate internal values

void Hop::recalc()
{
    hbu_ = 0.75 * alpha_ * weight_.amount(Weight::ounce);
}
