#ifndef LOOP_H
#define LOOP_H

/**
 * \file loop.h
 * \author Martin Peres (martin dot peres at ensi-bourges dot fr)
 * \date 2010-06-07
 */

#include "abstractrule.h"

/// A transition rule whose target domain is always the source domain
class Loop : public AbstractRule
{
public:
	Loop();

	/*!
	 * \brief Construct a Loop.
	 * \param program The program associated to this loop
	 * \param display_name An optional name meant to quickly describe this loop
	 */
	Loop(Program program, QString display_name=QString());

	/// A string representation of the type of rule
	virtual QString ruleType() const;

private:
	Domain targetDomain(Domain current) const;
};

typedef QSharedPointer<Loop> LoopPtr;
#endif // LOOP_H
