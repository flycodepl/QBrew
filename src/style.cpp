/***************************************************************************
  style.cpp
  -------------------
  AHA-like style class
  -------------------
  Copyright 1999-2008, David Johnson
  Please see the header file for copyright and license information
 ***************************************************************************/

#include "resource.h"
#include "style.h"

using namespace Resource;

//////////////////////////////////////////////////////////////////////////////
// Construction, Destruction                                                //
//////////////////////////////////////////////////////////////////////////////

Style::Style()
    : name_("Generic Ale"), oglow_(0.0), oghi_(0.0), fglow_(0.0), fghi_(0.0),
      ibulow_(0), ibuhi_(100), srmlow_(0), srmhi_(40)
{ ; }

//////////////////////////////////////////////////////////////////////////////
// Style()
// -------
// Constructor

Style::Style(const QString name, const double &oglow, const double &oghi,
             const double &fglow, const double &fghi,
             const int &ibulow, const int &ibuhi,
             const int &srmlow, const int &srmhi)
    : name_(name), oglow_(oglow), oghi_(oghi), fglow_(fglow), fghi_(fghi),
      ibulow_(ibulow), ibuhi_(ibuhi), srmlow_(srmlow), srmhi_(srmhi)
{
    // older qbrewdata files might not have FG values
    if (fglow_ == 0.0) fglow_ = ((oglow_ - 1.0) * 0.25) + 1.0;
    if (fghi_ == 0.0) fghi_ = ((oghi_ - 1.0) * 0.25) + 1.0;
}

Style::Style(const Style &s)
    : name_(s.name_), oglow_(s.oglow_), oghi_(s.oghi_), fglow_(s.fglow_),
      fghi_(s.fghi_), ibulow_(s.ibulow_), ibuhi_(s.ibuhi_),
      srmlow_(s.srmlow_), srmhi_(s.srmhi_)
{ ; }

Style::~Style() { ; }

//////////////////////////////////////////////////////////////////////////////
// Miscellaneous                                                            //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// operator=
// ---------
// Assignment operator

Style Style::operator=(const Style &s)
{
    if (&s != this) {
        name_ = s.name_;
        oglow_ = s.oglow_; oghi_ = s.oghi_;
        fglow_ = s.fglow_; fghi_ = s.fghi_;
        ibulow_ = s.ibulow_; ibuhi_ = s.ibuhi_;
        srmlow_ = s.srmlow_; srmhi_ = s.srmhi_;
    }
    return *this;
}

//////////////////////////////////////////////////////////////////////////////
// operator==
// ----------
// Equivalence operator

bool Style::operator==(const Style &s) const
{
    return ( (name_ == s.name_) &&
             (oglow_ == s.oglow_) &&
             (oghi_ == s.oghi_) &&
             (fglow_ == s.fglow_) &&
             (fghi_ == s.fghi_) &&
             (ibulow_ == s.ibulow_) &&
             (ibuhi_ == s.ibuhi_) &&
             (srmlow_ == s.srmlow_) &&
             (srmhi_ == s.srmhi_) );
}
