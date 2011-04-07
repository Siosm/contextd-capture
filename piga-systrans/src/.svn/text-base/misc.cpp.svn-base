#include "misc.h"
#include <stdio.h>
#include "syslog.h"

void printd(const char* msg)
{
	qDebug("%s", msg);
	syslog( LOG_NOTICE, "%s", msg );
}

void printd(const std::string msg)
{
	printd(msg.c_str());
}

void printd(const QString msg)
{
	printd(msg.toAscii().data());
}

