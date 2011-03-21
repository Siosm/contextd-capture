#ifndef ACTIONDIALOG_H
#define ACTIONDIALOG_H

#include <QDialog>
#include "program.h"
#include "programaction.h"

namespace Ui {
    class ActionDialog;
}

/// A dialog for creating and editing ProgramAction objects
class ActionDialog : public QDialog
{
    Q_OBJECT

public:
	/*!
	 * \brief Construct an Action Dialog
	 * \param blacklist A list of already reserved action names
	 * \param parent The parent widget of this dialog
	 */
	explicit ActionDialog(QStringList blacklist, QWidget *parent = 0);

	/*!
	 * \brief Construct an Action Dialog
	 * \param blacklist A list of already reserved action names
	 * \param subject The ProgramAction to edit
	 * \param vars A list of ProgramVariable objects attached to this ProgramAction
	 * \param parent The parent widget of this dialog
	 */
	explicit ActionDialog(QStringList blacklist, const ProgramAction& subject, const QList<ProgramVariable> &vars, QWidget *parent = 0);
    ~ActionDialog();

	//Used to tell which action was performed once the dialog is closed
	enum DialogAction {
		Created=QDialog::Accepted+1,
		Edited
	};

	//Override of the result/setResult methods of QDialog to allow custom values
	int result() const;
	void setResult (int result);

	/// Fetch the action and variables as they currently are in the dialog
	ProgramAction fetchAction() const;
	QList<ProgramVariable> fetchVariables() const;

protected:
    void changeEvent(QEvent *e);

private:
	//Custom QDialog result
	int _result;
	Ui::ActionDialog *ui;
	ProgramAction _action;

	void initDialog();

private slots:
	void onOkClicked();
	void onMandatoryFieldsChanged();
	void onVarMandatoryFieldsChanged();
	void onTypeChanged(int index);
	void onAddClicked();
	void onRemoveClicked();
	void onSelectionChanged();
};

#endif // ACTIONDIALOG_H
