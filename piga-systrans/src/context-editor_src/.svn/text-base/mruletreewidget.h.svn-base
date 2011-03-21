#ifndef MRULETREEWIDGET_H
#define MRULETREEWIDGET_H

#include <QWidget>
#include "mrule.h"
#include "program.h"

namespace Ui {
	class MRuleTreeWidget;
}

/// A class for adding / editing / deleting a list of MRules
class MRuleTreeWidget : public QWidget
{
    Q_OBJECT

public:
	/*!
	 * \brief Construct a MRule TreeWidget
	 * \param parent The parent widget of the tree widget
	 */
	explicit MRuleTreeWidget(QWidget *parent = 0);
	~MRuleTreeWidget();

	/// Return the count of rules currently in the tree widget
	int ruleCount() const;

	/// Add a rule to the widget, given an action name, a list of conditions, and a display name
	void addRule(QString action_name, QMap<QString, QString> map, QString name);

	/// Return the list of matching rules currently in the tree widget
	QList<MRule> matchingRules() const;

protected:
    void changeEvent(QEvent *e);

private:
	Ui::MRuleTreeWidget *ui;

	/// A copy of the program which the mrules are meant to be used with
	Program _prog;

signals:
	void ruleCountChanged(int);

public slots:
	/// Set the program to use for checking the validity of the rules
	void onProgramChanged(Program &p);

private slots:
	void onRuleViewSelectionChanged();
	void onRuleAddClicked();
	void onRuleEditClicked();
	void onRuleRemoveClicked();
};

#endif // MRULETREEWIDGET_H
