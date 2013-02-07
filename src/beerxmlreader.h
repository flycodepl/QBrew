/***************************************************************************
  beerxmlreader.h
  -------------------
  XML stream reader for BeerXML
  -------------------
  Copyright 2008, David Johnson
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

#ifndef BEERXMLREADER_H
#define BEERXMLREADER_H

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "grain.h"
#include "hop.h"
#include "misc.h"

class Recipe;

class BeerXmlReader : public QXmlStreamReader
{
public:
    // constructor
    BeerXmlReader(QIODevice *device);
    // check for valid BeerXML format
    bool isBeerXmlFormat();
    // read xml into recipe
    bool readRecipe(Recipe *recipe);

private:
    // read an individual recipe
    Recipe readSingleRecipe();
    // read grain
    Grain readFermentable();
    // read hop
    Hop readHop();
    // read misc
    Misc readMisc();
    // read yeast
    Misc readYeast();
    // read style
    QString readStyle();
    // skip over an element
    void skipElement();
};

class BeerXmlWriter : public QXmlStreamWriter
{
public:
    // constructor
    BeerXmlWriter(QIODevice *device);
    // write out recipe to xml
    bool writeRecipe(Recipe *recipe);
};

#endif // BEERXMLREADER_H
