#include "abstractcontextdplugin.h"

AbstractContextdPlugin::AbstractContextdPlugin(QString plugin_name) : plugin_name(plugin_name)
{

}

QString AbstractContextdPlugin::name()
{
	return plugin_name;
}
