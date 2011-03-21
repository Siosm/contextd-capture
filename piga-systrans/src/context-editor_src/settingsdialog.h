#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
    class SettingsDialog;
}

/// A dialog for editing the application settings of Context Editor
class SettingsDialog : public QDialog
{
Q_OBJECT

public:
	/*!
	 * \brief Construct a Settings Dialog
	 * \param parent The parent widget of the dialog
	 */
	explicit SettingsDialog(QWidget *parent = 0);
	~SettingsDialog();

protected:
	void changeEvent(QEvent *e);

private:
	Ui::SettingsDialog *ui;

signals:
	void dockSettingChanged();
	void viewSettingChanged();
	void drawingSettingChanged();
	void warningSettingsChanged();

private slots:
	/// Initialise the widgets with the values of the application's QSettings
	void setWidgetValues();

	/// Handlers for input events of each widget
	void onAutosaveFrequencyChange(int value);
	void onReopenLastFileChanged(bool checked);
	void onArrowProgramModeChanged(int index);
	void onHideModeChanged(int index);
	void onDisplayLoopsChange(bool checked);
	void onFastDrawChange(bool checked);
	void onArrowColorSchemeChanged(int index);
	void onDomainColorSchemeChanged(int index);
	void onShowConfRelInDomLabel(bool checked);
	void onUnreachableDomainChanged(bool checked);
	void onUnreachableRuleChanged(bool checked);
	void onBypassedRuleChanged(bool checked);
	void onConfLossChanged(bool checked);
	void onReliabilityLossChanged(bool checked);
	void onUnknownProgramChanged(bool checked);
	void onUndefinedActionChanged(bool checked);
	void onInvalidMRuleChanged(bool checked);
	void onDuplicateMRuleChanged(bool checked);
	void onOutlineArrowProgramsChanged(bool checked);
};

#endif // SETTINGSDIALOG_H
