#include <QMessageBox>
#include <QSettings>
#include "appdialog.h"
#include "browserdock.h"
#include "configurationstack.h"
#include "domaindialog.h"
#include "editorconfig.h"
#include "loopdialog.h"
#include "ruledialog.h"
#include "selectdefaultdomaindialog.h"
#include "ui_browserdock.h"

BrowserDock::BrowserDock(QWidget *parent) :
	EditorDock(parent),
	ui(new Ui::BrowserDock),
	_abs_rules_tree()
{
	ui->setupUi(this);
	ui->treeContainer->addWidget(&_abs_rules_tree);

	ui->comboBox->addItem("Domains", DomainMode);
	ui->comboBox->addItem("Programs", ApplicationMode);
	ui->comboBox->addItem("Transitions", RuleMode);

	//Selected items management
	connect(this, SIGNAL(viewModeChanged(int)), this, SLOT(onCurrentSelectionChanged()));
	connect(ui->listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(onCurrentSelectionChanged()));
	connect(&_abs_rules_tree, SIGNAL(itemSelectionChanged()), this, SLOT(onCurrentSelectionChanged()));
	connect(&_abs_rules_tree, SIGNAL(ruleActivated(int)), this, SIGNAL(ruleActivated(int)));

	//Double click on items
	connect(ui->listWidget, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(onItemActivated(QListWidgetItem*)));

	//Management of rules
	ConfigurationStack &stack=ConfigurationStack::instance();
	connect(&stack, SIGNAL(configurationChanged()), this, SLOT(onDomainChanged()));
	connect(&stack, SIGNAL(configurationChanged()), this, SLOT(onProgramsChanged()));
	connect(&stack, SIGNAL(domainChanged(QString,QString)), this, SLOT(onDomainChanged()));
	connect(&stack, SIGNAL(programChanged(QString,QString)), this, SLOT(onProgramsChanged()));

	//Application mode buttons
	connect(ui->appNewButton, SIGNAL(clicked()), this, SLOT(onAddProgram()));
	connect(ui->appEditButton, SIGNAL(clicked()), this, SLOT(onAppEditClicked()));
	connect(ui->appCloseButton, SIGNAL(clicked()), this, SLOT(onAppCloseClicked()));
	connect(ui->appZoomButton, SIGNAL(clicked()), this, SLOT(onAppFocusClicked()));

	//Domain mode buttons
	connect(ui->domainAddButton, SIGNAL(clicked()), this, SLOT(onAddDomain()));
	connect(ui->domainDeleteButton, SIGNAL(clicked()), this, SLOT(onDomainDeleteClicked()));
	connect(ui->domainEditButton, SIGNAL(clicked()), this, SLOT(onDomainEditClicked()));
	connect(ui->domainZoomButton, SIGNAL(clicked()), this, SLOT(onDomainFocusClicked()));

	//Rule mode buttons
	connect(ui->ruleAddButton, SIGNAL(clicked()), this, SLOT(onAddRule()));
	connect(ui->ruleDeleteButton, SIGNAL(clicked()), this, SLOT(onRuleDeleteClicked()));
	connect(ui->ruleEditButton, SIGNAL(clicked()), this, SLOT(onRuleEditClicked()));
	connect(ui->priorityDownButton, SIGNAL(clicked()), this, SLOT(onPriorityDownClicked()));
	connect(ui->priorityUpButton, SIGNAL(clicked()), this, SLOT(onPriorityUpClicked()));

	//Check when apps/domains become hidden
	connect(&stack, SIGNAL(hiddenDomainChanged(QString,bool)), this, SLOT(onHiddenDomainChanged()));
	connect(&stack, SIGNAL(hiddenProgramChanged(QString,bool)), this, SLOT(onHiddenProgramChanged()));

	//Current view mode (app/domain/rule)
	connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onViewModeChanged(int)));
	int viewMode=QSettings().value("ui_browserViewMode", DomainMode).toInt();
	ui->comboBox->setCurrentIndex(ui->comboBox->findData(viewMode));
	setViewMode(viewMode);
}

BrowserDock::~BrowserDock()
{
	QSettings().setValue("ui_browserViewMode", viewMode());
    delete ui;
}

void BrowserDock::changeEvent(QEvent *e)
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

void BrowserDock::setViewMode(int mode)
{
	bool change=true;

	switch(mode)
	{
	case ApplicationMode:
		ui->domainButtonsContainer->hide();
		ui->ruleButtonsContainer->hide();
		ui->appButtonsContainer->show();
		updateApplicationsList();
		break;
	case DomainMode:
		ui->appButtonsContainer->hide();
		ui->ruleButtonsContainer->hide();
		ui->domainButtonsContainer->show();
		updateDomainsList();
		break;
	case RuleMode:
		ui->appButtonsContainer->hide();
		ui->domainButtonsContainer->hide();
		ui->ruleButtonsContainer->show();
		updateRulesList();
		break;
	default:
		change=false;
		qWarning("Invalid view mode %d for the Browser dock", mode);
		break;
	}

	if(change)
	{
		_view_mode=mode;
		emit viewModeChanged(mode);
	}
}

int BrowserDock::viewMode() const
{
	return _view_mode;
}

void BrowserDock::updateApplicationsList()
{
	disconnect(ui->listWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onListItemChanged(QListWidgetItem*)));
	ui->listWidget->clear();
	ui->listWidget->show();
	_abs_rules_tree.hide();

	QList<Program> list = ConfigurationStack::instance().current().programList();
	QList<Program>::const_iterator it;

	for(it=list.begin(); it!=list.end(); ++it)
	{
		const Program &prog = *it;

		QPixmap icon(prog.icon());
		if(icon.isNull())
			icon.load(":/icons/default-program");

		QListWidgetItem *item = new QListWidgetItem(QIcon(icon), prog.displayName(), ui->listWidget);
		item->setData(Qt::UserRole, prog.name());

		if(ConfigurationStack::instance().isProgramHidden(prog.name()))
			item->setCheckState(Qt::Unchecked);
		else
			item->setCheckState(Qt::Checked);
	}

	connect(ui->listWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onListItemChanged(QListWidgetItem*)));
}

void BrowserDock::updateDomainsList()
{
	disconnect(ui->listWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onListItemChanged(QListWidgetItem*)));
	ui->listWidget->clear();
	ui->listWidget->show();
	_abs_rules_tree.hide();

	QList<Domain> list = ConfigurationStack::instance().current().domainList();
	QList<Domain>::const_iterator it;

	for(it=list.begin(); it!=list.end(); ++it)
	{
		const Domain &dom = *it;

		QPixmap icon(dom.iconPath());
		if(icon.isNull())
			icon.load(":/icons/default-domain");

		QListWidgetItem *item = new QListWidgetItem(QIcon(icon), dom.displayName(), ui->listWidget);
		item->setData(Qt::UserRole, dom.name());

		if(ConfigurationStack::instance().isDomainHidden(dom.name()))
			item->setCheckState(Qt::Unchecked);
		else
			item->setCheckState(Qt::Checked);
	}

	connect(ui->listWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onListItemChanged(QListWidgetItem*)));
}

void BrowserDock::updateRulesList()
{
	ui->listWidget->hide();
	_abs_rules_tree.show();
	_abs_rules_tree.updateRulesList();
}

void BrowserDock::onViewModeChanged(int index)
{
	setViewMode(ui->comboBox->itemData(index, Qt::UserRole).toInt());
}

void BrowserDock::onCurrentSelectionChanged()
{
	bool selected;

	//Use the selection of the currently shown widget
	if(ui->listWidget->isHidden())
		selected=_abs_rules_tree.hasSelection();
	else
		selected=(ui->listWidget->selectedItems().size() > 0);

	if(_view_mode==ApplicationMode)
	{
		ui->appEditButton->setEnabled(selected);
		ui->appCloseButton->setEnabled(selected);
		ui->appZoomButton->setEnabled(selected);
	}
	else if(_view_mode==DomainMode)
	{
		ui->domainDeleteButton->setEnabled(selected);
		ui->domainEditButton->setEnabled(selected);
		ui->domainZoomButton->setEnabled(selected);
	}
	else if(_view_mode==RuleMode)
	{
		ui->ruleDeleteButton->setEnabled(selected);
		ui->ruleEditButton->setEnabled(selected);
		ui->priorityDownButton->setEnabled(selected && (_abs_rules_tree.nextRuleId() != AbstractRule::InvalidPriority));
		ui->priorityUpButton->setEnabled(selected && (_abs_rules_tree.previousRuleId() != AbstractRule::InvalidPriority));
	}
}

void BrowserDock::onDomainChanged()
{
	if(_view_mode==DomainMode)
		updateDomainsList();
}

void BrowserDock::onProgramsChanged()
{
	if(_view_mode==ApplicationMode)
		updateApplicationsList();
	else if(_view_mode==RuleMode)
		updateRulesList();
}

void BrowserDock::onAddProgram()
{
	AppDialog dialog(this);

	dialog.show();
	dialog.exec();

	if(dialog.result()==AppDialog::Created)
		ConfigurationStack::instance().addProgram(dialog.fetchProgram());
}

void BrowserDock::onAppEditClicked()
{
	if(_view_mode!=ApplicationMode)
		return;

	Program prog=ConfigurationStack::instance().current().getProgramByName(ui->listWidget->currentItem()->data(Qt::UserRole).toString());
	AppDialog dialog(prog, this);

	dialog.show();
	dialog.exec();

	if(dialog.result()==AppDialog::Edited)
		ConfigurationStack::instance().updateProgram(prog.name(), dialog.fetchProgram());
}

void BrowserDock::onAppCloseClicked()
{
	if(_view_mode!=ApplicationMode)
		return;

	ConfigurationStack::instance().deleteProgram(ui->listWidget->currentItem()->data(Qt::UserRole).toString());
}

void BrowserDock::onAppFocusClicked()
{
	if(_view_mode!=ApplicationMode)
		return;

	emit applicationFocused(ui->listWidget->currentItem()->data(Qt::UserRole).toString());
}

void BrowserDock::onAddDomain()
{
	DomainDialog dialog;

	dialog.show();
	dialog.exec();

	if(dialog.result() == DomainDialog::Created)
		ConfigurationStack::instance().currentForModifying().addDomain(dialog.retrieveDomain(), dialog.defaultDomainChecked());
}

void BrowserDock::onDomainDeleteClicked()
{
	if(_view_mode!=DomainMode)
		return;

	ConfigurationStack &stack=ConfigurationStack::instance();
	QString domName=ui->listWidget->currentItem()->data(Qt::UserRole).toString();

	//If the deleted domain was the default one, and if there is more than one domain left
	if(stack.current().defaultDomain().name()==domName && stack.current().domainCount()>1)
	{

		SelectDefaultDomainDialog dialog(this->window());
		dialog.show();
		dialog.exec();

		if(dialog.result()==QDialog::Accepted)
			stack.currentForModifying().deleteDomain(domName, dialog.newDefaultDomainName());
	}
	else
		stack.currentForModifying().deleteDomain(domName);
}

void BrowserDock::onDomainEditClicked()
{
	if(_view_mode!=DomainMode)
		return;

	int index=ui->listWidget->currentRow();
	DomainDialog::editDomain(ui->listWidget->currentItem()->data(Qt::UserRole).toString());
	ui->listWidget->setCurrentRow(index);
}

void BrowserDock::onDomainFocusClicked()
{
	if(_view_mode!=DomainMode)
		return;

	emit domainFocused(ui->listWidget->currentItem()->data(Qt::UserRole).toString());
}

void BrowserDock::onAddRule()
{
	//Ask for which kind of rule to create
	QMessageBox box(QMessageBox::Question, "New Transition Rule", "What kind of transition rule to you want to add?", QMessageBox::Cancel);
	QPushButton *ruleButton = box.addButton("&Transition Rule", QMessageBox::NoRole);
	QPushButton *loopButton = box.addButton("&Loop", QMessageBox::YesRole);
	box.button(QMessageBox::Cancel)->hide();
	box.setEscapeButton(QMessageBox::Cancel);
	box.show();
	box.exec();

	//Create and show a loop/rule dialog
	AbstractRuleDialog *dialog;
	if(box.clickedButton() == loopButton)
		dialog = new LoopDialog();
	else if(box.clickedButton() == ruleButton)
		dialog = new RuleDialog();
	else
		return;

	dialog->show();
	dialog->exec();

	//Create and fill the rule if the user pressed ok
	if(dialog->result() == AbstractRuleDialog::Created)
	{
		AbstractRule *newRule;

		//The type of rule depends on the button that was clicked
		if(box.clickedButton() == loopButton)
		{
			newRule = new Loop(dialog->program(), dialog->displayName());
		} else if(box.clickedButton() == ruleButton)
		{
			RuleDialog *rule_dialog = dynamic_cast<RuleDialog *>(dialog);
			newRule = new Rule(dialog->program(), rule_dialog->targetDomain(), dialog->displayName());
		}

		newRule->setNotify(dialog->notify());
		newRule->setPrompt(dialog->prompt());

		QList<MRule> mrules = dialog->matchingRules();
		QList<MRule>::iterator it;

		for(it=mrules.begin(); it!=mrules.end(); ++it)
			newRule->addMRule(*it);

		QList<TRule> trules = dialog->domains();
		QList<TRule>::iterator domit;

		for(domit=trules.begin(); domit!=trules.end(); ++domit)
			newRule->addTRule(*domit);

		if(box.clickedButton() == loopButton)
		{
			Loop *loop = dynamic_cast<Loop *>(newRule);
			ConfigurationStack::instance().currentForModifying().addLoop(*loop);
		} else if(box.clickedButton() == ruleButton)
		{
			Rule *rule = dynamic_cast<Rule *>(newRule);
			ConfigurationStack::instance().currentForModifying().addRule(*rule);
		}

		delete newRule;
	}

	delete dialog;
}

void BrowserDock::onUpdateDock()
{
	if(_view_mode==ApplicationMode)
		updateApplicationsList();
	else if(_view_mode==DomainMode)
		updateDomainsList();
	else if(_view_mode==RuleMode)
		updateRulesList();
}

void BrowserDock::onRuleDeleteClicked()
{
	if(_view_mode!=RuleMode)
		return;

	if(_abs_rules_tree.currentRuleType() == Loop().ruleType())
		ConfigurationStack::instance().currentForModifying().deleteLoop(_abs_rules_tree.currentRuleId());
	else if(_abs_rules_tree.currentRuleType() == Rule().ruleType())
		ConfigurationStack::instance().currentForModifying().deleteRule(_abs_rules_tree.currentRuleId());
}

void BrowserDock::onRuleEditClicked()
{
	if(_view_mode!=RuleMode)
		return;

	_abs_rules_tree.onEditCurrentItem();
}

void BrowserDock::onPriorityDownClicked()
{
	if(_view_mode!=RuleMode)
		return;

	int index=_abs_rules_tree.currentIndex();
	ConfigurationStack::instance().currentForModifying().swapPriorities(_abs_rules_tree.currentRuleId(), _abs_rules_tree.nextRuleId());
	_abs_rules_tree.selectIndex(index+1);
}

void BrowserDock::onPriorityUpClicked()
{
	if(_view_mode!=RuleMode)
		return;

	int index=_abs_rules_tree.currentIndex();
	ConfigurationStack::instance().currentForModifying().swapPriorities(_abs_rules_tree.currentRuleId(), _abs_rules_tree.previousRuleId());
	_abs_rules_tree.selectIndex(index-1);
}

void BrowserDock::onItemActivated(QListWidgetItem *item)
{
	if(_view_mode == ApplicationMode)
		emit programActivated(item->data(Qt::UserRole).toString());
	else if(_view_mode == DomainMode)
		emit domainActivated(item->data(Qt::UserRole).toString());
}

void BrowserDock::onListItemChanged(QListWidgetItem *item)
{
	ConfigurationStack &stack=ConfigurationStack::instance();

	if(_view_mode == ApplicationMode)
	{
		if(item->checkState()==Qt::Checked)
			stack.removeHiddenProgram(item->data(Qt::UserRole).toString());
		else
			stack.addHiddenProgram(item->data(Qt::UserRole).toString());
	}
	else if(_view_mode == DomainMode)
	{
		if(item->checkState()==Qt::Checked)
			stack.removeHiddenDomain(item->data(Qt::UserRole).toString());
		else
			stack.addHiddenDomain(item->data(Qt::UserRole).toString());
	}
}

void BrowserDock::onHiddenDomainChanged()
{
	if(_view_mode == DomainMode)
		updateDomainsList();
}

void BrowserDock::onHiddenProgramChanged()
{
	if(_view_mode == ApplicationMode)
		updateApplicationsList();
}
