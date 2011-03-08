#ifndef CONTEXTD_COMMUNICATOR_H
#define CONTEXTD_COMMUNICATOR_H

#include <QObject>

#include "../context-common.h"
#include "contextd_interface.h"
#include "libcontext.h"

//#include "../contextd_interface.h"
#include<QDBusInterface>

class Contextd_Communicator : public QObject
{
Q_OBJECT
private:
	QDBusInterface iface;

	contextChangedCallback cc_callback;

	QString app_name;
	QString lastError;

	context_result QString2context_result(QString result);
	bool isConnected();

	void setError(QString error="none");

	//Listen for context changes
	/*struct thread_data
	{
		contextChangedCallback ccc;
		const char* path_fifo;
		void* user_data;
	};
	static QString readLine(int fdfifo);
	static bool parseContextLine(const QString line, QString& previousContext, QString& newContext);
	static void* readContextChanges(void* user_data);*/
	static void sigHandler(int num);

public:
	Contextd_Communicator();

	bool isRegistered();
	bool registerApplication(QString app_name);

	context_result contextChanged(QString str);
	unsigned int contextChangedAsync(contextChangedCallbackSuccess success_cb,
								contextChangedCallbackError error_cb,
								QString str);
	bool requiredContext(QString str, QString& res);

	bool setContextChangedCallback(contextChangedCallback ccc, void* user_data);

	QString currentLocalContext();
	QString currentGlobalContext();

	QString toXMLEntities(QString str);

	QString getLastError();
};

#endif // CONTEXTD_COMMUNICATOR_H
