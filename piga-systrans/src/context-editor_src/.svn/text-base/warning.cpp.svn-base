#include "configurationstack.h"
#include "warning.h"

Warning::Warning() : _warning_type(InvalidType)
{}

inline QString Warning::text(bool /*item_pov*/) const
{	return "Warning";	}

inline QString Warning::icon() const
{	return ":/icons/warning32";	}

WarningType Warning::warningType() const
{	return _warning_type;	}



DomainWarning::DomainWarning(QString name) : _domain(name)
{}

QString DomainWarning::domain() const
{	return _domain;	}



UnreachableDomainWarning::UnreachableDomainWarning(QString name) :
		DomainWarning(name)
{
	_warning_type=Unreachable;
}

QString UnreachableDomainWarning::text(bool dom_pov) const
{
	QString header;

	//Fetch the current domain
	ConfigurationStack &stack=ConfigurationStack::instance();
	const Domain &dom=stack.current().getDomainByName(_domain);

	//Setup header with current rule information
	if(!dom_pov)
	{
		header="<b>Domain "+QString(" <a href=\"dom://%1\">").arg(dom.name())+dom.displayName()+"</a>";
		header+=" is unreachable in the transitions graph.</b>";
	}
	else
	{
		header="<b>Is unreachable in the transitions graph.</b>";
	}

	return header;
}

QString UnreachableDomainWarning::icon() const
{	return ":/icons/unreachable-domain32";	}



RuleWarning::RuleWarning(int id) : _rule(id)
{}

int RuleWarning::rule() const
{	return _rule;	}



BypassRuleWarning::BypassRuleWarning(int id, Domain source, QMap<QString, QList<int> >bypassed_mrules) :
		RuleWarning(id),
		_source(source),
		_bypassed_mrules(bypassed_mrules)
{
	_warning_type=Bypassed;
}

QString BypassRuleWarning::text(bool rule_pov) const
{
	QString header;
	QString body;

	//Fetch the current rule
	ConfigurationStack &stack=ConfigurationStack::instance();
	AbstractRulePtr abs=stack.findAbstractRule(_rule);

	//Setup header with current rule and domain information
	if(!rule_pov)
	{
		header="<b>"+abs->ruleType()+QString(" <a href=\"rule://%1\">").arg(abs->id())+abs->displayName()+"</a> is";
		if(_bypassed_mrules.count()!=abs->mRules().count())
			header+=" partially";
		header+=" bypassed in Domain <a href=\"dom://"+_source.name()+"\">"+_source.displayName()+"</a>.</b>";
	}
	else
	{
		if(_bypassed_mrules.count()!=abs->mRules().count())
			header="<b>Partially bypassed";
		else
			header="<b>Bypassed";

		header+=" in Domain <a href=\"dom://"+_source.name()+"\">"+_source.displayName()+"</a>.</b>";
	}

	//Setup body
	for(QMap<QString, QList<int> >::const_iterator it=_bypassed_mrules.begin(); it!=_bypassed_mrules.end(); ++it)
	{
		//Fetch a readable list of all the rules
		QString rulesStr;
		QList<int> rules=it.value();
		for(int i=0; i<rules.size(); ++i)
		{
			AbstractRulePtr bypasser=stack.findAbstractRule(rules[i]);
			rulesStr+=QString("<a href=\"rule://%1\">").arg(rules[i])+bypasser->displayName()+"</a>, ";
		}
		rulesStr.chop(QString(", ").length());

		//Add a line to the body for the mrule
		body+="- Matching rule '"+it.key()+"' is bypassed by "+rulesStr+".<br />";
	}
	if(body.endsWith("<br />"))
		body.chop(6);

	return header+"<br />"+body;
}

QString BypassRuleWarning::icon() const
{	return ":/icons/bypass32";	}

Domain BypassRuleWarning::source() const
{	return _source;	}



UnreachableRuleWarning::UnreachableRuleWarning(int id) :
		RuleWarning(id)
{
	_warning_type=Unreachable;
}

QString UnreachableRuleWarning::text(bool rule_pov) const
{
	QString header;
	QString body;

	//Fetch the current rule
	ConfigurationStack &stack=ConfigurationStack::instance();
	AbstractRulePtr abs=stack.findAbstractRule(_rule);

	//Setup header with current rule information
	if(!rule_pov)
	{
		header="<b>"+abs->ruleType()+QString(" <a href=\"rule://%1\">").arg(abs->id())+abs->displayName()+"</a>";
		header+=" is never called because it only has unreachable domains.</b>";
	}
	else
	{
		header="<b>Only has unreachable domains.</b>";
	}

	//Setup body
	QVector<Domain> domains=abs->matchedDomains(stack.current().domainList());
	if(domains.isEmpty())
	{
		body="- The transition rule has no domain defined.";
	}
	else if (domains.size()==1)
	{
		body="- Domain <a href=\"dom://"+domains[0].name()+"\">"+domains[0].displayName()+"</a> is never reached in the graph.";
	}
	else if(domains.size()>1)
	{
		body="- Domains ";
		for(int i=0; i<domains.size()-1; ++i)
		{
			body+="<a href=\"dom://"+domains[i].name()+"\">"+domains[i].displayName()+"</a>, ";
		}
		body.chop(QString(", ").length());
		body+=" and <a href=\"dom://"+domains[domains.size()-1].name()+"\">"+domains[domains.size()-1].displayName()+"</a>";
		body+=" are never reached in the graph.";
	}


	return header+"<br />"+body;
}

QString UnreachableRuleWarning::icon() const
{	return ":/icons/unreachable-domain32";	}



UnknownProgramRuleWarning::UnknownProgramRuleWarning(int id) :
		RuleWarning(id)
{
	_warning_type=UnknownProgram;
}

QString UnknownProgramRuleWarning::text(bool rule_pov) const
{
	QString header;
	QString body;

	//Fetch the current rule
	ConfigurationStack &stack=ConfigurationStack::instance();
	AbstractRulePtr abs=stack.findAbstractRule(_rule);

	//Setup header with current rule information
	if(!rule_pov)
	{
		header="<b>"+abs->ruleType()+QString(" <a href=\"rule://%1\">").arg(abs->id())+abs->displayName()+"</a>";
		header+=" has an unknown program.</b>";
	}
	else
	{
		header="<b>Has an unknown program.</b>";
	}

	//Setup body
	body="- Program '"+abs->appName()+"' has not been defined or imported in the Editor yet.";

	return header+"<br />"+body;
}

QString UnknownProgramRuleWarning::icon() const
{	return ":/icons/unknown-program32";	}



InvalidMRuleRuleWarning::InvalidMRuleRuleWarning(int id) :
		RuleWarning(id)
{
	_warning_type=InvalidMRuleProgramData;
}

QString InvalidMRuleRuleWarning::text(bool rule_pov) const
{
	QString header;
	QString body;

	//Fetch the current rule
	ConfigurationStack &stack=ConfigurationStack::instance();
	AbstractRulePtr abs=stack.findAbstractRule(_rule);

	//Setup header with current rule information
	if(!rule_pov)
	{
		header="<b>"+abs->ruleType()+QString(" <a href=\"rule://%1\">").arg(abs->id())+abs->displayName()+"</a>";
		header+=" has matching rules with invalid data.</b>";
	}
	else
	{
		header="<b>Has matching rules with invalid data.</b>";
	}

	//Setup body: for each mrule, check if it has anything wrong, and if so, add a line to the body
	Program prog=stack.current().getProgramByName(abs->appName());
	QVector<MRule> mrules=abs->mRules();
	for(QVector<MRule>::iterator it=mrules.begin(); it!=mrules.end(); ++it)
	{
		const MRule &mrule=*it;

		//Check that the action is valid (if defined)
		if(mrule.actionName()!=QString() && prog.getActionByName(mrule.actionName())==ProgramAction())
		{
			body+="- Matching rule '"+mrule.displayName()+"' uses action '"+mrule.actionName()+"' which does not exist";
			body+=" in Program <a href=\"prog://"+prog.name()+"\">"+prog.displayName()+"</a>.<br />";
		}
		//If it is valid and defined, check that all variables exist and have a valid value
		else if(mrule.actionName()!=QString())
		{
			QMap<QString, QString> conds=mrule.conditions();
			for(QMap<QString, QString>::const_iterator it=conds.begin(); it!=conds.end(); ++it)
			{
				ProgramVariable var=prog.getVariableByName(it.key(), mrule.actionName());

				//Invalid variable
				if(var==ProgramVariable())
				{
					body+="- Matching rule '"+mrule.displayName()+"' uses variable '"+var.name()+"' which does not exist";
					body+=" in action '"+mrule.actionName()+"'";
					body+=" of Program <a href=\"prog://"+prog.name()+"\">"+prog.displayName()+"</a>.<br />";
				}
				//Invalid value for the variable's type
				else if(!var.validate(it.value()))
				{
					//Build an intelligible string of what kind of value was expected
					QString expected;

					switch(var.type())
					{
					case Type::Int:
						if(var.intRange().startsWith(":"))
						{
							if(var.intRange().endsWith(":"))
								expected="an integer";
							else
								expected=QString("an integer in range [-&infin;, %1]").arg(var.intMaximum());
						}
						else
						{
							if(var.intRange().endsWith(":"))
								expected=QString("an integer in range [%1, &infin;]").arg(var.intMinimum());
							else
								expected="an integer in range ["+var.intRange().replace(":", ", ")+"]";
						}
						expected="an integer in range ["+var.intRange()+"]";
						break;

					case Type::String:
						expected="a string matching '"+var.stringValidation()+"'";
						break;

					case Type::RegExp:
						expected="a regular expression matching '"+var.stringValidation()+"'";
						break;

					case Type::StringList:
						expected="one of ["+var.stringList().join(", ")+"]";
						break;

					case Type::Boolean:
						expected="a boolean";
						break;
					}

					body+="- In matching rule '"+mrule.displayName()+"', variable '"+var.name()+"' is invalid: ";
					body+="'"+it.value()+"' (expected "+expected+").<br />";
				}
			}
		}
	}
	if(body.endsWith("<br />"))
		body.chop(6);

	return header+"<br />"+body;
}

QString InvalidMRuleRuleWarning::icon() const
{	return ":/icons/invalid-mrule32";	}



UndefinedMRuleRuleWarning::UndefinedMRuleRuleWarning(int id) :
		RuleWarning(id)
{
	_warning_type=UndefinedMRuleProgramData;
}

QString UndefinedMRuleRuleWarning::text(bool rule_pov) const
{
	QString header;
	QString body;

	//Fetch the current rule
	ConfigurationStack &stack=ConfigurationStack::instance();
	AbstractRulePtr abs=stack.findAbstractRule(_rule);

	//Setup header with current rule information
	if(!rule_pov)
	{
		header="<b>"+abs->ruleType()+QString(" <a href=\"rule://%1\">").arg(abs->id())+abs->displayName()+"</a>";
		header+=" has matching rules with no action defined.</b>";
	}
	else
	{
		header="<b>Has matching rules with no action defined.</b>";
	}

	//Setup body: fetch mrules, remove the ones with a defined action, and then do the display
	QVector<MRule> mrules=abs->mRules();
	for(QVector<MRule>::iterator it=mrules.begin(); it!=mrules.end();)
	{
		const MRule &mrule=*it;

		if(mrule.actionName()==QString())
			++it;
		else
			it=mrules.erase(it);
	}

	body="- Action missing for ";
	if(mrules.size()==1)
	{
		body+="matching rule '"+mrules[0].displayName()+"'.";
	}
	else if(mrules.size()>1)
	{
		body+="matching rules ";
		for(int i=0; i<mrules.size()-1; ++i)
		{
			body+="'"+mrules[i].displayName()+"', ";
		}
		body.chop(QString(", ").length());
		body+=" and '"+mrules[mrules.size()-1].displayName()+"'.";
	}

	return header+"<br />"+body;
}

QString UndefinedMRuleRuleWarning::icon() const
{	return ":/icons/undefined-mrule32";	}




MatchedSomewhereElseRuleWarning::MatchedSomewhereElseRuleWarning(int id, int other, MRule this_mrule, MRule other_mrule) :
		RuleWarning(id),
		_other(other),
		_this_mrule(this_mrule),
		_other_mrule(other_mrule)
{
	_warning_type=MatchedSomewhereElse;
}

QString MatchedSomewhereElseRuleWarning::text(bool /*rule_pov*/) const
{
	return "Not implemented yet.";
}

QString MatchedSomewhereElseRuleWarning::icon() const
{	return ":/icons/duplicate-mrule";	}




ConfidentialityLossRuleWarning::ConfidentialityLossRuleWarning(int id) :
		RuleWarning(id)
{
	_warning_type=ConfidentialityLoss;
}

QString ConfidentialityLossRuleWarning::text(bool rule_pov) const
{
	QString header;
	QString body;

	//Fetch the current rule (only makes sense for a Rule and not a Loop)
	ConfigurationStack &stack=ConfigurationStack::instance();
	const Rule &rule=stack.current().findRule(_rule);

	if(!rule_pov)
	{
		header="<b>"+rule.ruleType()+QString(" <a href=\"rule://%1\">").arg(rule.id())+rule.displayName()+"</a>";
		header+=" causes a loss of confidentiality.</b>";
	}
	else
	{
		header="<b>Causes a loss of confidentiality.</b>";
	}

	//Setup body: fetch mrules, remove the ones with a defined action, and then do the display
	QVector<Domain> domains=rule.matchedDomains(stack.current().domainList());
	for(QVector<Domain>::iterator it=domains.begin(); it!=domains.end();)
	{
		const Domain &dom=*it;

		if(dom.confidentiality()>rule.transitTo().confidentiality())
			++it;
		else
			it=domains.erase(it);
	}

	if(domains.size()==1)
	{
		body="- Domain <a href=\"dom://"+domains[0].name()+"\">"+domains[0].displayName()+
			 QString("</a> (%1) is").arg(domains[0].confidentiality());
	}
	else if(domains.size()>1)
	{
		body="- Domains ";
		for(int i=0; i<domains.size()-1; ++i)
		{
			body+="<a href=\"dom://"+domains[i].name()+"\">"+domains[i].displayName()+
				  QString("</a> (%1), ").arg(domains[0].confidentiality());
		}
		body.chop(QString(", ").length());
		body+=" and <a href=\"dom://"+domains[domains.size()-1].name()+"\">"+domains[domains.size()-1].displayName()+
			  QString("</a> (%1) are").arg(domains[domains.size()-1].confidentiality());
	}
	body+=" more confidential than target Domain <a href=\"dom://"+rule.transitTo().name()+"\">"+
		  rule.transitTo().displayName()+QString("</a> (%1).").arg(rule.transitTo().confidentiality());

	return header+"<br />"+body;
}

QString ConfidentialityLossRuleWarning::icon() const
{	return ":/icons/confidentiality-decrease32";	}




ReliabilityLossRuleWarning::ReliabilityLossRuleWarning(int id) :
		RuleWarning(id)
{
	_warning_type=ReliabilityLoss;
}

QString ReliabilityLossRuleWarning::text(bool rule_pov) const
{
	QString header;
	QString body;

	//Fetch the current rule (only makes sense for a Rule and not a Loop)
	ConfigurationStack &stack=ConfigurationStack::instance();
	const Rule &rule=stack.current().findRule(_rule);

	//Setup header with current rule information
	if(!rule_pov)
	{
		header="<b>"+rule.ruleType()+QString(" <a href=\"rule://%1\">").arg(rule.id())+rule.displayName()+"</a>";
		header+=" causes a loss of reliability.</b>";
	}
	else
	{
		header="<b>Causes a loss of reliability.</b>";
	}

	//Setup body: fetch mrules, remove the ones with a defined action, and then do the display
	QVector<Domain> domains=rule.matchedDomains(stack.current().domainList());
	for(QVector<Domain>::iterator it=domains.begin(); it!=domains.end();)
	{
		const Domain &dom=*it;

		if(dom.reliability()>rule.transitTo().reliability())
			++it;
		else
			it=domains.erase(it);
	}

	if(domains.size()==1)
	{
		body="- Domain <a href=\"dom://"+domains[0].name()+"\">"+domains[0].displayName()+
			 QString("</a> (%1) is").arg(domains[0].reliability());
	}
	else if(domains.size()>1)
	{
		body="- Domains ";
		for(int i=0; i<domains.size()-1; ++i)
		{
			body+="<a href=\"dom://"+domains[i].name()+"\">"+domains[i].displayName()+
				  QString("</a> (%1), ").arg(domains[0].reliability());
		}
		body.chop(QString(", ").length());
		body+=" and <a href=\"dom://"+domains[domains.size()-1].name()+"\">"+domains[domains.size()-1].displayName()+
			  QString("</a> (%1) are").arg(domains[domains.size()-1].reliability());
	}
	body+=" more reliable than target Domain <a href=\"dom://"+rule.transitTo().name()+"\">"+
		  rule.transitTo().displayName()+QString("</a> (%1).").arg(rule.transitTo().reliability());

	return header+"<br />"+body;
}

QString ReliabilityLossRuleWarning::icon() const
{	return ":/icons/reliability-decrease32";	}
