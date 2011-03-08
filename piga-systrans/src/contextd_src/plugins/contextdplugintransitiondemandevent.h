#ifndef CONTEXTDPLUGINTRANSITIONDEMANDEVENT_H
#define CONTEXTDPLUGINTRANSITIONDEMANDEVENT_H

#include "contextdpluginevent.h"
#include "../contextclient.h"

class ContextdPluginTransitionDemandEvent : public ContextdPluginEvent
{
private:
	const ContextClient* client;
	Context prevContext;
	QString result;
	Transition transition;
	
public:
	ContextdPluginTransitionDemandEvent();
	ContextdPluginTransitionDemandEvent(const ContextClient* client, Context previousContext, QString result, Transition transition=Transition());

	EventType type() const;
	Transition transitionDemanded() const;
	Context previousContext() const;


	const ContextClient* clientConcerned() const;
	QString demandResult() const;
};

#endif // CONTEXTDPLUGINGLOBALCONTEXTCHANGEDEVENT_H
