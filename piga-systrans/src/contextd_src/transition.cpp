#include "transition.h"
#include <QRegExp>
#include "eventdispatcher.h"
#include "context-common.h"

Transition::Transition() :  valid(false), localTransition(true)
{
}

Transition::Transition(Program program, QString transit_from, Context transit_to, bool local_transition, bool prompt_user, bool notify_user) :\
		valid(true), \
		p_program(program), \
		str_transit_from(transit_from), \
		_transit_to(transit_to), \
		localTransition(local_transition), \
                prompt(prompt_user), \
                notify(notify_user)
{

}

bool Transition::addRule(QString name, QString value)
{
	if(rules.find(name)==rules.end())
	{
		rules[name]=value;
		return true;
	}
	else
	{
		EventDispatcher::instance().sendDebug("Trying to add the rule '" + name + "' to the application '" + appName() +"' but the rule already exists\n");
		return false;
	}
}

bool Transition::matches(QString current_state, QMap<QString, QString> values) const
{
	QMap<QString, QString> paramsUsed;

	//If the current state is compatible with this transition rule
	if(QRegExp(transitFrom()).exactMatch(current_state))
	{
		QMap<QString, QString>::iterator i;
		for(i=values.begin(); i!=values.end(); ++i)
		{
			//If the rule doesn't exist, consider it as being true
			if(rules.find(i.key())!=rules.end())
			{
				QRegExp regexp(rules.find(i.key()).value());

				//If the regexp is not valid, advertise for it and continue
				if(regexp.isValid())
				{
					QString val = i.value();
					// correct problem with clawsmail
					if ( (QString::compare(i.key(), CONTEXT_FROM, Qt::CaseInsensitive)==0) ||
						 (QString::compare(i.key(), CONTEXT_TO, Qt::CaseInsensitive)==0)  )
					{
						if ((val.contains("$lt;")) && (val.length()>4)){
							val = val.remove("$lt;");
							if (val.contains(">")){
								val = val.left(val.indexOf(">"));
							}
						}
					}
					//If the regexp is matched, then continue, otherwise, return false
					if(!regexp.exactMatch(val))
					{
						//EventDispatcher::instance().sendDebug("The rule "+i.key()+" (" + appName() + "," + transitFrom() + "," + transitTo().name() + ") has not been matched because : " + rules.find(i.key()).value() + " was not matched by : " + val + "\n");
						return false;
					}

					//Tell the rule i.key() has been matched
					paramsUsed[i.key()]="yes";
				}
				else
					EventDispatcher::instance().sendError("	--> " + i.key() + " is not a valid regexp : "+regexp.errorString()+"\n");
			}
			//EventDispatcher::instance().sendDebug("The rule " +  i.key() + " has not been found in the current rule.");
		}

		//Only return true if all the params of the rule have been used !
		return paramsUsed.size()==rules.size();
	}
	/*else
		EventDispatcher::instance().sendDebug("The current state ("+current_state+") is not compatible with the transit_from transition rule("+transitFrom()+").");*/

	return false;
}

bool Transition::operator==(const Transition& b) const
{
	return appName()==b.appName() && transitFrom()==b.transitFrom() && transitTo()==b.transitTo() && isALocalTransition()==b.isALocalTransition() && promptUser()==b.promptUser() && notifyUser()==b.notifyUser();
}

bool Transition::operator!=(const Transition& b) const
{
	return !operator==(b);
}
