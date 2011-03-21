#ifndef PROGRAMVARIABLE_H
#define PROGRAMVARIABLE_H

/**
 * \file programvariable.h
 * \author Martin Peres (martin dot peres at ensi-bourges dot fr)
 * \date 2010-06-07
 */

#include <QMap>
#include <QPair>
#include <QString>
#include <QStringList>
#include "variabletypes.h"

class ProgramVariable
{
private:
	QString _name;
	QString _action;
	QString _description;

	/// Type of the variable
	int _type;

	/// Data used for Int variables
	QString _int_min, _int_max;

	/// Data used for String and RegExp variables
	QString _string_validation;

	/// Data used for StringList variables
	QStringList _string_list_vals;

	/// A map of the values previously assigned to each variable
	static QMap<QPair<QString, int>, QStringList> _assigned_values;

public:
	ProgramVariable();
	ProgramVariable(const QString& name, const QString& action, const QString& description);

	QString name() const;
	void setName(const QString& name);

	QString action() const;
	void setAction(const QString& action);

	QString description() const;
	void setDescription(const QString& desc);

	int type() const;
	void setType(int type);
	bool setType(const QString& type);

	QString intRange() const;
	void setIntRange(int min, int max);
	bool setIntRangeFromString(const QString &range);

	int intMinimum() const;
	void setIntMinimum(int min);

	int intMaximum() const;
	void setIntMaximum(int max);

	QString stringValidation() const;
	void setStringValidation(const QString& validation);

	QString stringListAsString() const;
	QStringList stringList() const;
	void setStringListFromString(const QString& list);
	void setStringList(const QStringList& list);

	bool validate(const QString& value) const;
	bool validate(int value) const;

	bool operator==(const ProgramVariable& b) const;
	bool operator!=(const ProgramVariable& b) const;

	static QStringList assignedValues(const QString& name, const int& type);
	static void addAssignedValue(const QString& name, const int& type, const QString& value);
	static void clearAssignedValues();
};

#endif // PROGRAMVARIABLE_H
