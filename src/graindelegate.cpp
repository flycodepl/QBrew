/***************************************************************************
  graindelegate.cpp
  -------------------
  Grain delegate editor
  -------------------
  Copyright 2006-2008, David Johnson
  Please see the header file for copyright and license information
 ***************************************************************************/

#include <QAbstractItemModel>
#include <QComboBox>
#include <QDoubleSpinBox>

#include "data.h"
#include "grain.h"
#include "graindelegate.h"
#include "grainmodel.h"

GrainDelegate::GrainDelegate(QObject *parent) : QItemDelegate(parent) {}

GrainDelegate::~GrainDelegate() {}

QWidget *GrainDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &/*option*/,
                                     const QModelIndex &index) const
{
    QComboBox *combo;
    QDoubleSpinBox *spin;
    QString suffix;

    // can only edit name on blank row
    if (index.row() >= index.model()->rowCount()) return 0;

    // different kind of editor for each column
    switch (index.column()) {
      case GrainModel::NAME:
          combo = new QComboBox(parent);
          combo->setEditable(true);
          combo->addItem(QString());
          combo->addItems(Data::instance()->grainsList());
          combo->installEventFilter(const_cast<GrainDelegate*>(this));
          return combo;

      case GrainModel::WEIGHT:
          spin = new QDoubleSpinBox(parent);
          spin->setDecimals(3);
          spin->setRange(0.00, 1000.00);
          spin->setSingleStep(0.25);
          spin->setAccelerated(true);
          suffix = " " + Data::instance()->defaultGrainUnit().symbol();
          spin->setSuffix(suffix);
          spin->installEventFilter(const_cast<GrainDelegate*>(this));
          return spin;

      case GrainModel::EXTRACT:
          spin = new QDoubleSpinBox(parent);
          spin->setDecimals(3);
          spin->setRange(1.000, 1.100);
          spin->setSingleStep(0.001);
          spin->setAccelerated(true);
          spin->installEventFilter(const_cast<GrainDelegate*>(this));
          return spin;

      case GrainModel::COLOR:
          spin = new QDoubleSpinBox(parent);
          spin->setDecimals(1);
          spin->setRange(0.0, 500.0);
          spin->setSingleStep(1.0);
          spin->setSuffix(Resource::DEGREE);
          spin->setAccelerated(true);
          spin->installEventFilter(const_cast<GrainDelegate*>(this));
          return spin;

      case GrainModel::TYPE:
          combo = new QComboBox(parent);
          combo->setEditable(false);
          combo->addItems(Grain::typeStringList());
          combo->installEventFilter(const_cast<GrainDelegate*>(this));
          return combo;

      case GrainModel::USE:
          combo = new QComboBox(parent);
          combo->setEditable(false);
          combo->addItems(Grain::useStringList());
          combo->installEventFilter(const_cast<GrainDelegate*>(this));
          return combo;

      default:
          return 0;
    }
}

void GrainDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
    QComboBox *combo;
    QDoubleSpinBox *spin;
    int comboindex;

    QVariant value = index.model()->data(index, Qt::EditRole);

    // different kind of editor for each column
    switch (index.column()) {
      case GrainModel::NAME:
          combo = static_cast<QComboBox*>(editor);
          if (!combo) return;
          comboindex = combo->findText(value.toString());
          if (comboindex > 0) {
              combo->setCurrentIndex(comboindex);
          } else {
              combo->setEditText(value.toString());
          }
          break;

      case GrainModel::WEIGHT:
      case GrainModel::EXTRACT:
      case GrainModel::COLOR:
          spin = static_cast<QDoubleSpinBox*>(editor);
          if (!spin) return;
          spin->setValue(value.toDouble());
          break;

      case GrainModel::TYPE:
      case GrainModel::USE:
          combo = static_cast<QComboBox*>(editor);
          if (!combo) return;
          combo->setCurrentIndex(combo->findText(value.toString()));
          break;

      default:
          QItemDelegate::setEditorData(editor, index);
          break;
    }
}

void GrainDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const
{
    QComboBox *combo;
    QDoubleSpinBox *spin;
    QVariant value;

    // different kind of editor for each column
    switch (index.column()) {
      case GrainModel::NAME:
      case GrainModel::TYPE:
      case GrainModel::USE:
          combo = static_cast<QComboBox*>(editor);
          if (!combo) return;
          value = combo->currentText();
          model->setData(index, value);
          break;

      case GrainModel::WEIGHT:
      case GrainModel::EXTRACT:
      case GrainModel::COLOR:
          spin = static_cast<QDoubleSpinBox*>(editor);
          if (!spin) return;
          value = spin->value();
          model->setData(index, value);
          break;

      default:
          QItemDelegate::setModelData(editor, model,index);
          break;
    }
}

void GrainDelegate::updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &/*index*/) const
{
    if (editor) editor->setGeometry(option.rect);
}

