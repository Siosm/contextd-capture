#ifndef ABSTRACTRULETREEWIDGET_H
#define ABSTRACTRULETREEWIDGET_H

#include <QWidget>
#include <QTreeWidgetItem>
#include "program.h"

namespace Ui {
	class AbstractRuleTreeWidget;
}

/// A tree widget for displaying abstract rules
class AbstractRuleTreeWidget : public QWidget
{
	Q_OBJECT

public:
	/*!
	 * \brief Construct an AbstractRule TreeWidget
	 * \param parent The parent widget of this tree widget
	 */
	explicit AbstractRuleTreeWidget(QWidget *parent = 0);

	/*!
	 * \brief Construct an AbstractRule TreeWidget
	 * \param filtered Whether to filter the rules per program and rule type
	 * \param parent The parent widget of this tree widget
	 */
	explicit AbstractRuleTreeWidget(bool filtered, QWidget *parent = 0);
	~AbstractRuleTreeWidget();

	/// The program to use for filtering
	void setProgram(Program prog);
	Program program() const;

	/// The rule type to use for filtering
	void setRuleType(QString type);
	QString ruleType() const;

	/// The index of the currently selected rule
	int currentIndex() const;

	/// Select the rule at the given index
	void selectIndex(int index);

	/// The ID of the currently selected rule
	int currentRuleId() const;

	/// The rule type of the currently selected rule
	QString currentRuleType() const;

	/// Whether the tree currently has a rule selected
	bool hasSelection() const;

	/// The ID of the rule before the selected one, if any
	int previousRuleId() const;

	/// The ID of the rule after the selected one, if any
	int nextRuleId() const;

	/// Update/Clear the list of rules
	void updateRulesList();
	void clearRulesList();

protected:
    void changeEvent(QEvent *e);

private:
	Ui::AbstractRuleTreeWidget *ui;

	/// Whether the tree's rules are filtered by program / rule type
	bool _filtered;
	Program _program;
	QString _rule_type;

	/// Types of data contained in the tree widget
	enum {
		RuleItem=0,
		MRuleItem,
		MRuleConditionItem,
		ProgramItem,
		SourceDomainItem,
		TargetDomainItem,
		NotifyItem,
		PromptItem
	};
	/// The QTreeWidgetItem data role for the item type
	static const int ItemTypeRole=Qt::UserRole+1;

	void initWidget();

signals:
	void itemSelectionChanged();
	void ruleActivated(int);

private slots:
	void onProgramsOrRulesChanged();
	void onHiddenRuleChanged();
	void onItemActivated(QTreeWidgetItem* item, int column);
	void onItemChanged(QTreeWidgetItem* item, int column);

public slots:
	void onEditCurrentItem();
};

#endif // ABSTRACTRULETREEWIDGET_H
