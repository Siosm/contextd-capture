#include <QRegExp>
#include <limits.h>
#include "programvariable.h"
#include "variabletypes.h"

QMap<QPair<QString, int>, QStringList> ProgramVariable::_assigned_values;

ProgramVariable::ProgramVariable() :
		_type(Type::Invalid)
{
}

ProgramVariable::ProgramVariable(const QString& name, const QString& action, const QString& description) :
		_name(name), _action(action), _description(description), _type(Type::RegExp)
{
}

QString ProgramVariable::name() const
{
	return _name;
}

void ProgramVariable::setName(const QString& name)
{
	_name=name;
}

QString ProgramVariable::action() const
{
	return _action;
}

void ProgramVariable::setAction(const QString& action)
{
	_action=action;
}

QString ProgramVariable::description() const
{
	return _description;
}

void ProgramVariable::setDescription(const QString& desc)
{
	_description=desc;
}

int ProgramVariable::type() const
{
	return _type;
}

void ProgramVariable::setType(int type)
{
	_type=type;
}

bool ProgramVariable::setType(const QString& type)
{
	_type=Type::fromString(type);
	return _type!=Type::Invalid;
}

QString ProgramVariable::intRange() const
{
	if (_int_min!=QString() || _int_max!=QString())
		return _int_min+":"+_int_max;
	else
		return QString();
}

void ProgramVariable::setIntRange(int min, int max)
{
	setIntMinimum(min);
	setIntMaximum(max);
}

bool ProgramVariable::setIntRangeFromString(const QString& range)
{
	if(range.startsWith(":"))
	{
		bool ok;
		QString maxStr=range;
		int max=maxStr.remove(0, 1).toInt(&ok);

		if(ok)
			setIntMaximum(max);
		return ok;
	}
	else if(range.endsWith(":"))
	{
		bool ok;
		QString minStr=range;
		minStr.chop(1);
		int min=minStr.toInt(&ok);

		if(ok)
			setIntMinimum(min);
		return ok;
	}
	else
	{
		QStringList values=range.split(':');
		if(values.size()==2)
		{
			bool ok;

			QString maxStr=values.at(1);
			int max=maxStr.toInt(&ok);

			if(ok)
				setIntMaximum(max);
			else
				return false;

			QString minStr=values.at(0);
			int min=minStr.toInt(&ok);

			if(ok)
				setIntMinimum(min);
			return ok;
		}
		else
			return false;
	}
}

int ProgramVariable::intMinimum() const
{
	bool ok;
	int tmp;

	tmp=_int_min.toInt(&ok);
	if(!ok)
		return INT_MIN;
	else
		return tmp;
}

void ProgramVariable::setIntMinimum(int min)
{
	_int_min=QString("%1").arg(min);
}

int ProgramVariable::intMaximum() const
{
	bool ok;
	int tmp;

	tmp=_int_max.toInt(&ok);
	if(!ok)
		return INT_MAX;
	else
		return tmp;
}

void ProgramVariable::setIntMaximum(int max)
{
	_int_max=QString("%1").arg(max);
}

QString ProgramVariable::stringValidation() const
{
	return _string_validation;
}

void ProgramVariable::setStringValidation(const QString& validation)
{
	_string_validation=validation;
}

QString ProgramVariable::stringListAsString() const
{
	return QString(";")+_string_list_vals.join(";")+QString(";");
}

QStringList ProgramVariable::stringList() const
{
	return _string_list_vals;
}

void ProgramVariable::setStringListFromString(const QString& list)
{
	QStringList l;

	if(list.size()>2 && list[0]!=list[list.size()-1])
		l=QStringList(list);
	else if(list.size()>2)
	{
		QString list_t=list.mid(1, list.size()-2);
		l=list_t.split(list[0]);
	}

	setStringList(l);
}

void ProgramVariable::setStringList(const QStringList& list)
{
	_string_list_vals=list;
}

bool ProgramVariable::validate(const QString& value) const
{
	bool ok;
	int value_i;

	switch(type())
	{
	case Type::RegExp:
		return QRegExp(value).isValid() && (_string_validation==QString() || QRegExp(_string_validation).exactMatch(value));

	case Type::String:
		return _string_validation==QString() || QRegExp(_string_validation).exactMatch(value);

	case Type::StringList:
		return _string_list_vals.contains(value);

	case Type::Int:
		value_i=value.toInt(&ok);
		if(!ok)
			return false;

		return validate(value_i);

	case Type::Boolean:
		return true;
	default:
		return false;
	}

	return false;
}

bool ProgramVariable::validate(int value) const
{
	bool ok;
	int tmp;

	if(type()==Type::Int)
	{
		tmp=_int_min.toInt(&ok);
		if(_int_min!=QString() && (!ok || value<tmp))
			return false;

		tmp=_int_max.toInt(&ok);
		if(_int_max!=QString() && (!ok || value>tmp))
			return false;

		return true;
	}
	else
		return false;
}

bool ProgramVariable::operator==(const ProgramVariable& b) const
{
	return name()==b.name() && action()==b.action() && description()==b.description() &&
			type()==b.type() && _int_min==b._int_min &&
			_int_max==b._int_max && _string_validation==b._string_validation &&
			_string_list_vals==b._string_list_vals;
}

bool ProgramVariable::operator!=(const ProgramVariable& b) const
{
	return !operator ==(b);
}

QStringList ProgramVariable::assignedValues(const QString& name, const int& type)
{
	return _assigned_values[QPair<QString, int>(name, type)];
}

void ProgramVariable::addAssignedValue(const QString& name, const int& type, const QString& value)
{
	QStringList &l=_assigned_values[QPair<QString, int>(name, type)];
	if(!l.contains(value))
		l.append(value);
}

void ProgramVariable::clearAssignedValues()
{
	_assigned_values.clear();
}
