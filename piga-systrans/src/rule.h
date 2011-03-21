#ifndef RULE_H
#define RULE_H

/**
 * \file rule.h
 * \author Martin Peres (martin dot peres at ensi-bourges dot fr)
 * \date 2010-06-07
 */

#include "abstractrule.h"

/// A transition rule with a specified destination domain. It is the object representation of the <rule> tag in transitions.xml
class Rule : public AbstractRule
{
private:
	/// The destination domain if the rule is matched
	Domain _transit_to;

	Domain targetDomain(Domain current) const;

public:
	Rule();

	/*!
	 * \brief Construct a transition rule.
	 * \param program The application matched by the transition rule
	 * \param transit_to The destination domain of the rule
	 * \param display_name An optional name that should quickly describe the rule
	 */
	Rule(Program program, Domain transit_to, QString display_name=QString());

	/// The destination domain if the rule is matched
	Domain transitTo() const;
	bool setTransitTo(Domain dest);

	/// A string representation of the type of rule
	virtual QString ruleType() const;

	/// Whether the object represents a valid transition rule
	bool isValid() const;
};

inline Domain Rule::transitTo() const
{
	return _transit_to;
}

typedef QSharedPointer<Rule> RulePtr;
#endif // RULE_H
