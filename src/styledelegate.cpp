/***************************************************************************
  styledelegate.cpp
  -------------------
  Style delegate editor
  -------------------
  Copyright 2006-2008, David Johnson
  Please see the header file for copyright and license information
 ***************************************************************************/

#include <QAbstractItemModel>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QSpinBox>

#include "data.h"
#include "style.h"
#include "styledelegate.h"
#include "stylemodel.h"

StyleDelegate::StyleDelegate(QObject *parent) : QItemDelegate(parent) {}

StyleDelegate::~StyleDelegate() {}

QWidget *StyleDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &/*option*/,
                                     const QModelIndex &index) const
{
    QLineEdit *edit;
    QDoubleSpinBox *dspin;
    QSpinBox *spin;
    QString suffix;

    // can only edit name on blank row
    bool blank = index.row() >= index.model()->rowCount();

    // different kind of editor for each column
    switch (index.column()) {
      case StyleModel::NAME:
          edit = new QLineEdit(parent);
          edit->installEventFilter(const_cast<StyleDelegate*>(this));
          return edit;

      case StyleModel::OGLOW:
      case StyleModel::OGHI:
      case StyleModel::FGLOW:
      case StyleModel::FGHI:
          if (blank) return 0;
          dspin = new QDoubleSpinBox(parent);
          dspin->setDecimals(3);
          dspin->setRange(0.990, 1.150);
          dspin->setSingleStep(0.001);
          dspin->setAccelerated(true);
          dspin->installEventFilter(const_cast<StyleDelegate*>(this));
          return dspin;

      case StyleModel::IBULOW:
      case StyleModel::IBUHI:
          if (blank) return 0;
          spin = new QSpinBox(parent);
          spin->setRange(0, 120);
          spin->setSingleStep(1);
          spin->setAccelerated(true);
          spin->installEventFilter(const_cast<StyleDelegate*>(this));
          return spin;

      case StyleModel::SRMLOW:
      case StyleModel::SRMHI:
          if (blank) return 0;
          spin = new QSpinBox(parent);
          spin->setRange(0, 50);
          spin->setSingleStep(1);
          spin->setAccelerated(true);
          spin->installEventFilter(const_cast<StyleDelegate*>(this));
          return spin;

      default:
          return 0;
    }
}

void StyleDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
    QLineEdit *edit;
    QDoubleSpinBox *dspin;
    QSpinBox *spin;

    QVariant value = index.model()->data(index, Qt::EditRole);

    // different kind of editor for each column
    switch (index.column()) {
      case StyleModel::NAME:
          edit = static_cast<QLineEdit*>(editor);
          if (!edit) return;
          edit->setText(value.toString());
          break;

      case StyleModel::OGLOW:
      case StyleModel::OGHI:
      case StyleModel::FGLOW:
      case StyleModel::FGHI:
          dspin = static_cast<QDoubleSpinBox*>(editor);
          if (!dspin) return;
          dspin->setValue(value.toDouble());
          break;

      case StyleModel::IBULOW:
      case StyleModel::IBUHI:
      case StyleModel::SRMLOW:
      case StyleModel::SRMHI:
          spin = static_cast<QSpinBox*>(editor);
          if (!spin) return;
          spin->setValue(value.toUInt());
          break;
      default:
          QItemDelegate::setEditorData(editor, index);
          break;
    }
}

void StyleDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const
{
    QLineEdit *edit;
    QDoubleSpinBox *dspin;
    QSpinBox *spin;
    QVariant value;

    // different kind of editor for each column
    switch (index.column()) {
      case StyleModel::NAME:
          edit = static_cast<QLineEdit*>(editor);
          if (!edit) return;
          value = edit->text();
          model->setData(index, value);
          break;

      case StyleModel::OGLOW:
      case StyleModel::OGHI:
      case StyleModel::FGLOW:
      case StyleModel::FGHI:
          dspin = static_cast<QDoubleSpinBox*>(editor);
          if (!dspin) return;
          value = dspin->value();
          model->setData(index, value);
          break;

      case StyleModel::IBULOW:
      case StyleModel::IBUHI:
      case StyleModel::SRMLOW:
      case StyleModel::SRMHI:
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

void StyleDelegate::updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}
