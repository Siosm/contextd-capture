#include "sortedrulestreewidget.h"
#include "ui_sortedrulestreewidget.h"
#include "configurationstack.h"
#include "editorconfig.h"
#include "loopdialog.h"
#include "ruledialog.h"

SortedRulesTreeWidget::SortedRulesTreeWidget(QWidget *parent) :
    QWidget(parent),
	ui(new Ui::SortedRulesTreeWidget),
	_sort_mode(SortedRulesTreeWidget::SortByApp)
{
    ui->setupUi(this);

	//Set resize mode of headers
	ui->rulesList->header()->setResizeMode(0, QHeaderView::ResizeToContents);
	ui->rulesList->header()->setResizeMode(1, QHeaderView::ResizeToContents);

	//Connect sorting buttons to their handlers
	connect(ui->appSortButton, SIGNAL(clicked()), this, SLOT(onSortByAppClicked()));
	connect(ui->domSortButton, SIGNAL(clicked()), this, SLOT(onSortByDomainClicked()));
	onSortByAppClicked();

	//Connect handler for activation of items
	connect(ui->rulesList, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(onItemActivated(QTreeWidgetItem*,int)));
}

SortedRulesTreeWidget::~SortedRulesTreeWidget()
{
    delete ui;
}

void SortedRulesTreeWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
	switch (e->type())
	{
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void SortedRulesTreeWidget::addRule(const AbstractRule *rule, const Domain dom)
{
	QTreeWidgetItem *topItem=NULL, *secondItem=NULL;

	//Look for the top-level item (either the rule's application or domain)
	for(int i=0, count=ui->rulesList->topLevelItemCount(); i<count && !topItem; ++i)
	{
		QTreeWidgetItem *tmp=ui->rulesList->topLevelItem(i);

		if(((_sort_mode==SortByApp) && (rule->program().name()==tmp->data(0, Qt::UserRole).toString())) ||
		   ((_sort_mode==SortByDomain) && (dom.name()==tmp->data(0, Qt::UserRole).toString())))
			topItem=tmp;
	}

	//If there was no top-level item, create one
	if(!topItem)
	{
		if(_sort_mode==SortByApp)
		{
			topItem=new QTreeWidgetItem(ui->rulesList, QStringList(rule->program().displayName()));
			topItem->setData(0, ItemTypeRole, ProgramItem);
			topItem->setData(0, Qt::UserRole, rule->program().name());
			topItem->setFirstColumnSpanned(true);

			QPixmap icon(rule->program().icon());
			if(icon.isNull())
				icon.load(":/icons/default-program");

			topItem->setIcon(0, QIcon(icon));
		}
		else
		{
			topItem=new QTreeWidgetItem(ui->rulesList, QStringList(dom.displayName()));
			topItem->setData(0, ItemTypeRole, DomainItem);
			topItem->setData(0, Qt::UserRole, dom.name());
			topItem->setFirstColumnSpanned(true);

			QPixmap icon(dom.iconPath());
			if(icon.isNull())
				icon.load(":/icons/default-domain");

			topItem->setIcon(0, QIcon(icon));
		}

	}

	//Check if it has the child item we want (respectively domain or application)
	for(int i=0, count=topItem->childCount(); i<count && !secondItem; ++i)
	{
		QTreeWidgetItem *tmp=topItem->child(i);

		if(((_sort_mode==SortByDomain) && (rule->program().name()==tmp->data(0, Qt::UserRole).toString())) ||
		   ((_sort_mode==SortByApp) && (dom.name()==tmp->data(0, Qt::UserRole).toString())))
			secondItem=tmp;
	}

	//Create one if it didn't exist
	if(!secondItem)
	{
		if(_sort_mode==SortByApp)
		{
			secondItem=new QTreeWidgetItem(topItem, QStringList(dom.displayName()));
			secondItem->setData(0, ItemTypeRole, DomainItem);
			secondItem->setData(0, Qt::UserRole, dom.name());
			secondItem->setFirstColumnSpanned(true);

			QPixmap icon(dom.iconPath());
			if(icon.isNull())
				icon.load(":/icons/default-domain");

			secondItem->setIcon(0, QIcon(icon));
		}
		else
		{
			secondItem=new QTreeWidgetItem(topItem, QStringList(rule->program().displayName()));
			secondItem->setData(0, ItemTypeRole, ProgramItem);
			secondItem->setData(0, Qt::UserRole, rule->program().name());
			secondItem->setFirstColumnSpanned(true);

			QPixmap icon(rule->program().icon());
			if(icon.isNull())
				icon.load(":/icons/default-program");

			secondItem->setIcon(0, QIcon(icon));
		}
	}

	//Add our rule to the second-level item
	QStringList itemStrList(rule->displayName());
	itemStrList.append(rule->ruleType());
	QTreeWidgetItem *ruleItem= new QTreeWidgetItem(secondItem, itemStrList);
	ruleItem->setData(0, ItemTypeRole, RuleItem);
	ruleItem->setData(0, Qt::UserRole, rule->id());
	ruleItem->setData(1, Qt::UserRole, rule->ruleType());

	//If the rule has warnings, display a warning icon
	if(ConfigurationStack::instance().ruleHasWarnings(rule->id()))
		ruleItem->setIcon(0, QIcon(":/icons/warning"));

	//Finally, display the matching rules of the rule
	QVector<MRule> mrules=rule->mRules();
	for(int i=0; i<mrules.count(); ++i)
	{
		QTreeWidgetItem *mruleItem=new QTreeWidgetItem(ruleItem, QStringList(mrules[i].displayName()));
		mruleItem->setData(0, ItemTypeRole, MRuleItem);
		mruleItem->setFirstColumnSpanned(true);
		mruleItem->setToolTip(0, mrules[i].displayName());

		//Add the conditions of the matching rule
		const QMap<QString, QString> &map=mrules[i].conditions();
		for(QMap<QString, QString>::const_iterator it=map.begin(); it!=map.end(); ++it)
		{
			QStringList itemStrList(it.key());
			itemStrList.append(it.value());
			QTreeWidgetItem *condItem=new QTreeWidgetItem(mruleItem, itemStrList);
			condItem->setData(0, ItemTypeRole, MRuleConditionItem);
			condItem->setToolTip(0, it.key());
			condItem->setToolTip(1, it.value());
		}
	}
}

void SortedRulesTreeWidget::clearRules()
{
	ui->rulesList->clear();
}

void SortedRulesTreeWidget::onSortByAppClicked()
{
	if(_sort_mode==SortByApp)
		return;

	ui->rulesList->headerItem()->setText(0, "App/Domain/Rule");
	_sort_mode=SortByApp;

	//A map where we will store new items for each domain
	QMap<QString, QTreeWidgetItem *> domains;

	//Go through all application QTreeWidgetItems and create new items for each met domain
	while(ui->rulesList->topLevelItemCount() > 0)
	{
		QTreeWidgetItem *topItem=ui->rulesList->takeTopLevelItem(0);
		QIcon domIcon=topItem->icon(0);
		QString domName=topItem->data(0, Qt::UserRole).toString();
		QString domDisplayName=topItem->text(0);

		//Go through each domain, add the application to the domain's new item, and transfer the rules to the new item
		for(int i=0, count=topItem->childCount(); i<count; ++i)
		{
			QTreeWidgetItem *oldProgItem=topItem->child(i);
			QIcon progIcon=oldProgItem->icon(0);
			QString progName=oldProgItem->data(0, Qt::UserRole).toString();
			QString progDisplayName=oldProgItem->text(0);

			//Fetch the new domain QTreeWidgetItem
			QTreeWidgetItem *newItem;
			if(domains.contains(progName))
			{
				newItem=domains[progName];
			}
			else
			{
				domains[progName]=newItem=new QTreeWidgetItem(QStringList(progDisplayName));
				newItem->setData(0, ItemTypeRole, ProgramItem);
				newItem->setData(0, Qt::UserRole, progName);
				newItem->setFirstColumnSpanned(true);
				newItem->setIcon(0, progIcon);
			}

			//Add the application to the new domain item if it's not there already
			QTreeWidgetItem *appItem=NULL;
			for(int i=0, count=newItem->childCount(); i<count && !appItem; ++i)
			{
				QTreeWidgetItem *tmp=newItem->child(i);

				if(domName==tmp->data(0, Qt::UserRole).toString())
					appItem=tmp;
			}

			if(!appItem)
			{
				appItem=new QTreeWidgetItem(newItem, QStringList(domDisplayName));
				appItem->setData(0, ItemTypeRole, DomainItem);
				appItem->setData(0, Qt::UserRole, domName);
				appItem->setFirstColumnSpanned(true);
				appItem->setIcon(0, domIcon);
			}

			//Transfer all the rule items to the new application item
			while(oldProgItem->childCount() > 0)
				appItem->addChild(oldProgItem->takeChild(0));
		}
	}

	//Add the new items to the now empty rules list
	QMap<QString, QTreeWidgetItem *>::const_iterator it;
	for(it=domains.begin(); it!=domains.end(); ++it)
		ui->rulesList->addTopLevelItem(it.value());
}

void SortedRulesTreeWidget::onSortByDomainClicked()
{
	if(_sort_mode==SortByDomain)
		return;

	ui->rulesList->headerItem()->setText(0, "Domain/App/Rule");
	_sort_mode=SortByDomain;

	//A map where we will store new items for each application
	QMap<QString, QTreeWidgetItem *> programs;

	//Go through all application QTreeWidgetItems and create new items for each met application
	while(ui->rulesList->topLevelItemCount() > 0)
	{
		QTreeWidgetItem *topItem=ui->rulesList->takeTopLevelItem(0);
		QIcon progIcon=topItem->icon(0);
		QString progName=topItem->data(0, Qt::UserRole).toString();
		QString progDisplayName=topItem->text(0);

		//Go through each application, add the application to the app's new item, and transfer the rules to the new item
		for(int i=0, count=topItem->childCount(); i<count; ++i)
		{
			QTreeWidgetItem *oldDomItem=topItem->child(i);
			QIcon domIcon=oldDomItem->icon(0);
			QString domName=oldDomItem->data(0, Qt::UserRole).toString();
			QString domDisplayName=oldDomItem->text(0);

			//Fetch the new application QTreeWidgetItem
			QTreeWidgetItem *newItem;
			if(programs.contains(domName))
			{
				newItem=programs[domName];
			}
			else
			{
				programs[domName]=newItem=new QTreeWidgetItem(QStringList(domDisplayName));
				newItem->setData(0, ItemTypeRole, DomainItem);
				newItem->setData(0, Qt::UserRole, domName);
				newItem->setFirstColumnSpanned(true);
				newItem->setIcon(0, domIcon);
			}

			//Add the domain to the new application item if it's not there already
			QTreeWidgetItem *domItem=NULL;
			for(int i=0, count=newItem->childCount(); i<count && !domItem; ++i)
			{
				QTreeWidgetItem *tmp=newItem->child(i);

				if(progName==tmp->data(0, Qt::UserRole).toString())
					domItem=tmp;
			}

			if(!domItem)
			{
				domItem=new QTreeWidgetItem(newItem, QStringList(progDisplayName));
				domItem->setData(0, ItemTypeRole, ProgramItem);
				domItem->setData(0, Qt::UserRole, progName);
				domItem->setFirstColumnSpanned(true);
				domItem->setIcon(0, progIcon);
			}

			//Transfer all the rule items to the new domain item
			while(oldDomItem->childCount() > 0)
				domItem->addChild(oldDomItem->takeChild(0));
		}
	}

	//Add the new items to the now empty rules list
	QMap<QString, QTreeWidgetItem *>::const_iterator it;
	for(it=programs.begin(); it!=programs.end(); ++it)
		ui->rulesList->addTopLevelItem(it.value());
}

void SortedRulesTreeWidget::onItemActivated(QTreeWidgetItem *item, int /*column*/)
{
//	bool editRule=false;

	//Select the item on which we're going to work
	switch(item->data(0, ItemTypeRole).toInt())
	{
	case MRuleConditionItem:
		item=item->parent();
	case MRuleItem:
		item=item->parent();

	case RuleItem:
		emit ruleActivated(item->data(0, Qt::UserRole).toInt());
//		editRule=true;
		break;

	case DomainItem:
		emit domainActivated(item->data(0, Qt::UserRole).toString());
		break;

	case ProgramItem:
		emit programActivated(item->data(0, Qt::UserRole).toString());
		break;
	}

	//If the item is a Rule
//	if(editRule)
//	{
//		if(item->data(1, Qt::UserRole).toString()==Loop().ruleType())
//			LoopDialog::editLoop(item->data(0, Qt::UserRole).toInt());
//		else if(item->data(1, Qt::UserRole).toString()==Rule().ruleType())
//			RuleDialog::editRule(item->data(0, Qt::UserRole).toInt());
//	}
}
