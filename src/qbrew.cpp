/***************************************************************************
  qbrew.cpp
  -------------------
  Control class for QBrew
  -------------------
  Copyright 2006-2008 David Johnson
  Please see the header file for copyright and license information.
***************************************************************************/

#include <QtGui>

#include "alcoholtool.h"
#include "configure.h"
#include "data.h"
#include "databasetool.h"
#include "recipe.h"
#include "helpviewer.h"
#include "hydrometertool.h"
#include "resource.h"
#include "textprinter.h"
#include "view.h"

#include "qbrew.h"

using namespace Resource;

QBrew *QBrew::instance_ = 0;
static QMutex instancelock;

//////////////////////////////////////////////////////////////////////////////
// Construction, Destruction, Initialization                                //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// QBrew()
// ------------
// Private constructor

QBrew::QBrew()
    : data_(0), recipe_(0), view_(0), alcoholtool_(0), databasetool_(0),
      hydrometertool_(0), configure_(0), handbook_(0), primer_(0), state_(),
      filename_(), newflag_(false), backed_(false), autosave_(0),
      autosavename_(), textprinter_(0)
{ ; }

//////////////////////////////////////////////////////////////////////////////
// initialize()
// ------------
// Initialize mainwindow

void QBrew::initialize(const QString &filename)
{
    setAttribute(Qt::WA_DeleteOnClose);
    readConfig();

    // show splashscreen
    if (state_.general.showsplash
        && QFile::exists(dataBase() + tr("splash.png"))) {
        QSplashScreen *splash = new QSplashScreen(dataBase() + tr("splash.png"),
						 Qt::WindowStaysOnTopHint);
        splash->show();
        QApplication::flush();
        QTimer::singleShot(3000, splash, SLOT(close()));
    }

    // setup look and feel
    setWindowIcon(QIcon(":/pics/qbrew.png"));
    if ((state_.general.lookfeel != QApplication::style()->objectName()) &&
        (QStyleFactory::keys().contains(state_.general.lookfeel))) {
        QApplication::setStyle(state_.general.lookfeel);
    }

    // setup UI
    ui.setupUi(this);
    initActions();
#if defined (Q_WS_MAC)
    // initial defaults for mac
    setUnifiedTitleAndToolBarOnMac(false);
    ui.maintoolbar->setVisible(false);
#endif

    // restore window state
    restoreState();

    // initialize data
    data_ = Data::instance();
    data_->initialize(state_);

    // initialize calculations
    data_->setSteepYield(state_.calc.steepyield);
    data_->setTinseth(state_.calc.tinseth);
    data_->setMorey(state_.calc.morey);

    // create recipe
    recipe_ = new Recipe(this);
    connect(recipe_, SIGNAL(recipeModified()), this, SLOT(recipeModified()));

    // create view
    view_ = new View(this, recipe_);

    // load or create a recipe
    bool emptyfilename = (filename.isEmpty() ||
                          (filename == tr(DEFAULT_FILE)));
    if (emptyfilename &&
        (state_.general.loadlast && !state_.general.recentfiles.isEmpty())) {
        if (QFile::exists(state_.general.recentfiles.first())) {
            filename_ = state_.general.recentfiles.first();
        }
    } else {
        filename_ = filename;
    }
    emptyfilename = (filename_.isEmpty() ||
                     (filename_ == tr(DEFAULT_FILE)));
    if (!emptyfilename) {
        openFile(filename_);
    } else {
        recipe_->newRecipe();
        newflag_ = true;
        backed_ = false;
        filename_ = tr(DEFAULT_FILE) + "." + FILE_EXT;
        setWindowTitle(TITLE + " " + VERSION + " [*]");
    }

    // connections
    connect(ui.menuopenrecent, SIGNAL(aboutToShow()),
            this, SLOT(recentMenuShow()));

    // other initialization
    initAutoSave();
    setCentralWidget(view_);
    view_->refresh();
    textprinter_ = new TextPrinter(this);
}

//////////////////////////////////////////////////////////////////////////////
// ~QBrew()
// -------------
// Private destructor

QBrew::~QBrew()
{
    saveState();

    // make sure any open windows are closed
    if (alcoholtool_) alcoholtool_->close();
    if (hydrometertool_) hydrometertool_->close();
    if (databasetool_) databasetool_->close();
    if (configure_) configure_->close();
    if (handbook_) handbook_->close();
    if (primer_) primer_->close();
}

//////////////////////////////////////////////////////////////////////////////
// instance()
// ----------
// Return pointer to the instance

QBrew *QBrew::instance()
{
    if (!instance_) {
        QMutexLocker lock(&instancelock);
        if (!instance_) instance_ = new QBrew();
    }
    return instance_;
}

//////////////////////////////////////////////////////////////////////////////
// initActions()
// -------------
// Initialize the actions

void QBrew::initActions()
{
    connect(ui.actionfilenew, SIGNAL(triggered()),
            this, SLOT(fileNew()));
    connect(ui.actionfileopen, SIGNAL(triggered()),
            this,  SLOT(fileOpen()));
    connect(ui.actionfilesave, SIGNAL(triggered()),
            this, SLOT(fileSave()));
    connect(ui.actionfilesaveas, SIGNAL(triggered()),
            this, SLOT(fileSaveAs()));
    connect(ui.actionfileprintpreview, SIGNAL(triggered()),
            this, SLOT(filePrintPreview()));
    connect(ui.actionfileprint, SIGNAL(triggered()),
            this, SLOT(filePrint()));
    connect(ui.actionfileexport, SIGNAL(triggered()),
            this, SLOT(fileExport()));
    connect(ui.actionexit, SIGNAL(triggered()), qApp,
            SLOT(closeAllWindows()));

    connect(ui.actionalcoholtool, SIGNAL(triggered()),
            this, SLOT(toolsAlcohol()));
    connect(ui.actionhydrometertool, SIGNAL(triggered()),
            this, SLOT(toolsHydrometer()));
    connect(ui.actiondatabasetool, SIGNAL(triggered()),
            this, SLOT(toolsDatabase()));

    connect(ui.actiontogglestatusbar, SIGNAL(toggled(bool)),
            this, SLOT(optionsStatusbar(bool)));
    connect(ui.actionconfigure, SIGNAL(triggered()),
            this, SLOT(optionsConfigure()));

    connect(ui.actionhelpcontents, SIGNAL(triggered()),
            this, SLOT(helpContents()));
    connect(ui.actionprimer, SIGNAL(triggered()),
            this, SLOT(helpPrimer()));
    connect(ui.actioncontexthelp, SIGNAL(triggered()),
            this, SLOT(helpContext()));
    connect(ui.actionabout, SIGNAL(triggered()), this, SLOT(helpAbout()));

#if (QT_VERSION < QT_VERSION_CHECK(4, 4, 0))
    ui.actionfileprintpreview->setVisible(false);
#endif

    // insert toolbar toggle action
    ui.maintoolbar->toggleViewAction()->setStatusTip(tr("Toggle the toolbar"));
    ui.maintoolbar->toggleViewAction()->setWhatsThis(tr("Enable or disable the Toolbar"));
    ui.menuoptions->insertAction(ui.actiontogglestatusbar,
                                 ui.maintoolbar->toggleViewAction());
}

//////////////////////////////////////////////////////////////////////////////
// initAutoSave()
// ---------------
// Initialize the autosave timer

void QBrew::initAutoSave()
{
    // destroy existing timer, if any
    if (autosave_) {
        autosave_->stop();
        disconnect(autosave_, SIGNAL(timeout()), this, SLOT(autoSave()));
        delete autosave_; autosave_ = 0;
    }

    autosavename_ = QDir::currentPath() + tr("/autosave.") + FILE_EXT;

    if (state_.general.autosave) {
        autosave_ = new QTimer(this);
        connect(autosave_, SIGNAL(timeout()), this, SLOT(autoSave()));
        autosave_->start(state_.general.saveinterval * 60000);
    }
}

//////////////////////////////////////////////////////////////////////////////
// File Menu Implementation                                                 //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// fileNew()
// ---------
// Create a new recipe

void QBrew::fileNew()
{
    if (recipe_->modified()) {
        // query user to save file
        switch (querySave()) {
          case QMessageBox::Yes:
              fileSave();
              break;
          case QMessageBox::No:
              break;
          case QMessageBox::Cancel:
              statusBar()->showMessage(tr("Canceled..."), 2000);
              // exit function
              return;
        }
    }

    // create a new file
    recipe_->newRecipe();
    newflag_ = true;
    backed_ = false;

    ui.actionfilesave->setEnabled(true);

    filename_ = tr(DEFAULT_FILE);
    setFileCaption(filename_);
    statusBar()->showMessage(tr("Created new recipe"), 2000);
}

//////////////////////////////////////////////////////////////////////////////
// fileOpen()
// ----------
// Open a recipe

void QBrew::fileOpen(const QString &filename)
{
    if (recipe_->modified()) {
        // query user to save file
        switch (querySave()) {
          case QMessageBox::Yes:
              fileSave();
              break;
          case QMessageBox::No:
              break;
          case QMessageBox::Cancel:
              statusBar()->showMessage(tr("Canceled..."), 2000);
              return;
        }
    }

    // open the file
    statusBar()->showMessage(tr("Opening recipe..."));
    QString fname;
    if (filename.isEmpty()) {
        fname = QFileDialog::getOpenFileName(this,
                                             tr("Open..."),
                                             QString(),
                                             tr(OPEN_FILTER));
    } else {
        fname = filename;
    }
    if (openFile(fname)) {
        statusBar()->showMessage(tr("Loaded recipe"), 2000);
    } else {
        statusBar()->showMessage(tr("Load failed"), 2000);
    }
}

//////////////////////////////////////////////////////////////////////////////
// fileRecent()
// -----------
// Selection has been made from recent file menu

void QBrew::fileRecent()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action) return;

    if (recipe_->modified()) {
        // file needs to be saved, what do we do
        switch (querySave()) {
          case QMessageBox::Yes:
              fileSave();
              break;
          case QMessageBox::No:
              break;
          case QMessageBox::Cancel:
              statusBar()->showMessage(tr("Canceled..."), 2000);
              return;
        }
    }

    // open the file
    QString fname = action->text();
    if (openFile(fname)) {
        statusBar()->showMessage(tr("Loaded recipe"), 2000);
        addRecent(fname);
    }
}

//////////////////////////////////////////////////////////////////////////////
// fileSave()
// ----------
// Save a recipe

void QBrew::fileSave()
{
    if (newflag_) {
        fileSaveAs();
    } else {
        if (state_.general.autobackup) {
            if (!backupFile())
                QMessageBox::warning(this, TITLE,
                    tr("%1 was unable to make a backup of %2")
                                     .arg(PACKAGE).arg(filename_));
        }
        if (saveFile(filename_)) {
            // successful in saving file
            newflag_ = false;
            ui.actionfilesave->setEnabled(false);
            setFileCaption(filename_);
            statusBar()->showMessage(tr("Saved recipe"), 2000);
        } else {
            // error in saving file
            QMessageBox::warning(this, TITLE,
                                 tr("%1 was unable to save %2")
                                 .arg(PACKAGE).arg(filename_));
            statusBar()->showMessage(tr("Error in saving recipe"), 2000);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// fileSaveAs()
// ------------
// Save a recipe under a new name

void QBrew::fileSaveAs()
{
    statusBar()->showMessage(tr("Saving recipe under new filename..."));
    QString selected;
    QString fname;
    fname = QFileDialog::getSaveFileName(this,
                                         tr("Save As..."),
                                         QString(),
                                         tr(SAVE_FILTER),
                                         &selected,
                                         QFileDialog::DontConfirmOverwrite);
    if (!fname.isEmpty()) { // we got a filename
        // add suffix if there is none
        if (!QFile::exists(fname)) {
            if (fname.indexOf('.') == -1) fname += '.' + FILE_EXT;
        }

        if (state_.general.autobackup) backupFile();
        if (QFile::exists(fname)) {
            // overwrite?
            switch (queryOverwrite(fname)) {
              case QMessageBox::Yes:
                  break;
              case QMessageBox::Cancel:
                  statusBar()->showMessage(tr("Canceled..."), 2000);
                  return;
            }
        }

        if (saveFile(fname)) {
            // successfully saved
            newflag_ = false;
            addRecent(fname);
            ui.actionfilesave->setEnabled(false);
            setFileCaption(fname);
            statusBar()->showMessage(tr("Saved recipe"), 2000);
            // save name of file
            filename_ = fname;
        } else {
            // error in saving
            QMessageBox::warning(this, TITLE,
                                 tr("Unable to save to %1")
                                 .arg(QFileInfo(filename_).fileName()));
            statusBar()->showMessage(tr("Error in saving recipe"), 2000);
        }
    } else {
        // no file chosen
        statusBar()->showMessage(tr("Saving aborted"), 2000);
    }
}

//////////////////////////////////////////////////////////////////////////////
// fileExport()
// ------------
// Export recipe to non-native format

void QBrew::fileExport()
{
    bool status = false;
    QString selected;
    QString fname;
    QString dirfile;

    fname = QFileDialog::getSaveFileName(this,
                                         tr("Export..."),
                                         QString(),
                                         tr(EXPORT_FILTER),
                                         &selected,
                                         QFileDialog::DontConfirmOverwrite);

    if (!fname.isEmpty()) { // we got a filename
        // add suffix if there is none
        if (!QFile::exists(fname)) {
            if (fname.indexOf('.') == -1) {
               if (selected == tr(HTML_FILTER)) fname += ".html";
               else if (selected == tr(BEERXML_FILTER)) fname += ".xml";
               else if (selected == tr(PDF_FILTER)) fname += ".pdf";
               else if (selected == tr(TEXT_FILTER)) fname += ".txt";
            }
        }

        if (QFile::exists(fname)) {
            // overwrite?
            switch (queryOverwrite(fname)) {
              case QMessageBox::Yes:
                  break;
              case QMessageBox::Cancel:
                  statusBar()->showMessage(tr("Canceled..."), 2000);
                  return;
            }
        }

        // export recipe
        if (selected == tr(HTML_FILTER)) {
            status = recipe_->exportHtml(fname);
        } else if (selected == tr(BEERXML_FILTER)) {
            status = recipe_->exportBeerXML(fname);
        } else if (selected == tr(PDF_FILTER)) {
            status = recipe_->exportPdf(textprinter_, fname);
        } else if (selected == tr(TEXT_FILTER)) {
            status = recipe_->exportText(fname);
        }
        if (status) {
            // successfully exported
            statusBar()->showMessage(tr("Exported recipe"), 2000);
        } else {
            // error in exporting
            QMessageBox::warning(this, TITLE,
                                 tr("Unable to export to %1")
                                 .arg(QFileInfo(filename_).fileName()));
            statusBar()->showMessage(tr("Error exporting recipe"), 2000);
        }
    } else {
        // no file chosen
        statusBar()->showMessage(tr("Export aborted"), 2000 );
    }
}

//////////////////////////////////////////////////////////////////////////////
// filePrintPreview()
// ------------
// Print the recipe

void QBrew::filePrintPreview()
{
    statusBar()->showMessage(tr("Print preview..."));

    recipe_->previewRecipe(textprinter_);
    statusBar()->showMessage(tr(READY), 2000);
}

//////////////////////////////////////////////////////////////////////////////
// filePrint()
// ------------
// Print the recipe

void QBrew::filePrint()
{
    statusBar()->showMessage(tr("Printing..."));

    recipe_->printRecipe(textprinter_);
    statusBar()->showMessage(tr(READY), 2000);
}

//////////////////////////////////////////////////////////////////////////////
// Tools Menu Implementation                                                //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// toolsAlcohol()
// --------------
// A utility dialog for alchohol calculation

void QBrew::toolsAlcohol()
{
    if (!alcoholtool_) alcoholtool_  = new AlcoholTool(this);
    alcoholtool_->show();
    alcoholtool_->raise();
    if (alcoholtool_->isMinimized()) alcoholtool_->showNormal();

    statusBar()->showMessage(tr(READY), 2000);
}

//////////////////////////////////////////////////////////////////////////////
// toolsHydrometer()
// -----------------
// A utility dialog for hydrometer correction

void QBrew::toolsHydrometer()
{
    if (!hydrometertool_) hydrometertool_ = new HydrometerTool(this);
    hydrometertool_->show();
    hydrometertool_->raise();
    if (hydrometertool_->isMinimized()) hydrometertool_->showNormal();

    statusBar()->showMessage(tr(READY), 2000);
}

//////////////////////////////////////////////////////////////////////////////
// toolsDatabase()
// ---------------
// A database editor for ingredients

void QBrew::toolsDatabase()
{
    if (!databasetool_)
        databasetool_ = new DatabaseTool(this);
    databasetool_->show();
    databasetool_->raise();
    if (databasetool_->isMinimized()) databasetool_->showNormal();

    statusBar()->showMessage(tr(READY), 2000);
}

//////////////////////////////////////////////////////////////////////////////
// Options Menu Implementation                                              //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// optionsSatusbar()
// -----------------
// Toggle statusbar status

void QBrew::optionsStatusbar(bool on)
{
    statusBar()->setVisible(on);
    state_.window.statusbar = on;
    statusBar()->showMessage(tr(READY), 2000);
}

//////////////////////////////////////////////////////////////////////////////
// optionsConfigure()
// ------------
// Display the configuration dialog

void QBrew::optionsConfigure()
{
    statusBar()->showMessage(tr("Configuring %1 ...").arg(PACKAGE), 2000);

    if (!configure_) {
        configure_ = new Configure(this);

        connect(configure_, SIGNAL(generalApply(const GenConfigState&)),
                this, SLOT(applyGeneralState(const GenConfigState&)));
        connect(configure_, SIGNAL(recipeApply(const RecipeConfigState&)),
                this, SLOT(applyRecipeState(const RecipeConfigState&)));
        connect(configure_, SIGNAL(calcApply(const CalcConfigState&)),
                this, SLOT(applyCalcState(const CalcConfigState&)));
        connect(configure_, SIGNAL(configureAccept()),
                this, SLOT(saveConfig()));
    }

    if (!configure_->isVisible()) {
        configure_->setState(state_);
    }
    configure_->show(); // non-modal
    configure_->raise();
    if (configure_->isMinimized()) configure_->showNormal();

    statusBar()->showMessage(tr(READY), 2000);
}

//////////////////////////////////////////////////////////////////////////////
// Help Menu Implementation                                                 //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// helpContents()
// --------------
// Display the application manual

void QBrew::helpContents()
{
    QString home = docBase() + tr(HELP_FILE);

    if (!handbook_) handbook_ = new HelpViewer(home, 0);
    handbook_->show();
    handbook_->raise();
    if (handbook_->isMinimized()) handbook_->showNormal();

    statusBar()->showMessage(tr(READY), 2000);
}

//////////////////////////////////////////////////////////////////////////////
// helpPrimer()
// --------------
// Display the brewing primer

void QBrew::helpPrimer()
{
    QString home = docBase() + tr(PRIMER_FILE);

    if (!primer_) primer_ = new HelpViewer(home, 0);
    primer_->show();
    primer_->raise();
    if (primer_->isMinimized()) primer_->showNormal();

    statusBar()->showMessage(tr(READY), 2000);
}

//////////////////////////////////////////////////////////////////////////////
// helpContext()
// -------------
// Display context help cursor

void QBrew::helpContext()
{
    QWhatsThis::enterWhatsThisMode();
}

//////////////////////////////////////////////////////////////////////////////
// helpAbout()
// -----------
// Display the About dialog

void QBrew::helpAbout()
{
    QString message;

    message = "<center><nobr><big><strong>" + TITLE + tr(" Version ")
        + VERSION + "</strong></big></nobr></center>";
    message += "<p align=center><strong>" + tr(DESCRIPTION) + "</strong></p>";
    message += "<p align=center>" + tr(COPYRIGHT) + ' ' + AUTHOR + "</p>";
    message += tr("<p align=center><small>Contributions by ")
        + CONTRIBUTORS + "</small></p>";

    QMessageBox::about(this, tr("About ") + TITLE, message);
}

//////////////////////////////////////////////////////////////////////////////
// Settings                                                                 //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// applyGeneralState()
// -------------------
// Set the general configuration state

void QBrew::applyGeneralState(const GenConfigState &state)
{
    GenConfigState oldstate = state_.general;
    state_.general = state;

    // apply immediate state changes
    if (state.lookfeel != oldstate.lookfeel) {
        QApplication::setStyle(state.lookfeel);
    }
    if ((state.autosave != oldstate.autosave) ||
        (state.saveinterval != oldstate.saveinterval)) {
        initAutoSave();
    }
    if (state.recentnum < oldstate.recentnum) {
        while (state_.general.recentfiles.count() > state_.general.recentnum) {
            state_.general.recentfiles.removeLast();
        }
    }
    statusBar()->showMessage(tr(READY), 2000);
}

//////////////////////////////////////////////////////////////////////////////
// applyRecipeState()
// ------------------
// Set the recipe configuration state

void QBrew::applyRecipeState(const RecipeConfigState &state)
{
    RecipeConfigState oldstate = state_.recipe;
    state_.recipe = state;

    // apply state changes
    if (state.batch != oldstate.batch) {
        if (state_.calc.units == UNIT_METRIC) {
            data_->setDefaultSize(Volume(state.batch, Volume::liter));
        } else if (state_.calc.units == UNIT_US) {
            data_->setDefaultSize(Volume(state.batch, Volume::gallon));
        }
    }
    if (state.style != oldstate.style) {
        data_->setDefaultStyle(state.style);
    }
    if (state.hoptype != oldstate.hoptype) {
        data_->setDefaultHopType(state.hoptype);
    }

    statusBar()->showMessage(tr(READY), 2000);
}

//////////////////////////////////////////////////////////////////////////////
// applyCalcState()
// ----------------
// Set the calc configuration state

void QBrew::applyCalcState(const CalcConfigState &state)
{
    CalcConfigState oldstate = state_.calc;
    state_.calc = state;

    // apply state changes
    if (state.steepyield != oldstate.steepyield) {
        data_->setSteepYield(state.steepyield);
    }
    if (state.efficiency != oldstate.efficiency) {
        data_->setEfficiency(state.efficiency);
    }
    if (state.morey != oldstate.morey) {
        data_->setMorey(state.morey);
    }
    if (state.tinseth != oldstate.tinseth) {
        data_->setTinseth(state.tinseth);
    }
    if (state.units != oldstate.units) {
        if (state.units == UNIT_METRIC) {
            data_->setDefaultSize(Volume(state_.recipe.batch,
                                          Volume::liter));
            data_->setDefaultGrainUnit(Weight::kilogram);
            data_->setDefaultHopUnit(Weight::gram);
            data_->setDefaultTempUnit(Temperature::celsius);
        } else if (state.units == UNIT_US) {
            data_->setDefaultSize(Volume(state_.recipe.batch,
                                          Volume::gallon));
            data_->setDefaultGrainUnit(Weight::pound);
            data_->setDefaultHopUnit(Weight::ounce);
            data_->setDefaultTempUnit(Temperature::fahrenheit);
        }
        // delete hydrometer tool
        if (hydrometertool_) {
            delete hydrometertool_;
            hydrometertool_ = 0;
        }
    }


    // recalculate
    recipe_->recalc();
    view_->flush();

    statusBar()->showMessage(tr(READY), 2000);
}

//////////////////////////////////////////////////////////////////////////////
// saveConfig()
// -----------
// Save the configuration (when OK pressed in dialog)

void QBrew::saveConfig()
{
    writeConfig();
    statusBar()->showMessage(tr("Saved configuration"), 2000);
}

//////////////////////////////////////////////////////////////////////////////
// readConfig()
// ------------
// Read configuration from settings file

void QBrew::readConfig()
{
    QSettings config;
    config.beginGroup(QString("/") + PACKAGE);

    // read general config
    config.beginGroup(CONFGROUP_GENERAL);
    state_.general.lookfeel =
        config.value(CONF_GEN_LOOK_FEEL, state_.general.lookfeel).toString();
    state_.general.showsplash =
        config.value(CONF_GEN_SHOW_SPLASH, state_.general.showsplash).toBool();
    state_.general.autosave =
        config.value(CONF_GEN_AUTOSAVE, state_.general.autosave).toBool();
    state_.general.saveinterval =
        config.value(CONF_GEN_SAVEINTERVAL, state_.general.saveinterval).toUInt();
    state_.general.autobackup =
        config.value(CONF_GEN_AUTOBACKUP, state_.general.autobackup).toBool();
    state_.general.loadlast =
        config.value(CONF_GEN_LOADLAST, state_.general.loadlast).toBool();
    state_.general.recentnum =
        config.value(CONF_GEN_RECENTNUM, state_.general.recentnum).toUInt();
    config.endGroup();

    // read recipe config
    config.beginGroup(CONFGROUP_RECIPE);
    state_.recipe.batch =
        config.value(CONF_RECIPE_BATCH, state_.recipe.batch).toDouble();
    state_.recipe.style =
        config.value(CONF_RECIPE_STYLE, state_.recipe.style).toString();
    // TODO: CONF_RECIPE_HOPFORM is deprecated (0.4.0)
    if (config.contains(CONF_RECIPE_HOPFORM)) {
        state_.recipe.hoptype =
            config.value(CONF_RECIPE_HOPFORM, state_.recipe.hoptype).toString();
        config.remove(CONF_RECIPE_HOPFORM);
    } else {
        state_.recipe.hoptype =
            config.value(CONF_RECIPE_HOPTYPE, state_.recipe.hoptype).toString();
    }
    config.endGroup();

    // read calc config
    config.beginGroup(CONFGROUP_CALC);
    state_.calc.steepyield =
        config.value(CONF_CALC_STEEPYIELD, state_.calc.steepyield).toDouble();
    state_.calc.efficiency =
        config.value(CONF_CALC_EFFICIENCY, state_.calc.efficiency).toDouble();
    state_.calc.morey =
        config.value(CONF_CALC_MOREY, state_.calc.morey).toBool();
    state_.calc.tinseth =
        config.value(CONF_CALC_TINSETH, state_.calc.tinseth).toBool();
    state_.calc.units =
        config.value(CONF_CALC_UNITS, state_.calc.units).toString();
    config.endGroup();

    config.endGroup();
}

//////////////////////////////////////////////////////////////////////////////
// writeConfig()
// -------------
// Write configuration to settings file

void QBrew::writeConfig()
{
    QSettings config;
    config.beginGroup(QString("/") + PACKAGE);

    // write general config
    config.beginGroup(CONFGROUP_GENERAL);
    config.setValue(CONF_GEN_LOOK_FEEL, state_.general.lookfeel);
    config.setValue(CONF_GEN_SHOW_SPLASH, state_.general.showsplash);
    config.setValue(CONF_GEN_AUTOSAVE, state_.general.autosave);
    config.setValue(CONF_GEN_SAVEINTERVAL, (int)state_.general.saveinterval);
    config.setValue(CONF_GEN_AUTOBACKUP, state_.general.autobackup);
    config.setValue(CONF_GEN_LOADLAST, state_.general.loadlast);
    config.setValue(CONF_GEN_RECENTNUM, (int)state_.general.recentnum);
    config.endGroup();

    // write recipe config
    config.beginGroup(CONFGROUP_RECIPE);
    config.setValue(CONF_RECIPE_BATCH, (double)state_.recipe.batch);
    config.setValue(CONF_RECIPE_STYLE, state_.recipe.style);
    config.setValue(CONF_RECIPE_HOPTYPE, state_.recipe.hoptype);
    config.endGroup();

    // write calc config
    config.beginGroup(CONFGROUP_CALC);
    config.setValue(CONF_CALC_STEEPYIELD, (double)state_.calc.steepyield);
    config.setValue(CONF_CALC_EFFICIENCY, (double)state_.calc.efficiency);
    config.setValue(CONF_CALC_MOREY, state_.calc.morey);
    config.setValue(CONF_CALC_TINSETH, state_.calc.tinseth);
    config.setValue(CONF_CALC_UNITS, state_.calc.units);
    config.endGroup();

    config.endGroup();
}

//////////////////////////////////////////////////////////////////////////////
// restoreState()
// ------------
// Restore application state

void QBrew::restoreState()
{
    QSettings config;
    config.beginGroup(QString("/") + PACKAGE);

    // read window state
    config.beginGroup(CONFGROUP_WINDOW);
    state_.window.statusbar = config.value(CONF_WIN_STATUSBAR,
                                           state_.window.statusbar).toBool();

    QByteArray mainwindow = config.value(CONF_WIN_MAINWINDOW).toByteArray();
    QMainWindow::restoreState(mainwindow);
    resize(config.value(CONF_WIN_SIZE, QSize(600, 400)).toSize());
    config.endGroup();

    // read general state
    config.beginGroup(CONFGROUP_GENERAL);
    state_.general.recentfiles = config.value(CONF_GEN_RECENTFILES).toStringList();
    while (state_.general.recentfiles.count() > state_.general.recentnum) {
        state_.general.recentfiles.removeLast();
    }

    config.endGroup();

    config.endGroup();

    // show or hide statusbar depending on initial setting
    statusBar()->setVisible(state_.window.statusbar);
    ui.actiontogglestatusbar->setChecked(state_.window.statusbar);
}

//////////////////////////////////////////////////////////////////////////////
// saveSate()
// ----------
// Save application state

void QBrew::saveState()
{
    // load config from settings
    QSettings config;
    config.beginGroup(QString("/") + PACKAGE);

    // write window state
    config.beginGroup(CONFGROUP_WINDOW);
    config.setValue(CONF_WIN_MAINWINDOW, QMainWindow::saveState());
    config.setValue(CONF_WIN_SIZE, size());
    config.setValue(CONF_WIN_STATUSBAR, state_.window.statusbar);

    config.endGroup();

    // write general state
    config.beginGroup(CONFGROUP_GENERAL);
    config.setValue(CONF_GEN_RECENTFILES, state_.general.recentfiles);
    config.endGroup();

    config.endGroup();
}

//////////////////////////////////////////////////////////////////////////////
// Miscellaneous                                                            //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// openFile()
// ----------
// Open the named file

bool QBrew::openFile(const QString &filename)
{

    if (!filename.isEmpty()) {
        if (recipe_->nativeFormat(filename)) {
            if (recipe_->loadRecipe(filename)) {
                filename_ = filename;
                setFileCaption(filename_);
            } else {
                // load was unsuccessful
                QMessageBox::warning(this, TITLE,
                                     tr("Unable to load the recipe %1")
                                     .arg(QFileInfo(filename_).fileName()));
                return false;
            }
        } else {
            bool status = false;
            // attempt to import file
            if (recipe_->beerXmlFormat(filename)) {
                status = recipe_->importBeerXml(filename);
                if (status) {
                    // set filename to the default (forces a saveas)
                    filename_ = DEFAULT_FILE;
                    setFileCaption(filename_);
                }
            }
            if (!status) {
                // import was unsuccessful
                QMessageBox::warning(this, TITLE,
                                     tr("Unable to import the file %1")
                                     .arg(QFileInfo(filename).fileName()));
                return false;
            }
        }

        newflag_ = false;
        backed_ = false;
        addRecent(filename_);
        ui.actionfilesave->setEnabled(false);

        return true;
    }
    // empty filename
    return false;
}

//////////////////////////////////////////////////////////////////////////////
// saveFile()
// ----------
// Save a file

bool QBrew::saveFile(const QString &filename)
{
    bool status = true;

    if (!filename.isEmpty()) {
        status = recipe_->saveRecipe(filename);

        if (status) {
            // remove old autosave file if present
            if (QFile::exists(autosavename_)) {
                QFile::remove(autosavename_);
                initAutoSave();
            }
        }
    }

    return status;
}

//////////////////////////////////////////////////////////////////////////////
// backupFile()
// ------------
// Backup the current file

bool QBrew::backupFile()
{
    if (backed_) return true;

    bool status = false;
    if (!filename_.isEmpty() && (filename_ != tr(DEFAULT_FILE))) {
        if (QFile::exists(filename_ + '~'))
            QFile::remove(filename_ + '~');
        status = QFile::copy(filename_, filename_ + '~');
        if (status) backed_ = true;
    }
    return status;
}

//////////////////////////////////////////////////////////////////////////////
// recentMenuShow()
// ----------------
// about to show recent file menu

void QBrew::recentMenuShow()
{
    ui.menuopenrecent->clear();

    QAction *action;
    foreach(QString recentfile, state_.general.recentfiles) {
        action = new QAction(recentfile, ui.menuopenrecent);
        connect(action, SIGNAL(triggered()),
                this, SLOT(fileRecent()));
	ui.menuopenrecent->addAction(action);
    }
}

//////////////////////////////////////////////////////////////////////////////
// addRecent()
// -----------
// Add a file to the recent files list

void QBrew::addRecent(const QString &filename)
{
    if (state_.general.recentnum == 0) return;

    QFileInfo finfo(filename);
    QString filepath = finfo.absoluteFilePath();

    if (state_.general.recentfiles.contains(filepath))
	state_.general.recentfiles.removeAll(filepath);
    if (state_.general.recentfiles.count() >= state_.general.recentnum)
	state_.general.recentfiles.removeLast();
    state_.general.recentfiles.prepend(filepath);
}

//////////////////////////////////////////////////////////////////////////////
// autoSave()
// ----------
// Time to autosave

void QBrew::autoSave()
{
    if (!recipe_->modified()) return;
    statusBar()->showMessage(tr("Autosaving recipe..."), 2000);
    if ((filename_.isEmpty()) || (filename_ == tr(DEFAULT_FILE))) {
        recipe_->saveRecipe(autosavename_);
    } else {
        if (state_.general.autobackup) backupFile();
        recipe_->saveRecipe(filename_);
    }
    // remove "modified" from caption
    setFileCaption(filename_);
}

//////////////////////////////////////////////////////////////////////////////
// setFileCaption()
// ----------------
// Set the window caption from file name

void QBrew::setFileCaption(const QString &filename)
{
    setWindowModified(false); // clear modified *

    QString shownname;
    if (filename.isEmpty()) shownname = tr(DEFAULT_FILE);
    else                    shownname = QFileInfo(filename).fileName();

    setWindowTitle(QString("%1 - %2[*]").arg(TITLE.constData()).arg(shownname));
}

//////////////////////////////////////////////////////////////////////////////
// querySave()
// -----------
// Ask the user if they want to save their work before going on

int QBrew::querySave()
{
    return QMessageBox::question(this, TITLE + tr(" - Save?"),
                                 tr("<p>Do you wish to save your work first?"),
                                 QMessageBox::Yes,
                                 QMessageBox::No,
                                 QMessageBox::Cancel);
}

//////////////////////////////////////////////////////////////////////////////
// queryOverwrite()
// -----------
// Ask the user if they want to overwrite an existing file

int QBrew::queryOverwrite(const QString filename)
{
    return QMessageBox::question(this, TITLE + tr(" - Overwrite?"),
                                 tr("<p>Are you sure you want to "
                                    "overwrite the file \"%1\"")
                                 .arg(filename),
                                 QMessageBox::Yes,
                                 QMessageBox::Cancel);
}

//////////////////////////////////////////////////////////////////////////////
// recipeModified()
// ------------------
// Received when recipe is modified

void QBrew::recipeModified()
{
    ui.actionfilesave->setEnabled(true);
    setWindowModified(true);
}

//////////////////////////////////////////////////////////////////////////////
// closeEvent()
// -----------
// Catch the close event

void QBrew::closeEvent(QCloseEvent* e)
{
    if (!recipe_->modified()) {
        e->accept();
    } else {
        // file needs to be saved, what do we do
        switch (querySave()) {
          case QMessageBox::Yes:
              fileSave();
              e->accept();
              break;
          case QMessageBox::Cancel:
              statusBar()->showMessage(tr(READY), 2000);
              e->ignore();
              break;
          case QMessageBox::No:
          default:
              e->accept();
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// eventFilter()
// -----------
// Filter events

bool QBrew::eventFilter(QObject *obj, QEvent *event)
{
    // necessary on Mac
    if (event->type() == QEvent::FileOpen) {
        QFileOpenEvent *openevent = static_cast<QFileOpenEvent *>(event);
        fileOpen(openevent->file());
        return true;
    } else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}

//////////////////////////////////////////////////////////////////////////////
// dataBase()
// ---------
// Figure out the base directory for the data

QString QBrew::dataBase()
{
    // TODO: can QDesktopServices help us here?
    QString base = qApp->applicationDirPath();

#if defined(Q_WS_X11)
    if (QRegExp("qbrew/?$").indexIn(base) != -1) {
        // we have our own application directory (it ends in 'qbrew')
        base += "/";
    } else if (QRegExp("bin/?$").indexIn(base) != -1) {
        // we are in the bin dir of a filesystem hiearchy like '/usr/local/bin'
        base += "/../share/qbrew/";
    } else {
        // otherwise punt
        base += "/";
    }

#elif defined(Q_WS_MAC)
    if (QRegExp("Contents/MacOS/?$").indexIn(base) != -1) {
        // we're pointing inside an application bundle
        base += "/../Resources/";
    } else {
        // otherwise punt
        base += "/";
    }

#else // Win32 and others
    base += "/";
#endif
    return QDir::cleanPath(base) + "/";
}

//////////////////////////////////////////////////////////////////////////////
// docBase()
// ---------
// Figure out the base directory for the documentation

QString QBrew::docBase()
{
    QString base = qApp->applicationDirPath();

#if defined(Q_WS_X11)
    if (QRegExp("qbrew/?$").indexIn(base) != -1) {
        // we have our own application directory (it ends in 'qbrew')
        base += "/doc/";
    } else if (QRegExp("bin/?$").indexIn(base) != -1) {
        // we are in the bin dir of a filesystem hiearchy like '/usr/local/bin'
        base += "/../share/doc/qbrew/";
    } else {
        // otherwise punt'
        base += "/doc/";
    }

#elif defined(Q_WS_MAC)
    if (QRegExp("Contents/MacOS/?$").indexIn(base) != -1) {
        // we're pointing inside an application bundle
        base += "/../Resources/en.lproj/";
    } else if (QFile::exists(base + "/en.lproj/")) {
        // some other hierarchy using Mac style l10n
        base += "/en.lproj/";
    } else {
        // otherwise punt
        base += "/doc/";
    }

#else // Win32 and others
    base += "/doc/";
#endif
    return QDir::cleanPath(base) + "/";
}
