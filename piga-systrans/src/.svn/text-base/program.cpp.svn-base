#include "program.h"
#include "xml_common.h"

#include <QSet>
#include <QtXml/QDomDocument>
#include <QFileInfo>
#include "variabletypes.h"

Program::Program()
{
}

Program::Program(QFile& file)
{
	//Open file
	if(file.open(QIODevice::ReadOnly))
	{
		//Create DOM document out of file
		QDomDocument doc;
		if(!doc.setContent(&file))
		{
			file.close();
			qWarning("Program: Error while parsing the file '%s'", qPrintable(file.fileName()));
			return;
		}

		//Make sure the root tag is a program
		QDomElement root=doc.firstChildElement(PROGRAM_TAG);
		if(root.isNull())
		{
			qWarning("Program: XML file is empty.");
			return;
		}

		//Check for all the mandatory data in the Program
		QDomElement vars=root.firstChildElement(VARIABLES_TAG);
		QDomElement actions=root.firstChildElement(ACTIONS_TAG);
		if(vars.isNull() || actions.isNull() || (root.attribute(NAME_TAG)==QString()))
		{
			qWarning("Program: XML file does not contain all the required tags.");
			return;
		}

		//Fetch actions
		QDomElement action=actions.firstChildElement(ACTION_TAG);
		while(!action.isNull())
		{
			QString name=action.attribute(NAME_TAG);
			QString displayName=action.attribute(DISPLAY_NAME_TAG);
			QString desc=action.attribute(DESCRIPTION_TAG);

			if(name!=QString())
			{
				if(!_actions.contains(name))
					_actions[name]=ProgramAction(name, displayName, desc);
				else
					qWarning("Program: Duplicate action found (%s)", qPrintable(name));
			}
			else
				qWarning("Program: Invalid action found");

			action=action.nextSiblingElement(ACTION_TAG);
		}

		//Fetch variables
		QDomElement var=vars.firstChildElement(VARIABLE_TAG);
		while(!var.isNull())
		{
			QString name=var.attribute(NAME_TAG);
			QString action=var.attribute(ACTION_TAG);
			QString desc=var.attribute(DESCRIPTION_TAG);
			QString type=var.attribute(VARIABLE_TYPE_TAG);
			QString validation=var.attribute(VARIABLE_STRING_VALIDATION_TAG);
			QString range=var.attribute(VARIABLE_INT_RANGE_TAG);
			QString listStr=var.attribute(VARIABLE_STRING_LIST_TAG);

			if(name!=QString() && _actions.contains(action))
			{
				if(!_variables[action].contains(name))
				{
					ProgramVariable variable(name, action, desc);

					if(type!=QString())
						variable.setType(type);
					if(validation!=QString())
						variable.setStringValidation(validation);
					if(range!=QString())
						variable.setIntRangeFromString(range);
					if(listStr!=QString())
						variable.setStringListFromString(listStr);

					_variables[action][name]=variable;
					_var_map.insert(name, variable);
				}
				else
					qWarning("Program: Duplicate variable found (%s)", qPrintable(name));
			}
			else
				qWarning("Program: Invalid variable found");

			var=var.nextSiblingElement(VARIABLE_TAG);
		}

		//Fetch attributes of the program
		_app_name=root.attribute(NAME_TAG);
		_display_name=root.attribute(DISPLAY_NAME_TAG);
		_icon_path=root.attribute(ICON_TAG);
		_full_path=root.attribute(FULLPATH_TAG);

		//Fetch description of the program
		QDomElement desc=root.firstChildElement(DESCRIPTION_TAG);
		if(!desc.isNull())
			_description=desc.text();
	}
	else
		qWarning("Program: The file '%s' cannot be opened (%s)", qPrintable(file.fileName()), qPrintable(file.errorString()));
}

Program::Program(QString app_name, QString display_name, QString icon, QString full_path, QString description) :
		_app_name(app_name),
		_display_name(display_name),
		_icon_path(icon), _full_path(full_path),
		_description(description)
{
}

bool Program::isValid() const
{
	return _app_name!=QString() && QFileInfo(_full_path).isAbsolute();
}

QString Program::name() const
{
	return _app_name;
}

QString Program::displayName() const
{
	return _display_name;
}

QString Program::icon() const
{
	return _icon_path;
}

QString Program::fullPath() const
{
	return _full_path;
}


QString Program::description() const
{
	return _description;
}

QList<ProgramAction> Program::actions() const
{
	return _actions.values();
}

void Program::setActionsAndVariables(const QMap<QString, ProgramAction>& actions, const QList<ProgramVariable>& variables)
{
	//Remove previous actions and variables
	_variables.clear();
	_var_map.clear();
	_actions.clear();

	//Add each action to the map
	_actions=actions;

	//Add each variable to its containers
	QList<ProgramVariable>::const_iterator varIt;
	for(varIt=variables.begin(); varIt<variables.end(); ++varIt)
	{
		const ProgramVariable& var=*varIt;
		_var_map.insert(var.name(), var);

		if(_variables.contains(var.action()))
			_variables[var.action()][var.name()]=var;
		else
		{
			QMap<QString, ProgramVariable> map;
			map[var.name()]=var;
			_variables[var.action()]=map;
		}
	}
}

QList<QString> Program::actionNames() const
{
	return _actions.keys();
}

QList<ProgramVariable> Program::variables(const QString& action) const
{
	if(_actions.contains(action))
		return _variables[action].values();
	else
		return _var_map.values();
}

QList<QString> Program::variableNames(const QString& action) const
{
	if(_actions.contains(action))
		return _variables[action].keys();
	else
		return _var_map.keys();
}

ProgramAction Program::getActionByName(const QString& name) const
{
	return _actions[name];
}

ProgramVariable Program::getVariableByName(const QString &name, const QString &action) const
{
	QList<ProgramVariable> vars=_var_map.values(name);

	if(vars.size()==0)
		return ProgramVariable();


	if(action!=QString())
	{
		for(int i=0; i<vars.size(); ++i)
			if(vars.at(i).action()==action)
				return vars.at(i);

		return ProgramVariable();
	}
	else
		return vars.first();
}

QString Program::retrieveDOM() const
{
	QDomDocument doc;

	//Create DOM tree root
	QDomElement root=doc.createElement(PROGRAM_TAG);
	doc.appendChild(root);

	//Set parameters
	root.setAttribute(NAME_TAG, _app_name);
	root.setAttribute(DISPLAY_NAME_TAG, _display_name);
	root.setAttribute(ICON_TAG, _icon_path);
	root.setAttribute(FULLPATH_TAG, _full_path);

	//Add the description
	QDomElement descElem=doc.createElement(DESCRIPTION_TAG);
	root.appendChild(descElem);
	QDomText descText=doc.createTextNode(_description);
	descElem.appendChild(descText);

	//Insert actions
	QDomElement actRoot=doc.createElement(ACTIONS_TAG);
	root.appendChild(actRoot);

	QMap<QString, ProgramAction>::const_iterator actIt;
	for(actIt=_actions.begin(); actIt!=_actions.end(); ++actIt)
	{
		const ProgramAction &act=actIt.value();

		//Create the action element and put it in the tree
		QDomElement actElem=doc.createElement(ACTION_TAG);
		actRoot.appendChild(actElem);

		//Set it's attributes
		actElem.setAttribute(DESCRIPTION_TAG, act.description());
		actElem.setAttribute(DISPLAY_NAME_TAG, act.displayName());
		actElem.setAttribute(NAME_TAG, act.name());
	}

	//Insert variables
	QDomElement varRoot=doc.createElement(VARIABLES_TAG);
	root.appendChild(varRoot);

	QMap<QString, ProgramVariable>::const_iterator varIt;
	for(varIt=_var_map.begin(); varIt!=_var_map.end(); ++varIt)
	{
		const ProgramVariable &var=varIt.value();

		//Create the action element and put it in the tree
		QDomElement varElem=doc.createElement(VARIABLE_TAG);
		varRoot.appendChild(varElem);

		//Set it's attributes
		varElem.setAttribute(DESCRIPTION_TAG, var.description());
		varElem.setAttribute(ACTION_TAG, var.action());
		varElem.setAttribute(NAME_TAG, var.name());
		varElem.setAttribute(VARIABLE_TYPE_TAG, Type::toString(var.type()));

		//Set type-specific attributes
		switch(var.type())
		{
		case Type::RegExp:
		case Type::String:
			varElem.setAttribute(VARIABLE_STRING_VALIDATION_TAG, var.stringValidation());
			break;

		case Type::Int:
			varElem.setAttribute(VARIABLE_INT_RANGE_TAG, var.intRange());
			break;

		case Type::StringList:
			varElem.setAttribute(VARIABLE_STRING_LIST_TAG, var.stringListAsString());
			break;
		}
	}

	return doc.toString();
}

bool Program::operator==(const Program& b) const
{
	return _app_name==b._app_name && _display_name==b._display_name && _icon_path==b._icon_path && _full_path==b._full_path && _description==b._description;
}

bool Program::operator!=(const Program& b) const
{
	return !operator==(b);
}
