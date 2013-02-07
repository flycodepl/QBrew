/***************************************************************************
  data.h
  -------------------
  Brewing data
  -------------------
  Copyright 2001-2008 David Johnson
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

#ifndef DATA_H
#define DATA_H

#include <QList>
#include <QString>

#include "configstate.h"
#include "grain.h"
#include "hop.h"
#include "misc.h"
#include "style.h"

class QDomElement;

struct UEntry {
    // hop utilization table entry structure
    unsigned time;
    unsigned utilization;
};

class Data {
public:
    friend class DatabaseTool;

    ~Data();
    static Data *instance();
    void initialize(const ConfigState &state);

    // load the data
    bool loadData(const QString &filename, bool quiet=false);
    // save the data
    bool saveData(const QString &filename);

    // get/set the default size volume
    const Volume &defaultSize() const;
    void setDefaultSize(const Volume &v);
    // get/set the default style
    const Style &defaultStyle() const;
    void setDefaultStyle(const QString &s);
    void setDefaultStyle(const Style &s);
    // get/set default hop type
    const QString &defaultHopType() const;
    void setDefaultHopType(const QString &t);
    // get/set the default grain units
    const Unit &defaultGrainUnit() const;
    void setDefaultGrainUnit(Unit &u);
    // get/set the default hop units
    const Unit &defaultHopUnit() const;
    void setDefaultHopUnit(Unit &u);
    // get/set the default misc units
    const Unit &defaultMiscUnit() const;
    void setDefaultMiscUnit(Unit &u);
    // get/set the default temperature unit
    const Unit &defaultTempUnit() const;
    void setDefaultTempUnit(Unit &u);

    // return stringlists of data
    QStringList stylesList();
    QStringList grainsList();
    QStringList hopsList();
    QStringList miscList();

    // return given data
    Style style(const QString &name);
    Grain grain(const QString &name);
    Hop hop(const QString &name);
    Misc misc(const QString &name);

    // does data exist
    bool hasStyle(const QString &name);
    bool hasGrain(const QString &name);
    bool hasHop(const QString &name);
    bool hasMisc(const QString &name);

    // insert data
    void insertStyle(const Style &style);
    void insertGrain(const Grain &grain);
    void insertHop(const Hop &hop);
    void insertMisc(const Misc &misc);

    // clear data
    void clearStyles();
    void clearGrains();
    void clearHops();
    void clearMiscs();

    // get utilization based on time
    double utilization(unsigned time);
    // add entry to utilization table
    void addUEntry(const UEntry &entry);
    // efficiency
    double efficiency();
    void setEfficiency(double e);
    // steep yield
    double steepYield();
    void setSteepYield(double y);
    // set ibu calc method
    bool tinseth();
    void setTinseth(bool tinseth);
    // set srm calc method
    bool morey();
    void setMorey(bool morey);

private:
    Data();
    Data(const Data&);
    Data &operator=(const Data&);

private:
    friend class DataReader;
    friend class DataWriter;
    static Data *instance_;

    Volume defaultsize_;
    Style defaultstyle_;
    QString defaulthoptype_;
    Unit *defaultgrainunit_;
    Unit *defaulthopunit_;
    Unit *defaultmiscunit_;
    Unit *defaulttempunit_;

    GrainMap grainmap_;
    HopMap hopmap_;
    MiscMap miscmap_;
    StyleMap stylemap_;

    QList<UEntry> utable_;
    double steepyield_;
    double efficiency_;
    bool tinseth_;
    bool morey_;
};

// Inlined Methods ///////////////////////////////////////////////////////////

inline const QString &Data::defaultHopType() const { return defaulthoptype_; }

inline void Data::setDefaultHopType(const QString &t) { defaulthoptype_ = t; }

inline const Volume &Data::defaultSize() const { return defaultsize_; }

inline void Data::setDefaultSize(const Volume &v) { defaultsize_ = v; }

inline const Style &Data::defaultStyle() const { return defaultstyle_; }

inline void Data::setDefaultStyle(const Style &s) { defaultstyle_ = s; }

inline const Unit &Data::defaultGrainUnit() const { return *defaultgrainunit_; }

inline void Data::setDefaultGrainUnit(Unit &u) { defaultgrainunit_ = &u; }

inline const Unit &Data::defaultHopUnit() const { return *defaulthopunit_; }

inline void Data::setDefaultHopUnit(Unit &u) { defaulthopunit_ = &u; }

inline const Unit &Data::defaultMiscUnit() const { return *defaultmiscunit_; }

inline void Data::setDefaultMiscUnit(Unit &u) { defaultmiscunit_ = &u; }

inline const Unit &Data::defaultTempUnit() const { return *defaulttempunit_; }

inline void Data::setDefaultTempUnit(Unit &u) { defaulttempunit_ = &u; }

inline QStringList Data::stylesList() { return stylemap_.keys(); }

inline QStringList Data::grainsList() { return grainmap_.keys(); }

inline QStringList Data::hopsList() { return hopmap_.keys(); }

inline QStringList Data::miscList() { return miscmap_.keys(); }

inline bool Data::hasStyle(const QString &name) { return stylemap_.contains(name); }

inline bool Data::hasGrain(const QString &name) { return grainmap_.contains(name); }

inline bool Data::hasHop(const QString &name) { return hopmap_.contains(name); }

inline bool Data::hasMisc(const QString &name) { return miscmap_.contains(name); }

inline void Data::insertStyle(const Style &style)
    { stylemap_.insert(style.name(), style); }

inline void Data::insertGrain(const Grain &grain)
    { grainmap_.insert(grain.name(), grain); }

inline void Data::insertHop(const Hop &hop)
    { hopmap_.insert(hop.name(), hop); }

inline void Data::insertMisc(const Misc &misc)
    { miscmap_.insert(misc.name(), misc); }

inline void Data::clearStyles() { stylemap_.clear(); }

inline void Data::clearGrains() { grainmap_.clear(); }

inline void Data::clearHops() { hopmap_.clear(); }

inline void Data::clearMiscs() { miscmap_.clear(); }

inline double Data::efficiency() { return efficiency_; }

inline void Data::setEfficiency(double e) { efficiency_ = e; }

inline double Data::steepYield() { return steepyield_; }

inline void Data::setSteepYield(double y) { steepyield_ = y; }

inline bool Data::tinseth() { return tinseth_; }

inline void Data::setTinseth(bool tinseth) { tinseth_ = tinseth; }

inline bool Data::morey() { return morey_; }

inline void Data::setMorey(bool morey) { morey_ = morey; }

#endif // DATA_H
