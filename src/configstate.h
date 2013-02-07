/***************************************************************************
  configstate.h
  -------------------
  State for config dialog
  -------------------
  Copyright 2006-2008 David Johnson <david@usermode.org>
  All rights reserved.
 
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
 
  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
 
  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
 
  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

#ifndef CONFIGSTATE_H
#define CONFIGSTATE_H

#include <QStringList>
#include "hop.h"
#include "resource.h"

struct WinConfigState {
#if defined (Q_WS_MAC)
   WinConfigState() : statusbar(true) { ; }
#else
   WinConfigState() : statusbar(false) { ; }
#endif
    bool statusbar;
};

struct GenConfigState {
    GenConfigState()
        : lookfeel(""), showsplash(true), autosave(true), saveinterval(5),
          autobackup(true), loadlast(true), recentnum(5), recentfiles() { ; }
    QString lookfeel;
    bool showsplash;
    bool autosave;
    int saveinterval;
    bool autobackup;
    bool loadlast;
    int recentnum;
    QStringList recentfiles;
};

struct RecipeConfigState {
    RecipeConfigState()
        : batch(5.00), style("Generic Ale"), hoptype(Hop::PELLET_STRING) { ; }
    double batch;
    QString style;
    QString hoptype;
};

struct CalcConfigState {
    CalcConfigState()
        : steepyield(0.5), efficiency(0.75), morey(false), tinseth(false),
          units(Resource::UNIT_US) { ; }
    double steepyield;
    double efficiency;
    bool morey;
    bool tinseth;
    QString units;
};

struct ConfigState
{
    WinConfigState window;
    GenConfigState general;
    RecipeConfigState recipe;
    CalcConfigState calc;
};

#endif // CONFIGSTATE_H
