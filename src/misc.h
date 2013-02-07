/***************************************************************************
  misc.h
  -------------------
  A generic ingredient class
  -------------------
  Copyright 1999-2008, David Johnson
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

#ifndef MISC_H
#define MISC_H

#include <QList>
#include <QMap>
#include <QString>

#include "quantity.h"

class Misc {
public:
    // misc strings
    static const QByteArray YEAST_STRING;
    static const QByteArray FINING_STRING;
    static const QByteArray HERB_STRING;
    static const QByteArray SPICE_STRING;
    static const QByteArray FLAVOR_STRING;
    static const QByteArray ADDITIVE_STRING;
    static const QByteArray OTHER_STRING;

    // default constructor
    Misc();
    // full constructor
    Misc(const QString &name, const Quantity &quantity,
         const QString &typ, const QString &notes);
    // copy constructor
    Misc(const Misc &m);
    // assignment operator
    Misc operator=(const Misc &m);
    // comparison operator
    bool operator==(const Misc &m) const;
    bool operator<(const Misc &m) const;
    // destructor
    ~Misc();

    //get/set name
    const QString &name() const;
    void setName(const QString &name);
    // get/set quantity
    const Quantity &quantity() const;
    void setQuantity(const Quantity &quantity);
    // get/set type
    const QString &type() const;
    void setType(const QString &typ);
    // get/set notes
    const QString &notes() const;
    void setNotes(const QString &notes);

    // list of misc type
    static QStringList typeStringList();

private:
    QString name_;
    Quantity quantity_;
    QString type_;
    QString notes_;
};

typedef QList<Misc> MiscList;
typedef QMap<QString,Misc> MiscMap;

//////////////////////////////////////////////////////////////////////////////
// Inlined Methods

inline bool Misc::operator<(const Misc &m) const { return (name_ < m.name_); }

inline void Misc::setName(const QString &name) { name_ = name; }

inline const QString &Misc::name() const { return name_; }

inline void Misc::setQuantity(const Quantity &quantity) { quantity_ = quantity; }

inline const Quantity &Misc::quantity() const { return quantity_; }

inline void Misc::setType(const QString &typ) { type_ = typ; }

inline const QString &Misc::type() const  { return type_; }

inline void Misc::setNotes(const QString &notes) { notes_ = notes; }

inline const QString &Misc::notes() const{ return notes_; }

#endif // MISC_H
