#include "contextclient.h"
#include "contextnotify.h"
#include "context-common.h"

#include "pigahandler.h"

#include <QFile>
#include <QStringList>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

//Events
#include "eventdispatcher.h"
#include "plugins/contextdpluginnotifyevent.h"
#include "plugins/contextdplugintransitiondemandevent.h"

//Plugins
#include "plugins/domainholder/domainholder.h"

QReadWriteLock ContextClient::readwritelock;

//Private
bool ContextClient::updateCurrentState(QString xmlState, bool& updateNeeded)
{
	//Parse the new state
	bool parseOK=xmlcontexttomap.parse(xmlState.toStdString());

	//If the context is valid, copy the previous state to _previousState
	if(parseOK)
	{
		if(_currentState==xmlcontexttomap.context())
			updateNeeded=false;
		else
		{
			updateNeeded=true;
			_previousState = _currentState;
			_currentState=xmlcontexttomap.context();
		}

	}

	return parseOK;
}



void ContextClient::onError(QString error)
{
	ContextdPluginNotifyEvent test_msg(ContextdPluginNotifyEvent::NOTIFY_ERROR, error);
	EventDispatcher::instance().sendEvent(&test_msg);
}

// Hooks
QString ContextClient::onAcceptHooks(Transition t)
{
	QString hookRes;

	// Ask PIGA
	short decision=PigaHandler::instance().sendTrace(this, t, CONTEXT_ACCEPT);

	// If it is OK or disabled, allow the transition
	if ((decision==PigaHandler::Allowed) || (decision==PigaHandler::Disabled))
		hookRes=CONTEXT_ACCEPT;
	else
	{
		// Otherwise, log/kill and refuse the transition
		hookRes=CONTEXT_REFUSE;
		if (decision==PigaHandler::Killed)
		{
			PigaHandler::instance().logTrace(this, t, CONTEXT_PIGA_KILL);
			kill(appPid(), SIGKILL);
		}
		else
			PigaHandler::instance().logTrace(this, t, CONTEXT_PIGA_REFUSE);
	}

	return hookRes;
}
void ContextClient::onRefuseHooks(Transition t)
{
	PigaHandler::instance().logTrace(this, t, CONTEXT_REFUSE);
}

// Inform plugins of the result
void ContextClient::onAcceptTransition(Transition t)
{
	//Change domain
	DomainHolder::instance().setCurrentDomain(t.transitTo());

	//Send the event to other plugins
	ContextdPluginTransitionDemandEvent td_event(this, t.transitFrom(), CONTEXT_ACCEPT, t);
	EventDispatcher::instance().sendEvent(&td_event);

	//Send an event to the user
	if(t.notifyUser())
		ContextNotify::notifyUserOfDomainChanged(t.appName(), t.transitTo().name());
}
void ContextClient::onRefuseTransition(Transition t)
{
	// Call the hooks
	onRefuseHooks(t);

	// Send the event to other plugins
	ContextdPluginTransitionDemandEvent td_event(this, t.transitFrom(), CONTEXT_REFUSE, t);
	EventDispatcher::instance().sendEvent(&td_event);
}
void ContextClient::onNoTransitionHasBeenFound()
{
	// Send the event to other plugins
	ContextdPluginTransitionDemandEvent td_event(this, currentDomain(), CONTEXT_REFUSE);
	EventDispatcher::instance().sendEvent(&td_event);
}

//Public
ContextClient::ContextClient()
{}

ContextClient::ContextClient(QString dbusId, Program program, pid_t pid) :
		id(dbusId), _program(program), pid(pid)
{

}

ContextClient::~ContextClient()
{

}

QString ContextClient::updateState(QString xmlContext)
{
	//Avoid other process to switch their context before the end of this transition
	QReadLocker lock(&readwritelock);

	//Call the rules to know what to do
	bool updateNeeded;
	bool parseOK=updateCurrentState(xmlContext, updateNeeded);
	if(parseOK)
	{
		const Transition transition_found=Configuration::instance().getFirstMatchingTransition(program(), currentDomain(), currentState());

		if(transition_found.isValid())
		{
			//get the current state
			Domain cstate=currentDomain();
			Domain transitTo=transition_found.transitTo();

			//If the rule prompts the user
			if(!transition_found.promptUser() || ContextNotify::promptUserForAutorisation(cstate.name(), transition_found.transitTo().name(), program().name()))
			{
				//Unlock the readLock so as we can change it
				lock.unlock();

				//Hook
				QString hookRes=onAcceptHooks(transition_found);
				if(hookRes==CONTEXT_ACCEPT)
					onAcceptTransition(transition_found);

				return hookRes;
			}
			else if(transition_found.promptUser())	//The user refused the transition
			{
				onRefuseTransition(transition_found);
				return CONTEXT_REFUSE;
			}
		}
		else
		{
			onNoTransitionHasBeenFound();
			return CONTEXT_REFUSE;
		}
	}
	else if(!updateNeeded)
	{
		QString msg="ContextClient::contextFromXML: The client is already in the context ("+ xmlContext + ")";
		ContextdPluginNotifyEvent test_msg(ContextdPluginNotifyEvent::NOTIFY_INFO, msg);
		EventDispatcher::instance().sendEvent(&test_msg);
	}
	else if(!parseOK)
		onError("ContextClient::contextFromXML failed, XML(" + xmlContext + ")");

	return DBUS_ERROR;
}

QString ContextClient::required_domain(QString xmlContext)
{
	//Call the rules to know what to do
	bool parseOK=xmlcontexttomap.parse(xmlContext.toStdString());
	if(parseOK)
	{
		const Transition transition_found=Configuration::instance().getFirstMatchingTransition(program(), currentDomain(), xmlcontexttomap.context());

		if(transition_found.isValid())
		{
			QString msg=QString("The application '%1' will accept this context (%2) in the domain %3").arg(program().name(), xmlContext, transition_found.transitTo().name());
			EventDispatcher::instance().sendDebug(msg);
			return transition_found.transitTo().name();
		}
		else
		{
			QString msg=QString("The application '%1' has no domain that would accept this context (%2)").arg(program().name(), xmlContext);
			EventDispatcher::instance().sendDebug(msg);
			return CONTEXT_REFUSE;
		}
	}
	else
		onError("ContextClient::contextFromXML failed, XML(" + xmlContext + ")");

	return DBUS_ERROR;
}

QMap<QString, QString> ContextClient::currentState() const
{
	return _currentState;
}

QMap<QString, QString> ContextClient::previousState() const
{
	return _previousState;
}

QString ContextClient::dbusId() const
{
	return id;
}

Program ContextClient::program() const
{
	return _program;
}

pid_t ContextClient::appPid() const
{
	return pid;
}

pid_t ContextClient::appPpid() const
{
	QFile file(QString("/proc/%1/stat")
				.arg(pid, 0, 10));
	if (!file.exists())
		return -1;

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return -1;

	char buf[1024];
	qint64 lineLength = file.readLine(buf, sizeof(buf));
	if (lineLength != -1)
	{
		QStringList list = QString(buf).split(' ');
		if (list.size() < 4)
			return -1;
		else
			return list.at(3).toInt();
	}
	else
		return -1;
}

Domain ContextClient::currentDomain() const
{
	return DomainHolder::currentDomain();
}

QString ContextClient::registerForContextChangesUpdates()
{
	/*QString path="/tmp/contextd_%1_%2";
	path=path.arg(appName()).arg(appPid());

	if(path_to_context_fifo!="")
		return path_to_context_fifo;
	else
	{
		printf("make fifo: %s\n", qPrintable(path));
		if(mkfifo(qPrintable(path), S_IWUSR|S_IROTH)!=0)
		{
			eventDispatcher.notifyError(QString("Contextd: Cannot create the FIFO '%1': %2").arg(path, strerror(errno)));
			return DBUS_ERROR;
		}

		path_to_context_fifo=path;

		return path_to_context_fifo;
	}*/

	return "";
}
