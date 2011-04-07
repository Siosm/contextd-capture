#ifndef CONFIGURATIONSTACK_H
#define CONFIGURATIONSTACK_H

/**
 * \file configurationstack.h
 * \brief
 * \author Steve Dodier (steve dot dodier at ensi-bourges dot fr)
 * \date 2010-06-15
 */

#include <QObject>
#include <QVector>
#include <QPair>
#include <QTimer>
#include "configuration.h"
#include "warning.h"

#define CHANGE_TYPE_NEW				"new"
#define CHANGE_TYPE_CONFIGURATION	"configuration"
#define CHANGE_TYPE_DOMAIN			"domain"
#define CHANGE_TYPE_ALL_RULES		"allRules"
#define CHANGE_TYPE_LOOPS			"loops"
#define CHANGE_TYPE_RULES			"rules"

/// A Class that allows managing several instances of Configuration
class ConfigurationStack : public QObject
{
    Q_OBJECT
public:
	/// Returns an instance
	static ConfigurationStack &instance();

	/// The instance of Configuration currently in use.
	const Configuration &current() const;

	/// Same as instance, but allows modifying the returned object.
	Configuration &currentForModifying();

	/// Tells whether the current instance has changed since it was last saved to a file.
	bool hasChanges() const;

	/// Set the parent widget to use for displaying error dialogs.
	void setParentWidget(QWidget *parent);

	/// Returns the last valid file path used to save the instance
	QString transitionsFilePath(QString alternative=QString()) const;

	/// A dialog to propose to save the current Configuration to a file, returns false if user pressed Cancel
	bool proposeSaveTransitions();

	/// Open a file with an already specified file path
	void openTransitionsFile(QString path);
	void openTmpTransitions(QString path);

	/// Add a program to the programs map and the instances of configuration
	void addProgram(Program p, QString path = QString());

	/// Add programs to the instances of Configuration in the stack
	void addProgramsFromFiles(QStringList paths, bool compact_errors=false);

	/// Update a program for all instances of Configuration in the stack
	void updateProgram(QString old_name, const Program& new_program);

	/// Delete a program from the instances of Configuration in the stack
	void deleteProgram(QString name);

	/// Save all the programs to their respective paths
	void savePrograms();

	/// Find an Abstract Rule (uses the local stack cache or looks in current())
	AbstractRulePtr findAbstractRule(int id);

	/// Get RuleWarnings or WarningTypes for a given rule ID
	int warningTypesForRule(int id) const;
	int warningTypesForRuleAndSourceDomain(int id, QString name) const;
	QVector<WarningPtr> warningsForRule(int id) const;
	bool ruleHasWarnings(int id) const;

	/// Get DomainWarnings or WarningTypes for a given domain name
	int warningTypesForDomain(QString name) const;
	QVector<WarningPtr> warningsForDomain(QString name) const;
	bool domainHasWarnings(QString name) const;

	/// Get all existing warnings
	QVector<WarningPtr> allWarnings() const;

	/// Tells whether it is possible to move in the stack
	bool canUndo() const;
	bool canRedo() const;

	/// Adds and removes objects from the list of things that must not be drawn
	void addHiddenDomain(QString name);
	void addHiddenProgram(QString name);
	void addHiddenRule(int id);
	void removeHiddenDomain(QString name);
	void removeHiddenProgram(QString name);
	void removeHiddenRule(int id);
	bool isDomainHidden(QString name) const;
	bool isProgramHidden(QString name) const;
	bool isRuleHidden(int id) const;
	QStringList hiddenDomains() const;
	QStringList hiddenPrograms() const;
	QList<int> hiddenRules() const;

private:
	QVector<QPair<Configuration *, QString> > _stack;	/// A stack containing instances of Configuration
	int _current_index;									/// The index of the Configuration currently returned by instance()
	int _last_saved_index;								/// The index of the last saved Configuration
	Configuration _current_before_last_change;			/// The state of the current instance before it's last modification
	QString _file_path;									/// The file path to use to save the transitions.xml file of the instance
	QWidget *_parent;									/// A parent widget for the dialogs displayed from the stack
	QMap<QString, QPair<Program, QString> > _programs;	/*! A list of programs known of the instances in the stack
														 *  and the path to their file in the application cache */
	QMap<int, AbstractRulePtr> _arp_cache;				/// A cache for frequently requested loops and rules
	QTimer _autosave_timer;

	/// Objects that are not to be drawn in the graphics view
	QList<int> _hidden_rules;
	QStringList _hidden_domains;
	QStringList _hidden_programs;
	void updateHiddenDomain(QString old_name, QString new_name);
	void updateHiddenProgram(QString old_name, QString new_name);

	/// Existing RuleWarnings per involved rule, updated in real time
	QMap<QString, QVector<WarningPtr> > _dom_warnings;
	QMap<int, QVector<WarningPtr> > _rule_warnings;

	/// RuleWarnings that need to be calculated on user demand
	QMap<QString, QVector<WarningPtr> > _od_dom_warnings;
	QMap<int, QVector<WarningPtr> > _od_rule_warnings;

	/// Constructors with a path parameter for the initial Configuration
	explicit ConfigurationStack(QObject *parent = 0);
	~ConfigurationStack();

	/// Save the current instance in the stack (useful only if it has been modified).
	void pushCurrentInstance(QString changeType=CHANGE_TYPE_NEW);

	/// Initialises the stack with a new Configuration instance.
	void initialise(bool reload_programs=false);

	/// Sets a new file for transitions, empties the stack, and loads a new Configuration
	void reinitialise(QString new_path=QString());

	/// Does save the transitions to the given path
	bool saveTransitions(QString path);

	/// Quickly check that a transitions.xml file is correctly formed
	bool transitionsFileWellFormed(QString path);

	/// Emit a signal for a given type of change between two instances of configuration
	void emitSignalForChangeType(QString changeType, bool revert_changes=false);

	/// Clear the cache of recently requested loops and rules
	void clearARPCache();

	/// Empty the on-demand rule warnings map when data in the stack has changed
	void clearOnDemandWarnings();

signals:
	void domainChanged(QString, QString);
	void programChanged(QString, QString);
	void allRulesChanged();
	void loopsChanged();
	void rulesChanged();
	void configurationChanged();
	void configurationSaved();
	void newConfiguration();
	void stackChanged();
	void hiddenDomainChanged(QString, bool);
	void hiddenProgramChanged(QString, bool);
	void hiddenRuleChanged(int, bool);
	void warningsChanged();

private slots:
	void onDomainChanged(QString old_name, QString new_name);
	void onConfigurationChanged();
	void onProgramChanged(QString old_name, QString new_name);
	void onAllRulesChanged(bool side_effect);
	void onLoopsChanged(bool side_effect);
	void onRulesChanged(bool side_effect);
	void onSaveToTmp();

public slots:
	void onNew();
	void onOpen();
	void onSave();
	void onSaveAs();
	void onUndo();
	void onRedo();
	void onSettingsChange();

	/// Compute real-time (fast to compute) and on-demand (slow) warnings
	void onUpdateWarnings();
	void onUpdateOnDemandWarnings();
};

#endif // CONFIGURATIONSTACK_H
