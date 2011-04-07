#include "configurationstack.h"
#include "abstractruleinfodock.h"
#include "ui_abstractruleinfodock.h"
#include "loopdialog.h"
#include "ruledialog.h"

AbstractRuleInfoDock::AbstractRuleInfoDock(QWidget *parent) :
	EditorDock(parent),
	ui(new Ui::AbstractRuleInfoDock),
	_current_rule_id(AbstractRule::InvalidId)
{
    ui->setupUi(this);

	//Resizing of columns in the tree widgets
	ui->domainsTreeWidget->header()->setResizeMode(0, QHeaderView::ResizeToContents);
	ui->domainsTreeWidget->header()->setResizeMode(1, QHeaderView::ResizeToContents);
	ui->domainsTreeWidget->header()->setResizeMode(2, QHeaderView::ResizeToContents);
	ui->mrulesTreeWidget->header()->setResizeMode(0, QHeaderView::ResizeToContents);
	ui->mrulesTreeWidget->header()->setResizeMode(1, QHeaderView::ResizeToContents);

	//Connect signals to handlers
	connect(ui->appName, SIGNAL(linkActivated(QString)), this, SIGNAL(programActivated(QString)));
	connect(ui->targetDomain, SIGNAL(linkActivated(QString)), this, SIGNAL(domainActivated(QString)));
	connect(ui->domainsTreeWidget, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(onDomainActivated(QTreeWidgetItem*,int)));

	ConfigurationStack &stack=ConfigurationStack::instance();
	connect(&stack, SIGNAL(allRulesChanged()), this, SLOT(onRulesChanged()));
	connect(&stack, SIGNAL(rulesChanged()), this, SLOT(onRulesChanged()));
	connect(&stack, SIGNAL(loopsChanged()), this, SLOT(onRulesChanged()));
	connect(&stack, SIGNAL(warningsChanged()), this, SLOT(onRulesChanged()));
	connect(ui->editButton, SIGNAL(clicked()), this, SLOT(onEditButtonClicked()));
}

AbstractRuleInfoDock::~AbstractRuleInfoDock()
{
    delete ui;
}

void AbstractRuleInfoDock::changeEvent(QEvent *e)
{
	QDockWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void AbstractRuleInfoDock::setRule(int id)
{
	_current_rule_id=id;
	if(isValid())
		updateDock();
	else
		resetDock();
}

bool AbstractRuleInfoDock::isValid() const
{
	ConfigurationStack &stack=ConfigurationStack::instance();

	return stack.findAbstractRule(_current_rule_id)->isValid();
}

void AbstractRuleInfoDock::addRuleWarning(const RuleWarning &warn)
{
	ui->warningsGroupBox->show();

	QLabel *icon=new QLabel();
	icon->setPixmap(QPixmap(warn.icon()));

	QLabel *text=new QLabel(warn.text(true));
	connect(text, SIGNAL(linkActivated(QString)), this, SLOT(onLinkActivated(QString)));
	text->setTextInteractionFlags(Qt::TextBrowserInteraction);
	text->setWordWrap(true);

	//Insert the row in the layout of the warning area
	ui->formLayout->insertRow(0, icon, text);
}

void AbstractRuleInfoDock::updateDock()
{
	ConfigurationStack &stack=ConfigurationStack::instance();

	//The AbstractRule that will be used to fill the dock
	AbstractRulePtr abs=stack.findAbstractRule(_current_rule_id);
	RulePtr rule=abs.dynamicCast<Rule>();

	//Do Loop specific actions
	if(rule.isNull())
	{
		ui->targetDomainLabel->hide();
		ui->targetDomain->hide();
		ui->domainsGroupBox->setTitle("Domains");
		setWindowTitle("Loop Info Dock");
	}
	//Do Rule specific actions
	else
	{
		ui->targetDomainLabel->show();
		ui->targetDomain->show();
		ui->targetDomain->setText("<a href=\""+rule->transitTo().name()+"\">"+rule->transitTo().displayName()+"</a>");
		ui->domainsGroupBox->setTitle("Source Domains");
		setWindowTitle("Rule Info Dock");
	}

	//Finish updating the dock with the common parts of both rule types
	ui->displayName->setText(abs->displayName());
	Program app=stack.current().getProgramByName(abs->appName(), true);
	if(app.isValid())
		ui->appName->setText("<a href=\""+abs->appName()+"\">"+app.displayName()+"</a>");
	else
		ui->appName->setText(app.displayName());

	//Fill the list of domains
	ui->domainsTreeWidget->clear();
	const QVector<TRule> &doms=abs->tRules();
	for(int i=0; i<doms.size(); ++i)
	{
		const TRule &trule=doms[i];

		//Check if the text is a domain name or a regexp
		Domain dom=stack.current().getDomainByName(trule.fromDomain());

		QTreeWidgetItem *item=new QTreeWidgetItem(ui->domainsTreeWidget,
												  QStringList(dom==Domain() ? trule.fromDomain() : dom.displayName()));

		//Set the notify and prompt flags
		item->setCheckState(1, trule.hasNotify() ? (trule.notify()? Qt::Checked:Qt::Unchecked) : abs->notify() ? Qt::Checked:Qt::Unchecked);
		item->setCheckState(2, trule.hasPrompt() ? (trule.prompt()? Qt::Checked:Qt::Unchecked) : abs->prompt() ? Qt::Checked:Qt::Unchecked);
		item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);

		//Set the appropriate domain icon, or regexp icon
		if(dom!=Domain())
		{
			QPixmap icon(dom.iconPath());
			if(icon.isNull())
				icon.load(":/icons/default-domain");
			item->setIcon(0, QIcon(icon));
			item->setData(0, Qt::UserRole, trule.fromDomain());
		}
		else
		{
			item->setIcon(0, QIcon(":/icons/regexp"));
			item->setData(0, Qt::UserRole, QString());
		}
	}

	//Fill the matching rules tree
	ui->mrulesTreeWidget->clear();
	const QVector<MRule> &rules=abs->mRules();
	for(int i=0; i<rules.size(); ++i)
	{
		const MRule &mrule=rules[i];

		QTreeWidgetItem *mruleItem=new QTreeWidgetItem(ui->mrulesTreeWidget, QStringList(mrule.displayName()));
		mruleItem->setFirstColumnSpanned(true);

		//The matching rule has its conditions as children
		const QMap<QString, QString> &map=mrule.conditions();
		QMap<QString, QString>::const_iterator it;
		for(it=map.begin(); it!=map.end(); ++it)
		{
			QStringList itemStrList(it.key());
			itemStrList.append(it.value());
			QTreeWidgetItem *condItem=new QTreeWidgetItem(mruleItem, itemStrList);
			condItem->setToolTip(0, it.key());
			condItem->setToolTip(1, it.value());
		}

	}

	//Clear the warning area and add warnings for the current rule id
	clearWarningArea();

	QVector<WarningPtr> warnings=stack.warningsForRule(_current_rule_id);
	for(QVector<WarningPtr>::const_iterator it=warnings.begin(); it!=warnings.end(); ++it)
	{
		RuleWarningPtr rulePtr=(*it).dynamicCast<RuleWarning>();
		if(!rulePtr.isNull())
			addRuleWarning(*rulePtr);
	}
}

void AbstractRuleInfoDock::resetDock()
{
	ui->displayName->clear();
	ui->appName->clear();
	ui->domainsTreeWidget->clear();
	ui->mrulesTreeWidget->clear();
	clearWarningArea();

	close();
}

void AbstractRuleInfoDock::clearWarningArea()
{
	QLayoutItem *item;
	while((item=ui->formLayout->itemAt(0)) != 0)
	{
		ui->formLayout->removeItem(item);
		delete item->widget();
		delete item;
	}

	ui->warningsGroupBox->hide();
}

void AbstractRuleInfoDock::onRulesChanged()
{
	setRule(_current_rule_id);
}

void AbstractRuleInfoDock::onEditButtonClicked()
{
	ConfigurationStack &stack=ConfigurationStack::instance();
	AbstractRulePtr abs=stack.findAbstractRule(_current_rule_id);

	if(!abs.dynamicCast<Loop>().isNull())
		LoopDialog::editLoop(_current_rule_id);
	else
		RuleDialog::editRule(_current_rule_id);
}

void AbstractRuleInfoDock::onDomainActivated(QTreeWidgetItem *item, int column)
{
	if(column==0)
	{
		QString data=item->data(0, Qt::UserRole).toString();
		if(data!=QString())
			emit domainActivated(data);
	}
}

void AbstractRuleInfoDock::onLinkActivated(QString uri)
{
	if(uri.startsWith("rule://"))
	{
		bool ok;
		int id=uri.remove(0, 7).toInt(&ok);
		if(ok)
			emit ruleActivated(id);
	}
	else if(uri.startsWith("dom://"))
		emit domainActivated(uri.remove(0, 6));
	else if(uri.startsWith("prog://"))
		emit programActivated(uri.remove(0, 7));
}
