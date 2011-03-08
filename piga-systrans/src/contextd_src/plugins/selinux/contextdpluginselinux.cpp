#include "contextdpluginselinux.h"
#include "../../../context-common.h"

//Events
#include "../../eventdispatcher.h"
#include "../contextdplugintransitiondemandevent.h"
#include "../contextdpluginreloadevent.h"

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

#include <QStringList>

QReadWriteLock ContextdPluginSELinux::rwlock;

#define SELINUX_ROOT_TAG "PIGA_SELinux_Plugin"
#define SELINUX_RULE_TAG "module"
#define SELINUX_APP_TAG "app_name"
#define SELINUX_CONTEXT_TAG "context"
#define SELINUX_FULL_PATH_TAG "full_path"
bool ContextdPluginSELinux::on_tag_open(const std::string& tag_name, XMLSP::StringMap& attributes)
{
	if(tag_name==SELINUX_ROOT_TAG) {}
	else if(tag_name==SELINUX_RULE_TAG)
	{
		QString app=QString::fromUtf8(attributes[SELINUX_APP_TAG].c_str());
		QString context=QString::fromUtf8(attributes[SELINUX_CONTEXT_TAG].c_str());
		QString full_path=QString::fromUtf8(attributes[SELINUX_FULL_PATH_TAG].c_str());

		if(app!="")
		{
			if(full_path[0]=='/' && full_path.right(3)==".pp")
			{
				QPair<QString, QString> key=qMakePair(app, context);

				if(!modules.contains(key))
				{
					modules[key]=SELinux_Module(full_path, app, context);
					EventDispatcher::instance().sendNotification(QObject::tr("%1: The module(%2,%3)='%4' has been added !").arg(name(), app, context, full_path));
				}
				else
					EventDispatcher::instance().sendError(QObject::tr("%1: The module(%2,%3)='%4' already exists !").arg(name(), app, context, full_path));
			}
			else
				EventDispatcher::instance().sendError(QObject::tr("%1: The module<%2,%3> is invalid because '%3' is not a valid module path").arg(name(), app, context, full_path));
		}
		else
			EventDispatcher::instance().sendError(QObject::tr("%1: %2 is not a valid program name in the module<%3,%4>").arg(name(), app, app, context));
	}
	else
		return false;

	return true;
}

void ContextdPluginSELinux::on_error(int errnr, int line, int col, const std::string& message)
{
	EventDispatcher::instance().sendError(QString("selinux.xml: Error %1 at the line %2 and column %3: %s").arg(errnr, line, col).arg(message.c_str()));
}

bool ContextdPluginSELinux::readConf()
{
	bool res=false;

	const char* path="/etc/context.d/selinux.xml";
	std::ifstream is(path);
	if(is.is_open())
	{
		std::ostringstream iss;
		iss << is.rdbuf();

		modules.clear();

		rwlock.lockForWrite();
		bool result=parse(iss.str());
		rwlock.unlock();

		if(!result)
			EventDispatcher::instance().sendError("Error while loading the selinux's rules : parse(iss.str()) returned false !");
		else
			res=true;

	}
	else
		EventDispatcher::instance().sendError(QString("Error while loading the selinux's rules : Cannot read the file '%1' !").arg(path));

	return res;
}

SELinux_Module ContextdPluginSELinux::getModule(QString app_name, QString context)
{
	QReadLocker lock(&rwlock);

	QPair<QString, QString> key=qMakePair(app_name, context);

	if(modules.contains(key))
		return modules[key];
	else
		return SELinux_Module();
}

bool ContextdPluginSELinux::exec(QString cmd)
{
	int pid=fork();
	if(pid==0)
	{
		const QStringList cmd_l=cmd.split(' ', QString::SkipEmptyParts);

		QString appName=cmd_l[0];
		char* app=new char[appName.size()+1];
		strcpy(app, qPrintable(appName));

		//Create the args buffer
		char** params=new char*[cmd_l.size()];
		for(int i=0;i<cmd_l.size();i++)
		{
			params[i]=new char[cmd_l.at(i).size()];
			strcpy(params[i],qPrintable(cmd_l.at(i)));
		}

		//Execute the command
		execv(app, params);

		return false;
	}
	else if(pid<0)
		return false;

	return true;
}

ContextdPluginSELinux::ContextdPluginSELinux() : AbstractContextdPlugin("SELinux")
{
	readConf();
}

void ContextdPluginSELinux::loadModule(QString app, QString path)
{
	QString cmd="";

	if(currentModule[app].modulePath()!=QString())
	{
		cmd+="semodule -r \""+ currentModule[app].modulePath() + "\";";

		EventDispatcher::instance().sendNotification(QString("SELinux: Unload the module '%1' !").arg(currentModule[app].modulePath()));
	}

	cmd+="semodule -i \""+path+"\"";
	EventDispatcher::instance().sendNotification(QString("SELinux: Load the module '%1' !").arg(path));

	//Exec the commands
	//exec(cmd);
	system(qPrintable(cmd));
}

void ContextdPluginSELinux::eventHandler(const ContextdPluginEvent* event)
{
	//Transition Demand
	if(ContextdPluginTransitionDemandEvent().type()==event->type())
	{
		const ContextdPluginTransitionDemandEvent* td_event;
		td_event=static_cast<const ContextdPluginTransitionDemandEvent*>(event);

		if(td_event->transitionDemanded().isValid() && td_event->demandResult()==CONTEXT_ACCEPT)
		{
			Context globalContext=td_event->clientConcerned()->currentGlobalState();
			QString scope=td_event->transitionDemanded().isALocalTransition()?SCOPE_LOCAL:SCOPE_GLOBAL;
			QString app=td_event->clientConcerned()->appName();

			SELinux_Module module=getModule(app, globalContext.name());
			if(module!=SELinux_Module())
			{
				//If we actually change the context
				if(currentModule[app].modulePath()!=module.modulePath())
				{
					loadModule(app, module.modulePath());

					//Update the current module
					currentModule[app]=module;
				}
			}
			else
				EventDispatcher::instance().sendNotification(QString("SELinux: No module has been found for (%1, %2)").arg(app, globalContext.name()));
		}
	}
	else if(ContextdPluginReloadEvent().type()==event->type())
	{
		readConf();
	}
}
