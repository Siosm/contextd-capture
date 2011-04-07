#include "xmlcontext2map.h"
#include "eventdispatcher.h"

XMLContext2Map::XMLContext2Map() : XMLSP::Parser()
{
}

bool XMLContext2Map::on_document_begin()
{
	_currentState.clear();
	return true;
}

bool XMLContext2Map::on_tag_open(const std::string& tag_name, XMLSP::StringMap& attributes)
{
	if(tag_name=="parameter")
	{
		if(attributes["name"]!="" && attributes["value"]!="")
		{
			QString value=QString::fromUtf8(attributes["value"].c_str());
			QString name=QString::fromUtf8(attributes["name"].c_str());

			_currentState[name]=value;
			return true;
		}
		else
		{
			EventDispatcher::instance().sendError("ContextClient::on_tag_open : assert failed : if(attributes[\"name\"]!=\"\" && attributes[\"value\"]!=\"\"");
			return false;
		}
	}

	return true;
}

void XMLContext2Map::on_error(int errnr, int line, int col, const std::string& message)
{
	QString error = "XML error(%1) at the line, col(%2, %3) : %4\n\n";
	error=error.arg(errnr).arg(line).arg(col).arg(message.c_str());

	EventDispatcher::instance().sendError(error);
}

QMap<QString, QString> XMLContext2Map::context()
{
	return _currentState;
}
