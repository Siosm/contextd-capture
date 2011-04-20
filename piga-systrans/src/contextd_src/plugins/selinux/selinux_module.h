#ifndef SELINUX_MODULE_H
#define SELINUX_MODULE_H

#include <QtCore/QString>

class SELinux_Module
{
private:
	QString full_path, app_name, needed_context;

public:
	SELinux_Module();
	SELinux_Module(QString fullPath, QString appName, QString neededContext);

	QString modulePath();
	QString appName();
	QString neededContext();

	bool operator==(const SELinux_Module& b) const;
	bool operator!=(const SELinux_Module& b) const;
};

#endif // SELINUX_MODULE_H
