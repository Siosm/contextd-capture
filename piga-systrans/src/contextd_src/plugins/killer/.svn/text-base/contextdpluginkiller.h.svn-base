#ifndef CONTEXTDPLUGINKILLER_H
#define CONTEXTDPLUGINKILLER_H

#include "../abstractcontextdplugin.h"
#include "../../contextclient.h"
#include "../../../xmlsp.h"

#include <QList>
#include <QReadWriteLock>
#include "killingrule.h"

class ContextdPluginKiller : public AbstractContextdPlugin, public XMLSP::Parser
{
private:
	static QReadWriteLock rwlock;
	QList<KillingRule> rules;

	//XML Parsing
	bool on_tag_open(const std::string& tag_name, XMLSP::StringMap& attributes);
	bool on_cdata(const std::string& cdata);
	bool on_tag_close(const std::string& tag_name);
	void on_error(int errnr, int line, int col, const std::string& message);

	bool readConf();

	// Killing
	bool noKillingSpree;
	void killClient(const ContextClient* client, int signal, QString msg);
	QList<pid_t> getPIDList();
	QString getFullPathFromPID(pid_t pid);
	uid_t getEuidFromPID(pid_t pid);

public:
    ContextdPluginKiller();
    ~ContextdPluginKiller();

    void eventHandler(const ContextdPluginEvent* event);
};

#endif // CONTEXTDPLUGINKILLER_H
