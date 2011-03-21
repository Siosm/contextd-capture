#ifndef APPINFODOCK_H
#define APPINFODOCK_H

#include "abstractruletreewidget.h"
#include "editordock.h"
#include "program.h"

namespace Ui {
    class AppInfoDock;
}

/// A dock for displaying information related to a Program
class AppInfoDock : public EditorDock
{
    Q_OBJECT
public:
	/*!
	 * \brief Construct an App Info Dock
	 * \param parent The parent widget of the dock
	 */
    explicit AppInfoDock(QWidget *parent = 0);
    ~AppInfoDock();

	/// The program of the dock
	void setProgram(Program prog);
	Program program() const;

	bool isValid() const;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::AppInfoDock *ui;
	/// The widgets used for displaying abstract rules
	AbstractRuleTreeWidget _loops_tree;
	AbstractRuleTreeWidget _rules_tree;

	/// The program of the dock
	Program _program;

	/// Functions for updating the dock's content
	void updateDock();
	void resetDock();

signals:
	void ruleActivated(int);

private slots:
	void onProgramChanged(QString old_name, QString new_name);
	void onEditClicked();
};

#endif // APPINFODOCK_H
