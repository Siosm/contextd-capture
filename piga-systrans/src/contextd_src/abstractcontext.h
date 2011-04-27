#ifndef ABSTRACTCONTEXT_H
#define ABSTRACTCONTEXT_H

#include <QtCore/QObject>
#include <QtCore/QMap>

#include "contextclient.h"

class AbstractContext : public QObject
{
Q_OBJECT
protected:
	static QMap<pid_t, ContextClient> clients;

public:
    AbstractContext();

	QString getFullPathFromPID(pid_t pid);

};

#endif // ABSTRACTCONTEXT_H
