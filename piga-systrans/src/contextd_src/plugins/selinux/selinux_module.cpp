#include "selinux_module.h"

SELinux_Module::SELinux_Module()
{

}

SELinux_Module::SELinux_Module(QString fullPath, QString appName, QString neededContext) :
		full_path(fullPath), app_name(appName), needed_context(neededContext)
{

}

QString SELinux_Module::modulePath()
{
	return full_path;
}

QString SELinux_Module::appName()
{
	return app_name;
}

QString SELinux_Module::neededContext()
{
	return needed_context;
}

bool SELinux_Module::operator==(const SELinux_Module& b) const
{
	return full_path==b.full_path && app_name==b.app_name && needed_context==b.needed_context;
}

bool SELinux_Module::operator!=(const SELinux_Module& b) const
{
	return !operator==(b);
}
