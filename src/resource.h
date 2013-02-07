/***************************************************************************
  resource.h
  -------------------
  Global application resources. Constants, enumerations and strings.
  -------------------
  Copyright 2006-2008 David Johnson
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

#ifndef RESOURCE_H
#define RESOURCE_H

#include <QDebug>
#include <QString>

#define PACKAGE "qbrew"
#define VERSION "0.4.1"

// some stuff to differentiate between platforms
#if defined(Q_WS_X11)
#define QDIR_HOME   QDir::homePath()
#elif defined(Q_WS_MACX)
#define QDIR_HOME   QDir::homePath()
#else
#define QDIR_HOME   QDir::currentPath()
#endif

namespace Resource {

///////////////////////////////////////////////////////////////////
// general application values

const QByteArray TITLE        = "QBrew";
const QByteArray DESCRIPTION  = QT_TRANSLATE_NOOP("description",
                                "A Homebrewer's Recipe Calculator");
const QByteArray COPYRIGHT    = QT_TRANSLATE_NOOP("copyright",
                                "Copyright 1999-2008");
const QByteArray AUTHOR       = "David Johnson";
const QByteArray AUTHOR_EMAIL = "david@usermode.org";
const QByteArray CONTRIBUTORS = "Lee Henderson, "
                                "Rob Hudson, "
                                "Abe Kabakoff, "
                                "Stephen Lowrie, "
                                "Michal Palczewski, "
                                "Kevin Pullin, "
                                "Tobias Quathamer";

const QByteArray READY        = QT_TRANSLATE_NOOP("message",
                                "Ready");

const QChar DEGREE            = QChar(0xB0);  // ISO 8859-1

///////////////////////////////////////////////////////////////////
// file locations and stuff

const QByteArray FILE_EXT        = PACKAGE;
const QByteArray FILE_FILTER     = QT_TRANSLATE_NOOP("file filter",
                                   "QBrew files (*.qbrew)");
const QByteArray TEXT_FILTER     = QT_TRANSLATE_NOOP("file filter",
                                   "Text files (*.txt)");
const QByteArray HTML_FILTER     = QT_TRANSLATE_NOOP("file filter",
                                   "HTML files (*.html *.htm)");
const QByteArray BEERXML_FILTER  = QT_TRANSLATE_NOOP("file filter",
                                   "BeerXML recipes (*.xml)");
const QByteArray PDF_FILTER      = QT_TRANSLATE_NOOP("file filter",
                                   "PDF files (*.pdf)");
const QByteArray ALL_FILTER      = QT_TRANSLATE_NOOP("file filter",
                                   "All files (*)");
const QByteArray OPEN_FILTER     = QT_TRANSLATE_NOOP("file filter",
                                   FILE_FILTER + ";;"
                                   + BEERXML_FILTER + ";;"
                                   + ALL_FILTER);
const QByteArray SAVE_FILTER     = QT_TRANSLATE_NOOP("file filter",
                                   FILE_FILTER + ";;"
                                   + ALL_FILTER);
const QByteArray EXPORT_FILTER   = QT_TRANSLATE_NOOP("file filter",
                                   HTML_FILTER + ";;"
                                   + BEERXML_FILTER + ";;"
                                   + PDF_FILTER + ";;"
                                   + TEXT_FILTER);

const QByteArray DATA_FILE       = "qbrewdata";
const QByteArray HELP_FILE       = "book/handbook-index.html";
const QByteArray PRIMER_FILE     = "primer/primer.html";
const QByteArray DEFAULT_FILE    = QT_TRANSLATE_NOOP("file name", "untitled");

// previous valid data formats
const QByteArray RECIPE_PREVIOUS = "0.1.7";
const QByteArray DATA_PREVIOUS   = "0.3.0";

///////////////////////////////////////////////////////////////////
// configuration strings

const QByteArray CONFGROUP_WINDOW      = "/window";
const QByteArray CONF_WIN_STATUSBAR    = "/statusbar";
const QByteArray CONF_WIN_MAINWINDOW   = "/mainwindow";
const QByteArray CONF_WIN_SIZE         = "/size";

const QByteArray CONFGROUP_GENERAL     = "/general";
const QByteArray CONF_GEN_LOOK_FEEL    = "/lookfeel";
const QByteArray CONF_GEN_SHOW_SPLASH  = "/showsplash";
const QByteArray CONF_GEN_AUTOSAVE     = "/autosave";
const QByteArray CONF_GEN_SAVEINTERVAL = "/saveinterval";
const QByteArray CONF_GEN_AUTOBACKUP   = "/autobackup";
const QByteArray CONF_GEN_LOADLAST     = "/loadlast";
const QByteArray CONF_GEN_RECENTFILES  = "/recentfiles";
const QByteArray CONF_GEN_RECENTNUM    = "/recentnum";

const QByteArray CONFGROUP_RECIPE       = "/recipedefaults";
const QByteArray CONF_RECIPE_BATCH      = "/batch";
const QByteArray CONF_RECIPE_STYLE      = "/style";
 const QByteArray CONF_RECIPE_HOPFORM    = "/hopform"; // TODO: deprecated 0.4.0
const QByteArray CONF_RECIPE_HOPTYPE    = "/hoptype";

const QByteArray CONFGROUP_CALC         = "/calc";
const QByteArray CONF_CALC_STEEPYIELD   = "/steepyield";
const QByteArray CONF_CALC_EFFICIENCY   = "/efficiency";
const QByteArray CONF_CALC_MOREY        = "/morey";
const QByteArray CONF_CALC_TINSETH      = "/tinseth";
const QByteArray CONF_CALC_UNITS        = "/units";

//////////////////////////////////////////////////////////////////////////////
// XML Format Strings (need to be QString)

const QString tagRecipe             = "recipe";
const QString attrApplication       = "application";
const QString attrGenerator         = "generator";
const QString tagTitle              = "title";
const QString tagBrewer             = "brewer";
const QString tagBatch              = "batch";
const QString tagNotes              = "notes";
const QString attrClass             = "class";
const QString classRecipe           = tagRecipe;
const QString classBatch            = tagBatch;

const QString tagDoc                = "qbrewdata"; 
const QString attrVersion           = "version";
const QString tagStyles             = "styles";
const QString tagStyle              = "style";
const QString attrOGLow             = "oglow";
const QString attrOGHigh            = "oghigh";
const QString attrFGLow             = "fglow";
const QString attrFGHigh            = "fghigh";
const QString attrIBULow            = "ibulow";
const QString attrIBUHigh           = "ibuhigh";
const QString attrSRMLow            = "srmlow";
const QString attrSRMHigh           = "srmhigh";
const QString tagGrains             = "grains";
const QString tagGrain              = "grain";
const QString attrQuantity          = "quantity";
const QString attrSize              = "size"; // TODO: deprecated
const QString attrExtract           = "extract";
const QString attrColor             = "color";
const QString attrType              = "type";
const QString attrUse               = "use";
const QString tagHops               = "hops";
const QString tagHop                = "hop";
const QString attrForm              = "form";
const QString attrAlpha             = "alpha";
const QString attrTime              = "time";
const QString tagMiscs              = "miscingredients"; // TODO: rename
const QString tagMisc               = "miscingredient";
const QString attrNotes             = "notes";
const QString tagUtilization        = "utilization";
const QString tagEntry              = "entry";
const QString attrUtil              = "util";

// BeerXML Format Strings

// TODO: see if I can use lowercase, converting on the fly during import/export
const QString beerXMLVersion        = "1";
const QString tagRECIPES            = "RECIPES";
const QString tagRECIPE             = "RECIPE";
const QString tagNAME               = "NAME";
const QString tagVERSION            = "VERSION";
const QString tagTYPE               = "TYPE";
const QString tagSTYLE              = "STYLE";
const QString tagBREWER             = "BREWER";
const QString tagBATCHSIZE          = "BATCH_SIZE";
const QString tagEFFICIENCY         = "EFFICIENCY";
const QString tagFERMENTABLES       = "FERMENTABLES";
const QString tagFERMENTABLE        = "FERMENTABLE";
const QString tagHOPS               = "HOPS";
const QString tagHOP                = "HOP";
const QString tagYEASTS             = "YEASTS";
const QString tagYEAST              = "YEAST";
const QString tagMISCS              = "MISCS";
const QString tagMISC               = "MISC";
const QString tagNOTES              = "NOTES";
const QString tagTASTENOTES         = "TASTE_NOTES";
const QString tagWATERS             = "WATERS";

const QString tagOGMIN              = "OG_MIN";
const QString tagOGMAX              = "OG_MAX";
const QString tagFGMIN              = "FG_MIN";
const QString tagFGMAX              = "FG_MAX";
const QString tagIBUMIN             = "IBU_MIN";
const QString tagIBUMAX             = "IBU_MAX";
const QString tagCOLORMIN           = "COLOR_MIN";
const QString tagCOLORMAX           = "COLOR_MAX";

const QString tagAMOUNT             = "AMOUNT";
const QString tagAMOUNTISWEIGHT     = "AMOUNT_IS_WEIGHT";
const QString tagYIELD              = "YIELD";
const QString tagCOLOR              = "COLOR";

const QString tagALPHA              = "ALPHA";
const QString tagUSE                = "USE";
const QString tagTIME               = "TIME";
const QString tagFORM               = "FORM";

///////////////////////////////////////////////////////////////////
// Misc

const QByteArray UNIT_METRIC        = QT_TRANSLATE_NOOP("units", "Metric");
const QByteArray UNIT_US            = QT_TRANSLATE_NOOP("units", "US");

}; // namespace Resource

#endif // RESOURCE_H
