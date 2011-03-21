#include "dbuscallback.h"
#include "context-common.h"

#include <QDBusMessage>
#include <QDBusPendingReply>

unsigned int DBusCallBack::nb=0;
QMutex DBusCallBack::mutex;

DBusCallBack::DBusCallBack(contextChangedCallbackSuccess success, contextChangedCallbackError error) : QObject()
{
	mutex.lock();
	nb++;
	id=nb;
	mutex.unlock();

	success_cb=success;
	error_cb=error;
}

context_result DBusCallBack::QString2context_result(QString result)
{
	if(result==CONTEXT_REFUSE)
		return CONTEXT_REFUSED;
	else if(result==CONTEXT_ACCEPT || result==DBUS_SUCCESS)
		return CONTEXT_ACCEPTED;
	else
		return CONTEXT_ERROR;
}

unsigned int DBusCallBack::ID()
{
	return id;
}

void DBusCallBack::on_error(QDBusError error)
{
	error_cb(id, qPrintable(error.name()), qPrintable(error.message()));
	delete this;
}

void DBusCallBack::on_success(QString ret)
{
	if(ret==DBUS_SUCCESS||ret==CONTEXT_ACCEPT)
		success_cb(id, QString2context_result(ret));
	else if(ret==DBUS_ERROR_NOT_REGISTERED)
	{
		const char* error_msg=qPrintable(tr("You have to register your application before being able to send information about the context !"));
		error_cb(id, qPrintable(tr("Not Registered Error")), error_msg );
	}
	else
	{
		const char* error_msg=qPrintable(tr("The context has been rejected for un unknown reason. Call your administrator to check syslog-ng output."));
		error_cb(id, qPrintable(tr("Type error")), error_msg);
	}

	delete this;
}
