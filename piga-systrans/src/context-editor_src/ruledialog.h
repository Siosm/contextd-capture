#ifndef RULEDIALOG_H
#define RULEDIALOG_H

#include <QComboBox>
#include "abstractruledialog.h"
#include "domain.h"
#include "rule.h"

/// A dialog for creating and editing Rule objects
class RuleDialog : public AbstractRuleDialog
{
    Q_OBJECT
public:
	/*!
	 * \brief Construct a New Rule Dialog
	 * \param parent The parent widget of the dialog
	 */
	explicit RuleDialog(QWidget *parent = 0);

	/*!
	 * \brief Construct an Edit Rule Dialog
	 * \param subject The Rule to edit
	 * \param parent The parent widget of the dialog
	 */
	explicit RuleDialog(const Rule &subject, QWidget *parent = 0);

	/// Return the domain that was selected as a target for this rule
	Domain targetDomain() const;

	/// Spawn a dialog for editing the Rule whose id is passed as a parameter
	static void editRule(int id);

private:
	/// The combo box used to select the target domain of the Rule
	QComboBox _domain_combo_box;

	/// Initialise the parts of the dialog that are common to both constructors
	void initDialog();

};

#endif // RULEDIALOG_H
