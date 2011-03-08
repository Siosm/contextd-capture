#include "contextd_communicator.h"

#include "dbuscallback.h"
#include "contextd_interface.h"

#include <QDBusError>
#include <QCoreApplication>

#include <sys/time.h>
#include <signal.h>

struct thread_data
{
	contextChangedCallback ccc;
	void* user_data;
} data;

QString currentContext;

Contextd_Communicator::Contextd_Communicator() :
		iface(SERVICE_NAME, LISTENER_PATH, SERVICE_NAME, QDBusConnection::systemBus(), this),
		cc_callback(NULL)
{
	//Create the notify dbus service
	if (!QDBusConnection::systemBus().isConnected())
	{
		printf("Cannot connect to the D-Bus system bus.\n"
			 "To start it, run:\n"
			 "\teval `dbus-launch --auto-syntax`\n");
	}

	if(!iface.isValid())
	{
		setError(tr("LibContext : The system bus \"%1\" is not valid:\n\nb	Error: %2\n")
			    .arg(SERVICE_NAME).arg(iface.lastError().message()));
	}
}

context_result Contextd_Communicator::QString2context_result(QString result)
{
	if(result==CONTEXT_REFUSE)
		return CONTEXT_REFUSED;
	else if(result==CONTEXT_ACCEPT || result==DBUS_SUCCESS)
		return CONTEXT_ACCEPTED;
	else
		return CONTEXT_ERROR;
}

QString Contextd_Communicator::toXMLEntities(QString str)
{
	return str.replace("&","&amp;").replace("<", "$lt;").replace(">", "&gt;").replace("\"", "&quot;");
}

bool Contextd_Communicator::isConnected()
{
	return QDBusConnection::systemBus().isConnected() && iface.isValid();
}

void Contextd_Communicator::setError(QString error)
{
	lastError=error;
	fprintf(stderr, "%s", qPrintable("libcontext error: " + error));
}

bool Contextd_Communicator::isRegistered()
{
	if(!isConnected())
		return false;

	QDBusReply<QString> reg = iface.call("is_registered");
	if (!reg.isValid())
	{
		QString error=tr("LibContext : Call to 'is_registered' failed: %1\n").arg(reg.error().message());
		setError(qPrintable(error));

		return false;
	}
	else
		return reg.value()==DBUS_SUCCESS;
}

bool Contextd_Communicator::registerApplication(QString app_name)
{
	this->app_name=app_name;

	if(!isConnected())
		return false;

	QDBusReply<QString> reg = iface.call("register_application", app_name, (uint)getpid());
	if (!reg.isValid())
	{
		QString error=QObject::tr("LibContext : Call failed: %1\n").arg(reg.error().message());
		setError(error);

		return false;
	}
	else
		return true;
}

context_result Contextd_Communicator::contextChanged(QString str)
{
	if(!isConnected())
		return CONTEXT_ERROR;

	QDBusReply<QString> reply = iface.call("context_changed", str);
	if (!reply.isValid())
	{
		QString error=QObject::tr("LibContext : Call failed: %1\n").arg(reply.error().message());
		setError(error);
	}

	return QString2context_result(reply.value());
}

unsigned int Contextd_Communicator::contextChangedAsync(contextChangedCallbackSuccess success_cb,
							contextChangedCallbackError error_cb,
							QString str)
{
	if(!isConnected())
		return 0;

	QList<QVariant> args;
	args.push_back(str);

	DBusCallBack* callback=new DBusCallBack(success_cb, error_cb);

	bool ret = iface.callWithCallback("context_changed", args, callback,
							    SLOT( on_success(QString)),
							    SLOT(on_error(QDBusError)));

	if(!ret)
	{
		QString error=QObject::tr("LibContext : Async call failed: %1\n").arg(iface.lastError().message());
		setError(error);
	}

	return ret;
}

bool Contextd_Communicator::requiredContext(QString str, QString& res)
{
	if(!isConnected())
		return CONTEXT_ERROR;

	QDBusReply<QString> reply = iface.call("required_context", str);
	if (!reply.isValid())
	{
		QString error=QObject::tr("LibContext : Call failed: %1\n").arg(reply.error().message());
		setError(error);

		return false;
	}

	res=reply.value();

	if(res!=CONTEXT_REFUSE && res!=DBUS_ERROR_NOT_REGISTERED && res!=DBUS_ERROR)
		return true;
	else
		return false;
}

void Contextd_Communicator::sigHandler(int num)
{
	if(num==SIGALRM)
	{
		char buf[151];
		if(context_current_global_context(buf, sizeof(buf)-1)==CONTEXT_TRUE)
		{
			QString newContext=buf;
			if(newContext!=currentContext)
			{
				printf("SigHandler: context_current_global_context='%s' !\n", buf);
				data.ccc(qPrintable(currentContext), qPrintable(newContext), data.user_data);

				currentContext=newContext;
			}
		}
	}
}

bool Contextd_Communicator::setContextChangedCallback(contextChangedCallback /*ccc*/, void* /*user_data*/)
{
	return true;
}

QString Contextd_Communicator::currentLocalContext()
{
	if(!isConnected())
		return "<error>";

	QDBusReply<QString> reply = iface.call("current_local_context");
	if (!reply.isValid())
	{
		QString error=QObject::tr("LibContext : Call failed: %1\n").arg(reply.error().message());
		setError(error);
		return "<error>";
	}
	else
		return reply.value();
}

QString Contextd_Communicator::currentGlobalContext()
{
	if(!isConnected())
		return "<error>";

	QDBusReply<QString> reply = iface.call("current_global_context");
	if (!reply.isValid())
	{
		QString error=QObject::tr("LibContext : Call failed: %1\n").arg(reply.error().message());
		setError(error);
		return "<error>";
	}
	else
		return reply.value();
}

QString Contextd_Communicator::getLastError()
{
	return lastError;
}
