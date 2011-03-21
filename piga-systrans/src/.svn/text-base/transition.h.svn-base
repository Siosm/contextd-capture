#ifndef TRANSITION_H
#define TRANSITION_H

/**
 * \file transition.h
 * \author Martin Peres (martin dot peres at ensi-bourges dot fr)
 * \date 2010-06-07
 */

#include <QMap>
#include <QString>

#include "domain.h"
#include "program.h"

class Transition
{
	bool valid;

	Program _program;
	Domain _transit_from;
	Domain _transit_to;
	bool _prompt, _notify;

public:
	Transition();
	Transition(Program program, Domain transit_from, Domain transit_to, bool prompt_user, bool notify_user);

	bool isValid() const;

	QString appName() const;
	Program program() const;
	Domain transitTo() const;
	Domain transitFrom() const;
	bool promptUser() const;
	bool notifyUser() const;

	bool operator==(const Transition& b) const;
	bool operator!=(const Transition& b) const;
};

inline bool Transition::isValid() const
{
	return valid;
}

inline QString Transition::appName() const
{
	return _program.name();
}

inline Program Transition::program() const
{
	return _program;
}

inline Domain Transition::transitTo() const
{
	return _transit_to;
}
inline Domain Transition::transitFrom() const
{
	return _transit_from;
}

inline bool Transition::promptUser() const
{
	return _prompt;
}

inline bool Transition::notifyUser() const
{
    return _notify;
}

#endif // TRANSITION_H
