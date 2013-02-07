/***************************************************************************
  ingredientview.cpp
  -------------------
  Ingredient view
  -------------------
  Copyright 2006-2008, David Johnson
  Please see the header file for copyright and license information
 ***************************************************************************/

#include <QAction>
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QMenu>

#include "ingredientview.h"

///////////////////////////////////////////////////////////////////////////////
// IngredientView                                                            //
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// IngredientView()
// ------------
// Constructor

IngredientView::IngredientView(QWidget *parent)
    : QTableView(parent), addaction_(0), removeaction_(0)
{
    setSortingEnabled(true);

    // create context menu actions
    addaction_ = new QAction(tr("&Add Ingredient"), this);
    addaction_->setShortcut(QKeySequence(tr("Ctrl+Insert")));
    addaction_->setStatusTip(tr("Add a new ingredient"));
    connect(addaction_, SIGNAL(triggered()), this, SLOT(addIngredient()));

    removeaction_ = new QAction(tr("&Remove Ingredient"), this);
    removeaction_->setShortcut(QKeySequence(tr("Ctrl+Delete")));
    removeaction_->setStatusTip(tr("Remove selected ingredient"));
    connect(removeaction_, SIGNAL(triggered()), this, SLOT(removeIngredient()));

    clearaction_ = new QAction(tr("&Clear Selection"), this);
    clearaction_->setShortcut(QKeySequence(tr("Escape")));
    clearaction_->setStatusTip(tr("Clear selection"));
    connect(clearaction_, SIGNAL(triggered()), this, SLOT(clearSelection()));

    addAction(addaction_);
    addAction(removeaction_);
    addAction(clearaction_);
}

IngredientView::~IngredientView()
{
}

//////////////////////////////////////////////////////////////////////////////
// contextMenuEvent()
// ------------------
// Create and show context menu

void IngredientView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(addaction_);
    menu.addAction(removeaction_);
    menu.addSeparator();
    menu.addAction(clearaction_);
    menu.exec(event->globalPos());
}

//////////////////////////////////////////////////////////////////////////////
// addIngredient()
// ---------------
// Add a new blank ingredient

void IngredientView::addIngredient()
{
    model()->insertRows(currentIndex().row(), 1, QModelIndex());
}

//////////////////////////////////////////////////////////////////////////////
// removeIngredient()
// ------------------
// Remove the selected ingredient

void IngredientView::removeIngredient()
{
    // can only remove valid indexes
    if (currentIndex().isValid()) {
        model()->removeRows(currentIndex().row(), 1, QModelIndex());
        clearSelection();
    }
}
