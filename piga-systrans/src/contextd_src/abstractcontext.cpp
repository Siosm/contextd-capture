#include "abstractcontext.h"


AbstractContext::AbstractContext()
{
	clients = QMap<pid_t, ContextClient>();
}


QString AbstractContext::getFullPathFromPID(pid_t pid)
{
	QString proc_path="/proc/%1/exe";
	proc_path=proc_path.arg(pid);

	return QFile::readLink(proc_path);
}
