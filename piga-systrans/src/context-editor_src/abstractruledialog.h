#ifndef ABSTRACTRULEDIALOG_H
#define ABSTRACTRULEDIALOG_H

#include <QDialog>
#include "domainselectionwidget.h"
#include "rule.h"
#include "mruletreewidget.h"

namespace Ui {
	class AbstractRuleDialog;
}

/// A class for creating and editing abstract rule objects
class AbstractRuleDialog : public QDialog
{
    Q_OBJECT
public:
	~AbstractRuleDialog();

	/// Custom return values for QDialog::result()
	enum DialogAction {
		Created=QDialog::Accepted+1,
		Deleted,
		Edited
	};

	/// Override of the result/setResult methods of QDialog to allow custom values
	int result() const;
	void setResult (int result);

	/// Various functions for retrieving information from the Abstract Rule Dialog
	QString displayName() const;
	Program program() const;
	bool notify() const;
	bool prompt() const;
	QList<TRule> domains() const;
	QList<MRule> matchingRules() const;


protected:
	/*!
	 * \brief Construct a New Abstract Rule Dialog
	 * \param parent The parent widget of the dialog
	 */
	explicit AbstractRuleDialog(QWidget *parent = 0);

	/*!
	 * \brief Construct an Abstract Rule Rule Dialog
	 * \param subject The Abstract Rule to edit
	 * \param parent The parent widget of the dialog
	 */
	explicit AbstractRuleDialog(const AbstractRule &subject, QWidget *parent = 0);

    void changeEvent(QEvent *e);
	Ui::AbstractRuleDialog *ui;

	/// Whether the dialog has been created with an existing Abstract Rule
	bool _has_abstract_rule;

	/// Initialise the parts of the dialog that are common to both constructors
	void initDialog();

private:
	/// Custom QDialog result storage variable
	int _result;

private slots:
	void onDeleteClicked();
	void onOkClicked();
	void onMandatoryFieldsChanged();
	void onCurrentProgramChanged(int);
};

#endif // ABSTRACTRULEDIALOG_H
