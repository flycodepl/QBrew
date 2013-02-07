/***************************************************************************
  grain.h
  -------------------
  A grain class
  -------------------
  Copyright 1999-2008, David Johnson
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

#ifndef GRAIN_H
#define GRAIN_H

#include <QList>
#include <QMap>
#include <QStringList>

#include "quantity.h"

class Grain {
public:
    // grain strings
    static const QByteArray EXTRACT_STRING;
    static const QByteArray MASHED_STRING;
    static const QByteArray STEEPED_STRING;
    static const QByteArray GRAIN_STRING;
    static const QByteArray ADJUNCT_STRING;
    static const QByteArray SUGAR_STRING;
    static const QByteArray OTHER_STRING;

    // default constructor
    Grain();
    // constructor
    Grain(const QString &name, const Weight &quantity, const double &extract,
          const double &color, const QString &typ, const QString &use);
    // copy constructor
    Grain(const Grain &g);
    // operators
    Grain operator=(const Grain &g);
    bool operator==(const Grain &g) const;
    bool operator<(const Grain &g) const;
    // destructor
    ~Grain();

    // get/set name
    const QString &name() const;
    void setName(const QString &name);
    // get/set weight
    const Weight &weight() const;
    void setWeight(const Weight &weight);
    // get/set extract
    double extract() const;
    void setExtract(double extract);
    // get/set color
    double color() const;
    void setColor(double color);
    // get/set type
    const QString &type() const;
    void setType(const QString &typ);
    // get/set usage
    const QString &use() const;
    void setUse(const QString &use);

    // return the yield (quantity times extract)
    double yield() const;
    // return the HCU (quantity times color)
    double HCU() const;

    // return a list of use strings
    static QStringList useStringList();
    // return a list of type strings
    static QStringList typeStringList();

private:
    // recalc values
    void recalc();

private:
    QString name_;
    Weight weight_;
    double extract_;
    double color_;
    QString type_;
    QString use_;
    double yield_;
    double hcu_;
};

typedef QList<Grain> GrainList;
typedef QMap<QString,Grain> GrainMap;

//////////////////////////////////////////////////////////////////////////////
// Inlined Methods

inline bool Grain::operator<(const Grain &g) const
    { return (name_<g.name_); }

inline void Grain::setName(const QString &name)
    { name_ = name; }

inline const QString &Grain::name() const
    { return name_; }

inline void Grain::setWeight(const Weight &weight)
    { weight_ = weight; recalc(); }

inline const Weight &Grain::weight() const
    { return weight_; }

inline void Grain::setExtract(double extract)
    { extract_ = extract; recalc(); }

inline double Grain::extract() const
    { return extract_; }

inline void Grain::setColor(double color)
    { color_ = color; recalc(); }

inline double Grain::color() const
    { return color_; }

inline void Grain::setType(const QString &typ)
    { type_ = typ; }

inline const QString &Grain::type() const
    { return type_; }

inline void Grain::setUse(const QString &use)
    { use_ = use; }

inline const QString &Grain::use() const
    { return use_; }

inline double Grain::yield() const
    { return yield_; }

inline double Grain::HCU() const
    { return hcu_; }

#endif // GRAIN_H
