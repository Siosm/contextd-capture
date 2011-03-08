/*
 * piga_context -- for Claws Mail
 *
 * Copyright (C) 2005 Colin Leroy <colin@colino.net>
 *
 * Sylpheed is a GTK+ based, lightweight, and fast e-mail client
 * Copyright (C) 1999-2005 Hiroyuki Yamamoto and the Claws Mail Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <string.h>

#include <glib.h>
#include "gettext.h"
#include <gtk/gtk.h>

#include "defs.h"
#include "claws.h"
#include "version.h"
#include "prefs.h"
#include "prefs_gtk.h"
#include "main.h"
#include "menu.h"
#include "hooks.h"
#include "plugin.h"
#include "procmsg.h"
#include "summaryview.h"
#include "mainwindow.h"
#include "alertpanel.h"
#include "compose.h"
#include "sourcewindow.h"
#include <libcontext.h>

#define PREFS_BLOCK_NAME "ContextNotifier"
#define PLUGIN_NAME _("Context Notifier")

#ifndef BLOCK_LISTING
//Set to TRUE to filter the mail list. FALSE otherwise
gboolean filter_mail_list=FALSE;
#else
gboolean filter_mail_list=TRUE;
#endif

/*struct _MsgInfo
{
	guint refcnt;

	guint  msgnum;
	goffset  size;
	time_t mtime;
	time_t date_t;
	time_t thread_date;

	MsgFlags flags;

	gchar *fromname;

	gchar *date;
	gchar *from;
	gchar *to;
	gchar *cc;
	gchar *newsgroups;
	gchar *subject;
	gchar *msgid;
	gchar *inreplyto;
	gchar *xref;

	FolderItem *folder;
	FolderItem *to_folder;

	FolderItem *to_filter_folder;	
	FiltOp filter_op;

	GSList *references;
	gchar *fromspace;

	gint score;

	gchar *plaintext_file;
        
        gint hidden;
        * 
	gint total_size;
	gint planned_download;

	GSList *tags;

	MsgInfoExtraData *extradata;
};

struct _MsgInfoExtraData
{
	gchar *xface;
	gchar *face;

	gchar *dispositionnotificationto;
	gchar *returnreceiptto;

	// used only for partially received messages
	gchar *partial_recv;
	gchar *account_server;
	gchar *account_login;

 	// Mailing list support
 	gchar *list_post;
 	gchar *list_subscribe;
 	gchar *list_unsubscribe;
 	gchar *list_help;
 	gchar *list_archive;
 	gchar *list_owner;
};

struct _MsgInfoUpdate {
	MsgInfo	*msginfo;
	MsgInfoUpdateFlags flags;
};*/

/*struct _Session
{
	SessionType type;

	SockInfo *sock;

	gchar *server;
	gushort port;

...
}*/

/*typedef enum {
	SESSION_UNKNOWN,
	SESSION_IMAP,
	SESSION_NEWS,
	SESSION_SMTP,
	SESSION_POP3
} SessionType;*/


void check_drawing_rights(MessageView* msgview, const char* previousContext, const char* nextContext)
{
	MsgInfo* infos=msgview->msginfo;
	if(infos)
	{
		char required_context[101];
		
		context_bool ret=context_required_context(required_context, sizeof(required_context), 
			"from", infos->from, "to", infos->to, "subject", infos->subject, "date", infos->date, 
			"inreplyto", infos->inreplyto, NULL, NULL);

		if(ret==CONTEXT_TRUE && strncmp(required_context, nextContext, sizeof(required_context))!=0)
		{
			TextView * textview=messageview_get_current_textview(msgview);
			textview_show_error(textview, _("Context changed\n\nYou are not able to see this mail in this context."));
		}
		else
		{
			//Refresh
			messageview_show(msgview, infos, TRUE);
		}
	}
}

static void contextChanged(const char* previousContext, const char* nextContext, void* user_data)
{
	if(strcmp(previousContext, nextContext)!=0)
	{	
		//Refresh the main window
		MainWindow* mainwindow=mainwindow_get_mainwindow();
		if(mainwindow)
		{
			MessageView* msgview=mainwindow->messageview;
			
			if(msgview)
			{
				check_drawing_rights(msgview, previousContext, nextContext);
				
				//Refresh the other windows
				GList* list=messageview_get_msgview_list();
				for(;list != NULL; list = g_list_next(list))
				{
					msgview=(MessageView*)list->data;
					check_drawing_rights(msgview, previousContext, nextContext);
				}
				
				//Close all compose windows
				compose_destroy_all();
			}
		}
		
		//close all source windows
			sourcewindow_destroy_all();
		
	}
}

static gboolean on_open_mail(gpointer source, gpointer data)
{
        PreOpenHookData *hookdata=(PreOpenHookData *)source;
        MsgInfo *infos = (MsgInfo *)(hookdata->msginfo);
        
        printf("\n\nOpen mail !\n");
        printf("        from: %s\n", infos->from);
        printf("        to: %s\n", infos->to);
        printf("        subject: %s\n", infos->subject);
        printf("        date: %s\n", infos->date);
        printf("        inreplyto: %s\n", infos->inreplyto);
        
        context_result res=context_changed("from", infos->from, "to", infos->to, "subject", 
        infos->subject, "date", infos->date, "inreplyto", infos->inreplyto, NULL, NULL);
        	
        if(res!=CONTEXT_ACCEPTED)
        {
                const gchar* msg=_("PIGAContext : Access Forbidden\n\nOpening this mail does not comply with the current set of rule.");
                hookdata->error=g_memdup(msg, strlen(msg)+1);
                
                return TRUE;
        }
        else
                return FALSE;
}

gint check_timeout=0;
static gint check_context_callback( gpointer data )
{
	static char previousContext[151];
	char context[151];
	if(context_current_global_context(context, sizeof(context)-1)==CONTEXT_TRUE)
	{
		if(strncmp(previousContext, context, sizeof(previousContext)-1)!=0)
		{
			contextChanged(previousContext, context, data);
			strncpy(previousContext, context, sizeof(previousContext)-1);
		}
	}
	
	check_timeout=gtk_timeout_add(200, (GtkFunction)check_context_callback, data);
	
	return 0;
}

//List mail
static gboolean on_list_mail(gpointer source, gpointer data)
{
	MsgInfo *infos = (MsgInfo *)source;
	if(infos==NULL)
		return TRUE;
		
	printf("on_list_mail_Infos = %i\n", infos);
	
	char current_context[101];
	
	context_bool ret=context_current_global_context(current_context, sizeof(current_context));
	if(ret==CONTEXT_TRUE)
	{
		char required_context[101];
		
		context_bool ret=context_required_context(required_context, sizeof(required_context), 
			"from", infos->from, "to", infos->to, "subject", infos->subject, "date", infos->date, 
			"inreplyto", infos->inreplyto, NULL, NULL);
			
		printf("on_list_mail: current_context=%s, required_context=%s\n", current_context, required_context);

		if(ret==CONTEXT_TRUE && strncmp(current_context, required_context, sizeof(required_context))==0)
			return FALSE;	
	}
	
	return TRUE;
}

// compose_on_reply_mail 
static gboolean on_reply_mail(gpointer source, gpointer data)
{
	MsgInfo *infos = (MsgInfo *)source;
	if(infos==NULL)
		return TRUE;
	
	printf("on_reply_mail_Infos = %i\n", infos);
		
	char current_context[101];
	
	context_bool ret=context_current_global_context(current_context, sizeof(current_context));
	if(ret==CONTEXT_TRUE)
	{
		char required_context[101];
		
		context_bool ret=context_required_context(required_context, sizeof(required_context), 
			"from", infos->from, "to", infos->to, "subject", infos->subject, "date", infos->date, 
			"inreplyto", infos->inreplyto, NULL, NULL);
			
		printf("\compose_close_reply_mail: current_context=%s, required_context=%s\n", current_context, required_context);

		if(ret==CONTEXT_TRUE && strncmp(current_context, required_context, sizeof(required_context))==0)
			return FALSE;	
	}
	
	return TRUE;
}

const char* session_type(SessionType type)
{
	const char* s_type;
	switch(type)
	{
		case SESSION_UNKNOWN:
			s_type="unknown";
			break;
		case SESSION_IMAP:
			s_type="imap";
			break;
		case SESSION_NEWS:
			s_type="news";
			break;
		case SESSION_SMTP:
			s_type="smtp";
			break;
		case SESSION_POP3:
			s_type="pop3";
			break;
	}
	
	return s_type;
}

// session_on_open
static gboolean on_open_session(gpointer source, gpointer data)
{
	Session *session = (Session *)source;
	if(session==NULL)
		return TRUE;
		
	const char* s_type=session_type(session->type);
	
	char s_port[11];
	snprintf(s_port, sizeof(s_port)-1, "%i", session->port);

	printf("\n\nOpen session !\n");
	printf("        server: %s\n", session->server);
	printf("        port: %s\n", s_port);
	printf("        type: %s\n", s_type);

	context_result res=context_changed("status", "connect", "host", session->server, "port", s_port, "session_type", s_type, NULL, NULL);
		
	if(res==CONTEXT_ACCEPTED)
		return FALSE;
	else
		return TRUE;
}

// session_on_close
static gboolean on_close_session(gpointer source, gpointer data)
{
	Session *session = (Session *)source;
	if(session==NULL)
		return TRUE;
		
	const char* s_type=session_type(session->type);
	
	char s_port[11];
	snprintf(s_port, sizeof(s_port)-1, "%i", session->port);

	printf("\n\nClose session !\n");
	printf("        server: %s\n", session->server);
	printf("        port: %s\n", s_port);
	printf("        type: %s\n", s_type);

	context_result res=context_changed("status", "close", "host", session->server, "port", s_port, "session_type", s_type, NULL, NULL);
		
	if(res==CONTEXT_ACCEPTED)
		return FALSE;
	else
		return TRUE;
}

gint plugin_init(gchar **error)
{
#ifdef G_OS_UNIX
	bindtextdomain(TEXTDOMAIN, LOCALEDIR);
#else
	bindtextdomain(TEXTDOMAIN, get_locale_dir());
#endif
	bind_textdomain_codeset (TEXTDOMAIN, "UTF-8");

	if( !check_plugin_version(MAKE_NUMERIC_VERSION(2,9,2,72),
				VERSION_NUMERIC, PLUGIN_NAME, error) )
		return -1;

	//Hooks
	hooks_register_hook (MAIL_PRE_OPEN_HOOKLIST, on_open_mail, NULL);
	if(filter_mail_list)
		hooks_register_hook (MAIL_PRE_LIST_HOOKLIST, on_list_mail, NULL);
	hooks_register_hook (COMPOSE_ON_REPLY_MAIL_HOOKLIST, on_reply_mail, NULL);
	hooks_register_hook (SESSION_ON_OPEN_HOOKLIST, on_open_session, NULL);	
	hooks_register_hook (SESSION_ON_CLOSE_HOOKLIST, on_close_session, NULL);	

	//Register clawsMail
	context_register_application("claws-mail");
	
	//Register the callback for context changes
	//context_set_context_changed_callback(contextChanged, NULL);
	check_context_callback(NULL);

	return 0;
}

gboolean plugin_done(void)
{
	if (claws_is_exiting())
		return TRUE;

	hooks_unregister_hook (MAIL_PRE_OPEN_HOOKLIST, on_open_mail);
	if(filter_mail_list)
		hooks_unregister_hook (MAIL_PRE_LIST_HOOKLIST, on_list_mail);
	hooks_unregister_hook (COMPOSE_ON_REPLY_MAIL_HOOKLIST, on_reply_mail);
	hooks_unregister_hook (SESSION_ON_OPEN_HOOKLIST, on_open_session);
	hooks_unregister_hook (SESSION_ON_CLOSE_HOOKLIST, on_close_session);
	
	gtk_timeout_remove(check_timeout);

	return TRUE;
}

const gchar *plugin_name(void)
{
	return PLUGIN_NAME;
}

const gchar *plugin_desc(void)
{
	return _("Turns claws-mail into a PIGAContext-aware E-mail client !");
}

const gchar *plugin_type(void)
{
	return "GTK2";
}

const gchar *plugin_licence(void)
{
	return "GPL2+";
}

const gchar *plugin_version(void)
{
	return PLUGINVERSION;
}

struct PluginFeature *plugin_provides(void)
{
	static struct PluginFeature features[] = 
		{ {PLUGIN_FILTERING, N_("PIGA Context")},
		  {PLUGIN_NOTHING, NULL}};
	return features;
}
