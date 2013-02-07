/***************************************************************************
  hopdelegate.cpp
  -------------------
  Hop delegate editor
  -------------------
  Copyright 2006-2008, David Johnson
  Please see the header file for copyright and license information
 ***************************************************************************/

#include <QAbstractItemModel>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>

#include "data.h"
#include "hop.h"
#include "hopdelegate.h"
#include "hopmodel.h"

HopDelegate::HopDelegate(QObject *parent) : QItemDelegate(parent) {}

HopDelegate::~HopDelegate() {}

QWidget *HopDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &/*option*/,
                                     const QModelIndex &index) const
{
    QComboBox *combo;
    QSpinBox *spin;
    QDoubleSpinBox *dspin;
    QString suffix;
    // can only edit name on blank row
    bool blank = index.row() >= index.model()->rowCount();


    // different kind of editor for each column
    switch (index.column()) {
      case HopModel::NAME:
          combo = new QComboBox(parent);
          combo->setEditable(true);
          combo->addItem(QString());
          combo->addItems(Data::instance()->hopsList());
          combo->installEventFilter(const_cast<HopDelegate*>(this));
          return combo;

      case HopModel::WEIGHT:
          if (blank) return 0;
          dspin = new QDoubleSpinBox(parent);
          dspin->setDecimals(3);
          dspin->setRange(0.00, 1000.00);
          dspin->setSingleStep(0.25);
          suffix = " " + Data::instance()->defaultHopUnit().symbol();
          dspin->setSuffix(suffix);
          dspin->setAccelerated(true);
          dspin->installEventFilter(const_cast<HopDelegate*>(this));
          return dspin;

      case HopModel::ALPHA:
          if (blank) return 0;
          dspin = new QDoubleSpinBox(parent);
          dspin->setDecimals(1);
          dspin->setRange(0.0, 50.0);
          dspin->setSingleStep(0.1);
          dspin->setSuffix("%");
          dspin->setAccelerated(true);
          dspin->installEventFilter(const_cast<HopDelegate*>(this));
          return dspin;

      case HopModel::TIME:
          if (blank) return 0;
          spin = new QSpinBox(parent);
          spin->setRange(0, 120);
          spin->setSingleStep(5);
          spin->setSuffix(tr(" min", "minutes"));
          spin->setAccelerated(true);
          spin->installEventFilter(const_cast<HopDelegate*>(this));
          return spin;

      case HopModel::TYPE:
          if (blank) return 0;
          combo = new QComboBox(parent);
          combo->setEditable(true);
          combo->addItems(Hop::typeStringList());
          combo->installEventFilter(const_cast<HopDelegate*>(this));
          return combo;

      default:
          return 0;
    }
}

void HopDelegate::setEditorData(QWidget *editor,
                                const QModelIndex &index) const
{
    QComboBox *combo;
    QSpinBox *spin;
    QDoubleSpinBox *dspin;
    int comboindex;

    QVariant value = index.model()->data(index, Qt::EditRole);

    // different kind of editor for each column
    switch (index.column()) {
      case HopModel::NAME:
          combo = static_cast<QComboBox*>(editor);
          if (!combo) return;
          comboindex = combo->findText(value.toString());
          if (comboindex > 0) {
              combo->setCurrentIndex(comboindex);
          } else {
              combo->setEditText(value.toString());
          }
          break;

      case HopModel::WEIGHT:
      case HopModel::ALPHA:
          dspin = static_cast<QDoubleSpinBox*>(editor);
          if (!dspin) return;
          dspin->setValue(value.toDouble());
          break;

      case HopModel::TIME:
          spin = static_cast<QSpinBox*>(editor);
          if (!spin) return;
          spin->setValue(value.toUInt());
          break;

      case HopModel::TYPE:
          combo = static_cast<QComboBox*>(editor);
          if (!combo) return;
          comboindex = combo->findText(value.toString());
          if (comboindex > 0) {
              combo->setCurrentIndex(comboindex);
          } else {
              combo->setEditText(value.toString());
          }
          break;

      default:
          QItemDelegate::setEditorData(editor, index);
          break;
    }
}

void HopDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const
{
    QComboBox *combo;
    QSpinBox *spin;
    QDoubleSpinBox *dspin;
    QVariant value;

    // different kind of editor for each column
    switch (index.column()) {
      case HopModel::NAME:
      case HopModel::TYPE:
          combo = static_cast<QComboBox*>(editor);
          if (!combo) return;
          value = combo->currentText();
          model->setData(index, value);
          break;

      case HopModel::WEIGHT:
      case HopModel::ALPHA:
          dspin = static_cast<QDoubleSpinBox*>(editor);
          if (!dspin) return;
          value = dspin->value();
          model->setData(index, value);
          break;

      case HopModel::TIME:
          spin = static_cast<QSpinBox*>(editor);
          if (!spin) return;
          value = spin->value();
          model->setData(index, value);
          break;

      default:
          QItemDelegate::setModelData(editor, model,index);
          break;
    }
}

void HopDelegate::updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}
