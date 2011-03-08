#ifndef TRANSITION_H
#define TRANSITION_H

#include <QMap>
#include <QString>

#include "context.h"
#include "program.h"

class Transition
{
	bool valid;

	QMap<QString, QString> rules;
	Program p_program;
	QString str_transit_from;
	Context _transit_to;
	bool localTransition, prompt, notify;

public:
	Transition();
	Transition(Program program, QString transit_from, Context transit_to, bool local_transition, bool prompt_user, bool notify_user);

	bool isValid() const;

	QString appName() const;
	Program program() const;
	Context transitTo() const;
	QString transitFrom() const;
	bool isALocalTransition() const;
	bool promptUser() const;
	bool notifyUser() const;

	bool addRule(QString name, QString value);

	bool matches(QString current_state, QMap<QString, QString> values) const;

	bool operator==(const Transition& b) const;
	bool operator!=(const Transition& b) const;
};

inline bool Transition::isValid() const
{
	return valid;
}

inline QString Transition::appName() const
{
	return p_program.appName();
}

inline Program Transition::program() const
{
	return p_program;
}

inline Context Transition::transitTo() const
{
	return _transit_to;
}
inline QString Transition::transitFrom() const
{
	return str_transit_from;
}

inline bool Transition::isALocalTransition() const
{
	return localTransition;
}

inline bool Transition::promptUser() const
{
	return prompt;
}

inline bool Transition::notifyUser() const
{
    return notify;
}

#endif // TRANSITION_H
