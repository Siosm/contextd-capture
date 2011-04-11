#ifndef BROWSERDOCK_H
#define BROWSERDOCK_H

#include <QListWidgetItem>
#include "abstractruletreewidget.h"
#include "editordock.h"

namespace Ui {
    class BrowserDock;
}

/// A dock for displaying lists of domains/programs/rules
class BrowserDock : public EditorDock
{
    Q_OBJECT
public:
	/*!
	 * \brief Construct a Browser Dock
	 * \param parent The parent widget of the dock
	 */
    explicit BrowserDock(QWidget *parent = 0);
    ~BrowserDock();


protected:
	void changeEvent(QEvent *e);

private:
    Ui::BrowserDock *ui;

	/// The widget used for displaying abstract rules
	AbstractRuleTreeWidget _abs_rules_tree;

	/// Available view modes
	enum {
		DomainMode=0,
		ApplicationMode,
		RuleMode
	};
	int _view_mode;

	/// Change/Fetch what should be displayed in the browser
	void setViewMode(int mode);
	int viewMode() const;

	/// Functions for updating the browser's content
	void updateApplicationsList();
	void updateDomainsList();
	void updateRulesList();

signals:
	void viewModeChanged(int);

	/// App/Domain focus buttons
	void applicationFocused(QString);
	void domainFocused(QString);

public slots:
	void onAddProgram();
	void onAddDomain();
	void onAddRule();
	void onUpdateDock();

private slots:
	/// Handler for changes of the view mode in the view mode combo box
	void onViewModeChanged(int index);

	/// Handler for enabling/disabling buttons depending on the selection
	void onCurrentSelectionChanged();

	void onDomainChanged();
	void onProgramsChanged();

	void onAppEditClicked();
	void onAppCloseClicked();
	void onAppFocusClicked();

	void onDomainDeleteClicked();
	void onDomainEditClicked();
	void onDomainFocusClicked();

	void onRuleDeleteClicked();
	void onRuleEditClicked();
	void onPriorityDownClicked();
	void onPriorityUpClicked();

	/// Handler for when an item is double clicked
	void onItemActivated(QListWidgetItem *item);

	/// Handler for when an item's checkbox is checked/unchecked
	void onListItemChanged(QListWidgetItem *item);

	void onHiddenDomainChanged();
	void onHiddenProgramChanged();
};

#endif // BROWSERDOCK_H
