/***************************************************************************
  miscmodel.cpp
  -------------------
  Misc model
  -------------------
  Copyright 2006-2008, David Johnson
  Please see the header file for copyright and license information
 ***************************************************************************/

#include <QApplication>
#include <QMessageBox>
#include <QPair>

#include "data.h"
#include "miscmodel.h"

using namespace Resource;

//////////////////////////////////////////////////////////////////////////////
// MiscModel()
// ------------
// Constructor

MiscModel::MiscModel(QObject *parent, MiscList *list)
    : QAbstractTableModel(parent), list_(list)
{}

MiscModel::~MiscModel() {}

//////////////////////////////////////////////////////////////////////////////
// flush()
// -------
// Reset the model

void MiscModel::flush()
{
    reset();
}

//////////////////////////////////////////////////////////////////////////////
// data()
// ------
// Return data at index

QVariant MiscModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    if (index.row() >= list_->count()) return QVariant();

    // row is the entry in the QList
    const Misc &misc = list_->at(index.row());

    // column is the ingredient "field"
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
          case NAME:
              return misc.name();
          case QUANTITY:
              return misc.quantity().toString(3);
          case TYPE:
              return misc.type();
          case NOTES:
              return misc.notes();
          default:
              return QVariant();
        }
    } else if (role == Qt::EditRole) {
        switch (index.column()) {
          case NAME:
              return misc.name();
          case QUANTITY: {
              // return converted quantity
              Quantity quantity = misc.quantity();
              quantity.convert(Data::instance()->defaultMiscUnit());
              return quantity.amount();
          }
          case TYPE:
              return misc.type();
          case NOTES:
              return misc.notes();
          default:
              return QVariant();
        }
    } else if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
          case NAME:
          case TYPE:
          case NOTES:
              return Qt::AlignLeft;
          case QUANTITY:
          default:
              return Qt::AlignRight;
        }
    } else {
        return QVariant();
    }
}

//////////////////////////////////////////////////////////////////////////////
// setData()
// ---------
// Set data at index

bool MiscModel::setData(const QModelIndex &index,
                        const QVariant &value, int role)
{
    static bool deleting = false;

    Misc misc;
    QString name;
    int row = index.row();
    int column = index.column();

    if (!index.isValid()) return false;
    if (role != Qt::EditRole) return false;
    if (row >= list_->count()) return false;

    // grab existing misc
    if (row < list_->count()) misc = list_->value(row);

    switch (column) {
      case NAME:
          // editing name as several special cases
          name = value.toString();

          // deleting name deletes ingredient
          if (name.isEmpty()) {
              if (row >= list_->count()) return false; // empty

              // TODO: for some reason this gets entered recursively...
              if (deleting) return false;
              deleting = true;

              int status = QMessageBox::question(QApplication::activeWindow(),
                               TITLE + tr(" - Delete?"),
                               tr("Do you wish to remove this entry?"),
                               QMessageBox::Yes | QMessageBox::Cancel);
              if (status == QMessageBox::Yes) {
                  // remove misc
                  beginRemoveRows(index.parent(), row, row);

                  list_->removeAt(row);
                  emit modified();
                  endRemoveRows();

                  deleting = false;
                  return true;
              } else {
                  // ignore
                  deleting = false;
                  return false;
              }
          }

          // no change, nothing to do
          if (name == list_->at(row).name()) {
              return false;
          }

          // changed name
          misc.setName(name);
          if (Data::instance()->hasMisc(name)) {
              Misc newmisc = Data::instance()->misc(name);
              // we don't override weight
              misc.setType(newmisc.type());
              misc.setNotes(newmisc.notes());
          }
          break;

      case QUANTITY:
          misc.setQuantity(Quantity(value.toDouble(),
                                    Data::instance()->defaultMiscUnit()));
          break;

      case TYPE:
          misc.setType(value.toString());
          break;

      case NOTES:
          misc.setNotes(value.toString());
          break;

      default:
          return false;
    }

    list_->replace(row, misc);
    emit modified();

    // whole row may have changed
    emit dataChanged(index.sibling(row, NAME),
                     index.sibling(row, NOTES));

    return true;
}

//////////////////////////////////////////////////////////////////////////////
// insertRows()
// ------------
// Insert rows into table

bool MiscModel::insertRows(int row, int count, const QModelIndex&)
{
    if (count != 1) return false; // only insert one row at a time
    if ((row < 0) || (row >= list_->count())) row = list_->count();

    Misc misc = Data::instance()->misc(tr("Generic"));

    beginInsertRows(QModelIndex(), row, row);
    list_->insert(row, misc);
    emit modified();
    endInsertRows();
    return true;
}

//////////////////////////////////////////////////////////////////////////////
// removeRows()
// ------------
// Remove rows from table

bool MiscModel::removeRows(int row, int count, const QModelIndex&)
{
    if (count != 1) return false; // only remove one row at a time
    if ((row < 0) || (row >= list_->count())) return false;

    int status = QMessageBox::question(QApplication::activeWindow(),
                               TITLE + tr(" - Delete?"),
                               tr("Do you wish to remove this entry?"),
                               QMessageBox::Yes | QMessageBox::Cancel);
    if (status == QMessageBox::Cancel) {
        return false;
    }

    beginRemoveRows(QModelIndex(), row, row);
    list_->removeAt(row);
    emit modified();
    endRemoveRows();
    return true;
}

//////////////////////////////////////////////////////////////////////////////
// headerData()
// ------------
// Return header information

QVariant MiscModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
          case NAME:
              return tr("Misc");
          case QUANTITY:
              return tr("Quantity");
          case TYPE:
              return tr("Type");
          case NOTES:
              return tr("Notes");
          default:
              return QVariant();
        }
    }

    return QVariant();
}

//////////////////////////////////////////////////////////////////////////////
// flags()
// -------
// Return flags at index

Qt::ItemFlags MiscModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

//////////////////////////////////////////////////////////////////////////////
// rowCount()
// ----------
// Return number of rows of data

int MiscModel::rowCount(const QModelIndex &) const
{
    return list_->count();
}

//////////////////////////////////////////////////////////////////////////////
// columnCount()
// -------------
// Return number of columns of data

int MiscModel::columnCount(const QModelIndex &) const
{
    return COUNT;
}

//////////////////////////////////////////////////////////////////////////////
// sort()
// ------
// Sort by column

void MiscModel::sort(int column, Qt::SortOrder order)
{
    QList<QPair<QString,Misc> > sortlist;

    foreach(Misc misc, *list_) {
        QString field;
        switch (column) {
          case NAME:
              field = misc.name();
              break;
          case QUANTITY:
              field = QString::number(misc.quantity().amount()).rightJustified(8,'0');
              break;
          case TYPE:
              field = misc.type();
              break;
          case NOTES:
          default:
              field = misc.notes();
              break;
        }
        sortlist.append(QPair<QString,Misc>(field, misc));
    }

    // sort list
    qSort(sortlist.begin(), sortlist.end());

    emit layoutAboutToBeChanged();

    // create new list
    list_->clear();
    QPair<QString,Misc> pair;
    foreach(pair, sortlist) {
        if (order == Qt::AscendingOrder)
            list_->append(pair.second);
        else
            list_->prepend(pair.second);
    }

    emit layoutChanged();
}
