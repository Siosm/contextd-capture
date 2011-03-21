#include <QtNetwork/QHostAddress>
#include <QStringList>
#include <QDateTime>
#include <syslog.h>
#include <sys/stat.h>

#include "context-common.h"
#include "pigahandler.h"

#ifdef HAS_SELINUX
#include <selinux/selinux.h>
#endif

//Static members
int PigaHandler::traceId = 0;

//Private functions
PigaHandler::PigaHandler() : enabled(true)
{
#ifndef HAS_SELINUX
	disable();
	qDebug("PigaHandler is disabled because you built Contextd without SELinux support.");
	return;
#endif

#ifdef DISABLE_PIGAHANDLER
	disable();
	qDebug("PigaHandler is disabled because you decided not to built Contextd with it.");
#endif

	connectSocket();
}

PigaHandler::~PigaHandler()
{
	disconnectSocket();
}

void PigaHandler::connectSocket(bool value)
{
	if(value)
	{
		mkdir("/tmp/contextd/", S_IRWXU);
		socket.connectToServer("/tmp/contextd/piga-socket");
	}
	else
		socket.disconnectFromServer();
}

void PigaHandler::disconnectSocket()
{
	connectSocket(false);
}

QString PigaHandler::writeTransitionTrace(const ContextClient *c, Transition t, QString decision) const
{
	// Trace information
	QString s = QString("id=%1 timestamp=%2")
		.arg(traceId, 0, 10)
		.arg(QDateTime::currentDateTime().toTime_t(), 0, 10);

	// Contextd decision
	if (decision == CONTEXT_ACCEPT)
		s+=" allowed";
	else if (decision == CONTEXT_REFUSE)
		s+=" refused";
	else if (decision == CONTEXT_PIGA_REFUSE)
		s+=" piga-refused";
	else if (decision == CONTEXT_PIGA_KILL)
		s+=" piga-killed";
	else
		s+=" unknown-decision";

	// Transition
	s += QString(" { transition from \"%1\" to \"%2\" }")
		.arg(t.transitFrom().name().replace("\"", "\\\""))
		.arg(t.transitTo().name().replace("\"", "\\\""));

	// Program name and ID
	s += QString(" for program=\"%1\" pid=%2 ppid=%3")
		.arg(c->program().name().replace("\"", "\\\""))
		.arg(c->appPid(), 0, 10)
		.arg(c->appPpid(), 0, 10);

#ifdef HAS_SELINUX
	// SELinux context if present
	security_context_t seccon;
	if (getpidcon(c->appPid(), &seccon) == 0)
	{
		s+= QString (" selinux_context=\"%1\"")
		    .arg(QString(seccon).replace("\"", "\\\""));
		freecon (seccon);
	}
#endif

	QMap<QString,QString>::const_iterator it;
	for (it=c->currentState().constBegin(); it!=c->currentState().constEnd(); ++it)
	{
		s += QString (" %1=\"%2\"")
			.arg(it.key())
			.arg(QString(it.value()).replace("\"", "\\\""));
	}

	return s;
}

//Public functions
PigaHandler& PigaHandler::instance()
{
	static PigaHandler s;
	return s;
}

void PigaHandler::enable(bool value)
{
	if (value && !enabled)
		connectSocket();
	else if(!value && enabled)
		disconnectSocket();

	enabled=value;
}

void PigaHandler::disable()
{
	enable(false);
}

char PigaHandler::sendTrace(const ContextClient *c, Transition t, QString decision)
{
	//If disabled, return disabled
	if (!enabled)
		return PigaHandler::Disabled;

	//Increment the trace identifier - beware, if several traces are sent
	// for a single transition, traceId will be wrong.
	++traceId;

	//Get a string from the current state
	QString trace = writeTransitionTrace(c, t, decision);

	//Log onto syslog and on a socket
	syslog(LOG_ALERT, "%s", qPrintable(trace));
	if (enabled && socket.isValid())
		socket.write(qPrintable(trace));

	//Wait for the answer
	return PigaHandler::instance().waitForNextDecision(traceId);
}

void PigaHandler::logTrace(const ContextClient *c, Transition t, QString decision) const
{
	//If disabled, return
	if (!enabled)
		return;

	//Get a string from the current state
	QString trace = writeTransitionTrace(c, t, decision);

	//Log to syslog
	syslog(LOG_NOTICE, "%s", qPrintable(trace));
}

char PigaHandler::waitForNextDecision(int traceId)
{
	if (!enabled)
		return PigaHandler::Disabled;
	else
	{
		if (socket.isValid() && socket.waitForReadyRead()) //will timeout after 30 seconds
		{
			char buf[1024];
			qint64 lineLength = socket.readLine(buf, sizeof(buf));
			if (lineLength != -1) {
				QStringList list = QString(buf).split(' ');
				if ((list.size() == 2) && (list.at(0).toInt() == traceId))
				{
					if (list.at(1) == "allowed")
						return PigaHandler::Allowed;
					else if (list.at(1) == "killed")
						return PigaHandler::Killed;
					else // "refused"
						return PigaHandler::Refused;
				}
			}
		}
	}

	return PigaHandler::Error;
}
