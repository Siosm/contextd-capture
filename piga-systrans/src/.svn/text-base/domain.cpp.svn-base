#include "domain.h"

Domain::Domain() :
		_confidentiality(ConfidentialityDefault),
		_reliability(ReliabilityDefault)
{
}

Domain::Domain(QString name, QString display_name, QString icon_path, QString description, int confidentiality, int reliability) :
		_name(name),
		_display_name(display_name),
		_icon_path(icon_path),
		_description(description),
		_confidentiality(confidentiality),
		_reliability(reliability)
{

}

QString Domain::name() const
{
	return _name;
}

QString Domain::displayName() const
{
	return _display_name;
}

QString Domain::iconPath() const
{
	return _icon_path;
}

QString Domain::description() const
{
	return _description;
}

int Domain::confidentiality() const
{
	return _confidentiality;
}

int Domain::reliability() const
{
	return _reliability;
}

bool Domain::operator==(const Domain& b) const
{
	return _name==b._name &&  _display_name==b._display_name && _icon_path==b._icon_path && _description==b._description
			&& _confidentiality==b._confidentiality && _reliability==b._reliability;
}

bool Domain::operator!=(const Domain& b) const
{
	return !operator==(b);
}
