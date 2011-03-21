#ifndef PROGRAM_H
#define PROGRAM_H

/**
 * \file program.h
 * \author Martin Peres (martin dot peres at ensi-bourges dot fr)
 * \date 2010-06-07
 */

#include "programaction.h"
#include "programvariable.h"

#include <QString>
#include <QStringList>
#include <QMap>
#include <QtXml/QDomDocument>

#include <QFile>

/// This class represents a program that interacts with contextd
class Program
{
private:
	QString _app_name, _display_name, _icon_path, _full_path, _description;

	QMap<QString, ProgramAction> _actions;
	QMap<QString, QMap<QString, ProgramVariable> > _variables;
	QMultiMap<QString, ProgramVariable> _var_map;

public:
	Program();

	/*!
	 * \brief Construct a program from an xml file.
	 * \param file: The file to get the program from.
	 */
	Program(QFile& file);

	/*!
	 * \brief Construct a program.
	 * \param app_name The name of the application
	 * \param display_name The name of the application, meant to be displayed
	 * \param icon The filepath to an icon representing the application
	 * \param full_path The full path of the executable represented by this class
	 * \param description The description of the application (can be long text)
	 */
	Program(QString app_name, QString display_name, QString icon, QString full_path, QString description);

	/// Tells if the program has a valid name and path
	bool isValid() const;

	/// The name of the application
	QString name() const;

	/// The name of the application, meant to be displayed
	QString displayName() const;

	/// The filepath to an icon representing the application
	QString icon() const;

	/// The full path of the executable represented by this class
	QString fullPath() const;

	/// The description of the application (long text)
	QString description() const;

	/// The actions exported by the program. For instance, claws mail would support "mail_open" and "connect_server" actions.
	QList<ProgramAction> actions() const;

	/// The names of the actions exported by the program.
	QList<QString> actionNames() const;

	/*! The list of the variables exported by the program. If action is empty, it will return all the variables exported
	 * by the program. If action is set to a valid action, it will only return the variables of the specified action.
	 * \param action The action you would like to get the variables from. Set to QString() if you want to get the complete list
	 */
	QList<ProgramVariable> variables(const QString& action=QString()) const;

	/// The names of the variables exported by the program. See variables() for parameter usage.
	QList<QString> variableNames(const QString& action=QString()) const;

	/*! \brief Get an action by its name.
	 * \param name The name of the action you want to get.
	 */
	ProgramAction getActionByName(const QString& name) const;

	/*! \brief Get a variable by its name.
	 * \param name The name of the variable you want to get.
	 * \param action The name of the action for this variable.
	 */
	ProgramVariable getVariableByName(const QString& name, const QString& action=QString()) const;

	/// Change the actions and variables exported by the program.
	void setActionsAndVariables(const QMap<QString, ProgramAction>& actions, const QList<ProgramVariable>& variables);

	/// Retrieve the DOM document for this program
	QString retrieveDOM() const;

	bool operator==(const Program& b) const;
	bool operator!=(const Program& b) const;
};

#endif // PROGRAM_H
