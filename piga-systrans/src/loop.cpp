#include "loop.h"

Loop::Loop() :
		AbstractRule()
{
}

Loop::Loop(Program program, QString display_name) :
		AbstractRule(program, display_name)
{
}

Domain Loop::targetDomain(Domain current) const
{
	return current;
}

QString Loop::ruleType() const
{
	return "Loop";
}
