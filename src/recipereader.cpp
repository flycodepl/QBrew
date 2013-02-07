/***************************************************************************
  recipereader.cpp
  -------------------
  XML stream reader for recipe
  -------------------
  Copyright 2008, David Johnson
  Please see the header file for copyright and license information
 ***************************************************************************/

#include <QApplication>
#include "recipe.h"
#include "resource.h"

#include "recipereader.h"

using namespace Resource;

// RecipeReader ///////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// RecipeReader()
// --------------
// Constructor

RecipeReader::RecipeReader(QIODevice *device)
    : QXmlStreamReader(device)
{
}

/////////////////////////////////////////////////////////////////////////////
// isRecipeFormat()
// ----------------
// Is xml native Recipe format?
// Defined as "recipe" doctype with generator or application as "qbrew"

bool RecipeReader::isRecipeFormat()
{
    // find root/first element
    do {
        readNext();
    } while (!atEnd() && !isStartElement());

    if (hasError()) return false;

    // check the document type
    if (name() != tagRecipe) return false;

    // check application
    if (attributes().value(attrApplication) != PACKAGE) {
        // check generator if no application
        if (attributes().value(attrGenerator) != PACKAGE) {
            return false;
        }
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////
// readRecipe()
// ------------
// Read in recipe

bool RecipeReader::readRecipe(Recipe *recipe)
{
    QString buf;
    bool grainsflag=false, hopsflag=false, miscsflag=false;

    // parse file
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {

            // recipe root tag
            if (name() == tagRecipe) {
                // check application or generator
                if (attributes().value(attrApplication) != PACKAGE) {
                    if (attributes().value(attrGenerator) != PACKAGE) {
                        raiseError(QObject::tr("Not a recipe for qbrew"));
                        return false;
                    }
                }

                // check file version
                buf = attributes().value(attrVersion).toString();
                if (buf < RECIPE_PREVIOUS) {
                    // too old of a version
                    raiseError(QObject::tr("Unsupported file version"));
                    return false;
                }
            }

            // title
            else if (name() == tagTitle) {
                recipe->setTitle(readElementText());
            }

            // brewer
            else if (name() == tagBrewer) {
                recipe->setBrewer(readElementText());
            }

            // style
            // TODO: load/save entire style
            else if (name() == tagStyle) {
                recipe->setStyle(readElementText());
            }

            // get batch settings
            // TODO: eliminate this tag, use quantity, efficiency
            else if (name() == tagBatch) {
                if (!attributes().value(attrQuantity).isEmpty()) {
                    buf = attributes().value(attrQuantity).toString();
                    recipe->setSize(Volume(buf, Volume::gallon));
                } else if (!attributes().value(attrSize).isEmpty()) {
                    // deprecated tag
                    buf = attributes().value(attrSize).toString();
                    recipe->setSize(Volume(buf, Volume::gallon));
                }
            }

            // get notes
            else if (name() == tagNotes) {
                if (attributes().value(attrClass) == classRecipe) {
                    buf = recipe->recipeNotes();
                    if (!buf.isEmpty()) buf += '\n';
                    buf += readElementText();
                    recipe->setRecipeNotes(buf);
                } else if (attributes().value(attrClass) == classBatch) {
                    buf = recipe->batchNotes();
                    if (!buf.isEmpty()) buf += '\n';
                    buf += readElementText();
                    recipe->setBatchNotes(buf);
                }
            }

            // grains
            else if (name() == tagGrains) {
                grainsflag = true;
            }

            // grain
            else if (name() == tagGrain) {
                if (!grainsflag) qWarning("Warning: mislocated grain tag");
                Grain grain;
                buf = attributes().value(attrQuantity).toString();
                grain.setWeight(Weight(buf, Weight::pound));
                buf = attributes().value(attrExtract).toString();
                grain.setExtract(buf.toDouble());
                buf = attributes().value(attrColor).toString();
                grain.setColor(buf.toDouble());
                grain.setType(attributes().value(attrType).toString());
                grain.setUse(attributes().value(attrUse).toString());
                grain.setName(readElementText());
                recipe->addGrain(grain);
            }

            // hops
            else if (name() == tagHops) {
                hopsflag = true;
            }

            // hop
            else if (name() == tagHop) {
                // TODO: attrForm is deprecated 0.4.0
                if (!hopsflag) qWarning("Warning: mislocated hop tag");
                Hop hop;
                buf = attributes().value(attrQuantity).toString();
                hop.setWeight(Weight(buf, Weight::ounce));
                buf = attributes().value(attrType).toString();
                if (buf.isEmpty()) buf = attributes().value(attrForm).toString();
                hop.setType(buf);
                buf = attributes().value(attrAlpha).toString();
                hop.setAlpha(buf.toDouble());
                buf = attributes().value(attrTime).toString();
                hop.setTime(buf.toUInt());
                hop.setName(readElementText());
                recipe->addHop(hop);
            }

            // miscs
            else if (name() == tagMiscs) {
                miscsflag = true;
            }

            // misc
            else if (name() == tagMisc) {
                if (!miscsflag) qWarning("Warning: mislocated misc tag");
                Misc misc;
                buf = attributes().value(attrQuantity).toString();
                misc.setQuantity(Quantity(buf, Quantity::generic));
                misc.setType(attributes().value(attrType).toString());
                misc.setNotes(attributes().value(attrNotes).toString());
                misc.setName(readElementText());
                recipe->addMisc(misc);
            }

            // unknown tag, skip
            else {
                qWarning() << "Warning: Unknown tag" << name().toString();
                skipElement();
            }
        }

        else if (isEndElement()) {
            // unset flags
            if (name() == tagGrains) grainsflag = false;
            else if (name() == tagHops) hopsflag = false;
            else if (name() == tagMiscs) miscsflag = false;
        }
    }
    return true;
}

// skip over xml element
void RecipeReader::skipElement()
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

// RecipeWriter ///////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// RecipeWriter()
// --------------
// Constructor

RecipeWriter::RecipeWriter(QIODevice *device)
    : QXmlStreamWriter(device)
{
#if (QT_VERSION >= QT_VERSION_CHECK(4, 4, 0))
    setAutoFormatting(true);
    setAutoFormattingIndent(2);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// writeRecipe()
// -------------
// Write out recipe

bool RecipeWriter::writeRecipe(Recipe *recipe)
{
    writeStartDocument();
    writeDTD(QString("<!DOCTYPE %1>").arg(tagRecipe));
     
    // write root element
    writeStartElement(tagRecipe);
    writeAttribute(attrApplication, PACKAGE);
    writeAttribute(attrVersion, VERSION);

    // write recipe information
    writeTextElement(tagTitle, recipe->title());
    writeTextElement(tagBrewer, recipe->brewer());
     // TODO: save entire style
    writeTextElement(tagStyle, recipe->style().name());
    writeEmptyElement(tagBatch);
    writeAttribute(attrQuantity, recipe->size().toString());

    // write notes
    if (!recipe->recipeNotes().isEmpty()) {
        writeStartElement(tagNotes);
        writeAttribute(attrClass, classRecipe);
        writeCharacters(recipe->recipeNotes());
        writeEndElement();
    }
    if (!recipe->batchNotes().isEmpty()) {
        writeStartElement(tagNotes);
        writeAttribute(attrClass, classBatch);
        writeCharacters(recipe->batchNotes());
        writeEndElement();
    }

    // write grains
    writeStartElement(tagGrains);
    foreach(Grain grain, *recipe->grains()) {
        // iterate through grain list
        writeStartElement(tagGrain);
        writeAttribute(attrQuantity, grain.weight().toString());
        writeAttribute(attrExtract, QString::number(grain.extract()));
        writeAttribute(attrColor, QString::number(grain.color()));
        writeAttribute(attrType, grain.type());
        writeAttribute(attrUse, grain.use());
        writeCharacters(grain.name());
        writeEndElement();
    }
    writeEndElement(); // tagGrains

    // write hops
    writeStartElement(tagHops);
    foreach(Hop hop, *recipe->hops()) {
        // iterate through hop list
        writeStartElement(tagHop);
        writeAttribute(attrQuantity, hop.weight().toString());
        writeAttribute(attrAlpha, QString::number(hop.alpha()));
        writeAttribute(attrTime, QString::number(hop.time()));
        writeAttribute(attrType, hop.type());
        writeCharacters(hop.name());
        writeEndElement();
    }
    writeEndElement(); // tagHops

    // write misc ingredients
    writeStartElement(tagMiscs);
    foreach(Misc misc, *recipe->miscs()) {
        // iterate through misc list
        writeStartElement(tagMisc);
        writeAttribute(attrQuantity, misc.quantity().toString());
        writeAttribute(attrType, misc.type());
        writeAttribute(attrNotes, misc.notes());
        writeCharacters(misc.name());
        writeEndElement();
    }
    writeEndElement(); // tagMiscs

    writeEndElement(); // tagRecipe
    writeEndDocument();

    return true;
}
