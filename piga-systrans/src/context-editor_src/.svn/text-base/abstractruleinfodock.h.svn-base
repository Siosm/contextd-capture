#ifndef ABSTRACTRULEINFODOCK_H
#define ABSTRACTRULEINFODOCK_H

#include <QCloseEvent>
#include <QTreeWidgetItem>
#include "editordock.h"
#include "loop.h"
#include "rule.h"
#include "warning.h"

namespace Ui {
    class AbstractRuleInfoDock;
}

/// A dock displaying information about an abstract rule, and warnings related to this rule
class AbstractRuleInfoDock : public EditorDock
{
    Q_OBJECT
public:
	/*!
	 * \brief Construct an AbstractRule Info Dock
	 * \param parent The parent widget of this dock
	 */
    explicit AbstractRuleInfoDock(QWidget *parent = 0);
    ~AbstractRuleInfoDock();

	/// Set the loop or rule to display in the dock
	void setRule(int id);

	/// Whether the dock contains data related to an existing rule
	bool isValid() const;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::AbstractRuleInfoDock *ui;

	/// Id of the currently set rule
	int _current_rule_id;

	/// Utility function for adding a rule warning to the warnings area
	void addRuleWarning(const RuleWarning &warn);

	/// Functions to update and reset the contents of the dock
	void updateDock();
	void resetDock();

	/// A function for clearing the content of the warning area
	void clearWarningArea();

private slots:
	void onRulesChanged();
	void onEditButtonClicked();
	void onDomainActivated(QTreeWidgetItem *item, int column);

	/// Handler for clicks on links inside the warning area
	void onLinkActivated(QString uri);

};

#endif // ABSTRACTRULEINFODOCK_H
