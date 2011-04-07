#include "abstractruletreewidget.h"
#include "ui_abstractruletreewidget.h"
#include "configurationstack.h"
#include "editorconfig.h"
#include "loop.h"
#include "loopdialog.h"
#include "mruledialog.h"
#include "rule.h"
#include "ruledialog.h"

AbstractRuleTreeWidget::AbstractRuleTreeWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AbstractRuleTreeWidget),
	_filtered(false)
{
	initWidget();
}

AbstractRuleTreeWidget::AbstractRuleTreeWidget(bool filtered, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AbstractRuleTreeWidget),
	_filtered(filtered)
{
	initWidget();
}

void AbstractRuleTreeWidget::initWidget()
{
	ui->setupUi(this);
	ConfigurationStack &stack=ConfigurationStack::instance();

	//In unfiltered mode, we add icons for showing/hiding the rules, and we watch for shown/hidden rules
	if(!_filtered)
	{
		ui->treeWidget->headerItem()->setIcon(0, QIcon(":/icons/eye"));
		connect(&stack, SIGNAL(hiddenRuleChanged(int,bool)), this, SLOT(onHiddenRuleChanged()));
	}

	//Resizing of the columns
	ui->treeWidget->header()->setResizeMode(0, QHeaderView::ResizeToContents);
	ui->treeWidget->header()->setResizeMode(1, QHeaderView::ResizeToContents);

	//Let parents know of selection changes
	connect(ui->treeWidget, SIGNAL(itemSelectionChanged()), this, SIGNAL(itemSelectionChanged()));
	connect(ui->treeWidget, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(onItemActivated(QTreeWidgetItem*,int)));

	//Watch for changes in the stack
	connect(&stack, SIGNAL(configurationChanged()), this, SLOT(onProgramsOrRulesChanged()));
	connect(&stack, SIGNAL(allRulesChanged()), this, SLOT(onProgramsOrRulesChanged()));
	connect(&stack, SIGNAL(loopsChanged()), this, SLOT(onProgramsOrRulesChanged()));
	connect(&stack, SIGNAL(rulesChanged()), this, SLOT(onProgramsOrRulesChanged()));
	connect(&stack, SIGNAL(programChanged(QString,QString)), this, SLOT(onProgramsOrRulesChanged()));
	connect(&stack, SIGNAL(warningsChanged()), this, SLOT(onProgramsOrRulesChanged()));
}

AbstractRuleTreeWidget::~AbstractRuleTreeWidget()
{
	delete ui;
}

void AbstractRuleTreeWidget::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch(e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void AbstractRuleTreeWidget::setProgram(Program prog)
{
	if(prog.isValid())
	{
		_program=prog;
		updateRulesList();
	}
}

Program AbstractRuleTreeWidget::program() const
{
	return _program;
}

void AbstractRuleTreeWidget::setRuleType(QString type)
{
	if((type==Rule().ruleType()) ||(type==Loop().ruleType()))
	{
		_rule_type=type;
		updateRulesList();
	}
}

QString AbstractRuleTreeWidget::ruleType() const
{
	return _rule_type;
}

int AbstractRuleTreeWidget::currentIndex() const
{
	return ui->treeWidget->indexOfTopLevelItem(ui->treeWidget->currentItem());
}

void AbstractRuleTreeWidget::selectIndex(int index)
{
	ui->treeWidget->setCurrentItem(ui->treeWidget->topLevelItem(index));
}

int AbstractRuleTreeWidget::currentRuleId() const
{
	//Check if there is a selection
	QList<QTreeWidgetItem *> selected=ui->treeWidget->selectedItems();
	if(selected.size()==0)
		return AbstractRule::InvalidId;

	//Get the top level item for the selected item
	QTreeWidgetItem *top=selected.at(0);
	while(top->parent())
		top=top->parent();

	//Return the id
	return top->data(0, Qt::UserRole).toInt();
}

QString AbstractRuleTreeWidget::currentRuleType() const
{
	//If there is a rule type filter, return it directly
	if(_filtered)
			return _rule_type;

	//Else check if there is a selection
	QList<QTreeWidgetItem *> selected=ui->treeWidget->selectedItems();
	if(selected.size()==0)
		return "";

	//Get the top level item for the selected item
	QTreeWidgetItem *top=selected.at(0);
	while(top->parent())
		top=top->parent();

	//Return the rule type
	return top->data(1, Qt::UserRole).toString();
}

bool AbstractRuleTreeWidget::hasSelection() const
{
	return !ui->treeWidget->selectedItems().isEmpty();
}

int AbstractRuleTreeWidget::previousRuleId() const
{
	//Check if there is a selection
	QList<QTreeWidgetItem *> selected=ui->treeWidget->selectedItems();
	if(selected.size()==0)
		return AbstractRule::InvalidId;

	//Get the top level item for the selected item
	QTreeWidgetItem *top=selected.at(0);
	while(top->parent())
		top=top->parent();

	//Get the index of the previous element
	int index=ui->treeWidget->indexOfTopLevelItem(top) - 1;

	//If the index is out of bounds, return an invalid id
	if(index < 0)
		return AbstractRule::InvalidId;
	else
		return ui->treeWidget->topLevelItem(index)->data(0, Qt::UserRole).toInt();
}

int AbstractRuleTreeWidget::nextRuleId() const
{
	//Check if there is a selection
	QList<QTreeWidgetItem *> selected=ui->treeWidget->selectedItems();
	if(selected.size()==0)
		return AbstractRule::InvalidId;

	//Get the top level item for the selected item
	QTreeWidgetItem *top=selected.at(0);
	while(top->parent())
		top=top->parent();

	//Get the index of the next element
	int index=ui->treeWidget->indexOfTopLevelItem(top) + 1;

	//If the index is out of bounds, return an invalid id
	if(index >= ui->treeWidget->topLevelItemCount())
		return AbstractRule::InvalidId;
	else
		return ui->treeWidget->topLevelItem(index)->data(0, Qt::UserRole).toInt();
}

void AbstractRuleTreeWidget::updateRulesList()
{
	//Stop checking for changes to items as it will have side effects
	disconnect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(onItemChanged(QTreeWidgetItem*,int)));
	ui->treeWidget->clear();

	if(!_filtered)
		ui->treeWidget->headerItem()->setText(1, "Type/Value");
	else
		ui->treeWidget->headerItem()->setText(1, "Value");

	//A vector of abstract rules we'll fill depending on the filters
	QVector<AbstractRulePtr> abstracts;

	//Fill with loops only if there is no filter, or if the filter allows loops
	if(!_filtered || _rule_type==Loop().ruleType())
	{
		QVector<Loop> loops=ConfigurationStack::instance().current().allLoops();

		for(int i=0; i<loops.size(); ++i)
		{
			const Loop &loop=loops[i];

			//Only add loops that have the good program, if filtered
			if(!_filtered || _program==loop.program())
				abstracts.append(AbstractRulePtr(new Loop(loop)));
		}
	}

	//Fill with rules only if there is no filter, or if the filter allows rules
	if(!_filtered || _rule_type==Rule().ruleType())
	{
		QVector<Rule> rules=ConfigurationStack::instance().current().allRules();

		for(int i=0; i<rules.size(); ++i)
		{
			const Rule &rule=rules[i];

			//Only add rules that have the good program, if filtered
			if(!_filtered || _program==rule.program())
				abstracts.append(AbstractRulePtr(new Rule(rule)));
		}
	}

	//Sort the rules list by priority
	qSort(abstracts.begin(), abstracts.end(), AbstractRule::hasHigherPriority);

	//Go through the valid abstract rules
	for(int i=0; i<abstracts.size(); ++i)
	{
		AbstractRulePtr rule=abstracts[i];

		//Create the item for the rule (add it's type if not filtered)
		QStringList itemStrList(rule->displayName());
		if(!_filtered)
				itemStrList.append(rule->ruleType());
		QTreeWidgetItem *item=new QTreeWidgetItem(ui->treeWidget, itemStrList);
		item->setData(0, ItemTypeRole, RuleItem);
		item->setData(0, Qt::UserRole, rule->id());
		item->setData(1, Qt::UserRole, rule->ruleType());
		item->setFirstColumnSpanned(_filtered);

		//If the rule has warnings, display a warning icon
		if(ConfigurationStack::instance().ruleHasWarnings(rule->id()))
			item->setIcon(0, QIcon(":/icons/warning"));

		//In unfiltered mode, set the checkstate of items depending on whether the rules are hidden
		if(!_filtered)
		{
			if(ConfigurationStack::instance().isRuleHidden(rule->id()))
				item->setCheckState(0, Qt::Unchecked);
			else
				item->setCheckState(0, Qt::Checked);
		}

		//Put a child for each matching rule
		QVector<MRule> mrules=rule->mRules();
		for(QVector<MRule>::const_iterator mrulesIt=mrules.begin(); mrulesIt!=mrules.end(); ++mrulesIt)
		{
			const MRule mrule=*mrulesIt;
			QTreeWidgetItem *mruleItem=new QTreeWidgetItem(item, QStringList(mrule.displayName()));
			mruleItem->setData(0, ItemTypeRole, MRuleItem);
			mruleItem->setFirstColumnSpanned(true);

			//The matching rule has its conditions as children
			const QMap<QString, QString> &map=mrule.conditions();
			QMap<QString, QString>::const_iterator it;
			for(it=map.begin(); it!=map.end(); ++it)
			{
				QStringList itemStrList(it.key());
				itemStrList.append(it.value());
				QTreeWidgetItem *condItem=new QTreeWidgetItem(mruleItem, itemStrList);
				condItem->setData(0, ItemTypeRole, MRuleConditionItem);
				condItem->setToolTip(0, it.key());
				condItem->setToolTip(1, it.value());
			}
		}

		//Create a list of domains for the rule
		QStringList domItemStrList;
		if(rule->ruleType()==Rule().ruleType())
			domItemStrList.append("Source domains");
		else if(rule->ruleType()==Loop().ruleType())
			domItemStrList.append("Domains");

		QStringList domainNames;
		QVector<TRule> trules=rule->tRules();
		QVector<TRule>::const_iterator trulesIt;

		for(trulesIt=trules.begin(); trulesIt!=trules.end(); ++trulesIt)
		{
			Domain dom=ConfigurationStack::instance().current().getDomainByName((*trulesIt).fromDomain());
			if(dom!=Domain())
				domainNames.append(ConfigurationStack::instance().current().getDomainByName((*trulesIt).fromDomain()).displayName());
			else
				domainNames.append((*trulesIt).fromDomain());
		}

		//Create an item for the list of domains
		qSort(domainNames.begin(), domainNames.end());
		domItemStrList.append(domainNames.join(", "));
		QTreeWidgetItem *domItem=new QTreeWidgetItem(domItemStrList);
		domItem->setData(0, ItemTypeRole, SourceDomainItem);
		domItem->setToolTip(1, domainNames.join(", "));
		item->insertChild(0, domItem);

		//If the rule is a Transition Rule, add the target domain
		if(!rule.dynamicCast<Rule>().isNull())
		{
			RulePtr childRule=rule.dynamicCast<Rule>();
			//Create an item for the target domain
			QStringList targetStrList("Target domain");
			targetStrList.append(childRule->transitTo().displayName());

			QTreeWidgetItem *targetItem=new QTreeWidgetItem(targetStrList);
			targetItem->setData(0, ItemTypeRole, TargetDomainItem);
			targetItem->setToolTip(1, childRule->transitTo().displayName());
			item->insertChild(1, targetItem);
		}

		//Create an item for notify, and one for prompt
		QStringList promptStrList("Prompt User");
		promptStrList.append(rule->prompt() ? "Yes" : "No");
		QTreeWidgetItem *promptItem=new QTreeWidgetItem(promptStrList);
		promptItem->setData(0, ItemTypeRole, PromptItem);
		item->insertChild(0, promptItem);

		QStringList notifyStrList("Notify Transition");
		notifyStrList.append(rule->notify() ? "Yes" : "No");
		QTreeWidgetItem *notifyItem=new QTreeWidgetItem(notifyStrList);
		notifyItem->setData(0, ItemTypeRole, NotifyItem);
		item->insertChild(0, notifyItem);

		//Create an item for the program if we don't filter by app
		if(!_filtered)
		{
			QStringList appStrList("Application");
			appStrList.append(rule->program().displayName());

			QTreeWidgetItem *appItem=new QTreeWidgetItem(appStrList);
			appItem->setData(0, ItemTypeRole, ProgramItem);
			appItem->setToolTip(1, rule->program().displayName());
			item->insertChild(0, appItem);
		}
	}

	//From now we can watch changes to items again
	connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(onItemChanged(QTreeWidgetItem*,int)));
}

void AbstractRuleTreeWidget::clearRulesList()
{
	ui->treeWidget->clear();
}

void AbstractRuleTreeWidget::onProgramsOrRulesChanged()
{
	updateRulesList();
}

void AbstractRuleTreeWidget::onHiddenRuleChanged()
{
	updateRulesList();
}

void AbstractRuleTreeWidget::onItemActivated(QTreeWidgetItem* item, int /*column*/)
{
	bool activatedChild=false;

	//Select the item on which we're going to work
	switch(item->data(0, ItemTypeRole).toInt())
	{
	case MRuleConditionItem:
		activatedChild=true;
		item=item->parent();

	case MRuleItem:
	case ProgramItem:
	case SourceDomainItem:
	case TargetDomainItem:
	case NotifyItem:
	case PromptItem:
		activatedChild=true;
		item=item->parent();

	case RuleItem:
		break;
	}

	//Send a signal to tell parents to open the Rule Info Dock
	emit ruleActivated(item->data(0, Qt::UserRole).toInt());
}

void AbstractRuleTreeWidget::onItemChanged(QTreeWidgetItem* item, int column)
{
	ConfigurationStack &stack=ConfigurationStack::instance();

	disconnect(&stack, SIGNAL(hiddenRuleChanged(int,bool)), this, SLOT(onHiddenRuleChanged()));

	if(column==0 && item->parent()==0)
	{
		ConfigurationStack &stack=ConfigurationStack::instance();

		if(item->checkState(0)==Qt::Checked)
			stack.removeHiddenRule(item->data(0, Qt::UserRole).toInt());
		else
			stack.addHiddenRule(item->data(0, Qt::UserRole).toInt());
	}

	connect(&stack, SIGNAL(hiddenRuleChanged(int,bool)), this, SLOT(onHiddenRuleChanged()));
}

void AbstractRuleTreeWidget::onEditCurrentItem()
{
	QList<QTreeWidgetItem*> selected=ui->treeWidget->selectedItems();

	if(selected.isEmpty())
		return;

	bool activatedChild=false;
	QTreeWidgetItem *item=selected.at(0);

	//Select the item on which we're going to work
	switch(item->data(0, ItemTypeRole).toInt())
	{
	case MRuleConditionItem:
		activatedChild=true;
		item=item->parent();

	case MRuleItem:
	case ProgramItem:
	case SourceDomainItem:
	case TargetDomainItem:
	case NotifyItem:
	case PromptItem:
		activatedChild=true;
		item=item->parent();

	case RuleItem:
		break;
	}

	//Remember the currently selected item
	int index=ui->treeWidget->indexOfTopLevelItem(item);

	//Edit the item's content
	if(item->data(1, Qt::UserRole).toString()==Loop().ruleType())
		LoopDialog::editLoop(item->data(0, Qt::UserRole).toInt());
	else if(item->data(1, Qt::UserRole).toString()==Rule().ruleType())
		RuleDialog::editRule(item->data(0, Qt::UserRole).toInt());

	//Select and expand the item if needed
	ui->treeWidget->setCurrentItem(ui->treeWidget->topLevelItem(index));
	if(activatedChild)
		ui->treeWidget->expandItem(ui->treeWidget->topLevelItem(index));
}
