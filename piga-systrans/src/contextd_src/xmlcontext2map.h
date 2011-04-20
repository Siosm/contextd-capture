#ifndef XMLCONTEXT2MAP_H
#define XMLCONTEXT2MAP_H

#include "xmlsp.h"

#include <QtCore/QMap>

class XMLContext2Map : public XMLSP::Parser
{
private:
	QMap<QString, QString> _currentState;

	//XML Parsing
	bool on_tag_open(const std::string& tag_name, XMLSP::StringMap& attributes);
	void on_error(int errnr, int line, int col, const std::string& message);
	bool on_document_begin();
public:
    XMLContext2Map();

    QMap<QString, QString> context();
};

#endif // XMLCONTEXT2MAP_H
