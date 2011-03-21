#include "automaton.h"
#include <QFile>

Automaton::Automaton()
{
}

Automaton::Automaton(const RegExp &expression) :
		_initial_state(expression)
{
	_states.insert(expression);

	StateList toDo;
	StateList done;

	//STATES == DONE

	toDo.append(expression);
	while(!toDo.isEmpty())
	{
		const RegExp &p=toDo.takeLast();
		done.append(p);

		QVector<Derivation> destinations=p.leftDerivations();
		for(int i=0; i<destinations.size(); ++i)
		{
			const Derivation& deriv=destinations.at(i);

			//TODO RegExp simplifier
			if(!done.contains(deriv.expr) && (!toDo.contains(deriv.expr)))
				toDo.append(deriv.expr);

			//TODO RegExp simplifier
			//FIXME appending to toDo triggers appending to Done, so _states can be deduced from done
			if(!_states.contains(deriv.expr))
				_states.insert(deriv.expr);

			_transitions[p].append(deriv);
		}

		if(p.isNullable())
			_final_states.append(p);
	}
}

void Automaton::toDot() const
{
	QString dot="digraph automata {\nrankdir=LR;\nsize=\"8\";\n";

	QMap<State, int> dotID;
	int i=0;
	for(QSet<State>::const_iterator it=_states.begin(); it!=_states.end(); ++it)
		dotID[*it]=i++;

	QList<State> states=_states.toList();

	//Special shape for initial state
	if(_final_states.contains(_initial_state))
		dot+="node [shape=doubleoctagon];\n";
	else
		dot+="node [shape=octagon];\n";

	states.removeAll(_initial_state);
	dot+=QString("node [label=\""+_initial_state.pattern()+"\"] ND_%2 ;\n").arg(dotID[_initial_state]);

	//Special shape for final states
	dot+="node [shape=doublecircle];\n";
	for(int i=0; i<_final_states.size(); ++i)
	{
		const State &p=_final_states.at(i);
		states.removeAll(p);

		dot+=QString("node [label=\""+p.pattern()+"\"] ND_%2 ;\n").arg(dotID[p]);
	}

	//Normal shape for normal states
	dot+="node [shape=circle];\n";
	for(int i=0; i<states.size(); ++i)
	{
		const State &p=states.at(i);
		dot+=QString("node [label=\""+p.pattern()+"\"] ND_%2 ;\n").arg(dotID[p]);
	}

	//Transitions
	for(QMap<State, QVector<Derivation> >::const_iterator it=_transitions.begin(); it!=_transitions.end(); ++it)
	{
		const QVector<Derivation> &deriv=it.value();
		for(int i=0; i<deriv.size(); ++i)
		{
			QString tokenLabel=deriv.at(i).token.data;
			if(deriv.at(i).token.type==RegExp::ExcludeType)
				tokenLabel.prepend("^");
			else if(deriv.at(i).token.type==RegExp::AnyType)
				tokenLabel="*";

			dot+=QString("ND_%1").arg(dotID[it.key()])+" -> "+
				 QString("ND_%1").arg(dotID[deriv.at(i).expr])+" [ label = \""+tokenLabel+"\" ];\n";
		}

	}

	dot+="}";

	QFile out("/tmp/out.gv");
	out.open(QFile::WriteOnly);
	out.write(qPrintable(dot));
}

bool Automaton::operator <(const Automaton &b) const
{
	return _initial_state < b._initial_state;
}

bool Automaton::operator ==(const Automaton &b) const
{
	return false; //TODO

	//Parcours des états en renommant de façon à avoir tjrs le mm ordre (largeur/profondeur puis ordre lexicographique pr transit°)
}

//void Automaton::subset(QMap<RegExp, QVector<Derivation> > transitions,
//					   StateVector start_states,
//					   StateVector final_states)
//{
//	//Clear current automaton
//	_states.clear();
//	_transitions.clear();
//	_final_states.clear();

//	//Start states are, by definition, reachable
//	_start_states=start_states;

//	//Calculate the subset of reachable states of the automaton
//	StateList toDo;
//	StateList done;

//	for(int i=0; i<start_states.size(); ++i)
//		toDo.append(start_states.at(i));

//	//We browse the automaton to find reachable states
//	while(!toDo.isEmpty())
//	{
//		const RegExp &p=toDo.takeFirst();
//		done.append(p);

//		//Since p is reached, add it to states (as well as its transitions)
//		_states.insert(p);
//		const QVector<Derivation> &trans=_transitions[p]=transitions[p];

//		//And to final states if it is final in the original automaton
//		if(final_states.contains(p))
//			_final_states.append(p);

//		for(int i=0; i<trans.size(); ++i)
//		{
//			const RegExp &dest=trans.at(i).expr;
//			if(!done.contains(dest) && !toDo.contains(dest))
//				toDo.append(dest);
//		}
//	}
//}
