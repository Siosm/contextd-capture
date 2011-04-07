#include "configurationstack.h"
#include "domaindialog.h"
#include "domaininfodock.h"
#include "ui_domaininfodock.h"

DomainInfoDock::DomainInfoDock(QWidget *parent) :
	EditorDock(parent),
	ui(new Ui::DomainInfoDock)
{
    ui->setupUi(this);

	//Resize the edit button so that it fits the icon button above
	ui->editButton->setMaximumWidth(ui->iconButton->geometry().width());

	//Update rules whenever they change
	ConfigurationStack &stack=ConfigurationStack::instance();
	connect(&stack, SIGNAL(domainChanged(QString,QString)), this, SLOT(onDomainChanged(QString,QString)));
	connect(&stack, SIGNAL(configurationChanged()), this, SLOT(onProgramsOrRulesChanged()));
	connect(&stack, SIGNAL(allRulesChanged()), this, SLOT(onProgramsOrRulesChanged()));
	connect(&stack, SIGNAL(loopsChanged()), this, SLOT(onProgramsOrRulesChanged()));
	connect(&stack, SIGNAL(rulesChanged()), this, SLOT(onProgramsOrRulesChanged()));
	connect(&stack, SIGNAL(programChanged(QString,QString)), this, SLOT(onProgramsOrRulesChanged()));
	connect(&stack, SIGNAL(warningsChanged()), this, SLOT(onProgramsOrRulesChanged()));

	//Connect the edit button
	connect(ui->editButton, SIGNAL(clicked()), this, SLOT(onEditClicked()));

	//Forward domain/program activated signals to parents
	connect(ui->loopsTreeWidget, SIGNAL(programActivated(QString)), this, SIGNAL(programActivated(QString)));
	connect(ui->loopsTreeWidget, SIGNAL(ruleActivated(int)), this, SIGNAL(ruleActivated(int)));
	connect(ui->incomingTreeWidget, SIGNAL(domainActivated(QString)), this, SIGNAL(domainActivated(QString)));
	connect(ui->incomingTreeWidget, SIGNAL(programActivated(QString)), this, SIGNAL(programActivated(QString)));
	connect(ui->incomingTreeWidget, SIGNAL(ruleActivated(int)), this, SIGNAL(ruleActivated(int)));
	connect(ui->leavingTreeWidget, SIGNAL(domainActivated(QString)), this, SIGNAL(domainActivated(QString)));
	connect(ui->leavingTreeWidget, SIGNAL(programActivated(QString)), this, SIGNAL(programActivated(QString)));
	connect(ui->leavingTreeWidget, SIGNAL(ruleActivated(int)), this, SIGNAL(ruleActivated(int)));
}

DomainInfoDock::~DomainInfoDock()
{
    delete ui;
}

void DomainInfoDock::changeEvent(QEvent *e)
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

void DomainInfoDock::setDomain(Domain dom)
{
	if(dom != Domain())
	{
		_domain=dom;
		updateDock();
	}
	else
		resetDock();
}

Domain DomainInfoDock::domain() const
{
	return _domain;
}

bool DomainInfoDock::isValid() const
{
	return(_domain != Domain());
}

void DomainInfoDock::updateDock()
{
	//Set domain info
	QPixmap icon(_domain.iconPath());
	if(icon.isNull())
		icon.load(":/icons/default-domain");
	ui->iconButton->setIcon(QIcon(icon));
	ui->iconButton->setToolTip(_domain.iconPath());

	QString descStr="<p><b>"+_domain.displayName();

	if(ConfigurationStack::instance().current().defaultDomain()==_domain)
		descStr+=" (default domain)";

	descStr+=":</b> ";
	if(_domain.description().size() > 0)
		descStr+=_domain.description() + "</p>";
	else
		descStr+="no description available</p>";

	ui->descriptionLabel->setText(descStr);
	ui->descriptionLabel->setToolTip(descStr);
	ui->editButton->setEnabled(true);

	//Setup confidentiality and reliability labels
	ui->confidentialityLabel->setText(QString("<b>Confidentiality:</b> %1").arg(_domain.confidentiality()));
	ui->reliabilityLabel->setText(QString("<b>Reliability:</b> %1").arg(_domain.reliability()));

	//Clear previous rules
	ui->loopsTreeWidget->clearRules();
	ui->incomingTreeWidget->clearRules();
	ui->leavingTreeWidget->clearRules();

	//Go through all rules and loops for this domain
	const Configuration &inst=ConfigurationStack::instance().current();

	//Loops always go to looping
	QVector<Loop> loops=inst.loopsForDomain(_domain);
	QVector<Loop>::const_iterator loopit;
	for(loopit=loops.begin(); loopit!=loops.end(); ++loopit)
	{
		const Loop &loop=*loopit;
		ui->loopsTreeWidget->addRule(&loop);
	}

	//Rules can go to looping, leaving or incoming
	QVector<Rule> rules=inst.rulesForDomain(_domain);
	QVector<Rule>::const_iterator ruleit;
	for(ruleit=rules.begin(); ruleit!=rules.end(); ++ruleit)
	{
		const Rule &rule=*ruleit;

		if(rule.transitTo()==_domain)
		{
			const QVector<TRule> &trules=rule.tRules();
			QVector<TRule>::const_iterator it;

			for(it=trules.begin(); it!=trules.end(); ++it)
			{
				const TRule &trule=*it;

				//Domain both in sources and destination, this is a loop
				if(trule.matches(_domain.name()))
					ui->loopsTreeWidget->addRule(&rule);
				//Domain only in destination, this is an incoming transition
				else
				{
					Domain source=ConfigurationStack::instance().current().getDomainByName(trule.fromDomain());

					//If the origin domain isn't recognised, it's a regular expression
					if(source==Domain())
					{
						QList<Domain> allDomains=ConfigurationStack::instance().current().domainList();
						QList<Domain>::const_iterator it;

						for(it=allDomains.begin(); it!=allDomains.end(); ++it)
						{
							if(trule.matches((*it).name()))
								ui->incomingTreeWidget->addRule(&rule, *it);
						}
					}
					else
					{
						ui->incomingTreeWidget->addRule(&rule, source);
					}
				}
			}
		}
		//The rule has our domain in it's trules, otherwise it wouldn't have been returned by the configuration instance
		else
			ui->leavingTreeWidget->addRule(&rule, rule.transitTo());
	}
}

void DomainInfoDock::resetDock()
{
	ui->iconButton->setIcon(QIcon());
	ui->iconButton->setToolTip(QString());
	ui->descriptionLabel->clear();
	ui->confidentialityLabel->clear();
	ui->reliabilityLabel->clear();
	ui->editButton->setEnabled(false);

	ui->loopsTreeWidget->clearRules();
	ui->incomingTreeWidget->clearRules();
	ui->leavingTreeWidget->clearRules();

	close();
}

void DomainInfoDock::onUpdateDock()
{
	updateDock();
}

void DomainInfoDock::onDomainChanged(QString oldName, QString newName)
{
	if(oldName==_domain.name())
		setDomain(ConfigurationStack::instance().current().getDomainByName(newName));
}
void DomainInfoDock::onProgramsOrRulesChanged()
{
	if(isValid())
		updateDock();
	else
		resetDock();
}

void DomainInfoDock::onEditClicked()
{
	DomainDialog::editDomain(_domain.name());
}
