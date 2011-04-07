#include "domainselectionwidget.h"
#include "ui_domainselectionwidget.h"
#include "configurationstack.h"
#include "typeddelegate.h"
#include "variabletypes.h"

DomainSelectionWidget::DomainSelectionWidget(QWidget *parent) :
    QWidget(parent),
	ui(new Ui::DomainSelectionWidget),
	_delegate(new TypedDelegate(Type::RegExp, 0, this, ".+"))
{
    ui->setupUi(this);

	//Insert the existing domains into the tree
	const Configuration &inst=ConfigurationStack::instance().current();
	QList<Domain> domains=inst.domainList();

	for(int i=0; i<domains.size(); ++i)
	{
		const Domain &dom=domains.at(i);

		//Create the item, of DomainName type
		QTreeWidgetItem *item=new QTreeWidgetItem(ui->domainsList, QStringList(dom.displayName()));
		item->setIcon(0, QIcon(dom.iconPath()));
		item->setData(0, Qt::UserRole, dom.name());
		item->setData(0, ItemTypeRole, DomainNameItem);

		item->setCheckState(0, Qt::Unchecked);
		item->setCheckState(1, Qt::PartiallyChecked);
		item->setCheckState(2, Qt::PartiallyChecked);
	}
	ui->domainsList->sortItems(0, Qt::AscendingOrder);

	//Connect buttons and handler for selection changes
	connect(ui->regexpAddButton, SIGNAL(clicked()), this, SLOT(onRegexpAddClicked()));
	connect(ui->regexpRemoveButton, SIGNAL(clicked()), this, SLOT(onRegexpRemoveClicked()));
	connect(ui->clearCheckboxButton, SIGNAL(clicked()), this, SLOT(onClearCheckboxClicked()));
	connect(ui->domainsList, SIGNAL(itemSelectionChanged()), this, SLOT(onTreeSelectionChanged()));
	connect(ui->domainsList, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(onItemChanged(QTreeWidgetItem*,int)));
	onTreeSelectionChanged();

	//Sizing of the items in the tree
	ui->domainsList->header()->setResizeMode(0, QHeaderView::ResizeToContents);
	ui->domainsList->header()->setResizeMode(1, QHeaderView::ResizeToContents);
	ui->domainsList->header()->setResizeMode(2, QHeaderView::ResizeToContents);

	//Allow selecting several items
	ui->domainsList->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

DomainSelectionWidget::~DomainSelectionWidget()
{
    delete ui;
	delete _delegate;
}

void DomainSelectionWidget::changeEvent(QEvent *e)
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

bool DomainSelectionWidget::hasCheckedDomains() const
{
	for(int i=0, count=ui->domainsList->topLevelItemCount(); i<count; ++i)
		if(ui->domainsList->topLevelItem(i)->checkState(0)==Qt::Checked)
			return true;

	return false;
}

bool DomainSelectionWidget::checkDomain(QString name, Qt::CheckState notify, Qt::CheckState prompt)
{
	//Look for the item with the given name
	for(int i=0, count=ui->domainsList->topLevelItemCount(); i<count; ++i)
	{
		QTreeWidgetItem *item=ui->domainsList->topLevelItem(i);

		//If the name is found, set the check state
		if(item->data(0, Qt::UserRole).toString()==name)
		{
			item->setCheckState(0, Qt::Checked);
			item->setCheckState(1, notify);
			item->setCheckState(2, prompt);
			return true;
		}
	}

	return false;
}

void DomainSelectionWidget::addExpression(QString expr, Qt::CheckState notify, Qt::CheckState prompt)
{
	//Create a new item
	QTreeWidgetItem *item=new QTreeWidgetItem(ui->domainsList, QStringList(expr));
	item->setCheckState(1, notify);
	item->setCheckState(2, prompt);
	item->setFlags(Qt::ItemIsEditable|Qt::ItemIsSelectable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
	item->setData(0, ItemTypeRole, DomainRegExpItem);
	item->setIcon(0, QIcon(":/icons/regexp"));
	ui->domainsList->setItemDelegateForRow(ui->domainsList->indexOfTopLevelItem(item), _delegate);

	//If it's expression is valid and not empty (ie. not a newly created item), then check it already
	item->setCheckState(0, (!expr.isEmpty() && QRegExp(expr).isValid()) ? Qt::Checked : Qt::Unchecked);

	if(expr.isEmpty())
		ui->domainsList->editItem(item, 0);
}

QList<TRule> DomainSelectionWidget::domains() const
{
	QList<TRule> list;

	//Go through each line, and fetch the domain if it is checked
	for(int i=0, count=ui->domainsList->topLevelItemCount(); i<count; ++i)
	{
		QTreeWidgetItem *item=ui->domainsList->topLevelItem(i);

		if(item->checkState(0)==Qt::Checked)
		{
			//Create the rule with either the domain name or the expression
			QString name;
			if(item->data(0, ItemTypeRole).toInt()==DomainNameItem)
				name=item->data(0, Qt::UserRole).toString();
			else
				name=item->text(0);

			TRule rule(name);

			//Setup prompt and notify
			if(item->checkState(1)!=Qt::PartiallyChecked)
				rule.setNotify(item->checkState(1)==Qt::Checked);
			if(item->checkState(2)!=Qt::PartiallyChecked)
				rule.setPrompt(item->checkState(2)==Qt::Checked);

			//Append to the list
			list.append(rule);
		}
	}

	return list;
}

void DomainSelectionWidget::onTreeSelectionChanged()
{
	QList<QTreeWidgetItem *> list=ui->domainsList->selectedItems();

	//Allow clearing the checkboxes only if something is selected
	ui->clearCheckboxButton->setEnabled(!list.isEmpty());

	bool onlyRegexpSelected=!list.isEmpty();
	for(int i=0; i<list.size() && onlyRegexpSelected; ++i)
	{
		if(list.at(i)->data(0, ItemTypeRole).toInt()==DomainNameItem)
			onlyRegexpSelected=false;
	}

	ui->regexpRemoveButton->setEnabled(onlyRegexpSelected);
}

void DomainSelectionWidget::onRegexpAddClicked()
{
	addExpression();
}

void DomainSelectionWidget::onRegexpRemoveClicked()
{
	qDeleteAll(ui->domainsList->selectedItems());
	emit domainChanged();
}

void DomainSelectionWidget::onClearCheckboxClicked()
{
	QList<QTreeWidgetItem*> list=ui->domainsList->selectedItems();
	for(int i=0, count=list.size(); i<count; ++i)
	{
		QTreeWidgetItem *item=list.at(i);
		item->setCheckState(1, Qt::PartiallyChecked);
		item->setCheckState(2, Qt::PartiallyChecked);
	}
}

void DomainSelectionWidget::onItemChanged(QTreeWidgetItem *item, int column)
{
	//Only watch column 0
	if(column==0)
	{
		//If the item is of type regexp, we should check that it's still valid
		if(item->data(0, ItemTypeRole).toInt()!=DomainNameItem)
		{
			//Disable the event handler for now because we will change the item
			disconnect(ui->domainsList, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(onItemChanged(QTreeWidgetItem*,int)));

			//If the item's regexp is valid, check it, otherwise uncheck it
			item->setCheckState(0, (QRegExp(item->text(0)).isValid() && !item->text(0).isEmpty()) ? Qt::Checked : Qt::Unchecked);

			//Reconnect the handler now
			connect(ui->domainsList, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(onItemChanged(QTreeWidgetItem*,int)));
		}

		emit domainChanged();
	}
}
