#include "configuration.h"
#include "xml_common.h"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include "rule.h"
#include "loop.h"

#include <QDir>
#include <QFileInfo>


bool Configuration::parseMRules(QDomElement &parent, AbstractRule *rule)
{
	QDomElement root=parent.firstChildElement(RULE_MATCHING_TAG);
	if(root.isNull())
	{
		qWarning("Parser: Rule '%s' contains no MRule tag.", qPrintable(rule->displayName()));
		return false;
	}

	//Go through each child of the matching element
	QDomElement element=root.firstChildElement(RULE_MRULE_TAG);
	while(!element.isNull())
	{
		if(element.hasChildNodes())
		{
			//Create a new MRule
			MRule mrule(rule->program(), element.attribute(ACTION_TAG), element.attribute(DISPLAY_NAME_TAG));

			//For each child tag, add a condition to the MRule
			QDomElement child=element.firstChildElement();
			while(!child.isNull())
			{
				mrule.addCondition(child.tagName(), child.text());
				child=child.nextSiblingElement();
			}

			rule->addMRule(mrule);
		}
		else
		{
			qWarning("Parser: Empty MRule tag inside Rule '%s'.", qPrintable(rule->displayName()));
		}

		element=element.nextSiblingElement(RULE_MRULE_TAG);
	}

	return true;
}

bool Configuration::parseTRulesForLoop(QDomElement &parent, Loop &loop)
{
	QDomElement root=parent.firstChildElement(LOOP_DOMAINS_TAG);
	if(root.isNull())
	{
		qWarning("Parser: Loop '%s' contains no TRule tag.", qPrintable(loop.displayName()));
		return false;
	}

	//Go through each child of the matching element
	QDomElement element=root.firstChildElement(LOOP_DOMAIN_TAG);
	while(!element.isNull())
	{
		QString name=element.attribute(LOOP_DOMAIN_NAME_TAG);

		//If there is a name attribute
		if(name!=QString())
		{
			//Create a new TRule
			TRule trule(name);

			QString notify=element.attribute(RULE_NOTIFY_TAG);
			if (notify!=QString())
				trule.setNotify(notify==RULE_TRUE? true:false);

			QString prompt=element.attribute(RULE_PROMPT_TAG);
			if (prompt!=QString())
				trule.setPrompt(prompt==RULE_TRUE? true:false);

			loop.addTRule(trule);
		}
		else
			qWarning("Parser: TRule without source domain inside Loop '%s'.", qPrintable(loop.displayName()));

		element=element.nextSiblingElement(LOOP_DOMAIN_TAG);
	}

	return true;
}

bool Configuration::parseTRulesForRule(QDomElement &parent, Rule &rule)
{
	QDomElement root=parent.firstChildElement(RULE_TRANSITIONS_TAG);
	if(root.isNull())
	{
		qWarning("Parser: Rule '%s' contains no TRule tag.", qPrintable(rule.displayName()));
		return false;
	}

	//Check that there are allowed domains in the transitions tag
	QDomElement allowed=root.firstChildElement(ALLOW_TAG);
	if(allowed.isNull())
	{
		qWarning("Parser: Rule '%s' has no allowed transitions.", qPrintable(rule.displayName()));
		return false;
	}

	//Go through each child of the matching element
	QDomElement element=allowed.firstChildElement(RULE_TRANSIT_TAG);
	while(!element.isNull())
	{
		QString transitFrom=element.attribute(RULE_TRANSIT_FROM_TAG);

		//If there is a from_domain attribute
		if(transitFrom!=QString())
		{
			//Create a new TRule
			TRule trule(transitFrom);

			QString notify=element.attribute(RULE_NOTIFY_TAG);
			if (notify!=QString())
				trule.setNotify(notify==RULE_TRUE? true:false);

			QString prompt=element.attribute(RULE_PROMPT_TAG);
			if (prompt!=QString())
				trule.setPrompt(prompt==RULE_TRUE? true:false);

			rule.addTRule(trule);
		}
		else
			qWarning("Parser: TRule without source domain inside Rule '%s'.", qPrintable(rule.displayName()));

		element=element.nextSiblingElement(RULE_TRANSIT_TAG);
	}

	return true;
}

void Configuration::parseLoop(QDomElement &element)
{
	//Read the attributes needed for creating the loop
	QString appName, displayName;
	appName=element.attribute(RULE_APP_NAME_TAG);
	displayName=element.attribute(RULE_DISPLAY_NAME_TAG);

	//Check if the program exists
	Program program;
	if(!_programs.contains(appName))
	{
		//Program does not exist, create a fake program for this loop
		program=Program(appName, "Unknown ("+appName+")", ":/icons/unknown-program", QString(), QString());
	}
	else
		program=_programs[appName];

	//Create the loop
	Loop tmpLoop(program, displayName);

	//Read the other attributes
	QString notify, prompt;
	notify=element.attribute(RULE_NOTIFY_TAG);
	prompt=element.attribute(RULE_PROMPT_TAG);

	//Check if notify and prompt are coherent, otherwise set default values
	if(notify==QString())
	{
		qWarning("Parser: Loop '%s' has an invalid notify attribute, using default.", qPrintable(displayName));
		notify=RULE_DEFAULT_NOTIFY;
	}
	if(prompt==QString())
	{
		qWarning("Parser: Loop '%s' has an invalid prompt attribute, using default.", qPrintable(displayName));
		prompt=RULE_DEFAULT_PROMPT;
	}

	//Set notify and prompt in the loop
	tmpLoop.setNotify(notify==RULE_TRUE ? true:false);
	tmpLoop.setPrompt(prompt==RULE_TRUE ? true:false);

	//Read the MRules
	if(!parseMRules(element, &tmpLoop))
	{
		qWarning("Parser: Loop '%s' contains invalid matching rules.", qPrintable(displayName));
		return;
	}

	//Read the TRules
	if(!parseTRulesForLoop(element, tmpLoop))
	{
		qWarning("Parser: Loop '%s' contains invalid transition rules.", qPrintable(displayName));
		return;
	}

	//Add the loop to our map of loops
	_loops[appName].push_back(tmpLoop);
}

void Configuration::parseRule(QDomElement &element)
{
	//Read the attributes needed for creating the rule
	QString appName, strTransitTo, displayName;
	appName=element.attribute(RULE_APP_NAME_TAG);
	strTransitTo=element.attribute(RULE_TRANSIT_TO_TAG);
	displayName=element.attribute(RULE_DISPLAY_NAME_TAG);

	//Check if the target domain exists
	Domain transitTo=_domains[strTransitTo];
	if(transitTo==Domain())
	{
		qWarning("Parser: Domain '%s' does not exist.", qPrintable(strTransitTo));
		return;
	}

	//Check if the program exists
	Program program;
	if(!_programs.contains(appName))
	{
		//Program does not exist, create a fake program for this rule
		program=Program(appName, "Unknown ("+appName+")", ":/icons/unknown-program", QString(), QString());
	}
	else
		program=_programs[appName];

	//Create the rule
	Rule tmpRule(program, transitTo, displayName);

	//Read the other attributes
	QString notify, prompt;
	notify=element.attribute(RULE_NOTIFY_TAG);
	prompt=element.attribute(RULE_PROMPT_TAG);

	//Check if notify and prompt are coherent, otherwise set default values
	if(notify==QString())
	{
		qWarning("Parser: Rule '%s' has an invalid notify attribute, using default.", qPrintable(displayName));
		notify=RULE_DEFAULT_NOTIFY;
	}
	if(prompt==QString())
	{
		qWarning("Parser: Rule '%s' has an invalid prompt attribute, using default.", qPrintable(displayName));
		prompt=RULE_DEFAULT_PROMPT;
	}

	//Set notify and prompt in the rule
	tmpRule.setNotify(notify==RULE_TRUE ? true:false);
	tmpRule.setPrompt(prompt==RULE_TRUE ? true:false);

	//Read the MRules
	if(!parseMRules(element, &tmpRule))
	{
		qWarning("Parser: Rule '%s' contains invalid matching rules.", qPrintable(displayName));
		return;
	}

	//Read the TRules
	if(!parseTRulesForRule(element, tmpRule))
	{
		qWarning("Parser: Rule '%s' contains invalid transition rules.", qPrintable(displayName));
		return;
	}

	//Add the rule to our map of rules
	_rules[appName].push_back(tmpRule);
}

void Configuration::parseDomain(QDomElement &element)
{
	QString name, displayName, iconPath, description;
	int confidentiality, reliability;

	//Fetch the attributes
	name=element.attribute(NAME_TAG);
	displayName=element.attribute(DISPLAY_NAME_TAG);
	iconPath=element.attribute(ICON_TAG);

	//Fetch attributes that may be absent in old versions of the transitions file
	confidentiality=element.attribute(CONFIDENTIALITY_TAG, QString("%1").arg(Domain::ConfidentialityDefault)).toInt();
	reliability=element.attribute(RELIABILITY_TAG, QString("%1").arg(Domain::ReliabilityDefault)).toInt();

	//Fetch the description
	const QDomElement &child=element.firstChildElement(DESCRIPTION_TAG);
	if(!child.isNull())
		description=child.text();

	//Add the domain to the domain list
	if(name!=QString() && !_domains.contains(name))
	{
		_domains[name]=Domain(name, displayName, iconPath, description, confidentiality, reliability);
		if(element.attribute(DEFAULT_TAG, RULE_FALSE)==RULE_TRUE)
		{
			_default_domain=_domains[name];
		}
		else if(name==QString())
		{
			qWarning("Parser: Invalid domain name ('%s').", qPrintable(name));
		}
	}
	else if(_domains.contains(name))
	{
		qWarning("Parser: The domain '%s' already exists.", qPrintable(name));
	}
}

void Configuration::parseNoKillProgram(QDomElement &element)
{
	QString path=element.attribute(FULLPATH_TAG);
	if(path==QString())
	{
		qWarning("Parser: Trying to add an unkillable program with an empty path.");
		return;
	}

	if(QFileInfo(path).isAbsolute())
		_no_kill_programs.push_back(path);
	else
		qWarning("Parser: Trying to add an unkillable program using a relative path (%s).", qPrintable(path));
}

void Configuration::parseTransitionsFile()
{
	//No transitions file set, begin empty
	if (_transitions_path==QString())
		return;

	QFile file(_transitions_path);

	if(!file.open(QIODevice::ReadOnly))
	{
		qWarning("Parser: Can not open file '%s'.", qPrintable(_transitions_path));
		return;
	}
	if(!_transitions_doc.setContent(&file))
	{
		file.close();
		qWarning("Parser: Not syntaxically valid, can not parse transitions file.");
		return;
	}

	QDomElement root=_transitions_doc.firstChildElement(TRANSITIONS_XML_ROOT);
	if(root.isNull())
	{
		qWarning("Parser: Transitions file is empty.");
		return;
	}

	//Parse domains
	QDomElement element=root.firstChildElement(DOMAIN_TAG);
	while(!element.isNull())
	{
		parseDomain(element);
		element=element.nextSiblingElement(DOMAIN_TAG);
	}

	//Parse programs that should not be killed
	element=root.firstChildElement(NOKILL_ROOT_TAG);
	if(!element.isNull())
		element=element.firstChildElement(NOKILL_PROGRAM_TAG);

	while(!element.isNull())
	{
		parseNoKillProgram(element);
		element=element.nextSiblingElement(NOKILL_PROGRAM_TAG);
	}

	//Parse rules and loops in the order they appear now that domains are defined
	element=root.firstChildElement();
	while(!element.isNull())
	{
		if(element.tagName()==LOOP_TAG)
			parseLoop(element);
		else if(element.tagName()==RULE_TAG)
			parseRule(element);

		element=element.nextSiblingElement();
	}
}

// Program parsing
void Configuration::parsePrograms()
{
	QDir dir("/etc/context.d/program.d/");
	QFileInfoList list=dir.entryInfoList(QStringList("*.xml"));
	bool found=false;

	for(int i=0; i<list.size(); i++)
	{
		QFile file(list.at(i).filePath());

		Program p(file);
		if(p.isValid())
		{
			if(!_programs.contains(p.name()))
			{
				_programs[p.name()]=p;
				found=true;
			}
			else
				qWarning("Configuration: Program %s already exists.", qPrintable(p.name()));
		}
	}
}

bool Configuration::addProgram(Program p, bool quiet)
{
	if(!_programs.contains(p.name()))
	{
		//Add the program
		_programs[p.name()]=p;

		//Update the loops
		for(int i=0; i<_loops[p.name()].size(); ++i)
		{
			Loop *loop=const_cast<Loop *>(&_loops[p.name()].at(i));
			loop->setProgram(p);
		}

		//Update the rules
		for(int i=0; i<_rules[p.name()].size(); ++i)
		{
			Rule *rule=const_cast<Rule *>(&_rules[p.name()].at(i));
			rule->setProgram(p);
		}

		if (!quiet)
			emit programChanged(QString(), p.name());

		return true;
	}
	else
	{
		qWarning("Configuration: Program %s already exists.", qPrintable(p.name()));
		return false;
	}
}

void Configuration::addProgram(Program p)
{
	addProgram(p, false);
}

Program Configuration::updateProgram(QString old_name, const Program& prog)
{
	if(_programs.contains(old_name))
	{
		//Remove the program
		_programs.remove(old_name);
		_programs.insert(prog.name(), prog);

		//Update the loops
		for(int i=0; i<_loops[old_name].size(); ++i)
		{
			Loop *loop=const_cast<Loop *>(&_loops[old_name].at(i));
			loop->setProgram(prog, true);
		}

		//Update the rules
		for(int i=0; i<_rules[old_name].size(); ++i)
		{
			Rule *rule=const_cast<Rule *>(&_rules[old_name].at(i));
			rule->setProgram(prog, true);
		}

		emit programChanged(old_name, prog.name());
		return prog;
	}
	else
	{
		qWarning("Configuration: Program %s does not exist.", qPrintable(old_name));
		return Program();
	}
}

Program Configuration::deleteProgram(QString name)
{
	if(_programs.contains(name))
	{
		//Remove the program
		Program p=_programs[name];
		Program invalid=Program(name, "Unknown ("+name+")", ":/icons/unknown-program", QString(), QString());
		_programs.remove(name);

		//Update the loops
		for(int i=0; i<_loops[name].size(); ++i)
		{
			Loop *loop=const_cast<Loop *>(&_loops[p.name()].at(i));
			loop->setProgram(invalid, true);
		}

		//Update the rules
		for(int i=0; i<_rules[name].size(); ++i)
		{
			Rule *rule=const_cast<Rule *>(&_rules[p.name()].at(i));
			rule->setProgram(invalid, true);
		}

		emit programChanged(name, QString());
		return p;
	}
	else
	{
		qWarning("Configuration: Program %s does not exist.", qPrintable(name));
		return Program();
	}
}

QString Configuration::retrieveTransitionsDOM() const
{
	QDomDocument doc;

	//Create DOM tree root
	QDomElement root=doc.createElement(TRANSITIONS_XML_ROOT);
	doc.appendChild(root);

	//Insert domains
	QMap<QString, Domain>::const_iterator domsIt;
	for(domsIt=_domains.begin(); domsIt!=_domains.end(); ++domsIt)
	{
		const Domain &dom=domsIt.value();

		//Create the domain element and put it in the tree
		QDomElement domElem=doc.createElement(DOMAIN_TAG);
		root.appendChild(domElem);

		//Set it's attributes
		domElem.setAttribute(ICON_TAG, dom.iconPath());
		domElem.setAttribute(DISPLAY_NAME_TAG, dom.displayName());
		domElem.setAttribute(NAME_TAG, dom.name());
		domElem.setAttribute(CONFIDENTIALITY_TAG, dom.confidentiality());
		domElem.setAttribute(RELIABILITY_TAG, dom.reliability());
		if (_default_domain==dom)
			domElem.setAttribute(DEFAULT_TAG, RULE_TRUE);

		//Add the description
		QDomElement descElem=doc.createElement(DESCRIPTION_TAG);
		domElem.appendChild(descElem);
		QDomText descText=doc.createTextNode(dom.description());
		descElem.appendChild(descText);
	}

	//Create the root no kill element
	QDomElement noKillRoot=doc.createElement(NOKILL_ROOT_TAG);
	root.appendChild(noKillRoot);

	//Insert non killable programs
	QList<QString>::const_iterator nokIt;
	for(nokIt=_no_kill_programs.begin(); nokIt!=_no_kill_programs.end(); ++nokIt)
	{
		const QString &path=*nokIt;

		//Create the nokill element and put it in the tree
		QDomElement nokElem=doc.createElement(NOKILL_PROGRAM_TAG);
		noKillRoot.appendChild(nokElem);

		//Set it's attributes
		nokElem.setAttribute(FULLPATH_TAG, path);
	}

	//Get the list of all rules and loops
	QVector<AbstractRulePtr> abstracts=allAbstractRules(true);

	//Go through the list of AbstractRule and save each rule in the DOM tree
	for(int i=0; i<abstracts.size(); ++i)
	{
		AbstractRulePtr &abs=abstracts[i];
		//Create a Loop or Rule element and add it to the root element
		QDomElement elem;
		if(abs->ruleType()==Loop().ruleType())
			elem=doc.createElement(LOOP_TAG);
		else
			elem=doc.createElement(RULE_TAG);
		root.appendChild(elem);

		//Set the attributes
		elem.setAttribute(DISPLAY_NAME_TAG, abs->displayName());
		elem.setAttribute(RULE_PROMPT_TAG, abs->prompt()? RULE_TRUE:RULE_FALSE);
		elem.setAttribute(RULE_NOTIFY_TAG, abs->notify()? RULE_TRUE:RULE_FALSE);
		elem.setAttribute(RULE_APP_NAME_TAG, abs->appName());

		//Special transitTo attribute for Rule
		if(!abs.dynamicCast<Rule>().isNull())
			elem.setAttribute(RULE_TRANSIT_TO_TAG, abs.dynamicCast<Rule>()->transitTo().name());

		//Create a matching tag for appending all the mrules
		QDomElement matchElem=doc.createElement(RULE_MATCHING_TAG);
		elem.appendChild(matchElem);

		const QVector<MRule> &mrules=abs->mRules();
		for(int i=0; i<mrules.size(); ++i)
		{
			//For each MRule create a new DOM element
			const MRule &mrule=mrules[i];
			QDomElement mruleElem=doc.createElement(RULE_MRULE_TAG);
			matchElem.appendChild(mruleElem);

			mruleElem.setAttribute(ACTION_TAG, mrule.actionName());
			mruleElem.setAttribute(DISPLAY_NAME_TAG, mrule.displayName());

			//Add each condition of the MRule as a child
			const QMap<QString, QString> &conds=mrule.conditions();
			QMap<QString, QString>::const_iterator condsIt;
			for(condsIt=conds.begin(); condsIt!=conds.end(); ++condsIt)
			{
				QDomElement condElem=doc.createElement(condsIt.key());
				QDomText condText=doc.createTextNode(condsIt.value());
				condElem.appendChild(condText);
				mruleElem.appendChild(condElem);
			}
		}

		//Now create a root element for inserting the TRules, depending on the rule type
		QDomElement truleParentElem;
		if(abs->ruleType()==Loop().ruleType())
		{
			truleParentElem=doc.createElement(LOOP_DOMAINS_TAG);
			elem.appendChild(truleParentElem);
		}
		else
		{
			QDomElement tmpElem=doc.createElement(RULE_TRANSITIONS_TAG);
			elem.appendChild(tmpElem);
			truleParentElem=doc.createElement(ALLOW_TAG);
			tmpElem.appendChild(truleParentElem);
		}

		//Then insert TRules inside the truleElem
		const QVector<TRule> &trules=abs->tRules();
		for(int i=0; i<trules.size(); ++i)
		{
			const TRule &trule=trules[i];
			QDomElement truleElem;

			//Create a DOM element for each TRule depending on the rule type
			if(abs->ruleType()==Loop().ruleType())
			{
				truleElem=doc.createElement(LOOP_DOMAIN_TAG);
				truleElem.setAttribute(LOOP_DOMAIN_NAME_TAG, trule.fromDomain());
			}
			else
			{
				truleElem=doc.createElement(RULE_TRANSIT_TAG);
				truleElem.setAttribute(RULE_TRANSIT_FROM_TAG, trule.fromDomain());
			}
			truleParentElem.appendChild(truleElem);

			//Then add notify and prompt for the TRule
			if (trule.hasNotify())
				truleElem.setAttribute(RULE_NOTIFY_TAG, trule.notify()? RULE_TRUE:RULE_TRUE);
			if (trule.hasPrompt())
				truleElem.setAttribute(RULE_PROMPT_TAG, trule.prompt()? RULE_TRUE:RULE_TRUE);
		}
	}

	return doc.toString();
}

void Configuration::readConf()
{
	//Clear all the lists
	_rules.clear();
	_loops.clear();
	_domains.clear();
	_no_kill_programs.clear();

	//Read transitions.xml
	parseTransitionsFile();
}

Configuration::Configuration(const QString &path, bool parse_programs, QList<Program>programs) :
		QObject(),
		_transitions_path(path),
		_parse_programs(parse_programs)
{
	//Setup the initial programs
	for(int i=0; i<programs.size(); ++i)
	{
		Program p=programs.at(i);
		_programs[p.name()]=p;
	}

	//Parse the programs list
	if (_parse_programs)
		parsePrograms();

	//Read the configuration.
	//It will only be read once as Configuration::Configuration() is private and only instance() can call it
	readConf();
}

Configuration::Configuration(const Configuration &other) :
		QObject()
{
	this->operator =(other);
}

const Configuration &Configuration::operator =(const Configuration &other)
{
	_transitions_doc=other._transitions_doc;
	_transitions_path=other._transitions_path;

	_rules=other._rules;
	_loops=other._loops;

	_domains=other._domains;
	_default_domain=other._default_domain;

	_programs=other._programs;
	_no_kill_programs=other._no_kill_programs;
	_parse_programs=other._parse_programs;

	return *this;
}

const Configuration& Configuration::instance()
{
	static Configuration tr("/etc/context.d/transitions.xml");

	return tr;
}

Configuration& Configuration::instance_nonconst()
{
	return const_cast<Configuration&>(instance());
}

void Configuration::updateRules()
{
	_programs.clear();

	//First, read the programs list
	if (_parse_programs)
		parsePrograms();

	readConf();

	emit configurationChanged();
}

Domain Configuration::getDomainByName(QString name) const
{
	return _domains[name];
}

Domain Configuration::defaultDomain() const
{
	return _default_domain;
}

const Transition Configuration::getFirstMatchingTransition(Program app, Domain current_domain, QMap<QString, QString> values) const
{
	bool matchedButRefused=false;

	// Match loops first
	QVector<Loop> app_loops=_loops[app.name()];
	for(int i=0; !matchedButRefused && i<app_loops.size(); i++)
	{
		Transition t=app_loops[i].matches(app, current_domain, values, matchedButRefused);
		if(t.isValid())
			return t;
	}

	// Match rules next
	QVector<Rule> app_rules=_rules[app.name()];
	for(int i=0; !matchedButRefused && i<app_rules.size(); i++)
	{
		Transition t=app_rules[i].matches(app, current_domain, values, matchedButRefused);
		if(t.isValid())
			return t;
	}

	return Transition();
}

QList<QString> Configuration::domainNameList() const
{
	return _domains.keys();
}

QList<Domain> Configuration::domainList() const
{
	return _domains.values();
}

int Configuration::domainCount() const
{
	return _domains.count();
}

void Configuration::addDomain(Domain domain, bool set_default)
{
	if(domain != Domain())
	{
		_domains[domain.name()]=domain;

		if(set_default)
			_default_domain=domain;

		emit domainChanged(QString(), domain.name());
	}
	else
		qWarning("Tried to add an invalid domain to the domains list.");
}

void Configuration::updateDomain(QString old_name, Domain domain, bool set_default)
{
	//If the old domain did exist, replace it
	if(_domains.contains(old_name))
	{
		_domains.remove(old_name);
		_domains[domain.name()]=domain;
		if(set_default)
			_default_domain=domain;

		//Go through each item referencing the domain
		bool loopsUpdated=false;
		bool rulesUpdated=false;

		//If a Loop transits to the changed domain, update it
		QVector<Loop> loops=allLoops();
		for(int i=0; i<loops.size(); ++i)
		{
			Loop &loop=loops[i];
			TRule trule=loop.deleteTRule(old_name);

			if(trule.isValid())
			{
				trule.setFromDomain(domain.name());
				loop.addTRule(trule);
				updateLoop(loop, true);
				loopsUpdated=true;
			}
		}

		//Same with Rules, also check the domain they transit to
		QVector<Rule> rules=allRules();
		for(int i=0; i<rules.size(); ++i)
		{
			Rule &rule=rules[i];
			TRule trule=rule.deleteTRule(old_name);

			if(trule.isValid())
			{
				trule.setFromDomain(domain.name());
				rule.addTRule(trule);
				updateRule(rule, true);
				rulesUpdated=true;
			}

			if(rule.transitTo().name() == old_name)
			{
				rule.setTransitTo(domain);
				updateRule(rule, true);
				rulesUpdated=true;
			}
		}

		//If any item changed, let the application know now that Configuration is entirely updated internally
		if(loopsUpdated && rulesUpdated)
			emit allRulesChanged(true);
		else if (loopsUpdated)
			emit loopsChanged(true);
		else if(rulesUpdated)
			emit rulesChanged(true);

		emit domainChanged(old_name, domain.name());
	}
	else
		qWarning("Domain \"%s\" is not in the domains list and thus cannot be updated.", qPrintable(domain.displayName()));
}

Domain Configuration::deleteDomain(QString name, QString new_default)
{
	Domain result=_domains[name];

	//If the domain has been removed, references to it inside Rules and Loops must be cleared
	if(_domains.remove(name) > 0)
	{
		bool loopsUpdated=false;
		bool rulesUpdated=false;

		//If a Loop contains the deleted domain, remove it, if there are no domains left, remove the whole Loop
		QVector<Loop> loops=allLoops();
		for(int i=0; i<loops.size(); ++i)
		{
			Loop &loop=loops[i];

			if(loop.deleteTRule(name).isValid())
			{
				if(loop.tRules().size() == 0)
					deleteLoop(loop.id(), true);
				else
					updateLoop(loop, true);

				loopsUpdated=true;
			}
		}

		//Same with Rules, also check the target domain
		QVector<Rule> rules=allRules();
		for(int i=0; i<rules.size(); ++i)
		{
			Rule &rule=rules[i];

			if(rule.deleteTRule(name).isValid())
			{
				if(rule.tRules().size() == 0)
					deleteRule(rule.id(), true);
				else
					updateRule(rule, true);

				rulesUpdated=true;
			}

			//If the rule transits to the deleted domain, delete the rule
			if(rule.transitTo().name() == name)
			{
				deleteRule(rule.id(), true);
				rulesUpdated=true;
			}
		}

		//If any item changed, let the application know now that Configuration is entirely updated internally
		if(loopsUpdated && rulesUpdated)
			emit allRulesChanged(true);
		else if (loopsUpdated)
			emit loopsChanged(true);
		else if(rulesUpdated)
			emit rulesChanged(true);

		//Set the new default domain, if the deleted one used to be default
		if(_default_domain==result)
		{
			if(_domains.contains(new_default))
				_default_domain=_domains[new_default];
			else
				_default_domain=Domain();
		}

		emit domainChanged(name, QString());
	}

	return result;
}

bool Configuration::isProgramAllowed(QString name, QString full_path) const
{
	if(!_programs.contains(name))
		return false;
	else
		return _programs[name].fullPath()==full_path;
}

Program Configuration::getProgramByName(QString name, bool allow_invalid) const
{
	Program p=_programs[name];

	if(!p.isValid() && allow_invalid)
		return Program(name, "Unknown ("+name+")", ":/icons/unknown-program", QString(), QString());
	else
		return p;
}

QList<Program> Configuration::programList() const
{
	return _programs.values();
}

QList<QString> Configuration::programNameList() const
{
	return _programs.keys();
}

QList<QString> Configuration::noKillProgramList() const
{
	return _no_kill_programs;
}

bool Configuration::isProgramKillable(const QString& full_path) const
{
	return !_no_kill_programs.contains(full_path);
}

QVector<AbstractRulePtr> Configuration::allAbstractRules(bool sort) const
{
	QVector<AbstractRulePtr> result;

	for(QMap<QString, QVector<Loop> >::const_iterator it=_loops.begin(); it!=_loops.end(); ++it)
	{
		const QVector<Loop> &vect=it.value();

		for(QVector<Loop>::const_iterator vectit=vect.begin(); vectit!=vect.end(); ++vectit)
			result.append(AbstractRulePtr(new Loop(*vectit)));
	}

	for(QMap<QString, QVector<Rule> >::const_iterator it=_rules.begin(); it!=_rules.end(); ++it)
	{
		const QVector<Rule> &vect=it.value();

		for(QVector<Rule>::const_iterator vectit=vect.begin(); vectit!=vect.end(); ++vectit)
			result.append(AbstractRulePtr(new Rule(*vectit)));
	}

	if(sort)
		qSort(result.begin(), result.end(), AbstractRule::hasHigherPriority);

	return result;
}

QVector<Loop> Configuration::allLoops() const
{
	QVector<Loop> result;

	for(QMap<QString, QVector<Loop> >::const_iterator it=_loops.begin(); it!=_loops.end(); ++it)
		result << it.value();

	return result;
}

QVector<Rule> Configuration::allRules() const
{
	QVector<Rule> result;

	for(QMap<QString, QVector<Rule> >::const_iterator it=_rules.begin(); it!=_rules.end(); ++it)
		result << it.value();

	return result;
}

QVector<AbstractRulePtr> Configuration::abstractRulesForProgram(QString name, bool sort) const
{
	QVector<AbstractRulePtr> result;

	QVector<Loop> loops=loopsForProgram(name);
	for(int i=0; i<loops.size(); ++i)
		result << AbstractRulePtr(new Loop(loops[i]));

	QVector<Rule> rules=rulesForProgram(name);
	for(int i=0; i<rules.size(); ++i)
		result << AbstractRulePtr(new Rule(rules[i]));

	if(sort)
		qSort(result.begin(), result.end(), AbstractRule::hasHigherPriority);

	return result;
}

QVector<Loop> Configuration::loopsForProgram(QString name) const
{
	return _loops[name];
}

QVector<Rule> Configuration::rulesForProgram(QString name) const
{
	return _rules[name];
}

QVector<Loop> Configuration::loopsForDomain(Domain dom) const
{
	QVector<Loop> vect;
	QMap<QString, QVector<Loop> >::const_iterator it;

	for(it=_loops.begin(); it!=_loops.end(); ++it)
	{
		const QVector<Loop> &cur=it.value();
		for(int i=0, count=cur.size(); i<count; ++i)
		{
			const Loop &loop=cur[i];
			if(loop.tRulesContain(dom))
				vect.append(loop);
		}
	}

	return vect;
}

QVector<Rule> Configuration::rulesForDomain(Domain dom) const
{
	QVector<Rule> vect;
	QMap<QString, QVector<Rule> >::const_iterator it;

	for(it=_rules.begin(); it!=_rules.end(); ++it)
	{
		const QVector<Rule> &cur=it.value();
		for(int i=0, count=cur.size(); i<count; ++i)
		{
			const Rule &rule=cur[i];
			if((rule.transitTo().name()==dom.name()) || (rule.tRulesContain(dom)))
				vect.append(rule);
		}
	}

	return vect;
}

void Configuration::addLoop(Loop loop)
{
	addLoop(loop, false);
}

void Configuration::addLoop(Loop loop, bool quiet)
{
	QVector<Loop> &loops=_loops[loop.program().name()];

	//Look for the position where to insert the new loop
	int pos=loops.size();
	for(int i=0; i<pos; ++i)
	{
		if(loops[i].priority() > loop.priority())
			pos=i;
	}

	//Insert the loop, and emit a signal if asked to
	loops.insert(pos, loop);
	if(!quiet)
		emit loopsChanged();
}

void Configuration::addRule(Rule rule)
{
	addRule(rule, false);
}

void Configuration::addRule(Rule rule, bool quiet)
{
	QVector<Rule> &rules=_rules[rule.program().name()];

	//Look for the position where to insert the new rule
	int pos=rules.size();
	for(int i=0; i<pos; ++i)
	{
		if(rules[i].priority() > rule.priority())
			pos=i;
	}

	//Insert the rule, and emit a signal if asked to
	rules.insert(pos, rule);
	if(!quiet)
		emit rulesChanged();
}

void Configuration::updateLoop(Loop loop)
{
	updateLoop(loop, false);
}

void Configuration::updateLoop(Loop loop, bool quiet)
{
	deleteLoop(loop, true);
	addLoop(loop, quiet);
}

void Configuration::updateRule(Rule rule)
{
	updateRule(rule, false);
}

void Configuration::updateRule(Rule rule, bool quiet)
{
	deleteRule(rule, true);
	addRule(rule, quiet);
}

AbstractRulePtr Configuration::findAbstractRule(int id) const
{
	//Return a valid found loop
	Loop loop=findLoop(id);
	if(loop.isValid())
		return AbstractRulePtr(new Loop(loop));

	//Return a valid found rule
	Rule rule=findRule(id);
	if(rule.isValid())
		return AbstractRulePtr(new Rule(rule));

	//Return an invalid rule
	return AbstractRulePtr(new Rule());
}

Loop Configuration::findLoop(int id) const
{
	QMap<QString, QVector<Loop> >::const_iterator it;

	for(it=_loops.begin(); it!=_loops.end(); ++it)
	{
		const QVector<Loop> &vect=it.value();

		for(QVector<Loop>::const_iterator loopit=vect.begin(); loopit!=vect.end(); ++loopit)
			if((*loopit).id() == id)
				return const_cast<Loop&>(*loopit);
	}

	return Loop();
}

Rule Configuration::findRule(int id) const
{
	QMap<QString, QVector<Rule> >::const_iterator it;

	for(it=_rules.begin(); it!=_rules.end(); ++it)
	{
		const QVector<Rule> &vect=it.value();

		for(QVector<Rule>::const_iterator ruleit=vect.begin(); ruleit!=vect.end(); ++ruleit)
			if((*ruleit).id() == id)
				return const_cast<Rule&>(*ruleit);
	}

	return Rule();
}

AbstractRulePtr Configuration::deleteAbstractRule(int id)
{
	AbstractRulePtr ptr=findAbstractRule(id);
	deleteAbstractRule(ptr);

	return ptr;
}

void Configuration::deleteAbstractRule(AbstractRulePtr &rule)
{
	if(rule->ruleType()==Loop().ruleType())
		deleteLoop(*rule.dynamicCast<Loop>());
	else
		deleteRule(*rule.dynamicCast<Rule>());

}

Loop Configuration::deleteLoop(int id)
{
	return deleteLoop(id, false);
}

Loop Configuration::deleteLoop(int id, bool quiet)
{
	QMap<QString, QVector<Loop> >::iterator it;

	for(it=_loops.begin(); it!=_loops.end(); ++it)
	{
		QVector<Loop> &vect=it.value();
		QVector<Loop>::iterator loopit;

		for(loopit=vect.begin(); loopit!=vect.end(); ++loopit)
		{
			if((*loopit).id() == id)
			{
				Loop copy=*loopit;
				vect.erase(loopit);

				if(!quiet)
					emit loopsChanged();
				return copy;
			}
		}
	}

	return Loop();
}

void Configuration::deleteLoop(Loop &loop)
{
	deleteLoop(loop, false);
}

void Configuration::deleteLoop(Loop &loop, bool quiet)
{
	QVector<Loop>::iterator it;

	for(it=_loops[loop.program().name()].begin(); it!=_loops[loop.program().name()].end(); ++it)
	{
		if((*it).id() == loop.id())
		{
			_loops[loop.program().name()].erase(it);

			if(!quiet)
				emit loopsChanged();
			return;
		}
	}
}

Rule Configuration::deleteRule(int id)
{
	return deleteRule(id, false);
}

Rule Configuration::deleteRule(int id, bool quiet)
{
	QMap<QString, QVector<Rule> >::iterator it;

	for(it=_rules.begin(); it!=_rules.end(); ++it)
	{
		QVector<Rule> &vect=it.value();
		QVector<Rule>::iterator ruleit;

		for(ruleit=vect.begin(); ruleit!=vect.end(); ++ruleit)
		{
			if((*ruleit).id() == id)
			{
				Rule copy=*ruleit;
				vect.erase(ruleit);

				if(!quiet)
					emit rulesChanged();
				return copy;
			}
		}
	}

	return Rule();
}

void Configuration::deleteRule(Rule &rule)
{
	deleteRule(rule, false);
}

void Configuration::deleteRule(Rule &rule, bool quiet)
{
	QVector<Rule>::iterator it;

	for(it=_rules[rule.program().name()].begin(); it!=_rules[rule.program().name()].end(); ++it)
	{
		if((*it).id() == rule.id())
		{
			_rules[rule.program().name()].erase(it);

			if(!quiet)
				emit rulesChanged();
			return;
		}
	}
}

void Configuration::swapPriorities(int id, int other_id)
{
	Loop loop=deleteLoop(id, true);
	if(loop.isValid())
	{
		Loop other_loop=deleteLoop(other_id, true);

		//Both rules are Loops
		if(other_loop.isValid())
		{
			AbstractRule::swapPriorities(&loop, &other_loop);
			addLoop(loop, true);
			addLoop(other_loop, true);
			emit loopsChanged();
		}
		else
		{
			Rule other_rule=deleteRule(other_id, true);

			//Second item is a Rule
			if(other_rule.isValid())
			{
				AbstractRule::swapPriorities(&loop, &other_rule);
				addLoop(loop, true);
				addRule(other_rule, true);
				emit allRulesChanged();
			}
		}
	}
	else
	{
		Rule rule=deleteRule(id, true);
		if(rule.isValid())
		{
			Loop other_loop=deleteLoop(other_id, true);

			//Second item is a Loop
			if(other_loop.isValid())
			{
				AbstractRule::swapPriorities(&rule, &other_loop);
				addRule(rule, true);
				addLoop(other_loop, true);
				emit allRulesChanged();
			}
			else
			{
				Rule other_rule=deleteRule(other_id, true);

				//Both items are Rules
				if(other_rule.isValid())
				{
					AbstractRule::swapPriorities(&rule, &other_rule);
					addRule(rule, true);
					addRule(other_rule, true);
					emit rulesChanged();
				}
			}
		}
	}
}
