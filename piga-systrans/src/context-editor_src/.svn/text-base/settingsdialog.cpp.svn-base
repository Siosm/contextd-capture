#include <QPushButton>
#include "settingsdialog.h"
#include "editorconfig.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
	ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);

	//Initialise widgets
	ui->autosaveFrequency->setRange(EditorConfig::AutosaveFrequencyMin, EditorConfig::AutosaveFrequencyMax);
	ui->arrowProgramMode->addItem("Icons alongside arrow", EditorConfig::HorizontalIcons);
	ui->arrowProgramMode->addItem("Popup on arrow hover", EditorConfig::Popup);
	ui->hideMode->addItem("Lower opacity", EditorConfig::LowerOpacity);
	ui->hideMode->addItem("Lower opacity and forbid selection", EditorConfig::LowerOpacityAndForbidSelection);
	ui->hideMode->addItem("Remove from view", EditorConfig::RemoveFromView);
	ui->arrowColorScheme->addItem("No color scheme", EditorConfig::NoColorScheme);
	ui->arrowColorScheme->addItem("Rule warning based", EditorConfig::WarningColor);
	ui->domainColorScheme->addItem("No color scheme", EditorConfig::NoColorScheme);
	ui->domainColorScheme->addItem("Confidentiality based", EditorConfig::ConfidentialityBased);
	ui->domainColorScheme->addItem("Reliability based", EditorConfig::ReliabilityBased);
	ui->domainColorScheme->addItem("Domain warning based", EditorConfig::WarningColor);

	//Set values and connect widgets's signals to their handlers
	setWidgetValues();
	connect(ui->autosaveFrequency, SIGNAL(valueChanged(int)), this, SLOT(onAutosaveFrequencyChange(int)));
	connect(ui->reopenLastFile, SIGNAL(clicked(bool)), this, SLOT(onReopenLastFileChanged(bool)));
	connect(ui->arrowProgramMode, SIGNAL(currentIndexChanged(int)), this, SLOT(onArrowProgramModeChanged(int)));
	connect(ui->hideMode, SIGNAL(currentIndexChanged(int)), this, SLOT(onHideModeChanged(int)));
	connect(ui->displayLoops, SIGNAL(clicked(bool)), this, SLOT(onDisplayLoopsChange(bool)));
	connect(ui->fastDraw, SIGNAL(clicked(bool)), this, SLOT(onFastDrawChange(bool)));
	connect(ui->arrowColorScheme, SIGNAL(currentIndexChanged(int)), this, SLOT(onArrowColorSchemeChanged(int)));
	connect(ui->domainColorScheme, SIGNAL(currentIndexChanged(int)), this, SLOT(onDomainColorSchemeChanged(int)));
	connect(ui->showConfRelInDomLabel, SIGNAL(clicked(bool)), this, SLOT(onShowConfRelInDomLabel(bool)));
	connect(ui->unreachableDomWarn, SIGNAL(toggled(bool)), this, SLOT(onUnreachableDomainChanged(bool)));
	connect(ui->unreachableRuleWarn, SIGNAL(toggled(bool)), this, SLOT(onUnreachableRuleChanged(bool)));
	connect(ui->bypassedRuleWarn, SIGNAL(toggled(bool)), this, SLOT(onBypassedRuleChanged(bool)));
	connect(ui->confidentialityRuleWarn, SIGNAL(toggled(bool)), this, SLOT(onConfLossChanged(bool)));
	connect(ui->reliabilityRuleWarn, SIGNAL(toggled(bool)), this, SLOT(onReliabilityLossChanged(bool)));
	connect(ui->unknownAppWarn, SIGNAL(toggled(bool)), this, SLOT(onUnknownProgramChanged(bool)));
	connect(ui->undefinedActionWarn, SIGNAL(toggled(bool)), this, SLOT(onUndefinedActionChanged(bool)));
	connect(ui->invalidMRuleWarn, SIGNAL(toggled(bool)), this, SLOT(onInvalidMRuleChanged(bool)));
	connect(ui->duplicateMRuleWarn, SIGNAL(toggled(bool)), this, SLOT(onDuplicateMRuleChanged(bool)));
	connect(ui->outlineArrowPrograms, SIGNAL(toggled(bool)), this, SLOT(onOutlineArrowProgramsChanged(bool)));

	//Check for updates of the editor's configuration, and watch the restore to defaults button
	connect(&EditorConfig::instance(), SIGNAL(updated()), this, SLOT(setWidgetValues()));
	connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()), &EditorConfig::instance(), SLOT(onRestoreDefaults()));
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

void SettingsDialog::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void SettingsDialog::setWidgetValues()
{
	ui->autosaveFrequency->setValue(EditorConfig::autosaveFrequency());
	ui->reopenLastFile->setChecked(EditorConfig::reopenLastFile());
	ui->arrowProgramMode->setCurrentIndex(ui->arrowProgramMode->findData(EditorConfig::arrowProgramMode()));
	ui->hideMode->setCurrentIndex(ui->hideMode->findData(EditorConfig::hideMode()));
	ui->displayLoops->setChecked(EditorConfig::displayLoops());
	ui->fastDraw->setChecked(EditorConfig::fastDraw());
	ui->arrowColorScheme->setCurrentIndex(ui->arrowColorScheme->findData(EditorConfig::arrowColorScheme()));
	ui->domainColorScheme->setCurrentIndex(ui->domainColorScheme->findData(EditorConfig::domainColorScheme()));
	ui->showConfRelInDomLabel->setChecked(EditorConfig::showConfRelInDomLabel());
	ui->unreachableDomWarn->setChecked(EditorConfig::unreachableDomainWarn());
	ui->unreachableRuleWarn->setChecked(EditorConfig::unreachableRuleWarn());
	ui->bypassedRuleWarn->setChecked(EditorConfig::bypassedRuleWarn());
	ui->confidentialityRuleWarn->setChecked(EditorConfig::confidentialityLossRuleWarn());
	ui->reliabilityRuleWarn->setChecked(EditorConfig::reliabilityLossRuleWarn());
	ui->unknownAppWarn->setChecked(EditorConfig::unknownProgramWarn());
	ui->undefinedActionWarn->setChecked(EditorConfig::undefinedActionWarn());
	ui->invalidMRuleWarn->setChecked(EditorConfig::invalidMRuleWarn());
	ui->duplicateMRuleWarn->setChecked(EditorConfig::duplicateMRuleWarn());
	ui->outlineArrowPrograms->setChecked(EditorConfig::outlineArrowPrograms());

	ui->slowPerformanceOptions->setEnabled(!EditorConfig::fastDraw());
}

void SettingsDialog::onAutosaveFrequencyChange(int value)
{
	if(value==1)
		ui->autosaveFrequency->setSuffix(" minute");
	else
		ui->autosaveFrequency->setSuffix(" minutes");

	EditorConfig::setAutosaveFrequency(value);
}

void SettingsDialog::onReopenLastFileChanged(bool checked)
{
	EditorConfig::setReopenLastFile(checked);
}

void SettingsDialog::onArrowProgramModeChanged(int index)
{
	EditorConfig::setArrowProgramMode(ui->arrowProgramMode->itemData(index, Qt::UserRole).toInt());
	emit viewSettingChanged();
}

void SettingsDialog::onHideModeChanged(int index)
{
	EditorConfig::setHideMode(ui->hideMode->itemData(index, Qt::UserRole).toInt());
	emit viewSettingChanged();
}

void SettingsDialog::onDisplayLoopsChange(bool checked)
{
	EditorConfig::setDisplayLoops(checked);
	emit viewSettingChanged();
}

void SettingsDialog::onFastDrawChange(bool checked)
{
	//Allow enabling/disabling of slow performance options only if not in fast draw mode
	ui->slowPerformanceOptions->setEnabled(!checked);

	//Disable slow performance options
	if(checked)
	{
		ui->outlineArrowPrograms->setChecked(false);
	}

	EditorConfig::setFastDraw(checked);
	emit drawingSettingChanged();
}

void SettingsDialog::onArrowColorSchemeChanged(int index)
{
	EditorConfig::setArrowColorScheme(ui->arrowColorScheme->itemData(index, Qt::UserRole).toInt());
	emit drawingSettingChanged();
}

void SettingsDialog::onDomainColorSchemeChanged(int index)
{
	EditorConfig::setDomainColorScheme(ui->domainColorScheme->itemData(index, Qt::UserRole).toInt());
	emit drawingSettingChanged();
}

void SettingsDialog::onShowConfRelInDomLabel(bool checked)
{
	EditorConfig::setShowConfRelInDomLabel(checked);
	emit drawingSettingChanged();
}

void SettingsDialog::onUnreachableDomainChanged(bool checked)
{
	EditorConfig::setUnreachableDomainWarn(checked);
	emit warningSettingsChanged();
}

void SettingsDialog::onUnreachableRuleChanged(bool checked)
{
	EditorConfig::setUnreachableRuleWarn(checked);
	emit warningSettingsChanged();
}

void SettingsDialog::onBypassedRuleChanged(bool checked)
{
	EditorConfig::setBypassedRuleWarn(checked);
	emit warningSettingsChanged();
}

void SettingsDialog::onConfLossChanged(bool checked)
{
	EditorConfig::setConfidentialityLossRuleWarn(checked);
	emit warningSettingsChanged();
}

void SettingsDialog::onReliabilityLossChanged(bool checked)
{
	EditorConfig::setReliabilityLossRuleWarn(checked);
	emit warningSettingsChanged();
}

void SettingsDialog::onUnknownProgramChanged(bool checked)
{
	EditorConfig::setUnknownProgramWarn(checked);
	emit warningSettingsChanged();
}

void SettingsDialog::onUndefinedActionChanged(bool checked)
{
	EditorConfig::setUndefinedActionWarn(checked);
	emit warningSettingsChanged();
}

void SettingsDialog::onInvalidMRuleChanged(bool checked)
{
	EditorConfig::setInvalidMRuleWarn(checked);
	emit warningSettingsChanged();
}

void SettingsDialog::onDuplicateMRuleChanged(bool checked)
{
	EditorConfig::setDuplicateMRuleWarn(checked);
	emit warningSettingsChanged();
}

void SettingsDialog::onOutlineArrowProgramsChanged(bool checked)
{
	EditorConfig::setOutlineArrowPrograms(checked);
	emit drawingSettingChanged();
}
