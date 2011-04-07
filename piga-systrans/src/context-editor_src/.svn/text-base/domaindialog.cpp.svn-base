#include <QFileDialog>
#include <QPushButton>
#include "configurationstack.h"
#include "domaindialog.h"
#include "editorconfig.h"
#include "ui_domaindialog.h"

DomainDialog::DomainDialog(QWidget *parent) :
	QDialog(parent),
	_result(QDialog::Rejected),
	ui(new Ui::DomainDialog)
{
	initDialog();
	setWindowTitle("New Domain");
	ui->buttonBox->button(QDialogButtonBox::Ok)->setText("C&reate");

	//Setup the blacklist for the name line edit
	QStringList blacklist=ConfigurationStack::instance().current().domainNameList();
	ui->nameEdit->setBlacklist(blacklist);

	//Setup the default domain checkbox depending on whether there is a default domain
	if(ConfigurationStack::instance().current().defaultDomain()==Domain())
	{
		ui->defaultDomain->setChecked(true);
		ui->defaultDomain->setEnabled(false);
	}

	//Set default values for the sliders
	ui->confidentialitySlider->setValue(Domain::ConfidentialityDefault);
	ui->reliabilitySlider->setValue(Domain::ReliabilityDefault);
}


DomainDialog::DomainDialog(Domain subject, QWidget *parent) :
	QDialog(parent),
	_result(QDialog::Rejected),
	ui(new Ui::DomainDialog),
	_domain(subject)
{
	initDialog();
	setWindowTitle("Edit Domain");

	//Setup the blacklist for the name line edit
	ui->nameEdit->setText(subject.name());
	QStringList blacklist=ConfigurationStack::instance().current().domainNameList();
	blacklist.removeAll(subject.name());
	ui->nameEdit->setBlacklist(blacklist);

	//Setup the data of the widgets
	ui->displayNameEdit->setText(subject.displayName());
	ui->iconButton->setIcon(QIcon(subject.iconPath()));
	ui->iconButton->setToolTip(subject.iconPath());
	ui->descriptionPlainTextEdit->setPlainText(subject.description());
	ui->confidentialitySlider->setValue(subject.confidentiality());
	ui->reliabilitySlider->setValue(subject.reliability());

	//Setup the default domain checkbox depending on whether the domain is default
	if(ConfigurationStack::instance().current().defaultDomain()==subject)
	{
		ui->defaultDomain->setChecked(true);
		ui->defaultDomain->setEnabled(false);
	}
	else
		ui->defaultDomain->setChecked(false);
}

void DomainDialog::initDialog()
{
	ui->setupUi(this);

	//Connect signals to their handlers, and make sure all fields are properly filled
	connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(onOkClicked()));
	connect(ui->nameEdit, SIGNAL(textChanged(QString)), this, SLOT(onMandatoryFieldsChanged()));
	connect(ui->displayNameEdit, SIGNAL(textChanged(QString)), this, SLOT(onMandatoryFieldsChanged()));
	connect(ui->iconButton, SIGNAL(clicked()), this, SLOT(onIconClicked()));
	onMandatoryFieldsChanged();

	//Setup the line edits
	ui->nameEdit->setFocus(Qt::PopupFocusReason);
	ui->nameEdit->setValidationExpression(".+");
	ui->displayNameEdit->setValidationExpression(".+");

	//Little hack to make the layout look better
	for(int i=0; i<ui->gridLayout->rowCount(); ++i)
		ui->gridLayout->setRowMinimumHeight(i, ui->displayNameEdit->geometry().height());
	ui->gridLayout->update();

	//Set the range and value of the sliders based on Domain's defaults
	ui->confidentialitySlider->setRange(Domain::ConfidentialityMin, Domain::ConfidentialityMax);
	ui->reliabilitySlider->setRange(Domain::ReliabilityMin, Domain::ReliabilityMax);

	//Another hack to force the sliders' associated labels' text to be updated
	ui->confidentialityLabel->setNum(ui->confidentialitySlider->value());
	ui->reliabilityLabel->setNum(ui->reliabilitySlider->value());
}

DomainDialog::~DomainDialog()
{
    delete ui;
}

void DomainDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
	switch(e->type())
	{
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/* Overriden dialog result functions */
void DomainDialog::setResult(int result)
{
	_result = result;
}

int DomainDialog::result() const
{
	if(_result > QDialog::Accepted)
		return _result;
	else
		return QDialog::result();
}

Domain DomainDialog::retrieveDomain() const
{
	if(!ui->buttonBox->button(QDialogButtonBox::Ok)->isEnabled())
		return Domain();
	else
		return Domain(ui->nameEdit->text(),
					  ui->displayNameEdit->text(),
					  ui->iconButton->toolTip(),
					  ui->descriptionPlainTextEdit->document()->toPlainText(),
					  ui->confidentialitySlider->value(),
					  ui->reliabilitySlider->value());
}

bool DomainDialog::defaultDomainChecked() const
{
	return ui->defaultDomain->isChecked();
}

void DomainDialog::editDomain(QString name)
{
	Domain dom = ConfigurationStack::instance().current().getDomainByName(name);

	if(dom != Domain())
	{
		DomainDialog dialog(dom);

		dialog.show();
		dialog.exec();

		if(dialog.result() == DomainDialog::Edited)
			ConfigurationStack::instance().currentForModifying().updateDomain(name,
																			  dialog.retrieveDomain(),
																			  dialog.defaultDomainChecked());
	}
	else
	{
		qWarning("Tried to edit an invalid domain.");
	}
}

void DomainDialog::onIconClicked()
{
	QString dir=ui->iconButton->toolTip()!=QString() ? ui->iconButton->toolTip() : EditorConfig::lastOpenedPath();
	QString fileName=QFileDialog::getOpenFileName(this, "Select icon...", dir, "Icon Files (*.png *.svg *.ico *.xpm *.gif)");

	if(fileName.size() > 0)
	{
		EditorConfig::setLastOpenedPath(fileName);
		ui->iconButton->setIcon(QIcon(fileName));
		ui->iconButton->setToolTip(fileName);
	}
}

void DomainDialog::onOkClicked()
{
	if(_domain != Domain())
		this->setResult(Edited);
	else
		this->setResult(Created);
	this->accept();
}

void DomainDialog::onMandatoryFieldsChanged()
{
	//Make sure no mandatory field is empty
	bool valid=(ui->nameEdit->text().size() > 0) && (ui->displayNameEdit->text().size() > 0);

	//Make sure no other domain than the one in the dialog has the currently written name
	if(valid)
		valid=ui->nameEdit->valueIsValid();

	//Enable the Ok button if the domain in the dialog is valid
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
}
