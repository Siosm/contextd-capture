#ifndef VARIABLE_TYPES_H
#define VARIABLE_TYPES_H

#include <QString>

class Type
{
private:
	static const QString InvalidTag;
	static const QString RegExpTag;
	static const QString StringTag;
	static const QString StringListTag;
	static const QString IntTag;
	static const QString BooleanTag;

public:
	enum { Invalid=0, RegExp, String, StringList, Int, Boolean};

	static QString toString(int type);
	static QString toStringForDisplay(int type);
	static int fromString(QString type);
};


#endif // VARIABLE_TYPES_H
