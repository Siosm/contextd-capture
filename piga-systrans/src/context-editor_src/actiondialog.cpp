#include "actiondialog.h"
#include "ui_actiondialog.h"
#include "variabletypes.h"

ActionDialog::ActionDialog(QStringList blacklist, QWidget *parent) :
	QDialog(parent),
	_result(Rejected),
	ui(new Ui::ActionDialog)
{
	initDialog();
	setWindowTitle("New Program Action");
	ui->buttonBox->button(QDialogButtonBox::Ok)->setText("C&reate");

	//Setup the blacklist for the name line edit
	ui->nameLineEdit->setBlacklist(blacklist);
}

ActionDialog::ActionDialog(QStringList blacklist, const ProgramAction &subject, const QList<ProgramVariable> &vars, QWidget *parent) :
	QDialog(parent),
	_result(Rejected),
	ui(new Ui::ActionDialog),
	_action(subject)
{
	initDialog();
	setWindowTitle("Edit Program Action");

	//Setup the blacklist for the action line edit
	blacklist.removeAll(subject.name());
	ui->nameLineEdit->setBlacklist(blacklist);

	//Fill the widgets with the action's values
	ui->nameLineEdit->setText(subject.name());
	ui->displayNameLineEdit->setText(subject.displayName());
	ui->descriptionLineEdit->setText(subject.description());

	//Fill the variables tree widget and build the blacklist for the variables line edit
	blacklist.clear();
	QList<ProgramVariable>::const_iterator it;

	for(it=vars.begin(); it!=vars.end(); ++it)
	{
		const ProgramVariable &var=*it;
		blacklist << var.name();

		//Create the tree item
		QTreeWidgetItem *item=new QTreeWidgetItem(ui->varTree, QStringList(var.name()));

		//Set its type
		item->setText(1, Type::toStringForDisplay(var.type()));
		item->setData(1, Qt::UserRole, var.type());

		//Fill the constraints column
		switch(var.type())
		{
		case Type::Int:
			item->setText(2, var.intRange());
			break;

		case Type::String:
		case Type::RegExp:
			item->setText(2, var.stringValidation());
			break;

		case Type::StringList:
			item->setText(2, QString("%1 items").arg(var.stringList().size()));
			item->setData(2, Qt::UserRole, var.stringList());
			item->setToolTip(2, var.stringList().join("\n"));
			break;
		}

		//Fill the description
		item->setText(3, var.description());
		item->setToolTip(3, var.description());
	}

	ui->varNameLineEdit->setBlacklist(blacklist);
	onMandatoryFieldsChanged();
}

ActionDialog::~ActionDialog()
{
    delete ui;
}

void ActionDialog::initDialog()
{
	ui->setupUi(this);

	//Fill the variable type combo
//	ui->varTypeBox->addItem(Type::toStringForDisplay(Type::Boolean), Type::Boolean);
	ui->varTypeBox->addItem(Type::toStringForDisplay(Type::Int), Type::Int);
	ui->varTypeBox->addItem(Type::toStringForDisplay(Type::RegExp), Type::RegExp);
//	ui->varTypeBox->addItem(Type::toStringForDisplay(Type::String), Type::String);
	ui->varTypeBox->addItem(Type::toStringForDisplay(Type::StringList), Type::StringList);

	//Connect signals to their handlers
	connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(onOkClicked()));
	connect(ui->varAddButton, SIGNAL(clicked()), this, SLOT(onAddClicked()));
	connect(ui->varRemoveButton, SIGNAL(clicked()), this, SLOT(onRemoveClicked()));

	//Make sure all fields are properly filled
	connect(ui->nameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(onMandatoryFieldsChanged()));
	connect(ui->displayNameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(onMandatoryFieldsChanged()));
	connect(ui->varAddButton, SIGNAL(clicked()), this, SLOT(onMandatoryFieldsChanged()));
	connect(ui->varRemoveButton, SIGNAL(clicked()), this, SLOT(onMandatoryFieldsChanged()));
	onMandatoryFieldsChanged();

	connect(ui->varTree, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChanged()));
	onSelectionChanged();

	connect(ui->varTypeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onTypeChanged(int)));
	connect(ui->varTypeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onVarMandatoryFieldsChanged()));
	connect(ui->varNameLineEdit, SIGNAL(textChanged(QString)), this, SLOT (onVarMandatoryFieldsChanged()));
	connect(ui->varExpLineEdit, SIGNAL(textChanged(QString)), this, SLOT (onVarMandatoryFieldsChanged()));
	connect(ui->varMinLineEdit, SIGNAL(textChanged(QString)), this, SLOT (onVarMandatoryFieldsChanged()));
	connect(ui->varMaxLineEdit, SIGNAL(textChanged(QString)), this, SLOT (onVarMandatoryFieldsChanged()));
	connect(ui->varListTextEdit, SIGNAL(textChanged()), this, SLOT (onVarMandatoryFieldsChanged()));
	onTypeChanged(ui->varTypeBox->currentIndex());
	onVarMandatoryFieldsChanged();

	//Setup the line edits
	ui->nameLineEdit->setFocus(Qt::PopupFocusReason);
	ui->nameLineEdit->setValidationExpression(".+");
	ui->displayNameLineEdit->setValidationExpression(".+");
	ui->varNameLineEdit->setValidationExpression(".+");
	ui->varMinLineEdit->setValidationExpression("(-[0-9]+)|([0-9]*)");
	ui->varMaxLineEdit->setValidationExpression("(-[0-9]+)|([0-9]*)");
	ui->varExpLineEdit->setIsRegexpSyntax(true);

	//Setup the variables tree
	ui->varTree->header()->setResizeMode(1, QHeaderView::ResizeToContents);
	ui->varTree->header()->setResizeMode(2, QHeaderView::ResizeToContents);
}

/* Overriden dialog result functions */
void ActionDialog::setResult(int result)
{
	_result = result;
}

int ActionDialog::result() const
{
	if(_result > QDialog::Accepted)
		return _result;
	else
		return QDialog::result();
}

ProgramAction ActionDialog::fetchAction() const
{
	return ProgramAction(ui->nameLineEdit->text(), ui->displayNameLineEdit->text(), ui->descriptionLineEdit->text());
}

QList<ProgramVariable> ActionDialog::fetchVariables() const
{
	QList<ProgramVariable> vars;

	for(int i=0; i<ui->varTree->topLevelItemCount(); ++i)
	{
		QTreeWidgetItem *item=ui->varTree->topLevelItem(i);
		ProgramVariable var(item->text(0), ui->nameLineEdit->text(), item->text(3));
		var.setType(item->data(1, Qt::UserRole).toInt());

		switch(var.type())
		{
		case Type::Int:
			var.setIntRangeFromString(item->text(2));
			break;

		case Type::RegExp:
		case Type::String:
			var.setStringValidation(item->text(2));
			break;

		case Type::StringList:
			var.setStringList(item->data(2, Qt::UserRole).toStringList());
			break;
		}

		vars.append(var);
	}

	return vars;
}

void ActionDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
	switch(e->type())
	{
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ActionDialog::onOkClicked()
{
	if(_action != ProgramAction())
		this->setResult(Edited);
	else
		this->setResult(Created);
	this->accept();
}

void ActionDialog::onMandatoryFieldsChanged()
{
	//Make sure no mandatory field is empty
	bool valid=(ui->nameLineEdit->text().size() > 0) &&
			   (ui->displayNameLineEdit->text().size() > 0) &&
			   (ui->varTree->topLevelItemCount() > 0);

	//Make sure the action name is unique
	if(valid)
		ui->nameLineEdit->valueIsValid();


	//Enable the Ok button if the domain in the dialog is valid
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
}

void ActionDialog::onVarMandatoryFieldsChanged()
{
	//Make sure no mandatory field is empty
	bool valid=(ui->varNameLineEdit->text().size() > 0);

	//Make sure the variable name is unique
	if(valid)
		valid=ui->varNameLineEdit->valueIsValid();

	//Check the constraints depending on the variable type
	if(valid)
	{
		int type=ui->varTypeBox->itemData(ui->varTypeBox->currentIndex(), Qt::UserRole).toInt();
		if(type==Type::RegExp || type==Type::String)
			valid=ui->varExpLineEdit->valueIsValid();
		else if(type==Type::StringList)
			valid=ui->varListTextEdit->document()->toPlainText().size()>0;
		else if(type==Type::Int)
		{
			valid=ui->varMaxLineEdit->valueIsValid() && ui->varMinLineEdit->valueIsValid();
			if(ui->varMaxLineEdit->text().size()>0 && ui->varMinLineEdit->text().size()>0)
				valid&=ui->varMaxLineEdit->text().toInt() >= ui->varMinLineEdit->text().toInt();
		}
	}

	//Enable the Ok button if the domain in the dialog is valid
	ui->varAddButton->setEnabled(valid);
}

void ActionDialog::onTypeChanged(int index)
{
	int type=ui->varTypeBox->itemData(index, Qt::UserRole).toInt();

	//Hide all type-specific widgets in a first time
	ui->varListTextEdit->hide();
	ui->varExpLineEdit->hide();
	ui->varMinLineEdit->hide();
	ui->varMaxLineEdit->hide();
	ui->varRangeFromLabel->hide();
	ui->varRangeToLabel->hide();
	ui->varConstraintsLabel->hide();

	//Then display those that are needed
	if(type==Type::RegExp || type==Type::String)
	{
		ui->varConstraintsLabel->show();
		ui->varConstraintsLabel->setText("Matches");
		ui->varExpLineEdit->show();
	}
	else if(type==Type::Int)
	{
		ui->varConstraintsLabel->show();
		ui->varConstraintsLabel->setText("Range");
		ui->varMinLineEdit->show();
		ui->varMaxLineEdit->show();
		ui->varRangeFromLabel->show();
		ui->varRangeToLabel->show();
	}
	else if(type==Type::StringList)
	{
		ui->varConstraintsLabel->show();
		ui->varConstraintsLabel->setText("Values");
		ui->varListTextEdit->show();
	}
}

void ActionDialog::onAddClicked()
{
	QString name=ui->varNameLineEdit->text();
	int type=ui->varTypeBox->itemData(ui->varTypeBox->currentIndex(), Qt::UserRole).toInt();

	//Update varNameLineEdit's blacklist
	QStringList blacklist=ui->varNameLineEdit->blacklist();
	if(!blacklist.contains(name))
	{
		blacklist<<name;
		ui->varNameLineEdit->setBlacklist(blacklist);
	}

	//Create the item
	QTreeWidgetItem *item=new QTreeWidgetItem(ui->varTree, QStringList(name));

	//Set its type
	item->setText(1, ui->varTypeBox->currentText());
	item->setData(1, Qt::UserRole, type);

	//Fill the constraints column
	switch(type)
	{
	case Type::Int:
		if(ui->varMinLineEdit->text().size()>0)
		{
			if (ui->varMaxLineEdit->text().size()>0)
				item->setText(2, ui->varMinLineEdit->text()+":"+ui->varMaxLineEdit->text());
			else
				item->setText(2, ui->varMinLineEdit->text()+":");

		}
		else if (ui->varMaxLineEdit->text().size()>0)
			item->setText(2, ":"+ui->varMaxLineEdit->text());
		break;

	case Type::String:
	case Type::RegExp:
		if(ui->varExpLineEdit->valueIsValid())
			item->setText(2, ui->varExpLineEdit->text());
		break;

	case Type::StringList:
		if(ui->varListTextEdit->document()->characterCount()>0)
		{
			item->setText(2, QString("%1 items").arg(ui->varListTextEdit->document()->lineCount()));
			item->setData(2, Qt::UserRole, ui->varListTextEdit->document()->toPlainText().split("\n", QString::SkipEmptyParts));
			item->setToolTip(2, ui->varListTextEdit->document()->toPlainText());
		}
		break;
	}

	//Fetch the description
	item->setText(3, ui->varDescriptionLineEdit->text());
	item->setToolTip(3, ui->varDescriptionLineEdit->text());

	//Clear all widgets
	ui->varNameLineEdit->clear();
	ui->varDescriptionLineEdit->clear();
	ui->varExpLineEdit->clear();
	ui->varMinLineEdit->clear();
	ui->varMaxLineEdit->clear();
	ui->varListTextEdit->clear();
}

void ActionDialog::onRemoveClicked()
{
	QList<QTreeWidgetItem *> selection=ui->varTree->selectedItems();
	if(selection.size()==0)
	{
		qCritical("ActionDialog: tried to remove an empty selection of variables");
		return;
	}

	//Update varNameLineEdit's blacklist
	QStringList blacklist=ui->varNameLineEdit->blacklist();
	blacklist.removeAll(selection.first()->text(0));
	ui->varNameLineEdit->setBlacklist(blacklist);

	//Delete the selected item
	qDeleteAll(ui->varTree->selectedItems());

	//Call mandatoryFieldsChanged in case the name of the deleted variable was used in the addVariable name field
	onVarMandatoryFieldsChanged();
}

void ActionDialog::onSelectionChanged()
{
	ui->varRemoveButton->setEnabled(ui->varTree->selectedItems().size()>0);
}
