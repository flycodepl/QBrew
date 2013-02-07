/***************************************************************************
  hop.h
  -------------------
  A hop class
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

#ifndef HOP_H
#define HOP_H

#include <QList>
#include <QMap>
#include <QString>

#include "quantity.h"

class Hop {
public:
    // hop strings
    // TODO: make sure am comparing lowercase / handle lowercase, because old format uses it!!!
    static const QByteArray PELLET_STRING;
    static const QByteArray PLUG_STRING;
    static const QByteArray WHOLE_STRING;

    // default constructor
    Hop();
    // full constructor
    Hop(const QString &name, const Weight &weight,
        const QString &typ, const double &alpha,
        const unsigned &time);
    // copy constructor
    Hop(const Hop &h);
    // operators
    Hop operator=(const Hop &h);
    bool operator==(const Hop &h) const;
    bool operator<(const Hop &h) const;
    // destructor
    ~Hop();

    // get/set name
    const QString &name() const;
    void setName(const QString &name);
    // get/set weight
    const Weight &weight() const;
    void setWeight(const Weight &weight);
    // get/set alpha content
    void setAlpha(double alpha);
    double alpha() const;
    //get/set boil time
    unsigned time() const;
    void setTime(unsigned time);
    // get/set type
    const QString &type() const;
    void setType(const QString &typ);

    // return precalculated value for bitterness
    double HBU();

    // return hop type
    static QStringList typeStringList();

private:
    // recalc values
    void recalc();

private:
    QString name_;
    Weight weight_;
    QString type_;
    double alpha_;
    unsigned time_;
    double hbu_;
};

typedef QList<Hop> HopList;
typedef QMap<QString,Hop> HopMap;

//////////////////////////////////////////////////////////////////////////////
// Inlined Operators

inline bool Hop::operator<(const Hop &h) const
    { return (name_ < h.name_); }

inline void Hop::setName(const QString &name)
    { name_ = name; }

inline const QString &Hop::name() const
    { return name_; }

inline void Hop::setWeight(const Weight &weight)
    { weight_ = weight; recalc(); }

inline const Weight &Hop::weight() const
    { return weight_; }

inline void Hop::setType(const QString &typ)
    { type_ = typ; }

inline const QString &Hop::type() const
    { return type_; }

inline void Hop::setAlpha(double alpha)
    { alpha_ = alpha; recalc(); }

inline double Hop::alpha() const
    { return alpha_; }

inline void Hop::setTime(unsigned time)
    { time_ = time; }

inline unsigned Hop::time() const
    { return time_; }

inline double Hop::HBU()
    { return hbu_; }

#endif // HOP_H
