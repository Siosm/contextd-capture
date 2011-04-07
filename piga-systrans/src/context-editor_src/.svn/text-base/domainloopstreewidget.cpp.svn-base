#include "domainloopstreewidget.h"
#include "ui_domainloopstreewidget.h"
#include "configurationstack.h"
#include "editorconfig.h"
#include "loopdialog.h"
#include "ruledialog.h"

DomainLoopsTreeWidget::DomainLoopsTreeWidget(QWidget *parent) :
    QWidget(parent),
	ui(new Ui::DomainLoopsTreeWidget)
{
    ui->setupUi(this);

	ui->rulesList->header()->setResizeMode(0, QHeaderView::ResizeToContents);
	ui->rulesList->header()->setResizeMode(1, QHeaderView::ResizeToContents);

	connect(ui->rulesList, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(onItemActivated(QTreeWidgetItem*,int)));
}

DomainLoopsTreeWidget::~DomainLoopsTreeWidget()
{
    delete ui;
}

void DomainLoopsTreeWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
	switch(e->type())
	{
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void DomainLoopsTreeWidget::addRule(const AbstractRule *rule)
{
	QTreeWidgetItem *topItem=NULL;

	//Look for the top-level item (which represents the rule's application)
	for(int i=0, count=ui->rulesList->topLevelItemCount(); i<count && !topItem; ++i)
	{
		QTreeWidgetItem *tmp=ui->rulesList->topLevelItem(i);

		if(rule->program().name() == tmp->data(0, Qt::UserRole).toString())
			topItem=tmp;
	}

	//Create one if it didn't exist
	if(!topItem)
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

	//Add our rule to the application item
	QStringList itemStrList(rule->displayName());
	itemStrList.append(rule->ruleType());
	QTreeWidgetItem *ruleItem= new QTreeWidgetItem(topItem, itemStrList);
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
		const MRule &mrule=mrules[i];
		QTreeWidgetItem *mruleItem=new QTreeWidgetItem(ruleItem, QStringList(mrule.displayName()));
		mruleItem->setData(0, ItemTypeRole, MRuleItem);
		mruleItem->setFirstColumnSpanned(true);
		mruleItem->setToolTip(0, mrule.displayName());

		//Fill the mrule's item with the mrule's conditions
		const QMap<QString, QString> &map=mrule.conditions();
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

void DomainLoopsTreeWidget::clearRules()
{
	ui->rulesList->clear();
}

void DomainLoopsTreeWidget::onItemActivated(QTreeWidgetItem *item, int /*column*/)
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
