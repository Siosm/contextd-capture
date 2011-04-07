#include "mruledialog.h"
#include "mruletreewidget.h"
#include "ui_mruletreewidget.h"

MRuleTreeWidget::MRuleTreeWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MRuleTreeWidget)
{
	ui->setupUi(this);

	//Various signals
	connect(ui->ruleAddButton, SIGNAL(clicked()), this, SLOT(onRuleAddClicked()));
	connect(ui->ruleEditButton, SIGNAL(clicked()), this, SLOT(onRuleEditClicked()));
	connect(ui->ruleRemoveButton, SIGNAL(clicked()), this, SLOT(onRuleRemoveClicked()));
	connect(ui->rulesTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(onRuleViewSelectionChanged()));

	//Make sure buttons are properly enabled - ruleAdd will be available once a Program has been set
	ui->ruleAddButton->setEnabled(false);
	onRuleViewSelectionChanged();
}

MRuleTreeWidget::~MRuleTreeWidget()
{
    delete ui;
}

void MRuleTreeWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

int MRuleTreeWidget::ruleCount() const
{
	return ui->rulesTreeWidget->topLevelItemCount();
}

void MRuleTreeWidget::addRule(QString action_name, QMap<QString,QString> map, QString name)
{
	//Create an item and set the first line to show the display name
	QTreeWidgetItem *top=new QTreeWidgetItem(ui->rulesTreeWidget, QStringList(name));
	top->setFirstColumnSpanned(true);
	top->setData(0, Qt::UserRole, action_name);

	//Create a child item for each condition in the map
	for(QMap<QString,QString>::const_iterator it=map.begin(); it!=map.end(); ++it)
	{
		QStringList itemStrList;
		itemStrList.append(it.key());
		itemStrList.append(it.value());
		QTreeWidgetItem *item=new QTreeWidgetItem(top, itemStrList);

		//The condition is assumed to be valid (since it should be coming from a MRuleNewDialog)
		item->setData(0, Qt::UserRole, true);
	}

	emit ruleCountChanged(ruleCount());
}

QList<MRule> MRuleTreeWidget::matchingRules() const
{
	QList<MRule> list;

	//Go through each item of the widget
	for(int i=0, count=ui->rulesTreeWidget->topLevelItemCount(); i<count; ++i)
	{
		QTreeWidgetItem *top=ui->rulesTreeWidget->topLevelItem(i);

		//Create a MRule with the information of the item
		MRule rule(_prog, top->data(0, Qt::UserRole).toString(), top->text(0));

		//Go through each condition of the rule
		for(int j=0, count=top->childCount(); j<count; ++j)
		{
			QTreeWidgetItem *child=top->child(j);

			//If valid variable, add the condition to our MRule
			if(child->data(0, Qt::UserRole).toBool())
				rule.addCondition(child->text(0), child->text(1));
		}

		list.append(rule);
	}

	return list;
}

void MRuleTreeWidget::onProgramChanged(Program &p)
{
	//Now rules can be added since there is a program with variables
	_prog=p;
	ui->ruleAddButton->setEnabled(true);

	//Every TopLevelItem is a Rule, containing properties that need to be checked
	for(int i=0, count=ui->rulesTreeWidget->topLevelItemCount(); i<count; ++i)
	{
		QTreeWidgetItem *item=ui->rulesTreeWidget->topLevelItem(i);
		QString actionName=item->data(0, Qt::UserRole).toString();

		//Check each variable:value couple to see if the variable exists in the new Program
		for(int j=0, childCount=item->childCount(); j<childCount; ++j)
		{
			QTreeWidgetItem *child=item->child(j);
			ProgramVariable var=_prog.getVariableByName(child->text(0), actionName);

			//var represents an actual and existing ProgramVariable
			if(var!=ProgramVariable())
			{
				child->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
				child->setData(0, Qt::UserRole, true);
			}
			//var is invalid
			else
			{
				child->setFlags(Qt::ItemIsSelectable);
				child->setData(0, Qt::UserRole, false);
			}
		}
	}
}

void MRuleTreeWidget::onRuleViewSelectionChanged()
{
	int size=ui->rulesTreeWidget->selectedItems().size();
	ui->ruleRemoveButton->setEnabled(size > 0);
	ui->ruleEditButton->setEnabled(size > 0);
}

void MRuleTreeWidget::onRuleAddClicked()
{
	MRuleDialog dialog(_prog);

	dialog.show();
	dialog.exec();

	if(dialog.result()==QDialog::Accepted)
		addRule(dialog.actionName(), dialog.ruleConditions(), dialog.displayName());
}

void MRuleTreeWidget::onRuleEditClicked()
{
	//Fetch the first selected matching rule
	QList<QTreeWidgetItem*> list=ui->rulesTreeWidget->selectedItems();
	if(list.isEmpty())
		return;

	QTreeWidgetItem *top=list.at(0);
	while(top->parent())
		top=top->parent();

	//Extract the action out of the MRule (stored in data of the top line)
	QString action=top->data(0, Qt::UserRole).toString();

	//Separate the valid and invalid conditions for this rule
	QMap<QString,QString> map, rejected;

	for(int i=0, count=top->childCount(); i<count; ++i)
	{
		QTreeWidgetItem *item=top->child(i);
		if(item->data(0, Qt::UserRole).toBool() == true)
			map[item->text(0)]=item->text(1);
		else
			rejected[item->text(0)]=item->text(1);
	}

	//Call the edit dialog and give it only the valid conditions
	MRuleDialog dialog(_prog, action, map, top->text(0));
	dialog.show();
	dialog.exec();

	//If the ok button was clicked in the dialog
	if(dialog.result() == QDialog::Accepted)
	{
		//Remove old conditions and update the display name and action
		top->takeChildren();
		top->setText(0, dialog.displayName());
		top->setData(0, Qt::UserRole, dialog.actionName());

		//Repopulate the TreeWidgetItem with the new valid conditions
		QMap<QString,QString> result=dialog.ruleConditions();
		QMap<QString,QString>::const_iterator it;

		for(it=result.begin(); it!=result.end(); ++it)
		{
			QStringList itemStrList(it.key());
			itemStrList.append(it.value());
			QTreeWidgetItem *child=new QTreeWidgetItem(top, itemStrList);
			child->setData(0, Qt::UserRole, true);
			child->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		}

		//Still keep the invalid conditions incase the user changes the current application
		for(it=rejected.begin(); it!=rejected.end(); ++it)
		{
			QStringList itemStrList(it.key());
			itemStrList.append(it.value());
			QTreeWidgetItem *child=new QTreeWidgetItem(top, itemStrList);
			child->setData(0, Qt::UserRole, false);
			child->setFlags(Qt::ItemIsSelectable);
		}

		//Make sure child items are well sorted
		top->sortChildren(0, Qt::AscendingOrder);
	}
}

void MRuleTreeWidget::onRuleRemoveClicked()
{
	QList<QTreeWidgetItem *> list=ui->rulesTreeWidget->selectedItems();
	if(list.isEmpty())
		return;

	QTreeWidgetItem *top=list.at(0);
	while(top->parent())
		top=top->parent();
	delete top;
	emit ruleCountChanged(ruleCount());
}
