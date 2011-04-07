#ifndef MRULE_H
#define MRULE_H

/**
 * \file mrule.h
 * \author Martin Peres (martin dot peres at ensi-bourges dot fr)
 * \date 2010-06-07
 */

#include <QMap>
#include <QString>

#include "domain.h"
#include "program.h"

/// A block of conditions. It is the object representation of the <mrule> tag in transitions.xml
class MRule
{
	QMap<QString, QString> _conditions;
	Program _prog;
	QString _action;
	QString _display_name;

public:
	MRule(Program prog=Program(), QString action=QString(), QString display_name=QString());

	/*!
	 * \brief Add a condition to the mrule.
	 * \param name The name of the variable
	 * \param value A regular expression that should be matched by the variable "name".
	 * \return true if the value has been added correctly, false otherwise.
	 */
	bool addCondition(QString name, QString value);

	/*!
	 * \brief Does this matching rule matches this set of variable.
	 * \param values The set of variable (QMap<name, value>).
	 * \param value A regular expression that should be matched by the variable "name".
	 * \return true if this mrule matches, false otherwise.
	 */
	bool matches(QMap<QString, QString> values) const;

	/*!
	 * \brief Sets the action name of the variables of the matching rule
	 * \param action_name The name for the action
	 */
	void setActionName(QString action_name);

	/*!
	 * \brief Sets the display name of the matching rule
	 * \param name The display name
	 */
	void setDisplayName(QString name);

	QMap<QString, QString> conditions() const;
	QString actionName() const;
	QString displayName() const;

	/*bool operator==(const MRule& b) const;
	bool operator!=(const MRule& b) const;*/
};

#endif // TRANSITION_H
