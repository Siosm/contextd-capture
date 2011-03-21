#ifndef APPEXPORTDIALOG_H
#define APPEXPORTDIALOG_H

#include <QDialog>

namespace Ui {
    class AppExportDialog;
}

/// A dialog for exporting applications
class AppExportDialog : public QDialog
{
    Q_OBJECT

public:
	/*!
	 * \brief Construct an App Export Dialog
	 * \parent The parent widget of this dialog
	 */
    explicit AppExportDialog(QWidget *parent = 0);
    ~AppExportDialog();

	/// The names of the applications selected for export
	QStringList selectedApplications() const;

	/// The path where the user wants the applications to be exported
	QString exportPath() const;

	/// Export the selected applications to the export path
	static void doExport(QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::AppExportDialog *ui;

private slots:
	void onSelectAllClicked();
	void onUnselectAllClicked();
	void onToggleClicked();
	void onBrowseClicked();
	void onMandatoryFieldsChanged();
};

#endif // APPEXPORTDIALOG_H
