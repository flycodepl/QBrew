/***************************************************************************
  misc.cpp
  -------------------
  A generic ingredient class
  -------------------
  Copyright 1999-2008, David Johnson
  Please see the header file for copyright and license information
 ***************************************************************************/

#include <QStringList>
#include "resource.h"
#include "misc.h"

using namespace Resource;

const QByteArray Misc::YEAST_STRING    = QT_TRANSLATE_NOOP("misc", "Yeast");
const QByteArray Misc::FINING_STRING   = QT_TRANSLATE_NOOP("misc", "Fining");
const QByteArray Misc::HERB_STRING     = QT_TRANSLATE_NOOP("misc", "Herb");
const QByteArray Misc::SPICE_STRING    = QT_TRANSLATE_NOOP("misc", "Spice");
const QByteArray Misc::FLAVOR_STRING   = QT_TRANSLATE_NOOP("misc", "Flavor");
const QByteArray Misc::ADDITIVE_STRING = QT_TRANSLATE_NOOP("misc", "Additive");
const QByteArray Misc::OTHER_STRING    = QT_TRANSLATE_NOOP("misc", "Other");

//////////////////////////////////////////////////////////////////////////////
// Misc()
// ------
// constructor

Misc::Misc()
    : name_(QObject::tr("Generic")), quantity_(), type_(OTHER_STRING), notes_()
{ ; }

Misc::Misc(const QString &name, const Quantity &quantity, const QString &typ,
           const QString &notes)
    : name_(name), quantity_(quantity),  type_(typ),
      notes_(notes)
{ ; }

Misc::~Misc() { ; }

//////////////////////////////////////////////////////////////////////////////
// Misc()
// ------
// copy constructor

Misc::Misc(const Misc &m)
    : name_(m.name_), quantity_(m.quantity_), type_(m.type_), notes_(m.notes_)
{ ; }

//////////////////////////////////////////////////////////////////////////////
// operator=()
// -----------
// Assignment operator

Misc Misc::operator=(const Misc &m)
{
    if (this != &m) {
        name_ = m.name_;
        quantity_ = m.quantity_;
        type_ = m.type_;
        notes_ = m.notes_;
    }
    return *this;
}

//////////////////////////////////////////////////////////////////////////////
// operator==()
// -----------
// Equivalence operator

bool Misc::operator==(const Misc &m) const
{
    return (name_ == m.name_)
        && (quantity_ == m.quantity_)
        && (type_ == m.type_)
        && (notes_ == m.notes_);
}

///////////////////////////////////////////////////////////////////////////////
// typeStringList()
// ----------------
// Return string list of misc types

QStringList Misc::typeStringList()
{
    return (QStringList()
            << YEAST_STRING
            << FINING_STRING
            << HERB_STRING
            << SPICE_STRING
            << FLAVOR_STRING
            << ADDITIVE_STRING
            << OTHER_STRING);
}
