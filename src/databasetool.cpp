/***************************************************************************
  databasetool.cpp
  -------------------
  Database editor for QBrew
  -------------------
  Copyright 2005-2008, David Johnson
  Please see the header file for copyright and license information
 ***************************************************************************/

#include <QDir>
#include <QFile>
#include <QHeaderView>
#include <QMessageBox>
#include <QTableView>

#include "data.h"
#include "resource.h"

#include "graindelegate.h"
#include "grainmodel.h"
#include "hopdelegate.h"
#include "hopmodel.h"
#include "miscdelegate.h"
#include "miscmodel.h"
#include "styledelegate.h"
#include "stylemodel.h"
#include "databasetool.h"

//////////////////////////////////////////////////////////////////////////////
// Construction, Destruction                                                //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// DatabaseTool()
// --------------
// Constructor

DatabaseTool::DatabaseTool(QWidget* parent)
    : QMainWindow(parent), grainmodel_(0), hopmodel_(0), miscmodel_(0),
      modified_(false)
{
    ui.setupUi(this);
    statusBar()->hide();
    ui.actionsave->setEnabled(false);

    // setup actions
    connect(ui.actionsave, SIGNAL(triggered()), this, SLOT(fileSave()));
    connect(ui.actionquit, SIGNAL(triggered()), this, SLOT(close()));

    // get current font information, for sizing
    QFontMetrics fm(font());
    unsigned mh = (unsigned)(fm.lineSpacing() * 1.5);
    unsigned mw = fm.width('M');

    // grain page
    QWidget *widget = new QWidget();
    grainpage.setupUi(widget);
    ui.ingredients->addTab(widget, tr("&Grains"));

    grains_ = Data::instance()->grainmap_.values();
    grainmodel_ = new GrainModel(this, &grains_);
    grainpage.view->setModel(grainmodel_);
    QItemDelegate *delegate = new GrainDelegate(this);
    grainpage.view->setItemDelegate(delegate);

    grainpage.view->verticalHeader()->setDefaultSectionSize(mh);
    grainpage.view->verticalHeader()->hide();
    grainpage.view->horizontalHeader()->setClickable(true);
    grainpage.view->horizontalHeader()->setHighlightSections(false);

    grainpage.view->setColumnWidth(GrainModel::NAME, 20*mw);
    grainpage.view->setColumnHidden(GrainModel::WEIGHT, true);
    grainpage.view->setColumnWidth(GrainModel::EXTRACT, 8*mw);
    grainpage.view->setColumnWidth(GrainModel::COLOR, 8*mw);
    grainpage.view->setColumnWidth(GrainModel::TYPE, 8*mw);
    grainpage.view->setColumnWidth(GrainModel::USE, 8*mw);

    // hop page
    widget = new QWidget();
    hoppage.setupUi(widget);
    ui.ingredients->addTab(widget, tr("&Hops"));

    hops_ = Data::instance()->hopmap_.values();
    hopmodel_ = new HopModel(this, &hops_);
    hoppage.view->setModel(hopmodel_);
    delegate = new HopDelegate(this);
    hoppage.view->setItemDelegate(delegate);

    hoppage.view->verticalHeader()->setDefaultSectionSize(mh);
    hoppage.view->verticalHeader()->hide();
    hoppage.view->horizontalHeader()->setClickable(true);
    hoppage.view->horizontalHeader()->setHighlightSections(false);

    hoppage.view->setColumnHidden(HopModel::WEIGHT, true);
    hoppage.view->setColumnHidden(HopModel::TIME, true);
    hoppage.view->setColumnHidden(HopModel::TYPE, true);
    hoppage.view->setColumnWidth(HopModel::NAME, 20*mw);
    hoppage.view->setColumnWidth(HopModel::ALPHA, 8*mw);

    // misc page
    widget = new QWidget();
    miscpage.setupUi(widget);
    ui.ingredients->addTab(widget, tr("&Miscellaneous"));

    miscs_ = Data::instance()->miscmap_.values();
    miscmodel_ = new MiscModel(this, &miscs_);
    miscpage.view->setModel(miscmodel_);
    delegate = new MiscDelegate(this);
    miscpage.view->setItemDelegate(delegate);

    miscpage.view->verticalHeader()->setDefaultSectionSize(mh);
    miscpage.view->verticalHeader()->hide();
    miscpage.view->horizontalHeader()->setClickable(true);
    miscpage.view->horizontalHeader()->setHighlightSections(false);

    miscpage.view->setColumnHidden(MiscModel::QUANTITY, true);
    miscpage.view->setColumnWidth(MiscModel::NAME, 20*mw);
    miscpage.view->setColumnWidth(MiscModel::TYPE, 8*mw);
    miscpage.view->horizontalHeader()->setStretchLastSection(true);

    // style page
    widget = new QWidget();
    stylepage.setupUi(widget);
    ui.ingredients->addTab(widget, tr("&Styles"));

    styles_ = Data::instance()->stylemap_.values();
    stylemodel_ = new StyleModel(this, &styles_);
    stylepage.view->setModel(stylemodel_);
    delegate = new StyleDelegate(this);
    stylepage.view->setItemDelegate(delegate);

    stylepage.view->verticalHeader()->setDefaultSectionSize(mh);
    stylepage.view->verticalHeader()->hide();
    stylepage.view->horizontalHeader()->setClickable(true);
    stylepage.view->horizontalHeader()->setHighlightSections(false);

    stylepage.view->setColumnWidth(StyleModel::NAME, 20*mw);
    stylepage.view->setColumnWidth(StyleModel::OGLOW, 8*mw);
    stylepage.view->setColumnWidth(StyleModel::OGHI, 8*mw);
    stylepage.view->setColumnWidth(StyleModel::FGLOW, 8*mw);
    stylepage.view->setColumnWidth(StyleModel::FGHI, 8*mw);
    stylepage.view->setColumnWidth(StyleModel::IBULOW, 8*mw);
    stylepage.view->setColumnWidth(StyleModel::IBUHI, 8*mw);
    stylepage.view->setColumnWidth(StyleModel::SRMLOW, 8*mw);
    stylepage.view->setColumnWidth(StyleModel::SRMHI, 8*mw);

    // setup connections
    connect(grainmodel_, SIGNAL(modified()),
            this, SLOT(dataModified()));
    connect(grainpage.addbutton, SIGNAL(clicked()),
            grainpage.view, SLOT(addIngredient()));
    connect(grainpage.removebutton, SIGNAL(clicked()),
            grainpage.view, SLOT(removeIngredient()));

    connect(hopmodel_, SIGNAL(modified()),
            this, SLOT(dataModified()));
    connect(hoppage.addbutton, SIGNAL(clicked()),
            hoppage.view, SLOT(addIngredient()));
    connect(hoppage.removebutton, SIGNAL(clicked()),
            hoppage.view, SLOT(removeIngredient()));

    connect(miscmodel_, SIGNAL(modified()),
            this, SLOT(dataModified()));
    connect(miscpage.addbutton, SIGNAL(clicked()),
            miscpage.view, SLOT(addIngredient()));
    connect(miscpage.removebutton, SIGNAL(clicked()),
            miscpage.view, SLOT(removeIngredient()));

    connect(stylemodel_, SIGNAL(modified()),
            this, SLOT(dataModified()));
    connect(stylepage.addbutton, SIGNAL(clicked()),
            stylepage.view, SLOT(addIngredient()));
    connect(stylepage.removebutton, SIGNAL(clicked()),
            stylepage.view, SLOT(removeIngredient()));

    grainmodel_->flush();
    hopmodel_->flush();
    miscmodel_->flush();
    stylemodel_->flush();
}

DatabaseTool::~DatabaseTool() {}

void DatabaseTool::fileSave()
{
    // TODO: use QDesktopServices in next non-bugfix release (0.5.0)
    QString localbase = QDIR_HOME + "/." + Resource::DATA_FILE;

    QFileInfo finfo(localbase);
    if (finfo.exists() && !finfo.isWritable()) {
        // no write permission
        QMessageBox::warning(this, Resource::TITLE,
                             tr("<p>Unable to save the database."
                                "You do not have permission "
                                "to write to %1").arg(localbase));
    } else {
        // sync with Data...
        Data::instance()->clearGrains();
        foreach(Grain grain, grains_) {
            Data::instance()->insertGrain(grain);
        }
        Data::instance()->clearHops();
        foreach(Hop hop, hops_) {
            Data::instance()->insertHop(hop);
        }
        Data::instance()->clearMiscs();
        foreach(Misc misc, miscs_) {
            Data::instance()->insertMisc(misc);
        }
        Data::instance()->clearStyles();
        foreach(Style style, styles_) {
            Data::instance()->insertStyle(style);
        }

        if (!Data::instance()->saveData(localbase)) {
            // error in saving file
            QMessageBox::warning(this, Resource::TITLE,
                                 tr("<p>Unable to save the database."
                                    "Error in saving %1").arg(localbase));
        }
        ui.actionsave->setEnabled(false);
        modified_ = false;
    }
}

void DatabaseTool::dataModified()
{
    ui.actionsave->setEnabled(true);
    modified_ = true;
}
