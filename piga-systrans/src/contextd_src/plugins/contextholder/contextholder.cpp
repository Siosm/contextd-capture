#include "contextholder.h"
#include "../contextdpluginrestartevent.h"
#include "../../eventdispatcher.h"

//Static variables
Context ContextHolder::c_global_state;
Context ContextHolder::default_global_state;
QReadWriteLock ContextHolder::readwritelock;

//Methods
ContextHolder& ContextHolder::instance()
{
	static ContextHolder ch;

	return ch;
}

ContextHolder::ContextHolder(QObject *parent) : QObject(parent), AbstractContextdPlugin("ContextHolder")
{
}

void ContextHolder::eventHandler(const ContextdPluginEvent* event)
{
	if(event->type()==ContextdPluginRestartEvent().type())
	{
		//const ContextdPluginRestartEvent* restart=static_cast<const ContextdPluginRestartEvent*>(event);

		//reset the current global state
		setCurrentGlobalState(default_global_state);

		EventDispatcher::instance().sendNotification("The global context has been reset");
	}
}

Context ContextHolder::currentGlobalState()
{
	//Avoid other processes to change c_global_state while I return it
	QReadLocker lock(&readwritelock);

	return c_global_state;
}

Context ContextHolder::setCurrentGlobalState(Context context)
{
	QWriteLocker lock(&readwritelock);

	//Copy the current global state to return it after
	Context ret=c_global_state;

	//Update the context
	c_global_state=context;

	//Send a signal to everyone
	emit instance().globalContextChanged(ret, c_global_state);

	return ret;
}

Context ContextHolder::setDefaultGlobalContext(Context default_context)
{
	QWriteLocker lock(&readwritelock);

	//Copy the current default global state to return it after
	Context ret=default_global_state;

	//Update the default context
	default_global_state=default_context;

	return ret;
}

Context ContextHolder::defaultGlobalContext()
{
	QReadLocker lock(&readwritelock);

	return default_global_state;
}

Context ContextHolder::resetToGlobalDefaultContext()
{
	return setCurrentGlobalState(defaultGlobalContext());
}
