#include <QMessageBox>
#include "mruledialog.h"
#include "typeddelegate.h"
#include "ui_mruledialog.h"
#include "variabletypes.h"

const QString MRuleDialog::UndefinedAction="<undefined>";

MRuleDialog::MRuleDialog(Program prog, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::MRuleDialog),
	_prog(prog),
	_current_variable_type(Type::Invalid)
{
	initDialog();
	setWindowTitle("New Matching Rule");
}

MRuleDialog::MRuleDialog(Program prog, QString action, QMap<QString,QString> conds, QString name, QWidget *parent) :
		QDialog(parent),
		ui(new Ui::MRuleDialog),
		_prog(prog),
		_current_variable_type(Type::Invalid)
{
	initDialog();
	setWindowTitle("Edit Matching Rule");

	//Set the current action
	ui->actionBox->setCurrentIndex(ui->actionBox->findData(action));
	if(ui->actionBox->currentIndex()==-1)
		ui->actionBox->setCurrentIndex(ui->actionBox->findText(UndefinedAction));

	//If the current action is valid, take care of the variables
	if(ui->actionBox->currentIndex()!=-1)
	{
		//Add the existing conditions to the list, and remove their variables from the variable combobox
		for(QMap<QString,QString>::const_iterator it=conds.begin(); it!=conds.end(); ++it)
		{
			QStringList itemStrList;
			ui->variableBox->removeItem(ui->variableBox->findText(it.key()));
			itemStrList.append(it.key());
			itemStrList.append(it.value());
			QTreeWidgetItem *item=new QTreeWidgetItem(ui->propList, itemStrList);
			item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);

			//Setup the delegate for editing the condition's value directly in the tree widget
			TypedDelegate *delegate=0;

			if(actionName()==QString())
				delegate=new TypedDelegate(Type::String, 1, this);
			else
			{
				ProgramVariable var=_prog.getVariableByName(it.key(), action);

				switch (var.type())
				{
				case Type::Int:
					delegate=new TypedDelegate(Type::Int, 1, this, var.intMinimum(), var.intMaximum());
					break;

				case Type::String:
				case Type::RegExp:
					delegate=new TypedDelegate(Type::String, 1, this, var.stringValidation());
					break;

				case Type::StringList:
					delegate=new TypedDelegate(Type::StringList, 1, this, var.stringList());
					break;

				case Type::Boolean:
					delegate=new TypedDelegate(Type::Boolean, 1, this);
				}
				ui->propList->setItemDelegateForRow(ui->propList->indexOfTopLevelItem(item), delegate);
			}
		}
	}

	//Initialise the display name line edit
	ui->displayNameLineEdit->setText(name);

	//Make sure the Ok button is enabled if there are enough properties
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(conditionCount() > 0);
}

MRuleDialog::~MRuleDialog()
{
	delete ui;
}

void MRuleDialog::initDialog()
{
	ui->setupUi(this);

	//Resizing and sorting of widgets
	ui->variableBox->setInsertPolicy(QComboBox::InsertAlphabetically);
	ui->propList->header()->setResizeMode(0, QHeaderView::ResizeToContents);

	//Make sure the button is enabled only when rules have been added
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

	//Hide description labels for now
	ui->actionDescLabel->hide();
	ui->valueDescLabel->hide();

	//Add the actions list and fill the variables list according to the selected action
	QList<ProgramAction> actList=_prog.actions();
	for(int i=0; i<actList.size(); i++)
		ui->actionBox->addItem(actList[i].displayName(), actList[i].name());
	connect(ui->actionBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onActionChanged(int)));
	ui->actionBox->insertItem(0, UndefinedAction);
	ui->actionBox->setCurrentIndex(0);

	//Setup the variables combobox and use the variable changed handler to initialise the editor widget
	connect(ui->variableBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onVariableChanged(QString)));
	onVariableChanged(QString());

	//Various signals for dialog buttons, mandatory fields, tree widget related buttons
	connect(ui->propAddButton, SIGNAL(clicked()), this, SLOT(onCondAddButtonClicked()));
	connect(ui->propRemoveButton, SIGNAL(clicked()), this, SLOT(onCondRemoveButtonClicked()));
	connect(ui->variableBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onMandatoryFieldsChanged()));
	connect(ui->valueLineEdit, SIGNAL(textChanged(QString)), this, SLOT(onMandatoryFieldsChanged()));
	connect(ui->valueComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onMandatoryFieldsChanged()));
	connect(ui->valueCheckBox, SIGNAL(clicked()), this, SLOT(onMandatoryFieldsChanged()));
	connect(ui->valueSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onMandatoryFieldsChanged()));
	connect(ui->propList, SIGNAL(itemSelectionChanged()), this, SLOT(onCondListSelectionChanged()));
	onMandatoryFieldsChanged();
	onCondListSelectionChanged();
}

QMap<QString, QString> MRuleDialog::ruleConditions() const
{
	//Build a variable:value map from the tree widget
	QMap<QString, QString> map;
	for(int i=0; i<ui->propList->topLevelItemCount(); ++i)
	{
		QTreeWidgetItem *item = ui->propList->topLevelItem(i);
		map[item->text(0)] =item->text(1);
	}

	return map;
}

QString MRuleDialog::actionName() const
{
	return ui->actionBox->itemData(ui->actionBox->currentIndex()).toString();
}

QString MRuleDialog::displayName() const
{
	return ui->displayNameLineEdit->text();
}

void MRuleDialog::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

int MRuleDialog::conditionCount() const
{
	return ui->propList->topLevelItemCount();
}

void MRuleDialog::onCondAddButtonClicked()
{
	//Data of the future new row
	QStringList itemStrList;
	itemStrList.append(ui->variableBox->currentText());
	QStringList data;

	//Delegate used to edit the row
	TypedDelegate *delegate=0;

	if(actionName()==QString())
		delegate=new TypedDelegate(Type::String, 1, this);
	else
	{
		//Variable that we have to add
		ProgramVariable var=_prog.getVariableByName(ui->variableBox->currentText(), actionName());
		switch (var.type())
		{
		case Type::Int:
			itemStrList.append(QString("%1").arg(ui->valueSpinBox->value()));
			delegate=new TypedDelegate(Type::Int, 1, this, ui->valueSpinBox->minimum(), ui->valueSpinBox->maximum());
			break;

		case Type::String:
			itemStrList.append(ui->valueLineEdit->text());
			delegate=new TypedDelegate(Type::String, 1, this, ui->valueLineEdit->validationExpression());
			break;

		case Type::RegExp:
			itemStrList.append(ui->valueLineEdit->text());
			qDebug("%s", qPrintable(ui->valueLineEdit->text()));
			delegate=new TypedDelegate(Type::String, 1, this, ui->valueLineEdit->validationExpression());
			break;

		case Type::StringList:
			itemStrList.append(ui->valueComboBox->currentText());
			for(int i=0; i<ui->valueComboBox->count(); ++i)
				data.append(ui->valueComboBox->itemText(i));
			delegate=new TypedDelegate(Type::StringList, 1, this, data);
			break;

		case Type::Boolean:
			itemStrList.append(ui->valueCheckBox->isChecked()? "True":"False");
			delegate=new TypedDelegate(Type::Boolean, 1, this);
		}
	}

	QTreeWidgetItem *item=new QTreeWidgetItem(ui->propList, itemStrList);
	item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled);
	ui->propList->setItemDelegateForRow(ui->propList->indexOfTopLevelItem(item), delegate);

	ui->variableBox->removeItem(ui->variableBox->currentIndex());
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(conditionCount() > 0);
}

void MRuleDialog::onCondRemoveButtonClicked()
{
	QList<QTreeWidgetItem *>list = ui->propList->selectedItems();
	for (int i=0, listsize=list.size(); i<listsize; ++i) {
		QTreeWidgetItem *item = list.at(i);
		ui->variableBox->addItem(item->text(0));
		delete item;
	}

	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(conditionCount() > 0);
}

void MRuleDialog::onCondListSelectionChanged()
{
	ui->propRemoveButton->setEnabled(ui->propList->selectedItems().size() > 0);
}

void MRuleDialog::onMandatoryFieldsChanged()
{
	bool valid=ui->variableBox->count() > 0;

	if (valid && actionName()!=QString())
	{
		ProgramVariable var=_prog.getVariableByName(ui->variableBox->currentText(), actionName());
		switch(var.type())
		{
		case Type::RegExp:
		case Type::String:
			valid=var.validate(ui->valueLineEdit->text());
			break;

		case Type::StringList:
			valid=var.validate(ui->valueComboBox->currentText());
			break;

		case Type::Int:
			valid=var.validate(ui->valueSpinBox->value());
			break;

		case Type::Boolean:
			valid=true;
			break;

		default:
			valid=false;
		}
	}

	ui->propAddButton->setEnabled(valid);
}

void MRuleDialog::onVariableChanged(QString var_name)
{
	//Hide all the edit widgets, and show below the one to be used
	ui->valueInvalidType->hide();
	ui->valueCheckBox->hide();
	ui->valueComboBox->hide();
	ui->valueLineEdit->hide();
	ui->valueSpinBox->hide();

	//Check that there is an action setup. If there is one, fetch current variable and put the good widget for its type
	if(actionName()!=QString())
	{
		ProgramVariable var=_prog.getVariableByName(var_name, actionName());
		ui->valueDescLabel->setText("<i>"+var.description()+"</i>");
		ui->valueDescLabel->show();

		//Setup the options for the editor widget we'll use
		switch(var.type())
		{
		case Type::RegExp:
		case Type::String:
			ui->valueLineEdit->setIsRegexpSyntax(var.type()==Type::RegExp);
			ui->valueLineEdit->setValidationExpression(var.stringValidation());
			ui->valueLineEdit->setAutocompletionList(ProgramVariable::assignedValues(var_name, var.type()));
			ui->valueLineEdit->clear();

			if(var.stringValidation()!=QString() && var.stringValidation()!=".*")
				ui->valueDescLabel->setText(ui->valueDescLabel->text()+"<i> The value must match the expression '"+ var.stringValidation()+"'.</i>");

			ui->valueLineEdit->show();
			break;

		case Type::StringList:
			ui->valueComboBox->clear();
			ui->valueComboBox->addItems(var.stringList());
			ui->valueComboBox->show();
			break;

		case Type::Int:
			ui->valueSpinBox->setRange(var.intMinimum(), var.intMaximum());
			ui->valueSpinBox->setValue(0 > var.intMinimum() ? 0:var.intMinimum());
			ui->valueSpinBox->show();
				break;

		case Type::Boolean:
			ui->valueCheckBox->setChecked(false);
			ui->valueCheckBox->show();
			break;

		default:
			ui->valueDescLabel->hide();
			ui->valueInvalidType->show();
		}
	}
	//Else just put a line edit for a string
	else
	{
		ui->valueLineEdit->setIsRegexpSyntax(false);
		ui->valueLineEdit->setValidationExpression(".*");
		ui->valueLineEdit->clear();
		ui->valueDescLabel->hide();
		ui->valueLineEdit->show();
	}
}

void MRuleDialog::onActionChanged(int index)
{
	QString actionName=ui->actionBox->itemData(index).toString();

	if(actionName!=UndefinedAction)
	{
		ui->propList->clear();

		ProgramAction act=_prog.getActionByName(actionName);
		if(act.description()!=QString())
		{
			ui->actionDescLabel->setText(act.description());
			ui->actionDescLabel->show();
		}
	}
	else
		ui->actionDescLabel->hide();

	//Clear the variables and add those of the new action
	ui->variableBox->clear();
	QList<ProgramVariable> varList=_prog.variables(actionName!=UndefinedAction ? actionName : QString());
	for(int i=0; i<varList.size(); ++i)
	{
		if(ui->variableBox->findText(varList[i].name())==-1)
			ui->variableBox->addItem(varList[i].name());
	}

	//Remove the variables that are already in use
	for(int i=0; i<ui->propList->topLevelItemCount(); ++i)
		ui->variableBox->removeItem(ui->variableBox->findText(ui->propList->topLevelItem(i)->text(0)));
}
