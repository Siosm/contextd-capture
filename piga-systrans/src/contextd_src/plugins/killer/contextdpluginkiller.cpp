#include "contextdpluginkiller.h"
#include "../../../context-common.h"

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

#include <signal.h>
#include <QtCore/QRegExp>
#include <QtCore/QDir>

#include <syslog.h>

//Events
#include "../../eventdispatcher.h"
#include "../contextdpluginnotifyevent.h"
#include "../contextdplugintransitiondemandevent.h"
#include "../contextdpluginreloadevent.h"

//Static variables
QReadWriteLock ContextdPluginKiller::rwlock;

//XML Parsing
#define KILLER_ROOT_TAG "PIGA_Killer"
#define KILLER_OPTION_TAG "option"
#define KILLER_OPTION_NAME_TAG "name"
#define KILLER_OPTION_VALUE_TAG "value"
#define KILLER_RULE_TAG "rule"
#define KILLER_APP_TAG "app_name"
#define KILLER_CONTEXT_TAG "context"
#define KILLER_DOMAIN_TAG "domain"
#define KILLER_SIGNUM_TAG "sig_num"
#define KILLER_MSG_TAG "msg_kill"
KillingRule rule=KillingRule();
QString paramName;

bool ContextdPluginKiller::on_tag_open(const std::string& tag_name, XMLSP::StringMap& attributes)
{
	if(tag_name==KILLER_ROOT_TAG) {}
	else if(tag_name==KILLER_RULE_TAG /*&& rule==KillingRule()*/)
	{
		QString app=QString::fromUtf8(attributes[KILLER_APP_TAG].c_str());
		QString msg=QString::fromUtf8(attributes[KILLER_MSG_TAG].c_str());
		QString str_sig_num=QString::fromUtf8(attributes[KILLER_SIGNUM_TAG].c_str());
		QString domain=QString::fromUtf8(attributes[KILLER_DOMAIN_TAG].c_str());
		if(domain==QString())
			domain=QString::fromUtf8(attributes[KILLER_CONTEXT_TAG].c_str());

		rule=KillingRule(app, domain, str_sig_num.toInt(), msg);
	}
	else if(paramName==KILLER_OPTION_TAG)
	{
		QString name=attributes[KILLER_OPTION_NAME_TAG].c_str();
		QString value=attributes[KILLER_OPTION_NAME_TAG].c_str();

		if(name=="noKillingSpree")
			noKillingSpree = value=="true"?true:false;
	}
	else if(paramName==QString() && rule!=KillingRule())
		paramName=QString::fromUtf8(tag_name.c_str());
	else
		return false;

	return true;
}

bool ContextdPluginKiller::on_cdata(const std::string& cdata)
{
	if(rule!=KillingRule() && paramName!=QString())
		rule.addParameter(paramName, QString::fromUtf8(cdata.c_str()));

	return true;
}

bool ContextdPluginKiller::on_tag_close(const std::string& tag_name)
{
	if(tag_name==paramName.toStdString())
		paramName=QString();
	else if(tag_name==KILLER_RULE_TAG)
	{
		rules.push_back(rule);
		rule=KillingRule();
	}
	else
		return false;

	return true;
}

void ContextdPluginKiller::on_error(int errnr, int line, int col, const std::string& message)
{
	qWarning("Killer.xml: Error %i at the line %i and column %i: %s", errnr, line, col, message.c_str());
}

bool ContextdPluginKiller::readConf()
{
	bool res=false;

	const char* path="/etc/context.d/killer.xml";
	std::ifstream is(path);
	if(is.is_open())
	{
		std::ostringstream iss;
		iss << is.rdbuf();

		// Clear the rules
		rules.clear();

		rwlock.lockForWrite();
		bool result=parse(iss.str());
		rwlock.unlock();

		if(!result)
			qWarning("Error while loading the killer's rules : parse(iss.str()) returned false!");
		else
			res=true;
	}
	else
		qWarning("Error while loading the killer's rules : Cannot read the file '%s'!", path);

	return res;
}

// Killing
void ContextdPluginKiller::killClient(const ContextClient* client, int signal, QString msg)
{
	//Kill the software
	kill(client->appPid(), signal); //SIGTERM

	QString message=QObject::tr("%1 has been interrupted because it went into an illegal state : %2").arg(client->program().name(), msg);
	ContextdPluginNotifyEvent event(ContextdPluginNotifyEvent::NOTIFY_CRITICAL, message);
	EventDispatcher::instance().sendEvent(&event);
}

QList<pid_t> ContextdPluginKiller::getPIDList()
{
	QList<pid_t> pidList;

	QDir dir("/proc");
	QStringList entryList=dir.entryList();
	for(int i=0; i<entryList.size(); i++)
	{
		bool ok=false;
		pid_t pid=entryList[i].toInt(&ok);

		if(ok)
			pidList.push_back(pid);
	}

	return pidList;
}

QString ContextdPluginKiller::getFullPathFromPID(pid_t pid)
{
	QString proc_path="/proc/%1/exe";
	proc_path=proc_path.arg(pid);

	return QFile::readLink(proc_path);;
}

uid_t ContextdPluginKiller::getEuidFromPID(pid_t pid)
{
	QString path=QString("/proc/%1/status").arg(pid);

	QFile file(path);

	if(file.open(QIODevice::ReadOnly))
	{
		// first=dummy, second=euid, third=suid and fourth=fuid
		QRegExp reg("Uid:\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+).*");

		QString line;
		do
		{
			line=file.readLine();
			if(reg.exactMatch(line))
			{
				file.close();
				return reg.capturedTexts()[2].toInt();
			}
		}while(line!=QString());

		// We didn't find it. Let's issue a warning
		qWarning("Killer: Didn't find the Uid line for the pid %i!\n", pid);
	}
	else
		qWarning("Killer: Cannot read file '%s'. Return euid=-1.", qPrintable(path));

	return -1;
}

ContextdPluginKiller::ContextdPluginKiller() : AbstractContextdPlugin("Contextd_Killer"), noKillingSpree(false)
{
	readConf();
}

ContextdPluginKiller::~ContextdPluginKiller()
{

}

void ContextdPluginKiller::eventHandler(const ContextdPluginEvent* event)
{
	//Transition Demand
	if(ContextdPluginTransitionDemandEvent().type()==event->type())
	{
		const ContextdPluginTransitionDemandEvent* td_event;
		td_event=static_cast<const ContextdPluginTransitionDemandEvent*>(event);

		if(td_event->demandResult()==CONTEXT_ACCEPT)
		{
			QString app=td_event->clientConcerned()->program().name();
			QString context=td_event->clientConcerned()->currentDomain().name();

			rwlock.lockForRead();

			// Try to kill programs that are in an illegal state as stated in the killer.xml configuration file
			for(int i=0;i<rules.size();i++)
			{
				if(rules[i].isMatched(app, context, td_event->clientConcerned()->currentState()))
					killClient(td_event->clientConcerned(), rules[i].signal(), rules[i].messageKill());
			}

			// If this transition lead to a domain change, let's kill every program that is not in the nokill list
			if(td_event->transitionDemanded().transitFrom()!=td_event->transitionDemanded().transitTo())
			{
				QList<pid_t> pids=getPIDList();
				for(int i=0; i<pids.size(); i++)
				{
					// Only kill user applications
					uid_t euid=getEuidFromPID(pids[i]);
					if((int)euid>=1000)
					{
						QString full_path=getFullPathFromPID(pids[i]);
						if(Configuration::instance().isProgramKillable(full_path))
						{
							EventDispatcher::instance().sendNotification(QString("The program(pid=%0) '%1' with euid=%2 has been killed").arg(pids[i]).arg(full_path).arg(euid));

							// Only kill when we are in the killing spree mode
							if(!noKillingSpree)
								kill(pids[i], SIGKILL);
						}
						else
							EventDispatcher::instance().sendDebug(QString("The program(pid=%0) '%1' with euid=%2 is not killable").arg(pids[i]).arg(full_path).arg(euid));
					}
				}
			}

			rwlock.unlock();
		}

	}
	else if(ContextdPluginReloadEvent().type()==event->type())
	{
		readConf();
	}
}
