#include "mrule.h"
#include <QRegExp>
#include "context-common.h"

#include <stdio.h>

//#include "eventdispatcher.h"

MRule::MRule(Program prog, QString action, QString display_name) :
		_prog(prog),
		_action(action),
		_display_name(display_name)
{
	if(prog==Program())
		qDebug("MRule created without a program, no integrity checking possible.");
}

bool MRule::addCondition(QString name, QString value)
{
	ProgramVariable var;
	bool varExists=false;

	if(_prog!=Program())
	{
		//Check that the variable exists in the program
		var=_prog.getVariableByName(name, _action);
		if(var==ProgramVariable())
			qWarning("MRule with program '%s' will never be matched because variable '%s' does not exist.", qPrintable(_prog.displayName()), qPrintable(name));
		else
			varExists=true;

		//If there is already an action defined, check that the variable's action is the same
		QString act=var.action();
		if(_action!=QString() && (act!=_action))
				qWarning("MRule may not be matched because variable '%s' does not belong to MRule's action '%s'.", qPrintable(name), qPrintable(_action));
	}

	//Adding the condition to the rule
	if(_conditions.find(name)==_conditions.end())
	{
		if(varExists)
			ProgramVariable::addAssignedValue(var.name(), var.type(), value);

		_conditions[name]=value;
		return true;
	}
	else
	{
		qWarning("%s\n", qPrintable("Trying to add the condition '" + name + "' to MRule, but it already exists."));
		return false;
	}
}

void MRule::setActionName(QString action_name)
{
	_action = action_name;
}

void MRule::setDisplayName(QString name)
{
	_display_name = name;
}

bool MRule::matches(QMap<QString, QString> values) const
{
	QMap<QString, QString> paramsUsed;

	QMap<QString, QString>::iterator i;
	for(i=values.begin(); i!=values.end(); ++i)
	{
		//If the rule doesn't exist, consider it as being true
		if(_conditions.find(i.key())!=_conditions.end())
		{
			QRegExp regexp(_conditions.find(i.key()).value());

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
					//EventDispatcher::instance().sendDebug("The rule "+i.key()+" (" + appName() + "," + transitFrom() + "," + transitTo().name() + ") has not been matched because : " + conditions.find(i.key()).value() + " was not matched by : " + val + "\n");
					//fprintf(stderr, qPrintable("The rule '"+i.key()+" has not been matched because the condition '" + conditions.find(i.key()).value() + "' was not matched by '" + val + "'\n"));
					return false;
				}

				//Tell the rule i.key() has been matched
				paramsUsed[i.key()]="yes";
			}
			else
				qDebug("%s\n",qPrintable("	--> " + i.key() + " is not a valid regexp : "+regexp.errorString()+"\n"));
		}
		//EventDispatcher::instance().sendDebug("The rule " +  i.key() + " has not been found in the current rule.");
	}

	//Only return true if all the params of the rule have been used !
	return paramsUsed.size()==_conditions.size();
}

/*bool MRule::operator==(const MRule& b) const
{
	return
}

bool MRule::operator!=(const MRule& b) const
{
	return !operator==(b);
}*/

QMap<QString, QString> MRule::conditions() const
{
	return _conditions;
}

QString MRule::actionName() const
{
	return _action;
}

QString MRule::displayName() const
{
	return _display_name;
}
