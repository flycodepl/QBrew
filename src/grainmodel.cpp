/***************************************************************************
  grainmodel.cpp
  -------------------
  Grain model
  -------------------
  Copyright 2006-2008, David Johnson
  Please see the header file for copyright and license information
 ***************************************************************************/

#include <QApplication>
#include <QMessageBox>
#include <QPair>

#include "data.h"
#include "recipe.h"
#include "resource.h"
#include "grainmodel.h"

using namespace Resource;

//////////////////////////////////////////////////////////////////////////////
// GrainModel()
// ------------
// Constructor

// TODO: add "unique" flag, disallowing  duplicates (for DatabaseTool)

GrainModel::GrainModel(QObject *parent, GrainList *list)
    : QAbstractTableModel(parent), list_(list)
{}

GrainModel::~GrainModel(){}

//////////////////////////////////////////////////////////////////////////////
// flush()
// -------
// Reset the model

void GrainModel::flush()
{
    reset();
}

//////////////////////////////////////////////////////////////////////////////
// data()
// ------
// Return data at index

QVariant GrainModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    if (index.row() >= list_->count()) return QVariant();

    // row is the entry in the QList
    const Grain &grain = list_->at(index.row());

    // column is the grain "field"
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
          case NAME:
              return grain.name();
          case WEIGHT:
              return grain.weight().toString(3);
          case EXTRACT:
              return QString::number(grain.extract(), 'f', 3);
          case COLOR:
              return QString::number(grain.color(), 'f', 1) + Resource::DEGREE;
          case TYPE:
              return grain.type();
          case USE:
              return grain.use();
          default:
              return QVariant();
        }
    } else if (role == Qt::EditRole) {
        switch (index.column()) {
          case NAME:
              return grain.name();
          case WEIGHT: {
              // return converted quantity
              Weight weight = grain.weight();
              weight.convert(Data::instance()->defaultGrainUnit());
              return weight.amount();
          }
          case EXTRACT:
              return grain.extract();
          case COLOR:
              return grain.color();
          case TYPE:
              return grain.type();
          case USE:
              return grain.use();
          default:
              return QVariant();
        }
    } else if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
          case NAME:
          case TYPE:
          case USE:
              return Qt::AlignLeft;
          case WEIGHT:
          case EXTRACT:
          case COLOR:
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

bool GrainModel::setData(const QModelIndex &index,
                       const QVariant &value, int role)
{
    static bool deleting = false;

    Grain grain;
    QString name;
    int row = index.row();
    int column = index.column();

    if (!index.isValid()) return false;
    if (role != Qt::EditRole) return false;
    if (row >= list_->count()) return false;

    // grab existing grain
    grain = list_->value(row);

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
                  // remove grain
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
          grain.setName(name);
          if (Data::instance()->hasGrain(name)) {
              Grain newgrain = Data::instance()->grain(name);
              // we don't override weight
              grain.setExtract(newgrain.extract());
              grain.setColor(newgrain.color());
              grain.setType(newgrain.type());
              grain.setUse(newgrain.use());
          }
          break;

      case WEIGHT:
          grain.setWeight(Weight(value.toDouble(),
                                 Data::instance()->defaultGrainUnit()));
          break;

      case EXTRACT:
          grain.setExtract(value.toDouble());
          break;

      case COLOR:
          grain.setColor(value.toDouble());
          break;

      case TYPE:
          grain.setType(value.toString());
          break;

      case USE:
          grain.setUse(value.toString());
          break;

      default:
          return false;
    }

    list_->replace(row, grain);
    emit modified();

    // whole row may have changed
    emit dataChanged(index.sibling(row, NAME),
                     index.sibling(row, USE));

    return true;
}

//////////////////////////////////////////////////////////////////////////////
// insertRows()
// ------------
// Insert rows into table

bool GrainModel::insertRows(int row, int count, const QModelIndex&)
{
    if (count != 1) return false; // only insert one row at a time
    if ((row < 0) || (row >= list_->count())) row = list_->count();

    Grain grain = Data::instance()->grain(tr("Generic"));

    beginInsertRows(QModelIndex(), row, row);
    list_->insert(row, grain);
    emit modified();
    endInsertRows();
    return true;
}

//////////////////////////////////////////////////////////////////////////////
// removeRows()
// ------------
// Remove rows from table

bool GrainModel::removeRows(int row, int count, const QModelIndex&)
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

QVariant GrainModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
          case NAME:
              return tr("Grain");
          case WEIGHT:
              return tr("Weight");
          case EXTRACT:
              return tr("Extract");
          case COLOR:
              return tr("Color");
          case TYPE:
              return tr("Type");
          case USE:
              return tr("Use");
          default:
              return QVariant();
        }
    }

    return QVariant();
}

//////////////////////////////////////////////////////////////////////////////
// flags()
// ------
// Return flags at index

Qt::ItemFlags GrainModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

//////////////////////////////////////////////////////////////////////////////
// rowCount()
// ----------
// Return number of rows of data

int GrainModel::rowCount(const QModelIndex &) const
{
    return list_->count();
}

//////////////////////////////////////////////////////////////////////////////
// columnCount()
// -------------
// Return number of columns of data

int GrainModel::columnCount(const QModelIndex &) const
{
    return COUNT;
}

//////////////////////////////////////////////////////////////////////////////
// sort()
// ------
// Sort by column

void GrainModel::sort(int column, Qt::SortOrder order)
{
    QList<QPair<QString,Grain> > sortlist;

    foreach(Grain grain, *list_) {
        QString field;
        switch (column) {
          case NAME:
              field = grain.name();
              break;
          case WEIGHT:
              field = QString::number(grain.weight().amount()).rightJustified(8,'0');
              break;
          case EXTRACT:
              field = QString::number(grain.extract(),'f',3).rightJustified(8,'0');
              break;
          case COLOR:
              field = QString::number(grain.color(),'f',1).rightJustified(8,'0');
              break;
          case TYPE:
              field = grain.type();
              break;
          case USE:
          default:
              field = grain.use();
              break;
        }
        sortlist.append(QPair<QString,Grain>(field, grain));
    }

    // sort list
    qSort(sortlist.begin(), sortlist.end());

    emit layoutAboutToBeChanged();

    // create new list
    list_->clear();
    QPair<QString,Grain> pair;
    foreach(pair, sortlist) {
        if (order == Qt::AscendingOrder)
            list_->append(pair.second);
        else
            list_->prepend(pair.second);
    }

    emit layoutChanged();
}
