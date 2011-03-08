#include "context.h"

Context::Context()
{
}

Context::Context(QString name, QString display_name, QString icon_path) :
		_name(name), _display_name(display_name), _icon_path(icon_path)
{

}

QString Context::name()
{
	return _name;
}

QString Context::displayName()
{
	return _display_name;
}

QString Context::iconPath()
{
	return _icon_path;
}

bool Context::operator==(const Context& b) const
{
	return _name==b._name &&  _display_name==b._display_name && _icon_path==b._icon_path;
}

bool Context::operator!=(const Context& b) const
{
	return !operator==(b);
}
