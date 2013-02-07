/***************************************************************************
  beerxmlreader.cpp
  -------------------
  XML stream reader for BeerXML
  -------------------
  Copyright 2008, David Johnson
  Please see the header file for copyright and license information
 ***************************************************************************/

#include <QApplication>
#include <QInputDialog>
#include "data.h"
#include "recipe.h"
#include "resource.h"

#include "beerxmlreader.h"

using namespace Resource;

// BeerXmlReader /////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// BeerXmlReader()
// ---------------
// Constructor

BeerXmlReader::BeerXmlReader(QIODevice *device)
    : QXmlStreamReader(device)
{
}

/////////////////////////////////////////////////////////////////////////////
// isBeerXmlFormat()
// -----------------
// Is xml BeerXML format?
// Defined as XML with "<RECIPES>" and "<RECIPE>", VERSION 1
// Note that BeerXML 1.0 is poorly designed format

bool BeerXmlReader::isBeerXmlFormat()
{
    // find root/first element
    do {
        readNext();
    } while (!atEnd() && !isStartElement());
    if (hasError()) return false;

    // check the document type
    if (name() != tagRECIPES) return false;

    // check for RECIPE
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (name() == tagRECIPE) {
                // check VERSION
                while (!atEnd()) {
                    readNext();
                    if (isStartElement()) {
                        if (name() == tagVERSION) {
                            if (readElementText() == beerXMLVersion)
                                return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////
// readRecipe()
// ------------
// Read in recipe

bool BeerXmlReader::readRecipe(Recipe *recipe)
{
    QString buf;

    // BeerXML may have more than one recipe,
    // so we read each one into a different recipe object
    QMap<QString, Recipe> recipes;
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (name() == tagRECIPES) {
                while (!atEnd()) {
                    readNext();
                    if (isStartElement()) {
                        if (name() == tagRECIPE) {
                            Recipe r = readSingleRecipe();
                            if (!r.title().isEmpty()) {
                                recipes[r.title()] = r;
                            }
                        } else {
                            skipElement();
                        }
                    }
                }
            } else {
                qWarning() << "Warning: Unknown tag" << name().toString();
                skipElement();
            }
        }
    }
    if (hasError()) return false;

    if (recipes.count() == 0) {
        raiseError("No recipes found");
        return false;
    }

    // select just one recipe
    if (recipes.count() > 1) {
        bool ok;
        QString name = QInputDialog::getItem(0, TITLE,
            QObject::tr("Multiple recipes found. Please select one:"),
            recipes.keys(), 0, false, &ok);
        if (ok) {
            // copy over recipe
            *recipe = recipes[name];
        }
    } else {
        // only one recipe
        *recipe = recipes.begin().value();
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////
// readSingleRecipe()
// ------------------
// Read an individual recipe from xml

Recipe BeerXmlReader::readSingleRecipe()
{
    Recipe recipe(0);
    QString buf;
    bool mashed = false;
    bool grainsflag=false, hopsflag=false, miscsflag=false, yeastsflag=false;

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            // version
            if (name() == tagVERSION) {
                if (readElementText() != beerXMLVersion) {
                    qWarning() << "Wrong BeerXML version";
                    return Recipe();
                }
            }

            // name
            else if (name() == tagNAME) {
                recipe.setTitle(readElementText());
            }

            // brewer
            else if (name() == tagBREWER) {
                recipe.setBrewer(readElementText());
            }

            // style
            else if (name() == tagSTYLE) {
                recipe.setStyle(readStyle());
            }

            // mashed
            else if (name() == tagTYPE) {
                mashed = (readElementText() != Grain::EXTRACT_STRING);
            }

            // batch size
            else if (name() == tagBATCHSIZE) {
                buf = readElementText();
                recipe.setSize(Volume(buf, Volume::liter));
            }

            // notes
            else if (name() == tagNOTES) {
                recipe.setRecipeNotes(readElementText());
            }

            else if (name() == tagTASTENOTES) {
                recipe.setBatchNotes(readElementText());
            }

            // fermentables
            else if (name() == tagFERMENTABLES) {
                grainsflag = true;
            }
            // fermentable
            else if (name() == tagFERMENTABLE) {
                if (!grainsflag) qWarning("Warning: mislocated fermentable tag");
                recipe.addGrain(readFermentable());
            }

            // hops
            else if (name() == tagHOPS) {
                hopsflag = true;
            }
            // hop
            else if (name() == tagHOP) {
                if (!hopsflag) qWarning("Warning: mislocated hop tag");
                recipe.addHop(readHop());
            }

            // miscs
            else if (name() == tagMISCS) {
                miscsflag = true;
            }
            // misc
            else if (name() == tagMISC) {
                if (!miscsflag) qWarning("Warning: mislocated misc tag");
                recipe.addMisc(readMisc());
            }

            // yeasts
            else if (name() == tagYEASTS) {
                yeastsflag = true;
            }
            // yeast
            else if (name() == tagYEAST) {
                if (!yeastsflag) qWarning("Warning: mislocated yeast tag");
                recipe.addMisc(readYeast());
            }

            else if (name() == tagEFFICIENCY) {
                // unused, ignore tag...
                skipElement();
            }

            else if (name() == tagWATERS) {
                // unused, ignore tag...
                skipElement();
            }

            // unknown tag, skip
            else {
                skipElement();
            }
        }

        else if (isEndElement()) {
            if (name() == tagRECIPE) break;
            // unset flags
            else if (name() == tagFERMENTABLES) grainsflag = false;
            else if (name() == tagHOPS) hopsflag = false;
            else if (name() == tagMISCS) miscsflag = false;
            else if (name() == tagYEASTS) yeastsflag = false;
        }
    }
    if (hasError()) {
        raiseError("Problem reading recipe");
        return false;
    }

    // set grain use according to recipe type
    for (int n=0; n<recipe.grains()->count(); n++) {
        if (mashed) (*recipe.grains())[n].setUse(Grain::MASHED_STRING);
        else        (*recipe.grains())[n].setUse(Grain::STEEPED_STRING);
    }

    return recipe;
}

// read grain
Grain BeerXmlReader::readFermentable()
{
    QString buf;
    Grain grain;

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            // version
            if (name() == tagVERSION) {
                if (readElementText() != beerXMLVersion) {
                    qWarning() << "Error: Wrong BeerXML version";
                    return Grain();
                }
            }

            // name
            else if (name() == tagNAME) {
                grain.setName(readElementText());
            }

            // amount
            else if (name() == tagAMOUNT) {
                buf = readElementText();
                grain.setWeight(Weight(buf.toDouble(), Weight::kilogram));
            }

            // yield
            else if (name() == tagYIELD) {
                buf = readElementText();
                grain.setExtract(Recipe::yieldToExtract(buf.toDouble() / 100.0));
            }

            // color
            else if (name() == tagCOLOR) {
                buf = readElementText();
                grain.setColor(buf.toDouble());
            }

            // type (and guess at use)
            else if (name() == tagTYPE) {
                buf = readElementText();
                grain.setType(Grain::OTHER_STRING);
                grain.setUse(Grain::OTHER_STRING);
                if (buf == "Grain") {
                    grain.setType(Grain::GRAIN_STRING);
                }
                if (buf == "Sugar") {
                    grain.setType(Grain::SUGAR_STRING);
                    grain.setUse(Grain::OTHER_STRING);
                }
                if ((buf == "Extract") ||
                    (buf == "Dry Extract")) {
                    grain.setType(Grain::EXTRACT_STRING);
                    grain.setUse(Grain::EXTRACT_STRING);
                }
                if (buf== "Adjunct") {
                    grain.setType(Grain::ADJUNCT_STRING);
                    grain.setUse(Grain::MASHED_STRING);
                }
            }
        }

        else if (isEndElement()) {
            if (name() == tagFERMENTABLE) break;
        }
    }

    return grain;
}

// read hop
Hop BeerXmlReader::readHop()
{
    QString buf;
    Hop hop;

    // TODO: add USE to future hop notes

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            // version
            if (name() == tagVERSION) {
                if (readElementText() != beerXMLVersion) {
                    qWarning() << "Error: Wrong BeerXML version";
                    return hop;
                }
            }

            // name
            if (name() == tagNAME) {
                hop.setName(readElementText());
            }

            // amount
            else if (name() == tagAMOUNT) {
                buf = readElementText();
                hop.setWeight(Weight(buf.toDouble(), Weight::kilogram));
            }

            // alpha
            else if (name() == tagALPHA) {
                buf = readElementText();
                hop.setAlpha(buf.toDouble());
            }

            // time
            else if (name() == tagTIME) {
                buf = readElementText();
                hop.setTime((int)buf.toDouble());
            }

            // form
            else if (name() == tagFORM) {
                buf = readElementText();
                if (buf == "Pellet") {
                    hop.setType(Hop::PELLET_STRING);
                } else if (buf == "Plug") {
                    hop.setType(Hop::PLUG_STRING);
                } else {
                    hop.setType(Hop::WHOLE_STRING);
                }
            }



        }
        else if (isEndElement()) {
            if (name() == tagHOP) break;
        }
    }

    return hop;
}

// read misc ingredient
Misc BeerXmlReader::readMisc()
{
    QString buf;
    Misc misc;
    double temp_amount = 0;
    bool temp_aiw = false;

    // TODO: put TYPE and USE in notes

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            // version
            if (name() == tagVERSION) {
                if (readElementText() != beerXMLVersion) {
                    qWarning() << "Error: Wrong BeerXML version";
                    return misc;
                }
            }

            // name
            if (name() == tagNAME) {
                misc.setName(readElementText());
            }

            // amount
            else if (name() == tagAMOUNT) {
                buf = readElementText();
                temp_amount = buf.toDouble();
            }

            // amount is weight
            else if (name() == tagAMOUNTISWEIGHT) {
                buf = readElementText();
                temp_aiw = (buf.toLower() == "true");
            }

            // notes
            if (name() == tagNOTES) {
                misc.setNotes(readElementText());
            }
            // type
            else if (name() == tagTYPE) {
                buf = readElementText();
                misc.setType(Misc::OTHER_STRING);
                if (buf == "Spice") misc.setType(Misc::SPICE_STRING);
                if (buf == "Fining") misc.setType(Misc::FINING_STRING);
                if (buf == "Herb") misc.setType(Misc::HERB_STRING);
                if (buf == "Flavor") misc.setType(Misc::FLAVOR_STRING);
                if (buf == "Water Agent") misc.setType(Misc::ADDITIVE_STRING);
            }
        }
        else if (isEndElement()) {
            if (name() == tagMISC) break;
        }
    }

    if (temp_aiw) {
        misc.setQuantity(Weight(temp_amount, Weight::kilogram));
    } else {
        misc.setQuantity(Volume(temp_amount, Volume::liter));
    }

    return misc;
}

// read yeast
Misc BeerXmlReader::readYeast()
{
    QString buf, notes;
    Misc misc;
    double temp_amount = 0;
    bool temp_aiw = false;
    misc.setType(Misc::YEAST_STRING);

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            // version
            if (name() == tagVERSION) {
                if (readElementText() != beerXMLVersion) {
                    qWarning() << "Error: Wrong BeerXML version";
                    return misc;
                }
            }

            // name
            if (name() == tagNAME) {
                misc.setName(readElementText());
            }

            // amount
            else if (name() == tagAMOUNT) {
                buf = readElementText();
                temp_amount = buf.toDouble();
            }

            // amount is weight
            else if (name() == tagAMOUNTISWEIGHT) {
                buf = readElementText();
                temp_aiw = (buf.toLower() == "true");
            }

            // notes
            if (name() == tagNOTES) {
                if (!notes.isEmpty()) notes += ' ';
                notes += readElementText();
            }

            // form
            else if (name() == tagFORM) {
                if (!notes.isEmpty()) notes += ' ';
                notes += readElementText() + " yeast.";
            }
        }
        else if (isEndElement()) {
            if (name() == tagYEAST) break;
        }
    }

    misc.setNotes(notes);
    if (temp_aiw) {
        misc.setQuantity(Weight(temp_amount, Weight::kilogram));
    } else {
        misc.setQuantity(Volume(temp_amount, Volume::liter));
    }

    return misc;
}

// read style from xml
QString BeerXmlReader::readStyle()
{
    // the only thing I care about is name
    QString stylename;
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            if (name() == tagNAME) {
                stylename = readElementText();
            }
        }

        else if (isEndElement()) {
            if (name() == tagSTYLE) break;
        }
    }
    return stylename;
}

// skip over xml element
void BeerXmlReader::skipElement()
{
    if (isStartElement()) {
        int level = 0;
        while (!atEnd()) {
            readNext();

            if (isStartElement()) level++;
            if (isEndElement()) level--;

            if (level < 0) break;
        }
    }
}

// BeerXmlWriter /////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// RecipeWriter()
// --------------
// Constructor

BeerXmlWriter::BeerXmlWriter(QIODevice *device)
    : QXmlStreamWriter(device)
{
#if (QT_VERSION >= QT_VERSION_CHECK(4, 4, 0))
    setAutoFormatting(true);
    setAutoFormattingIndent(2);
#endif
}

//////////////////////////////////////////////////////////////////////////////
// writeRecipe()
// -------------
// Write out recipe

bool BeerXmlWriter::writeRecipe(Recipe *recipe)
{
    QString buf;

    writeStartDocument(); // BeerXML 1 doesn't have a doctype
    writeComment(QString("BeerXML generated by %1 %2")
                     .arg(PACKAGE).arg(VERSION));
    writeStartElement(tagRECIPES);

    // write recipe information
    writeStartElement(tagRECIPE);
    writeTextElement(tagVERSION, beerXMLVersion);
    writeTextElement(tagNAME, recipe->title());
    writeTextElement(tagTYPE, recipe->method());
    writeTextElement(tagBREWER, recipe->brewer());
    double size = recipe->size().amount(Volume::liter);
    writeTextElement(tagBATCHSIZE, QString::number(size,'f',8));
    writeTextElement(tagEFFICIENCY,
        QString::number(Data::instance()->efficiency()*100.0,'f',1));

    // write style
    // TODO: unfinished...
    // category
    // category number
    // style letter
    // style guide
    // type
    // og
    writeStartElement(tagSTYLE);
    writeTextElement(tagVERSION, beerXMLVersion);
    writeTextElement(tagNAME, recipe->style().name());
    writeTextElement(tagOGMIN, QString::number(recipe->style().OGLow(),'f',4));
    writeTextElement(tagOGMAX, QString::number(recipe->style().OGHi(),'f',4));
    writeTextElement(tagFGMIN, QString::number(recipe->style().FGLow(),'f',4));
    writeTextElement(tagFGMAX, QString::number(recipe->style().FGHi(),'f',4));
    writeTextElement(tagIBUMIN, QString::number(recipe->style().IBULow(),'f',2));
    writeTextElement(tagIBUMAX, QString::number(recipe->style().IBUHi(),'f',2));
    writeTextElement(tagCOLORMIN, QString::number(recipe->style().SRMLow(),'f',2));
    writeTextElement(tagCOLORMAX, QString::number(recipe->style().SRMHi(),'f',2));
    writeEndElement(); // tagSTYLE

    // fermentables list
    writeStartElement(tagFERMENTABLES);
    foreach (Grain grain, *recipe->grains()) {
        writeStartElement(tagFERMENTABLE);
        writeTextElement(tagVERSION, beerXMLVersion);
        writeTextElement(tagNAME, grain.name());
        size = grain.weight().amount(Weight::kilogram);
        writeTextElement(tagAMOUNT, QString::number(size,'f',8));
        writeTextElement(tagCOLOR, QString::number(grain.color(),'f',2));
        double yield = recipe->extractToYield(grain.extract()) * 100.0;
        writeTextElement(tagYIELD, QString::number(yield,'f',2));
        writeTextElement(tagTYPE, grain.type());
        writeEndElement(); // tagFERMENTABLE
    }
    writeEndElement(); // tagFERMENTABLES

    // hop list
    writeStartElement(tagHOPS);
    foreach (Hop hop, *recipe->hops()) {
        writeStartElement(tagHOP);
        writeTextElement(tagVERSION, beerXMLVersion);
        writeTextElement(tagNAME, hop.name());
        size = hop.weight().amount(Weight::kilogram);
        writeTextElement(tagAMOUNT, QString::number(size,'f',8));
        writeTextElement(tagALPHA, QString::number(hop.alpha(),'f',2));
        // TODO: note that I'm using "Boil" for all hops...
        writeTextElement(tagUSE, "Boil");
        writeTextElement(tagTIME, QString::number(hop.time()));
        buf = "Leaf";
        if (hop.type() == Hop::PELLET_STRING) buf = "Pellet";
        if (hop.type() == Hop::PLUG_STRING) buf = "Plug";
        writeTextElement(tagFORM, buf);
        writeEndElement(); // tagHOP
    }
    writeEndElement(); // tagHOPS

    // yeast list
    writeStartElement(tagYEASTS);
    foreach (Misc misc, *recipe->miscs()) {
        // iterate through list looking for yeasts
        if (misc.type() == Misc::YEAST_STRING) {
            writeStartElement(tagYEAST);
            writeTextElement(tagVERSION, beerXMLVersion);
            writeTextElement(tagNAME, misc.name());
            writeTextElement(tagAMOUNT, "0.00"); // TODO: fixup for miscs
            writeTextElement(tagTYPE, Misc::YEAST_STRING);
            // TODO: form???
            writeTextElement(tagNOTES, misc.notes());
            writeEndElement(); // tagYEAST
        }
    }
    writeEndElement(); // tagYEASTS

    // misc list
    writeStartElement(tagMISCS);
    foreach (Misc misc, *recipe->miscs()) {
        // iterate through list looking for yeasts
        if (misc.type() != Misc::YEAST_STRING) {
            writeStartElement(tagMISC);
            writeTextElement(tagVERSION, beerXMLVersion);
            writeTextElement(tagNAME, misc.name());
            writeTextElement(tagAMOUNT, "0.00"); // TODO: fixup for miscs
            writeTextElement(tagTYPE, misc.type());
            writeTextElement(tagNOTES, misc.notes());
            // use ???
            // time ???
            writeEndElement(); // tagMISC
        }
    }
    writeEndElement(); // tagMISCS

    // waters
    // NOTE: not currently supporting water
    writeStartElement(tagWATERS);
    writeEndElement();

    // mash
    // NOTE: not currently supporting mash

    // notes
    if (!(recipe->recipeNotes().isEmpty() && recipe->batchNotes().isEmpty())) {
        buf = recipe->recipeNotes();
        if (!recipe->batchNotes().isEmpty()) {
            if (!buf.isEmpty()) buf += "\n\n";
            buf += recipe->batchNotes();
        }
        writeTextElement(tagNOTES, buf);
    }

    writeEndElement(); // tagRECIPE
    writeEndElement(); // tagRECIPES

    return true;
}
