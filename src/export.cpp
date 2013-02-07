/***************************************************************************
  export.cpp
  -------------------
  Export methods for the Recipe class
  -------------------
  Copyright 2005-2008 David Johnson
  Please see the header file for copyright and license information.
 ***************************************************************************/

#include <QApplication>
#include <QFile>
#include <QInputDialog>
#include <QLocale>
#include <QMessageBox>
#include <QPrinter>
#include <QTextDocument>
#include <QTextStream>

#include "beerxmlreader.h"
#include "data.h"
#include "resource.h"
#include "textprinter.h"

#include "recipe.h"

using namespace Resource;

static QString escape(const QString &s);
static QString underline(const QString &line);

//////////////////////////////////////////////////////////////////////////////
// escape()
// --------
// Escape special xml/html characters

QString escape(const QString &s)
{
    QString x = s;
    x.replace(QLatin1Char('&'), QLatin1String("&amp;"));
    x.replace(QLatin1Char('<'), QLatin1String("&lt;"));
    x.replace(QLatin1Char('>'), QLatin1String("&gt;"));
    x.replace(QLatin1Char('\''), QLatin1String("&apos;"));
    x.replace(QLatin1Char('"'), QLatin1String("&quot;"));
    return x;
}

QString underline(const QString &line)
{
    QString text;
    text = line + '\n' + text.fill('-', line.length()) + '\n';
    return text;
}

//////////////////////////////////////////////////////////////////////////////
// exportHTML()
// ------------
// Export recipe as html

bool Recipe::exportHtml(const QString &filename)
{
    if (!filename.isEmpty()) {
        QFile datafile(filename);
        if (!datafile.open(QFile::WriteOnly | QFile::Text)) {
            // error opening file
            qWarning() << "Error: Cannot open file" << filename;
            QMessageBox::warning(0, TITLE,
                                 QObject::tr("Cannot write file %1:\n%2")
                                 .arg(filename)
                                 .arg(datafile.errorString()));
            datafile.close();
            return false;
        }

        QTextStream data(&datafile);
        QApplication::setOverrideCursor(Qt::WaitCursor);

        data << recipeHTML();

        QApplication::restoreOverrideCursor();
        datafile.close();
        return true;
    }  
    return false;
}

//////////////////////////////////////////////////////////////////////////////
// recipeHTML()
// ------------
// Get the html of the recipe for printing and exporting

const QString Recipe::recipeHTML()
{
    const QString header = "<big><strong>%1</strong></big>\n";
    const QString table = "<table summary=\"%1\" border=0 cellpadding=0 cellspacing=%2>\n";
    const QString th = "<td><strong>%1</strong></td>\n";
    
    // heading
    QString html = "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n";
    html += "<html>\n<head>\n";
    html += "<meta name=\"generator\" content=\"" + TITLE + " " + VERSION + " \">\n";
    html += "<title>" + escape(title_) + "</title>\n";
    html += "<meta name=\"author\" content=\"" + escape(brewer_) + "\">\n";
    html += "<style type='text/css'>\n";
    html += "body { font-family: sans-serif; }\n";
    html += "</style>\n";
    html += "</head>\n\n";

    html += "<body>\n";

    html += table.arg("header").arg("5 bgcolor=\"#CCCCCC\" width=\"100%\"");
    html += "<tr><td>\n" + header.arg(escape(title_)) + "</td></tr>\n";
    html += "</table>\n<p>\n\n";

    // recipe table
    html += table.arg("recipe").arg(0);
    html += "<tbody>\n<tr>\n";
    html += th.arg(tr("Recipe"));
    html += "<td width=\"15\"></td>\n";
    html += "<td>" + escape(title_) + "</td>\n";
    html += "<td width=\"25\"></td>\n";

    html += th.arg(tr("Style"));
    html += "<td width=\"15\"></td>\n";
    html += "<td>" + escape(style_.name()) + "</td>\n";
    html += "</tr>\n<tr>\n";

    html += th.arg(tr("Brewer"));
    html += "<td></td>\n";
    html += "<td>" + escape(brewer_) + "</td>\n";
    html += "<td></td>\n";

    html += th.arg(tr("Batch"));
    html += "<td></td>\n";
    html += "<td>" + size_.toString(2) + "</td>\n";

    html += "</tr>\n<tr>\n" + th.arg(tr(method().toUtf8()));
    html += "</tr>\n</tbody>\n</table>\n<p>\n\n";

    // characteristics table
    html += header.arg(tr("Recipe Characteristics"));
    html += table.arg("characteristics").arg(0);
    html += "<tbody>\n<tr>\n";

    html += th.arg(tr("Recipe Gravity"));
    html += "<td width=\"15\"></td>\n";
    html += "<td>" + QString::number(og_, 'f', 3) + tr(" OG</td>\n");
    html += "<td width=\"25\"></td>\n";

    html += th.arg(tr("Estimated FG"));
    html += "<td width=\"15\"></td>\n";
    html += "<td>" + QString::number(FGEstimate(), 'f', 3) + tr(" FG</td>\n");
    html += "</tr>\n<tr>\n";

    html += th.arg(tr("Recipe Bitterness"));
    html += "<td></td>\n";
    html += "<td>" + QString::number(ibu_, 'f', 0) + tr(" IBU</td>\n");
    html += "<td></td>\n";

    html += th.arg(tr("Alcohol by Volume"));
    html += "<td></td>\n";
    html += "<td>" + QString::number(ABV() * 100.0, 'f', 1) + tr("%</td>\n");
    html += "</tr>\n<tr>\n";

    html += th.arg(tr("Recipe Color"));
    html += "<td></td>\n";
    html += "<td>" + QString::number(srm_, 'f', 0) + DEGREE + tr(" SRM</td>\n");
    html += "<td></td>\n";

    html += th.arg(tr("Alcohol by Weight"));
    html += "<td></td>\n";
    html += "<td>" + QString::number(ABW() * 100.0, 'f', 1) + tr("%</td>\n");
    html += "</tr>\n</tbody>\n</table>\n<p>\n\n";

    // ingredients table
    html += header.arg(tr("Ingredients"));
    html += table.arg("ingredients").arg(0);
    html += "<tbody>\n";

    // grains
    html += "<tr>\n" + th.arg(tr("Quantity"));
    html += "<td width=\"15\"></td>\n";
    html += th.arg(tr("Grain"));
    html += "<td width=\"15\"></td>\n";
    html += th.arg(tr("Type"));
    html += "<td width=\"15\"></td>\n";
    html += th.arg(tr("Use"));
    html += "</tr>\n\n";

    foreach (Grain grain, grains_) {
        html += "<tr>\n<td>" + grain.weight().toString(2) + "</td>\n";
        html += "<td></td>\n";
        html += "<td>" + escape(grain.name()) + "</td>\n";
        html += "<td></td>\n";
        html += "<td>" + grain.type() + "</td>\n";
        html += "<td></td>\n";
        html += "<td>" + grain.use() + "</td>\n</tr>\n\n";
    }

    // hops
    html += "<tr>\n" + th.arg(tr("Quantity"));
    html += "<td width=\"15\"></td>\n";
    html += th.arg(tr("Hop"));
    html += "<td width=\"15\"></td>\n";
    html += th.arg(tr("Type"));
    html += "<td width=\"15\"></td>\n";
    html += th.arg(tr("Time"));
    html += "</tr>\n\n";

    foreach (Hop hop, hops_) {
        html += "<tr>\n<td>" + hop.weight().toString(2) + "</td>\n";
        html += "<td></td>\n";
        html += "<td>" + escape(hop.name()) + "</td>\n";
        html += "<td></td>\n";
        html += "<td>" + hop.type() + "</td>\n";
        html += "<td></td>\n";
        html += "<td colspan=2>" + QString::number(hop.time()) + tr(" minutes</td>\n</tr>\n\n");
    }

    // misc ingredients
    html += "<tr>\n" + th.arg(tr("Quantity"));
    html += "<td width=\"15\"></td>\n";
    html += th.arg(tr("Misc"));
    html += "<td width=\"15\"></td>\n";
    html += th.arg(tr("Notes"));
    html += "</tr>\n\n";

    foreach (Misc misc, miscs_) {
        html += "<tr>\n<td>" + misc.quantity().toString(2) + "</td>\n";
        html += "<td></td>\n";
        html += "<td>" + escape(misc.name()) + "</td>\n";
        html += "<td></td>\n";
        html += "<td>" + misc.type() + "</td>\n";
        html += "<td></td>\n";
        html += "<td colspan=3>" + escape(misc.notes()) + "</td>\n</tr>\n\n";
    }

    html += "</tbody>\n</table>\n<p>\n\n";

    // notes
    // TODO: using replace() might be dangerous if we ever use richtext in notes
    html += header.arg(tr("Recipe Notes")) + "\n";
    html += "<p>" + escape(recipenotes_).replace('\n', "<br>\n") + "\n</p>\n<p>\n";

    html += header.arg(tr("Batch Notes")) + "\n";
    html += "<p>" + escape(batchnotes_).replace('\n', "<br>\n") + "\n</p>\n";

    html += "</body>\n</html>\n";

    return html;
}

//////////////////////////////////////////////////////////////////////////////
// exportText()
// ------------
// Export recipe as text

bool Recipe::exportText(const QString &filename)
{
    if (!filename.isEmpty()) {
        QFile datafile(filename);
        if (!datafile.open(QFile::WriteOnly | QFile::Text)) {
        // error opening file
            qWarning() << "Error: Cannot open file" << filename;
            QMessageBox::warning(0, TITLE,
                                 QObject::tr("Cannot write file %1:\n%2")
                                 .arg(filename)
                                 .arg(datafile.errorString()));
            datafile.close();
            return false;
        }

        QTextStream data(&datafile);
        QApplication::setOverrideCursor(Qt::WaitCursor);

        data << recipeText();

        QApplication::restoreOverrideCursor();
        datafile.close();
        return true;
    }  
    return false;
}

//////////////////////////////////////////////////////////////////////////////
// recipeText()
// ------------
// Get the ascii text of the recipe for exporting

const QString Recipe::recipeText()
{
    // title stuff
    QString text = underline(title());
    text += tr("Brewer: ") + brewer_ + '\n';
    text += tr("Style: ") +  style_.name() + '\n';
    text += tr("Batch: ") + size_.toString(2);
    text += tr(method().toUtf8());
    text += "\n\n";

    // style stuff
    text += underline(tr("Characteristics"));
    text += tr("Recipe Gravity: ") +
        QString::number(og_, 'f', 3) + tr(" OG\n");
    text += tr("Recipe Bitterness: ") +
        QString::number(ibu_, 'f', 0) + tr(" IBU\n");
    text += tr("Recipe Color: ") +
        QString::number(srm_, 'f', 0) + DEGREE + tr(" SRM\n");
    text += tr("Estimated FG: ") +
        QString::number(FGEstimate(), 'f', 3) + '\n';
    text += tr("Alcohol by Volume: ") +
        QString::number(ABV() * 100.0, 'f', 1) + tr("%\n");
    text += tr("Alcohol by Weight: ") +
        QString::number(ABW() * 100.0, 'f', 1) + tr("%\n\n");

    // ingredients
    text += underline(tr("Ingredients"));

    // grains
    // using a map will sort the grains
    QMultiMap<QString, Grain> gmap;
    foreach (Grain grain, grains_) gmap.insert(grain.name(), grain);

    foreach (Grain grain, gmap.values()) {
        text += grain.name().leftJustified(30, ' ');
        text += grain.weight().toString(2) + ", ";
        text += grain.type() + ", ";
        text += grain.use() + '\n';
    }
    text += '\n';

    // hops
    // using a map will sort the hops
    QMultiMap<QString, Hop> hmap;
    foreach (Hop hop, hops_) hmap.insert(hop.name(), hop);

    foreach (Hop hop, hmap.values()) {
        text += hop.name().leftJustified(30, ' ');
        text += hop.weight().toString(2) + ", ";
        text += hop.type() + ", ";
        text += QString::number(hop.time()) + tr(" minutes\n");
    }
    text += '\n';

    // misc ingredients
    // using a map will sort the ingredients
    QMultiMap<QString, Misc> mmap;
    foreach (Misc misc, miscs_) mmap.insert(misc.name(), misc);

    foreach (Misc misc, mmap.values()) {
        text += misc.name().leftJustified(30, ' ');
        text += misc.quantity().toString(2) + ", ";
        text += misc.type() + ", ";
        text += misc.notes() + '\n';
    }
    text += '\n';

    // notes
    text += underline(tr("Notes"));

    // TODO: wrap long notes
    text += tr("Recipe Notes:\n") + recipenotes_ + "\n\n";
    text += tr("Batch Notes:\n") + batchnotes_ + "\n\n";

    return text;
}

//////////////////////////////////////////////////////////////////////////////
// exportBeerXML()
// ---------------
// Export recipe to BeerXML format

bool Recipe::exportBeerXML(const QString &filename)
{
    // open file
    QFile datafile(filename);
    if (!datafile.open(QFile::WriteOnly | QFile::Text)) {
        // error opening file
        qWarning() << "Error: Cannot open file" << filename;
        QMessageBox::warning(0, TITLE,
                             QObject:: tr("Cannot write file %1:\n%2")
                             .arg(filename)
                             .arg(datafile.errorString()));
        datafile.close();
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    // write out xml
    BeerXmlWriter writer(&datafile);
    writer.writeRecipe(this);
    datafile.close();

    // recipe is saved, so set flags accordingly
    setModified(false);
    QApplication::restoreOverrideCursor();
    return true;
}

//////////////////////////////////////////////////////////////////////////////
// importBeerXml()
// ---------------
// Import recipe from BeerXML format. Assumes beerXmlFormat() has been called

bool Recipe::importBeerXml(const QString &filename)
{
    // open file
    QFile datafile(filename);
    if (!datafile.open(QFile::ReadOnly | QFile::Text)) {
        // error opening file
        qWarning() << "Error: Cannot open" << filename;
        QMessageBox::warning(0, TITLE,
                             QObject::tr("Cannot read file %1:\n%2")
                             .arg(filename)
                             .arg(datafile.errorString()));
        datafile.close();
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    title_.clear();
    brewer_.clear();
    size_ = Data::instance()->defaultSize();
    style_ = Data::instance()->defaultStyle();
    grains_.clear();
    hops_.clear();
    miscs_.clear();
    recipenotes_.clear();
    batchnotes_.clear();

    // parse file
    BeerXmlReader reader(&datafile);
    bool status = reader.readRecipe(this);
    datafile.close();

    if (!status) {
        qWarning() << "Error: Problem reading file" << filename;
        qWarning() << reader.errorString();
        QMessageBox::warning(0, TITLE,
                             tr("Error reading file %1").arg(filename));
        QApplication::restoreOverrideCursor();
        return false;
    }


    // calculate the numbers
    recalc();

    // just loaded recipes  are not modified
    setModified(false);
    emit recipeChanged();

    QApplication::restoreOverrideCursor();
    return true;
}

//////////////////////////////////////////////////////////////////////////////
// exportPdf()
// ------------
// Export recipe as pdf

bool Recipe::exportPdf(TextPrinter *textprinter, const QString &filename)
{
    if (textprinter && (!filename.isEmpty())) {
        QTextDocument document;
        document.setHtml(recipeHTML());
        textprinter->exportPdf(&document, QString(), filename);
        return true;
    }
    return false;
}
