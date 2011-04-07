#ifndef DOMAINLOOPSTREEWIDGET_H
#define DOMAINLOOPSTREEWIDGET_H

#include <QTreeWidgetItem>
#include <QWidget>
#include "abstractrule.h"
#include "domain.h"

namespace Ui {
	class DomainLoopsTreeWidget;
}

/// A tree widget displaying transition rules and loops without the domain information
class DomainLoopsTreeWidget : public QWidget
{
    Q_OBJECT
public:
	/*!
	 * \brief Construct a Sorted Rules TreeWidget
	 * \param parent The parent widget of this tree widget
	 */
	explicit DomainLoopsTreeWidget(QWidget *parent = 0);
	~DomainLoopsTreeWidget();

	/// Add a rule to the widget's tree
	void addRule(const AbstractRule *rule);

	/// Clear rules in the widget's tree
	void clearRules();

protected:
    void changeEvent(QEvent *e);

private:
	Ui::DomainLoopsTreeWidget *ui;

	/// Types of data contained in the tree widget
	enum {
		ProgramItem=0,
		RuleItem,
		MRuleItem,
		MRuleConditionItem
	};
	/// The QTreeWidgetItem data role for the item type
	static const int ItemTypeRole=Qt::UserRole+1;

private slots:
	void onItemActivated(QTreeWidgetItem *item, int column);

signals:
	void programActivated(QString name);
	void ruleActivated(int id);
};

#endif // DOMAINLOOPSTREEWIDGET_H
