#include "contextdplugintransitiondemandevent.h"

ContextdPluginTransitionDemandEvent::ContextdPluginTransitionDemandEvent()
{

}

ContextdPluginTransitionDemandEvent::ContextdPluginTransitionDemandEvent(const ContextClient* client, Context previousContext, QString result,  Transition transition) :
		client(client), prevContext(previousContext), result(result), transition(transition)
{

}

EventType ContextdPluginTransitionDemandEvent::type() const
{
	return "TransitionDemand";
}

Transition ContextdPluginTransitionDemandEvent::transitionDemanded() const
{
	return transition;
}

Context ContextdPluginTransitionDemandEvent::previousContext() const
{
	return prevContext;
}

const ContextClient* ContextdPluginTransitionDemandEvent::clientConcerned() const
{
	return client;
}

QString ContextdPluginTransitionDemandEvent::demandResult() const
{
	return result;
}
