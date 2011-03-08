#include "contextclient.h"
#include "context-common.h"
#include "promptuser.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

//Events
#include "eventdispatcher.h"
#include "plugins/contextdpluginnotifyevent.h"
#include "plugins/contextdplugintransitiondemandevent.h"

//Plugins
#include "plugins/contextholder/contextholder.h"

QReadWriteLock ContextClient::readwritelock;


TransitionRules& ContextClient::transRules()
{
	static TransitionRules transRules;

	return transRules;
}


ContextClient::ContextClient()
{}

ContextClient::ContextClient(const ContextClient& cc) : \
		QObject()
{
	operator=(cc);
}

ContextClient::ContextClient(QString dbusId, Program program, pid_t pid) :
		id(dbusId), program(program), pid(pid), fdfifo(-1)
{

}

ContextClient::~ContextClient()
{
	if(fdfifo!=-1)
		close(fdfifo);

	if(path_to_context_fifo!="")
		remove(qPrintable(path_to_context_fifo));
}

Context ContextClient::setGlobalState(Context globalState)
{
	return ContextHolder::setCurrentGlobalState(globalState);
}

Context ContextClient::setLocalState(Context localState)
{
	QWriteLocker lock(&readwritelock);

	//Copy the current local state to return it after
	Context ret=c_local_state;

	//Update the context
	c_local_state=localState;

	//Send a signal to everyone
	emit localContextChanged(ret, localState);

	return ret;
}

bool ContextClient::updateCurrentState(QString xmlState)
{
	//Parse the new state
	bool parseOK=xmlcontexttomap.parse(xmlState.toStdString());

	//If the context is valid, copy the previous state to _previousState
	if(parseOK)
	{
		_previousState = _currentState;
		_currentState=xmlcontexttomap.context();
	}

	return parseOK;
}

QString ContextClient::contextFromXML(QString xmlContext)
{
	//Avoid other process to switch their context before the end of this transition
	QReadLocker lock(&readwritelock);

	//Call the rules to know what to do
	bool parseOK=updateCurrentState(xmlContext);
	if(parseOK)
	{
		//OK, this only works BECAUSE we just use global states at the moment !
		const Transition transition_found=transRules().updatedContext(appName(), currentGlobalState().name(), currentState());

		if(transition_found.isValid())
		{
			//get the current state
			Context cstate=transition_found.isALocalTransition()?currentLocalState():currentGlobalState();
			Context transitTo=transition_found.transitTo();

			QString scope=transition_found.isALocalTransition()?SCOPE_LOCAL:SCOPE_GLOBAL;

			//If the rule prompts the user
			if(!transition_found.promptUser() || PromptUser::promptUserForAutorisation(scope, cstate.name(), transition_found.transitTo().name(), appName()))
			{
				//Unlock the readLock so as we can change it
				lock.unlock();

				//Change context
				Context previousContext;
				if(transition_found.isALocalTransition())
					previousContext=setLocalState(transitTo);
				else
					previousContext=setGlobalState(transitTo);

				//Hook
				onAcceptTransition(transition_found, previousContext);

				return CONTEXT_ACCEPT;
			}
			else if(transition_found.promptUser())	//The user refused the transition
			{
				onRefuseTransition(transition_found, cstate);
				return CONTEXT_REFUSE;
			}
		}
		else
		{
			onNoTransitionHasBeenFound();
			return CONTEXT_REFUSE;
		}
	}
	else
		onError("ContextClient::contextFromXML failed, XML(" + xmlContext + ")");

	return DBUS_ERROR;
}

QString ContextClient::required_context(QString xmlContext)
{
	//Call the rules to know what to do
	bool parseOK=updateCurrentState(xmlContext);
	if(parseOK)
	{
		//OK, this only works BECAUSE we just use global states at the moment !
		const Transition transition_found=transRules().updatedContext(appName(), currentGlobalState().name(), currentState());

		if(transition_found.isValid())
			return transition_found.transitTo().name();
		else
			return CONTEXT_REFUSE;
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

QString ContextClient::appName() const
{
	return program.appName();
}

pid_t ContextClient::appPid() const
{
	return pid;
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

Context ContextClient::currentGlobalState()
{
	return ContextHolder::currentGlobalState();
}

Context ContextClient::currentLocalState() const
{
	//Avoid other processes to change c_local_state while I return it
	QReadLocker lock(&readwritelock);

	return c_local_state;
}

void ContextClient::onError(QString error)
{
	ContextdPluginNotifyEvent test_msg(ContextdPluginNotifyEvent::NOTIFY_ERROR, error);
	EventDispatcher::instance().sendEvent(&test_msg);
}

void ContextClient::onAcceptTransition(Transition t, Context previousContext)
{
	ContextdPluginTransitionDemandEvent td_event(this, previousContext, CONTEXT_ACCEPT, t);
	EventDispatcher::instance().sendEvent(&td_event);
}
void ContextClient::onRefuseTransition(Transition t, Context previousContext)
{
	ContextdPluginTransitionDemandEvent td_event(this, previousContext, CONTEXT_REFUSE, t);
	EventDispatcher::instance().sendEvent(&td_event);
}
void ContextClient::onNoTransitionHasBeenFound()
{
	ContextdPluginTransitionDemandEvent td_event(this, Context(), CONTEXT_REFUSE);
	EventDispatcher::instance().sendEvent(&td_event);
}

/*void ContextClient::onGlobalContextChanged(QString previousGlobalContext, QString globalContext)
{
	if(path_to_context_fifo!="")
	{
		QString msg=previousGlobalContext+"-->"+globalContext+"\n";

		fdfifo=open(qPrintable(path_to_context_fifo), O_WRONLY|O_NONBLOCK);
		if(fdfifo!=-1)
		{
			if(write(fdfifo, qPrintable(msg), msg.size())<=0)
				eventDispatcher.notifyError("Contextd: Cannot write to the FIFO '"+path_to_context_fifo+"' !");
		}
		else
			eventDispatcher.notifyError("Contextd: Cannot open the FIFO '"+path_to_context_fifo+"' for writing");
	}
}*/

ContextClient& ContextClient::operator=(const ContextClient& cc)
{
	id=cc.id;
	program=cc.program;
	pid=cc.pid;
	path_to_context_fifo=cc.path_to_context_fifo;
	fdfifo=-1;

	return *this;
}
