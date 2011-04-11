#include "configurationstack.h"
#include "ruledialog.h"
#include "ui_abstractruledialog.h"

RuleDialog::RuleDialog(QWidget *parent) :
		AbstractRuleDialog(parent)
{
	initDialog();
	this->setWindowTitle("New Rule");
}

RuleDialog::RuleDialog(const Rule &subject, QWidget *parent) :
		AbstractRuleDialog(subject, parent)
{
	initDialog();
	_domain_combo_box.setCurrentIndex(_domain_combo_box.findData(subject.transitTo().name(), Qt::UserRole));
	this->setWindowTitle("Edit Rule");
}

Domain RuleDialog::targetDomain() const
{
	return ConfigurationStack::instance().current().getDomainByName(_domain_combo_box.itemData(_domain_combo_box.currentIndex(), Qt::UserRole).toString());
}

void RuleDialog::initDialog()
{
	//Adapt labels to this dialog
	ui->infoGroupBox->setTitle("Transition Rule Information");
	ui->notifyCheckBox->setText("Notify Transition (default)");
	ui->regexpGroupBox->setTitle("Source Domains");

	//Insert the combobox in the UI
	_domain_combo_box.show();
	ui->formLayout->insertRow(2, "Target Domain", &_domain_combo_box);
	_domain_combo_box.setInsertPolicy(QComboBox::InsertAlphabetically);

	//Fill the domains combobox
	QList<Domain> domains = ConfigurationStack::instance().current().domainList();
	for(QList<Domain>::const_iterator it=domains.begin(); it!=domains.end(); ++it)
	{
		const Domain &dom=*it;

		QIcon icon(dom.iconPath());
		if(icon.isNull())
			icon.addFile(":/icons/default-domain");

		_domain_combo_box.addItem(icon, dom.displayName(), dom.name());
	}
}

void RuleDialog::editRule(int id)
{
	//Create a dialog for this rule
	Rule rule = ConfigurationStack::instance().current().findRule(id);
	RuleDialog dialog(rule);

	dialog.show();
	dialog.exec();

	//Edited the Rule, update our copy of it
	if(dialog.result()==Edited)
	{
		rule.setDisplayName(dialog.displayName());
		rule.setProgram(dialog.program());
		rule.setTransitTo(dialog.targetDomain());
		rule.setNotify(dialog.notify());
		rule.setPrompt(dialog.prompt());

		rule.clearTRules();
		QList<TRule> trules = dialog.domains();
		for (int i=0; i<trules.size(); ++i)
			rule.addTRule(trules.at(i));

		rule.clearMRules();
		QList<MRule> mrules = dialog.matchingRules();
		for (int i=0; i<mrules.size(); ++i)
			rule.addMRule(mrules.at(i));

		//Update the Rule object in the configuration
		ConfigurationStack::instance().currentForModifying().updateRule(rule);
	}
	//Deleted the Rule
	else if(dialog.result()==Deleted)
		ConfigurationStack::instance().currentForModifying().deleteRule(rule);
}
