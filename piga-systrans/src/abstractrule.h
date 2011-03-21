#ifndef ABSTRACTRULE_H
#define ABSTRACTRULE_H

/**
 * \file abstractrule.h
 * \author Steve Dodier (steve dot dodier at ensi-bourges dot fr)
 * \date 2010-06-07
 */

#include <QVector>
#include <QMutex>
#include <QSharedPointer>
#include <limits.h>

#include "domain.h"
#include "mrule.h"
#include "trule.h"
#include "transition.h"

class AbstractRule;
typedef QSharedPointer<AbstractRule> AbstractRulePtr;

/// Define a set of transitions. It is the object representation of the <loop> and <rule> tags in transitions.xml
class AbstractRule
{
protected:
	/// The program associated with this rule
	Program _program;

	/// A display name quickly describing this rule
	QString _display_name;

private:
	static QMutex idCounterMutex;
	static int idCounter;
	/// Unique identifier
	int _id;

	/// Same as _id but can be swapped with another rule's priority later
	int _priority;

	/// Default prompt parameter
	bool _prompt;

	/// Default notify parameter
	bool _notify;

	/// List of matching rules (conditions)
	QVector<MRule> _mrules;

	/// List of transition rules (domains)
	QVector<TRule> _trules;

	/// The target domain when matching a rule
	virtual Domain targetDomain(Domain current) const = 0;

public:
	static const int InvalidId = 0;
	static const int InvalidPriority = 0;

	AbstractRule();

	/*!
	 *\brief Construct an abstract transition rule.
	 *\param program The program matched by this rule
	 *\param display_name An optional display name, quickly describing this rule
	 */
	AbstractRule(Program program, QString display_name=QString());

	/// A string representation of the type of rule, used to differentiate child objects
	virtual QString ruleType() const = 0;

	/// An unique identifier for the rule object
	int id() const;

	/// An unique integer representing the priority of the rule compared to others
	int priority() const;

	/// Whether this object represents a valid rule
	virtual bool isValid() const;

	/// The name of the application matched by this rule
	QString appName() const;

	/// The program associated with this rule
	Program program() const;
	bool setProgram(Program prog, bool allow_invalid=false);

	/// A display name quickly describing this rule
	QString displayName() const;
	void setDisplayName(QString display_name);

	/// Default prompt parameter
	bool prompt() const;
	void setPrompt(bool value);

	/// Default notify parameter
	bool notify() const;
	void setNotify(bool value);

	/// A list of matching rules representing the conditions for this rule to be matched
	QVector<MRule> mRules() const;
	bool addMRule(const MRule& mrule);
	void clearMRules();

	/// A list of transition rules representing domains from which the rule can be matched
	QVector<TRule> tRules() const;
	bool addTRule(const TRule& trule);
	TRule deleteTRule(const QString domain);
	void clearTRules();

	/// Whether this rule can be matched from a given domain
	bool tRulesContain(QString name, bool regexp_match_allowed=true) const;
	bool tRulesContain(Domain dom, bool regexp_match_allowed=true) const;

	/// Directly return the names of all the matched domains
	QVector<Domain> matchedDomains(QList<Domain> knownDomains) const;

	/// Directly return the names of all the matched domains
	QStringList matchedDomainNames(QStringList knownDomainNames) const;

	Transition matches(Program program, Domain current_domain, QMap<QString, QString> values, bool& matchedButRefused) const;

	/// Whether this rule's priority index is more little than the other rule's (means this rule is more important)
	bool operator <(const AbstractRule &other) const;

	/// Whether this rule's priority index is more little than the other rule's (means this rule is more important)
	static bool hasHigherPriority(const AbstractRulePtr rule, const AbstractRulePtr other);

	/// Exchange the priority of two existing rules.
	static void swapPriorities(AbstractRule *rule, AbstractRule *other);
};

inline QString AbstractRule::appName() const
{
	return _program.name();
}

inline Program AbstractRule::program() const
{
	return _program;
}

inline QString AbstractRule::displayName() const
{
	return _display_name;
}

inline QVector<MRule> AbstractRule::mRules() const
{
	return _mrules;
}

inline QVector<TRule> AbstractRule::tRules() const
{
	return _trules;
}

inline bool AbstractRule::prompt() const
{
	return _prompt;
}

inline void AbstractRule::setPrompt(bool value)
{
	_prompt=value;
}

inline bool AbstractRule::notify() const
{
	return _notify;
}

inline void AbstractRule::setNotify(bool value)
{
	_notify=value;
}

#endif // ABSTRACTRULE_H
