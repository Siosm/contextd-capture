#ifndef CONTEXTLIST_H
#define CONTEXTLIST_H

#include "xml_common.h"
#include "xmlsp.h"

#include <QVector>
#include <QList>
#include <QMap>
#include <QString>

class ContextList : public XMLSP::Parser
{
public:
	struct Rule
	{
	    ///Name of the context to transit
	    QString transit_from;

	    ///Name of the context to transit
	    QString transit_to;
	};

	struct Context
	{
	    ///Name of the context
	    QString context_name;

	    ///Name to be displayed
	    QString display_name;

	    ///path to the icon representing this state
	    QString icon;

	    ///Can we transit to this context ?
	    bool reachable;
	};

	ContextList();

	const QVector<Rule> rulesList();
	const QVector<Context> contextsList();

	const Context getContextByName(QString contextName);
	const Context getContextByName(QString currentContext, QString contextName);

	Context currentSystemContext();

private:
	QVector<Rule> rules;
	QMap<QString, Context> contexts;

	bool contextReachable(QString currrent_context, QString context);

	//XML Parsing
	bool on_tag_open(const std::string& tag_name, XMLSP::StringMap& attributes);
        void on_error(int errnr, int line, int col, const std::string& message);
};

#endif // CONTEXTLIST_H
