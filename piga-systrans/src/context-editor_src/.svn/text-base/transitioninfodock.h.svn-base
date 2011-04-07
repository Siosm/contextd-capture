#ifndef TRANSITIONINFODOCK_H
#define TRANSITIONINFODOCK_H

#include <QGraphicsScene>
#include <QVector>
#include <QTreeWidgetItem>
#include "abstractrule.h"
#include "domain.h"
#include "editordock.h"
#include "loop.h"
#include "program.h"
#include "rule.h"
#include "trule.h"

namespace Ui {
	class TransitionInfoDock;
}

/// A dock that displays information on transitions from a domain to a domain for a given program
class TransitionInfoDock : public EditorDock
{
	Q_OBJECT

public:
	/*!
	 * \brief Construct a Transition Info Dock
	 * \param parent The parent widget of the dock
	 */
	explicit TransitionInfoDock(QWidget *parent = 0);
	~TransitionInfoDock();

	/// Set the program, source and target domains of the dock (checks if the data is valid)
	bool setProgram(Program prog);
	bool setSourceDomain(Domain dom);
	bool setTargetDomain(Domain dom);
	bool setAllProperties(Program prog, Domain src, Domain dest);

	/// Fetch the program, source and target domains
	Program program() const;
	Domain sourceDomain() const;
	Domain targetDomain() const;

	/// Check that both domains of the dock are valid
	bool isValid() const;

protected:
	void changeEvent(QEvent *e);

private:
	Ui::TransitionInfoDock *ui;
	QGraphicsScene *_scene;
	Program _program;
	Domain _source;
	Domain _target;

	/// Add an Abstract Rule to the list of rules for this dock, with the transition rule that was matched
	void addAbstractRuleToList(AbstractRulePtr rule, const TRule &trule);

	/// Update or reset the contents of the dock
	void updateDock();
	void resetDock();

public slots:
	/// Slot to trigger an update of the dock
	void onUpdateDock();

private slots:
	/// Button handlers
	void onDeleteClicked();
	void onEditClicked();

	/// Rule activation handler
	void onItemActivated(QTreeWidgetItem *item, int column);

	/// Handler that checks if the edit and delete buttons should be enabled
	void onListSelectionChanged();

	/// Slots to update the dock when the stack's data changes
	void onConfigurationChanged();
	void onDomainChanged(QString oldName, QString newName);
	void onRulesChanged();
	void onProgramChanged(QString oldName, QString newName);
	};

#endif // TRANSITIONINFODOCK_H
