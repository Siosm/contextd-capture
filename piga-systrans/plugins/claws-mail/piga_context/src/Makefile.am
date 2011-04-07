plugindir = $(CLAWS_MAIL_PLUGINDIR)

plugin_LTLIBRARIES = piga_context.la

piga_context_la_SOURCES = \
	piga_context.c \
	gettext.h

piga_context_la_LDFLAGS = \
	-avoid-version -module

piga_context_la_LIBADD = \
	$(GTK_LIBS) \
	$(CONTEXT_LIBS)
	
AM_CPPFLAGS = \
	$(CLAWS_MAIL_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(GTK_CFLAGS) \
	$(CONTEXT_CFLAGS) \
	-DLOCALEDIR=\""$(localedir)"\" 

if FILTER_MAIL_LIST
AM_CPPFLAGS += -DBLOCK_LISTING
endif