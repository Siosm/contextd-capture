#include "automatamaps.h"

AutomataMaps &AutomataMaps::instance()
{
	static AutomataMaps map;

	return map;
}

AutomataMaps::AutomataMaps()
{
}


void AutomataMaps::addAutomaton(const RegExp &expression, const Automaton &aut)
{
	if(!_automata.contains(expression))
		_automata[expression]=aut;
	else
		qWarning("Automaton already contained in reToDfa");

	if(!_equiv[aut].contains(expression))
		_equiv[aut].append(expression);
	else
		qWarning("Expression already contained in dfaToRe");
}

const Automaton &AutomataMaps::getAutomatonForExpression(const RegExp &expression)
{
	if(!_automata.contains(expression))
	{
		_automata[expression]=Automaton(expression);
	}

	return _automata[expression];
}

QList<RegExp> AutomataMaps::getExpressionsForAutomaton(const Automaton &automaton)
{
	return _equiv.value(automaton, QList<RegExp>());
}

QList<RegExp> AutomataMaps::equivalentExpressions(const RegExp &expression)
{
	if(!_automata.contains(expression))
	{
		QList<RegExp> list;
		list.append(expression);
		return list;
	}
	else
	{
		return _equiv[_automata[expression]];
	}
}
