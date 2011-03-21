#ifndef PROGRAMACTION_H
#define PROGRAMACTION_H

/**
 * \file programaction.h
 * \author Martin Peres (martin dot peres at ensi-bourges dot fr)
 * \date 2010-06-07
 */

#include <QString>

class ProgramAction
{
	QString _name;
	QString _displayName;
	QString _description;

public:
	ProgramAction();
	ProgramAction(const QString& name, const QString& displayName, const QString& description);

	QString name() const;
	void setName(const QString& name);

	QString displayName() const;
	void setDisplayName(const QString& displayName);

	QString description() const;
	void setDescription(const QString& desc);

	bool operator==(const ProgramAction& b) const;
	bool operator!=(const ProgramAction& b) const;
};

#endif // PROGRAMACTION_H
