#ifndef CONTEXT_H
#define CONTEXT_H

/**
 * \file domain.h
 * \author Martin Peres (martin dot peres at ensi-bourges dot fr)
 * \date 2010-06-07
 */

#include <QString>

/// This class represents a domain. A domain can be understood as a state that lets the user perform an activity such as browsing the web, paying his/her taxes, etc...
class Domain
{
private:
	QString _name, _display_name, _icon_path, _description;
	int _confidentiality, _reliability;

public:
	/*!
	 * \brief Construct an empty invalid domain.
	 */
	Domain();

	/*!
	 * \brief Construct a domain.
	 * \param name The name of the domain
	 * \param display_name The name of the domain, meant to be displayed
	 * \param icon The filepath to an icon representing the domain
	 * \param description A long description of what the domain is for
	 * \param confidentiality The confidentiality level of the domain
	 * \param reliability The reliability level of the apps in the domain
	 */

	//FIXME
	Domain(QString name, QString display_name, QString icon_path, QString description/*=QString()*/, int confidentiality, int reliability);

	/// The name of the domain
	QString name() const;

	/// The name of the domain, meant to be displayed
	QString displayName() const;

	/// The filepath to an icon representing the domain
	QString iconPath() const;

	/// The description of the domain's role
	QString description() const;

	/// The level of confidentiality of the data manipulated in the domain
	int confidentiality() const;

	/// The level of reliability of the applications in the domain
	int reliability() const;

	bool operator==(const Domain& b) const;
	bool operator!=(const Domain& b) const;

	enum {
		ConfidentialityMin=0,
		ConfidentialityDefault=ConfidentialityMin,
		ConfidentialityMax=100,
		ReliabilityMin=0,
		ReliabilityDefault=ReliabilityMin,
		ReliabilityMax=1024,
	};
};

#endif // CONTEXT_H
