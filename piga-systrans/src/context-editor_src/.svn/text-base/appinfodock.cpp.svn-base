#include "configurationstack.h"
#include "appdialog.h"
#include "appinfodock.h"
#include "ui_appinfodock.h"

AppInfoDock::AppInfoDock(QWidget *parent) :
	EditorDock(parent),
	ui(new Ui::AppInfoDock),
	_loops_tree(true),
	_rules_tree(true)
{
	ui->setupUi(this);

	//Setup child tree widgets
	ui->loopsGroupBox->layout()->addWidget(&_loops_tree);
	ui->transitionsGroupBox->layout()->addWidget(&_rules_tree);

	_loops_tree.setRuleType(Loop().ruleType());
	_rules_tree.setRuleType(Rule().ruleType());

	//Resizing of columns in action tree
	ui->actionsTree->header()->setResizeMode(0, QHeaderView::ResizeToContents);
	ui->actionsTree->header()->setResizeMode(1, QHeaderView::ResizeToContents);

	//Resize the edit button so that it fits the icon button above
	ui->editButton->setMaximumWidth(ui->iconButton->geometry().width());
	connect(ui->editButton, SIGNAL(clicked()), this, SLOT(onEditClicked()));

	//Watch changes to programs
	connect(&ConfigurationStack::instance(), SIGNAL(programChanged(QString,QString)), this, SLOT(onProgramChanged(QString,QString)));

	//Forward ruleActivated signals
	connect(&_loops_tree, SIGNAL(ruleActivated(int)), this, SIGNAL(ruleActivated(int)));
	connect(&_rules_tree, SIGNAL(ruleActivated(int)), this, SIGNAL(ruleActivated(int)));
}

AppInfoDock::~AppInfoDock()
{
    delete ui;
}

void AppInfoDock::changeEvent(QEvent *e)
{
    QDockWidget::changeEvent(e);
	switch(e->type())
	{
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void AppInfoDock::setProgram(Program prog)
{
	if(prog.isValid())
	{
		_program = prog;
		_loops_tree.setProgram(prog);
		_rules_tree.setProgram(prog);
		updateDock();
	}
	else
	{
		resetDock();
	}
}

Program AppInfoDock::program() const
{
	return _program;
}

bool AppInfoDock::isValid() const
{
	return _program.isValid();
}

void AppInfoDock::updateDock()
{
	//Update the program information
	ui->actionsTree->clear();

	QString descStr="<p><b>" + _program.displayName() + " (</b>" + _program.fullPath() + "<b>):</b> ";
	if(_program.description().size() > 0)
		descStr+=_program.description() + "</p>";
	else
		descStr+="no description available</p>";
	ui->descriptionLabel->setText(descStr);
	ui->descriptionLabel->setToolTip(descStr);

	//Setup the icon
	QPixmap icon(_program.icon());
	if(icon.isNull())
		icon.load(":/icons/default-program");
	ui->iconButton->setIcon(QIcon(icon));
	ui->iconButton->setToolTip(_program.icon());

	//Go through all actions
	QList<ProgramAction> actions = _program.actions();
	QList<ProgramAction>::const_iterator it;

	for(it=actions.begin(); it!=actions.end(); ++it)
	{
		const ProgramAction action = *it;

		//Display the action in the tree
		QStringList actStrList(action.displayName());
		actStrList.append(action.description());
		QTreeWidgetItem *top = new QTreeWidgetItem(ui->actionsTree, actStrList);
		top->setData(0, Qt::UserRole, action.name());
		top->setToolTip(1, action.description());

		//Fetch all the variables of the action
		QList<ProgramVariable> vars = _program.variables(action.name());
		QList<ProgramVariable>::const_iterator varit;

		for(varit=vars.begin(); varit!=vars.end(); ++varit)
		{
			const ProgramVariable var = *varit;

			//Display each variable in the tree
			QStringList varStrList(var.name());
			varStrList.append(var.description());
			QTreeWidgetItem *varItem = new QTreeWidgetItem(top, varStrList);
			varItem->setToolTip(1, var.description());
		}
	}

	_loops_tree.updateRulesList();
	_rules_tree.updateRulesList();
}

void AppInfoDock::resetDock()
{
	ui->descriptionLabel->clear();

	ui->iconButton->setIcon(QIcon());
	ui->iconButton->setToolTip(QString());

	ui->actionsTree->clear();

	_loops_tree.clearRulesList();
	_rules_tree.clearRulesList();

	close();
}

void AppInfoDock::onProgramChanged(QString old_name, QString new_name)
{
	if(_program.name()==old_name)
		setProgram(ConfigurationStack::instance().current().getProgramByName(new_name));
}

void AppInfoDock::onEditClicked()
{
	AppDialog dialog(_program, this->window());

	dialog.show();
	dialog.exec();

	if(dialog.result()==AppDialog::Edited)
		ConfigurationStack::instance().updateProgram(_program.name(), dialog.fetchProgram());
}
