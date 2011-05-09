#ifndef CLIENTSCLEANER_H
#define CLIENTSCLEANER_H

#include <QtCore/QThread>
#include <QtCore/QMap>

#include "contextclient.h"


class ClientsCleaner: public QThread
{
Q_OBJECT
public:
	explicit ClientsCleaner(QMap<pid_t, ContextClient> * clients, QObject* parent = 0);
	void run();

private:
	QMap<pid_t, ContextClient> * _clients;
};

#endif // CLIENTSCLEANER_H
