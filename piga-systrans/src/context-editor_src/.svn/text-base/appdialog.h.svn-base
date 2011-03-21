#ifndef APPEDITDIALOG_H
#define APPEDITDIALOG_H

#include <QDialog>
#include "program.h"

namespace Ui {
	class AppDialog;
}

/// A dialog for creating and editing Program objects
class AppDialog : public QDialog
{
    Q_OBJECT

public:
	/*!
	 * \brief Construct an App Dialog
	 * \param parent The parent widget of this dialog
	 */
	explicit AppDialog(QWidget *parent = 0);

	/*!
	 * \brief Construct an App Dialog
	 * \param subject The Program to edit
	 * \param parent The parent widget of this dialog
	 */
	explicit AppDialog(const Program &subject, QWidget *parent = 0);
	~AppDialog();

	//Used to tell which action was performed once the dialog is closed
	enum DialogAction {
		Created=QDialog::Accepted+1,
		Edited
	};

	//Override of the result/setResult methods of QDialog to allow custom values
	int result() const;
	void setResult (int result);

	/// Fetch the Program as it currently is in the dialog
	Program fetchProgram() const;

protected:
    void changeEvent(QEvent *e);

private:
	//Custom QDialog result
	int _result;
	Ui::AppDialog *ui;
	Program _program;

	/// Actions and variables contained in the app
	QMap<QString, ProgramAction> _actions;
	QMap<QString, QList<ProgramVariable> > _variables;

	void initDialog();

	/// Update the tree widget used to display the app's actions
	void updateActions();

private slots:
	void onBrowseClicked();
	void onIconClicked();
	void onOkClicked();
	void onMandatoryFieldsChanged();
	void onAddClicked();
	void onEditClicked();
	void onRemoveClicked();
	void onSelectionChanged();
};

#endif // APPEDITDIALOG_H
