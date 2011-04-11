#ifndef AUTOMATAMAPS_H
#define AUTOMATAMAPS_H

#include "automaton.h"
#include "state.h"

class AutomataMaps
{
public:
	static AutomataMaps &instance();

	void addAutomaton(const RegExp &expression, const Automaton &aut);

	const Automaton &getAutomatonForExpression(const RegExp &expression);
	QList<RegExp> getExpressionsForAutomaton(const Automaton &automaton);

	QList<RegExp> equivalentExpressions(const RegExp &expression);

protected:
    AutomataMaps();

	QMap<RegExp, Automaton> _automata;
	QMap<Automaton, QList<RegExp > > _equiv;
};

#endif // AUTOMATAMAPS_H
