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
#include <unistd.h>

#include <glib.h>
#include "gettext.h"
#include <gtk/gtk.h>
#include "gtk/filesel.h"
#include "gtk/gtkmain.h"

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
#include "send_message.h"
#include "session.h"
#include "procmime.h"
#include "imap.h"
#include "msgcache.h"
#include <libcontext.h>

#define PREFS_BLOCK_NAME "ContextNotifier"
#define PLUGIN_NAME _("Context Notifier")

#ifndef BLOCK_LISTING
//Set to TRUE to filter the mail list. FALSE otherwise
gboolean filter_mail_list=TRUE;
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
};

struct _Session
{
	SessionType type;

	SockInfo *sock;

	gchar *server;
	gushort port;

...
}

typedef enum {
	SESSION_UNKNOWN,
	SESSION_IMAP,
	SESSION_NEWS,
	SESSION_SMTP,
	SESSION_POP3
} SessionType;


struct _MsgFileInfo
{
	MsgInfo *msginfo;
        gchar *file;
        MsgFlags *flags;
};

struct _AttachInfo
{
	gchar *file;
	gchar *content_type;
	EncodingType encoding;
	gchar *name;
	goffset size;
};
*/

int always_refuse=0;

void check_drawing_rights(MessageView* msgview, const char* previousContext, const char* nextContext)
{
	MsgInfo* infos=msgview->msginfo;
	if(infos)
	{
		char required_context[101];
		
		context_bool ret=context_required_domain(required_context, sizeof(required_context), 
			"action", "read", "from", infos->from, "to", infos->to, "subject", infos->subject, 
			"date", infos->date, "inreplyto", infos->inreplyto, NULL, NULL);

		if(ret==CONTEXT_TRUE && strncmp(required_context, nextContext, sizeof(required_context))!=0)
		{
			TextView * textview=messageview_get_current_textview(msgview);
			//textview_show_error(textview, _("Context changed\n\nYou are not able to see this mail in this context."));
			//messageview_destroy(msgview);
			textview_clear(textview);
			messageview_clear(msgview);
			messageview_destroy(msgview);
		}
		else
		{
			//Refresh
			messageview_show(msgview, infos, TRUE);
		}
	}
}

void refreshListMail()
{
	MainWindow *mainwin = mainwindow_get_mainwindow();
	FolderItem *fitem;
	FolderView *folderview = mainwin->folderview;

	if (!mainwin->summaryview->folder_item) return;
	if (!folderview->opened) return;

	folder_update_op_count();

	fitem = gtk_cmctree_node_get_row_data(GTK_CMCTREE(folderview->ctree),
					    folderview->opened);
	if (!fitem) return;

	folder_item_scan(fitem);
	summary_show(mainwin->summaryview, fitem);
}

static void contextChanged(const char* previousContext, const char* nextContext, void* user_data)
{
	if(strcmp(previousContext, nextContext)!=0)
	{
		//Claws mail is being changed in order to comply with the new domain
		always_refuse=1;
		
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
		
		//Refresh the mail list
		refreshListMail();
		
		//reset to normal mode
		always_refuse=0;
	}
}

static gboolean on_open_mail(gpointer source, gpointer data)
{
        PreOpenHookData *hookdata=(PreOpenHookData *)source;
        MsgInfo *infos = (MsgInfo *)(hookdata->msginfo);
	
	if(always_refuse)
		return TRUE;
        
	context_result res=context_changed("action", "read", "from", infos->from, "to", infos->to, "subject", infos->subject, 
			"date", infos->date, "inreplyto", infos->inreplyto, NULL, NULL);
		
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
	if(context_current_domain(context, sizeof(context)-1)==CONTEXT_TRUE)
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
	
	char current_context[101];
	
	context_bool ret=context_current_domain(current_context, sizeof(current_context));
	if(ret==CONTEXT_TRUE)
	{
		char required_context[101];
		
		context_bool ret=context_required_domain(required_context, sizeof(required_context), 
			"action", "list", "from", infos->from, "to", infos->to, "subject", infos->subject, 
			"date", infos->date, "inreplyto", infos->inreplyto, NULL, NULL);

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
		
	char current_context[101];
	
	context_bool ret=context_current_domain(current_context, sizeof(current_context));
	if(ret==CONTEXT_TRUE)
	{
		char required_context[101];
		
		context_bool ret=context_required_domain(required_context, sizeof(required_context), 
			"action", "read", "from", infos->from, "to", infos->to, "subject", infos->subject, 
			"date", infos->date, "inreplyto", infos->inreplyto, NULL, NULL);

		if(ret==CONTEXT_TRUE && strncmp(current_context, required_context, sizeof(required_context))==0)
			return FALSE;	
	}
	
	return TRUE;
}

// Re-edit a message
static gboolean on_send_mail_view(gpointer source, gpointer data)
{
	MsgInfo *infos = (MsgInfo *)source;
	if(infos==NULL)
		return TRUE;
		
	char current_context[101];
	
	context_bool ret=context_current_domain(current_context, sizeof(current_context));
	if(ret==CONTEXT_TRUE)
	{
		char required_context[101];
		
		context_bool ret=context_required_domain(required_context, sizeof(required_context),
			"action", "read", "from", infos->from, "to", infos->to, "subject", infos->subject, 
			"date", infos->date, "inreplyto", infos->inreplyto, NULL, NULL);

		if(ret==CONTEXT_TRUE && strncmp(current_context, required_context, sizeof(required_context))==0)
			return FALSE;	
	}
	
	return TRUE;
}

// send_message_on_check_mail
static gboolean on_check_mail(gpointer source, gpointer data)
{
	MsgInfo *infos = (MsgInfo *)source;
	if(infos==NULL)
		return TRUE;

	context_result res=context_changed("action", "send", "from", infos->from, "to", infos->to, "subject", 
        infos->subject, "date", infos->date, "inreplyto", infos->inreplyto, NULL, NULL);
		
	if(res==CONTEXT_ACCEPTED)
		return FALSE;
	else
		return TRUE;
}



// compose_on_check_attach_files
static gboolean on_check_attach_files(gpointer source, gpointer data)
{
	OnAttachHookData *hookdata=(OnAttachHookData *)source;
	
	if(hookdata->ainfo==NULL)
		return TRUE;

	char s_size[20];
	snprintf(s_size, sizeof(s_size)-1, "%i", (int)hookdata->ainfo->size);

	context_result res=context_changed("action", "attach_file", "file", hookdata->ainfo->file, "content_type", hookdata->ainfo->content_type, "name", 
        hookdata->ainfo->name, "file_size", s_size, NULL, NULL);
		
	if(res!=CONTEXT_ACCEPTED)
	{
                const gchar* msg=_("PIGAContext : Access Forbidden\n\nYou don't have the permission to attach this file.");
                hookdata->error=g_memdup(msg, strlen(msg)+1);
               
                return TRUE;
        }
        else
                return FALSE;

}


// compose_on_check_insert_files
static gboolean on_check_insert_files(gpointer source, gpointer data)
{
	OnInsertHookData *hookdata=(OnInsertHookData *)source;
	if(hookdata->file==NULL)
		return TRUE;

	context_result res=context_changed("action", "insert_file", "file", hookdata->file, NULL, NULL);
		
	if(res!=CONTEXT_ACCEPTED)
	{
                const gchar* msg=_("PIGAContext : Access Forbidden\n\nYou don't have the permission to insert this file.");
                hookdata->error=g_memdup(msg, strlen(msg)+1);
                
                return TRUE;
        }
        else
                return FALSE;
}

const char* session_type(SessionType type)
{
	const char* s_type=NULL;
	switch(type)
	{
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
		case SESSION_UNKNOWN:
		default:
			s_type="unknown";
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
	
	if(always_refuse)
		return TRUE;
		
	const char* s_type=session_type(session->type);
	
	char s_port[11];
	snprintf(s_port, sizeof(s_port)-1, "%i", session->port);

	context_result res=context_changed("action", "connect", "host", session->server, "port", s_port, "session_type", s_type, NULL, NULL);
		
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
	
	if(always_refuse)
		return TRUE;
	
	const char* s_type=session_type(session->type);
	
	char s_port[11];
	snprintf(s_port, sizeof(s_port)-1, "%i", session->port);

	printf("\n\nClose session !\n");
	printf("        server: %s\n", session->server);
	printf("        port: %s\n", s_port);
	printf("        type: %s\n", s_type);

	context_result res=context_changed("action", "close", "host", session->server, "port", s_port, "session_type", s_type, NULL, NULL);
		
	if(res==CONTEXT_ACCEPTED)
		return FALSE;
	else
		return TRUE;
}

// Hook ids
gint mail_pre_open=0;
gint mail_pre_list=0;
gint compose_on_reply=0;
gint session_on_open=0;
gint session_on_close=0;
gint compose_on_send_mail=0;
gint send_message_on_check_mail=0;
gint compose_check_attach_file=0;
gint compose_check_inserted_files=0;

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
	mail_pre_open=hooks_register_hook (MAIL_PRE_OPEN_HOOKLIST, on_open_mail, NULL);
	if(filter_mail_list)
		mail_pre_list=hooks_register_hook (MAIL_PRE_LIST_HOOKLIST, on_list_mail, NULL);
	compose_on_reply=hooks_register_hook (COMPOSE_ON_REPLY_MAIL_HOOKLIST, on_reply_mail, NULL);
	session_on_open=hooks_register_hook (SESSION_ON_OPEN_HOOKLIST, on_open_session, NULL);	
	session_on_close=hooks_register_hook (SESSION_ON_CLOSE_HOOKLIST, on_close_session, NULL);	
	compose_on_send_mail=hooks_register_hook (COMPOSE_ON_SEND_MAIL_VIEW_HOOKLIST, on_send_mail_view, NULL);
	send_message_on_check_mail=hooks_register_hook (SEND_MESSAGE_ON_CHECK_MAIL_HOOKLIST, on_check_mail, NULL);
	compose_check_attach_file=hooks_register_hook (COMPOSE_CHECK_ATTACH_FILES_HOOKLIST, on_check_attach_files, NULL);
	compose_check_inserted_files=hooks_register_hook (COMPOSE_CHECK_INSERT_FILES_HOOKLIST, on_check_insert_files, NULL);
	
	//Register clawsMail
	context_register_application("claws-mail");
	
	// Send the first event
	char s_pid[11];
	snprintf(s_pid, sizeof(s_pid)-1, "%i", getpid());
	context_changed("action", "claws_open", "pid", s_pid, NULL, NULL);
	
	//Register the callback for context changes
	//context_set_domain_changed_callback(contextChanged, NULL);
	check_context_callback(NULL);

	return 0;
}

gboolean plugin_done(void)
{

	// Send the last event
	char s_pid[11];
	snprintf(s_pid, sizeof(s_pid)-1, "%i", getpid());
	context_changed("action", "claws_close", "pid", s_pid, NULL, NULL);


	if (claws_is_exiting())
		return TRUE;

	hooks_unregister_hook (MAIL_PRE_OPEN_HOOKLIST, mail_pre_open);
	if(filter_mail_list)
		hooks_unregister_hook (MAIL_PRE_LIST_HOOKLIST, mail_pre_list);
	
	hooks_unregister_hook (COMPOSE_ON_REPLY_MAIL_HOOKLIST, compose_on_reply);
	hooks_unregister_hook (SESSION_ON_OPEN_HOOKLIST, session_on_open);
	hooks_unregister_hook (SESSION_ON_CLOSE_HOOKLIST, session_on_close);
	hooks_unregister_hook (COMPOSE_ON_SEND_MAIL_VIEW_HOOKLIST, compose_on_send_mail);
	hooks_unregister_hook (SEND_MESSAGE_ON_CHECK_MAIL_HOOKLIST, send_message_on_check_mail);
	hooks_unregister_hook (COMPOSE_CHECK_ATTACH_FILES_HOOKLIST, compose_check_attach_file);
	hooks_unregister_hook (COMPOSE_CHECK_INSERT_FILES_HOOKLIST, compose_check_inserted_files);

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
