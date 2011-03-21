#include "variabletypes.h"

const QString Type::InvalidTag="invalid";
const QString Type::RegExpTag="regexp";
const QString Type::StringTag="string";
const QString Type::StringListTag="string_list";
const QString Type::IntTag="int";
const QString Type::BooleanTag="boolean";

QString Type::toString(int type)
{
	switch(type)
	{
	case Type::RegExp:
		return RegExpTag;
	case Type::String:
		return StringTag;
	case Type::StringList:
		return StringListTag;
	case Type::Int:
		return IntTag;
	case Type::Boolean:
		return BooleanTag;
	default:
		return InvalidTag;
	}
}

QString Type::toStringForDisplay(int type)
{
	switch(type)
	{
	case Type::RegExp:
		return "RegExp";
	case Type::String:
		return "String";
	case Type::StringList:
		return "List";
	case Type::Int:
		return "Integer";
	case Type::Boolean:
		return "Boolean";
	default:
		return InvalidTag;
	}
}

int Type::fromString(QString type)
{
	QString t=type.toLower();

	if(t==RegExpTag)
		return Type::RegExp;
	else if(t==StringTag)
		return Type::String;
	else if(t==StringListTag)
		return Type::StringList;
	else if(t==IntTag)
		return Type::Int;
	else if(t==BooleanTag)
		return Type::Boolean;
	else
		return Type::Invalid;
}
