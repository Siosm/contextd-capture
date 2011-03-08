#ifndef PROGRAM_H
#define PROGRAM_H

#include <QString>

class Program
{
private:
	QString app_name, display_name, icon_path, full_path;

public:
	Program();
	Program(QString app_name, QString display_name, QString icon, QString full_path);

	QString  appName() const;
	QString  displayName() const;
	QString  icon() const;
	QString  fullPath() const;

	bool operator==(const Program& b) const;
	bool operator!=(const Program& b) const;
};

#endif // PROGRAM_H
