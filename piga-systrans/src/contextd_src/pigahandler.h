#ifndef PIGATRACESENDER_H
#define PIGATRACESENDER_H

#include <QtCore/QObject>
#include <QtNetwork/QLocalSocket>
#include <QtCore/QString>

#include "contextclient.h"
#include "transition.h"

class PigaHandler : public QObject
{
	Q_OBJECT

private:
	bool enabled;
	QLocalSocket socket;

	///Latest traceID fired
	static int traceId;

	PigaHandler();
	~PigaHandler();

	void connectSocket(bool value=true);
	void disconnectSocket();

	QString writeTransitionTrace(const ContextClient *c, Transition t, QString decision) const;

	char waitForNextDecision(int traceId);

public:
	static PigaHandler& instance();

	void enable(bool value=true);
	void disable();

	char sendTrace(const ContextClient *c, Transition t, QString decision);
	void logTrace(const ContextClient *c, Transition t, QString decision) const;

	enum PigaDecision
	{
		Error		=1<<0,
		Disabled	=1<<1,
		Allowed		=1<<2,
		Refused		=1<<3,
		Killed		=1<<4
	};
};



#endif // PIGATRACESENDER_H
