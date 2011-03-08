#ifndef CONTEXT_H
#define CONTEXT_H

#include <QString>

class Context
{
private:
	QString _name, _display_name, _icon_path;

public:
	Context();
	Context(QString name, QString display_name, QString icon_path);

	QString name();
	QString displayName();
	QString iconPath();

	bool operator==(const Context& b) const;
	bool operator!=(const Context& b) const;
};

#endif // CONTEXT_H
