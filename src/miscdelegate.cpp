/***************************************************************************
  miscdelegate.cpp
  -------------------
  Misc delegate editor
  -------------------
  Copyright 2006-2008, David Johnson
  Please see the header file for copyright and license information
 ***************************************************************************/

#include <QAbstractItemModel>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>

#include "data.h"
#include "misc.h"
#include "miscdelegate.h"
#include "miscmodel.h"

MiscDelegate::MiscDelegate(QObject *parent) : QItemDelegate(parent) {}

MiscDelegate::~MiscDelegate() {}

QWidget *MiscDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &/*option*/,
                                     const QModelIndex &index) const
{
    QComboBox *combo;
    QDoubleSpinBox *spin;
    QLineEdit *edit;
    QString suffix;

    // can only edit name on blank row
    bool blank = index.row() >= index.model()->rowCount();

    // different kind of editor for each column
    switch (index.column()) {
      case MiscModel::NAME:
          combo = new QComboBox(parent);
          combo->setEditable(true);
          combo->addItem(QString());
          combo->addItems(Data::instance()->miscList());
          combo->installEventFilter(const_cast<MiscDelegate*>(this));
          return combo;

      case MiscModel::QUANTITY:
          if (blank) return 0;
          spin = new QDoubleSpinBox(parent);
          spin->setDecimals(3);
          spin->setRange(0.00, 1000.00);
          spin->setSingleStep(0.25);
          suffix = " " + Data::instance()->defaultMiscUnit().symbol();
          spin->setSuffix(suffix);
          spin->setAccelerated(true);
          spin->installEventFilter(const_cast<MiscDelegate*>(this));
          return spin;

      case MiscModel::TYPE:
          combo = new QComboBox(parent);
          combo->setEditable(false);
          combo->addItems(Misc::typeStringList());
          combo->installEventFilter(const_cast<MiscDelegate*>(this));
          return combo;

      case MiscModel::NOTES:
          if (blank) return 0;
          edit = new QLineEdit(parent);
          edit->installEventFilter(const_cast<MiscDelegate*>(this));
          return edit;

      default:
          return 0;;
    }
}

void MiscDelegate::setEditorData(QWidget *editor,
                                 const QModelIndex &index) const
{
    QComboBox *combo;
    QDoubleSpinBox *spin;
    QLineEdit *edit;
    int comboindex;

    QVariant value = index.model()->data(index, Qt::EditRole);

    // different kind of editor for each column
    switch (index.column()) {
      case MiscModel::NAME:
          combo = static_cast<QComboBox*>(editor);
          if (!combo) return;
          comboindex = combo->findText(value.toString());
          if (comboindex > 0) {
              combo->setCurrentIndex(comboindex);
          } else {
              combo->setEditText(value.toString());
          }
          break;

      case MiscModel::QUANTITY:
          spin = static_cast<QDoubleSpinBox*>(editor);
          if (!spin) return;
          spin->setValue(value.toDouble());
          break;

      case MiscModel::NOTES:
          edit = static_cast<QLineEdit*>(editor);
          if (!edit) return;
          edit->setText(value.toString());
          break;

      case MiscModel::TYPE:
          combo = static_cast<QComboBox*>(editor);
          if (!combo) return;
          combo->setCurrentIndex(combo->findText(value.toString()));
          break;

      default:
          QItemDelegate::setEditorData(editor, index);
          break;
    }
}

void MiscDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const
{
    QComboBox *combo;
    QDoubleSpinBox *spin;
    QLineEdit *edit;
    QVariant value;

    // different kind of editor for each column
    switch (index.column()) {
      case MiscModel::NAME:
      case MiscModel::TYPE:
          combo = static_cast<QComboBox*>(editor);
          if (!combo) return;
          value = combo->currentText();
          model->setData(index, value);
          break;

      case MiscModel::NOTES:
          edit = static_cast<QLineEdit*>(editor);
          if (!edit) return;
          value = edit->text();
          model->setData(index, value);
          break;

      case MiscModel::QUANTITY:
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

void MiscDelegate::updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}
