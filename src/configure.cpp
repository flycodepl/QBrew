/***************************************************************************
  configure.cpp
  -------------------
  Application configuration dialog
  -------------------
  Copyright 2006-2008 David Johnson
  Please see the header file for copyright and licehnse information
 ***************************************************************************/

#include <QDialogButtonBox>
#include <QStyleFactory>

#include "configure.h"
#include "data.h"
#include "quantity.h"
#include "resource.h"

using namespace Resource;

enum {
    GENERAL_PAGE = 0,
    CALC_PAGE,
    RECIPE_PAGE
};

//////////////////////////////////////////////////////////////////////////////
// Construction, Destruction                                                //
//////////////////////////////////////////////////////////////////////////////

Configure::Configure(QWidget* parent)
    : QDialog(parent), calc_(0), general_(0), recipe_(0)
{
    setWindowTitle(TITLE + tr(" - Configure"));

    // tab widget
    tabwidget_ = new QTabWidget(this);

    // buttons
    buttonbox_ = new QDialogButtonBox(Qt::Horizontal, this);
    buttonbox_->addButton(QDialogButtonBox::Ok);
    buttonbox_->addButton(QDialogButtonBox::Cancel);
    buttonbox_->addButton(QDialogButtonBox::Apply);
    buttonbox_->addButton(QDialogButtonBox::Reset);

    connect(buttonbox_, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonbox_, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttonbox_, SIGNAL(clicked(QAbstractButton*)),
            this, SLOT(slotClicked(QAbstractButton*)));

    // pages
    QWidget *page = new QWidget();
    general_ = new Ui::GeneralConfig();
    general_->setupUi(page);
    tabwidget_->insertTab(GENERAL_PAGE, page, tr("&General"));
    page = new QWidget();
    calc_ = new Ui::CalcConfig();
    calc_->setupUi(page);
    tabwidget_->insertTab(CALC_PAGE, page, tr("&Calculations"));
    page = new QWidget();
    recipe_ = new Ui::RecipeConfig();
    recipe_->setupUi(page);
    tabwidget_->insertTab(RECIPE_PAGE, page, tr("&Recipe"));

    // additional setup
    recipe_->batch->
        setSuffix(" " + Data::instance()->defaultSize().unit().symbol());
    connect(calc_->units, SIGNAL(activated(const QString&)),
            this, SLOT(convertBatchSpin(const QString&)));

    QVBoxLayout *mainlayout = new QVBoxLayout();
    mainlayout->addWidget(tabwidget_);
    mainlayout->addWidget(buttonbox_);
    setLayout(mainlayout);
}

Configure::~Configure() { ; }

//////////////////////////////////////////////////////////////////////////////
// Miscellaneous                                                            //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// setState()
// ----------
// Read in the config to get the states

void Configure::setState(const ConfigState &state)
{
    state_ = state;
    setGeneralState(state_.general);
    setCalcState(state_.calc);
    setRecipeState(state_.recipe);
}

//////////////////////////////////////////////////////////////////////////////
// setGeneralState()
// -----------------
// Set the state for the general page

void Configure::setGeneralState(const GenConfigState &state)
{
    if (general_) {
        general_->lookfeel->clear();
        general_->lookfeel->addItem(QString());
        general_->lookfeel->addItems(QStyleFactory::keys());
        general_->showsplash->setChecked(state.showsplash);
        general_->autosave->setChecked(state.autosave);
        general_->saveinterval->setValue(state.saveinterval);
        general_->autobackup->setChecked(state.autobackup);
        general_->loadlast->setChecked(state.loadlast);
        general_->recentnum->setValue(state.recentnum);

        // set combo for lookfeel
        int index;
        if (state.lookfeel.isEmpty()) {
            index = general_->lookfeel->findText(QApplication::style()->objectName(),
                                                 Qt::MatchExactly);
        } else {
            index = general_->lookfeel->findText(state.lookfeel,
                                                 Qt::MatchExactly);
        }
        general_->lookfeel->setCurrentIndex(index);
    }
}

//////////////////////////////////////////////////////////////////////////////
// setCalcState()
// -----------------
// Set the state for the calculation page

void Configure::setCalcState(const CalcConfigState &state)
{
    if (calc_) {
        calc_->steepyield->setValue(state.steepyield);
        calc_->efficiency->setValue(state.efficiency);
        calc_->morey->setChecked(state.morey);
        calc_->tinseth->setChecked(state.tinseth);
        QStringList list = (QStringList() << UNIT_METRIC << UNIT_US);
        calc_->units->clear();
        calc_->units->addItems(list);

        // set combo for units string
        int index = calc_->units->findText(state.units, Qt::MatchExactly);
        if (index >= 0) {
            calc_->units->setCurrentIndex(index);
        } else {
            calc_->units->addItem(state.units);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// setRecipeState()
// -----------------
// Set the state for the recipe page

void Configure::setRecipeState(const RecipeConfigState &state)
{
    if (recipe_) {
        recipe_->batch->setValue(state.batch);
        recipe_->stylebox->clear();
        recipe_->stylebox->addItems(Data::instance()->stylesList());
        recipe_->hoptype->clear();
        recipe_->hoptype->addItems(Hop::typeStringList());

        // set combo for style string
        int index = recipe_->stylebox->findText(state.style, Qt::MatchExactly);
        if (index >= 0) {
            recipe_->stylebox->setCurrentIndex(index);
        } else {
            recipe_->stylebox->addItem(state.style);
        }
        // set combo for hoptype string
        index =  recipe_->hoptype->findText(state.hoptype, Qt::MatchExactly);
        if (index >= 0) {
            recipe_->hoptype->setCurrentIndex(index);
        } else {
            recipe_->hoptype->addItem(state.hoptype);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
// Slots                                                                    //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// clicked()
// ---------
// QDialogButtonBox clicked

void Configure::slotClicked(QAbstractButton *button)
{
    switch (buttonbox_->buttonRole(button)) {
      case QDialogButtonBox::ApplyRole:
          apply();
          break;
      case QDialogButtonBox::ResetRole:
          defaults();
          break;
      default:
          break;
    }
}

//////////////////////////////////////////////////////////////////////////////
// accept()
// --------
// OK button was pressed

void Configure::accept()
{
    apply();
    QDialog::accept();
    emit configureAccept();
}

//////////////////////////////////////////////////////////////////////////////
// defauls()
// ---------
// Set dialog to default values

void Configure::defaults()
{
    // only default the current page
    switch (tabwidget_->currentIndex()) {
      case GENERAL_PAGE:
          setGeneralState(GenConfigState()); break;
      case CALC_PAGE:
          setCalcState(CalcConfigState()); break;
      case RECIPE_PAGE:
          setRecipeState(RecipeConfigState()); break;
    }
}

//////////////////////////////////////////////////////////////////////////////
// apply()
// -------
// Apply values

void Configure::apply()
{
    if (general_) {
        // general page
        state_.general.lookfeel = general_->lookfeel->currentText();
        state_.general.showsplash = general_->showsplash->isChecked();
        state_.general.autosave = general_->autosave->isChecked();
        state_.general.saveinterval = general_->saveinterval->value();
        state_.general.autobackup = general_->autobackup->isChecked();
        state_.general.loadlast = general_->loadlast->isChecked();
        state_.general.recentnum = general_->recentnum->value();
        emit generalApply(state_.general);
    }

    if (calc_) {
        // calc page
        state_.calc.steepyield = calc_->steepyield->value();
        state_.calc.efficiency = calc_->efficiency->value();
        state_.calc.morey = calc_->morey->isChecked();
        state_.calc.tinseth = calc_->tinseth->isChecked();
        state_.calc.units = calc_->units->currentText();
        emit calcApply(state_.calc);
    }

    if (recipe_) {
        // recipe page
        state_.recipe.batch = recipe_->batch->value();
        state_.recipe.style = recipe_->stylebox->currentText();
        state_.recipe.hoptype = recipe_->hoptype->currentText();
        emit recipeApply(state_.recipe);
    }
}

//////////////////////////////////////////////////////////////////////////////
// convertBatchSpin()
// ------------------
// Set the appropriate suffice for the batch spinbox

void Configure::convertBatchSpin(const QString &selection)
{
    if (selection == Resource::UNIT_US)
        recipe_->batch->setSuffix(" " + Volume::gallon.symbol());
    else if (selection == Resource::UNIT_METRIC)
        recipe_->batch->setSuffix(" " + Volume::liter.symbol());
    else
        recipe_->batch->setSuffix("");
}
