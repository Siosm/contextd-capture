#ifndef MRULEDIALOG_H
#define MRULEDIALOG_H

#include <QDialog>
#include <QMap>
#include "program.h"

namespace Ui {
	class MRuleDialog;
}

/// A dialog for creating or editing a MRule
class MRuleDialog : public QDialog
{
	Q_OBJECT
public:
	/*!
	 * \brief Construct a New MRule Dialog
	 * \param prog The program with which the MRule will be used
	 * \parent The parent widget of this dialog
	 */
	explicit MRuleDialog(Program prog, QWidget *parent = 0);

	/*!
	 * \brief Construct an Edit MRule Dialog
	 * \param prog The program with which the MRule will be used
	 * \param action The name of the action assigned to the MRule to edit
	 * \param conds The existing conditions for the MRule to edit
	 * \param name The display name of the MRule to edit
	 * \parent The parent widget of this dialog
	 */
	explicit MRuleDialog(Program prog, QString action, QMap<QString,QString> conds, QString name, QWidget *parent = 0);
	~MRuleDialog();

	/// Functions for retrieving the information of the MRule
	QMap<QString, QString> ruleConditions() const;
	QString actionName() const;
	QString displayName() const;

protected:
	void changeEvent(QEvent *e);

private:
	Ui::MRuleDialog *ui;

	/// A string used in the actions combo box for when no action is defined
	static const QString UndefinedAction;

	/// The program associated with the MRule
	Program _prog;

	/// The type of the variable (see VariableType)
	int _current_variable_type;

	/// Return the number of conditions defined for the MRule
	int conditionCount() const;

	/// Initialise the parts of the dialog that are common to both constructors
	void initDialog();

private slots:
	void onCondAddButtonClicked();
	void onCondRemoveButtonClicked();
	void onCondListSelectionChanged();
	void onMandatoryFieldsChanged();
	void onVariableChanged(QString var_name);
	void onActionChanged(int index);
};
#endif // MRULEDIALOG_H
