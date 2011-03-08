#include "transitionrules.h"
#include "../xml_common.h"
#include "eventdispatcher.h"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include "contextclient.h"
#include "plugins/contextholder/contextholder.h"

//static variables
QMap<QString, QVector<Transition> > TransitionRules::rules;
QMap<QString, Context > TransitionRules::contexts;
QMap<QString, Program > TransitionRules::programs;
QMutex TransitionRules::init;
QReadWriteLock TransitionRules::readwritelock;

//global XML parsing variables
Transition trans;
bool rule_Oppenned=false;
std::string tag_data;

bool TransitionRules::on_tag_open(const std::string& tag_name, XMLSP::StringMap& attributes)
{
	if(tag_name==RULE_TAG)
	{
		rule_Oppenned=true;

		bool local=false;
		if(attributes[RULE_SCOPE_TAG]==SCOPE_LOCAL)
			local=true;
		else if(attributes[RULE_SCOPE_TAG]==SCOPE_GLOBAL)
			local=false;
		else
		{
			rule_Oppenned=false;
			EventDispatcher::instance().sendError((attributes[RULE_APP_NAME_TAG] + " : '" + attributes[RULE_SCOPE_TAG] + "' is an invalid scope name. Valid scope names are '" SCOPE_LOCAL "' and '" SCOPE_GLOBAL "'.\n").c_str());
		}

		bool prompt=false;
		if(attributes[RULE_PROMPT_TAG]==RULE_TRUE)
			prompt=true;
		else if(attributes[RULE_PROMPT_TAG]=="" || attributes[RULE_PROMPT_TAG]==RULE_FALSE)
			prompt=false;
		else
		{
			rule_Oppenned=false;
			EventDispatcher::instance().sendError((attributes[RULE_APP_NAME_TAG] + " : '" + attributes[RULE_PROMPT_TAG] + "' is an invalid prompt value. Valid prompt values are '" RULE_TRUE "' and '" RULE_FALSE "'.\n").c_str());
		}

		bool notify=true;
		if(attributes[RULE_NOTIFY_TAG]=="" || attributes[RULE_NOTIFY_TAG]==RULE_TRUE)
			notify=true;
		else if(attributes[RULE_NOTIFY_TAG]==RULE_FALSE)
			notify=false;
		else
		{
			rule_Oppenned=false;
			EventDispatcher::instance().sendError((attributes[RULE_APP_NAME_TAG] + " : '" + attributes[RULE_NOTIFY_TAG] + "' is an invalid notify value. Valid notify values are '" RULE_TRUE "' and '" RULE_FALSE "'.\n").c_str());
		}

		//Create the transition
		QString app_name=QString::fromUtf8(attributes[RULE_APP_NAME_TAG].c_str());
		QString transitFrom=QString::fromUtf8(attributes[RULE_TRANSIT_FROM_TAG].c_str());
		QString str_transitTo=QString::fromUtf8(attributes[RULE_TRANSIT_TO_TAG].c_str());

		//If the target context exists, we can continue
		Context transitTo=contexts[str_transitTo];
		if(transitTo!=Context())
		{
			Program program=programs[app_name];
			if(program!=Program())
				trans=Transition(program, transitFrom, transitTo, local, prompt, notify);
			else
			{
				rule_Oppenned=false;
				EventDispatcher::instance().sendError(QObject::tr("TransitionRules : The program %1 doesn't exist !").arg(app_name));
			}
		}
		else
		{
			rule_Oppenned=false;
			EventDispatcher::instance().sendError(QObject::tr("TransitionRules : The context %1 doesn't exist !").arg(str_transitTo));
		}
	}
	else if(tag_name==CONTEXT_TAG)
	{
		QString name=QString::fromUtf8(attributes[NAME_TAG].c_str());
		QString display_name=QString::fromUtf8(attributes[DISPLAY_NAME_TAG].c_str());
		QString icon_path=QString::fromUtf8(attributes[ICON_TAG].c_str());

		//Add the context to the context list
		if(name!=QString())
		{
			contexts[name]=Context(name, display_name, icon_path);
			if(attributes[DEFAULT_TAG]==RULE_TRUE)
				ContextHolder::instance().setDefaultGlobalContext(contexts[name]);
		}

	}
	else if(tag_name==PROGRAM_TAG)
	{
		QString name=QString::fromUtf8(attributes[NAME_TAG].c_str());
		QString display_name=QString::fromUtf8(attributes[DISPLAY_NAME_TAG].c_str());
		QString icon_path=QString::fromUtf8(attributes[ICON_TAG].c_str());
		QString full_path=QString::fromUtf8(attributes[FULLPATH_TAG].c_str());

		//Add the context to the context list
		if(name!=QString())
			programs[name]=Program(name, display_name, icon_path, full_path);
	}

	return true;
}

bool TransitionRules::on_cdata(const std::string& cdata)
{
	tag_data=cdata;
	return true;
}

bool TransitionRules::on_tag_close(const std::string& tag_name)
{
	if(tag_name==RULE_TAG && rule_Oppenned)
	{
		rules[trans.appName()].push_back(trans);
		rule_Oppenned=false;
	}
	else if(rule_Oppenned)
	{
		//printf("tag close: %s -> %s\n", tag_name.c_str(), tag_data.c_str());
		trans.addRule(QString::fromUtf8(tag_name.c_str()), QString::fromUtf8(tag_data.c_str()));
		tag_data="";
	}

	return true;
}

void TransitionRules::on_error(int errnr, int line, int col, const std::string& message)
{
	printf("transitions.xml: Error %i at the line %i and column %i: %s", errnr, line, col, message.c_str());
}

bool TransitionRules::readConf()
{
	bool res=false;

	readwritelock.lockForWrite();

	const char* path="/etc/context.d/transitions.xml";
	std::ifstream is(path);
	if(is.is_open())
	{
		std::ostringstream iss;
		iss << is.rdbuf();

		rules.clear();

		if(!parse(iss.str()))
		{
			std::cerr << "Error while loading the rules : parse(iss.str()) returned false !" << std::endl;
		}
		else
			res=true;
	}
	else
	    std::cerr << "Error while loading the rules : The file '" << path << "' cannot be openned !" << std::endl;

	readwritelock.unlock();

	return res;
}

TransitionRules& TransitionRules::instance()
{
	static TransitionRules tr;

	return tr;
}

TransitionRules::TransitionRules() : /*QObject(), */XMLSP::Parser()
{
	init.lock();

	//Avoid to reload the rules everytime we load a new client
	if(contexts.size()==0)
	{
		readConf();
	}

	init.unlock();
}

bool TransitionRules::updateRules()
{
	TransitionRules t;
	return t.readConf();
}

Context TransitionRules::getContextByName(QString name)
{
	QReadLocker locker(&readwritelock);

	return contexts[name];
}

const Transition TransitionRules::updatedContext(QString appName, QString current_state, QMap<QString, QString> values)
{
	int rule_index=-1;

	readwritelock.lockForWrite();

	QVector<Transition> trans=rules[appName];
	for(int i=0;i<trans.size() && rule_index < 0;i++)
	{
		if(trans[i].matches(current_state, values))
			rule_index=i;
	}

	readwritelock.unlock();

	//Return the matched rule
	if(rule_index < 0)
		return Transition();
	else
		return trans[rule_index];
}

bool TransitionRules::programAllowed(QString name, QString full_path)
{
	if(!programs.contains(name))
		return false;
	else
		return programs[name].fullPath()==full_path;
}

Program TransitionRules::getProgramByName(QString name)
{
	return programs[name];
}
