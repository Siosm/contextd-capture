#include <QtXml/QDomDocument>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QSet>
#include <QTime>
#include "appexportdialog.h"
#include "editorconfig.h"
#include "configurationstack.h"
#include "xml_common.h"

ConfigurationStack::ConfigurationStack(QObject *parent) :
	QObject(parent),
	_current_before_last_change(QString(), false),
	_file_path(),
	_parent(0)
{
	//Initialise the stack and the programs
	initialise(true);

	//Setup autosave timer
	connect(&_autosave_timer, SIGNAL(timeout()), this, SLOT(onSaveToTmp()));
	connect(&EditorConfig::instance(), SIGNAL(updated()), this, SLOT(onSettingsChange()));
	onSettingsChange();
}

ConfigurationStack::~ConfigurationStack()
{
	for(int i=0; i<_stack.size(); ++i)
		delete _stack[i].first;
	_stack.clear();
}

ConfigurationStack &ConfigurationStack::instance()
{
	static ConfigurationStack tr;

	return tr;
}

const Configuration& ConfigurationStack::current() const
{
	return *(_stack[_current_index].first);
}

Configuration& ConfigurationStack::currentForModifying()
{
	const Configuration &inst=current();
	_current_before_last_change=inst;

	return const_cast<Configuration&>(inst);
}

bool ConfigurationStack::hasChanges() const
{
	return _last_saved_index!=_current_index;
}

void ConfigurationStack::setParentWidget(QWidget *parent)
{
	_parent=parent;
}

QString ConfigurationStack::transitionsFilePath(QString alternative) const
{
	if(_file_path!=QString())
		return _file_path;
	else
		return alternative;
}

bool ConfigurationStack::proposeSaveTransitions()
{
	int answer=QMessageBox::question(_parent, "Save Transitions?",
									 "<b>You have unsaved changes to the transitions file.</b><br /><br />If you don't save, these changes will be lost. Do you want to save now?",
									 QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);

	//If the user wants to save
	if(answer==QMessageBox::Yes)
	{
		QString path=_file_path;

		//If there is no already set file path, ask for one, if the user cancels, don't perform next action
		if(path==QString())
			path=QFileDialog::getSaveFileName(_parent, "Save transitions as...", EditorConfig::lastSavedPath(), "All files (*);;XML files (*.xml)");
		if (path==QString())
			return false;
		else
			EditorConfig::setLastSavedPath(path);

		//Perform the next action only if saving does not fail
		return saveTransitions(path);
	}
	//Else, if the user doesn't want to save, we can perform the next action
	else
		return answer==QMessageBox::No;
}

void ConfigurationStack::openTransitionsFile(QString path)
{
	bool performAction=true;

	if(hasChanges())
		performAction=proposeSaveTransitions();

	if(performAction)
	{
		if (!QFile::exists(path))
			QMessageBox::critical(_parent, "Error",
								  "<b>Could not open file " + path +
								  ".</b><br /><br />The file could not be found.",
								  QMessageBox::Ok);
		else
		{
			if(transitionsFileWellFormed(path))
				reinitialise(path);
			else
			{
				QMessageBox::critical(_parent, "Error",
									  "<b>Could not open file " + path +
									  ".</b><br /><br />The file could not be read or is corrupted.",
									  QMessageBox::Ok);
			}
		}
	}
}

void ConfigurationStack::openTmpTransitions(QString path)
{
	openTransitionsFile(path);
	_file_path=QString();
}

void ConfigurationStack::addProgram(Program p, QString path)
{

	//Add the program if it was not already known
	if (!_programs.contains(p.name()))
	{
		//If the program is not stored in the application data location, change it's path
		QString dataLoc=QDesktopServices::storageLocation(QDesktopServices::DataLocation);
		if(!path.startsWith(dataLoc))
		{
			path=dataLoc+"/"+p.name()+".xml";
		}

		//Check that no program already has the same file path
		QMap<QString, QPair<Program, QString> >::const_iterator it;
		for(it=_programs.begin(); it!=_programs.end(); ++it)
			if(it.value().second==path)
			{
				//If there is one, ask if it should be replaced
				int answer=QMessageBox::question(_parent, "Program already exists",
									  "Program \""+p.name()+"\" has already been imported. Do you want to reload the file?",
									  QMessageBox::Yes, QMessageBox::No);

				if(answer==QMessageBox::Yes)
					deleteProgram(it.key());
				else
					return;
			}

		//Add the program, and set its now file path
		_programs.insert(p.name(), QPair<Program, QString>(p, path));

		//Try to save the file, but don't notify a failure (this will be done when the application is closed)
		QFile file(path);

		//Open the file in write mode and write the data
		if(file.open(QIODevice::WriteOnly))
			file.write(qPrintable(p.retrieveDOM()));

		//We update the current copy before to avoid any race condition and make sure it's up-to-date
		//before current() emits programChanged()
		const Configuration &inst=current();
		_current_before_last_change=inst;
		_current_before_last_change.addProgram(p);

		//Update all instances of configuration in the stack
		for(int i=0; i<_stack.size(); ++i)
		{
			Configuration *conf=_stack[i].first;
			conf->addProgram(p);
		}
	}
	//The program is known, ask if it should be replaced
	else
	{
		int answer=QMessageBox::question(_parent, "Program already exists",
							  "Program \""+p.displayName()+"\" has already been imported. Do you want to replace it?",
							  QMessageBox::Yes, QMessageBox::No);

		if(answer==QMessageBox::Yes)
		{
			deleteProgram(p.name());
			addProgram(p, path);
		}
	}
}

void ConfigurationStack::addProgramsFromFiles(QStringList paths, bool compact_errors)
{
	//A list of files that could not be opened
	QStringList failed;

	for(int i=0; i<paths.size(); ++i)
	{
		//Check if the file exists
		QString path=paths.at(i);
		if (QFile::exists(path))
		{
			QFile file(path);
			Program p(file);

			//Check if the program is valid
			if(p.isValid())
			{
				addProgram(p, path);
			}
			else
			{
				if (!compact_errors)
					QMessageBox::critical(_parent, "Error",
								   "<b>Could not load program " + path +
								   ".</b><br /><br />The file is not a program XML file or is corrupted.",
								   QMessageBox::Ok);
				else
					failed.append(path);
			}
		}
		else
		{
			if (!compact_errors)
				QMessageBox::critical(_parent, "Error",
								   "<b>Could not open program " + path +
								   ".</b><br /><br />The file does not exist.",
								   QMessageBox::Ok);
			else
				failed.append(path);
		}
	}

	//If we use a compact errors dialog and if some programs failed to open, display it
	if(compact_errors && failed.size()>0)
	{
		QMessageBox box(QMessageBox::Warning, "Warning",
						"<b>Could not open some applications</b><br /><br />"
						"The files for these applications could not be found or were corrupted. "
						"It will not be possible to edit the rules relying on these applications until you import them again.");

		qSort(failed);
		box.setDetailedText("Files that could not be opened:\n\n" + failed.join("\n"));

		box.show();
		box.exec();
	}
}

void ConfigurationStack::updateProgram(QString old_name, const Program& new_program)
{
	//Go through each known program
	QMap<QString, QPair<Program, QString> >::iterator it;
	for(it=_programs.begin(); it!=_programs.end(); ++it)
	{
		//If the program has the same name as the one we look for
		if(it.key()==old_name)
		{
			//Update the program internally
			QString path=it.value().second;
			_programs.remove(it.key());
			_programs.insert(new_program.name(), QPair<Program, QString>(new_program, it.value().second));

			//Update the program in each instance of Configuration
			const Configuration &inst=current();
			_current_before_last_change=inst;
			_current_before_last_change.updateProgram(old_name, new_program);

			//Update all instances of configuration in the stack
			for(int i=0; i<_stack.size(); ++i)
			{
				Configuration *conf=_stack[i].first;
				conf->updateProgram(old_name, new_program);
			}

			//Try to save the file, but don't notify a failure (this will be done when the application is closed)
			QFile file(path);

			//Open the file in write mode and write the data
			if(file.open(QIODevice::WriteOnly))
				file.write(qPrintable(new_program.retrieveDOM()));

			return;
		}
	}
}

void ConfigurationStack::deleteProgram(QString name)
{
	//Go through each known program
	QMap<QString, QPair<Program, QString> >::iterator it;
	for(it=_programs.begin(); it!=_programs.end(); ++it)
	{
		//If the program has the same name as the one we look for
		Program p=it.value().first;
		if(p.name()==name)
		{
			//Remove the program from the known ones
			_programs.erase(it);

			//Remove the program from each instance of Configuration
			const Configuration &inst=current();
			_current_before_last_change=inst;
			_current_before_last_change.deleteProgram(p.name());

			//Update all instances of configuration in the stack
			for(int i=0; i<_stack.size(); ++i)
			{
				Configuration *conf=_stack[i].first;
				conf->deleteProgram(p.name());
			}
			return;
		}
	}
}

void ConfigurationStack::savePrograms()
{
	//Save all the programs to their path and save the path of known programs
	QStringList names;
	QString dataLoc=QDesktopServices::storageLocation(QDesktopServices::DataLocation);
	QDir("/").mkpath(dataLoc);

	//Check if the user can write in the app data directory
	if(QFileInfo(dataLoc).isWritable())//FIXME testing
	{
		//For each program, save its contents to its path
		QMap<QString, QPair<Program, QString> >::const_iterator it;
		for(it=_programs.begin(); it!=_programs.end(); ++it)
		{
			Program p=it.value().first;

			if(p.isValid())
			{
				QFile file(it.value().second);

				//Open the file in write mode
				if(file.open(QIODevice::WriteOnly))
				{
					//Write the data
					if(!file.write(qPrintable(p.retrieveDOM())))
					{
						QMessageBox::critical(_parent, "Error",
													"<b>Could not save " + p.displayName() +
													".</b><br /><br />This may happen if there is no space left on your hard drive.",
													QMessageBox::Ok);
					}
					else
						names << it.value().second;
				}
			}
		}
	}
	else
	{
		int r=QMessageBox::question(_parent, "Error",
									"<b>Could not save applications to " + dataLoc +
									".</b><br /><br />You do not have the sufficient rights to write to this location. Do you want to export applications to another location?",
									QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);

		if(r==QMessageBox::Yes)
			AppExportDialog::doExport(_parent);
	}

	//Save the list of applications into the settings
	QSettings().setValue("knownApplications", names);
}

AbstractRulePtr ConfigurationStack::findAbstractRule(int id)
{
	if(!_arp_cache.contains(id))
		_arp_cache[id]=current().findAbstractRule(id);

	return _arp_cache[id];
}

int ConfigurationStack::warningTypesForRule(int id) const
{
	int result=InvalidType;

	if(_rule_warnings.contains(id))
		for(QVector<WarningPtr>::const_iterator it=_rule_warnings[id].begin(); it!=_rule_warnings[id].end(); ++it)
			result|=(*it)->warningType();

	if(_od_rule_warnings.contains(id))
		for(QVector<WarningPtr>::const_iterator it=_od_rule_warnings[id].begin(); it!=_od_rule_warnings[id].end(); ++it)
			result|=(*it)->warningType();

	return result;
}

int ConfigurationStack::warningTypesForRuleAndSourceDomain(int id, QString name) const
{
	//Fetch the rule and domain
	AbstractRulePtr abs=current().findAbstractRule(id);
	RulePtr rule=abs.dynamicCast<Rule>();
	const Domain &dom=current().getDomainByName(name);

	//If one of them is invalid, abort
	if(abs.isNull() || dom==Domain())
		return InvalidType;

	//Location where to store the result
	int result=InvalidType;

	//Merge all rule warnings for the ID
	QVector<WarningPtr> vect;
	if(_rule_warnings.contains(id))
		vect << _rule_warnings[id];

	if(_od_rule_warnings.contains(id))
		vect << _od_rule_warnings[id];

	//Go through the found warnings and only get the warning type for those involving the wanted source domain
	for(QVector<WarningPtr>::const_iterator it=vect.begin(); it!=vect.end(); ++it)
	{
		const WarningPtr &warn=*it;

		//If the warning concerns a bypass from our source domain, update the result
		if(warn->warningType() & Bypassed)
		{
			QSharedPointer<BypassRuleWarning> ptr=warn.dynamicCast<BypassRuleWarning>();
			if(!ptr.isNull() && (dom==ptr->source()))
				result|=Bypassed;

		}
		//Check the confidentiality of our source and the rule's target
		else if((warn->warningType() & ConfidentialityLoss) && !rule.isNull())
		{
			if(dom.confidentiality()>rule->transitTo().confidentiality())
				result|=ConfidentialityLoss;
		}
		//Check the reliability of our source and the rule's target
		else if((warn->warningType() & ReliabilityLoss) && !rule.isNull())
		{
			if(dom.reliability()>rule->transitTo().reliability())
				result|=ReliabilityLoss;
		}
		else
			result|=warn->warningType();
	}

	return result;
}

QVector<WarningPtr> ConfigurationStack::warningsForRule(int id) const
{
	QVector<WarningPtr> result;

	if(_rule_warnings.contains(id))
		result << _rule_warnings[id];

	if(_od_rule_warnings.contains(id))
		result << _od_rule_warnings[id];

	return result;
}

bool ConfigurationStack::ruleHasWarnings(int id) const
{
	return _rule_warnings.contains(id) || _od_rule_warnings.contains(id);
}

int ConfigurationStack::warningTypesForDomain(QString name) const
{
	int result=InvalidType;

	if(_dom_warnings.contains(name))
		for(QVector<WarningPtr>::const_iterator it=_dom_warnings[name].begin(); it!=_dom_warnings[name].end(); ++it)
			result|=(*it)->warningType();

	if(_od_dom_warnings.contains(name))
		for(QVector<WarningPtr>::const_iterator it=_od_dom_warnings[name].begin(); it!=_od_dom_warnings[name].end(); ++it)
			result|=(*it)->warningType();

	return result;
}

QVector<WarningPtr> ConfigurationStack::warningsForDomain(QString name) const
{
	QVector<WarningPtr> result;

	if(_dom_warnings.contains(name))
		result << _dom_warnings[name];

	if(_od_dom_warnings.contains(name))
		result << _od_dom_warnings[name];

	return result;
}

bool ConfigurationStack::domainHasWarnings(QString name) const
{
	return _dom_warnings.contains(name) || _od_dom_warnings.contains(name);
}

QVector<WarningPtr> ConfigurationStack::allWarnings() const
{
	QVector<WarningPtr> result;

	for(QMap<int, QVector<WarningPtr> >::const_iterator it=_rule_warnings.begin(); it!=_rule_warnings.end(); ++it)
		result << it.value();

	for(QMap<int, QVector<WarningPtr> >::const_iterator it=_od_rule_warnings.begin(); it!=_od_rule_warnings.end(); ++it)
		result << it.value();

	for(QMap<QString, QVector<WarningPtr> >::const_iterator it=_dom_warnings.begin(); it!=_dom_warnings.end(); ++it)
		result << it.value();

	for(QMap<QString, QVector<WarningPtr> >::const_iterator it=_od_dom_warnings.begin(); it!=_od_dom_warnings.end(); ++it)
		result << it.value();

	return result;
}

bool ConfigurationStack::canUndo() const
{
	return _current_index>0;
}

bool ConfigurationStack::canRedo() const
{
	return (_current_index+1)<_stack.size();
}

void ConfigurationStack::addHiddenDomain(QString name)
{
	_hidden_domains.append(name);
	emit hiddenDomainChanged(name, true);
}

void ConfigurationStack::addHiddenProgram(QString name)
{
	_hidden_programs.append(name);
	emit hiddenProgramChanged(name, true);
}

void ConfigurationStack::addHiddenRule(int id)
{
	_hidden_rules.append(id);
	emit hiddenRuleChanged(id, true);
}

void ConfigurationStack::removeHiddenDomain(QString name)
{
	_hidden_domains.removeAll(name);
	emit hiddenDomainChanged(name, false);
}

void ConfigurationStack::removeHiddenProgram(QString name)
{
	_hidden_programs.removeAll(name);
	emit hiddenProgramChanged(name, false);
}

void ConfigurationStack::removeHiddenRule(int id)
{
	_hidden_rules.removeAll(id);
	emit hiddenRuleChanged(id, false);
}

bool ConfigurationStack::isDomainHidden(QString name) const
{
	return _hidden_domains.contains(name);
}

bool ConfigurationStack::isProgramHidden(QString name) const
{
	return _hidden_programs.contains(name);
}

bool ConfigurationStack::isRuleHidden(int id) const
{
	return _hidden_rules.contains(id);
}

QStringList ConfigurationStack::hiddenDomains() const
{
	return _hidden_domains;
}

QStringList ConfigurationStack::hiddenPrograms() const
{
	return _hidden_programs;
}

QList<int> ConfigurationStack::hiddenRules() const
{
	return _hidden_rules;
}

void ConfigurationStack::pushCurrentInstance(QString changeType)
{
	Configuration *conf=new Configuration(_current_before_last_change);

	//The changeType of current() represents the change with the previous instance, so it
	//will be given to the non-changed copy of current(), and current() will be given the
	//changeType between it's copy and itself
	QString oldChangeType=_stack[_current_index].second;
	_stack[_current_index].second=changeType;

	//Insert the copy of the current instance before the one already stored
	_stack.insert(_current_index++, QPair<Configuration *, QString>(conf, oldChangeType));

	//Remove all the data after the current instance since the redo stack is now obsolete
	_stack.remove((_current_index+1), _stack.size()-(_current_index+1));

	//If the stack is getting over a certain size, remove an old element from it
	if(_stack.size() > EditorConfig::maxStackSize())
	{
		_stack.remove(0);
		_current_index--;
		_last_saved_index--;
	}

	emit stackChanged();
}

void ConfigurationStack::initialise(bool reload_programs)
{
	//Clear all internal data
	_hidden_domains.clear();
	_hidden_programs.clear();
	_hidden_rules.clear();
	clearARPCache();
	clearOnDemandWarnings();

	//Create a new Configuration with the transitions file and existing programs
	QList<Program> programs;

	QMap<QString, QPair<Program, QString> >::iterator it;
	for(it=_programs.begin(); it!=_programs.end(); ++it)
		programs.append(it.value().first);

	Configuration *conf=new Configuration(_file_path, false, programs);

	//Clear and setup the Configuration stack
	for(int i=0; i<_stack.size(); ++i)
		delete _stack[i].first;
	_stack.clear();
	_stack.reserve(100);
	_stack.append(QPair<Configuration *, QString>(conf, CHANGE_TYPE_NEW));

	//Initialise the current conf index
	_current_index=0;
	_last_saved_index=0;
	_current_before_last_change=*conf;

	//Reload programs from the settings if needed, before connecting signals to avoid some spam
	if(reload_programs)
		addProgramsFromFiles(QSettings().value("knownApplications").toStringList(), true);

	connect(conf, SIGNAL(domainChanged(QString,QString)), this, SLOT(onDomainChanged(QString,QString)));
	connect(conf, SIGNAL(configurationChanged()), this, SLOT(onConfigurationChanged()));
	connect(conf, SIGNAL(programChanged(QString,QString)), this, SLOT(onProgramChanged(QString,QString)));
	connect(conf, SIGNAL(allRulesChanged(bool)), this, SLOT(onAllRulesChanged(bool)));
	connect(conf, SIGNAL(loopsChanged(bool)), this, SLOT(onLoopsChanged(bool)));
	connect(conf, SIGNAL(rulesChanged(bool)), this, SLOT(onRulesChanged(bool)));

	onUpdateWarnings();
	emit stackChanged();
}

void ConfigurationStack::reinitialise(QString new_path)
{
	//Change the path
	_file_path=new_path;

	//Call the initialisation procedure
	initialise();

	//Emit signals to let Editor know about the change
	emit configurationChanged();
	emit newConfiguration();
}

bool ConfigurationStack::saveTransitions(QString path)
{	
	QFile file(path);

	//Open the file in write mode
	if(!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::critical(_parent, "Error",
									"<b>Could not open file " + path +
									".</b><br /><br />Please check that you have the sufficient rights to write in this directory.",
									QMessageBox::Ok);
		return false;
	}

	//Write the data
	if(!file.write(qPrintable(current().retrieveTransitionsDOM())))
	{
		QMessageBox::critical(_parent, "Error",
									"<b>Could not write data to " + path +
									".</b><br /><br />This may happen if there is no space left on your hard drive.",
									QMessageBox::Ok);
		return false;
	}
	else
	{
		_file_path=path;
		_last_saved_index=_current_index;
		emit configurationSaved();
		return true;
	}
}

bool ConfigurationStack::transitionsFileWellFormed(QString path)
{
	QFile file(path);
	QDomDocument doc;

	if(!file.open(QIODevice::ReadOnly))
	{
		qWarning("Stack syntax checker: Can not open file '%s'.", qPrintable(path));
		return false;
	}
	if(!doc.setContent(&file))
	{
		file.close();
		qWarning("Stack syntax checker: Not syntaxically valid, can not parse transitions file.");
		return false;
	}

	QDomElement root=doc.firstChildElement(TRANSITIONS_XML_ROOT);
	if(root.isNull())
	{
		qWarning("Stack syntax checker: Doesn't contain root element.");
		return false;
	}

	//Check that all children are valid
//	QDomElement element=root.firstChildElement();
//	while(!element.isNull())
//	{
//		QStringList goodTags;
//		goodTags.append(DOMAIN_TAG);
//		goodTags.append(NOKILL_ROOT_TAG);
//		goodTags.append(RULE_TAG);
//		goodTags.append(LOOP_TAG);

//		if(!goodTags.contains(element.tagName()))
//		{
//			qWarning("Stack syntax checker: Contains invalid tag %s.", qPrintable(element.tagName()));
//			return false;
//		}
//		element=element.nextSiblingElement();
//	}

	return true;
}

void ConfigurationStack::updateHiddenDomain(QString old_name, QString new_name)
{
	if(_hidden_domains.contains(old_name))
	{
		_hidden_domains.removeAll(old_name);

		if(new_name!=QString())
			_hidden_domains.append(new_name);
	}
}

void ConfigurationStack::updateHiddenProgram(QString old_name, QString new_name)
{
	if(_hidden_programs.contains(old_name))
	{
		_hidden_programs.removeAll(old_name);

		if(new_name!=QString())
			_hidden_programs.append(new_name);
	}
}

void ConfigurationStack::emitSignalForChangeType(QString changeType, bool revert_changes)
{
	clearARPCache();
	clearOnDemandWarnings();
	onUpdateWarnings();

	if(changeType==CHANGE_TYPE_CONFIGURATION)
		emit configurationChanged();
	else if(changeType==CHANGE_TYPE_ALL_RULES)
		emit allRulesChanged();
	else if(changeType==CHANGE_TYPE_LOOPS)
		emit loopsChanged();
	else if(changeType==CHANGE_TYPE_RULES)
		emit rulesChanged();
	else if(changeType.startsWith(CHANGE_TYPE_DOMAIN))
	{
		QStringList data=changeType.split("\n");
		if(data.size()>=3)
		{
			if(!revert_changes)
			{
				updateHiddenDomain(data.at(1), data.at(2));
				emit domainChanged(data.at(1), data.at(2));
			}
			else
			{
				updateHiddenDomain(data.at(2), data.at(1));
				emit domainChanged(data.at(2), data.at(1));
			}
		}
	}
}

void ConfigurationStack::clearARPCache()
{
	_arp_cache.clear();
}

void ConfigurationStack::onUpdateWarnings()
{
	//Clear existing warnings
	_rule_warnings.clear();
	_dom_warnings.clear();

	//A map used to get rules per source domain, for each domain
	QMap<QString, QVector<AbstractRulePtr> > sourceDomains;

	//Go through each rule and fill the source domains map
	QVector<AbstractRulePtr> abstracts=current().allAbstractRules();
	for(int i=0; i<abstracts.size(); ++i)
	{
		AbstractRulePtr abs=abstracts[i];

		QStringList sources=abs->matchedDomainNames(current().domainNameList());
		for(int j=0; j<sources.size(); ++j)
			sourceDomains[sources[j]].append(abs);
	}


	//A set in which domains are added only if they are reached in the transition graph
	QSet<QString> reachedDomains;
	Domain defDom=current().defaultDomain();

	//Starting from default, and using source domains, find all reached domains
	QStringList toScan;
	if(defDom!=Domain())
	{
		reachedDomains.insert(defDom.name());
		toScan << defDom.name();
	}

	while(!toScan.empty())
	{
		//Get the next domain to scan
		QString name=toScan.takeFirst();

		//If there are transitions from this domain, fill up toScan with the reached domains
		if(sourceDomains.contains(name))
		{
			QVector<AbstractRulePtr> sourceAbs=sourceDomains[name];
			for(int i=0; i<sourceAbs.size(); ++i)
			{
				AbstractRulePtr abs=sourceAbs[i];

				//There is a new target domain only if the rule is a Rule
				//If the target domain has not been reached yet, add it to the toScan list
				RulePtr rule=abs.dynamicCast<Rule>();
				if(!rule.isNull() && !reachedDomains.contains(rule->transitTo().name()))
				{
					toScan << rule->transitTo().name();
					reachedDomains.insert(rule->transitTo().name());
				}
			}
		}
	}

	//Find the never reached domains by comparing reachedDomains and current().domainNameList
	if(EditorConfig::unreachableDomainWarn())
	{
		QStringList unreachedDomains=current().domainNameList();
		for(QSet<QString>::const_iterator it=reachedDomains.begin(); it!=reachedDomains.end(); ++it)
			unreachedDomains.removeAll(*it);

		for(int i=0; i<unreachedDomains.size(); ++i)
		{
			QString name=unreachedDomains.at(i);
			_dom_warnings[name].append(WarningPtr(new UnreachableDomainWarning(name)));
		}
	}

	//Find the never reached rules by substracting the reached ones from abstracts
	if(EditorConfig::unreachableRuleWarn())
	{
		QList<AbstractRulePtr> unreachedRules=abstracts.toList();
		for(QSet<QString>::const_iterator it=reachedDomains.begin(); it!=reachedDomains.end(); ++it)
		{
			QVector<AbstractRulePtr> sourceAbs=sourceDomains[*it];
			for(int j=0; j<sourceAbs.size(); ++j)
				unreachedRules.removeAll(sourceAbs[j]);
		}

		for(int i=0; i<unreachedRules.size(); ++i)
		{
			int id=unreachedRules.at(i)->id();

			_rule_warnings[id].append(WarningPtr(new UnreachableRuleWarning(id)));
		}
	}

	//Look for bypassed transition rules
	//TODO this might be moved to ODRules, need to check the time it takes to calculate
	if(EditorConfig::bypassedRuleWarn())
	{
		QTime tm;
		tm.start();
		for(QMap<QString, QVector<AbstractRulePtr> >::iterator it=sourceDomains.begin(); it!=sourceDomains.end(); ++it)
		{
			QVector<AbstractRulePtr> vect=it.value();
			qSort(vect.begin(), vect.end(), AbstractRule::hasHigherPriority);

			//Starting from the less important rule, check for bypassing rules
			for(int i=vect.size()-1; i>=0; --i)
			{
				AbstractRulePtr abs=vect[i];

				for(int j=i-1; j>=0; --j)
				{
					AbstractRulePtr other=vect[j];

					//Get the target domain of both transitions
					QString target;
					QString otherTarget;

					RulePtr rule=abs.dynamicCast<Rule>();
					if(rule.isNull())
						target=it.key();
					else
						target=rule->transitTo().name();

					rule=other.dynamicCast<Rule>();
					if(rule.isNull())
						otherTarget=it.key();
					else
						otherTarget=rule->transitTo().name();

					//If both transitions have the same app and target domain they might be conflicting
					if(abs->appName()==other->appName() && target==otherTarget)
					{

						QVector<MRule> mrules=abs->mRules();
						QVector<MRule>otherMrules=other->mRules();
						QMap<QString, QList<int> > bypassed;

						//Go through each MRule of abs and check for mrules with the same action in other
						for(int k=0; k<mrules.size(); ++k)
						{
							for(int l=0; l<otherMrules.size(); ++l)
							{
								if(otherMrules[l].actionName()==mrules[k].actionName())
								{
									//Check if both mrules concern the same variables
									QMap<QString, QString> conds=mrules[k].conditions();
									QMap<QString, QString> otherConds=otherMrules[l].conditions();
									QMap<QString, QString>::const_iterator condIt;

									bool bypass=true;

									for(condIt=conds.begin(); condIt!=conds.end() && bypass; ++condIt)
									{
										if(!otherConds.contains(condIt.key()))
											bypass=false;
										//TODO value checking
									}

									if(bypass && !bypassed[mrules[k].displayName()].contains(other->id()))
										bypassed[mrules[k].displayName()].append(other->id());
								}
							}
						}

						if(bypassed.size()!=0)
							_rule_warnings[abs->id()].append(WarningPtr(
									new BypassRuleWarning(abs->id(), current().getDomainByName(it.key()), bypassed)));
					}
				}
			}
			//TODO need a RegExp comparison system for bypassing rules
		}

		qDebug("Time elapsed on bypass search: %d msecs", tm.elapsed());
	}

	//Look for unknown programs / wrong actions/variables / less reliable/confidential targets
	for(int i=0; i<abstracts.size(); ++i)
	{
		AbstractRulePtr abs=abstracts[i];
		const Program &prog=current().getProgramByName(abs->appName());

		//Check if the program is valid
		if(!prog.isValid())
		{
			if(EditorConfig::unknownProgramWarn())
				_rule_warnings[abs->id()].append(WarningPtr(new UnknownProgramRuleWarning(abs->id())));
		}
		//Else check the existence/validity of actions/variables
		else
		{
			const QVector<MRule> &mrules=abs->mRules();
			bool exists=true;
			bool valid=true;
			bool checkExists=EditorConfig::undefinedActionWarn();
			bool checkValid=EditorConfig::invalidMRuleWarn();

			//Check it for each matching rule
			for(int j=0; j<mrules.size(); ++j)
			{
				const MRule &mrule=mrules[j];
				const ProgramAction &act=prog.getActionByName(mrule.actionName());

				//If the mrule's action doesn't exist, set the exists flag to false
				if(checkExists && act==ProgramAction())
				{
					exists=false;
				}
				//Else check for the integrity of the data (only if another mrule hasn't been marked invalid before)
				else if(checkValid && valid)
				{
					const QMap<QString, QString> &conds=mrule.conditions();
					for(QMap<QString, QString>::const_iterator it=conds.begin(); it!=conds.end() && valid; ++it)
					{
						const ProgramVariable &var=prog.getVariableByName(it.key(), mrule.actionName());

						//Invalid variable
						if(var==ProgramVariable() || !var.validate(it.value()))
							valid=false;
					}
				}
			}

			//If there was one non-existing action, created an UndefinedMRule warning
			if(!exists)
				_rule_warnings[abs->id()].append(WarningPtr(new UndefinedMRuleRuleWarning(abs->id())));

			//If there was one invalid action/variable, created an InvalidMRule warning
			if(!valid)
				_rule_warnings[abs->id()].append(WarningPtr(new InvalidMRuleRuleWarning(abs->id())));
		}

		//If the rule is a Rule, there might be confidentiality/reliability decreases
		RulePtr rule=abs.dynamicCast<Rule>();
		if(!rule.isNull())
		{
			QVector<Domain> domains=rule->matchedDomains(current().domainList());
			bool confidential=true;
			bool reliable=true;

			//Check each source domain's confidentiality/reliability
			for(QVector<Domain>::const_iterator it=domains.begin(); it!=domains.end() && (confidential||reliable); ++it)
			{
				const Domain &dom=*it;

				confidential=dom.confidentiality() <= rule->transitTo().confidentiality();
				reliable=dom.reliability() <= rule->transitTo().reliability();
			}

			//If one source domain was more confidential
			if(!confidential && EditorConfig::confidentialityLossRuleWarn())
				_rule_warnings[abs->id()].append(WarningPtr(new ConfidentialityLossRuleWarning(abs->id())));

			//If one source domain was more reliable
			if(!reliable && EditorConfig::reliabilityLossRuleWarn())
				_rule_warnings[abs->id()].append(WarningPtr(new ReliabilityLossRuleWarning(abs->id())));
		}
	}

	emit warningsChanged();
}

void ConfigurationStack::onUpdateOnDemandWarnings()
{
	_od_rule_warnings.clear();
	_od_dom_warnings.clear();

	//TODO

	emit warningsChanged();
}

void ConfigurationStack::clearOnDemandWarnings()
{
	_od_rule_warnings.clear();
	_od_dom_warnings.clear();
	emit warningsChanged();
}

void ConfigurationStack::onDomainChanged(QString old_name, QString new_name)
{
	clearARPCache();
	clearOnDemandWarnings();
	onUpdateWarnings();

	updateHiddenDomain(old_name, new_name);
	pushCurrentInstance(CHANGE_TYPE_DOMAIN"\n"+old_name+"\n"+new_name);
	emit domainChanged(old_name, new_name);
}

void ConfigurationStack::onConfigurationChanged()
{
	clearARPCache();
	clearOnDemandWarnings();
	onUpdateWarnings();

	//TODO sanity check of hidden stuff, if fails, clear the 3
	pushCurrentInstance(CHANGE_TYPE_CONFIGURATION);
	emit configurationChanged();
}

void ConfigurationStack::onProgramChanged(QString old_name, QString new_name)
{
	clearARPCache();
	clearOnDemandWarnings();
	onUpdateWarnings();

	updateHiddenProgram(old_name, new_name);
	emit programChanged(old_name, new_name);
}

void ConfigurationStack::onAllRulesChanged(bool side_effect)
{
	clearARPCache();
	clearOnDemandWarnings();
	onUpdateWarnings();
	if(!side_effect)
		pushCurrentInstance(CHANGE_TYPE_ALL_RULES);
	emit allRulesChanged();
}

void ConfigurationStack::onLoopsChanged(bool side_effect)
{
	clearARPCache();
	clearOnDemandWarnings();
	onUpdateWarnings();
	if(!side_effect)
		pushCurrentInstance(CHANGE_TYPE_LOOPS);
	emit loopsChanged();
}

void ConfigurationStack::onRulesChanged(bool side_effect)
{
	clearARPCache();
	clearOnDemandWarnings();
	onUpdateWarnings();
	if(!side_effect)
		pushCurrentInstance(CHANGE_TYPE_RULES);
	emit rulesChanged();
}

void ConfigurationStack::onSaveToTmp()
{
	QDir("/").mkpath(QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/autosave/");
	QFile file(QDesktopServices::storageLocation(QDesktopServices::DataLocation)+QString("/autosave/%1.xml").arg(getpid()));

	if(file.open(QIODevice::WriteOnly))
		file.write(qPrintable(current().retrieveTransitionsDOM()));
}

void ConfigurationStack::onNew()
{
	bool performAction=true;

	if(hasChanges())
		performAction=proposeSaveTransitions();

	if(performAction)
		reinitialise();
}

void ConfigurationStack::onOpen()
{
	bool performAction=true;

	if(hasChanges())
		performAction=proposeSaveTransitions();

	if(performAction)
	{
		//Ask the user the path to the transitions file
		QString path=QFileDialog::getOpenFileName(_parent, "Open transitions...", EditorConfig::lastOpenedPath(), "All files (*);;XML files (*.xml)");

		if(path==QString())
			return;
		else
			EditorConfig::setLastOpenedPath(path);

		//Check that the path exists before trying to open the file
		if (!QFile::exists(path))
		{
			QMessageBox::critical(_parent, "Error",
								  "<b>Could not open file " + path +
								  ".</b><br /><br />The file could not be found.",
								  QMessageBox::Ok);
			return;
		}

		//Check the file syntax before reinitialising
		if(transitionsFileWellFormed(path))
			reinitialise(path);
		else
		{
			QMessageBox::critical(_parent, "Error",
								  "<b>Could not open file " + path +
								  ".</b><br /><br />The file could not be read or is corrupted.",
								  QMessageBox::Ok);
		}
	}
}

void ConfigurationStack::onSave()
{
	if(_file_path==QString())
		onSaveAs();
	else
		saveTransitions(_file_path);
}

void ConfigurationStack::onSaveAs()
{
	QString dir=_file_path==QString() ? EditorConfig::lastSavedPath():_file_path;
	QString path=QFileDialog::getSaveFileName(_parent, "Save transitions as...", dir, "All files (*);;XML files (*.xml)");

	if (path==QString())
		return;
	else
		EditorConfig::setLastSavedPath(path);

	saveTransitions(path);
}

void ConfigurationStack::onUndo()
{
	//Disconnect the old Configuration instance
	const Configuration &old=current();
	disconnect(&old, 0, this, 0);

	//Get the change type so that we can emit the matching signal
	//We get the changeType of the old instance, so that we restore the change
	QString changeType=_stack[_current_index].second;

	//Change the index
	if(_current_index>0)
		_current_index--;

	//Connect the new instance
	const Configuration &conf=current();
	connect(&conf, SIGNAL(domainChanged(QString,QString)), this, SLOT(onDomainChanged(QString,QString)));
	connect(&conf, SIGNAL(configurationChanged()), this, SLOT(onConfigurationChanged()));
	connect(&conf, SIGNAL(programChanged(QString,QString)), this, SLOT(onProgramChanged(QString,QString)));
	connect(&conf, SIGNAL(allRulesChanged(bool)), this, SLOT(onAllRulesChanged(bool)));
	connect(&conf, SIGNAL(loopsChanged(bool)), this, SLOT(onLoopsChanged(bool)));
	connect(&conf, SIGNAL(rulesChanged(bool)), this, SLOT(onRulesChanged(bool)));

	//Emit a signal to let the app update itself, and another to say the stack's current instance has changed
	emitSignalForChangeType(changeType, true);
	emit stackChanged();
}

void ConfigurationStack::onRedo()
{
	//Disconnect the old Configuration instance
	const Configuration &old=current();
	disconnect(&old, 0, this, 0);

	//Change the index
	if(_current_index+1<_stack.size())
		_current_index++;

	//Get the change type so that we can emit the matching signal
	//We fetch the changeType of the new instance, so that we reapply the change
	QString changeType=_stack[_current_index].second;

	//Connect the new instance
	const Configuration &conf=current();
	connect(&conf, SIGNAL(domainChanged(QString,QString)), this, SLOT(onDomainChanged(QString,QString)));
	connect(&conf, SIGNAL(configurationChanged()), this, SLOT(onConfigurationChanged()));
	connect(&conf, SIGNAL(programChanged(QString,QString)), this, SLOT(onProgramChanged(QString,QString)));
	connect(&conf, SIGNAL(allRulesChanged(bool)), this, SLOT(onAllRulesChanged(bool)));
	connect(&conf, SIGNAL(loopsChanged(bool)), this, SLOT(onLoopsChanged(bool)));
	connect(&conf, SIGNAL(rulesChanged(bool)), this, SLOT(onRulesChanged(bool)));

	//Emit a signal to let the app update itself, and another to say the stack's current instance has changed
	emitSignalForChangeType(changeType);
	emit stackChanged();
}

void ConfigurationStack::onSettingsChange()
{
	int freq=EditorConfig::autosaveFrequency();

	if(freq!=0)
		_autosave_timer.start(freq*60000);
	else
		_autosave_timer.stop();
}
