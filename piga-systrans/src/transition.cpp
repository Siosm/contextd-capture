#include "transition.h"
#include <QRegExp>
#include "context-common.h"

Transition::Transition() :  valid(false)
{
}

Transition::Transition(Program program, Domain transit_from, Domain transit_to, bool prompt_user, bool notify_user) :\
		valid(true), \
		_program(program), \
		_transit_from(transit_from), \
		_transit_to(transit_to), \
		_prompt(prompt_user), \
		_notify(notify_user)
{

}

bool Transition::operator==(const Transition& b) const
{
	return valid==b.valid && program()==b.program() && transitFrom()==b.transitFrom() && transitTo()==b.transitTo() && promptUser()==b.promptUser() && notifyUser()==b.notifyUser();
}

bool Transition::operator!=(const Transition& b) const
{
	return !operator==(b);
}
