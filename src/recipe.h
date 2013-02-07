/***************************************************************************
  recipe.h
  -------------------
  Recipe (document) class
  -------------------
  Copyright 2001-2008, David Johnson
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

#ifndef RECIPE_H
#define RECIPE_H

#include <QObject>
#include <QString>

#include "quantity.h"
#include "grain.h"
#include "hop.h"
#include "misc.h"
#include "style.h"

class TextPrinter;

class Recipe : public QObject {
    Q_OBJECT
public:
    // TODO: make sure am comparing lowercase
    // recipe strings
    static const QByteArray EXTRACT_STRING;
    static const QByteArray PARTIAL_STRING;
    static const QByteArray ALLGRAIN_STRING;

    // default constructor
    Recipe(QObject *parent=0);
    Recipe(const Recipe &recipe);
    Recipe operator=(const Recipe &recipe);
    virtual ~Recipe();

    // start a new recipe
    void newRecipe();
    // load a recipe
    bool loadRecipe(const QString &filename);
    // save the current recipe
    bool saveRecipe(const QString &filename);
    // preview the recipe
    void previewRecipe(TextPrinter *textprinter);
    // print the recipe
    void printRecipe(TextPrinter *textprinter);
    // is the file in native format?
    bool nativeFormat(const QString &filename);
    // is the file in BeerXml format?
    bool beerXmlFormat(const QString &filename);

    // import a recipe from another BeerXML format
    bool importBeerXml(const QString &filename);
    // export the recipe as html
    bool exportHtml(const QString &filename);
    // export the recipe to BeerXML format
    bool exportBeerXML(const QString &filename);
    // export the recipe as pdf
    bool exportPdf(TextPrinter *textprinter, const QString &filename);
    // export the recipe as html
    bool exportText(const QString &filename);

    // get and set
    const QString& title() const;
    void setTitle(const QString &t);
    const QString& brewer() const;
    void setBrewer(const QString &b);
    const Volume &size() const;
    void setSize(const Volume &s);
    const Style &style() const;
    void setStyle(const Style &s);
    void setStyle(const QString &s);
    const QString& recipeNotes() const;
    void setRecipeNotes(const QString &n);
    const QString& batchNotes() const;
    void setBatchNotes(const QString &n);

    // add an ingredient to the recipe
    void addGrain(const Grain &g);
    void addHop(const Hop &h);
    void addMisc(const Misc &m);

    // return pointer to ingredient lists
    GrainList *grains();
    HopList *hops();
    MiscList *miscs();

    // return original gravity
    double OG();
    // return bitterness
    int IBU();
    // return color
    int SRM();
    // return estimated final gravity
    double FGEstimate();
    // return method of recipe
    QString method();

    // return alcohol %v
    double ABV();
    static double ABV(double og, double fg);
    // return alcohol %w
    double ABW();
    static double ABW(double og, double fg);
    // convert Specific Gravity to Plato
    static double SgToP(double sg);
    // convert grain extract to yield
    static double extractToYield(double extract);
    static double yieldToExtract(double yield);

    // is the recipe modified?
    bool modified() const;

public slots:
    // recalculate the recipe values
    void recalc();
    // set recipe as modified
    void setModified(bool m);

signals:
    // send that the recipe has been loaded or created
    void recipeChanged();
    // send that the recipe has been modified
    void recipeModified();

private:
    // calculations
    double calcOG();
    int calcSRM();
    int calcIBU();
    int calcRagerIBU();
    int calcTinsethIBU();

    const QString recipeHTML();
    const QString recipeText();

private:
    bool modified_;

    QString title_;
    QString brewer_;

    Volume size_;
    Style style_;

    GrainList grains_;
    HopList hops_;
    MiscList miscs_;

    QString recipenotes_;
    QString batchnotes_;

    double og_;
    int ibu_;
    int srm_;
};

// Inlined Methods //////////////////////////////////////////////////////////

inline const QString &Recipe::title() const
    { return title_; }

inline void Recipe::setTitle(const QString &t)
{ title_ = t; setModified(true); }

inline const QString &Recipe::brewer() const
    { return brewer_; }

inline void Recipe::setBrewer(const QString &b)
    { brewer_ = b; setModified(true); }

inline const Volume &Recipe::size() const
    { return size_; }

inline void Recipe::setSize(const Volume &s)
    { size_ = s; recalc(); setModified(true); }

inline const Style &Recipe::style() const
    { return style_; }

inline void Recipe::setStyle(const Style &s)
    { style_ = s; setModified(true); }

inline const QString& Recipe::recipeNotes() const
    { return recipenotes_; }

inline void Recipe::setRecipeNotes(const QString &n)
    { recipenotes_ = n; setModified(true); }

inline const QString& Recipe::batchNotes() const
    { return batchnotes_; }

inline void Recipe::setBatchNotes(const QString &n)
    { batchnotes_ = n; setModified(true); }

inline GrainList *Recipe::grains() { return &grains_; }

inline HopList *Recipe::hops() { return &hops_; }

inline MiscList *Recipe::miscs() { return &miscs_; }

inline double Recipe::OG() { return og_; }

inline int Recipe::IBU() { return ibu_; }

inline int Recipe::SRM() { return srm_; }

#endif // RECIPE_H
