#include "contextlist.h"
#include <QRegExp>
#include <fstream>
#include <sstream>
#include <iostream>

#include "../libcontext_src/libcontext.h"

bool ContextList::on_tag_open(const std::string& tag_name, XMLSP::StringMap& attributes)
{
	if(tag_name==RULE_TAG)
	{
		if(attributes[RULE_APP_NAME_TAG]==APP_CONTEXT_NOTIFY)
		{
			Rule item;

			item.transit_from=QString::fromUtf8(attributes[RULE_TRANSIT_FROM_TAG].c_str());
			item.transit_to=QString::fromUtf8(attributes[RULE_TRANSIT_TO_TAG].c_str());

			if(item.transit_to!="")
				rules.push_back(item);
		}
	}
	else if(tag_name==CONTEXT_TAG)
	{
		QString name=QString::fromUtf8(attributes[NAME_TAG].c_str());
		QString display_name=QString::fromUtf8(attributes[DISPLAY_NAME_TAG].c_str());
		QString icon_path=QString::fromUtf8(attributes[ICON_TAG].c_str());

		//If the name is valid
		if(name!="")
		{
			Context context;
			context.context_name=name;
			context.display_name=display_name;
			context.icon=icon_path;

			//Add the context to the context list
			contexts[name]=context;
		}
	}

	return true;
}

void ContextList::on_error(int errnr, int line, int col, const std::string& message)
{
    qDebug("Parsing error: err=%i, line=%i, col=%i: %s\n", errnr, line, col, message.c_str());
}

ContextList::ContextList()
{
	std::ifstream is("/etc/context.d/transitions.xml");
	if(is.is_open())
	{
		std::ostringstream iss;
		iss << is.rdbuf();

		rules.clear();

		if(!parse(iss.str()))
		{
			std::cerr << "Error while loading the rules : parse(iss.str()) returned false !!!!!!!" << std::endl;
		}
	}
	else
	    std::cerr << "Error while loading the rules : parse(iss.str()) returned false !!!!!!!" << std::endl;
}

const QVector<ContextList::Rule> ContextList::rulesList()
{
	return rules;
}

const QVector<ContextList::Context> ContextList::contextsList()
{
	QVector<ContextList::Context> v_contexts=contexts.values().toVector();
	QString currentContext=currentSystemContext().context_name;

	//Update the reachable field of contexts
	for(int i=0;i<v_contexts.size();i++)
		v_contexts[i].reachable=contextReachable(currentContext, v_contexts[i].context_name);

	return v_contexts;
}

const ContextList::Context ContextList::getContextByName(QString contextName)
{
	QString currentContext=currentSystemContext().context_name;

	return getContextByName(currentContext, contextName);
}

const ContextList::Context ContextList::getContextByName(QString currentContext, QString contextName)
{
	if(contexts.contains(contextName))
		contexts[contextName].reachable=contextReachable(currentContext, contextName);

	return contexts[contextName];
}

ContextList::Context ContextList::currentSystemContext()
{
	QString currentContext;
	char* cContextBuffer=new char[101];

	if(context_current_global_context(cContextBuffer, 101)==CONTEXT_TRUE)
		currentContext=cContextBuffer;
	else
		currentContext="";

	delete cContextBuffer;

	contexts[currentContext].reachable=contextReachable(currentContext, currentContext);

	return contexts[currentContext];
}

bool ContextList::contextReachable(QString currentContext, QString context)
{
	//For all rules
	for(int i=0;i<rules.size();i++)
	{
		if(rules[i].transit_to==context)
		{
			QRegExp regexp(rules[i].transit_from);

			if(regexp.isValid())
				if(regexp.exactMatch(currentContext))
					return true;
		}
	}

	return false;
}
