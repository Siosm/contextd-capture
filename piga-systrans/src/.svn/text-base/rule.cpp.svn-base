#include "rule.h"

Rule::Rule() :
		AbstractRule()
{
}

Rule::Rule(Program program, Domain transit_to, QString display_name) :
		AbstractRule(program, display_name),
		_transit_to(transit_to)
{
}

bool Rule::setTransitTo(Domain dest)
{
	if (dest != Domain())
	{
		_transit_to=dest;
		return true;
	}
	else
	{
		qWarning("Tried to set an invalid destination domain.");
		return false;
	}
}

Domain Rule::targetDomain(Domain /*current*/) const
{
	return transitTo();
}

QString Rule::ruleType() const
{
	return "Transition";
}

bool Rule::isValid() const
{
	return AbstractRule::isValid() && (_transit_to!=Domain());
}
