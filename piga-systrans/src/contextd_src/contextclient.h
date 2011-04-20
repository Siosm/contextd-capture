#ifndef CONTEXTCLIENT_H
#define CONTEXTCLIENT_H

#include "configuration.h"
#include "eventdispatcher.h"
#include "xmlcontext2map.h"
#include "program.h"
#include "plugins/abstractcontextdplugin.h"

#include <QtCore/QMutex>
#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QReadWriteLock>

class ContextClient
{
private:
	//Associate a parameter name to its value (eg "host" -> "www.google.fr")
	QMap<QString, QString> _currentState, _previousState;

	//XML to map
	XMLContext2Map xmlcontexttomap;

	//Context
	static QReadWriteLock readwritelock;

	//Information about the associated program
	QString id;
	Program _program;
	pid_t pid;

	//Private functions
	bool updateCurrentState(QString xmlState, bool& updateNeeded);

	//Private hooks
	void onError(QString error);
	QString onAcceptHooks(Transition t);
	void onRefuseHooks(Transition t);
	void onAcceptTransition(Transition t);
	void onRefuseTransition(Transition t);
	void onNoTransitionHasBeenFound();

public:
	ContextClient();
	ContextClient(QString dbusId, Program program, pid_t pid);
	~ContextClient();

	QString updateState(QString xmlContext);
	QString required_domain(QString xmlContext);
	QMap<QString, QString> currentState() const;
	QMap<QString, QString> previousState() const;

	QString dbusId() const;
	pid_t appPid() const;
	pid_t appPpid() const;
	Program program() const;

	Domain currentDomain() const;

	QString registerForContextChangesUpdates(); //WIP

	//static Domain currentDomain();

	//ContextClient& operator=(const ContextClient&);
	//Domain setDomain(Domain globalState);
};

#endif // CONTEXTCLIENT_H
