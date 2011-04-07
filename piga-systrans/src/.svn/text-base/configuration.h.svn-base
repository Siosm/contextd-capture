#ifndef CONFIGURATION_H
#define CONFIGURATION_H

/**
 * \file configuration.h
 * \brief
 * \author Martin Peres (martin dot peres at ensi-bourges dot fr)
 * \date 2010-06-07
 */

#include "rule.h"
#include "loop.h"
#include "domain.h"
#include "program.h"
#include <QMap>
#include <QVector>
#include <QList>
#include <QSharedPointer>
#include <QString>
#include <QMutex>
#include <QtXml/QDomDocument>

/// The class that holds all the information stored into transitions.xml
class Configuration : public QObject
{
	friend class ConfigurationStack;
	Q_OBJECT


private:
	//A DOM document representing the transitions.xml rules file
	QDomDocument _transitions_doc;
	QString _transitions_path;

	//We associate the program name to its related rules (the vector)
	//that finally leads to a map associating a parameter name to its regexp
	QMap<QString, QVector<Rule> > _rules;

	//We associate the program name to its related loops (the vector)
	//that finally leads to a map associating a parameter name to its regexp
	QMap<QString, QVector<Loop> > _loops;

	//Domains
	QMap<QString, Domain > _domains;
	Domain _default_domain;

	//Programs
	QMap<QString, Program > _programs;
	QList<QString> _no_kill_programs;
	bool _parse_programs;

	/*###### NOTE TO DEVELOPERS #########################################################
	 #																					#
	 # If you add a parameter to this class don't forget to update the = operator.		#
	 #																					#
	 ###################################################################################*/

	//We forbid the creation of multiple configuration objects by putting constructors in private

	/*!
	 * \brief Construct an instance of Configuration
	 * \param path The path to an optional transitions.xml file (if empty, will begin with no rules)
	 * \param parse_programs If set, will parse /etc/context.d/program.d/ for initial programs
	 * \param programs An optional list of initial programs that will be added to the instance
	 */
	Configuration(const QString &path=QString(), bool parse_programs=true, QList<Program> programs=QList<Program>());
	Configuration(const Configuration &other);
	const Configuration &operator =(const Configuration &other);

	//XML Parsing
	void parseTransitionsFile();
	bool parseMRules(QDomElement &parent, AbstractRule *rule);
	bool parseTRulesForLoop(QDomElement &parent, Loop &loop);
	bool parseTRulesForRule(QDomElement &parent, Rule &rule);
	void parseLoop(QDomElement &element);
	void parseRule(QDomElement &element);
	void parseNoKillProgram(QDomElement &element);
	void parseDomain(QDomElement &element);

	//Program management
	void parsePrograms();
	bool addProgram(Program p, bool quiet);

	//Read the configuration from file
	void readConf();

	//Quiet versions of functions for loops and rules (to avoid signal spamming)
	void addLoop(Loop loop, bool quiet);
	void addRule(Rule rule, bool quiet);
	void updateLoop(Loop loop, bool quiet);
	void updateRule(Rule rule, bool quiet);
	Loop deleteLoop(int id, bool quiet);
	Rule deleteRule(int id, bool quiet);
	void deleteLoop(Loop &loop, bool quiet);
	void deleteRule(Rule &rule, bool quiet);

public:
	//Get the only instance of the class
	static const Configuration& instance();
	static Configuration& instance_nonconst();

	//Saving of the transitions.xml DOM tree
	QString retrieveTransitionsDOM() const;


	//Domains
	Domain getDomainByName(QString name) const;
	const Transition getFirstMatchingTransition(Program app, Domain current_domain, QMap<QString, QString> values) const;
	Domain defaultDomain() const;
	QList<QString> domainNameList() const;
	QList<Domain> domainList() const;
	int domainCount() const;
	void addDomain(Domain dom, bool set_default=false);
	void updateDomain(QString oldName, Domain dom, bool set_default=false);
	Domain deleteDomain(QString name, QString new_default=QString());
	
	//Programs
	void addProgram(Program p);
	Program updateProgram(QString old_name, const Program& prog);
	Program deleteProgram(QString name);
	bool isProgramAllowed(QString name, QString full_path) const;
	Program getProgramByName(QString name, bool allow_invalid=false) const;
	QList<Program> programList() const;
	QList<QString> programNameList() const;
	QList<QString> noKillProgramList() const;
	bool isProgramKillable(const QString& full_path) const;

	//Reload
	void updateRules();

	//Find Rules and Loops for a given program or domain
	QVector<AbstractRulePtr> allAbstractRules(bool sort=false) const;
	QVector<Loop> allLoops() const;
	QVector<Rule> allRules() const;
	QVector<AbstractRulePtr> abstractRulesForProgram(QString name, bool sort=false) const;
	QVector<Loop> loopsForProgram(QString name) const;
	QVector<Rule> rulesForProgram(QString name) const;
	QVector<AbstractRulePtr> abstractRulesForDomain(Domain dom, bool sort=false) const;
	QVector<Loop> loopsForDomain(Domain dom) const;
	QVector<Rule> rulesForDomain(Domain dom) const;

	//Add Rules and Loops to the instance - not implemented yet
	void addLoop(Loop loop);
	void addRule(Rule rule);

	//Update a Rule or Loop according to it's ID
	void updateLoop(Loop loop);
	void updateRule(Rule rule);

	//Get a copy of an existing Rule or Loop
	AbstractRulePtr findAbstractRule(int id) const;
	Loop findLoop(int id) const;
	Rule findRule(int id) const;

	//Delete an existing Rule or Loop, and retrieve it
	AbstractRulePtr deleteAbstractRule(int id);
	void deleteAbstractRule(AbstractRulePtr &rule);
	Loop deleteLoop(int id);
	void deleteLoop(Loop &loop);
	Rule deleteRule(int id);
	void deleteRule(Rule &rule);

	void swapPriorities(int id, int other_id);

signals:
	//Many things in the Configuration changed, should reload all
	void configurationChanged();

	//old_name is the id of the domain before it changed, new_name is it's new id
	void domainChanged(QString old_name, QString new_name);
	void programChanged(QString old_name, QString new_name);

	//side_effect tells if the rules changed due to a change in the domains
	void allRulesChanged(bool side_effect=false);
	void loopsChanged(bool side_effect=false);
	void rulesChanged(bool side_effect=false);
};

#endif // CONFIGURATION_H
