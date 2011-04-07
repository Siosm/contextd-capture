#include <QTreeWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include "typeddelegate.h"
#include "typedlineedit.h"
#include "variabletypes.h"

TypedDelegate::TypedDelegate(int type, int column, QObject *parent, QVariant data1, QVariant data2) :
	QItemDelegate(parent),
	_type(type),
	_column(column),
	_data1(data1),
	_data2(data2)
{
}

QWidget *TypedDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &index) const
{
	QCheckBox *check;
	QComboBox *combo;
	QSpinBox *spin;
	TypedLineEdit *edit;
	QWidget *widget=0;

	//Check that we edit the wanted column
	if(index.column()!=_column)
		return 0;

	//Depending on the delegate's type, return the good widget
	switch(_type)
	{
	case Type::RegExp:
		edit=new TypedLineEdit(parent);
		edit->setFrame(false);
		edit->setIsRegexpSyntax(true);

		if(_data1.isValid())
			edit->setValidationExpression(_data1.toString());
		if(_data2.isValid())
			edit->setBlacklist(_data2.toStringList());

		widget=edit;
		break;

	case Type::String:
		edit=new TypedLineEdit(parent);
		edit->setFrame(false);
		edit->setIsRegexpSyntax(false);

		if(_data1.isValid())
			edit->setValidationExpression(_data1.toString());
		if(_data2.isValid())
			edit->setBlacklist(_data2.toStringList());

		widget=edit;
		break;

	case Type::StringList:
		combo=new QComboBox(parent);

		if(_data1.isValid())
			combo->addItems(_data1.toStringList());

		widget=combo;
		break;

	case Type::Int:
		spin=new QSpinBox(parent);

		if(_data1.isValid())
			spin->setMinimum(_data1.toInt());
		if(_data2.isValid())
			spin->setMaximum(_data2.toInt());

		widget=spin;
		break;

	case Type::Boolean:
		check=new QCheckBox(parent);
		widget=check;
		break;
	}

	return widget;
}


void TypedDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QCheckBox *check;
	QComboBox *combo;
	QSpinBox *spin;
	TypedLineEdit *edit;

	switch(_type)
	{
	case Type::RegExp:
	case Type::String:
		edit=static_cast<TypedLineEdit*>(editor);
		edit->setText(index.data(Qt::EditRole).toString());
		break;

	case Type::StringList:
		combo=static_cast<QComboBox*>(editor);
		combo->setCurrentIndex(combo->findText(index.data(Qt::EditRole).toString()));
		break;

	case Type::Int:
		spin=static_cast<QSpinBox*>(editor);
		spin->setValue(index.data(Qt::EditRole).toInt());
		break;

	case Type::Boolean:
		check=static_cast<QCheckBox*>(editor);
		check->setChecked(index.data(Qt::EditRole).toBool());
		break;
	}
}

void TypedDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QVariant value=model->data(index, Qt::EditRole);

	QCheckBox *check;
	QComboBox *combo;
	QSpinBox *spin;
	TypedLineEdit *edit;

	switch(_type)
	{

	case Type::RegExp:
	case Type::String:
		edit=static_cast<TypedLineEdit*>(editor);
		if(edit->valueIsValid())
			value=edit->text();
		break;

	case Type::StringList:
		combo=static_cast<QComboBox*>(editor);
		value=combo->currentText();
		break;

	case Type::Int:
		spin=static_cast<QSpinBox*>(editor);
		value=spin->value();
		break;

	case Type::Boolean:
		check=static_cast<QCheckBox*>(editor);
		value=check->isChecked();
	}

	model->setData(index, value, Qt::EditRole);
}

void TypedDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
	editor->setGeometry(option.rect);
}
