/***************************************************************************
  view.cpp
  -------------------
  View class for application
  -------------------
  Copyright 2006-2008 David Johnson
  Please see the header file for copyright and license information
 ***************************************************************************/

#include <QHeaderView>
#include <QTableView>
#include <QLocale>

#include "data.h"
#include "recipe.h"
#include "resource.h"

#include "graindelegate.h"
#include "grainmodel.h"
#include "hopdelegate.h"
#include "hopmodel.h"
#include "miscdelegate.h"
#include "miscmodel.h"
#include "notepage.h"
#include "view.h"

//////////////////////////////////////////////////////////////////////////////
// Construction, Destruction                                                //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// View()
// ------
// Constructor

View::View(QWidget *parent, Recipe *recipe)
    : QWidget(parent), recipe_(recipe), grainmodel_(0), hopmodel_(0),
      miscmodel_(0), notepage_(0)
{
    ui.setupUi(this);

    // get current font information
    QFont bold(font());
    bold.setBold(true);
    QFontMetrics fm(font());
    unsigned mh = (unsigned)(fm.lineSpacing() * 1.5);
    unsigned mw = fm.width('M');

    // additional setup
    ui.ogreclabel->setFont(bold);
    ui.ogrec->setFont(bold);
    ui.ibureclabel->setFont(bold);
    ui.iburec->setFont(bold);
    ui.srmreclabel->setFont(bold);
    ui.srmrec->setFont(bold);
    ui.abvlabel->setFont(bold);
    ui.abv->setFont(bold);
    ui.abwlabel->setFont(bold);
    ui.abw->setFont(bold);
    ui.fglabel->setFont(bold);
    ui.fg->setFont(bold);

    // grain page
    QWidget *widget = new QWidget();
    grainpage.setupUi(widget);
    ui.ingredients->addTab(widget, tr("&Grains"));

    grainmodel_ = new GrainModel(this, recipe_->grains());
    grainpage.view->setModel(grainmodel_);
    QItemDelegate *delegate = new GrainDelegate(this);
    grainpage.view->setItemDelegate(delegate);

    grainpage.view->verticalHeader()->setDefaultSectionSize(mh);
    grainpage.view->verticalHeader()->hide();
    grainpage.view->horizontalHeader()->setClickable(true);
    grainpage.view->horizontalHeader()->setHighlightSections(false);

    grainpage.view->setColumnWidth(GrainModel::NAME, 20*mw);
    grainpage.view->setColumnWidth(GrainModel::WEIGHT, 8*mw);
    grainpage.view->setColumnWidth(GrainModel::EXTRACT, 8*mw);
    grainpage.view->setColumnWidth(GrainModel::COLOR, 8*mw);
    grainpage.view->setColumnWidth(GrainModel::TYPE, 8*mw);
    grainpage.view->setColumnWidth(GrainModel::USE, 8*mw);

    // hop page
    widget = new QWidget();
    hoppage.setupUi(widget);
    ui.ingredients->addTab(widget, tr("&Hops"));

    hopmodel_ = new HopModel(this, recipe_->hops());
    hoppage.view->setModel(hopmodel_);
    delegate = new HopDelegate(this);
    hoppage.view->setItemDelegate(delegate);

    hoppage.view->verticalHeader()->setDefaultSectionSize(mh);
    hoppage.view->verticalHeader()->hide();
    hoppage.view->horizontalHeader()->setClickable(true);
    hoppage.view->horizontalHeader()->setHighlightSections(false);

    hoppage.view->setColumnWidth(HopModel::NAME, 20*mw);
    hoppage.view->setColumnWidth(HopModel::WEIGHT, 8*mw);
    hoppage.view->setColumnWidth(HopModel::ALPHA, 8*mw);
    hoppage.view->setColumnWidth(HopModel::TIME, 8*mw);
    hoppage.view->setColumnWidth(HopModel::TYPE, 8*mw);

    // misc page
    widget = new QWidget();
    miscpage.setupUi(widget);
    ui.ingredients->addTab(widget, tr("&Miscellaneous"));

    miscmodel_ = new MiscModel(this, recipe_->miscs());
    miscpage.view->setModel(miscmodel_);
    delegate = new MiscDelegate(this);
    miscpage.view->setItemDelegate(delegate);

    miscpage.view->verticalHeader()->setDefaultSectionSize(mh);
    miscpage.view->verticalHeader()->hide();
    miscpage.view->horizontalHeader()->setClickable(true);
    miscpage.view->horizontalHeader()->setHighlightSections(false);

    miscpage.view->setColumnWidth(MiscModel::NAME, 20*mw);
    miscpage.view->setColumnWidth(MiscModel::QUANTITY, 8*mw);
    miscpage.view->setColumnWidth(MiscModel::TYPE, 8*mw);
    miscpage.view->horizontalHeader()->setStretchLastSection(true);

    // note page
    notepage_ = new NotePage(0, recipe_);
    ui.ingredients->addTab(notepage_, tr("&Notes"));

    // widget connections
    connect(grainmodel_, SIGNAL(modified()),
            this, SLOT(modelModified()));
    connect(grainpage.addbutton, SIGNAL(clicked()),
            grainpage.view, SLOT(addIngredient()));
    connect(grainpage.removebutton, SIGNAL(clicked()),
            grainpage.view, SLOT(removeIngredient()));

    connect(hopmodel_, SIGNAL(modified()),
            this, SLOT(modelModified()));
    connect(hoppage.addbutton, SIGNAL(clicked()),
            hoppage.view, SLOT(addIngredient()));
    connect(hoppage.removebutton, SIGNAL(clicked()),
            hoppage.view, SLOT(removeIngredient()));

    connect(miscmodel_, SIGNAL(modified()),
            this, SLOT(modelModified()));
    connect(miscpage.addbutton, SIGNAL(clicked()),
            miscpage.view, SLOT(addIngredient()));
    connect(miscpage.removebutton, SIGNAL(clicked()),
            miscpage.view, SLOT(removeIngredient()));

    connect(ui.titleedit, SIGNAL(textChanged(const QString &)),
            this, SLOT(setTitle(const QString &)));
    connect(ui.stylecombo, SIGNAL(activated(const QString &)),
            this, SLOT(setStyle(const QString &)));
    connect(ui.breweredit, SIGNAL(textChanged(const QString &)),
            this, SLOT(setBrewer(const QString &)));
    connect(ui.sizespin, SIGNAL(valueChanged(double)),
            this, SLOT(setSize(double)));

    connect(recipe_, SIGNAL(recipeChanged()), this, SLOT(flush()));
    connect(recipe_, SIGNAL(recipeModified()), this, SLOT(refresh()));

    // start with new view
    flush();
}

View::~View() { ; }

//////////////////////////////////////////////////////////////////////////////
// Slot Implementations                                                     //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// setTitle()
// ----------
// set the recipe title

void View::setTitle(const QString &title)
{
    recipe_->setTitle(title);
}

//////////////////////////////////////////////////////////////////////////////
// setStyle()
// ----------
// set the recipe style

void View::setStyle(const QString &style)
{
    recipe_->setStyle(style);
}

//////////////////////////////////////////////////////////////////////////////
// Brewer()
// --------
// set the recipeCombo brewer

void View::setBrewer(const QString &brewer)
{
    recipe_->setBrewer(brewer);
}

//////////////////////////////////////////////////////////////////////////////
// setSize()
// ---------
// set the recipe size

void View::setSize(double size)
{
    recipe_->setSize(Volume(size, recipe_->size().unit()));
}

///////////////////////////////////////////////////////////////////////////////
// Miscellaneous                                                             //
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// modelModified()
// ---------------
// One of the models has been modified

void View::modelModified()
{
    recipe_->recalc();
    recipe_->setModified(true);
}

//////////////////////////////////////////////////////////////////////////////
// flush()
// -------
// flush/reset the entire view

void View::flush()
{
    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = recipe_->modified();
    recipe_->blockSignals(true);

    // update recipe widgets
    ui.titleedit->setText(recipe_->title());
    ui.breweredit->setText(recipe_->brewer());

    ui.sizespin->setValue(recipe_->size().amount());
    ui.sizespin->setSuffix(" " + recipe_->size().unit().name());

    // set style combo to style
    ui.stylecombo->clear();
    ui.stylecombo->addItems(Data::instance()->stylesList());
    int index = ui.stylecombo->findText(recipe_->style().name(),
                                        Qt::MatchExactly);
    if (index >= 0) {
        ui.stylecombo->setCurrentIndex(index);
    } else {
        ui.stylecombo->addItem(recipe_->style().name());
    }

    // restore modified flag
    recipe_->setModified(oldmod);
    recipe_->blockSignals(false);

    // reset ingredient models
    grainmodel_->flush();
    hopmodel_->flush();
    miscmodel_->flush();
    notepage_->refresh();

    grainpage.view->sortByColumn(0, Qt::AscendingOrder);
    hoppage.view->sortByColumn(0, Qt::AscendingOrder);
    miscpage.view->sortByColumn(0, Qt::AscendingOrder);

    // update style widgets
    refresh();
}

//////////////////////////////////////////////////////////////////////////////
// refresh()
// ---------
// refresh just the characteristics

void View::refresh()
{
    const Style &style = recipe_->style();
    QLocale locale = QLocale::system();

    // beer style has changed, so update labels in stylelayout_
    ui.stylebox->setTitle(tr("%1 characteristics",
                             "Beer style characteristics, e.g. Generic ale")
                          .arg(ui.stylecombo->currentText()));

    ui.ogmin->setText(locale.toString(style.OGLow(), 'f', 3));
    ui.ogmax->setText(locale.toString(style.OGHi(), 'f', 3));
    ui.ogrec->setText(locale.toString(recipe_->OG(), 'f', 3));

    ui.ibumin->setText(locale.toString(style.IBULow()));
    ui.ibumax->setText(locale.toString(style.IBUHi()));
    ui.iburec->setText(locale.toString(recipe_->IBU()));

    ui.srmmin->setText(locale.toString(style.SRMLow()) + Resource::DEGREE);
    ui.srmmax->setText(locale.toString(style.SRMHi()) + Resource::DEGREE);
    ui.srmrec->setText(locale.toString(recipe_->SRM()) + Resource::DEGREE);

    ui.fg->setText(locale.toString(recipe_->FGEstimate(), 'f', 3));
    ui.abv->setText(QString(tr("%1%"))
                    .arg(locale.toString(recipe_->ABV() * 100.0, 'f', 1)));
    ui.abw->setText(QString(tr("%1%"))
                    .arg(locale.toString(recipe_->ABW() * 100.0, 'f', 1)));
}
