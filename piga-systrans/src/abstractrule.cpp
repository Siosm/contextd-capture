#include "abstractrule.h"

int AbstractRule::idCounter = AbstractRule::InvalidPriority;
QMutex AbstractRule::idCounterMutex;

AbstractRule::AbstractRule() :
		_id(AbstractRule::InvalidId),
		_priority(AbstractRule::InvalidPriority)
{

}

AbstractRule::AbstractRule(Program program, QString display_name) :
		_program(program),
		_display_name(display_name)
{
	idCounterMutex.lock();
	_id=++idCounter;
	_priority=_id;
	idCounterMutex.unlock();
}

int AbstractRule::id() const
{
	return _id;
}

int AbstractRule::priority() const
{
	return _priority;
}

bool AbstractRule::isValid() const
{
	return _id!=InvalidId;
}

bool AbstractRule::setProgram(Program prog, bool allow_invalid)
{
	if(prog.isValid() || allow_invalid)
	{
		_program=prog;
		return true;
	}
	else
	{
		qWarning("Tried to set an invalid program");
		return false;
	}
}

void AbstractRule::setDisplayName(QString display_name)
{
	_display_name=display_name;
}

bool AbstractRule::addMRule(const MRule& mrule)
{
	_mrules.push_back(mrule);
	return true;
}

void AbstractRule::clearMRules()
{
	_mrules.clear();
}

bool AbstractRule::addTRule(const TRule& trule)
{
	_trules.push_back(trule);
	return true;
}

TRule AbstractRule::deleteTRule(QString domain)
{
	QVector<TRule>::iterator it;

	for (it=_trules.begin(); it!=_trules.end(); ++it)
	{
		TRule &trule = *it;

		if(trule.fromDomain() == domain)
		{
			TRule copy = *it;
			_trules.erase(it);
			return copy;
		}
	}

	return TRule();
}

void AbstractRule::clearTRules()
{
	_trules.clear();
}

bool AbstractRule::tRulesContain(QString name, bool regexp_match_allowed) const
{
	bool found=false;

	for(int t=0, size=_trules.size(); t<size && !found; t++)
		if(regexp_match_allowed)
			found = _trules[t].matches(name);
		else
			found = _trules[t].fromDomain() == name;

	return found;
}

bool AbstractRule::tRulesContain(Domain dom, bool regexp_match_allowed) const
{
	return tRulesContain(dom.name(), regexp_match_allowed);
}

QVector<Domain> AbstractRule::matchedDomains(QList<Domain>knownDomains) const
{
	QVector<Domain> matched;
	QList<Domain>::iterator it;

	//Go through each TRule of the rule
	for(int i=0; i<_trules.size() && knownDomains.size()>0; ++i)
	{
		const TRule &trule=_trules.at(i);

		//For each of them, retrieve all the domains it matches
		for(it=knownDomains.begin(); it!=knownDomains.end();)
		{
			//If a domain is matched, transfer it to the matched domains list
			if(trule.matches((*it).name()))
			{
				matched.append(*it);
				it=knownDomains.erase(it);
			}
			else
				++it;
		}
	}

	return matched;
}

QStringList AbstractRule::matchedDomainNames(QStringList knownDomainNames) const
{
	QStringList matched;
	QStringList::iterator it;

	//Go through each TRule of the rule
	for(int i=0; i<_trules.size() && knownDomainNames.size()>0; ++i)
	{
		const TRule &trule=_trules.at(i);

		//For each of them, retrieve all the domains it matches
		for(it=knownDomainNames.begin(); it!=knownDomainNames.end();)
		{
			//If a domain is matched, transfer it to the matched domains list
			if(trule.matches(*it))
			{
				matched.append(*it);
				it=knownDomainNames.erase(it);
			}
			else
				++it;
		}
	}

	return matched;
}

Transition AbstractRule::matches(Program program, Domain current_domain, QMap<QString, QString> values, bool& matchedButRefused) const
{
	//Reset matchedButRefused
	matchedButRefused=false;

	//If  the program is not valid, abort
	if(!program.isValid())
		return Transition();

	//If the program is not the same, just abort
	if(program.name()!=this->program().name())
		return Transition();

	//First, try to match the mrules
	for(int m=0; m<_mrules.size() && !matchedButRefused; m++)
	{
		if(_mrules[m].matches(values))
		{
			//We have matched a rule, let's match a transition
			for(int t=0; t<_trules.size(); t++)
			{
				if(_trules[t].matches(current_domain.name()))
				{
					bool prompt=_trules[t].hasPrompt() ? _trules[t].prompt() : this->_prompt;
					bool notify=_trules[t].hasNotify() ? _trules[t].notify() : this->_notify;

					Transition trans(program, current_domain, targetDomain(current_domain), prompt, notify);
					return trans;
				}
			}

			//We haven't matched any trule. This means we have matched some conditions but not any incoming Domain
			//let's die and return an empty transition
			matchedButRefused=true;
		}
	}

	return Transition();
}

bool AbstractRule::operator <(const AbstractRule &other) const
{
	return this->priority() < other.priority();
}

bool AbstractRule::hasHigherPriority(AbstractRulePtr rule, AbstractRulePtr other)
{
	return rule->operator <(*other);
}

void AbstractRule::swapPriorities(AbstractRule *rule, AbstractRule *other)
{
	int tmp = rule->_priority;
	rule->_priority = other->_priority;
	other->_priority = tmp;
}
