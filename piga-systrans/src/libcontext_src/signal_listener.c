#include <stdio.h>
#include <stdlib.h>
#include "../context-common.h"
#include "libcontext.h"

#include <glib.h>
#include <glib-object.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>

struct context_thread_data
{
	contextChangedCallback ccc;
	void* user_data;
};

DBusConnection *bus;

static DBusHandlerResult
signal_filter(DBusConnection* connection, DBusMessage *message, void *user_data)
{
	DBusError error;
	struct context_thread_data* data=(struct context_thread_data*)user_data;

	dbus_error_init(&error);

	if (dbus_message_is_signal(message, SERVICE_NAME, "globalContextChanged"))
	{
		const char* previousContext;
		const char* nextContext;

		if (dbus_message_get_args(
				  message, &error,
				  DBUS_TYPE_STRING, &previousContext,
				  DBUS_TYPE_STRING, &nextContext,
				  DBUS_TYPE_INVALID))
		{
			printf("LibContext: Transit from %s to %s\n", previousContext, nextContext);
			data->ccc(previousContext, nextContext, data->user_data);
		}
		else
		{
			printf("Cannot get arguments !!\n");
			dbus_error_free(&error);
		}

		return DBUS_HANDLER_RESULT_HANDLED;
	}
	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

void* dbus_listen_for_signals(void* user_data)
{
	DBusError error;
	dbus_error_init(&error);

	GMainContext* mainContext=g_main_context_new();
	GMainLoop* loop = g_main_loop_new (mainContext, 0);

	dbus_g_thread_init();

	bus = dbus_bus_get(DBUS_BUS_SYSTEM, &error);
	if (bus == NULL) {
		g_printerr("Failed to open connection to bus: %s", error.message);
		dbus_error_free(&error);
		return NULL;
	}

	dbus_connection_setup_with_g_main (bus, mainContext);

	dbus_bus_add_match(bus, "interface='" SERVICE_NAME "'", &error);
	if(dbus_connection_add_filter(bus, signal_filter, user_data, NULL)==FALSE)
		printf("dbus_connection_add_filter returned false !\n");

	g_main_loop_run(loop);

	return NULL;
}

#include <pthread.h>
void context_set_context_changed_callback(contextChangedCallback ccc, void* user_data)
{
	static struct context_thread_data data;
	data.ccc=ccc;
	data.user_data=user_data;

	pthread_t thread;
	pthread_create(&thread, NULL,  dbus_listen_for_signals, (void*) &data);
}
