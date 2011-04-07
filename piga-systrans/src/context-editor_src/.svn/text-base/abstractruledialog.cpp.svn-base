#include <QMessageBox>
#include <QPushButton>
#include <QTreeWidgetItem>
#include <QTableWidget>
#include <QVector>

#include "configurationstack.h"
#include "mrule.h"
#include "abstractruledialog.h"
#include "trule.h"
#include "ui_abstractruledialog.h"

AbstractRuleDialog::AbstractRuleDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AbstractRuleDialog),
	_has_abstract_rule(false),
	_result(QDialog::Rejected)
{
	initDialog();
	setWindowTitle("New Abstract Rule");
	ui->buttonBox->button(QDialogButtonBox::Ok)->setText("C&reate");
}

AbstractRuleDialog::AbstractRuleDialog(const AbstractRule &subject, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AbstractRuleDialog),
	_has_abstract_rule(true),
	_result(QDialog::Rejected)
{
	initDialog();
	setWindowTitle("Edit Abstract Rule");

	//Add a Delete button to the dialog
	QPushButton *button=new QPushButton("&Delete", ui->buttonBox);
	ui->buttonBox->addButton(button, QDialogButtonBox::DestructiveRole);
	connect(button, SIGNAL(clicked()), this, SLOT(onDeleteClicked()));

	//Setup the dialog with the rule's data
	ui->displayNameEdit->setText(subject.displayName());
	ui->appComboBox->setCurrentIndex(ui->appComboBox->findData(subject.appName(), Qt::UserRole));
	ui->notifyCheckBox->setChecked(subject.notify());
	ui->promptCheckBox->setChecked(subject.prompt());

	//Setup the transition rules (ie. domains)
	QVector<TRule> trules=subject.tRules();
	for(int i=0; i<trules.count(); ++i)
	{
		const TRule &trule = trules[i];

		//Check the domain in the selection widget if it exists already
		bool found=ui->domainSelectionWidget->checkDomain(trule.fromDomain(),
					  trule.hasNotify()? (trule.notify()? Qt::Checked : Qt::Unchecked) : Qt::PartiallyChecked,
					  trule.hasPrompt()? (trule.prompt()? Qt::Checked : Qt::Unchecked) : Qt::PartiallyChecked);

		//If the domain isn't found in the domains list, it's a regular expression
		if(!found)
			ui->domainSelectionWidget->addExpression(trule.fromDomain(),
					   trule.hasNotify()? (trule.notify()? Qt::Checked : Qt::Unchecked) : Qt::PartiallyChecked,
					   trule.hasPrompt()? (trule.prompt()? Qt::Checked : Qt::Unchecked) : Qt::PartiallyChecked);
	}

	//Setup the matching rules
	QVector<MRule> mrules=subject.mRules();
	for(int i=0; i<mrules.count(); ++i)
	{
		const MRule &mrule=mrules[i];
		ui->mrulesWidget->addRule(mrule.actionName(), mrule.conditions(), mrule.displayName());
	}

	//Call the handler that will check which mrules are valid for this program
	onCurrentProgramChanged(ui->appComboBox->currentIndex());
}

AbstractRuleDialog::~AbstractRuleDialog()
{
	delete ui;
}

void AbstractRuleDialog::initDialog()
{
	ui->setupUi(this);

	//Signal for the Ok button
	connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(onOkClicked()));

	//Mandatory fields should trigger the onMandatoryFieldsChanged function
	ui->displayNameEdit->setValidationExpression(".+");
	connect(ui->domainSelectionWidget, SIGNAL(domainChanged()), this, SLOT(onMandatoryFieldsChanged()));
	connect(ui->mrulesWidget, SIGNAL(ruleCountChanged(int)), this, SLOT(onMandatoryFieldsChanged()));
	connect(ui->displayNameEdit, SIGNAL(textChanged(QString)), this, SLOT(onMandatoryFieldsChanged()));
	connect(ui->appComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onMandatoryFieldsChanged()));
	onMandatoryFieldsChanged();

	//Fill the applications combobox
	QList<Program> progList=ConfigurationStack::instance().current().programList();
	for(int i=0; i<progList.size(); ++i)
	{
		const Program &prog=progList.at(i);

		QPixmap icon(prog.icon());
		if(icon.isNull())
			icon.load(":/icons/default-program");

		ui->appComboBox->addItem(QIcon(icon), prog.displayName(), prog.name());
	}

	//Make sure to refresh the rules widget when the program changes
	connect(ui->appComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentProgramChanged(int)));
	onCurrentProgramChanged(0);
}

void AbstractRuleDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
	switch (e->type())
	{
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void AbstractRuleDialog::setResult(int result)
{
	_result = result;
}

int AbstractRuleDialog::result() const
{
	if(_result > QDialog::Accepted)
		return _result;
	else
		return QDialog::result();
}

QString AbstractRuleDialog::displayName() const
{
	return ui->displayNameEdit->text();
}

Program AbstractRuleDialog::program() const
{
	return ConfigurationStack::instance().current().getProgramByName(ui->appComboBox->itemData(ui->appComboBox->currentIndex(), Qt::UserRole).toString());
}

bool AbstractRuleDialog::notify() const
{
	return ui->notifyCheckBox->isChecked();
}

bool AbstractRuleDialog::prompt() const
{
	return ui->promptCheckBox->isChecked();
}

QList<TRule> AbstractRuleDialog::domains() const
{
	return ui->domainSelectionWidget->domains();
}

QList<MRule> AbstractRuleDialog::matchingRules() const
{
	return ui->mrulesWidget->matchingRules();
}

void AbstractRuleDialog::onDeleteClicked()
{
	QMessageBox box(QMessageBox::Warning, "Delete Abstract Rule",
					"Are you sure you want to delete this abstract rule? This action cannot be undone.",
					QMessageBox::Yes | QMessageBox::No, this);
	box.show();
	box.exec();
	if(box.result() == QMessageBox::Yes)
	{
		this->setResult(Deleted);
		this->reject();
	}
}

void AbstractRuleDialog::onOkClicked()
{
	if(_has_abstract_rule)
		this->setResult(Edited);
	else
		this->setResult(Created);
	this->accept();
}

void AbstractRuleDialog::onMandatoryFieldsChanged()
{
	//Valid if there is a display name and if there are mrules
	bool valid=(ui->displayNameEdit->valueIsValid()) &&
			   (ui->mrulesWidget->ruleCount() > 0);

	//Valid if there are programs
	if(valid)
		valid=ui->appComboBox->count() > 0;

	//Valid if there is at least one domain checked
	if(valid)
		valid=ui->domainSelectionWidget->hasCheckedDomains();

	//Valid if there is a selected application
	if(valid)
		valid=ui->appComboBox->currentIndex()!=-1;

	//If the Abstract Rule is complete, the Ok button is enabled
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
}

void AbstractRuleDialog::onCurrentProgramChanged(int index)
{
	if(index!=-1)
	{
		QString appName=ui->appComboBox->itemData(index).toString();
		Program app=ConfigurationStack::instance().current().getProgramByName(appName);
		ui->mrulesWidget->onProgramChanged(app);
	}
}
