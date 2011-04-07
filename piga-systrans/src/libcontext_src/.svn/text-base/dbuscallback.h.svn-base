#ifndef DBUSCALLBACKE_H
#define DBUSCALLBACKE_H

#include "libcontext.h"
#include <QObject>
#include <QMutex>
#include <QDBusError>


class DBusCallBack : public QObject
{
Q_OBJECT
	static QMutex mutex;
	static unsigned int nb;

	unsigned int id;

	contextChangedCallbackSuccess success_cb;
	contextChangedCallbackError error_cb;

	context_result QString2context_result(QString result);

public:
	DBusCallBack(contextChangedCallbackSuccess success_cb, contextChangedCallbackError error_cb);

	unsigned int ID();

public slots:
	void on_error(QDBusError error);
	void on_success(QString ret);
};

#endif // DBUSCALLBACKE_H
