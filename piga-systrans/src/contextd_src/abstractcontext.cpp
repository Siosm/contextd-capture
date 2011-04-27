#include "abstractcontext.h"
#include "plugins/contextdpluginrestartevent.h"
#include "plugins/domainholder/domainholder.h"


QString AbstractContext::getFullPathFromPID(pid_t pid)
{
	QString proc_path="/proc/%1/exe";
	proc_path=proc_path.arg(pid);

	return QFile::readLink(proc_path);
}


void AbstractContext::onEvent(ContextdPluginEvent* event)
{
	if(event->type()==ContextdPluginRestartEvent().type())
	{
		DomainHolder::instance().resetToDefaultDomain();
	}
}
