#include "programaction.h"

ProgramAction::ProgramAction()
{

}

ProgramAction::ProgramAction(const QString& name, const QString& displayName, const QString& description) :
		_name(name), _displayName(displayName), _description(description)
{
}

QString ProgramAction::name() const
{
	return _name;
}

void ProgramAction::setName(const QString& name)
{
	_name=name;
}

QString ProgramAction::displayName() const
{
	return _displayName;
}

void ProgramAction::setDisplayName(const QString& displayName)
{
	_displayName=displayName;
}

QString ProgramAction::description() const
{
	return _description;
}

void ProgramAction::setDescription(const QString& desc)
{
	_description=desc;
}

bool ProgramAction::operator==(const ProgramAction& b) const
{
	return name()==b.name() && displayName()==b.displayName() && description()==b.description();
}

bool ProgramAction::operator!=(const ProgramAction& b) const
{
	return !operator ==(b);
}

