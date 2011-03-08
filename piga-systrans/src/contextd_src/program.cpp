#include "program.h"

Program::Program()
{
}

Program::Program(QString app_name, QString display_name, QString icon, QString full_path) :
		app_name(app_name), display_name(display_name), icon_path(icon), full_path(full_path)
{
}

QString Program::appName() const
{
	return app_name;
}

QString Program::displayName() const
{
	return display_name;
}

QString Program::icon() const
{
	return icon_path;
}

QString Program::fullPath() const
{
	return full_path;
}

bool Program::operator==(const Program& b) const
{
	return app_name==b.app_name && display_name==b.display_name && icon_path==b.icon_path && full_path==b.full_path;
}

bool Program::operator!=(const Program& b) const
{
	return !operator==(b);
}
