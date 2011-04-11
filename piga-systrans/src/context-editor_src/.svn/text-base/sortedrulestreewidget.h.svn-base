#ifndef SORTEDRULESTREEWIDGET_H
#define SORTEDRULESTREEWIDGET_H

#include <QTreeWidgetItem>
#include <QWidget>
#include "abstractrule.h"
#include "domain.h"

namespace Ui {
	class SortedRulesTreeWidget;
}

/// A tree widget for displaying abstract rules sorted by domain or application
class SortedRulesTreeWidget : public QWidget
{
    Q_OBJECT
public:
	/*!
	 * \brief Construct a Sorted Rules TreeWidget
	 * \param parent The parent widget of this tree widget
	 */
	explicit SortedRulesTreeWidget(QWidget *parent = 0);
	~SortedRulesTreeWidget();

	/// Add a Rule or Loop to the tree widget
	void addRule(const AbstractRule *rule, const Domain dom);

	/// Clear existing rules
	void clearRules();

protected:
    void changeEvent(QEvent *e);

private:
	Ui::SortedRulesTreeWidget *ui;

	/// Sort modes available for the tree widget
	enum {
		SortByApp=0,
		SortByDomain
	};
	short _sort_mode;

	/// Types of data contained in the tree widget
	enum {
		ProgramItem=0,
		DomainItem,
		RuleItem,
		MRuleItem,
		MRuleConditionItem
	};
	/// The QTreeWidgetItem data role for the item type
	static const int ItemTypeRole=Qt::UserRole+1;

private slots:
	void onSortByAppClicked();
	void onSortByDomainClicked();

	void onItemActivated(QTreeWidgetItem* item, int column);

signals:
	void domainActivated(QString name);
	void programActivated(QString name);
	void ruleActivated(int id);
};

#endif // SORTEDRULESTREEWIDGET_H
