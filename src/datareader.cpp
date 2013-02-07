/***************************************************************************
  datareader.cpp
  -------------------
  XML stream reader for QBrew data
  -------------------
  Copyright 2008, David Johnson
  Please see the header file for copyright and license information
 ***************************************************************************/

#include "data.h"
#include "resource.h"

#include "datareader.h"

using namespace Resource;

// DataReader ///////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// DataReader()
// ------------
// Constructor

DataReader::DataReader(QIODevice *device)
    : QXmlStreamReader(device)
{
}

/////////////////////////////////////////////////////////////////////////////
// readData()
// ----------
// Read in data

bool DataReader::readData(Data *data)
{
    QString buf;
    bool grainsflag=false, hopsflag=false, miscsflag=false;
    bool stylesflag=false, utilflag=false;
 
    data->grainmap_.clear();
    data->hopmap_.clear();
    data->miscmap_.clear();
    data->stylemap_.clear();
    data->utable_.clear();

    // parse file
    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            // qbrewdata tag
            if (name() == tagDoc) {
                // check version
                buf = attributes().value(attrVersion).toString();
                if (buf < DATA_PREVIOUS) {
                    // too old of a version
                    qWarning() << "Error: Unsupported data version";
                    raiseError("Unsupported data version");
                    return false;
                }
            }

            // styles
            else if (name() == tagStyles) {
                stylesflag = true;
            }
            // style
            else if (name() == tagStyle) {
                if (!stylesflag) qWarning("Warning: mislocated style tag");
                Style style;
                buf = attributes().value(attrOGLow).toString();
                style.setOGLow(buf.toDouble());
                buf = attributes().value(attrOGHigh).toString();
                style.setOGHi(buf.toDouble());
                buf = attributes().value(attrFGLow).toString();
                style.setFGLow(buf.toDouble());
                buf = attributes().value(attrFGHigh).toString();
                style.setFGHi(buf.toDouble());
                buf = attributes().value(attrIBULow).toString();
                style.setIBULow(buf.toInt());
                buf = attributes().value(attrIBUHigh).toString();
                style.setIBUHi(buf.toInt());
                buf = attributes().value(attrSRMLow).toString();
                style.setSRMLow(buf.toInt());
                buf = attributes().value(attrSRMHigh).toString();
                style.setSRMHi(buf.toInt());
                style.setName(readElementText());
                data->insertStyle(style);
            }

            // grains
            else if (name() == tagGrains) {
                grainsflag = true;
            }
            // grain
            else if (name() == tagGrain) {
                if (!grainsflag) qWarning("Warning: mislocated grain tag");
                Grain grain;
                grain.setWeight(Weight(1.0, data->defaultGrainUnit()));
                buf = attributes().value(attrExtract).toString();
                grain.setExtract(buf.toDouble());
                buf = attributes().value(attrColor).toString();
                grain.setColor(buf.toDouble());
                // TODO: grain type deprecates data file format (0.4.0)
                grain.setType(attributes().value(attrType).toString());
                grain.setUse(attributes().value(attrUse).toString());
                grain.setName(readElementText());
                data->insertGrain(grain);
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
                hop.setWeight(Weight(1.0, data->defaultHopUnit()));
                buf = attributes().value(attrAlpha).toString();
                hop.setAlpha(buf.toDouble());
                hop.setName(readElementText());
                data->insertHop(hop);
            }

            // miscs
            else if (name() == tagMiscs) {
                miscsflag = true;
            }
            // misc
            else if (name() == tagMisc) {
                if (!miscsflag) qWarning("Warning: mislocated misc tag");
                Misc misc;
                misc.setQuantity(Quantity(1.0, data->defaultMiscUnit()));
                misc.setType(attributes().value(attrType).toString());
                misc.setNotes(attributes().value(attrNotes).toString());
                misc.setName(readElementText());
                data->insertMisc(misc);
            }

            // utilization
            else if (name() == tagUtilization) {
                utilflag = true;
            }
            // entry
            else if (name() == tagEntry) {
                if (!utilflag) qWarning("Warning: mislocated entry tag");
                UEntry entry;
                buf = attributes().value(attrTime).toString();
                entry.time = buf.toUInt();
                buf = attributes().value(attrUtil).toString();
                entry.utilization = buf.toUInt();
                data->addUEntry(entry);
            }

            // unknown tag, skip
            else {
                qWarning() << "Warning: Unknown tag" << name().toString();
                skipElement();
            }
        }

        else if (isEndElement()) {
            // unset flags
            if (name() == tagStyles) stylesflag = false;
            else if (name() == tagGrains) grainsflag = false;
            else if (name() == tagHops) hopsflag = false;
            else if (name() == tagMiscs) miscsflag = false;
            else if (name() == tagUtilization) utilflag = false;
        }
    }

    return true;
}

// skip over xml element
void DataReader::skipElement()
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

// DataWriter /////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// DataWriter()
// ------------
// Constructor

DataWriter::DataWriter(QIODevice *device)
    : QXmlStreamWriter(device)
{
#if (QT_VERSION >= QT_VERSION_CHECK(4, 4, 0))
    setAutoFormatting(true);
    setAutoFormattingIndent(2);
#endif
}

///////////////////////////////////////////////////////////////////////////////
// writeData()
// -----------
// Write out data

bool DataWriter::writeData(Data *data)
{
    writeStartDocument();
    writeDTD(QString("<!DOCTYPE %1>").arg(tagDoc));

    // write root element
    writeStartElement(tagDoc);
    writeAttribute(attrVersion, VERSION);

    // write styles
    writeStartElement(tagStyles);
    foreach(Style style, data->stylemap_) {
        // iterate through grain list
        writeStartElement(tagStyle);
        writeAttribute(attrOGLow, QString::number(style.OGLow()));
        writeAttribute(attrOGHigh, QString::number(style.OGHi()));
        writeAttribute(attrFGLow, QString::number(style.FGLow()));
        writeAttribute(attrFGHigh, QString::number(style.FGHi()));
        writeAttribute(attrIBULow, QString::number(style.IBULow()));
        writeAttribute(attrIBUHigh, QString::number(style.IBUHi()));
        writeAttribute(attrSRMLow, QString::number(style.SRMLow()));
        writeAttribute(attrSRMHigh, QString::number(style.SRMHi()));
        writeCharacters(style.name());
        writeEndElement();
    }
    writeEndElement(); // tagStyles

    // write grains
    writeStartElement(tagGrains);
    foreach(Grain grain, data->grainmap_) {
        // iterate through grain list
        writeStartElement(tagGrain);
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
    foreach(Hop hop, data->hopmap_) {
        // iterate through hop list
        writeStartElement(tagHop);
        writeAttribute(attrAlpha, QString::number(hop.alpha()));
        writeCharacters(hop.name());
        writeEndElement();
    }
    writeEndElement(); // tagHops


    // write misc ingredients
    writeStartElement(tagMiscs);
    foreach(Misc misc, data->miscmap_) {
        // iterate through misc list
        writeStartElement(tagMisc);
        writeAttribute(attrType, misc.type());
        writeAttribute(attrNotes, misc.notes());
        writeCharacters(misc.name());
        writeEndElement();
    }
    writeEndElement(); // tagMiscs

    // write utilization table
    writeStartElement(tagUtilization);
    foreach(UEntry uentry, data->utable_) {
        // iterate through entries
        writeStartElement(tagEntry);
        writeAttribute(attrTime, QString::number(uentry.time));
        writeAttribute(attrUtil, QString::number(uentry.utilization));
        writeEndElement();
    }
    writeEndElement(); // tagMiscs

    writeEndElement(); // tagDoc
    writeEndDocument();

    return true;
}
