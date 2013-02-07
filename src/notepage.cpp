/***************************************************************************
  notepage.cpp
  -------------------
  A dialog page for notes
  -------------------
  Copyright 2003-2008, David Johnson
  Please see the header file for copyright and license information
 ***************************************************************************/

#include <QTextEdit>

#include "recipe.h"
#include "notepage.h"

//////////////////////////////////////////////////////////////////////////////
// Construction, Destruction                                                //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// NotePage()
// ----------
// Constructor

NotePage::NotePage(QWidget *parent, Recipe *recipe)
    : QWidget(parent), recipe_(recipe)
{
    ui.setupUi(this);

    // set up connections
    connect(ui.recipeedit, SIGNAL(textChanged()),
            this, SLOT(updateRecipeNotes()));
    connect(ui.batchedit, SIGNAL(textChanged()),
            this, SLOT(updateBatchNotes()));
}

NotePage::~NotePage() { ; }

//////////////////////////////////////////////////////////////////////////////
// refresh()
// ---------
// initialize

void NotePage::refresh()
{
    ui.recipeedit->blockSignals(true);
    ui.batchedit->blockSignals(true);
    ui.recipeedit->setPlainText(recipe_->recipeNotes());
    ui.batchedit->setPlainText(recipe_->batchNotes());
    ui.recipeedit->blockSignals(false);
    ui.batchedit->blockSignals(false);
}

//////////////////////////////////////////////////////////////////////////////
// updateRecipeNotes()
// -----------------
// Update the notes in the model

void NotePage::updateRecipeNotes()
{
    // TODO: this is very inefficient, particularly for large amounts of text
    // consider making character have a list of paragraphs, then only update
    // the paragraph that changed.
    recipe_->setRecipeNotes(ui.recipeedit->toPlainText());
}

//////////////////////////////////////////////////////////////////////////////
// updateBatchNotes()
// -----------------
// Update the notes in the model

void NotePage::updateBatchNotes()
{
    // TODO: this is very inefficient, particularly for large amounts of text
    // consider making character have a list of paragraphs, then only update
    // the paragraph that changed.
    recipe_->setBatchNotes(ui.batchedit->toPlainText());
}

