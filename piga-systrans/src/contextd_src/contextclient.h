#ifndef CONTEXTCLIENT_H
#define CONTEXTCLIENT_H

#include "transitionrules.h"
#include "eventdispatcher.h"
#include "xmlcontext2map.h"
#include "program.h"
#include "plugins/abstractcontextdplugin.h"

#include <QObject>
#include <QMutex>
#include <QString>
#include <QMap>
#include <QReadWriteLock>

class ContextClient : public QObject
{
Q_OBJECT
private:
	//Associate a parameter name to its value (eg "host" -> "www.google.fr")
	QMap<QString, QString> _currentState, _previousState;

	static TransitionRules& transRules();
	XMLContext2Map xmlcontexttomap;

	//Context
	Context c_local_state;
	static QReadWriteLock readwritelock;

	QString id;
	Program program;
	pid_t pid;

	//context changes updator
	QString path_to_context_fifo;
	int fdfifo;

	//Private functions
	bool updateCurrentState(QString xmlState);
	Context setLocalState(Context localState);

public:
	ContextClient();
	ContextClient(const ContextClient& cc);
	ContextClient(QString dbusId, Program program, pid_t pid);
	~ContextClient();

	QString contextFromXML(QString xmlContext);
	QString required_context(QString xmlContext);
	QMap<QString, QString> currentState() const;
	QMap<QString, QString> previousState() const;

	QString dbusId() const;
	QString appName() const;
	pid_t appPid() const;
	QString fullPath() const;
	Context currentLocalState() const;

	QString registerForContextChangesUpdates(); //WIP

	static Context currentGlobalState();

	ContextClient& operator=(const ContextClient&);
	Context setGlobalState(Context globalState);

private slots:
	void onError(QString error);
	void onAcceptTransition(Transition t, Context previousContext);
	void onRefuseTransition(Transition t, Context previousContext);
	void onNoTransitionHasBeenFound();

signals:
	void localContextChanged(Context previousLocalContext, Context localContext);
};

#endif // CONTEXTCLIENT_H
