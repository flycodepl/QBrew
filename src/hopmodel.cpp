/***************************************************************************
  hopmodel.cpp
  -------------------
  Hop model
  -------------------
  Copyright 2006-2008, David Johnson
  Please see the header file for copyright and license information
 ***************************************************************************/

#include <QApplication>
#include <QMessageBox>
#include <QPair>

#include "data.h"
#include "hopmodel.h"

using namespace Resource;

//////////////////////////////////////////////////////////////////////////////
// HopModel()
// ------------
// Constructor

HopModel::HopModel(QObject *parent, HopList *list)
    : QAbstractTableModel(parent), list_(list)
{}

HopModel::~HopModel() {}

//////////////////////////////////////////////////////////////////////////////
// flush()
// -------
// Reset the model

void HopModel::flush()
{
    reset();
}

//////////////////////////////////////////////////////////////////////////////
// data()
// ------
// Return data at index

QVariant HopModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    if (index.row() >= list_->count()) return QVariant();

    // row is the entry in the QList
    const Hop &hop = list_->at(index.row());

    // column is the hop "field"
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
          case NAME:
              return hop.name();
          case WEIGHT:
              return hop.weight().toString(3);
          case ALPHA:
              return QString::number(hop.alpha(), 'f', 1) + '%';
          case TIME:
              return QString::number(hop.time()) + tr(" min", "minutes");
          case TYPE:
              return hop.type();
          default:
              return QVariant();
        }
    } else if (role == Qt::EditRole) {
        switch (index.column()) {
          case NAME:
              return hop.name();
          case WEIGHT: {
              // return converted quantity
              Weight weight = hop.weight();
              weight.convert(Data::instance()->defaultHopUnit());
              return weight.amount();
          }
          case ALPHA:
              return hop.alpha();
          case TIME:
              return hop.time();
          case TYPE:
              return hop.type();
          default:
              return QVariant();
        }
    } else if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
          case NAME:
          case TYPE:
              return Qt::AlignLeft;
          case WEIGHT:
          case ALPHA:
          case TIME:
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

bool HopModel::setData(const QModelIndex &index,
                       const QVariant &value, int role)
{
    static bool deleting = false;

    Hop hop;
    QString name;
    int row = index.row();
    int column = index.column();

    if (!index.isValid()) return false;
    if (role != Qt::EditRole) return false;
    if (row >= list_->count()) return false;

    // grab existing hop
    if (row < list_->count()) hop = list_->value(row);

    switch (column) {
      case NAME:
          // editing name as several special cases
          name = value.toString();

          // deleting name deletes hop
          if (name.isEmpty()) {
              if (row >= list_->count()) return false; // already blank

              // TODO: for some reason this gets entered recursively...
              if (deleting) return false;
              deleting = true;

              int status = QMessageBox::question(QApplication::activeWindow(),
                               TITLE + tr(" - Delete?"),
                               tr("Do you wish to remove this entry?"),
                               QMessageBox::Yes | QMessageBox::Cancel);
              if (status == QMessageBox::Yes) {
                  // remove hop
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
          hop.setName(name);
          if (Data::instance()->hasHop(name)) {
              Hop newhop = Data::instance()->hop(name);
              // we don't override weight or time
              hop.setType(newhop.type());
              hop.setAlpha(newhop.alpha());
          }
          break;

      case WEIGHT:
          hop.setWeight(Weight(value.toDouble(),
                               Data::instance()->defaultHopUnit()));
          break;

      case ALPHA:
          hop.setAlpha(value.toDouble());
          break;

      case TIME:
          hop.setTime(value.toUInt());
          break;

      case TYPE:
          hop.setType(value.toString());
          break;

      default:
          return false;
    }

    list_->replace(row, hop);
    emit modified();

    // whole row may have changed
    emit dataChanged(index.sibling(row, NAME),
                     index.sibling(row, TIME));

    return true;
}

//////////////////////////////////////////////////////////////////////////////
// insertRows()
// ------------
// Insert rows into table

bool HopModel::insertRows(int row, int count, const QModelIndex&)
{
    if (count != 1) return false; // only insert one row at a time
    if ((row < 0) || (row >= list_->count())) row = list_->count();

    Hop hop = Data::instance()->hop(tr("Generic"));

    beginInsertRows(QModelIndex(), row, row);
    list_->insert(row, hop);
    emit modified();
    endInsertRows();
    return true;
}

//////////////////////////////////////////////////////////////////////////////
// removeRows()
// ------------
// Remove rows from table

bool HopModel::removeRows(int row, int count, const QModelIndex&)
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

QVariant HopModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
          case NAME:
              return tr("Hop");
          case WEIGHT:
              return tr("Weight");
          case ALPHA:
              return tr("Alpha");
          case TIME:
              return tr("Time");
          case TYPE:
              return tr("Type");
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

Qt::ItemFlags HopModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

//////////////////////////////////////////////////////////////////////////////
// rowCount()
// ----------
// Return number of rows of data

int HopModel::rowCount(const QModelIndex &) const
{
    return list_->count();
}

//////////////////////////////////////////////////////////////////////////////
// columnCount()
// -------------
// Return number of columns of data

int HopModel::columnCount(const QModelIndex &) const
{
    return COUNT;
}

//////////////////////////////////////////////////////////////////////////////
// sort()
// ------
// Sort by column

void HopModel::sort(int column, Qt::SortOrder order)
{
    QList<QPair<QString,Hop> > sortlist;

    foreach(Hop hop, *list_) {
        QString field;
        switch (column) {
          case NAME:
              field = hop.name();
              break;
          case WEIGHT:
              field = QString::number(hop.weight().amount()).rightJustified(8,'0');
              break;
          case ALPHA:
              field = QString::number(hop.alpha(),'f',1).rightJustified(8,'0');
              break;
          case TYPE:
              field = hop.type();
              break;
          case TIME:
          default:
              field = QString::number(hop.time()).rightJustified(8,'0');
              break;
        }
        sortlist.append(QPair<QString,Hop>(field, hop));
    }

    // sort list
    qSort(sortlist.begin(), sortlist.end());

    emit layoutAboutToBeChanged();

    // create new list
    list_->clear();
    QPair<QString,Hop> pair;
    foreach(pair, sortlist) {
        if (order == Qt::AscendingOrder)
            list_->append(pair.second);
        else
            list_->prepend(pair.second);
    }

    emit layoutChanged();
}
