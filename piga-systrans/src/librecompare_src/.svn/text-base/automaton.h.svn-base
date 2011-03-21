#ifndef AUTOMATON_H
#define AUTOMATON_H

#include <QMap>
#include <QPair>
#include <QSet>
#include <QVector>

#include "regexp.h"
#include "state.h"
#include "statecontainer.h"

class Automaton
{
public:
	Automaton();
	Automaton(const RegExp &expression);

	void toDot() const;

	bool operator <(const Automaton &b) const;
	bool operator ==(const Automaton &b) const;

//	void subset(QMap<State, QVector<Derivation> > transitions,
//				StateVector start_states,
//				StateVector final_states);

private:
	StateSet _states;
	QMap<State, QVector<Derivation> > _transitions;
	State _initial_state;
	StateVector _final_states;
};

#endif // AUTOMATON_H
