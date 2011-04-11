#include <QFileDialog>
#include "actiondialog.h"
#include "configurationstack.h"
#include "appdialog.h"
#include "editorconfig.h"
#include "ui_appdialog.h"
#include "variabletypes.h"

AppDialog::AppDialog(QWidget *parent) :
	QDialog(parent),
	_result(QDialog::Rejected),
	ui(new Ui::AppDialog)
{
	initDialog();
	setWindowTitle("New Program");
	ui->buttonBox->button(QDialogButtonBox::Ok)->setText("C&reate");

	//Setup the blacklist for the name line edit
	QStringList blacklist=ConfigurationStack::instance().current().domainNameList();
	ui->nameEdit->setBlacklist(blacklist);
}

AppDialog::AppDialog(const Program &subject, QWidget *parent) :
	QDialog(parent),
	_result(QDialog::Rejected),
	ui(new Ui::AppDialog),
	_program(subject)
{
	initDialog();
	setWindowTitle("Edit Program");

	//Setup the blacklist for the name line edit
	ui->nameEdit->setText(subject.name());
	QStringList blacklist=ConfigurationStack::instance().current().programNameList();
	blacklist.removeAll(subject.name());
	ui->nameEdit->setBlacklist(blacklist);

	//Setup the widgets with the program's data
	ui->nameEdit->setText(subject.name());
	ui->displayNameEdit->setText(subject.displayName());
	ui->pathLineEdit->setText(subject.fullPath());
	ui->descriptionTextEdit->setPlainText(subject.description());

	//Setup icon
	ui->iconButton->setIcon(QIcon(subject.icon()));
	ui->iconButton->setToolTip(subject.icon());

	//Import actions
	QList<ProgramAction> actions = subject.actions();
	QList<ProgramAction>::const_iterator it;

	for(it=actions.begin(); it!=actions.end(); ++it)
	{
		const ProgramAction action = *it;
		_actions[action.name()]=action;
		_variables[action.name()]=subject.variables(action.name());
	}
	updateActions();
}

void AppDialog::initDialog()
{
	ui->setupUi(this);

	//Connect signals to their handlers
	connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(onOkClicked()));
	connect(ui->iconButton, SIGNAL(clicked()), this, SLOT(onIconClicked()));
	connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(onBrowseClicked()));
	connect(ui->addButton, SIGNAL(clicked()), this, SLOT(onAddClicked()));
	connect(ui->editButton, SIGNAL(clicked()), this, SLOT(onEditClicked()));
	connect(ui->removeButton, SIGNAL(clicked()), this, SLOT(onRemoveClicked()));

	//Make sure all fields are properly filled
	connect(ui->nameEdit, SIGNAL(textChanged(QString)), this, SLOT(onMandatoryFieldsChanged()));
	connect(ui->displayNameEdit, SIGNAL(textChanged(QString)), this, SLOT(onMandatoryFieldsChanged()));
	connect(ui->pathLineEdit, SIGNAL(textChanged(QString)), this, SLOT(onMandatoryFieldsChanged()));
	onMandatoryFieldsChanged();

	connect(ui->actVarTree, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChanged()));
	onSelectionChanged();

	//Setup the line edits
	ui->nameEdit->setFocus(Qt::PopupFocusReason);
	ui->nameEdit->setValidationExpression(".+");
	ui->displayNameEdit->setValidationExpression(".+");
	ui->pathLineEdit->setValidationExpression("/.*");

	//Setup the actions and variables tree
	ui->actVarTree->header()->setResizeMode(0, QHeaderView::ResizeToContents);
	ui->actVarTree->header()->setResizeMode(1, QHeaderView::ResizeToContents);
}

void AppDialog::updateActions()
{
	ui->actVarTree->clear();
	QMap<QString, ProgramAction>::const_iterator it;

	for(it=_actions.begin(); it!=_actions.end(); ++it)
	{
		const ProgramAction action = it.value();

		//Display the action in the tree
		QStringList actStrList(action.displayName());
		actStrList << QString() << action.description();
		QTreeWidgetItem *top = new QTreeWidgetItem(ui->actVarTree, actStrList);
		top->setData(0, Qt::UserRole, action.name());
		top->setToolTip(2, action.description());

		//Fetch all the variables of the action
		QList<ProgramVariable> vars = _variables[action.name()];
		QList<ProgramVariable>::const_iterator varit;

		for(varit=vars.begin(); varit!=vars.end(); ++varit)
		{
			const ProgramVariable var = *varit;

			//Display each variable in the tree
			QStringList varStrList(var.name());
			varStrList << Type::toStringForDisplay(var.type()) << var.description();
			QTreeWidgetItem *varItem = new QTreeWidgetItem(top, varStrList);
			varItem->setData(1, Qt::UserRole, Type::toString(var.type()));
			varItem->setToolTip(2, var.description());
		}
	}
}

AppDialog::~AppDialog()
{
    delete ui;
}

void AppDialog::changeEvent(QEvent *e)
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

/* Overriden dialog result functions */
void AppDialog::setResult(int result)
{
	_result = result;
}

int AppDialog::result() const
{
	if(_result > QDialog::Accepted)
		return _result;
	else
		return QDialog::result();
}

Program AppDialog::fetchProgram() const
{
	//Create the program
	Program prog(ui->nameEdit->text(),  ui->displayNameEdit->text(),  ui->iconButton->toolTip(),
				 ui->pathLineEdit->text(), ui->descriptionTextEdit->document()->toPlainText());

	//Fetch all variables into a single list
	QList<ProgramVariable> allVars;
	QMap<QString, QList<ProgramVariable> >::const_iterator it;
	for(it=_variables.begin(); it!=_variables.end(); ++it)
	{
		allVars << it.value();
	}

	//Set the program's actions and variables
	prog.setActionsAndVariables(_actions, allVars);

	return prog;
}

void AppDialog::onBrowseClicked()
{
	QString dir=ui->pathLineEdit->text()!=QString() ? ui->pathLineEdit->text() : EditorConfig::lastOpenedPath();
	QString fileName=QFileDialog::getOpenFileName(this, "Select binary...", dir);

	if(fileName.size() > 0)
	{
		EditorConfig::setLastOpenedPath(fileName);
		ui->pathLineEdit->setText(fileName);
	}
}

void AppDialog::onIconClicked()
{
	QString dir=ui->iconButton->toolTip()!=QString() ? ui->iconButton->toolTip() : EditorConfig::lastOpenedPath();
	QString fileName=QFileDialog::getOpenFileName(this, "Select icon...", dir, "Icon Files (*.png *.svg *.ico *.xpm *.gif)");

	if(fileName.size() > 0)
	{
		EditorConfig::setLastOpenedPath(fileName);
		ui->iconButton->setIcon(QIcon(fileName));
		ui->iconButton->setToolTip(fileName);
	}
}

void AppDialog::onOkClicked()
{
	if(_program != Program())
		this->setResult(Edited);
	else
		this->setResult(Created);
	this->accept();
}

void AppDialog::onMandatoryFieldsChanged()
{
	//Make sure no mandatory field is empty
	bool valid=(ui->nameEdit->text().size() > 0) &&
			   (ui->displayNameEdit->text().size() > 0) &&
			   (ui->pathLineEdit->text().size() > 0);

	//Make sure the app name is unique
	if(valid)
		valid=ui->nameEdit->valueIsValid();

	//Enable the Ok button if the domain in the dialog is valid
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
}

void AppDialog::onAddClicked()
{
	ActionDialog dialog(_actions.keys(), this);

	dialog.show();
	dialog.exec();

	if(dialog.result()==ActionDialog::Created)
	{
		ProgramAction action=dialog.fetchAction();
		_actions[action.name()]=action;
		_variables[action.name()]=dialog.fetchVariables();
	}

	updateActions();
}

void AppDialog::onEditClicked()
{
	QList<QTreeWidgetItem*> selected=ui->actVarTree->selectedItems();
	if(selected.isEmpty())
		return;

	//Get the top level item for the selected item
	QTreeWidgetItem *top=selected.at(0);
	while (top->parent())
		top=top->parent();

	ProgramAction oldAction=_actions.value(top->data(0, Qt::UserRole).toString());

	ActionDialog dialog(_actions.keys(), oldAction, _variables[oldAction.name()], this);

	dialog.show();
	dialog.exec();

	if(dialog.result()==ActionDialog::Edited)
	{
		_actions.remove(oldAction.name());

		ProgramAction action=dialog.fetchAction();
		_actions[action.name()]=action;
		_variables[action.name()]=dialog.fetchVariables();
	}

	updateActions();
}

void AppDialog::onRemoveClicked()
{
	QList<QTreeWidgetItem*> selected=ui->actVarTree->selectedItems();
	if(selected.isEmpty())
		return;

	//Get the top level item for the selected item
	QTreeWidgetItem *top=selected.at(0);
	while (top->parent())
		top=top->parent();

	QString name=top->data(0, Qt::UserRole).toString();
	_actions.remove(name);
	_variables.remove(name);

	delete top;
}

void AppDialog::onSelectionChanged()
{
	bool selected=ui->actVarTree->selectedItems().size() > 0;

	ui->editButton->setEnabled(selected);
	ui->removeButton->setEnabled(selected);
}
