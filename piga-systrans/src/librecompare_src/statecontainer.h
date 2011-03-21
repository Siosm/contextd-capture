#ifndef STATECONTAINER_H
#define STATECONTAINER_H

#include <QList>
#include <QSet>
#include <QVector>

#include "state.h"

template <class ForwardIterator>
bool StateContains(ForwardIterator first, ForwardIterator last, const State& t)
{
	for(ForwardIterator it=first; it!=last; ++it)
	{
		if(*it==t) //FIXME
			return true;
	}

	return false;
}

class StateList : public QList<State> {
public:
	bool contains(const State &t) const
	{
		return StateContains<QList<State>::const_iterator>(begin(), end(), t);
	}
};

class StateSet : public QSet<State> {
public:
	bool contains(const State &t) const
	{
		return StateContains<QSet<State>::const_iterator>(begin(), end(), t);
	}
};

class StateVector : public QVector<State> {
public:
	bool contains(const State &t) const
	{
		return StateContains<QVector<State>::const_iterator>(begin(), end(), t);
	}
};

#endif // STATECONTAINER_H
