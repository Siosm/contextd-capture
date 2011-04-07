#ifndef CONTEXTNOTIFY_H
#define CONTEXTNOTIFY_H

#include <QString>

class ContextNotify
{
public:
	static bool promptUserForAutorisation(const QString& current_state, const QString& new_state, const QString& app_name);
	static bool notifyUserOfDomainChanged(const QString& app_name, const QString& new_domain);
	static bool notify(const QString& msg, int severity);
};

#endif // CONTEXTNOTIFY_H
