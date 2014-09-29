
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

//*********************** INCLUDE 
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <vte/vte.h>
#include <libnotify/notify.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourcelanguagemanager.h>
#include <gtksourceview/gtksourcestyleschememanager.h>
#include <webkit/webkit.h>
#include <gtksourceview/completion-providers/words/gtksourcecompletionwords.h>
#include <gtksourceview/gtksourcemarkattributes.h>
#include <gtksourceview/gtksourcecompletion.h>
#include <dirent.h>
#include "griffon_text_document.h"
#include "callbacks.h"
#include "griffon_defs.h"
#include "griffon_hl.h"
#include "griffon_config.h"
#include "griffon_funx.h"
#include "griffon_gtk_utils.h"
#include "interface.h"
#include "griffon_proj.h"
#include "help_def.h"
#include "rox_strings.h"
#include "minido.h"

#define MAXI 500

#define REFRESH_INTERVAL 15


gchar *p_project_name = NULL;
gchar *p_project_file_name = NULL;
gchar *p_dir_makefile = NULL;
gchar *p_dir_source = NULL;
gchar *p_file_executable = NULL;
gchar *p_cmd_make = NULL;
gchar *p_info = NULL;
gchar *p_icon = NULL;

//*********************** AUTOCOMP
GtkSourceCompletionWords *word_provider;

//*********************** STRUCTURE
enum
{
	COL_ICON = 0,
	COL_TEXT,
	COL_TEXT2,
	NUM_COLS2
};


struct page
{
	GtkWidget *label_page;
};
struct page text_sup[MAXI];


typedef struct
{
	GtkWidget* window_web;
	GtkWidget* vbox3;
	GtkWidget *hbox3;
	WebKitWebView *webView_w;
	GtkWidget *scrolledWindow;
	GtkWidget *entry_myadmin;
} win_web;

//*********************** DECLARATION 
int save_controle=0,nbr_term=0;
GtkTreeSelection *selection_myadmin ;
GtkWidget *toolbar;
GtkTreeSelection *selection_file; 
GtkListStore *store;
GtkListStore *store_todo;
GtkWidget* view_list;
GtkWidget* view_list_todo;
GtkListStore *store_book;
GtkWidget* view_list_book;
GtkWidget *view_a;
GtkWidget *button_web_image, *button_web_image2, *window_run ;
GtkWidget *view_myadmin, *view_sftp, *view_ftp;
GtkWidget *window_center, *icon_ok, *fam_hbox, *pWindow, *window_center_perl, *entry_man, *vbox_myadmin, *hpaned1, *vbox_sftp, *vbox_ftp, *vte, *splash_bar;
GList *combo1_items = NULL;
GtkWidget *combo1, *textview1, *hbox2, *scrolledwindow1, *label_note1,*label_note2, *label_note3, *label_note4, *scrolledwindow3, *textview2, *textview3, *vbox3, *scrolledwindow4, *scrolledwindow5, *textview4, *vbox4, *vbox5, *button_help1, *button_custom, *label_note5, *label_note6, *button_help2, *vbox6, *scrolledwindow7, *vbox_proj,*vbox_proj_main, *button_proj1,  *button_proj2, *button_proj3, *button_proj4, *button_proj5, *button_include1, *button_note1;
GtkWidget *filechooserwidget1, *label_tol, *icon_proj, *hbox_proj, *vbox2_proj,*scrolledWindow_sftp,*scrolledWindow_ftp,*combo2, *entry_custom, *entry_myadmin, *button_help6, *entry_help_custom, *hseparator_help;
GList *combo2_items = NULL;
GtkWidget *sView_scan;
GtkWidget *vte_add;
GtkWidget *recent_file;
GtkWidget *hbox_no;
GtkWidget *hbox_no2;
GtkFileFilter* filefilter;
GtkWidget *window1_popup=NULL;
GtkWidget *window1_popup_line=NULL;
int win_popup=0;
int win_popup_line=0;
WebKitWebView *webView_doc_line;
GtkWidget *view_help;
GtkWidget *vbox_help;
GtkWidget *statusbar_help;

int tab_fold[19000];

//*********************** AUTOCOMPLEMENTATION 
struct _TestProviderClass
{
	GObjectClass parent_class;
};

static void test_provider_iface_init (GtkSourceCompletionProviderIface *iface);
GType test_provider_get_type (void);
G_DEFINE_TYPE_WITH_CODE (TestProvider,test_provider,G_TYPE_OBJECT,G_IMPLEMENT_INTERFACE (GTK_SOURCE_TYPE_COMPLETION_PROVIDER,test_provider_iface_init))

//*********************** AUTOCOMP FUNCTION
static gchar * test_provider_get_name (GtkSourceCompletionProvider *provider)
{
	return g_strdup (((TestProvider *)provider)->name);
}

static gint test_provider_get_priority (GtkSourceCompletionProvider *provider)
{
	return ((TestProvider *)provider)->priority;
}

static GList * select_random_proposals (GList *all_proposals)
{
	GList *selection = NULL;
	GList *prop;

	for (prop = all_proposals; prop != NULL; prop = g_list_next (prop))
	{
		if (g_random_boolean ())
		{
			selection = g_list_prepend (selection, prop->data);
		}
	}

	return selection;
}

static void test_provider_populate (GtkSourceCompletionProvider *completion_provider,GtkSourceCompletionContext  *context)
{
	TestProvider *provider = (TestProvider *)completion_provider;
	GList *proposals;

	if (provider->is_random)
	{
		proposals = select_random_proposals (provider->proposals);
	}
	else
	{
		proposals = provider->proposals;
	}

	gtk_source_completion_context_add_proposals (context,completion_provider,proposals,TRUE);
}

static void test_provider_iface_init (GtkSourceCompletionProviderIface *iface)
{
	iface->get_name = test_provider_get_name;
	iface->populate = test_provider_populate;
	iface->get_priority = test_provider_get_priority;
}

static void test_provider_dispose (GObject *gobject)
{
	TestProvider *self = (TestProvider *)gobject;
	g_list_free_full (self->proposals, g_object_unref);
	self->proposals = NULL;
	g_clear_object (&self->icon);
	G_OBJECT_CLASS (test_provider_parent_class)->dispose (gobject);
}

static void test_provider_finalize (GObject *gobject)
{
	TestProvider *self = (TestProvider *)gobject;
	g_free (self->name);
	self->name = NULL;
	G_OBJECT_CLASS (test_provider_parent_class)->finalize (gobject);
}

static void test_provider_class_init (TestProviderClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	gobject_class->dispose = test_provider_dispose;
	gobject_class->finalize = test_provider_finalize;
}

static void test_provider_init (TestProvider *self)
{
	GList *proposals = NULL;
	self->proposals = proposals;
}

static void create_completion (GtkSourceView       *source_view,		   GtkSourceCompletion *completion)
{
	word_provider = gtk_source_completion_words_new (NULL, NULL);
	gtk_source_completion_words_register (word_provider,gtk_text_view_get_buffer (GTK_TEXT_VIEW (source_view)));
	gtk_source_completion_add_provider (completion,GTK_SOURCE_COMPLETION_PROVIDER (word_provider),NULL);

	g_object_set (word_provider, "priority", 10, NULL);
}

//*********************** FUNCTION NAVIGATEUR WEB LINK BLANK
gboolean myadmin_new_window (WebKitWebView *web_view,WebKitWebFrame *frame,WebKitNetworkRequest *request, WebKitWebNavigationAction *navigation_action,WebKitWebPolicyDecision *policy_decision,gpointer user_data)
{

	if(frame==NULL){return FALSE;}
	if(policy_decision==NULL){return FALSE;}
	if(user_data==NULL){return FALSE;}
	if(navigation_action==NULL){return FALSE;}

	g_signal_stop_emission_by_name (WEBKIT_WEB_VIEW (web_view), "navigation-requested");

	gchar const *uri = webkit_network_request_get_uri (WEBKIT_NETWORK_REQUEST(request));

	win_web *web_win = (win_web *) g_malloc (sizeof (win_web));

	web_win->window_web = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for(GTK_WINDOW(web_win->window_web),GTK_WINDOW(tea_main_window));
	gtk_window_resize (GTK_WINDOW (web_win->window_web), 770, 400);
	//gtk_window_maximize (GTK_WINDOW(web_win->window_web));

	web_win->vbox3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (web_win->window_web), GTK_WIDGET(web_win->vbox3));
	gtk_widget_show (GTK_WIDGET(web_win->vbox3));  

	web_win->hbox3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(web_win->hbox3));
	gtk_box_pack_start (GTK_BOX (web_win->vbox3), web_win->hbox3, FALSE, TRUE, 0);


	web_win->webView_w = WEBKIT_WEB_VIEW(webkit_web_view_new());

	GtkToolItem *tool_sep;
	GtkWidget *toolbar_myadmin;
	toolbar_myadmin = gtk_toolbar_new ();

	GtkToolItem *tool_myadmin_reload = gtk_tool_button_new_from_stock(GTK_STOCK_REFRESH  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_reload, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_reload));
	g_signal_connect ((gpointer) tool_myadmin_reload, "clicked",G_CALLBACK (myadmin_reload_win),web_win->webView_w);
	gtk_tool_item_set_tooltip_text(tool_myadmin_reload,_("Reload"));

	GtkToolItem *tool_myadmin_back = gtk_tool_button_new_from_stock(GTK_STOCK_UNDO  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_back, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_back));
	g_signal_connect ((gpointer) tool_myadmin_back, "clicked",G_CALLBACK (myadmin_back_win),web_win->webView_w);
	gtk_tool_item_set_tooltip_text(tool_myadmin_back,_("Undo"));

	GtkToolItem *tool_myadmin_prev = gtk_tool_button_new_from_stock(GTK_STOCK_REDO  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_prev, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_prev));
	g_signal_connect ((gpointer) tool_myadmin_prev, "clicked",G_CALLBACK (myadmin_forward_win),web_win->webView_w);
	gtk_tool_item_set_tooltip_text(tool_myadmin_prev,_("Redo"));

	GtkToolItem *tool_myadmin_stop = gtk_tool_button_new_from_stock(GTK_STOCK_STOP  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_stop, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_stop));
	g_signal_connect ((gpointer) tool_myadmin_stop, "clicked",G_CALLBACK (myadmin_stop_win),web_win->webView_w);
	gtk_tool_item_set_tooltip_text(tool_myadmin_stop,_("Stop"));

	tool_sep=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin ), tool_sep, -1);
	gtk_widget_show(GTK_WIDGET(tool_sep));

	GtkToolItem *tool_myadmin_source = gtk_tool_button_new_from_stock(GTK_STOCK_PROPERTIES   );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_source, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_source));
	g_signal_connect ((gpointer) tool_myadmin_source, "clicked",G_CALLBACK (myadmin_source_mode_get_url_win),web_win->webView_w);
	gtk_tool_item_set_tooltip_text(tool_myadmin_source,_("Source view"));

	GtkToolItem *tool_myadmin_view = gtk_tool_button_new_from_stock(GTK_STOCK_CONVERT    );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_view, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_view));
	g_signal_connect ((gpointer) tool_myadmin_view, "clicked",G_CALLBACK (myadmin_view_mode_get_url_win),web_win->webView_w);
	gtk_tool_item_set_tooltip_text(tool_myadmin_view,_("Web view"));


	gtk_box_pack_start (GTK_BOX(web_win->hbox3), toolbar_myadmin, TRUE, TRUE, 0);
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_myadmin), GTK_TOOLBAR_ICONS);
	gtk_widget_show (GTK_WIDGET(toolbar_myadmin)); 

	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar_myadmin),GTK_ICON_SIZE_SMALL_TOOLBAR);

	web_win->entry_myadmin = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(web_win->entry_myadmin));
	gtk_box_pack_start (GTK_BOX (web_win->hbox3), web_win->entry_myadmin, TRUE, TRUE, 0);

	gtk_widget_show (GTK_WIDGET(web_win->webView_w));

	web_win->scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show (GTK_WIDGET(web_win->scrolledWindow));
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(web_win->scrolledWindow),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(web_win->scrolledWindow), GTK_WIDGET(web_win->webView_w));

	gtk_box_pack_start(GTK_BOX(web_win->vbox3), GTK_WIDGET(web_win->scrolledWindow), TRUE, TRUE, 1);

	gtk_widget_show_all(GTK_WIDGET(web_win->window_web)); 

	g_signal_connect ((gpointer) web_win->entry_myadmin, "activate",G_CALLBACK (enter_myweb_win),web_win->webView_w);
	g_signal_connect(web_win->webView_w, "document-load-finished",G_CALLBACK(myadmin_get_url_win), web_win->entry_myadmin);
	g_signal_connect(web_win->webView_w, "new-window-policy-decision-requested",G_CALLBACK(myadmin_new_window), web_win->webView_w);
	g_signal_connect(web_win->webView_w, "create-web-view",G_CALLBACK(web_new_w_click_go), web_win->webView_w);
	g_signal_connect(web_win->webView_w, "download-requested", G_CALLBACK(download_requested_cb), NULL);
	g_signal_connect(web_win->webView_w, "web-view-ready",G_CALLBACK(web_new_w_click), web_win->window_web);
	webkit_web_view_load_uri(web_win->webView_w, uri);

return TRUE;
}

//*********************** AFFICHAGE DUN NAVIGATEUR WEB APRES CLIQUE DROIT
gboolean web_new_w_click(WebKitWebView *web_view,gpointer user_data)
{
	if(web_view==NULL){return FALSE;}
	gtk_widget_show_all (GTK_WIDGET(user_data));
	return TRUE;
}

//*********************** PREPARATION DUN NAVIGATEUR WEB APRES CLIQUE DROIT
WebKitWebView * web_new_w_click_go(WebKitWebView  *web_view, WebKitWebFrame *frame, gpointer user_data)
{

	if(web_view==NULL){return FALSE;}
	if(frame==NULL){return FALSE;}
	if(user_data==NULL){return FALSE;}

	win_web *web_win = (win_web *) g_malloc (sizeof (win_web));

	web_win->window_web = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for(GTK_WINDOW(web_win->window_web),GTK_WINDOW(tea_main_window));
	gtk_window_resize (GTK_WINDOW (web_win->window_web), 770, 400);
	//gtk_window_maximize (GTK_WINDOW(web_win->window_web));

	web_win->vbox3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (web_win->window_web), GTK_WIDGET(web_win->vbox3));

	web_win->hbox3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start (GTK_BOX (web_win->vbox3), web_win->hbox3, FALSE, TRUE, 0);

	web_win->webView_w = WEBKIT_WEB_VIEW(webkit_web_view_new());

	GtkToolItem *tool_sep;
	GtkWidget *toolbar_myadmin;
	toolbar_myadmin = gtk_toolbar_new ();

	GtkToolItem *tool_myadmin_reload = gtk_tool_button_new_from_stock(GTK_STOCK_REFRESH  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_reload, -1);
	g_signal_connect ((gpointer) tool_myadmin_reload, "clicked",G_CALLBACK (myadmin_reload_win),web_win->webView_w);
	gtk_tool_item_set_tooltip_text(tool_myadmin_reload,_("Reload"));

	GtkToolItem *tool_myadmin_back = gtk_tool_button_new_from_stock(GTK_STOCK_UNDO  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_back, -1);
	g_signal_connect ((gpointer) tool_myadmin_back, "clicked",G_CALLBACK (myadmin_back_win),web_win->webView_w);
	gtk_tool_item_set_tooltip_text(tool_myadmin_back,_("Undo"));

	GtkToolItem *tool_myadmin_prev = gtk_tool_button_new_from_stock(GTK_STOCK_REDO  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_prev, -1);
	g_signal_connect ((gpointer) tool_myadmin_prev, "clicked",G_CALLBACK (myadmin_forward_win),web_win->webView_w);
	gtk_tool_item_set_tooltip_text(tool_myadmin_prev,_("Redo"));

	GtkToolItem *tool_myadmin_stop = gtk_tool_button_new_from_stock(GTK_STOCK_STOP  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_stop, -1);
	g_signal_connect ((gpointer) tool_myadmin_stop, "clicked",G_CALLBACK (myadmin_stop_win),web_win->webView_w);
	gtk_tool_item_set_tooltip_text(tool_myadmin_stop,_("Stop"));

	tool_sep=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin ), tool_sep, -1);

	GtkToolItem *tool_myadmin_source = gtk_tool_button_new_from_stock(GTK_STOCK_PROPERTIES   );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_source, -1);
	g_signal_connect ((gpointer) tool_myadmin_source, "clicked",G_CALLBACK (myadmin_source_mode_get_url_win),web_win->webView_w);
	gtk_tool_item_set_tooltip_text(tool_myadmin_source,_("Source view"));

	GtkToolItem *tool_myadmin_view = gtk_tool_button_new_from_stock(GTK_STOCK_CONVERT    );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_view, -1);
	g_signal_connect ((gpointer) tool_myadmin_view, "clicked",G_CALLBACK (myadmin_view_mode_get_url_win),web_win->webView_w);
	gtk_tool_item_set_tooltip_text(tool_myadmin_view,_("Web view"));

	gtk_box_pack_start (GTK_BOX(web_win->hbox3), toolbar_myadmin, TRUE, TRUE, 0);
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_myadmin), GTK_TOOLBAR_ICONS);

	web_win->entry_myadmin = gtk_entry_new ();
	gtk_box_pack_start (GTK_BOX (web_win->hbox3), web_win->entry_myadmin, TRUE, TRUE, 0);

	web_win->scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show (GTK_WIDGET(web_win->scrolledWindow));
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(web_win->scrolledWindow),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(web_win->scrolledWindow), GTK_WIDGET(web_win->webView_w));

	gtk_box_pack_start(GTK_BOX(web_win->vbox3), GTK_WIDGET(web_win->scrolledWindow), TRUE, TRUE, 1);

	g_signal_connect ((gpointer) web_win->entry_myadmin, "activate",G_CALLBACK (enter_myweb_win),web_win->webView_w);
	g_signal_connect(web_win->webView_w, "document-load-finished",G_CALLBACK(myadmin_get_url_win), web_win->entry_myadmin);
	g_signal_connect(web_win->webView_w, "new-window-policy-decision-requested",G_CALLBACK(myadmin_new_window), web_win->webView_w);
	g_signal_connect(web_win->webView_w, "create-web-view",G_CALLBACK(web_new_w_click_go), web_win->webView_w);
	g_signal_connect(web_win->webView_w, "download-requested", G_CALLBACK(download_requested_cb), NULL);
	g_signal_connect(web_win->webView_w, "web-view-ready",G_CALLBACK(web_new_w_click), web_win->window_web);

return web_win->webView_w;

}

//*********************** CREATION TOOLBAR
static GtkWidget* create_hardcoded_toolbar (void)
{
	toolbar = gtk_toolbar_new ();

	GtkToolItem *tool_sep;

	GtkToolItem *tool_new = gtk_tool_button_new_from_stock(GTK_STOCK_NEW);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_new, -1);
	gtk_widget_show(GTK_WIDGET(tool_new));
	g_signal_connect ((gpointer) tool_new, "clicked",G_CALLBACK (file_new),NULL);
	gtk_tool_item_set_tooltip_text(tool_new,(_("Create a new file")));

	tool_sep=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_sep, -1);
	gtk_widget_show(GTK_WIDGET(tool_sep));

	GtkToolItem *tool_open = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_open, -1);
	gtk_widget_show(GTK_WIDGET(tool_open));
	g_signal_connect ((gpointer) tool_open, "clicked",G_CALLBACK (file_open),NULL);
	gtk_tool_item_set_tooltip_text(tool_open,(_("Open a file")));

	tool_sep=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_sep, -1);
	gtk_widget_show(GTK_WIDGET(tool_sep));

	GtkToolItem *tool_save = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_save, -1);
	gtk_widget_show(GTK_WIDGET(tool_save));
	g_signal_connect ((gpointer) tool_save, "clicked",G_CALLBACK (file_save),NULL);
	gtk_tool_item_set_tooltip_text(tool_save,(_("Save the file")));

	GtkToolItem *tool_save_as = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE_AS);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_save_as, -1);
	gtk_widget_show(GTK_WIDGET(tool_save_as));
	g_signal_connect ((gpointer) tool_save_as, "clicked",G_CALLBACK (file_save_as),NULL);
	gtk_tool_item_set_tooltip_text(tool_save_as,(_("Save under another name")));

	tool_sep=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_sep, -1);
	gtk_widget_show(GTK_WIDGET(tool_sep));

	GtkToolItem *tool_pref = gtk_tool_button_new_from_stock(GTK_STOCK_PREFERENCES);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_pref, -1);
	gtk_widget_show(GTK_WIDGET(tool_pref));
	g_signal_connect ((gpointer) tool_pref, "clicked",G_CALLBACK (show_options),NULL);
	gtk_tool_item_set_tooltip_text(tool_pref,(_("Preferences")));

	tool_sep=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_sep, -1);
	gtk_widget_show(GTK_WIDGET(tool_sep));

	GtkToolItem *tool_undo = gtk_tool_button_new_from_stock(GTK_STOCK_UNDO);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_undo, -1);
	gtk_widget_show(GTK_WIDGET(tool_undo));
	g_signal_connect ((gpointer) tool_undo, "clicked",G_CALLBACK (on_mni_undo),NULL);
	gtk_tool_item_set_tooltip_text(tool_undo,(_("Undo")));


	GtkToolItem *tool_redo = gtk_tool_button_new_from_stock(GTK_STOCK_REDO);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_redo, -1);
	gtk_widget_show(GTK_WIDGET(tool_redo));
	g_signal_connect ((gpointer) tool_redo, "clicked",G_CALLBACK (on_mni_redo),NULL);
	gtk_tool_item_set_tooltip_text(tool_redo,(_("Redo")));

	tool_sep=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_sep, -1);
	gtk_widget_show(GTK_WIDGET(tool_sep));

	GtkToolItem *tool_copy = gtk_tool_button_new_from_stock(GTK_STOCK_COPY);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_copy, -1);
	gtk_widget_show(GTK_WIDGET(tool_copy));
	g_signal_connect ((gpointer) tool_copy, "clicked",G_CALLBACK (edit_copy),NULL);
	gtk_tool_item_set_tooltip_text(tool_copy,(_("Copy")));

	GtkToolItem *tool_cut = gtk_tool_button_new_from_stock(GTK_STOCK_CUT);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_cut, -1);
	gtk_widget_show(GTK_WIDGET(tool_cut));
	g_signal_connect ((gpointer) tool_cut, "clicked",G_CALLBACK (edit_cut),NULL);
	gtk_tool_item_set_tooltip_text(tool_cut,(_("Cut")));

	GtkToolItem *tool_past = gtk_tool_button_new_from_stock(GTK_STOCK_PASTE);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_past, -1);
	gtk_widget_show(GTK_WIDGET(tool_past));
	g_signal_connect ((gpointer) tool_past, "clicked",G_CALLBACK (edit_paste),NULL);
	gtk_tool_item_set_tooltip_text(tool_past,(_("Paste")));

	tool_sep=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_sep, -1);
	gtk_widget_show(GTK_WIDGET(tool_sep));

	GtkToolItem *tool_exe = gtk_tool_button_new_from_stock(GTK_STOCK_EXECUTE );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_exe, -1);
	gtk_widget_show(GTK_WIDGET(tool_exe));
	g_signal_connect ((gpointer) tool_exe, "clicked",G_CALLBACK (window_debug),NULL);
	gtk_tool_item_set_tooltip_text(tool_exe,(_("Run/Debug script")));

	tool_sep=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_sep, -1);
	gtk_widget_show(GTK_WIDGET(tool_sep));

	GtkToolItem *tool_book = gtk_tool_button_new_from_stock(GTK_STOCK_UNDERLINE  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_book, -1);
	gtk_widget_show(GTK_WIDGET(tool_book));
	g_signal_connect ((gpointer) tool_book, "clicked",G_CALLBACK (on_format_button_clicked),NULL);
	gtk_tool_item_set_tooltip_text(tool_book,(_("Development of selection and BookMark (F5 to remove)")));
	gtk_tool_button_set_label(GTK_TOOL_BUTTON(tool_book),"+BookMark");

	tool_sep=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_sep, -1);
	gtk_widget_show(GTK_WIDGET(tool_sep));

	GtkToolItem *tool_doc = gtk_tool_button_new_from_stock(GTK_STOCK_ABOUT  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_doc, -1);
	gtk_widget_show(GTK_WIDGET(tool_doc));
	g_signal_connect ((gpointer) tool_doc, "clicked",G_CALLBACK (gen_doc_html),NULL);
	gtk_tool_item_set_tooltip_text(tool_doc,(_("Generating a template code documentation in HTML")));
	gtk_tool_button_set_label(GTK_TOOL_BUTTON(tool_doc),"Doc Gen");

	tool_sep=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_sep, -1);
	gtk_widget_show(GTK_WIDGET(tool_sep));

	GtkToolItem *tool_log = gtk_tool_button_new_from_stock(GTK_STOCK_DIALOG_INFO  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_log, -1);
	gtk_widget_show(GTK_WIDGET(tool_log));
	g_signal_connect ((gpointer) tool_log, "clicked",G_CALLBACK (show_changelogs),NULL);
	gtk_tool_item_set_tooltip_text(tool_log,(_("ChangeLogs for current file")));
	gtk_tool_button_set_label(GTK_TOOL_BUTTON(tool_log),"ChangeLogs");

	tool_sep=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_sep, -1);
	gtk_widget_show(GTK_WIDGET(tool_sep));

	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS); 
	gtk_style_context_add_class (gtk_widget_get_style_context (toolbar),"primary-toolbar");
	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar),GTK_ICON_SIZE_SMALL_TOOLBAR);

	if (confile.do_show_main_toolbar)
		gtk_widget_show (GTK_WIDGET(toolbar));
	else
		gtk_widget_hide (GTK_WIDGET(toolbar));

	return toolbar;
}

//*********************** MAJ STATUSBAR
void statusbar_msg (const gchar *s)
{
  if (s)
     gtk_statusbar_push (GTK_STATUSBAR(statusbar1), gtk_statusbar_get_context_id (GTK_STATUSBAR(statusbar1), s), s);
}

void fill_entities_special_menu (void)
{
  fill_entities ();
  GList *l = ht_to_glist (ht_entities, opt_ht_to_glist_keys);
  l = glist_strings_sort (l);
  build_menu_from_glist (l, mni_insert_char_entities_menu, on_mni_entity);  
  glist_strings_free (l);
}

//*********************** MAIN WINDOW
GtkWidget* create_tea_main_window (void)
{

	GtkCssProvider * css_theme=gtk_css_provider_new();
	GdkDisplay *display = gdk_display_get_default ();
	GdkScreen *screen = gdk_display_get_default_screen (display);
 
	gtk_style_context_add_provider_for_screen(screen,GTK_STYLE_PROVIDER (css_theme),GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	gtk_css_provider_load_from_path (css_theme,"/usr/local/share/griffon/theme/gtk-3.0/gtk.css",NULL);

	confile_reload();
	//*********************** LOAD NOTIFY
	notify_init ("libnotify");

	FILE *fich;
	char carac;
	char mot[100];
	mot[0]='\0';

	//*********************** LOAD THEME
	if(fopen(confile.tea_theme,"r"))
	{
		fich=fopen(confile.tea_theme,"r");
			while ((carac =fgetc(fich)) != EOF)
			{
				if (carac =='\n')
				{
				break;
				}
				else
				{
				strncat(mot,&carac,1);
				}
			}
	fclose(fich);
	}
	else{strcpy(mot, "classic");}


	GtkWidget *button_web_current;
	//*********************** SHORT KEY GROUP
	accel_group = gtk_accel_group_new ();

	//*********************** MAIN WINDOW
	tea_main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_maximize (GTK_WINDOW(tea_main_window));
	gtk_widget_set_name (tea_main_window, "tea_main_window");
	gtk_window_set_title (GTK_WINDOW (tea_main_window), _("Griffon 1.7.2"));

	//*********************** ICON MAIN WINDOW
	gtk_window_set_icon_from_file (GTK_WINDOW(tea_main_window),"/usr/local/share/griffon/images/griffon_button.png",NULL);

	//*********************** MAIN SIGNALE
	g_signal_connect (tea_main_window, "destroy",G_CALLBACK (gtk_main_quit), NULL);
	g_signal_connect ((gpointer) tea_main_window, "destroy_event",G_CALLBACK (gtk_false),NULL);
	g_signal_connect ((gpointer) tea_main_window, "delete_event",G_CALLBACK (on_tea_window_delete_event),tea_main_window);

	//*********************** MAINVBOX
	mainvbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_name (mainvbox, "mainvbox");
	gtk_widget_show (GTK_WIDGET(mainvbox));
	gtk_container_add (GTK_CONTAINER (tea_main_window), GTK_WIDGET(mainvbox));

	//*********************** MAIN MENUBAR
	menubar1 = gtk_menu_bar_new ();

	gtk_widget_set_name (menubar1, "menubar1");
	gtk_widget_show (GTK_WIDGET(menubar1));
	gtk_box_pack_start (GTK_BOX (mainvbox), menubar1, FALSE, FALSE, 0);

	//*********************** MENU FILE
	mni_temp = new_menu_item (_("File"), menubar1, NULL);
	mni_file_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_tof (mni_file_menu);

	mni_temp = new_menu_item (_("New"), mni_file_menu, on_mni_new_file_activate);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_n, GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Crapbook"), mni_file_menu, on_mni_file_crapbook);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_M, GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);

	//mni_temp = new_menu_item (_("Add a bookmark"), mni_file_menu, on_mni_file_add_bookmark);

	mni_temp = new_menu_item (_("Doc Gen"), mni_file_menu, gen_doc_html);
	mni_temp = new_menu_sep (mni_file_menu);

	mni_temp = new_menu_item (_("Open"), mni_file_menu, on_mni_file_open_activate);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_o, GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Last open files"), mni_file_menu, on_mni_load_last_file);

	mni_temp = new_menu_item (_("Open different files"), mni_file_menu, NULL);
	gtk_widget_hide(mni_temp);
	mni_opendifferent_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_item (_("Open a snippet"), mni_opendifferent_menu, on_mni_snippet_file_open_activate);
	mni_temp = new_menu_item (_("Open a template"), mni_opendifferent_menu, on_mni_templ_file_open_activate);
	mni_temp = new_menu_item (_("Open a session file"), mni_opendifferent_menu, on_mni_session_file_open_activate);

	mni_temp = new_menu_sep (mni_file_menu);

	mni_recent_list = new_menu_item (_("Recent file"), mni_file_menu, NULL);
	mni_recent_menu = new_menu_submenu (GTK_WIDGET(mni_recent_list));

	mni_bookmarks_root = new_menu_item (_("BookMark"), mni_file_menu, NULL);
	mni_bookmarks_menu = new_menu_submenu (GTK_WIDGET(mni_bookmarks_root));
	gtk_widget_hide(mni_bookmarks_root);

	mni_sessions = new_menu_item (_("Sessions"), mni_file_menu, NULL); 
	mni_sessions_menu = new_menu_submenu (GTK_WIDGET(mni_sessions));
	gtk_widget_hide(mni_sessions);

	mni_templates = new_menu_item (_("Templates"), mni_file_menu, NULL); 
	mni_templates_menu = new_menu_submenu (GTK_WIDGET(mni_templates));
	gtk_widget_hide(mni_templates);

	mni_temp = new_menu_sep (mni_file_menu);

	mni_temp = new_menu_item (_("Save"), mni_file_menu, on_mni_file_save_activate);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_s, GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Save as"), mni_file_menu, on_mni_file_save_as_activate);

	mni_temp = new_menu_item (_("Save different files"), mni_file_menu, NULL);
	mni_savedifferent_menu = new_menu_submenu (GTK_WIDGET(mni_temp));

//	mni_temp = new_menu_item (_("Save session"), mni_savedifferent_menu, on_mni_file_save_session);

//	mni_temp = new_menu_item (_("Save as a template"), mni_savedifferent_menu, on_mni_file_save_as_template_activate);
//	mni_temp = new_menu_item (_("Save as a snippet"), mni_savedifferent_menu, on_mni_file_save_as_snippet_activate);

	mni_temp = new_menu_item (_("Save Version"), mni_savedifferent_menu, on_mni_file_save_version);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_s, GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Backup"), mni_savedifferent_menu, on_mni_file_backup);

	mni_temp = new_menu_item (_("Back to save"), mni_file_menu, on_mni_file_revert_to_saved);

	mni_temp = new_menu_sep (mni_file_menu);

	mni_temp = new_menu_item (_("Manager configuration file"), mni_file_menu, NULL);
	mni_file_configs_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_item (_("File terminal help custom"), mni_file_configs_menu, on_mni_custom_term_file_open);
	mni_temp = new_menu_item (_("File auto replacement"), mni_file_configs_menu, on_mni_autorep_file_open);
	mni_temp = new_menu_item (_("File autocomp"), mni_file_configs_menu, on_mni_autocomp_file_open);
	mni_temp = new_menu_item (_("main configuration"), mni_file_configs_menu , on_mni_config_open);
	mni_temp = new_menu_item (_("Charsets configuration"), mni_file_configs_menu, on_mni_utils_add_encodings_activate);
	//mni_temp = new_menu_item (_("Configuring the User Menu"), mni_file_configs_menu, on_mni_user_menu_open);
//	mni_temp = new_menu_item (_("Bookmark file"), mni_file_configs_menu, on_mni_file_open_bookmark);
//	mni_temp = new_menu_item (_("Config file for todo list"), mni_file_configs_menu, on_mni_file_todo);
	mni_temp = new_menu_item (_("Configuration file MyAdmin"), mni_file_configs_menu, on_mni_file_myadmin);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_n, GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Reload MyAdmin"), mni_file_configs_menu, myadmin);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_h, GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Preferences"), mni_file_menu, on_mni_show_options);

	mni_temp = new_menu_sep (mni_file_menu);

	mni_temp = new_menu_item (_("Close the file assets"), mni_file_menu, on_mni_file_close_current);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_Q, GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Exit"), mni_file_menu, on_mni_out_activate); 

	//*********************** MENU EDIT
	mni_temp = new_menu_item (_("Edit"), menubar1, NULL);
	mni_edit_menu =  new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_tof (mni_edit_menu);

	mni_temp = new_menu_item (_("Indent"), mni_edit_menu, on_mni_indent);
	mni_temp = new_menu_item (_("Unindent"), mni_edit_menu, on_mni_unindent);

	mni_temp = new_menu_sep (mni_edit_menu);

	mni_temp = new_menu_item (_("Copy"), mni_edit_menu, on_mni_edit_copy);
	mni_temp = new_menu_item (_("Copy all the contents of the file"), mni_edit_menu, on_mni_edit_copy_all);

	mni_temp = new_menu_item (_("Cut"), mni_edit_menu, on_mni_edit_cut);
	mni_temp = new_menu_item (_("Paste"), mni_edit_menu, on_mni_paste);
	mni_temp = new_menu_item (_("Replace with clipboard"), mni_edit_menu,on_mni_edit_replace_with_clipboard);

	mni_temp = new_menu_item (_("Remove"), mni_edit_menu, on_mni_edit_delete);

	mni_temp = new_menu_sep (mni_edit_menu);

	mni_temp = new_menu_item (_("Cut and new file"), mni_edit_menu, on_mni_cut_to_new);
	mni_temp = new_menu_item (_("Copy and new file"), mni_edit_menu, on_mni_copy_to_new);
	mni_temp = new_menu_item (_("New file and paste"), mni_edit_menu, on_mni_paste_to_new);
	mni_temp = new_menu_item (_("Add to Crapbook"), mni_edit_menu, on_mni_edit_append_to_crapbook);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_E, GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	mni_temp = new_menu_sep (mni_edit_menu);

	mni_temp = new_menu_item (_("Select all"), mni_edit_menu, on_mni_edit_select_all);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_A, GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Delete Row"), mni_edit_menu, on_mni_edit_delete_current_line);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_y, GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_sep (mni_edit_menu);

	mni_edit_undo = new_menu_item (_("Undo"), mni_edit_menu, on_mni_undo);
	gtk_widget_add_accelerator (mni_edit_undo, "activate", accel_group,GDK_KEY_Z, GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

	mni_edit_redo = new_menu_item (_("Redo"), mni_edit_menu, on_mni_redo);
	gtk_widget_add_accelerator (mni_edit_redo, "activate", accel_group,GDK_KEY_Z, GDK_CONTROL_MASK | GDK_SHIFT_MASK,GTK_ACCEL_VISIBLE);

	//*********************** MENU FIND
	mni_temp = new_menu_item (_("Find"), menubar1, NULL);

	mni_quest_menu =  new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_tof (mni_quest_menu);

	mni_temp = new_menu_item (_("BookMark"), mni_quest_menu, on_format_button_clicked);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_b, GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Find and Replace window"), mni_quest_menu, on_mni_find_and_replace_wnd);

	mni_temp = new_menu_item (_("Search (in the text box control)"), mni_quest_menu, on_mni_quest_find);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_f, GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Search case insensitive (the text box control)"), mni_quest_menu, on_mni_quest_find_ncase);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_f, GDK_CONTROL_MASK | GDK_SHIFT_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Search the following (text area control)"), mni_quest_menu, on_mni_quest_find_next);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_F3, 0,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Search according to case insensitive (the text box control)"), mni_quest_menu, on_mni_quest_find_next_ncase);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_F3, GDK_SHIFT_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Go to line"), mni_quest_menu, on_mni_goto_line);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_g, GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Replace All (Command Line FIND_TEXT~TEXTE_REMPLACEMENT)"), mni_quest_menu, on_mni_search_repall);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_F1, 0,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Scan local HTML link"), mni_quest_menu, on_mni_get_links );

	mni_temp = new_menu_item (_("Scan HTML image "), mni_quest_menu, on_mni_get_src);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_F7, GDK_SHIFT_MASK,GTK_ACCEL_VISIBLE); 

	//*********************** MENU FUNCTION
	mni_temp = new_menu_item (_("Function"), menubar1, NULL);
	mni_functions_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_tof (mni_functions_menu);


/*	mni_temp = new_menu_item (_("Analyze text UNIversal"), mni_functions_menu, NULL);
	mni_utils_unitaz_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_tof (mni_utils_unitaz_menu);
	mni_temp = new_menu_item (_("UNITAZ sorts with abc"), mni_utils_unitaz_menu, on_mni_tools_unitaz_sort);
	mni_temp = new_menu_item (_("UNITAZ with sorting by account"), mni_utils_unitaz_menu, on_mni_tools_unitaz_sort_by_count);
	mni_temp = new_menu_item (_("UNITAZ with case insensitive sorts with abc"), mni_utils_unitaz_menu, on_mni_tools_unitaz_sort_caseinsens);
	mni_temp = new_menu_item (_("UNITAZ with case insensitive sorts by counting"), mni_utils_unitaz_menu, on_mni_tools_unitaz_sort_by_count_caseinsens);
	mni_temp = new_menu_item (_("UNITAZ plaine"), mni_utils_unitaz_menu, on_mni_tools_unitaz_plain);
	mni_temp = new_menu_item (_("Extract words"), mni_utils_unitaz_menu, on_mni_extract_words);

	mni_temp = new_menu_sep (mni_functions_menu);
*/
	mni_snippets = new_menu_item (_("Snippets"), mni_functions_menu, NULL);
	mni_snippets_menu = new_menu_submenu (GTK_WIDGET(mni_snippets));
	gtk_widget_hide(mni_snippets);

	mni_user_menu = new_menu_item (_("User Menu"), mni_functions_menu, NULL);
	mni_user_menu_menu = new_menu_submenu (GTK_WIDGET(mni_user_menu));
	gtk_widget_hide(mni_user_menu);

//	mni_temp = new_menu_sep (mni_functions_menu);

	mni_temp = new_menu_item (_("Insert"), mni_functions_menu, NULL);
	mni_ins_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_tof (mni_ins_menu);

	mni_temp = new_menu_item (_("Path file"), mni_ins_menu, insert_dialog_path);
	mni_temp = new_menu_item (_("Path directory"), mni_ins_menu, insert_dialog_path_dir);
	mni_temp = new_menu_item (_("Date/Time"), mni_ins_menu, on_mni_funx_ins_time);
	mni_temp = new_menu_item (_("Dump menu"), mni_ins_menu, on_mni_dump_menu);

	mni_temp = new_menu_item (_("Char"), mni_functions_menu, NULL);
	mni_functions_case_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_tof (mni_functions_case_menu);

	mni_temp = new_menu_item (_("Capital"), mni_functions_case_menu, on_mni_case_upcase_activate);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_Up, GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Tiny"), mni_functions_case_menu, on_mni_case_locase_activate);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_Down, GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Capitalize the first letter of each word"), mni_functions_case_menu, on_mni_upcase_each_first_letter_ofw);

	mni_temp = new_menu_item (_("Numbers"), mni_functions_menu, NULL);
	mni_functions_number_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_tof (mni_functions_number_menu);

	mni_temp = new_menu_item (_("Arabic to Roman"), mni_functions_number_menu, on_mni_Functions_number_arabian2roman);
	mni_temp = new_menu_item (_("Hexadécimal au format décimal"), mni_functions_number_menu, on_mni_Functions_number_hex2dec);
	mni_temp = new_menu_item (_("Binary to decimal"), mni_functions_number_menu, on_mni_Functions_number_bin2dec);
	mni_temp = new_menu_item (_("Counter"), mni_functions_number_menu, on_mni_make_numbering);

	mni_temp = new_menu_item (_("Format"), mni_functions_menu, NULL);
	mni_format_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_tof (mni_format_menu);

	mni_temp = new_menu_item (_("Kill formatting"), mni_format_menu, on_mni_kill_formatting);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_D, GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Kill the formatting on all lines"), mni_format_menu, on_mni_kill_formatting_on_each_line);

	mni_temp = new_menu_item (_("Wrapping the first position to"), mni_format_menu, on_mni_wrap_raw);
	mni_temp = new_menu_item (_("Wrapped on the field at the position"), mni_format_menu, on_mni_wrap_on_spaces);

	mni_temp = new_menu_item (_("Kill the spaces at the beginning of each line"), mni_format_menu, on_mni_strings_remove_leading_whitespaces);
	mni_temp = new_menu_item (_("Kill spaces at the end of each line"), mni_format_menu, on_mni_strings_remove_trailing_whitespaces);

	mni_temp = new_menu_item (_("Sort"), mni_functions_menu, NULL);
	mni_sort_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_tof (mni_sort_menu);

	mni_temp = new_menu_item (_("Sort lines"), mni_sort_menu, on_mni_func_strings_sort);
	mni_temp = new_menu_item (_("Sort rows in length"), mni_sort_menu, on_mni_func_strings_sort_by_q);
	mni_temp = new_menu_item (_("Sort rows with box insensetive"), mni_sort_menu, on_mni_sort_case_insensetive);
	mni_temp = new_menu_item (_("Reverse the order of lines"), mni_sort_menu, on_mni_func_strings_rev);

	mni_temp = new_menu_item (_("String"), mni_functions_menu, NULL);
	mni_functions_string_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_tof (mni_functions_string_menu);

	mni_temp = new_menu_item (_("Insert"), mni_functions_string_menu, on_mni_reverse);
	mni_temp = new_menu_item (_("Remove blank lines"), mni_functions_string_menu, on_mni_func_strings_removeblanks);
	mni_temp = new_menu_item (_("Kill Duplicates"), mni_functions_string_menu, on_mni_str_kill_dups);
	mni_temp = new_menu_item (_("Number lines"), mni_functions_string_menu, on_mni_count_string_list);
	mni_temp = new_menu_item (_("Shuffle lines"), mni_functions_string_menu, on_mni_shuffle_strings);
	mni_temp = new_menu_item (_("Convert tabs to spaces"), mni_functions_string_menu, on_mni_tabs_to_spaces);
	mni_temp = new_menu_item (_("Convert spaces to tabs"), mni_functions_string_menu, on_mni_spaces_to_tabs);

	mni_temp = new_menu_item (_("A model apply each line selection (the Ex command line: %s<br>)"), mni_functions_string_menu, on_mni_func_strings_enclose);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_E, GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Filter"), mni_functions_menu, NULL);
	mni_filter_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_tof (mni_filter_menu);

	mni_temp = new_menu_item (_("Kill the lines that contain an expression"), mni_filter_menu, on_mni_func_filter_exclude_from_list);
	mni_temp = new_menu_item (_("Kill all lines except one sentence"), mni_filter_menu, on_mni_func_filter_antiexclude_from_list);

	mni_temp = new_menu_item (_("Kill all lines <N characters"), mni_filter_menu, on_mni_filter_kill_lesser);
	mni_temp = new_menu_item (_("Kill all lines> N characters"), mni_filter_menu, on_mni_filter_kill_more_than);

	mni_temp = new_menu_item (_("End of line"), mni_functions_menu, NULL);
	mni_eol_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_item (_("CR/LF to LF"), mni_eol_menu, on_mni_eol_to_lf);
	mni_temp = new_menu_item (_("LF to CR/LF"), mni_eol_menu, on_mni_eol_to_crlf);

	mni_temp = new_menu_item (_("Search with google text selection tab MyAdmin"), mni_functions_menu, google_search);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_F8, 0,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Google translation with text selection tab MyAdmin : FR to EN"), mni_functions_menu, google_traduction_fr_en);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_F6, 0,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Google translation with text selection tab MyAdmin : EN to FR"), mni_functions_menu, google_traduction_en_fr);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_F7, 0,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Devdocs window"), mni_functions_menu, window_devdocs);

	//*********************** MENU HTML
	mni_temp = new_menu_item (_("Html"), menubar1, NULL);
	mni_markup_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_tof (mni_markup_menu);

	mni_temp = new_menu_item (_("HTML tools"), mni_markup_menu, NULL);
	mni_html_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_tof (mni_html_menu);

	mni_temp = new_menu_item (_("Basic template"), mni_html_menu, on_mni_html_default_template);

	mni_temp = new_menu_item (_("Basic Table"), mni_html_menu, on_mni_html_make_table);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_T, GDK_CONTROL_MASK | GDK_SHIFT_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Remove HTML tags from the selection"), mni_html_menu, on_mni_html_strip_tags);
	mni_temp = new_menu_item (_("Link around the selection"), mni_html_menu, on_mni_html_enclose_link);
	mni_temp = new_menu_item (_("Convert special characters"), mni_html_menu, on_mni_convert_ascii_2_html);
	mni_temp = new_menu_item (_("View the document weight"), mni_html_menu, on_mni_html_calc_weight);

	mni_temp = new_menu_item (_("Insert a DOCTYPE"), mni_markup_menu, NULL);
	mni_insert_doctype_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_tof (mni_insert_doctype_menu);

	mni_temp = new_menu_item ("HTML 4.1 Transitional", mni_insert_doctype_menu, on_mni_insert_doctype);
	mni_temp = new_menu_item ("HTML 4.1 Strict", mni_insert_doctype_menu, on_mni_insert_doctype);
	mni_temp = new_menu_item ("HTML 4.1 Frameset", mni_insert_doctype_menu, on_mni_insert_doctype);
	mni_temp = new_menu_item ("XHTML 1.0 Strict", mni_insert_doctype_menu, on_mni_insert_doctype);
	mni_temp = new_menu_item ("XHTML 1.0 Transitional", mni_insert_doctype_menu, on_mni_insert_doctype);
	mni_temp = new_menu_item ("XHTML 1.0 Frameset", mni_insert_doctype_menu, on_mni_insert_doctype);
	mni_temp = new_menu_item ("WML 1.1", mni_insert_doctype_menu, on_mni_insert_doctype);

	mni_temp = new_menu_item (_("HTML special characters"), mni_markup_menu, NULL);
	mni_insert_char_entities_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_tof (mni_insert_char_entities_menu);

	fill_entities_special_menu ();
	mni_temp = new_menu_sep (mni_markup_menu);


	mni_temp = new_menu_item (_("Preview text selection in a web popup"), mni_markup_menu, preview_web_popup);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_p, GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Preview full text in a web popup"), mni_markup_menu, preview_web_popup_full);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_p, GDK_CONTROL_MASK | GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Bold"), mni_markup_menu, on_mni_Markup_bold_activate);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_B, GDK_CONTROL_MASK | GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Italics"), mni_markup_menu, on_mni_Markup_italic_activate);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_I, GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Underline"), mni_markup_menu, on_mni_Markup_underline_activate);

	mni_temp = new_menu_sep (mni_markup_menu);

	mni_temp = new_menu_item (_("Comment"), mni_markup_menu, on_mni_Markup_comment);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_slash, GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Paragraph"), mni_markup_menu, on_mni_Markup_para_activate);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_P, GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Link"), mni_markup_menu, on_mni_Markup_link_activate);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_L, GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Color"), mni_markup_menu, on_mni_markup_select_color_activate);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_c, GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Image"), mni_markup_menu, on_mni_image_activate);

	mni_temp = new_menu_item (_("Carriage return"), mni_markup_menu, on_mni_Markup_br);

	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_Return, GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item ("Space", mni_markup_menu,on_mni_Markup_nbsp);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_space, GDK_CONTROL_MASK | GDK_SHIFT_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_sep (mni_markup_menu);

	mni_temp = new_menu_item (_("Array elements"), mni_markup_menu, NULL);
	mni_table_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_tof (mni_table_menu);

	mni_temp = new_menu_item ("table", mni_table_menu, on_mni_markup_stuff);
	mni_temp = new_menu_item ("tr", mni_table_menu, on_mni_markup_stuff);
	mni_temp = new_menu_item ("td", mni_table_menu, on_mni_markup_stuff);

	mni_temp = new_menu_item (_("H"), mni_markup_menu, NULL);
	mni_markup_header_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_tof (mni_markup_header_menu);

	gint c;
	gchar *h;

	for (c = 1; c <= 6; c++)
	{
		h = g_strdup_printf ("h%d", c);
		mni_temp = new_menu_item (h, mni_markup_header_menu, on_mni_Markup_header_activate);
		g_free (h);
	}

	mni_temp = new_menu_item (_("Align"), mni_markup_menu, NULL);
	mni_align_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_tof (mni_align_menu);

	mni_temp = new_menu_item ((_("Center")), mni_align_menu, on_mni_markup_align);
	mni_temp = new_menu_item ((_("Left")), mni_align_menu, on_mni_markup_align);
	mni_temp = new_menu_item ((_("Right")), mni_align_menu, on_mni_markup_align);
	mni_temp = new_menu_item ((_("justify")), mni_align_menu, on_mni_markup_align);

	//*********************** MENU PROJET
	mni_ide = new_menu_item (_("Projects"), menubar1, NULL);
	mni_ide_menu = new_menu_submenu (GTK_WIDGET(mni_ide));
	mni_temp = new_menu_tof (mni_ide_menu);

	mni_temp = new_menu_item (_("Make"), mni_ide_menu, window_make_project);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_F11, GDK_SHIFT_MASK,GTK_ACCEL_VISIBLE);
	mni_temp = new_menu_item (_("Run"), mni_ide_menu, window_debug_project);  
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_F12, GDK_SHIFT_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Mount a server SFTP (SSH)"), mni_ide_menu, w_sftp_mount);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_F9, GDK_SHIFT_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Mount a server FTP (FTPFS)"), mni_ide_menu, w_ftp_mount);

	mni_temp = new_menu_item (_("Umount the last mounted server SFTP / FTP"), mni_ide_menu, umount_sftp);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_F10, GDK_SHIFT_MASK,GTK_ACCEL_VISIBLE);

	//*********************** MENU TABS
	mni_tabs = new_menu_item (_("Tabs"), menubar1, NULL);
	mni_tabs_menu = new_menu_submenu (GTK_WIDGET(mni_tabs));

	//*********************** MENU BROWSERS
	mni_temp = new_menu_item (_("Browsers"), menubar1, NULL);
	mni_bro_menu = new_menu_submenu (GTK_WIDGET(mni_temp));

	mni_temp = new_menu_tof (mni_bro_menu);
	mni_temp = new_menu_item ("Opera", mni_bro_menu, on_mni_open_in_browser);
	mni_temp = new_menu_item ("Konqueror", mni_bro_menu, on_mni_open_in_browser);
	mni_temp = new_menu_item ("Firefox", mni_bro_menu, on_mni_open_in_browser);
	mni_temp = new_menu_item ("Mozilla", mni_bro_menu, on_mni_open_in_browser);
	mni_temp = new_menu_item ("Galeon", mni_bro_menu, on_mni_open_in_browser);
	mni_temp = new_menu_item ("Dillo", mni_bro_menu, on_mni_open_in_browser);
	mni_temp = new_menu_item ("Elinks", mni_bro_menu, on_mni_open_in_browser);
	mni_temp = new_menu_item ("Links", mni_bro_menu, on_mni_open_in_browser);
	mni_temp = new_menu_item ("Lynx", mni_bro_menu, on_mni_open_in_browser);
	mni_temp = new_menu_item ("Epiphany", mni_bro_menu, on_mni_open_in_browser);

	//*********************** MENU NAVIGATION
	mni_temp = new_menu_item (_("Navigation"), menubar1, NULL);
	mni_nav_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_tof (mni_nav_menu);

	//mni_temp = new_menu_item (_("Scan links in the page"), mni_nav_menu,scan_links);

	mni_links = new_menu_item (_("Links"), mni_nav_menu, NULL);
	mni_links_menu = new_menu_submenu (GTK_WIDGET(mni_links));

	mni_temp = new_menu_item (_("Go to beginning of block {"), mni_nav_menu, on_mni_nav_block_start);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_Left, GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Go to end of block}"), mni_nav_menu, on_mni_nav_block_end);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_Right, GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Jump to recent tab"), mni_nav_menu, on_mni_nav_goto_recent_tab);

	mni_temp = new_menu_item (_("Go to the next tab"), mni_nav_menu, on_mni_nav_goto_next_tab);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_Page_Up, GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	mni_temp = new_menu_item (_("Go to the previous tab"), mni_nav_menu, on_mni_nav_goto_prev_tab);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_Page_Down, GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Go to first tab"), mni_nav_menu, on_mni_nav_goto_first_tab);
	mni_temp = new_menu_item (_("Go to the last tab"), mni_nav_menu, on_mni_nav_goto_last_tab);
//	mni_temp = new_menu_item (_("Go to the selection"), mni_nav_menu, on_mni_nav_goto_selection);

	mni_temp = new_menu_item (_("Focus on the command line"), mni_nav_menu, on_mni_nav_focus_to_famous);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_W, GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Reload Mini Web"), mni_nav_menu, focus_web);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_R, GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Focus and loading of the current page in the Web tab Mini"), mni_nav_menu, web_current_file);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_D, GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Focus on the text"), mni_nav_menu, on_mni_nav_focus_to_text);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_B, GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

//	mni_temp = new_menu_item (_("Open the cursor"), mni_nav_menu, on_mni_file_open_at_cursor);

//	mni_temp = new_menu_item (_("Edit the cursor"), mni_nav_menu, on_mni_file_edit_at_cursor );

	//*********************** MENU VIEW
	mni_temp = new_menu_item (_("View"), menubar1, NULL);
	mni_view_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_tof (mni_view_menu);

	mni_co = new_menu_item (_("Co"), mni_view_menu, NULL);
	mni_co_menu = new_menu_submenu (GTK_WIDGET(mni_co));

	mni_temp = new_menu_item (_("Syntax Color"), mni_view_menu, NULL);
	mni_hl_mode_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_item (HL_NONE, mni_hl_mode_menu, on_mni_set_hl_mode);
	mni_temp = new_menu_item (HL_MARKUP, mni_hl_mode_menu, on_mni_set_hl_mode);
	mni_temp = new_menu_item (HL_JAVASCRIPT, mni_hl_mode_menu, on_mni_set_hl_mode);
	mni_temp = new_menu_item (HL_XML, mni_hl_mode_menu, on_mni_set_hl_mode);
	mni_temp = new_menu_item (HL_CSS, mni_hl_mode_menu, on_mni_set_hl_mode);
	mni_temp = new_menu_item (HL_PHP, mni_hl_mode_menu, on_mni_set_hl_mode);
	mni_temp = new_menu_item (HL_PERL, mni_hl_mode_menu, on_mni_set_hl_mode);
	mni_temp = new_menu_item (HL_BASH, mni_hl_mode_menu, on_mni_set_hl_mode);
	mni_temp = new_menu_item (HL_C, mni_hl_mode_menu, on_mni_set_hl_mode);
	mni_temp = new_menu_item (HL_CPP, mni_hl_mode_menu, on_mni_set_hl_mode);
	mni_temp = new_menu_item (HL_PO, mni_hl_mode_menu, on_mni_set_hl_mode);
	mni_temp = new_menu_item (HL_PYTHON, mni_hl_mode_menu, on_mni_set_hl_mode);
	mni_temp = new_menu_item (HL_RUBY, mni_hl_mode_menu, on_mni_set_hl_mode);
	mni_temp = new_menu_item (HL_JAVA, mni_hl_mode_menu, on_mni_set_hl_mode);

	mni_temp = new_menu_item (_("Reload syntax color"), mni_view_menu, on_mni_refresh_hl);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_F5, 0,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Wrap"), mni_view_menu, on_mni_view_wrap);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_W, GDK_MOD1_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Display images in the text"), mni_view_menu, on_mni_show_images_in_text);
	mni_temp = new_menu_item (_("Switch between header / source"), mni_view_menu, on_source_header_switch);
	gtk_widget_add_accelerator (mni_temp, "activate", accel_group,GDK_KEY_H, GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Themes"), mni_view_menu, NULL);
	mni_theme_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_item (_("Theme Classic (Default)"), mni_theme_menu, theme_classic  );   
	mni_temp = new_menu_item (_("Theme Cobalt"), mni_theme_menu, theme_cobalt );
	mni_temp = new_menu_item (_("Theme Kate"), mni_theme_menu, theme_kate  );
	mni_temp = new_menu_item (_("Theme Oblivion"), mni_theme_menu, theme_oblivion );
	mni_temp = new_menu_item (_("Theme Tango"), mni_theme_menu, theme_tango  );

	mni_temp = new_menu_item (_("Help"), menubar1, NULL);
	mni_what_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
	mni_temp = new_menu_tof (mni_what_menu);

	mni_temp = new_menu_item (_("Watch news twitter Griffon IDE"), mni_what_menu, window_tweeter_info);
	mni_temp = new_menu_item (_("Check your version of Griffon IDE and update"), mni_what_menu, version_window);    
	mni_temp = new_menu_item (_("Read the manual / documentation Griffon IDE"), mni_what_menu, doc_window);
	mni_temp = new_menu_item (_("Send a bug report"), mni_what_menu, rapport_window);
	mni_temp = new_menu_item (_("About Griffon IDE"), mni_what_menu, create_about1);

	mni_temp = new_menu_sep (mni_what_menu);

	fam_hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(fam_hbox));
	gtk_box_pack_start (GTK_BOX (mainvbox), fam_hbox, FALSE, FALSE, 0);

	vpaned1 = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
	gtk_widget_show (GTK_WIDGET(vpaned1));
	gtk_box_pack_start (GTK_BOX (mainvbox), GTK_WIDGET(vpaned1), TRUE, TRUE, 0);
	gtk_paned_set_position (GTK_PANED (vpaned1), 0);

	hbox2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox2));
	hpaned1 = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_widget_show (GTK_WIDGET(hpaned1));
	gtk_box_pack_start (GTK_BOX (hbox2), hpaned1, TRUE, TRUE, 0);

	//*********************** NOTEBOOK MAIN
	notebook3 = gtk_notebook_new ();  
	gtk_widget_show (GTK_WIDGET(notebook3));  
	gtk_paned_pack1 (GTK_PANED (vpaned1), notebook3, TRUE, TRUE);
	gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook3), GTK_POS_TOP);
	gtk_notebook_set_group_name (GTK_NOTEBOOK (notebook3), "wnote");	

	vbox4 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (notebook3), GTK_WIDGET(vbox4));
	gtk_widget_show (GTK_WIDGET(vbox4));  

	//*********************** NOTEBOOK FILE
	notebook2 = gtk_notebook_new ();  
	gtk_widget_show (GTK_WIDGET(notebook2));  
	gtk_box_pack_start (GTK_BOX (vbox4), notebook2, TRUE, TRUE, 0);  
	gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook2), GTK_POS_TOP);
	gtk_notebook_set_group_name (GTK_NOTEBOOK (notebook2), "wnote");	

	//*********************** ONGLET FILE CHOSE 
	GtkWidget* vbox10 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (notebook2), GTK_WIDGET(vbox10));
	gtk_widget_show (GTK_WIDGET(vbox10));  

	GtkWidget* toolbar_manager2 = gtk_toolbar_new ();
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_manager2), GTK_TOOLBAR_ICONS); 
	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar_manager2),GTK_ICON_SIZE_SMALL_TOOLBAR);

	GtkToolItem *tool_mkdir = gtk_tool_button_new_from_stock(GTK_STOCK_DIRECTORY);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_manager2), tool_mkdir, -1);
	gtk_widget_show(GTK_WIDGET(tool_mkdir));
	g_signal_connect ((gpointer) tool_mkdir, "clicked",G_CALLBACK (new_dir_cmd),NULL);
	gtk_tool_item_set_tooltip_text(tool_mkdir,(_("Create a directory")));

	GtkToolItem *tool_touch = gtk_tool_button_new_from_stock(GTK_STOCK_FILE);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_manager2), tool_touch, -1);
	gtk_widget_show(GTK_WIDGET(tool_touch));
	g_signal_connect ((gpointer) tool_touch, "clicked",G_CALLBACK (new_file_cmd),NULL);
	gtk_tool_item_set_tooltip_text(tool_touch,(_("Create a File")));

	GtkToolItem *item_entry2  = gtk_tool_item_new();

	cmb_famous2 = gtk_entry_new ();     
	ent_search2 = cmb_famous2;
	gtk_widget_show (GTK_WIDGET(cmb_famous2));

	gtk_entry_set_width_chars (GTK_ENTRY(cmb_famous2),50);

	gtk_container_add( GTK_CONTAINER(item_entry2), GTK_WIDGET(cmb_famous2) );
	gtk_toolbar_insert( GTK_TOOLBAR(toolbar_manager2), GTK_TOOL_ITEM(item_entry2), -1 );
	gtk_widget_show (GTK_WIDGET(item_entry2));

	gtk_toolbar_set_show_arrow (GTK_TOOLBAR(toolbar_manager2),FALSE);
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_manager2), GTK_TOOLBAR_ICONS); 
	gtk_box_pack_start (GTK_BOX (vbox10), toolbar_manager2, FALSE , FALSE, 0);
	gtk_widget_show(GTK_WIDGET(toolbar_manager2));

	filechooserwidget2 = gtk_file_chooser_widget_new (GTK_FILE_CHOOSER_ACTION_OPEN);    
	gtk_widget_show (GTK_WIDGET(filechooserwidget2));
	gtk_box_pack_start (GTK_BOX (vbox10), filechooserwidget2, TRUE, TRUE, 0);

	filefilter = NULL;
 
	filefilter = gtk_file_filter_new();
	gtk_file_filter_add_mime_type (GTK_FILE_FILTER(filefilter), "image/png");
	gtk_file_filter_add_mime_type (GTK_FILE_FILTER(filefilter), "image/jpeg");
	gtk_file_filter_add_mime_type (GTK_FILE_FILTER(filefilter), "image/gif");
	gtk_file_filter_add_mime_type (GTK_FILE_FILTER(filefilter), "text/cmd");
	gtk_file_filter_add_mime_type (GTK_FILE_FILTER(filefilter), "text/css");
	gtk_file_filter_add_mime_type (GTK_FILE_FILTER(filefilter), "text/plain");
	gtk_file_filter_add_mime_type (GTK_FILE_FILTER(filefilter), "text/html");
	gtk_file_filter_add_mime_type (GTK_FILE_FILTER(filefilter), "text/csv");
	gtk_file_filter_add_mime_type (GTK_FILE_FILTER(filefilter), "text/xml");

	gtk_file_filter_set_name(filefilter,"Filter");
	//gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(filechooserwidget2), GTK_FILE_FILTER(filefilter));
	gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(filechooserwidget2), GTK_FILE_FILTER(filefilter));
 

	GtkWidget *preview;
	preview = gtk_image_new ();

	gtk_file_chooser_set_preview_widget ((GtkFileChooser *)filechooserwidget2, preview);
	g_signal_connect ((GtkFileChooser *)filechooserwidget2, "update-preview",G_CALLBACK (update_preview_cb), preview);


	g_signal_connect ((gpointer) filechooserwidget2,"file-activated",G_CALLBACK (file_ok_sel),NULL);

	if (confile.use_def_open_dir){gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooserwidget2) ,confile.def_open_dir);}

	label_note4 = gtk_label_new (_("Files"));
	gtk_widget_show (GTK_WIDGET(label_note4));

	gtk_widget_set_size_request (label_note4, 50, 20);

	GtkWidget *hbox_note = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox_note));
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 0), hbox_note);

	GtkWidget *image2 = gtk_image_new_from_stock ("gtk-directory", GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_show (GTK_WIDGET(image2));
	gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox_note), label_note4, TRUE, TRUE, 0);	

	gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 0), TRUE);

	vbox4 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (notebook2), GTK_WIDGET(vbox4));
	gtk_widget_show (GTK_WIDGET(vbox4));  

	search_in = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(search_in));
	gtk_box_pack_start (GTK_BOX (vbox4), search_in, FALSE, FALSE, 0);

	scrolledwindow5 = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (GTK_WIDGET(scrolledwindow5));
	gtk_box_pack_start(GTK_BOX(vbox4), GTK_WIDGET(scrolledwindow5), TRUE, TRUE, 1);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow5), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_placement (GTK_SCROLLED_WINDOW (scrolledwindow5), GTK_CORNER_TOP_LEFT);

	//*********************** ONGLET IN
	GtkTreeModel      *model;
	GtkTreeViewColumn *col, *col2;
	GtkCellRenderer   *renderer;
	GtkTreeSelection *selection_scan;

	model = GTK_TREE_MODEL(create_liststore());

	view_list = gtk_tree_view_new_with_model(model);

	gtk_tree_view_set_enable_search(GTK_TREE_VIEW(view_list),FALSE);
	gtk_tree_view_set_search_equal_func(GTK_TREE_VIEW(view_list),(GtkTreeViewSearchEqualFunc) util_treeview_match_all_words_callback, NULL, NULL);

	gtk_tree_view_set_search_entry((GtkTreeView *)view_list,(GtkEntry *)search_in);

	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, (_("List of include / functions     ")));

	col2 = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col2, "Line");

	renderer = gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_column_pack_start(col, renderer, FALSE);
	gtk_tree_view_column_set_attributes(col, renderer,"pixbuf", COL_ICON,NULL);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_set_attributes(col, renderer,"text", COL_TEXT,NULL);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col2, renderer, TRUE);
	gtk_tree_view_column_set_attributes(col2, renderer,"text", COL_TEXT2,NULL);

	gtk_tree_view_append_column(GTK_TREE_VIEW(view_list), col);
	gtk_tree_view_append_column(GTK_TREE_VIEW(view_list), col2);
	gtk_widget_show(GTK_WIDGET(view_list));
	gtk_container_add (GTK_CONTAINER (scrolledwindow5), GTK_WIDGET(view_list));

	gtk_tree_view_set_grid_lines (GTK_TREE_VIEW(view_list),GTK_TREE_VIEW_GRID_LINES_BOTH);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(view_list),TRUE);

	selection_scan = gtk_tree_view_get_selection(GTK_TREE_VIEW(view_list));

	g_signal_connect(view_list, "button-release-event",G_CALLBACK(on_changed_scan), selection_scan);

	button_include1 = gtk_button_new_with_label (_("Start searching functions and variables"));
	gtk_widget_show(GTK_WIDGET(button_include1));
	gtk_box_pack_start(GTK_BOX(vbox4), button_include1, FALSE, FALSE, 0);

	g_signal_connect ((gpointer) button_include1, "clicked",G_CALLBACK (scan_include),NULL);

	GtkWidget *button_include2;
	button_include2 = gtk_button_new_with_label (_("Open files include"));
	gtk_widget_show(GTK_WIDGET(button_include2));
	gtk_box_pack_start(GTK_BOX(vbox4), button_include2, FALSE, FALSE, 0);

	g_signal_connect ((gpointer) button_include2, "clicked",G_CALLBACK (open_include),NULL);  

	label_note4 = gtk_label_new (_("In"));
	gtk_widget_show (GTK_WIDGET(label_note4));

	//*********************** ONGLET HELP
	gtk_widget_set_size_request (label_note4, 50, 20);

	hbox_note = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox_note));
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 1), hbox_note);

	image2 = gtk_image_new_from_stock ("gtk-properties", GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_show (GTK_WIDGET(image2));
	gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox_note), label_note4, TRUE, TRUE, 0);	

	gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 1), TRUE);

	vbox5 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (notebook2), GTK_WIDGET(vbox5));
	gtk_widget_show (GTK_WIDGET(vbox5));  

	GtkWidget *vbox;
	GtkTreeSelection *selection; 
	GtkWidget *statusbar;
	statusbar = gtk_statusbar_new();

	vbox_help = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(vbox5), GTK_WIDGET(vbox_help));
	gtk_widget_show (GTK_WIDGET(vbox_help));

	view_help = create_view_and_model_help();
	gtk_widget_show (GTK_WIDGET(view_help));
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(view_help));
	gtk_box_pack_start(GTK_BOX(vbox_help), view_help, TRUE, TRUE, 1);

	statusbar_help = gtk_statusbar_new();
	gtk_box_pack_start(GTK_BOX(vbox_help), statusbar_help, FALSE, TRUE, 1);
	gtk_widget_show (GTK_WIDGET(statusbar_help));
	
	g_signal_connect(selection, "changed",  G_CALLBACK(on_changed), statusbar_help);

	label_note5 = gtk_label_new (_("Help"));
	gtk_widget_show (GTK_WIDGET(label_note5));

	GtkWidget* vbox10_help = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (vbox5), GTK_WIDGET(vbox10_help));
	gtk_widget_show (GTK_WIDGET(vbox10_help));  

	GtkWidget* toolbar_manager_help = gtk_toolbar_new ();
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_manager_help), GTK_TOOLBAR_ICONS); 
	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar_manager_help),GTK_ICON_SIZE_SMALL_TOOLBAR);

	GtkToolItem *tool_rmmkdir_help = gtk_tool_button_new_from_stock(GTK_STOCK_REMOVE);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_manager_help), tool_rmmkdir_help, -1);
	gtk_widget_show(GTK_WIDGET(tool_rmmkdir_help));
	g_signal_connect ((gpointer) tool_rmmkdir_help, "clicked",G_CALLBACK (rm_dir_cmd_help),NULL);
	gtk_tool_item_set_tooltip_text(tool_rmmkdir_help,(_("Delete a family assistance")));

	GtkToolItem *tool_mkdir_help = gtk_tool_button_new_from_stock(GTK_STOCK_ADD);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_manager_help), tool_mkdir_help, -1);
	gtk_widget_show(GTK_WIDGET(tool_mkdir_help));
	g_signal_connect ((gpointer) tool_mkdir_help, "clicked",G_CALLBACK (new_dir_cmd_help),NULL);
	gtk_tool_item_set_tooltip_text(tool_mkdir_help,(_("Create a new family assistance")));

	GtkToolItem *item_entry_help  = gtk_tool_item_new();

	cmb_famous_help = gtk_entry_new ();     
	ent_search_help = cmb_famous_help;
	gtk_widget_show (GTK_WIDGET(cmb_famous_help));

	gtk_entry_set_width_chars (GTK_ENTRY(cmb_famous_help),50);

	gtk_container_add( GTK_CONTAINER(item_entry_help), GTK_WIDGET(cmb_famous_help) );
	gtk_toolbar_insert( GTK_TOOLBAR(toolbar_manager_help), GTK_TOOL_ITEM(item_entry_help), -1 );
	gtk_widget_show (GTK_WIDGET(item_entry_help));

	gtk_toolbar_set_show_arrow (GTK_TOOLBAR(toolbar_manager_help),FALSE);
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_manager_help), GTK_TOOLBAR_ICONS); 
	gtk_box_pack_start (GTK_BOX (vbox10_help), toolbar_manager_help, FALSE , FALSE, 0);
	gtk_widget_show(GTK_WIDGET(toolbar_manager_help));

	//*********************** ONGLET SFTP
	gtk_widget_set_size_request (label_note5, 50, 20);

	hbox_note = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox_note));
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 2), hbox_note);

	image2 = gtk_image_new_from_stock ("gtk-help", GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_show (GTK_WIDGET(image2));
	gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox_note), label_note5, TRUE, TRUE, 0);	

	gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 2), TRUE);

	vbox_sftp = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (notebook2), GTK_WIDGET(vbox_sftp));
	gtk_widget_show (GTK_WIDGET(vbox_sftp)); 

	search_sftp = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(search_sftp));
	gtk_box_pack_start (GTK_BOX (vbox_sftp), search_sftp, FALSE, FALSE, 0);

	scrolledWindow_sftp = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show (GTK_WIDGET(scrolledWindow_sftp));
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow_sftp),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(vbox_sftp), scrolledWindow_sftp, TRUE, TRUE, 1);

	view_sftp = create_view_and_model_sftp();
	gtk_widget_show (GTK_WIDGET(view_sftp));
	selection_sftp = gtk_tree_view_get_selection(GTK_TREE_VIEW(view_sftp));
	gtk_container_add(GTK_CONTAINER(scrolledWindow_sftp), GTK_WIDGET(view_sftp));

	g_signal_connect(view_sftp, "button-release-event",G_CALLBACK(on_changed_sftp), selection_sftp);

	GtkWidget *button_vide_sftp = gtk_button_new_with_label (_("Clear history"));
	gtk_widget_show(GTK_WIDGET(button_vide_sftp));
	gtk_box_pack_start(GTK_BOX(vbox_sftp), button_vide_sftp, FALSE, FALSE, 0);

	g_signal_connect ((gpointer) button_vide_sftp, "clicked",G_CALLBACK (vide_configuration_sftp),NULL);

	GtkWidget *button_m_sftp = gtk_button_new_with_label (_("Mount a server"));
	gtk_widget_show(GTK_WIDGET(button_m_sftp));
	gtk_box_pack_start(GTK_BOX(vbox_sftp), button_m_sftp, FALSE, FALSE, 0);

	g_signal_connect ((gpointer) button_m_sftp, "clicked",G_CALLBACK (w_sftp_mount),NULL);

	GtkWidget *button_umount = gtk_button_new_with_label (_("Umount a server"));
	gtk_widget_show(GTK_WIDGET(button_umount));
	gtk_box_pack_start(GTK_BOX(vbox_sftp), button_umount, FALSE, FALSE, 0);

	g_signal_connect ((gpointer) button_umount, "clicked",G_CALLBACK (umount_sftp),NULL);

	label_note6 = gtk_label_new (_("SFTP"));
	gtk_widget_show (GTK_WIDGET(label_note6));

	//*********************** ONGLET FTP	
	gtk_widget_set_size_request (label_note6, 50, 20);

	hbox_note = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox_note));
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 3), hbox_note);

	image2 = gtk_image_new_from_stock ("gtk-network", GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_show (GTK_WIDGET(image2));
	gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox_note), label_note6, TRUE, TRUE, 0);	

	gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 3), TRUE);

	vbox_ftp = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (notebook2), GTK_WIDGET(vbox_ftp));
	gtk_widget_show (GTK_WIDGET(vbox_ftp)); 

	search_ftp = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(search_ftp));
	gtk_box_pack_start (GTK_BOX (vbox_ftp), search_ftp, FALSE, FALSE, 0);

	scrolledWindow_ftp = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show (GTK_WIDGET(scrolledWindow_ftp));
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow_ftp),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(vbox_ftp), scrolledWindow_ftp, TRUE, TRUE, 1);

	view_ftp = create_view_and_model_ftp();
	gtk_widget_show (GTK_WIDGET(view_ftp));
	selection_ftp = gtk_tree_view_get_selection(GTK_TREE_VIEW(view_ftp));
	gtk_container_add(GTK_CONTAINER(scrolledWindow_ftp), GTK_WIDGET(view_ftp));

	g_signal_connect(view_ftp, "button-release-event",G_CALLBACK(on_changed_ftp), selection_ftp);

	GtkWidget *button_vide_ftp = gtk_button_new_with_label (_("Clear history"));
	gtk_widget_show(GTK_WIDGET(button_vide_ftp));
	gtk_box_pack_start(GTK_BOX(vbox_ftp), button_vide_ftp, FALSE, FALSE, 0);

	g_signal_connect ((gpointer) button_vide_ftp, "clicked",G_CALLBACK (vide_configuration_ftp),NULL);

	GtkWidget *button_m_ftp = gtk_button_new_with_label (_("Mount a server"));
	gtk_widget_show(GTK_WIDGET(button_m_ftp));
	gtk_box_pack_start(GTK_BOX(vbox_ftp), button_m_ftp, FALSE, FALSE, 0);

	g_signal_connect ((gpointer) button_m_ftp, "clicked",G_CALLBACK (w_ftp_mount),NULL);

	GtkWidget *button_umount2 = gtk_button_new_with_label (_("Umount a server"));
	gtk_widget_show(GTK_WIDGET(button_umount2));
	gtk_box_pack_start(GTK_BOX(vbox_ftp), button_umount2, FALSE, FALSE, 0);

	g_signal_connect ((gpointer) button_umount2, "clicked",G_CALLBACK (umount_sftp),NULL);

	label_note6 = gtk_label_new (_("FTP"));
	gtk_widget_show (GTK_WIDGET(label_note6));
	gtk_widget_set_size_request (label_note6, 50, 20);

//*********************** ONGLET MARK
	hbox_note = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox_note));
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 4), hbox_note);

	image2 = gtk_image_new_from_stock ("gtk-network", GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_show (GTK_WIDGET(image2));
	gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox_note), label_note6, TRUE, TRUE, 0);	

	gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 4), TRUE);

	GtkWidget* vbox_book = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (notebook2), GTK_WIDGET(vbox_book));
	gtk_widget_show (GTK_WIDGET(vbox_book));  

	GtkWidget* scrolledwindow_book = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (GTK_WIDGET(scrolledwindow_book));
	gtk_box_pack_start(GTK_BOX(vbox_book), GTK_WIDGET(scrolledwindow_book), TRUE, TRUE, 1);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_book), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_placement (GTK_SCROLLED_WINDOW (scrolledwindow_book), GTK_CORNER_TOP_LEFT);

	GtkTreeModel *model_book;
	GtkTreeViewColumn *col_book, *col_book2;
	GtkCellRenderer *renderer_book;

	model_book = GTK_TREE_MODEL(create_liststore_book());

	view_list_book = gtk_tree_view_new_with_model(model_book);

	col_book = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col_book, "   ");

	col_book2 = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col_book2, "Mark");

	renderer = gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_column_pack_start(col_book, renderer, FALSE);
	gtk_tree_view_column_set_attributes(col_book, renderer,"pixbuf", COL_ICON,NULL);

	renderer_book = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col_book, renderer_book, TRUE);
	gtk_tree_view_column_set_attributes(col_book, renderer_book,"text", COL_TEXT,NULL);

	renderer_book = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col_book2, renderer_book, TRUE);
	gtk_tree_view_column_set_attributes(col_book2, renderer_book,"text", COL_TEXT2,NULL);

	gtk_tree_view_append_column(GTK_TREE_VIEW(view_list_book), col_book);
	gtk_tree_view_append_column(GTK_TREE_VIEW(view_list_book), col_book2);
	gtk_widget_show(GTK_WIDGET(view_list_book));
	gtk_container_add (GTK_CONTAINER (scrolledwindow_book), GTK_WIDGET(view_list_book));

	GtkTreeSelection *selection_book;

	gtk_tree_view_set_grid_lines (GTK_TREE_VIEW(view_list_book),GTK_TREE_VIEW_GRID_LINES_BOTH);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(view_list_book),TRUE);

	selection_book = gtk_tree_view_get_selection(GTK_TREE_VIEW(view_list_book));

	g_signal_connect(selection_book, "changed",G_CALLBACK(on_changed_book), NULL);

	GtkWidget *button_del_book = gtk_button_new_with_label (_("Remove BookMark"));
	gtk_widget_show(GTK_WIDGET(button_del_book));
	gtk_box_pack_start(GTK_BOX(vbox_book), button_del_book, FALSE, FALSE, 0);

	g_signal_connect ((gpointer) button_del_book, "clicked",G_CALLBACK (del_book),NULL);

	label_note4 = gtk_label_new (_("Mark"));
	gtk_widget_show (GTK_WIDGET(label_note4));

	gtk_widget_set_size_request (label_note4, 50, 20);

	hbox_note = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox_note));
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 5), hbox_note);

	image2 = gtk_image_new_from_stock ("gtk-jump-to", GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_show (GTK_WIDGET(image2));
	gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox_note), label_note4, TRUE, TRUE, 0);	

	gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 5), TRUE);

	label_note4 = gtk_label_new (_("File"));
	gtk_widget_show (GTK_WIDGET(label_note4));

	gtk_widget_set_size_request (label_note4, 50, 20);

	//*********************** ONGLET EDITOR
	hbox_note = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox_note));
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook3), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook3), 0), hbox_note);

	image2 = gtk_image_new_from_stock ("gtk-directory", GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_show (GTK_WIDGET(image2));
	gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox_note), label_note4, TRUE, TRUE, 0);	

	gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook3), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook3), 0), TRUE);

	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(notebook3), GTK_WIDGET(vbox));
	gtk_widget_show (GTK_WIDGET(vbox));

	hbox_bar = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_pack_start (GTK_BOX (vbox), hbox_bar, FALSE, TRUE, 0);
	gtk_widget_show (GTK_WIDGET(hbox_bar));

	tb_main_toolbar = create_hardcoded_toolbar ();
	gtk_box_pack_start (GTK_BOX (hbox_bar), tb_main_toolbar, FALSE, TRUE, 0);

	hbox_bar2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start (GTK_BOX (hbox_bar), hbox_bar2, FALSE, TRUE, 0);
	gtk_widget_show (GTK_WIDGET(hbox_bar2));

	GtkWidget* toolbar_manager = gtk_toolbar_new ();
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_manager), GTK_TOOLBAR_ICONS); 
	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar_manager),GTK_ICON_SIZE_SMALL_TOOLBAR);

	GtkToolItem *tool_find = gtk_tool_button_new_from_stock(GTK_STOCK_FIND);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_manager), tool_find, -1);
	gtk_widget_show(GTK_WIDGET(tool_find));
	g_signal_connect ((gpointer) tool_find, "clicked",G_CALLBACK (on_mni_quest_find),NULL);
	gtk_tool_item_set_tooltip_text(tool_find,(_("Find")));

	GtkToolItem *tool_find_next = gtk_tool_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_manager), tool_find_next, -1);
	gtk_widget_show(GTK_WIDGET(tool_find_next));
	g_signal_connect ((gpointer) tool_find_next, "clicked",G_CALLBACK (on_mni_quest_find_next),NULL);
	gtk_tool_item_set_tooltip_text(tool_find_next,(_("Find Next")));

	GtkToolItem *tool_find_r = gtk_tool_button_new_from_stock(GTK_STOCK_FIND_AND_REPLACE);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_manager), tool_find_r, -1);
	gtk_widget_show(GTK_WIDGET(tool_find_r));
	g_signal_connect ((gpointer) tool_find_r, "clicked",G_CALLBACK (on_mni_search_repall),NULL);
	gtk_tool_item_set_tooltip_text(tool_find_r,(_("Find~Replace")));

	GtkToolItem *tool_devhelp = gtk_tool_button_new_from_stock(GTK_STOCK_INDEX);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_manager), tool_devhelp, -1);
	gtk_widget_show(GTK_WIDGET(tool_devhelp));
	g_signal_connect ((gpointer) tool_devhelp, "clicked",G_CALLBACK (man_page),NULL);
	gtk_tool_item_set_tooltip_text(tool_devhelp,(_("Search with DevHelp")));

	GtkToolItem *tool_goline = gtk_tool_button_new_from_stock(GTK_STOCK_JUMP_TO);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_manager), tool_goline, -1);
	gtk_widget_show(GTK_WIDGET(tool_goline));
	g_signal_connect ((gpointer) tool_goline, "clicked",G_CALLBACK (on_mni_goto_line),NULL);
	gtk_tool_item_set_tooltip_text(tool_goline,(_("Go To Line")));

	gtk_toolbar_set_show_arrow (GTK_TOOLBAR(toolbar_manager),FALSE);
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_manager), GTK_TOOLBAR_ICONS); 
	gtk_box_pack_start (GTK_BOX (hbox_bar2), toolbar_manager, TRUE , TRUE, 0);
	gtk_widget_show(GTK_WIDGET(toolbar_manager));

	GtkToolItem *tool_sep2;
	tool_sep2=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_manager), tool_sep2, -1);
	gtk_widget_show(GTK_WIDGET(tool_sep2));

	GtkToolItem *item_entry  = gtk_tool_item_new();

	cmb_famous = gtk_entry_new ();
	ent_search = cmb_famous;
	gtk_widget_show (GTK_WIDGET(cmb_famous));

	gtk_entry_set_width_chars (GTK_ENTRY(cmb_famous),50);

	GtkEntryCompletion *completion_entry;
	completion_entry = gtk_entry_completion_new();

	gtk_entry_completion_set_text_column(completion_entry, CONTACT_NAME);
	gtk_entry_set_completion(GTK_ENTRY(cmb_famous), completion_entry);

	model_entry = gtk_list_store_new(1, G_TYPE_STRING);

	gtk_entry_completion_set_model(completion_entry, GTK_TREE_MODEL(model_entry));

	gtk_container_add( GTK_CONTAINER(item_entry), GTK_WIDGET(cmb_famous) );
	gtk_toolbar_insert( GTK_TOOLBAR(toolbar_manager), GTK_TOOL_ITEM(item_entry), -1 );
  gtk_widget_show (GTK_WIDGET(item_entry));

	g_signal_connect ((gpointer) ent_search, "key_press_event",G_CALLBACK (on_ent_search_key_press_event),NULL);
	g_signal_connect ((gpointer) ent_search, "key_release_event",G_CALLBACK (keyrelase_search),NULL);

	tool_sep2=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_manager), tool_sep2, -1);
	gtk_widget_show(GTK_WIDGET(tool_sep2));

	icon_ok = gtk_image_new_from_file("/usr/local/share/griffon/images/griffon_ok.png");
	gtk_widget_set_size_request (icon_ok, 35, 10);
	gtk_box_pack_start (GTK_BOX (hbox_bar2), icon_ok, FALSE, FALSE, 0);
	gtk_widget_show (GTK_WIDGET(icon_ok));

	webView_editor = WEBKIT_WEB_VIEW(webkit_web_view_new());

	scrolledWindow_editor = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show (GTK_WIDGET(scrolledWindow_editor));
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow_editor),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolledWindow_editor), GTK_WIDGET(webView_editor));

	hbox_no = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox_no));
	gtk_box_pack_start (GTK_BOX (vbox), hbox_no, TRUE, TRUE, 0);

	gtk_box_pack_start (GTK_BOX (hbox_no), scrolledWindow_editor, TRUE, TRUE, 0);

	webkit_web_view_load_uri(webView_editor, "http://griffon.lasotel.fr/main.php?version=1.7.2");

	g_signal_connect(webView_editor, "new-window-policy-decision-requested",G_CALLBACK(myadmin_new_window), webView_editor);
	g_signal_connect(webView_editor, "create-web-view",G_CALLBACK(web_new_w_click_go), webView_editor);

	hbox_no2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox_no2));
	gtk_box_pack_start (GTK_BOX (hbox_no), hbox_no2, FALSE, FALSE, 0);

	manager = gtk_recent_manager_get_default ();
	recent_file=gtk_recent_chooser_widget_new_for_manager(manager);

	gtk_box_pack_start (GTK_BOX (hbox_no2), recent_file, FALSE, FALSE, 0);
	gtk_widget_show (GTK_WIDGET(recent_file));

	GtkRecentFilter* filefilter2 = NULL;
 
	filefilter2 = gtk_recent_filter_new ();
	gtk_recent_filter_add_mime_type (GTK_RECENT_FILTER(filefilter2), "text/cmd");
	gtk_recent_filter_add_mime_type (GTK_RECENT_FILTER(filefilter2), "text/css");
	gtk_recent_filter_add_mime_type (GTK_RECENT_FILTER(filefilter2), "text/plain");
	gtk_recent_filter_add_mime_type(GTK_RECENT_FILTER(filefilter2), "text/html");
	gtk_recent_filter_add_mime_type (GTK_RECENT_FILTER(filefilter2), "text/csv");
	gtk_recent_filter_add_mime_type (GTK_RECENT_FILTER(filefilter2), "text/xml");

	gtk_recent_filter_set_name(filefilter2,"Filter");
	//gtk_recent_chooser_add_filter(GTK_RECENT_CHOOSER(recent_file), GTK_RECENT_FILTER(filefilter2));
	gtk_recent_chooser_set_filter(GTK_RECENT_CHOOSER(recent_file), GTK_RECENT_FILTER(filefilter2));

	g_signal_connect ((gpointer) recent_file,"item-activated",G_CALLBACK (file_ok_sel_recent),NULL);

	GtkWidget *label_don2=gtk_link_button_new_with_label("http://griffon.lasotel.fr/donate.html","\nDonate\n"); 
	gtk_widget_show (GTK_WIDGET(label_don2));
	gtk_box_pack_start (GTK_BOX (hbox_no2), label_don2, FALSE, FALSE, 0);

	GtkWidget *image_don; 
	image_don = gtk_image_new_from_file("/usr/local/share/griffon/images/don_griffon.png");
	gtk_widget_show (GTK_WIDGET(image_don));
	gtk_box_pack_start (GTK_BOX (hbox_no2), image_don, FALSE, FALSE, 0);

	GtkWidget *label_don1 = gtk_label_new (_("\n  You can make a donation,\n  to support the development of Griffon IDE.  \n\n  Thank you.\n\n  Files in todo/bug/fix list :\n"));
	gtk_widget_show (GTK_WIDGET(label_don1));
	gtk_box_pack_start (GTK_BOX (hbox_no2), label_don1, FALSE, FALSE, 0);

	combo_todo_main=gtk_combo_box_text_new();
	gtk_box_pack_start (GTK_BOX (hbox_no2), combo_todo_main, FALSE, FALSE, 0);
	gtk_widget_show (GTK_WIDGET(combo_todo_main));

	gchar lecture_combo_main[1024];
	FILE *fichier_combo_main;
	fichier_combo_main = fopen(confile.tea_todo,"rt");
	gchar **a_main;

	if(fichier_combo_main!=NULL)
	{
		while(fgets(lecture_combo_main, 1024, fichier_combo_main))
		{
				a_main = g_strsplit (lecture_combo_main, " ", -1);
				if(a_main[2]!=NULL)
				{
				gtk_combo_box_text_append ((GtkComboBoxText*)combo_todo_main,NULL, a_main[2]);
				}
		}

		fclose(fichier_combo_main);
	}

	g_signal_connect ((gpointer) combo_todo_main, "changed",G_CALLBACK (open_todo_combo_main),NULL);

	notebook1 = gtk_notebook_new ();
	gtk_widget_set_name (notebook1, "notebook1");
	gtk_widget_show (GTK_WIDGET(notebook1));
	gtk_notebook_set_scrollable (GTK_NOTEBOOK (notebook1), TRUE);
	gtk_box_pack_start (GTK_BOX (vbox), notebook1, TRUE, TRUE, 0);
	gtk_notebook_set_group_name (GTK_NOTEBOOK (notebook1), "wnote");

	label_note4 = gtk_label_new (_("Editor"));
	gtk_widget_show (GTK_WIDGET(label_note4));

	gtk_widget_set_size_request (label_note4, 50, 20);

	//*********************** ONGLET TOOLS
	hbox_note = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox_note));
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook3), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook3), 1), hbox_note);

	image2 = gtk_image_new_from_stock ("gtk-edit", GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_show (GTK_WIDGET(image2));
	gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox_note), label_note4, TRUE, TRUE, 0);	

	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(notebook3), GTK_WIDGET(vbox));
	gtk_widget_show (GTK_WIDGET(vbox));

	notebook_down = gtk_notebook_new ();  
	gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook_down), GTK_POS_TOP);
	gtk_widget_show (GTK_WIDGET(notebook_down));  
	gtk_box_pack_start (GTK_BOX (vbox), notebook_down, TRUE, TRUE, 0);

	gtk_notebook_set_group_name (GTK_NOTEBOOK (notebook_down), "wnote");	

	//*********************** ONGLET INFORMATION
	vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_show (GTK_WIDGET(vbox2));
	gtk_container_add (GTK_CONTAINER (notebook_down), GTK_WIDGET(vbox2));

	GtkWidget *toolbar_info;
	toolbar_info = gtk_toolbar_new ();

	GtkToolItem *tool_info_new = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE_AS  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_info ), tool_info_new, -1);
	gtk_widget_show(GTK_WIDGET(tool_info_new));
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_info), GTK_TOOLBAR_ICONS);
	g_signal_connect ((gpointer) tool_info_new, "clicked",G_CALLBACK (new_file_log_edit),NULL);
	gtk_tool_item_set_tooltip_text(tool_info_new,_("Open the logs in a file"));

	GtkToolItem *tool_info_clear = gtk_tool_button_new_from_stock(GTK_STOCK_CLEAR );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_info ), tool_info_clear, -1);
	gtk_widget_show(GTK_WIDGET(tool_info_clear));
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_info), GTK_TOOLBAR_ICONS);
	g_signal_connect ((gpointer) tool_info_clear, "clicked",G_CALLBACK (clear_info),NULL);
	gtk_tool_item_set_tooltip_text(tool_info_clear,_("Clear info logs"));

	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar_info),GTK_ICON_SIZE_SMALL_TOOLBAR);

	gtk_box_pack_start (GTK_BOX (vbox2), toolbar_info, FALSE, FALSE, 0);
	gtk_widget_show(GTK_WIDGET(toolbar_info));

	scrolledwindow3 = gtk_scrolled_window_new (NULL, NULL);
	gtk_box_pack_start (GTK_BOX (vbox2), scrolledwindow3, TRUE, TRUE, 0);

	PangoFontDescription *font_desc_logmemo;
	log_memo_textbuffer = GTK_SOURCE_BUFFER (gtk_source_buffer_new (NULL));
	tv_logmemo= gtk_source_view_new_with_buffer(log_memo_textbuffer);
	font_desc_logmemo = pango_font_description_from_string ("mono 8");
	gtk_widget_modify_font (tv_logmemo, font_desc_logmemo);
	pango_font_description_free (font_desc_logmemo);

	gtk_source_view_set_show_right_margin(GTK_SOURCE_VIEW(tv_logmemo),TRUE);
	gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(tv_logmemo),TRUE);
	gtk_source_view_set_show_line_marks(GTK_SOURCE_VIEW(tv_logmemo),TRUE);

	GtkSourceStyleSchemeManager* sm = gtk_source_style_scheme_manager_new();
	GtkSourceStyleScheme* scheme = gtk_source_style_scheme_manager_get_scheme(sm, mot);
	gtk_source_buffer_set_style_scheme(log_memo_textbuffer  , scheme);

	gtk_container_add (GTK_CONTAINER (scrolledwindow3), GTK_WIDGET(tv_logmemo));
	gtk_widget_show_all (GTK_WIDGET(scrolledwindow3));

	gtk_text_view_set_editable (GTK_TEXT_VIEW(tv_logmemo), FALSE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(tv_logmemo),FALSE);

	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (tv_logmemo), GTK_WRAP_WORD);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow3), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	label_note5 = gtk_label_new (_("Information"));
	gtk_widget_show (GTK_WIDGET(label_note5));

	gtk_widget_set_size_request (label_note5, 100, 20);

	//*********************** ONGLET TERMINAL
	hbox_note =gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox_note));
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook_down), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_down), 0), hbox_note);
  gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook_down), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_down), 0), TRUE);

	image2 = gtk_image_new_from_stock ("gtk-info", GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_show (GTK_WIDGET(image2));
	gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox_note), label_note5, TRUE, TRUE, 0);	

	vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_show (GTK_WIDGET(vbox2));
	gtk_container_add (GTK_CONTAINER (notebook_down), GTK_WIDGET(vbox2));

	GtkWidget *toolbar_term;
	toolbar_term = gtk_toolbar_new ();
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_term), GTK_TOOLBAR_ICONS);
	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar_term),GTK_ICON_SIZE_SMALL_TOOLBAR);

	GtkToolItem *tool_sep;

	GtkToolItem *tool_term_close = gtk_tool_button_new_from_stock(GTK_STOCK_REMOVE  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_term), tool_term_close, -1);
	gtk_widget_show(GTK_WIDGET(tool_term_close));
	g_signal_connect ((gpointer) tool_term_close, "clicked",G_CALLBACK (on_button_close_term),NULL);
	gtk_tool_item_set_tooltip_text(tool_term_close,_("Close Terminal"));

	GtkToolItem *tool_term_add = gtk_tool_button_new_from_stock(GTK_STOCK_ADD  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_term), tool_term_add, -1);
	gtk_widget_show(GTK_WIDGET(tool_term_add));
	g_signal_connect ((gpointer) tool_term_add, "clicked",G_CALLBACK (new_terminal),NULL);
	gtk_tool_item_set_tooltip_text(tool_term_add,_("New Terminal"));

	tool_sep=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_term), tool_sep, -1);
	gtk_widget_show(GTK_WIDGET(tool_sep));

	GtkToolItem *tool_term_back = gtk_tool_button_new_from_stock(GTK_STOCK_GO_BACK  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_term), tool_term_back, -1);
	gtk_widget_show(GTK_WIDGET(tool_term_back));
	g_signal_connect ((gpointer) tool_term_back, "clicked",G_CALLBACK (prev_term),NULL);
	gtk_tool_item_set_tooltip_text(tool_term_back,_("Prev Terminal"));

	GtkToolItem *tool_term_next = gtk_tool_button_new_from_stock(GTK_STOCK_GO_FORWARD  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_term), tool_term_next, -1);
	gtk_widget_show(GTK_WIDGET(tool_term_next));
	g_signal_connect ((gpointer) tool_term_next, "clicked",G_CALLBACK (next_term),NULL);
	gtk_tool_item_set_tooltip_text(tool_term_next,_("Next Terminal"));

	gtk_box_pack_start (GTK_BOX (vbox2), toolbar_term, FALSE, FALSE, 0);
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_term), GTK_TOOLBAR_ICONS);
	gtk_widget_show (GTK_WIDGET(toolbar_term)); 

	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar_term),GTK_ICON_SIZE_SMALL_TOOLBAR);
	notebook_term = gtk_notebook_new ();  
	gtk_widget_show (GTK_WIDGET(notebook_term));  
	gtk_box_pack_start (GTK_BOX (vbox2), notebook_term, TRUE, TRUE, 0); 
	gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook_term), GTK_POS_TOP);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK (notebook_term),TRUE);
	gtk_notebook_set_group_name (GTK_NOTEBOOK (notebook_term), "wnote");
	
	new_terminal ();

	label_note5 = gtk_label_new (_("Terminal"));
	gtk_widget_show (GTK_WIDGET(label_note5));
	gtk_widget_set_size_request (label_note5, 100, 20);

	hbox_note = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox_note));
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook_down), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_down), 1), hbox_note);

	image2 = gtk_image_new_from_stock ("gtk-yes", GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_show (GTK_WIDGET(image2));
	gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox_note), label_note5, TRUE, TRUE, 0);	

	gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook_down), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_down), 1), TRUE);

	//*********************** ONGLET PROJETS
	vbox_proj = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (notebook_down), GTK_WIDGET(vbox_proj));
	gtk_widget_show (GTK_WIDGET(vbox_proj)); 

	vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_show (GTK_WIDGET(vbox2));
	gtk_container_add (GTK_CONTAINER (vbox_proj), GTK_WIDGET(vbox2));

	GtkWidget *toolbar_proj;
	toolbar_proj = gtk_toolbar_new ();
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_proj), GTK_TOOLBAR_ICONS);
	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar_proj),GTK_ICON_SIZE_SMALL_TOOLBAR);

	GtkToolItem *tool_proj_new = gtk_tool_button_new_from_stock(GTK_STOCK_ADD  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_proj), tool_proj_new, -1);
	gtk_widget_show(GTK_WIDGET(tool_proj_new));
	g_signal_connect ((gpointer) tool_proj_new, "clicked",G_CALLBACK (new_project_window),NULL);
	gtk_tool_item_set_tooltip_text(tool_proj_new,_("New project"));

	GtkToolItem *tool_proj_make = gtk_tool_button_new_from_stock(GTK_STOCK_JUMP_TO  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_proj), tool_proj_make, -1);
	gtk_widget_show(GTK_WIDGET(tool_proj_make));
	g_signal_connect ((gpointer) tool_proj_make, "clicked",G_CALLBACK (window_make_project),NULL);
	gtk_tool_item_set_tooltip_text(tool_proj_make,_("Make project"));

	GtkToolItem *tool_proj_run = gtk_tool_button_new_from_stock(GTK_STOCK_EXECUTE  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_proj), tool_proj_run, -1);
	gtk_widget_show(GTK_WIDGET(tool_proj_run));
	g_signal_connect ((gpointer) tool_proj_run, "clicked",G_CALLBACK (window_debug_project),NULL);
	gtk_tool_item_set_tooltip_text(tool_proj_run,_("Run project"));

	gtk_box_pack_start (GTK_BOX (vbox2), toolbar_proj, FALSE, FALSE, 0);
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_proj), GTK_TOOLBAR_ICONS);
	gtk_widget_show (GTK_WIDGET(toolbar_proj)); 

	vbox_proj_main = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_container_add (GTK_CONTAINER (vbox_proj), GTK_WIDGET(vbox_proj_main));
	gtk_widget_show (GTK_WIDGET(vbox_proj_main)); 

	label_note2 = gtk_label_new (_("Projects"));
	gtk_widget_show (GTK_WIDGET(label_note2));
	gtk_widget_set_size_request (label_note2, 100, 20);

	hbox_note = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox_note));
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook_down), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_down), 2), hbox_note);

	image2 = gtk_image_new_from_stock ("gtk-execute", GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_show (GTK_WIDGET(image2));
	gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox_note), label_note2, TRUE, TRUE, 0);	

	gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook_down), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_down), 2), TRUE);

	//*********************** ONGLET NOTE
	vbox3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (notebook_down), GTK_WIDGET(vbox3));
	gtk_widget_show (GTK_WIDGET(vbox3));  

	scrolledwindow4 = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (GTK_WIDGET(scrolledwindow4));
	gtk_box_pack_start(GTK_BOX(vbox3), GTK_WIDGET(scrolledwindow4), TRUE, TRUE, 1);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow4), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_placement (GTK_SCROLLED_WINDOW (scrolledwindow4), GTK_CORNER_TOP_LEFT);

	PangoFontDescription *font_desc_note;
	GtkWidget *sView_note;
	GtkSourceLanguageManager *lm_note;
	GtkSourceLanguage *language_note = NULL;

	buffer_note = GTK_SOURCE_BUFFER (gtk_source_buffer_new (NULL));

	sView_note = gtk_source_view_new_with_buffer(buffer_note);
	font_desc_note = pango_font_description_from_string ("mono 8");
	gtk_widget_modify_font (sView_note, font_desc_note);
	pango_font_description_free (font_desc_note);

	gtk_source_view_set_show_right_margin(GTK_SOURCE_VIEW(sView_note),TRUE);
	gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(sView_note),TRUE);
	gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(sView_note),TRUE);
	gtk_source_view_set_show_line_marks(GTK_SOURCE_VIEW(sView_note),TRUE);

	GtkSourceCompletion *completion = gtk_source_view_get_completion (GTK_SOURCE_VIEW(sView_note));
	create_completion (GTK_SOURCE_VIEW(sView_note), completion);

	lm_note = gtk_source_language_manager_new();
	g_object_ref (lm_note);
	g_object_set_data_full ( G_OBJECT (buffer_note), "languages-manager",lm_note, (GDestroyNotify) g_object_unref);

	lm_note = g_object_get_data (G_OBJECT (buffer_note), "languages-manager");
	language_note = gtk_source_language_manager_get_language (lm_note,"perl");
	gtk_source_buffer_set_language (buffer_note, language_note);

	gtk_source_buffer_set_style_scheme(buffer_note, scheme);

	gtk_container_add (GTK_CONTAINER (scrolledwindow4), GTK_WIDGET(sView_note));
	gtk_widget_show_all (GTK_WIDGET(scrolledwindow4));

	button_note1 = gtk_button_new_with_label (_("Delete"));
	gtk_widget_show(GTK_WIDGET(button_note1));
	gtk_box_pack_start(GTK_BOX(vbox3), button_note1, FALSE, FALSE, 0);

	gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_note), (_("\nTo view notes [ALT+M]\n")), -1);
	gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_note), (_("To integrate a selection in the note  [CTR+E]\n\n____________________________________________________________________________________________________________________________________________________\n\n")), -1);

	label_note3 = gtk_label_new (_("Note"));
	gtk_widget_show (GTK_WIDGET(label_note3));
	gtk_widget_set_size_request (label_note3, 100, 20);

	hbox_note = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox_note));
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook_down), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_down), 3), hbox_note);

	image2 = gtk_image_new_from_stock ("gtk-edit", GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_show (GTK_WIDGET(image2));
	gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox_note), label_note3, TRUE, TRUE, 0);	

	gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook_down), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_down), 3), TRUE);

	//*********************** ONGLET MINIWEB
	GtkWidget *hbox3, *button2;

	vbox3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (notebook_down), GTK_WIDGET(vbox3));
	gtk_widget_show (GTK_WIDGET(vbox3));  

	GtkWidget *toolbar_miniweb;
	toolbar_miniweb = gtk_toolbar_new ();

	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_miniweb), GTK_TOOLBAR_ICONS);
	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar_miniweb),GTK_ICON_SIZE_SMALL_TOOLBAR);

	GtkToolItem *tool_miniweb_reload = gtk_tool_button_new_from_stock(GTK_STOCK_REFRESH  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_miniweb ), tool_miniweb_reload, -1);
	gtk_widget_show(GTK_WIDGET(tool_miniweb_reload));
	g_signal_connect ((gpointer) tool_miniweb_reload, "clicked",G_CALLBACK (miniweb_reload),NULL);
	gtk_tool_item_set_tooltip_text(tool_miniweb_reload,_("Reload"));

	GtkToolItem *tool_miniweb_back = gtk_tool_button_new_from_stock(GTK_STOCK_UNDO );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_miniweb ), tool_miniweb_back, -1);
	gtk_widget_show(GTK_WIDGET(tool_miniweb_back));
	g_signal_connect ((gpointer) tool_miniweb_back, "clicked",G_CALLBACK (miniweb_back),NULL);
	gtk_tool_item_set_tooltip_text(tool_miniweb_back,_("Undo"));

	GtkToolItem *tool_miniweb_prev = gtk_tool_button_new_from_stock(GTK_STOCK_REDO );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_miniweb ), tool_miniweb_prev, -1);
	gtk_widget_show(GTK_WIDGET(tool_miniweb_prev));
	g_signal_connect ((gpointer) tool_miniweb_prev, "clicked",G_CALLBACK (miniweb_forward),NULL);
  gtk_tool_item_set_tooltip_text(tool_miniweb_prev,_("Redo"));

	GtkToolItem *tool_miniweb_stop = gtk_tool_button_new_from_stock(GTK_STOCK_STOP );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_miniweb ), tool_miniweb_stop, -1);
	gtk_widget_show(GTK_WIDGET(tool_miniweb_stop));
	g_signal_connect ((gpointer) tool_miniweb_stop, "clicked",G_CALLBACK (miniweb_stop),NULL);
	gtk_tool_item_set_tooltip_text(tool_miniweb_stop,_("Stop"));

	tool_sep=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_miniweb ), tool_sep, -1);
	gtk_widget_show(GTK_WIDGET(tool_sep));

	GtkToolItem *tool_miniweb_source = gtk_tool_button_new_from_stock(GTK_STOCK_PROPERTIES );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_miniweb ), tool_miniweb_source, -1);
	gtk_widget_show(GTK_WIDGET(tool_miniweb_source));
	g_signal_connect ((gpointer) tool_miniweb_source, "clicked",G_CALLBACK (miniweb_source_mode_get_url),NULL);
	gtk_tool_item_set_tooltip_text(tool_miniweb_source,_("Source View"));

	GtkToolItem *tool_miniweb_view = gtk_tool_button_new_from_stock(GTK_STOCK_CONVERT );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_miniweb ), tool_miniweb_view, -1);
	gtk_widget_show(GTK_WIDGET(tool_miniweb_view));
	g_signal_connect ((gpointer) tool_miniweb_view, "clicked",G_CALLBACK (miniweb_view_mode_get_url),NULL);
	gtk_tool_item_set_tooltip_text(tool_miniweb_view,_("Web view"));

	tool_sep=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_miniweb ), tool_sep, -1);
	gtk_widget_show(GTK_WIDGET(tool_sep));

	GtkToolItem *tool_miniweb_new = gtk_tool_button_new_from_stock(GTK_STOCK_ADD    );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_miniweb), tool_miniweb_new, -1);
	gtk_widget_show(GTK_WIDGET(tool_miniweb_new));
	g_signal_connect ((gpointer) tool_miniweb_new, "clicked",G_CALLBACK (new_web_window_mini),NULL);
	gtk_tool_item_set_tooltip_text(tool_miniweb_new,_("New window Web"));

	tool_sep=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_miniweb ), tool_sep, -1);
	gtk_widget_show(GTK_WIDGET(tool_sep));

	GtkToolItem *tool_mini_find = gtk_tool_button_new_from_stock(GTK_STOCK_GO_FORWARD    );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_miniweb), tool_mini_find, -1);
	gtk_widget_show(GTK_WIDGET(tool_mini_find));
	g_signal_connect ((gpointer) tool_mini_find, "clicked",G_CALLBACK (web_find_mini),NULL);
	gtk_tool_item_set_tooltip_text(tool_mini_find,_("Find next"));

	GtkToolItem *item_entry4  = gtk_tool_item_new();

	entry_find_web_mini = gtk_entry_new ();     
	gtk_widget_show (GTK_WIDGET(entry_find_web_mini));

	g_signal_connect ((gpointer) entry_find_web_mini, "key_release_event",G_CALLBACK (web_find_mini),NULL);

	gtk_entry_set_width_chars (GTK_ENTRY(entry_find_web_mini),50);

	gtk_container_add( GTK_CONTAINER(item_entry4), GTK_WIDGET(entry_find_web_mini) );
	gtk_toolbar_insert( GTK_TOOLBAR(toolbar_miniweb), GTK_TOOL_ITEM(item_entry4), -1 );
	gtk_widget_show (GTK_WIDGET(item_entry4));

	gtk_box_pack_start (GTK_BOX (vbox3), toolbar_miniweb, FALSE, FALSE, 0);
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_miniweb), GTK_TOOLBAR_ICONS);
	gtk_widget_show (GTK_WIDGET(toolbar_miniweb)); 

	hbox3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox3));
	gtk_box_pack_start (GTK_BOX (vbox3), hbox3, FALSE, TRUE, 0);

	entry_web = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_web));
	gtk_box_pack_start (GTK_BOX (hbox3), entry_web, TRUE, TRUE, 0);

	GtkEntryCompletion *completion_entry_http;
	completion_entry_http = gtk_entry_completion_new();

	gtk_entry_completion_set_text_column(completion_entry_http, CONTACT_NAME_HTTP);
	gtk_entry_set_completion(GTK_ENTRY(entry_web), completion_entry_http);

	model_entry_http = gtk_list_store_new(1, G_TYPE_STRING);

	gtk_entry_completion_set_model(completion_entry_http, GTK_TREE_MODEL(model_entry_http));

	gtk_entry_set_text (GTK_ENTRY (entry_web), _("http://griffon.lasotel.fr/main.html"));

	button2 = gtk_button_new_from_stock (" Charger l'URL ");
	gtk_widget_show (GTK_WIDGET(button2));
	gtk_box_pack_start (GTK_BOX (hbox3), button2, FALSE, TRUE, 0);

	button_web_current = gtk_button_new_from_stock ((_(" Loading the current page ")));
	gtk_widget_show (GTK_WIDGET(button_web_current));
	gtk_box_pack_start (GTK_BOX (hbox3), button_web_current, FALSE, TRUE, 0);

	g_signal_connect ((gpointer) button_web_current, "clicked",G_CALLBACK (web_current_file),NULL);

	webView = WEBKIT_WEB_VIEW(webkit_web_view_new());
	gtk_widget_show (GTK_WIDGET(webView));

	g_signal_connect(webView, "new-window-policy-decision-requested",G_CALLBACK(myadmin_new_window), webView);
	g_signal_connect(webView, "download-requested", G_CALLBACK(download_requested_cb), NULL);
	g_signal_connect(webView, "create-web-view",G_CALLBACK(web_new_w_click_go), webView);

	GtkWidget *scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show (GTK_WIDGET(scrolledWindow));
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolledWindow), GTK_WIDGET(webView));

	gtk_box_pack_start(GTK_BOX(vbox3), GTK_WIDGET(scrolledWindow), TRUE, TRUE, 1);

	g_signal_connect ((gpointer) button2, "clicked",G_CALLBACK (focus_web),NULL);
	g_signal_connect ((gpointer) entry_web, "activate",G_CALLBACK (enter_web),entry_web);
	g_signal_connect(webView, "document-load-finished",G_CALLBACK(miniweb_get_url), NULL);

	button_web_image = gtk_button_new_from_stock ((_(" Open with Gimp ")));
	gtk_widget_show (GTK_WIDGET(button_web_image));
	gtk_box_pack_start (GTK_BOX (vbox3), button_web_image, FALSE, TRUE, 0);
	gtk_widget_hide (GTK_WIDGET(button_web_image));

	g_signal_connect ((gpointer) button_web_image, "clicked",G_CALLBACK (open_gimp),NULL);

	button_web_image2 = gtk_button_new_from_stock ((_(" Insert the name of the image file in the current page ")));
	gtk_widget_show (GTK_WIDGET(button_web_image2));
	gtk_box_pack_start (GTK_BOX (vbox3), button_web_image2, FALSE, TRUE, 0);
	gtk_widget_hide (GTK_WIDGET(button_web_image2));

	g_signal_connect ((gpointer) button_web_image2, "clicked",G_CALLBACK (insert_image_web),NULL);

	label_note3 = gtk_label_new (_("Mini Web"));
	gtk_widget_show (GTK_WIDGET(label_note3));
	gtk_widget_set_size_request (label_note3, 100, 20);

	hbox_note = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox_note));
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook_down), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_down), 4), hbox_note);

	image2 = gtk_image_new_from_stock ("gtk-network", GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_show (GTK_WIDGET(image2));
	gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox_note), label_note3, TRUE, TRUE, 0);		

	gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook_down), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_down), 4), TRUE);

	//*********************** ONGLET MYADMIN
	GtkWidget *hbox_myadmin1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (notebook_down), GTK_WIDGET(hbox_myadmin1));
	gtk_widget_show (GTK_WIDGET(hbox_myadmin1)); 

	GtkWidget* notebook_myadmin = gtk_notebook_new ();  
	gtk_widget_show (GTK_WIDGET(notebook_myadmin));  
	gtk_box_pack_start (GTK_BOX (hbox_myadmin1), notebook_myadmin, TRUE, TRUE, 0); 
	gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook_myadmin), GTK_POS_TOP);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK (notebook_myadmin),TRUE);
	gtk_notebook_set_group_name (GTK_NOTEBOOK (notebook_myadmin), "wnote");

	GtkWidget *hbox_myadmin = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (notebook_myadmin), GTK_WIDGET(hbox_myadmin));
	gtk_widget_show (GTK_WIDGET(hbox_myadmin)); 

	GtkWidget *toolbar_myadmin;
	toolbar_myadmin = gtk_toolbar_new ();
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_myadmin), GTK_TOOLBAR_ICONS);

	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar_myadmin),GTK_ICON_SIZE_SMALL_TOOLBAR);

	GtkToolItem *tool_myadmin_reload = gtk_tool_button_new_from_stock(GTK_STOCK_REFRESH  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_reload, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_reload));
	g_signal_connect ((gpointer) tool_myadmin_reload, "clicked",G_CALLBACK (myadmin_reload),NULL);
	gtk_tool_item_set_tooltip_text(tool_myadmin_reload,_("Reload"));

	GtkToolItem *tool_myadmin_back = gtk_tool_button_new_from_stock(GTK_STOCK_UNDO  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_back, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_back));
	g_signal_connect ((gpointer) tool_myadmin_back, "clicked",G_CALLBACK (myadmin_back),NULL);
	gtk_tool_item_set_tooltip_text(tool_myadmin_back,_("Undo"));

	GtkToolItem *tool_myadmin_prev = gtk_tool_button_new_from_stock(GTK_STOCK_REDO  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_prev, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_prev));
	g_signal_connect ((gpointer) tool_myadmin_prev, "clicked",G_CALLBACK (myadmin_forward),NULL);
	gtk_tool_item_set_tooltip_text(tool_myadmin_prev,_("Redo"));

	GtkToolItem *tool_myadmin_stop = gtk_tool_button_new_from_stock(GTK_STOCK_STOP  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_stop, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_stop));
	g_signal_connect ((gpointer) tool_myadmin_stop, "clicked",G_CALLBACK (myadmin_stop),NULL);
	gtk_tool_item_set_tooltip_text(tool_myadmin_stop,_("Stop"));

	tool_sep=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin ), tool_sep, -1);
	gtk_widget_show(GTK_WIDGET(tool_sep));

	GtkToolItem *tool_myadmin_source = gtk_tool_button_new_from_stock(GTK_STOCK_PROPERTIES   );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_source, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_source));
	g_signal_connect ((gpointer) tool_myadmin_source, "clicked",G_CALLBACK (myadmin_source_mode_get_url),NULL);
	gtk_tool_item_set_tooltip_text(tool_myadmin_source,_("Source view"));

	GtkToolItem *tool_myadmin_view = gtk_tool_button_new_from_stock(GTK_STOCK_CONVERT    );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_view, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_view));
	g_signal_connect ((gpointer) tool_myadmin_view, "clicked",G_CALLBACK (myadmin_view_mode_get_url),NULL);
	gtk_tool_item_set_tooltip_text(tool_myadmin_view,_("Web view"));

	GtkToolItem *tool_myadmin_full = gtk_tool_button_new_from_stock(GTK_STOCK_LEAVE_FULLSCREEN    );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_full, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_full));
	g_signal_connect ((gpointer) tool_myadmin_full, "clicked",G_CALLBACK (myadmin_bookmark_on),NULL);
	gtk_tool_item_set_tooltip_text(tool_myadmin_full,_("BookMark ON"));

	GtkToolItem *tool_myadmin_full2 = gtk_tool_button_new_from_stock(GTK_STOCK_FULLSCREEN    );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_full2, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_full2));
	g_signal_connect ((gpointer) tool_myadmin_full2, "clicked",G_CALLBACK (myadmin_bookmark_off),NULL);
	gtk_tool_item_set_tooltip_text(tool_myadmin_full2,_("BookMark OFF"));

	tool_sep=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin ), tool_sep, -1);
	gtk_widget_show(GTK_WIDGET(tool_sep));

	GtkToolItem *tool_myadmin_new = gtk_tool_button_new_from_stock(GTK_STOCK_ADD    );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_new, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_new));
	g_signal_connect ((gpointer) tool_myadmin_new, "clicked",G_CALLBACK (new_web_window),NULL);
	gtk_tool_item_set_tooltip_text(tool_myadmin_new,_("New window Web"));

	tool_sep=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin ), tool_sep, -1);
	gtk_widget_show(GTK_WIDGET(tool_sep));

	GtkToolItem *tool_myadmin_find = gtk_tool_button_new_from_stock(GTK_STOCK_GO_FORWARD    );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_find, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_find));
	g_signal_connect ((gpointer) tool_myadmin_find, "clicked",G_CALLBACK (web_find_myadmin),NULL);
	gtk_tool_item_set_tooltip_text(tool_myadmin_find,_("Find next"));

	GtkToolItem *item_entry3  = gtk_tool_item_new();

	entry_find_web = gtk_entry_new ();     
	gtk_widget_show (GTK_WIDGET(entry_find_web));

	g_signal_connect ((gpointer) entry_find_web, "key_release_event",G_CALLBACK (web_find_myadmin),NULL);

	gtk_entry_set_width_chars (GTK_ENTRY(entry_find_web),50);

	gtk_container_add( GTK_CONTAINER(item_entry3), GTK_WIDGET(entry_find_web) );
	gtk_toolbar_insert( GTK_TOOLBAR(toolbar_myadmin), GTK_TOOL_ITEM(item_entry3), -1 );
	gtk_widget_show (GTK_WIDGET(item_entry3));


	gtk_box_pack_start (GTK_BOX (hbox_myadmin), toolbar_myadmin, FALSE, FALSE, 0);
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_myadmin), GTK_TOOLBAR_ICONS);
	gtk_widget_show (GTK_WIDGET(toolbar_myadmin)); 

	entry_myadmin = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_myadmin));
	gtk_box_pack_start (GTK_BOX (hbox_myadmin), entry_myadmin, FALSE, FALSE, 0);
 
	GtkEntryCompletion *completion_entry_http2;
	completion_entry_http2 = gtk_entry_completion_new();

	gtk_entry_completion_set_text_column(completion_entry_http2, CONTACT_NAME_HTTP2);
	gtk_entry_set_completion(GTK_ENTRY(entry_myadmin), completion_entry_http2);

	model_entry_http2 = gtk_list_store_new(1, G_TYPE_STRING);
 
	gtk_entry_completion_set_model(completion_entry_http2, GTK_TREE_MODEL(model_entry_http2));

	gtk_entry_set_text (GTK_ENTRY (entry_myadmin), _("http://"));

	g_signal_connect ((gpointer) entry_myadmin, "activate",G_CALLBACK (enter_myweb),NULL);

	vbox_myadmin = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start(GTK_BOX(hbox_myadmin), GTK_WIDGET(vbox_myadmin), TRUE, TRUE, 1);

	gtk_widget_show (GTK_WIDGET(vbox_myadmin)); 

	webView_myadmin = WEBKIT_WEB_VIEW(webkit_web_view_new());
	gtk_widget_show (GTK_WIDGET(webView_myadmin));

	view_myadmin = gtk_tree_view_new(); 

	GtkWidget *scrolledWindow_myadmin = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show (GTK_WIDGET(scrolledWindow_myadmin));
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow_myadmin),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolledWindow_myadmin), GTK_WIDGET(webView_myadmin));

	gtk_box_pack_start(GTK_BOX(vbox_myadmin), GTK_WIDGET(scrolledWindow_myadmin), TRUE, TRUE, 1);

	view_a = create_view_and_model_myadmin();
	gtk_widget_show (GTK_WIDGET(view_a));
	selection_myadmin = gtk_tree_view_get_selection(GTK_TREE_VIEW(view_a));
	gtk_box_pack_start(GTK_BOX(vbox_myadmin), view_a, FALSE, TRUE, 1);

	g_signal_connect(selection_myadmin, "changed",G_CALLBACK(on_changed_myadmin), statusbar);  
	g_signal_connect(webView_myadmin, "document-load-finished",G_CALLBACK(myadmin_get_url), NULL);
	g_signal_connect(webView_myadmin, "new-window-policy-decision-requested",G_CALLBACK(myadmin_new_window), webView_myadmin);
	g_signal_connect(webView_myadmin, "create-web-view",G_CALLBACK(web_new_w_click_go), webView_myadmin);
	g_signal_connect(webView_myadmin, "download-requested", G_CALLBACK(download_requested_cb), NULL);

	label_note3 = gtk_label_new (_("Web"));
	gtk_widget_show (GTK_WIDGET(label_note3));

	gtk_widget_set_size_request (label_note3, 150, 20);

	//*********************** ONGLET TRANSLATION
	hbox_note = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox_note));

	image2 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_show (GTK_WIDGET(image2));
	gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox_note), label_note3, TRUE, TRUE, 0);		

	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook_myadmin), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_myadmin), 0), hbox_note);
	gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook_myadmin), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_myadmin), 0), TRUE);

	webView_myadmin_traduc = WEBKIT_WEB_VIEW(webkit_web_view_new());
	gtk_widget_show (GTK_WIDGET(webView_myadmin_traduc));

	GtkWidget *scrolledWindow_myadmin_traduc = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show (GTK_WIDGET(scrolledWindow_myadmin_traduc));
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow_myadmin_traduc),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolledWindow_myadmin_traduc), GTK_WIDGET(webView_myadmin_traduc));

	gtk_container_add(GTK_CONTAINER(notebook_myadmin), GTK_WIDGET(scrolledWindow_myadmin_traduc));  

	g_signal_connect(webView_myadmin_traduc, "new-window-policy-decision-requested",G_CALLBACK(myadmin_new_window), webView_myadmin_traduc);
	g_signal_connect(webView_myadmin_traduc, "download-requested", G_CALLBACK(download_requested_cb), NULL);
	g_signal_connect(webView_myadmin_traduc, "create-web-view",G_CALLBACK(web_new_w_click_go), webView_myadmin_traduc);

	label_note3 = gtk_label_new (_("Translation"));
	gtk_widget_show (GTK_WIDGET(label_note3));

	gtk_widget_set_size_request (label_note3, 150, 20);

	//*********************** ONGLET HELP/SEARCH
	hbox_note = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox_note));

	image2 = gtk_image_new_from_stock ("gtk-convert", GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_show (GTK_WIDGET(image2));
	gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox_note), label_note3, TRUE, TRUE, 0);		

	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook_myadmin), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_myadmin), 1), hbox_note);
	gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook_myadmin), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_myadmin), 1), TRUE);

	GtkWidget *vbox_bar = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (notebook_myadmin), GTK_WIDGET(vbox_bar));
	gtk_widget_show (GTK_WIDGET(vbox_bar));  

	GtkWidget *toolbar_help;
	toolbar_help = gtk_toolbar_new ();

	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_help), GTK_TOOLBAR_ICONS);
	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar_help),GTK_ICON_SIZE_SMALL_TOOLBAR);

	GtkToolItem *tool_help_find = gtk_tool_button_new_from_stock(GTK_STOCK_GO_FORWARD    );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_help), tool_help_find, -1);
	gtk_widget_show(GTK_WIDGET(tool_help_find));
	g_signal_connect ((gpointer) tool_help_find, "clicked",G_CALLBACK (web_find_web_help),NULL);
	gtk_tool_item_set_tooltip_text(tool_help_find,_("Find next"));

	GtkToolItem *item_entry5  = gtk_tool_item_new();

	entry_find_web_help = gtk_entry_new ();     
	gtk_widget_show (GTK_WIDGET(entry_find_web_help));

	g_signal_connect ((gpointer) entry_find_web_help, "key_release_event",G_CALLBACK (web_find_web_help),NULL);

	gtk_entry_set_width_chars (GTK_ENTRY(entry_find_web_help),50);

	gtk_container_add( GTK_CONTAINER(item_entry5), GTK_WIDGET(entry_find_web_help) );
	gtk_toolbar_insert( GTK_TOOLBAR(toolbar_help), GTK_TOOL_ITEM(item_entry5), -1 );
	gtk_widget_show (GTK_WIDGET(item_entry5));

	gtk_box_pack_start (GTK_BOX (vbox_bar), toolbar_help, FALSE, FALSE, 0);
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_help), GTK_TOOLBAR_ICONS);
	gtk_widget_show (GTK_WIDGET(toolbar_help)); 


	webView_myadmin_aide = WEBKIT_WEB_VIEW(webkit_web_view_new());
	gtk_widget_show (GTK_WIDGET(webView_myadmin_aide));

	GtkWidget *scrolledWindow_myadmin_aide = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show (GTK_WIDGET(scrolledWindow_myadmin_aide));
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow_myadmin_aide),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolledWindow_myadmin_aide), GTK_WIDGET(webView_myadmin_aide));

	gtk_box_pack_start (GTK_BOX (vbox_bar), scrolledWindow_myadmin_aide, TRUE, TRUE, 0);

	g_signal_connect(webView_myadmin_aide, "new-window-policy-decision-requested",G_CALLBACK(myadmin_new_window), webView_myadmin_aide);

	g_signal_connect(webView_myadmin_aide, "create-web-view",G_CALLBACK(web_new_w_click_go), webView_myadmin_aide);
	g_signal_connect(webView_myadmin_aide, "document-load-finished",G_CALLBACK(web_find_web_help), webView_myadmin_aide);
	g_signal_connect(webView_myadmin_aide, "download-requested", G_CALLBACK(download_requested_cb), NULL);

	label_note3 = gtk_label_new (_("Help/Search"));
	gtk_widget_show (GTK_WIDGET(label_note3));

	gtk_widget_set_size_request (label_note3, 150, 20);

	hbox_note = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox_note));

	image2 = gtk_image_new_from_stock ("gtk-help", GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_show (GTK_WIDGET(image2));
	gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox_note), label_note3, TRUE, TRUE, 0);		

	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook_myadmin), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_myadmin), 2), hbox_note);
	gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook_myadmin), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_myadmin), 2), TRUE);

	gtk_notebook_set_current_page(GTK_NOTEBOOK (notebook_myadmin),0);

	label_note3 = gtk_label_new (_("MyAdmin"));
	gtk_widget_show (GTK_WIDGET(label_note3));
	gtk_widget_set_size_request (label_note3, 100, 20);

	hbox_note = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox_note));
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook_down), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_down), 5), hbox_note);

	image2 = gtk_image_new_from_stock ("gtk-home", GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_show (GTK_WIDGET(image2));
	gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox_note), label_note3, TRUE, TRUE, 0);		

	gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook_down), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_down), 5), TRUE);

	//*********************** ONGLET TODOLIST
	vbox4 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (notebook_down), GTK_WIDGET(vbox4));
	gtk_widget_show (GTK_WIDGET(vbox4));  

	button_include1 = gtk_button_new_with_label (_("Start searching for words: TODO, FIXME, BUG in the current file (!save the file before!)"));
	gtk_widget_show(GTK_WIDGET(button_include1));
	gtk_box_pack_start(GTK_BOX(vbox4), button_include1, FALSE, FALSE, 0);

	g_signal_connect ((gpointer) button_include1, "clicked",G_CALLBACK (scan_include),NULL);


	scrolledwindow5 = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (GTK_WIDGET(scrolledwindow5));
	gtk_box_pack_start(GTK_BOX(vbox4), GTK_WIDGET(scrolledwindow5), TRUE, TRUE, 1);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow5), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_placement (GTK_SCROLLED_WINDOW (scrolledwindow5), GTK_CORNER_TOP_LEFT);

	GtkTreeModel *model_todo;
	GtkTreeViewColumn *col_todo, *col2_todo;
	GtkCellRenderer   *renderer_todo;
	GtkTreeSelection *selection_scan_todo;

	model_todo = GTK_TREE_MODEL(create_liststore_todo());

	view_list_todo = gtk_tree_view_new_with_model(model_todo);

	col_todo = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col_todo, (_("Todo List")));

	col2_todo = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col2_todo, "Line");

	renderer_todo = gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_column_pack_start(col_todo, renderer_todo, FALSE);
	gtk_tree_view_column_set_attributes(col_todo, renderer_todo,"pixbuf", COL_ICON,NULL);

	renderer_todo = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col_todo, renderer_todo, TRUE);
	gtk_tree_view_column_set_attributes(col_todo, renderer_todo,"text", COL_TEXT,NULL);

	renderer_todo = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col2_todo, renderer_todo, TRUE);
	gtk_tree_view_column_set_attributes(col2_todo, renderer_todo,"text", COL_TEXT2,NULL);

	gtk_tree_view_append_column(GTK_TREE_VIEW(view_list_todo), col_todo);
	gtk_tree_view_append_column(GTK_TREE_VIEW(view_list_todo), col2_todo);
	gtk_widget_show(GTK_WIDGET(view_list_todo));
	gtk_container_add (GTK_CONTAINER (scrolledwindow5), GTK_WIDGET(view_list_todo));

	gtk_tree_view_set_grid_lines (GTK_TREE_VIEW(view_list_todo),GTK_TREE_VIEW_GRID_LINES_BOTH);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(view_list_todo),TRUE);

	selection_scan_todo = gtk_tree_view_get_selection(GTK_TREE_VIEW(view_list_todo));

	g_signal_connect(view_list_todo, "button-release-event",G_CALLBACK(on_changed_scan), selection_scan_todo);

	GtkWidget *hbox_todo = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_container_add (GTK_CONTAINER (vbox4), GTK_WIDGET(hbox_todo));
	gtk_widget_show (GTK_WIDGET(hbox_todo));  

	GtkWidget *icon_bug = gtk_image_new_from_file("/usr/local/share/griffon/images/griffon_bug.png");
	gtk_widget_set_size_request (icon_bug, 35, 10);
	gtk_box_pack_start (GTK_BOX (hbox_todo), icon_bug, FALSE, FALSE, 0);
	gtk_widget_show (GTK_WIDGET(icon_bug));

	GtkWidget *button_com = gtk_button_new_with_label (_("Insert TODO in the current file and todo list"));
	gtk_widget_show(GTK_WIDGET(button_com));
	gtk_box_pack_start(GTK_BOX(hbox_todo), button_com, FALSE, FALSE, 0);

	g_signal_connect ((gpointer) button_com, "clicked",G_CALLBACK (add_todo_com),NULL);

	GtkWidget *icon_bug2 = gtk_image_new_from_file("/usr/local/share/griffon/images/griffon_advance.png");
	gtk_widget_set_size_request (icon_bug2, 35, 10);
	gtk_box_pack_start (GTK_BOX (hbox_todo), icon_bug2, FALSE, FALSE, 0);
	gtk_widget_show (GTK_WIDGET(icon_bug2));

	GtkWidget *button_bug = gtk_button_new_with_label (_("Insert BUG in the current file and todo list"));
	gtk_widget_show(GTK_WIDGET(button_bug));
	gtk_box_pack_start(GTK_BOX(hbox_todo), button_bug, FALSE, FALSE, 0);

	g_signal_connect ((gpointer) button_bug, "clicked",G_CALLBACK (add_todo_bug),NULL);

	GtkWidget *icon_bug3 = gtk_image_new_from_file("/usr/local/share/griffon/images/griffon_exe.png");
	gtk_widget_set_size_request (icon_bug3, 35, 10);
	gtk_box_pack_start (GTK_BOX (hbox_todo), icon_bug3, FALSE, FALSE, 0);
	gtk_widget_show (GTK_WIDGET(icon_bug3));

	GtkWidget *button_fixme = gtk_button_new_with_label (_("Insert FIXME in the current file and todo list"));
	gtk_widget_show(GTK_WIDGET(button_fixme));
	gtk_box_pack_start(GTK_BOX(hbox_todo), button_fixme, FALSE, FALSE, 0);

	g_signal_connect ((gpointer) button_fixme, "clicked",G_CALLBACK (add_todo_fixme),NULL);


	combo_todo=gtk_combo_box_text_new();
	gtk_box_pack_start (GTK_BOX (vbox4), combo_todo, FALSE, FALSE, 0);
	gtk_widget_show (GTK_WIDGET(combo_todo));

	gchar lecture_combo[1024];
	FILE *fichier_combo;
	fichier_combo = fopen(confile.tea_todo,"rt");
	gchar **a;

	if(fichier_combo!=NULL)
	{
		while(fgets(lecture_combo, 1024, fichier_combo))
		{
				a = g_strsplit (lecture_combo, "\n", -1);
				if(a[0]!=NULL)
				{
				gtk_combo_box_text_append ((GtkComboBoxText*)combo_todo,NULL, lecture_combo);
				}
		}

		fclose(fichier_combo);
	}

	g_signal_connect ((gpointer) combo_todo, "changed",G_CALLBACK (open_todo_combo),NULL);

	scrolledwindow4 = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (GTK_WIDGET(scrolledwindow4));
	gtk_box_pack_start(GTK_BOX(vbox4), GTK_WIDGET(scrolledwindow4), TRUE, TRUE, 1);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow4), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_placement (GTK_SCROLLED_WINDOW (scrolledwindow4), GTK_CORNER_TOP_LEFT);

	PangoFontDescription *font_desc_todo;
	GtkWidget *sView_todo;
	GtkSourceLanguageManager *lm_todo;
	GtkSourceLanguage *language_todo = NULL;

	buffer_todo = GTK_SOURCE_BUFFER (gtk_source_buffer_new (NULL));

	sView_todo = gtk_source_view_new_with_buffer(buffer_todo);
	font_desc_todo = pango_font_description_from_string ("mono 8");
	gtk_widget_modify_font (sView_todo, font_desc_todo);
	pango_font_description_free (font_desc_todo);

	gtk_source_view_set_show_right_margin(GTK_SOURCE_VIEW(sView_todo),TRUE);
	gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(sView_todo),TRUE);
	gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(sView_todo),TRUE);
	gtk_source_view_set_show_line_marks(GTK_SOURCE_VIEW(sView_todo),TRUE);

	lm_todo = gtk_source_language_manager_new();
	g_object_ref (lm_todo);
	g_object_set_data_full ( G_OBJECT (buffer_todo), "languages-manager",lm_todo, (GDestroyNotify) g_object_unref);

	lm_todo = g_object_get_data (G_OBJECT (buffer_todo), "languages-manager");
	language_todo = gtk_source_language_manager_get_language (lm_todo,"perl");
	gtk_source_buffer_set_language (buffer_todo, language_todo);

	gtk_source_buffer_set_style_scheme(buffer_todo, scheme);
	gtk_text_view_set_editable ((GtkTextView *)sView_todo, FALSE);
	gtk_text_view_set_cursor_visible((GtkTextView *)sView_todo,FALSE);

	gtk_container_add (GTK_CONTAINER (scrolledwindow4), GTK_WIDGET(sView_todo));
	gtk_widget_show_all (GTK_WIDGET(scrolledwindow4));

	GtkWidget *button_todo = gtk_button_new_with_label (_("DELETE LOGS"));
	gtk_widget_show(GTK_WIDGET(button_todo));
	gtk_box_pack_start(GTK_BOX(vbox4), button_todo, FALSE, FALSE, 0);

	g_signal_connect ((gpointer) button_todo, "clicked",G_CALLBACK (delete_todo),NULL);


	label_note3 = gtk_label_new (_("Todo list"));
	gtk_widget_show (GTK_WIDGET(label_note3));
	gtk_widget_set_size_request (label_note3, 100, 20);

	hbox_note = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox_note));
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook_down), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_down), 6), hbox_note);

	image2 = gtk_image_new_from_stock ("gtk-yes", GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_show (GTK_WIDGET(image2));
	gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox_note), label_note3, TRUE, TRUE, 0);		

	gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook_down), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_down), 6), TRUE);

	label_note4 = gtk_label_new (_("Tools"));
	gtk_widget_show (GTK_WIDGET(label_note4));

	gtk_widget_set_size_request (label_note4, 50, 20);

	hbox_note = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox_note));
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook3), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook3), 2), hbox_note);

	image2 = gtk_image_new_from_stock ("gtk-properties", GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_show (GTK_WIDGET(image2));
	gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox_note), label_note4, TRUE, TRUE, 0);	

	gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook3), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook3), 2), TRUE);

	statusbar1 = gtk_statusbar_new ();
	gtk_box_pack_start (GTK_BOX (mainvbox), statusbar1, FALSE, FALSE, 0);
	gtk_widget_show (GTK_WIDGET(statusbar1));

	win_tips_autocomp=NULL;

	g_signal_connect ((gpointer) notebook1, "switch_page",G_CALLBACK (on_notebook1_switch_page),NULL);
	g_signal_connect ((gpointer) notebook1, "focus-in-event",G_CALLBACK (switch_filechooser),NULL);

	gtk_window_add_accel_group (GTK_WINDOW (tea_main_window), accel_group);

	gtk_text_buffer_create_tag (GTK_TEXT_BUFFER(log_memo_textbuffer), "lm_greet","foreground", "#6c0606", NULL);

	gtk_text_buffer_create_tag (GTK_TEXT_BUFFER(log_memo_textbuffer), "gray_bg","background", "gray", NULL);

	tag_lm_error = gtk_text_buffer_create_tag (GTK_TEXT_BUFFER(log_memo_textbuffer), "lm_error","foreground", "red", NULL);

	gtk_text_buffer_create_tag (GTK_TEXT_BUFFER(log_memo_textbuffer), "lm_normal","foreground", "gray", NULL);

	gtk_text_buffer_create_tag (GTK_TEXT_BUFFER(log_memo_textbuffer), "lm_advice","foreground", "navy", NULL);

	set_lm_colors ();
	widget_apply_colors (tv_logmemo);

	GdkPixbuf *pixbuf;
	GtkTextBuffer* buffer_img;
	GtkTextIter itFin;
	pixbuf = gdk_pixbuf_new_from_file("/usr/local/share/griffon/images/griffon_start.png", NULL);
	buffer_img = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv_logmemo));
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer_img), &itFin);
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer_img), &itFin);
	gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(buffer_img),&itFin,pixbuf);
	tv_logmemo_set_pos (0);
	log_to_memo ("\n\n(C)2014 Philippe Muller <pmuller@lasotel.fr>\n Griffon 1.7.2 - http://griffon.lasotel.fr\n\n", NULL, LM_GREET); 
	tv_logmemo_set_pos (0);
	tv_logmemo_set_pos (0);
	ui_init ();

	g_signal_connect ((gpointer) button_note1, "clicked",G_CALLBACK (clear_note),NULL);
	g_signal_connect (notebook_down, "create-window",G_CALLBACK (window_creation_function), NULL);
	g_signal_connect (notebook2, "create-window",G_CALLBACK (window_creation_function), NULL);
	g_signal_connect (notebook_myadmin, "create-window",G_CALLBACK (window_creation_function), NULL);
	g_signal_connect (notebook_term, "create-window",G_CALLBACK (window_creation_function), NULL);
	g_signal_connect (notebook3, "create-window",G_CALLBACK (window_creation_function), NULL);

	gtk_notebook_set_current_page(GTK_NOTEBOOK (notebook_down),0);
	gtk_notebook_set_current_page(GTK_NOTEBOOK (notebook2),0);
	no_onglet_open() ;  

	webkit_web_view_load_uri(webView, "http://griffon.lasotel.fr/main.html");
	webkit_web_view_load_uri(webView_myadmin, "http://griffon.lasotel.fr/en/");
	webkit_web_view_load_uri(webView_myadmin_traduc, "https://translate.google.fr/?hl=fr&tab=wT");
	webkit_web_view_load_uri(webView_myadmin_aide, "https://www.google.fr/");
	gtk_notebook_set_current_page(GTK_NOTEBOOK (notebook3),1);

	window_run = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	back_history ();
	save_string_to_file_vide(confile.tea_cmd_history,"");
	save_string_to_file_vide(confile.file_tmp,"");
	on_mni_file_crapbook ();
	on_mni_file_todolist ();
	load_projects_list();

	window1_popup_line = gtk_window_new (GTK_WINDOW_POPUP);
	gtk_window_set_title (GTK_WINDOW (window1_popup_line), _((_("Web preview"))));
	gtk_window_set_position (GTK_WINDOW (window1_popup_line), GTK_WIN_POS_CENTER);
	gtk_window_resize (GTK_WINDOW (window1_popup_line), 300, 100);

	GtkWidget *vbox1;

	vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_show (GTK_WIDGET(vbox1));
	gtk_container_add (GTK_CONTAINER (window1_popup_line), GTK_WIDGET(vbox1));

	GtkWidget *scrolledwindow5 = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (GTK_WIDGET(scrolledwindow5));
	gtk_box_pack_start(GTK_BOX(vbox1), GTK_WIDGET(scrolledwindow5), TRUE, TRUE, 1);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow5), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_placement (GTK_SCROLLED_WINDOW (scrolledwindow5), GTK_CORNER_TOP_LEFT);

	webView_doc_line = WEBKIT_WEB_VIEW(webkit_web_view_new());
	gtk_widget_show (GTK_WIDGET(webView_doc_line));

	gtk_container_add(GTK_CONTAINER(scrolledwindow5), GTK_WIDGET(webView_doc_line));

	g_signal_connect (G_OBJECT (tea_main_window), "set-focus",
                    G_CALLBACK (hidden_popup), NULL);

	g_signal_connect (G_OBJECT (tea_main_window), "set-focus",
                    G_CALLBACK (window_popup_delete), NULL);

	return tea_main_window;
}

//*********************** ICON STATUS
GtkWidget* icon_affiche_ok (void)
{
	gtk_widget_destroy(icon_ok);
	icon_ok = gtk_image_new_from_file("/usr/local/share/griffon/images/griffon_ok.png");
	gtk_widget_set_size_request (icon_ok, 35, 10);
	gtk_box_pack_start (GTK_BOX (hbox_bar2), icon_ok, FALSE, FALSE, 0);
	gtk_widget_show (GTK_WIDGET(icon_ok));
	return window_center;
}

GtkWidget* icon_affiche_stop (void)
{
	gtk_widget_destroy(icon_ok);
	icon_ok = gtk_image_new_from_file("/usr/local/share/griffon/images/griffon_stop.png");
	gtk_widget_set_size_request (icon_ok, 35, 10);
	gtk_box_pack_start (GTK_BOX (hbox_bar2), icon_ok, FALSE, FALSE, 0);
	gtk_widget_show (GTK_WIDGET(icon_ok));
	return window_center;
}

GtkWidget* icon_affiche_net (void)
{
	gtk_widget_destroy(icon_ok);
	icon_ok = gtk_image_new_from_file("/usr/local/share/griffon/images/griffon_net.png");
	gtk_widget_set_size_request (icon_ok, 35, 10);
	gtk_box_pack_start (GTK_BOX (hbox_bar2), icon_ok, FALSE, FALSE, 0);
	gtk_widget_show (GTK_WIDGET(icon_ok));
	return window_center;
}

GtkWidget* icon_affiche_bug (void)
{
	gtk_widget_destroy(icon_ok);
	icon_ok = gtk_image_new_from_file("/usr/local/share/griffon/images/griffon_advance.png");
	gtk_widget_set_size_request (icon_ok, 35, 10);
	gtk_box_pack_start (GTK_BOX (hbox_bar2), icon_ok, FALSE, FALSE, 0);
	gtk_widget_show (GTK_WIDGET(icon_ok));
	return window_center;
}

GtkWidget* icon_affiche_save (void)
{
	gtk_widget_destroy(icon_ok);
	icon_ok = gtk_image_new_from_file("/usr/local/share/griffon/images/griffon_save.png");
	gtk_widget_set_size_request (icon_ok, 35, 10);
	gtk_box_pack_start (GTK_BOX (hbox_bar2), icon_ok, FALSE, FALSE, 0);
	gtk_widget_show (GTK_WIDGET(icon_ok));
	save_controle=1;
	controle_save_page_icon();
	return window_center;
}

GtkWidget* icon_affiche_nosave (void)
{
	gtk_widget_destroy(icon_ok);
	icon_ok = gtk_image_new_from_file("/usr/local/share/griffon/images/griffon_nosave.png");
	gtk_widget_set_size_request (icon_ok, 35, 10);
	gtk_box_pack_start (GTK_BOX (hbox_bar2), icon_ok, FALSE, FALSE, 0);
	gtk_widget_show (GTK_WIDGET(icon_ok));
	controle_save_page_icon_no();
	return window_center;
}

//*********************** RECHERCHE DEVHALP
GtkWidget* man_page (void)
{
	gchar *titre;
	char mot[100];

	titre = gtk_editable_get_chars(GTK_EDITABLE(ent_search),0, -1);

	FILE *fichier = NULL;
	fichier = fopen("/usr/bin/devhelp",  "r");

	if (fichier == NULL)
		{
		icon_stop_logmemo();
		log_to_memo (_("You must install the DevHelp"), NULL, LM_ERROR);
		statusbar_msg (_("Search DevHelp [ERROR]"));
		}
	else
		{
		fclose(fichier); 
		strcpy(mot,"devhelp -s ");
		strcat(mot,titre);
		strcat(mot," &");

		int systemRet =system (mot);
		if(systemRet == -1){return NULL;}
		log_to_memo (_("Search DevHelp %s"), titre, LM_NORMAL);
		statusbar_msg (_("Search DevHelp [OK]"));
		fclose(fichier);
		}
	return NULL;
}	

//*********************** AFFICHER PARTIE DROITE ET GAUCHE
GtkWidget* open_tools (void)
{
	gtk_widget_show(GTK_WIDGET(notebook2));
	gtk_widget_show(GTK_WIDGET(filechooserwidget2));
	return NULL;
}

//*********************** CACHER PARTIE DROITE ET GAUCHE
GtkWidget* close_tools (void)
{
	gtk_widget_hide(GTK_WIDGET(notebook2));
	gtk_widget_hide(GTK_WIDGET(filechooserwidget2));
	return NULL;
}

//*********************** CACHER PARTIE DROITE
GtkWidget* close_tools2 (void)
{
	gtk_widget_hide(GTK_WIDGET(notebook2));
	gtk_widget_show(GTK_WIDGET(filechooserwidget2));
	return NULL;
}

//*********************** EFFACER LES NOTES
GtkWidget* clear_note (void)
{
	save_string_to_file_vide(confile.crapbook_file,"");
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(buffer_note), "", -1);
	gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_note), (_("\nTo view notes [ALT+M]\n")), -1);
	gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_note), (_("To integrate a selection in the note  [CTR+E]\n\n____________________________________________________________________________________________________________________________________________________\n\n")), -1);
	return NULL;
}

//*********************** FICHIER SELECTIONNE
GtkWidget* file_ok_sel (void)
{
	gchar *file_selected;
	file_selected = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER( filechooserwidget2));
	doc_open_file (file_selected);
	return NULL;
}

//*********************** FICHIER SELECTIONNE RECENT
GtkWidget* file_ok_sel_recent (void)
{
	gchar *file_selected;
	file_selected = gtk_recent_chooser_get_current_uri (GTK_RECENT_CHOOSER( recent_file));
	gchar *file_selected2=g_filename_from_uri(file_selected,NULL,NULL);

	doc_open_file (file_selected2);
	return NULL;
}

//*********************** WINDOW A PROPOS DE GRIFFON
GtkWidget* create_about1 (void)
{
	GtkWidget *window1;
	GtkWidget *vbox1;
	GtkWidget *image1;
	GtkWidget *label1;
	GtkWidget *button1;
	GtkWidget *alignment1;
	GtkWidget *hbox1;
	GtkWidget *image2;
	GtkWidget *label2;

	window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for(GTK_WINDOW(window1),GTK_WINDOW(tea_main_window));
	gtk_window_set_title (GTK_WINDOW (window1), _((_("A-propos de Griffon IDE"))));
	gtk_widget_show (GTK_WIDGET(window1));

	vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_show (GTK_WIDGET(vbox1));
	gtk_container_add (GTK_CONTAINER (window1), GTK_WIDGET(vbox1));

	image1 = gtk_image_new_from_file("/usr/local/share/griffon/images/splash_griffon.png");
	gtk_widget_show (GTK_WIDGET(image1));
	gtk_box_pack_start (GTK_BOX (vbox1), image1, TRUE, TRUE, 0);

	label1 = gtk_label_new (_("\n    Griffon IDE 1.7.2 \n\n    Auteur \t\t: Philippe Muller \n    Email \t\t: pmuller@lasotel.fr\n\n    Developer and Administrator GNU/Linux system Lasotel (Lyon).    \n\n"));
	gtk_widget_show (GTK_WIDGET(label1));
	gtk_box_pack_start (GTK_BOX (vbox1), label1, FALSE, FALSE, 0);

	label2=gtk_link_button_new_with_label("http://griffon.lasotel.fr","http://griffon.lasotel.fr\n"); 
	gtk_widget_show (GTK_WIDGET(label2));
	gtk_box_pack_start (GTK_BOX (vbox1), label2, FALSE, FALSE, 0);

	button1 = gtk_button_new ();
	gtk_widget_show (GTK_WIDGET(button1));
	gtk_box_pack_start (GTK_BOX (vbox1), button1, FALSE, FALSE, 0);

	alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
	gtk_widget_show (GTK_WIDGET(alignment1));
	gtk_container_add (GTK_CONTAINER (button1), GTK_WIDGET(alignment1));

	hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox1));
	gtk_container_add (GTK_CONTAINER (alignment1), GTK_WIDGET(hbox1));

	image2 = gtk_image_new_from_stock ("gtk-close", GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_show (GTK_WIDGET(image2));
	gtk_box_pack_start (GTK_BOX (hbox1), image2, FALSE, FALSE, 0);

	label2 = gtk_label_new_with_mnemonic (_("Close"));
	gtk_widget_show (GTK_WIDGET(label2));
	gtk_box_pack_start (GTK_BOX (hbox1), label2, FALSE, FALSE, 0);

	g_signal_connect_swapped ((gpointer) button1, "clicked",G_CALLBACK (gtk_widget_destroy),window1);

	return window1;
}

GtkWidget* create_custom_win (void)
{
	gchar *titre;  
	titre = gtk_editable_get_chars(GTK_EDITABLE(entry_help_custom),0, -1);
	doc_insert_at_cursor (cur_text_doc, titre);	 
	return entry_custom;
}

//*********************** TREEVIEW 
enum
{
  COLUMN = 0,
  NUM_COLS
} ;

//*********************** TREEVIEW HELP
static GtkTreeModel * create_and_fill_model_help (void)
{
	GtkTreeStore *treestore;
	GtkTreeIter toplevel, child;

	treestore = gtk_tree_store_new(NUM_COLS,G_TYPE_STRING);

	gtk_tree_store_append(treestore, &toplevel, NULL);
	gtk_tree_store_set(treestore, &toplevel,COLUMN, (_("Interpreted languages")),-1);

	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,COLUMN, "[BASH]",-1);
	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,COLUMN, "[PERL]",-1);
	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,COLUMN, "[PHP]",-1);

	gtk_tree_store_append(treestore, &toplevel, NULL);
	gtk_tree_store_set(treestore, &toplevel,COLUMN, (_("Data base")),-1);

	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,COLUMN, "[MYSQL]",-1);

	gtk_tree_store_append(treestore, &toplevel, NULL);
	gtk_tree_store_set(treestore, &toplevel,COLUMN, (_("Web layout")),-1);

	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,COLUMN, "[HTML]",-1);
	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,COLUMN, "[CSS]",-1);
	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,COLUMN, "[JAVASCRIPT/JQUERY]",-1);
	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,COLUMN, "[HTACCESS]",-1);


	gtk_tree_store_append(treestore, &toplevel, NULL);
	gtk_tree_store_set(treestore, &toplevel,COLUMN, (_("Sys Admin")),-1);
	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,COLUMN, "[IPTABLES]",-1);

	gtk_tree_store_append(treestore, &toplevel, NULL);
	gtk_tree_store_set(treestore, &toplevel,COLUMN, (_("Helps Custom")),-1);

	struct dirent *lecture;
	DIR *rep;
	rep = opendir(confile.helps_dir );
	while ((lecture = readdir(rep))) 
	{
		if(strlen(lecture->d_name)>3)
		{
		gtk_tree_store_append(treestore, &child, &toplevel);
		gtk_tree_store_set(treestore, &child,COLUMN, lecture->d_name,-1);
		}
	}
	closedir(rep); 

	return GTK_TREE_MODEL(treestore);
}

//*********************** AFFICHER LE MODEL POUR LE TREEVIEW HELP
GtkWidget * create_view_and_model_help (void)
{
	GtkTreeViewColumn *col;
	GtkCellRenderer *renderer;
	GtkWidget *view;
	GtkTreeModel *model;

	view = gtk_tree_view_new();

	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, (_("Programming languages")));
	gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer,"text", COLUMN);

	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(view),TRUE);
	gtk_tree_view_set_enable_tree_lines (GTK_TREE_VIEW(view),TRUE);

	model = create_and_fill_model_help();
	gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);
	g_object_unref(model); 

	gtk_tree_view_expand_all (GTK_TREE_VIEW(view));

	return view;
}

//*********************** FOCUS SUR LE TERMINAL
void focus_term ()
{
	gtk_widget_grab_focus (vte);
}

//*********************** RELOAD ET FOCUS SUR LE MINIWEB
void focus_web ()
{
	gchar *tampon_web;
	tampon_web = gtk_editable_get_chars(GTK_EDITABLE(entry_web),0, -1);

	char *extension;
	if(strrchr(tampon_web,'.'))
		{
			extension = strrchr(tampon_web,'.');
			if (strcmp(".jpg", extension) == 0 || strcmp(".png", extension) == 0 || strcmp(".gif", extension) == 0  || strcmp(".jpeg", extension) == 0)
			{
				gtk_widget_show (GTK_WIDGET(button_web_image));
				gtk_widget_show (GTK_WIDGET(button_web_image2));
			}else{
				gtk_widget_hide (GTK_WIDGET(button_web_image));
				gtk_widget_hide (GTK_WIDGET(button_web_image2));
						}
		}

	webkit_web_view_load_uri(webView, tampon_web);
	griffon_notify(_("Visualization is available in the tab Mini Web"));
}

//*********************** MYADMIN TREEVIEW
static GtkTreeModel * create_and_fill_model_myadmin (void)
{
	GtkTreeStore *treestore;
	GtkTreeIter toplevel, child;

	treestore = gtk_tree_store_new(NUM_COLS,G_TYPE_STRING);

	gtk_tree_store_append(treestore, &toplevel, NULL);
	gtk_tree_store_set(treestore, &toplevel,COLUMN, (_("HTTP MyAdmin list                                          ")),-1);

	FILE *fich;
	char carac;
	char mot[100];
	mot[0]='\0';
	
	fich=fopen(confile.tea_myadmin,"r");
		while ((carac =fgetc(fich)) != EOF)
		{
			if (carac =='\n')
			{
			gtk_tree_store_append(treestore, &child, &toplevel);
			gtk_tree_store_set(treestore, &child,COLUMN, mot,-1);
			mot[0]='\0';
			}
			else
			{
			strncat(mot,&carac,1);
			}
		}

	fclose(fich);
	return GTK_TREE_MODEL(treestore);
}

//*********************** MODEL TREEVIEW MYADMIN
GtkWidget * create_view_and_model_myadmin (void)
{
	GtkTreeViewColumn *col;
	GtkCellRenderer *renderer;

	GtkTreeModel *model;

	gtk_widget_destroy(view_myadmin);

	view_myadmin = gtk_tree_view_new();   
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, (_("\n[ALT+N] to configure \n[ALT+H] to reload\n")));
	gtk_tree_view_append_column(GTK_TREE_VIEW(view_myadmin), col);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer, "text", COLUMN);

	model = create_and_fill_model_myadmin();
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(view_myadmin),TRUE);
	gtk_tree_view_set_enable_tree_lines (GTK_TREE_VIEW(view_myadmin),TRUE);
	gtk_tree_view_set_model(GTK_TREE_VIEW(view_myadmin), model);
	g_object_unref(model); 

	gtk_tree_view_expand_all (GTK_TREE_VIEW(view_myadmin));

	return view_myadmin;
}

//*********************** MATCH ET APPEL AUX FONCTIONS MYADMIN
void  on_changed_myadmin(GtkWidget *widget)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	char *value;

	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(widget), &model, &iter))
	{
		gtk_tree_model_get(model, &iter, COLUMN, &value,  -1);
		webkit_web_view_load_uri(webView_myadmin, value);
		g_free(value);
	}
}

//*********************** RELOAD TREEVIEW MYADMIN
void  myadmin()
{
	gtk_widget_destroy(view_a);

	view_a = create_view_and_model_myadmin();
	gtk_widget_show (GTK_WIDGET(view_a));
	selection_myadmin = gtk_tree_view_get_selection(GTK_TREE_VIEW(view_a));
	gtk_widget_show (GTK_WIDGET(selection_myadmin));
	gtk_box_pack_start(GTK_BOX(vbox_myadmin), view_a, FALSE, TRUE, 1);

	g_signal_connect(selection_myadmin, "changed",G_CALLBACK(on_changed_myadmin), NULL);  
}

//*********************** CHECK SI UN ONGLET ET OUVERT EN EDITION
void  no_onglet_open()
{
	if (! get_page_text())
	{
		gtk_widget_hide(GTK_WIDGET(notebook1));	
		gtk_widget_show(GTK_WIDGET(scrolledWindow_editor));
		gtk_widget_show(GTK_WIDGET(webView_editor));
		gtk_widget_show(GTK_WIDGET(recent_file));
		gtk_widget_show(GTK_WIDGET(hbox_no));
	}
	else
	{
		gtk_widget_hide(GTK_WIDGET(scrolledWindow_editor));	
		gtk_widget_hide(GTK_WIDGET(webView_editor));	
		gtk_widget_hide(GTK_WIDGET(recent_file));	
		gtk_widget_hide(GTK_WIDGET(hbox_no));	
		gtk_widget_show(GTK_WIDGET(notebook1));
	}
}

//*********************** WINDOW DOC GRIFFON
GtkWidget* doc_window (void)
{
	GtkWidget *window1;
	GtkWidget *vbox1;
	WebKitWebView *webView_doc;

	window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for(GTK_WINDOW(window1),GTK_WINDOW(tea_main_window));
	gtk_window_set_title (GTK_WINDOW (window1), _((_("Griffon IDE documentation"))));
	gtk_window_resize (GTK_WINDOW (window1), 930, 600);
	gtk_widget_show (GTK_WIDGET(window1));

	vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_show (GTK_WIDGET(vbox1));
	gtk_container_add (GTK_CONTAINER (window1), GTK_WIDGET(vbox1));

	webView_doc = WEBKIT_WEB_VIEW(webkit_web_view_new());
	gtk_widget_show (GTK_WIDGET(webView_doc));

	g_signal_connect(webView_doc, "new-window-policy-decision-requested",G_CALLBACK(myadmin_new_window), webView_doc);
	g_signal_connect(webView_doc, "create-web-view",G_CALLBACK(web_new_w_click_go), webView_doc);

	GtkWidget *scrolledWindow_doc = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show (GTK_WIDGET(scrolledWindow_doc));
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow_doc),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(scrolledWindow_doc), GTK_WIDGET(webView_doc));

	gtk_box_pack_start(GTK_BOX(vbox1), GTK_WIDGET(scrolledWindow_doc), TRUE, TRUE, 1);

	webkit_web_view_load_uri(webView_doc, "http://griffon.lasotel.fr/DOC/doc_en.html");

	return window1;
}

//*********************** WINDOW CHECK VERSION
GtkWidget* version_window (void)
{
	GtkWidget *window1;
	GtkWidget *vbox1;
	WebKitWebView *webView_doc;

	window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for(GTK_WINDOW(window1),GTK_WINDOW(tea_main_window));
	gtk_window_set_title (GTK_WINDOW (window1), _((_("Griffon IDE Version"))));
	gtk_window_resize (GTK_WINDOW (window1), 670, 300);
	gtk_widget_show (GTK_WIDGET(window1));

	vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_show (GTK_WIDGET(vbox1));
	gtk_container_add (GTK_CONTAINER (window1), GTK_WIDGET(vbox1));

	GtkWidget *scrolledwindow5 = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (GTK_WIDGET(scrolledwindow5));
	gtk_box_pack_start(GTK_BOX(vbox1), GTK_WIDGET(scrolledwindow5), TRUE, TRUE, 1);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow5), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_placement (GTK_SCROLLED_WINDOW (scrolledwindow5), GTK_CORNER_TOP_LEFT);


	webView_doc = WEBKIT_WEB_VIEW(webkit_web_view_new());
	gtk_widget_show (GTK_WIDGET(webView_doc));

	gtk_container_add(GTK_CONTAINER(scrolledwindow5), GTK_WIDGET(webView_doc));

	webkit_web_view_load_uri(webView_doc, "http://griffon.lasotel.fr/version.php?version=1.7.2");

	GtkWidget *button_fixme = gtk_button_new_with_label (_("Update for Ubuntu/Mint/Debian"));
	gtk_widget_show(GTK_WIDGET(button_fixme));
	gtk_box_pack_start(GTK_BOX(vbox1), button_fixme, FALSE, FALSE, 0);

	int ubuntu=1;
	g_signal_connect ((gpointer) button_fixme, "clicked",G_CALLBACK (window_update),(gpointer) ubuntu);

	GtkWidget *button_fixme2 = gtk_button_new_with_label (_("Update for Ubuntu/Mint/Debian version BETA"));
	gtk_widget_show(GTK_WIDGET(button_fixme2));
	gtk_box_pack_start(GTK_BOX(vbox1), button_fixme2, FALSE, FALSE, 0);

	int ubuntu_beta=2;
	g_signal_connect ((gpointer) button_fixme2, "clicked",G_CALLBACK (window_update),(gpointer) ubuntu_beta);


	return window1;
}

//*********************** WINDOW BUG RAPPORT
GtkWidget* rapport_window (void)
{
	GtkWidget *window1;
	GtkWidget *vbox1;
	WebKitWebView *webView_doc;

	window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for(GTK_WINDOW(window1),GTK_WINDOW(tea_main_window));
	gtk_window_set_title (GTK_WINDOW (window1), _((_("Report Griffon IDE"))));
	gtk_window_resize (GTK_WINDOW (window1), 670, 345);
	gtk_widget_show (GTK_WIDGET(window1));

	vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_show (GTK_WIDGET(vbox1));
	gtk_container_add (GTK_CONTAINER (window1), GTK_WIDGET(vbox1));

	webView_doc = WEBKIT_WEB_VIEW(webkit_web_view_new());
	gtk_widget_show (GTK_WIDGET(webView_doc));

	gtk_container_add(GTK_CONTAINER(vbox1), GTK_WIDGET(webView_doc));

	webkit_web_view_load_uri(webView_doc, "http://griffon.lasotel.fr/bug_rapport.php?clef=1.7.2");

	return window1;
}

//*********************** AFFICHAGE DU FICHIER EN EDITION DANS LE MINIWEB
void  web_current_file()
{
	if (! get_page_text()) return;
	char type[150];
	strcpy(type,"file://");
	strcat(type,cur_text_doc->file_name);
	gtk_entry_set_text (GTK_ENTRY (entry_web), _(type));
	focus_web ();
	griffon_notify(_("La visualisation est disponible dans l'onglet Mini Web"));
}

//*********************** ICON POUR LONGLET IN
void  insert_img_autoconf_var()
{
	GdkPixbuf *pixbuf;
	GtkTextIter itFin;
	pixbuf = gdk_pixbuf_new_from_file("/usr/local/share/griffon/images/griffon_package.png", NULL);
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer_scan), &itFin);
	gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(buffer_scan),&itFin,pixbuf);
}

void  insert_img_autoconf_mot()
{
	GdkPixbuf *pixbuf;
	GtkTextIter itFin;
	pixbuf = gdk_pixbuf_new_from_file("/usr/local/share/griffon/images/griffon_man.png", NULL);
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer_scan), &itFin);
	gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(buffer_scan),&itFin,pixbuf);
}

void  insert_img_autoconf_fonc()
{
	GdkPixbuf *pixbuf;
	GtkTextIter itFin;
	pixbuf = gdk_pixbuf_new_from_file("/usr/local/share/griffon/images/griffon_exe.png", NULL);
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer_scan), &itFin);
	gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(buffer_scan),&itFin,pixbuf);
}

//*********************** AJUSTEMENT DU SCROLL POUR LONGLET IN
void  fin_de_scroll_pos()
{
	GtkTextIter itFin;
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer_scan), &itFin);
	gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW(sView_scan), &itFin, 0.0, FALSE, 0.0, 0.0 );
}

//*********************** ICON POUR LE LOG ZONE DINFORMATION
void  icon_log_logmemo()
{
	GdkPixbuf *pixbuf;
	GtkTextBuffer* buffer_img;
	GtkTextIter itFin;
	pixbuf = gdk_pixbuf_new_from_file("/usr/local/share/griffon/images/griffon_log.png", NULL);
	buffer_img = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv_logmemo));
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer_img), &itFin);
	gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(buffer_img),&itFin,pixbuf);
}

void  icon_list_logmemo()
{
	GdkPixbuf *pixbuf;
	GtkTextBuffer* buffer_img;
	GtkTextIter itFin;
	pixbuf = gdk_pixbuf_new_from_file("/usr/local/share/griffon/images/griffon_man.png", NULL);
	buffer_img = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv_logmemo));
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer_img), &itFin);
	gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(buffer_img),&itFin,pixbuf);
}

void  icon_stop_logmemo()
{
	GdkPixbuf *pixbuf;
	GtkTextBuffer* buffer_img;
	GtkTextIter itFin;
	pixbuf = gdk_pixbuf_new_from_file("/usr/local/share/griffon/images/griffon_stop2.png", NULL);
	buffer_img = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv_logmemo));
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer_img), &itFin);
	gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(buffer_img),&itFin,pixbuf);
}

void  icon_ok_logmemo()
{
	GdkPixbuf *pixbuf;
	GtkTextBuffer* buffer_img;
	GtkTextIter itFin;
	pixbuf = gdk_pixbuf_new_from_file("/usr/local/share/griffon/images/griffon_ok2.png", NULL);
	buffer_img = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv_logmemo));
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer_img), &itFin);
	gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(buffer_img),&itFin,pixbuf);
}

void  icon_man_logmemo()
{
	GdkPixbuf *pixbuf;
	GtkTextBuffer* buffer_img;
	GtkTextIter itFin;
	pixbuf = gdk_pixbuf_new_from_file("/usr/local/share/griffon/images/griffon_man.png", NULL);
	buffer_img = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv_logmemo));
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer_img), &itFin);
	gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(buffer_img),&itFin,pixbuf);
}

void  icon_save_logmemo()
{
	GdkPixbuf *pixbuf;
	GtkTextBuffer* buffer_img;
	GtkTextIter itFin;
	pixbuf = gdk_pixbuf_new_from_file("/usr/local/share/griffon/images/griffon_save2.png", NULL);
	buffer_img = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv_logmemo));
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer_img), &itFin);
	gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(buffer_img),&itFin,pixbuf);			
}

void  icon_nosave_logmemo()
{
	GdkPixbuf *pixbuf;
	GtkTextBuffer* buffer_img;
	GtkTextIter itFin;
	pixbuf = gdk_pixbuf_new_from_file("/usr/local/share/griffon/images/griffon_nosave2.png", NULL);
	buffer_img = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv_logmemo));
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer_img), &itFin);
	gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(buffer_img),&itFin,pixbuf);
}

//*********************** CONTROLE SI LE FICHIER ET SAUVEGARDE
void  controle_save()
{
	if(save_controle==1)
	{
		icon_affiche_ok();	
		save_controle=2;	
	}
}

void  controle_save_page_icon_no()
{
	return;
	if (! get_page_text()) return;
	if(gtk_widget_get_visible (cur_text_doc->icon)){gtk_widget_set_visible (cur_text_doc->icon,FALSE);}

}

void  controle_save_page_icon() 
{
	return;
	if (! get_page_text()) return;

	gchar **a = g_strsplit (cur_text_doc->file_name, "_", -1);
	if (strcmp("noname", a[0]) == 0) return;

	if(! gtk_widget_get_visible (cur_text_doc->icon)){gtk_widget_set_visible (cur_text_doc->icon,TRUE);}
}

//*********************** SFTP TREEVIEW
GtkTreeModel * create_and_fill_model_sftp (void)
{
	GtkTreeStore *treestore;
	GtkTreeIter toplevel, child;

	treestore = gtk_tree_store_new(NUM_COLS,G_TYPE_STRING);

	gtk_tree_store_append(treestore, &toplevel, NULL);
	gtk_tree_store_set(treestore, &toplevel,COLUMN, (_("SFTP list                                          ")),-1);

	FILE *fich;
	char carac;
	char mot[100];
	mot[0]='\0';
	
	fich=fopen(confile.tea_sftp,"r");
	while ((carac =fgetc(fich)) != EOF)
	{
		if (carac =='\n')
		{
			if(strlen(mot)>3)
			{
				gtk_tree_store_append(treestore, &child, &toplevel);
				gtk_tree_store_set(treestore, &child,COLUMN, mot,-1);
				mot[0]='\0';
			}
		}
		else
		{
			strncat(mot,&carac,1);
		}
	}

	fclose(fich);
	return GTK_TREE_MODEL(treestore);
}

//*********************** SFTP TREEVIEW MODEL
GtkWidget * create_view_and_model_sftp (void)
{
	GtkTreeViewColumn *col;
	GtkCellRenderer *renderer;

	GtkTreeModel *model;

	view_sftp = gtk_tree_view_new();   
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, (_("\nHistory SFTP\n")));
	gtk_tree_view_append_column(GTK_TREE_VIEW(view_sftp), col);

	gtk_tree_view_set_enable_search(GTK_TREE_VIEW(view_sftp),FALSE);
	gtk_tree_view_set_search_equal_func(GTK_TREE_VIEW(view_sftp),(GtkTreeViewSearchEqualFunc) util_treeview_match_all_words_callback, NULL, NULL);

	gtk_tree_view_set_search_entry((GtkTreeView *)view_sftp,(GtkEntry *)search_sftp);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer, "text", COLUMN);

	model = create_and_fill_model_sftp();
	gtk_tree_view_set_model(GTK_TREE_VIEW(view_sftp), model);
	g_object_unref(model); 

	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(view_sftp),TRUE);
	gtk_tree_view_set_enable_tree_lines (GTK_TREE_VIEW(view_sftp),TRUE);

	gtk_tree_view_expand_all (GTK_TREE_VIEW(view_sftp));

	return view_sftp;
}

//*********************** MATCH DES FONCTION SFTP
void  on_changed_sftp(GtkWidget *widget,GdkEventKey *event,gpointer data)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	char *value;
	char mot[150];

	if(widget==NULL){printf(" ");}
	if(event==NULL){printf(" ");}

	const char *home_dir = g_getenv ("HOME");

	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(data), &model, &iter))
	{
		gtk_tree_model_get(model, &iter, COLUMN, &value,  -1);  
		strcpy(mot,value);

		gchar **a = g_strsplit (value, " ", -1);
		if (strcmp("SFTP", a[0]) == 0 || strcmp("SFTP", a[1]) == 0){return;}

		if(sshadd==NULL){int systemRet2 =system ("ssh-add");sshadd="ok";if(systemRet2 == -1){return;}}

		strcpy(mot,"sshfs ");
		strcat(mot,a[1]);
		strcat(mot,"@");				
		strcat(mot,a[0]);
		strcat(mot,":");
		strcat(mot,a[2]);	

		strcat(mot," ");
		strcat(mot,home_dir);
		strcat(mot,"/MOUNT/");

		strcat(mot,a[0]);	
		int systemRet =system (mot); 
		if(systemRet == -1){return;}

		new_terminal_ssh (a[0],a[1],a[2]);

		strcat(liste_mount,"fusermount -u ");
		strcat(liste_mount,home_dir);
		strcat(liste_mount,"/MOUNT/");

		strcat(liste_mount,a[0]);
		strcat(liste_mount," ; ");

		tampon_sftp=a[0];

		char total_path[300];total_path[0]='\0';
		strcat(total_path,home_dir);
		strcat(total_path,"/MOUNT/");
		strcat(total_path,a[0]);		
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooserwidget2) ,total_path);

		icon_log_logmemo();
		log_to_memo (_("%s in directory mount SFTP Griffon_MONTAGE_SFTP"), a[0], LM_NORMAL);
		statusbar_msg (_("Mount SFTP [OK]"));
		griffon_notify(_("Mount SFTP"));
		icon_affiche_net ();
		g_free(value);
	}
}

//*********************** RELOAD SFTP
void  sftp_reload()
{
	gtk_widget_destroy(view_sftp);

	view_sftp = create_view_and_model_sftp();
	gtk_widget_show (GTK_WIDGET(view_sftp));
	selection_sftp = gtk_tree_view_get_selection(GTK_TREE_VIEW(view_sftp));
	gtk_widget_show (GTK_WIDGET(selection_sftp));
	gtk_container_add(GTK_CONTAINER(scrolledWindow_sftp), GTK_WIDGET(view_sftp));

	g_signal_connect(view_sftp, "button-release-event",G_CALLBACK(on_changed_sftp), selection_sftp);
}

//*********************** FTP TREEVIEW
GtkTreeModel * create_and_fill_model_ftp (void)
{
	GtkTreeStore *treestore;
	GtkTreeIter toplevel, child;

	treestore = gtk_tree_store_new(NUM_COLS,G_TYPE_STRING);

	gtk_tree_store_append(treestore, &toplevel, NULL);
	gtk_tree_store_set(treestore, &toplevel,COLUMN, (_("FTP list")),-1);

	FILE *fich;
	char carac;
	char mot[100];
	mot[0]='\0';
	
	fich=fopen(confile.tea_ftp,"r");
	while ((carac =fgetc(fich)) != EOF)
	{
		if (carac =='\n')
		{
			if(strlen(mot)>3)
			{
				gtk_tree_store_append(treestore, &child, &toplevel);
				gtk_tree_store_set(treestore, &child,COLUMN, mot,-1);
				mot[0]='\0';
			}
		}
		else
		{
			strncat(mot,&carac,1);
		}
	}

	fclose(fich);
	return GTK_TREE_MODEL(treestore);
}

//*********************** FTP TREEVIEW MODEL
GtkWidget * create_view_and_model_ftp (void)
{
	GtkTreeViewColumn *col;
	GtkCellRenderer *renderer;

	GtkTreeModel *model;

	view_ftp = gtk_tree_view_new();   
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, "\nHistory FTP\n");
	gtk_tree_view_append_column(GTK_TREE_VIEW(view_ftp), col);

	gtk_tree_view_set_enable_search(GTK_TREE_VIEW(view_ftp),FALSE);
	gtk_tree_view_set_search_equal_func(GTK_TREE_VIEW(view_ftp),(GtkTreeViewSearchEqualFunc) util_treeview_match_all_words_callback, NULL, NULL);

	gtk_tree_view_set_search_entry((GtkTreeView *)view_ftp,(GtkEntry *)search_ftp);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer, "text", COLUMN);

	model = create_and_fill_model_ftp();
	gtk_tree_view_set_model(GTK_TREE_VIEW(view_ftp), model);
	g_object_unref(model); 

	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(view_ftp),TRUE);
	gtk_tree_view_set_enable_tree_lines (GTK_TREE_VIEW(view_ftp),TRUE);

	gtk_tree_view_expand_all (GTK_TREE_VIEW(view_ftp));

	return view_ftp;
}

//*********************** MATCH FONCTION TREEVIEW FTP
void  on_changed_ftp(GtkWidget *widget,GdkEventKey *event,gpointer data)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	char *value;
	char mot[150];

	if(widget==NULL){printf(" ");}
	if(event==NULL){printf(" ");}

	const char *home_dir = g_getenv ("HOME");

	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(data), &model, &iter))
	{
		gtk_tree_model_get(model, &iter, COLUMN, &value,  -1);  
		strcpy(mot,value);
		gchar **a = g_strsplit (value, " ", -1);
		if (strcmp("FTP", a[0]) == 0 || strcmp("FTP", a[1]) == 0){return;}

		strcpy(mot,"curlftpfs ");
		strcat(mot,a[1]);
		strcat(mot,":");				
		strcat(mot,a[2]);
		strcat(mot,"@");
		strcat(mot,a[0]);		
		strcat(mot," ");
		strcat(mot,home_dir);
		strcat(mot,"/MOUNT/");
		strcat(mot,a[0]);		
		
		int systemRet =system (mot);
		if(systemRet == -1){return;}

		strcat(liste_mount,"fusermount -u ");
		strcat(liste_mount,home_dir);
		strcat(liste_mount,"/MOUNT/");
		strcat(liste_mount,a[0]);
		strcat(liste_mount," ; ");

		tampon_sftp=a[0];

		char total_path[300];total_path[0]='\0';
		strcat(total_path,home_dir);
		strcat(total_path,"/MOUNT/");
		strcat(total_path,a[0]);		
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooserwidget2) ,total_path);

		icon_log_logmemo();
		log_to_memo (_("%s in directory mount SFTP Griffon_MONTAGE_SFTP"), a[0], LM_NORMAL);
		griffon_notify(_("Mount FTP"));
		icon_affiche_net ();                                                                                                 
		g_free(value);
		statusbar_msg (_("Mount FTP [OK]"));
	}
}

//*********************** RELOAD FTP
void  ftp_reload()
{
	gtk_widget_destroy(view_ftp);

	view_ftp = create_view_and_model_ftp();
	gtk_widget_show (GTK_WIDGET(view_ftp));
	selection_ftp = gtk_tree_view_get_selection(GTK_TREE_VIEW(view_ftp));
	gtk_widget_show (GTK_WIDGET(selection_ftp));
	gtk_container_add(GTK_CONTAINER(scrolledWindow_ftp), GTK_WIDGET(view_ftp));

	g_signal_connect(view_ftp, "button-release-event",G_CALLBACK(on_changed_ftp), selection_ftp);
}

//*********************** CLEAR HISTORIQUE FTP
void  vide_configuration_ftp()
{
	save_string_to_file_vide(confile.tea_ftp,"");
	ftp_reload();
}

//*********************** CLEAR HISTORIQUE SFTP
void  vide_configuration_sftp()
{
	save_string_to_file_vide(confile.tea_sftp,"");
	sftp_reload();
}

//*********************** RELOAD MINIWEB SI PAGE HTML EN EDITION
void  switch_html_page()
{
	if (! get_page_text()) return;

	gchar **a = g_strsplit (cur_text_doc->file_name, "_", -1);
	if (strcmp("noname", a[0]) == 0) return;

	char *extension;
	if(strrchr(cur_text_doc->file_name,'.'))
	{
		extension = strrchr(cur_text_doc->file_name,'.');
		if (strcmp(".htm", extension) == 0 || strcmp(".html", extension) == 0 )
		{
			char type[150];
			strcpy(type,"file://");
			strcat(type,cur_text_doc->file_name);
			gtk_entry_set_text (GTK_ENTRY (entry_web), _(type));
			focus_web ();	
			griffon_notify(_("La visualisation est disponible dans l'onglet Mini Web"));
		}
	}
}

//*********************** CREAT LIST STORE
 GtkListStore * create_liststore()
{
	store = gtk_list_store_new(3, GDK_TYPE_PIXBUF, G_TYPE_STRING,G_TYPE_STRING);
	return store;
}

//*********************** CREAT LIST STORE TODO LIST
 GtkListStore * create_liststore_todo()
{
	store_todo = gtk_list_store_new(3, GDK_TYPE_PIXBUF, G_TYPE_STRING,G_TYPE_STRING);
	return store_todo;
}

//*********************** ADD TODO
void add_to_list(gchar *str,gchar *str2)
{
	GtkTreeIter    iter;
	GdkPixbuf     *icon;

	icon = gdk_pixbuf_new_from_file("/usr/local/share/griffon/images/griffon_log.png", NULL);

	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,COL_ICON, icon,COL_TEXT, str,COL_TEXT2, str2,-1);	
}

void add_to_list_fc(gchar *str,gchar *str2)
{
	GtkTreeIter    iter;
	GdkPixbuf     *icon;

	icon = gdk_pixbuf_new_from_file("/usr/local/share/griffon/images/griffon_exe.png", NULL);

	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,COL_ICON, icon, COL_TEXT, str, COL_TEXT2, str2,-1);	
}

void add_to_list_var(gchar *str,gchar *str2)
{
	GtkTreeIter    iter;
	GdkPixbuf     *icon;

	icon = gdk_pixbuf_new_from_file("/usr/local/share/griffon/images/griffon_package.png", NULL);

	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,COL_ICON, icon, COL_TEXT, str, COL_TEXT2, str2,-1);	
}

void add_to_list_err(gchar *str,gchar *str2)
{
	GtkTreeIter    iter;
	GdkPixbuf     *icon;

	icon = gdk_pixbuf_new_from_file("/usr/local/share/griffon/images/griffon_bug.png", NULL);

	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter,COL_ICON, icon, COL_TEXT, str, COL_TEXT2, str2, -1);	
}

void add_to_list_todo(gchar *str,gchar *str2)
{
	GtkTreeIter    iter;
	GdkPixbuf     *icon;

	icon = gdk_pixbuf_new_from_file("/usr/local/share/griffon/images/griffon_bug.png", NULL);

	gtk_list_store_append(store_todo, &iter);
	gtk_list_store_set(store_todo, &iter,COL_ICON, icon,COL_TEXT, str, COL_TEXT2, str2,-1);	
}

void add_to_list_todo_bug(gchar *str,gchar *str2)
{
	GtkTreeIter    iter;
	GdkPixbuf     *icon;

	icon = gdk_pixbuf_new_from_file("/usr/local/share/griffon/images/griffon_advance.png", NULL);

	gtk_list_store_append(store_todo, &iter);
	gtk_list_store_set(store_todo, &iter,COL_ICON, icon, COL_TEXT, str, COL_TEXT2, str2, -1);	
}

void add_to_list_todo_fixme(gchar *str,gchar *str2)
{
	GtkTreeIter    iter;
	GdkPixbuf     *icon;

	icon = gdk_pixbuf_new_from_file("/usr/local/share/griffon/images/griffon_exe.png", NULL);

	gtk_list_store_append(store_todo, &iter);
	gtk_list_store_set(store_todo, &iter,COL_ICON, icon, COL_TEXT, str, COL_TEXT2, str2,-1);	
}

//*********************** CLEAR LIST
void clear_list_include (){gtk_list_store_clear(store);}
void clear_list_todo (){gtk_list_store_clear(store_todo);}

//*********************** ON CLICK TREEVIEW
void on_changed_scan (GtkWidget *widget,GdkEventKey *event,gpointer data)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	char *value;

	if(widget==NULL){printf(" ");}
	if(event==NULL){printf(" ");}

	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(data), &model, &iter)) 
	{
		if (! get_page_text()) return;
		gtk_tree_model_get(model, &iter, COL_TEXT2, &value,  -1);

		if(value!=NULL)
		{
			gtk_entry_set_text (GTK_ENTRY (ent_search), value);
			g_free(value);
			doc_select_line (cur_text_doc, strtol (gtk_entry_get_text (GTK_ENTRY (ent_search)), NULL, 10));
		}
	}
}

//*********************** INSERT DU CHEMIN DUNE IMAGE
void insert_image_web() 
{
	if (! get_page_text()) return;

	gchar *tampon_web;
	tampon_web = gtk_editable_get_chars(GTK_EDITABLE(entry_web),0, -1);

	gchar **a = g_strsplit (tampon_web, "://", -1);
	doc_insert_at_cursor (cur_text_doc, a[1]);
}

//*********************** CREAT LIST STORE BOOKMARK
GtkListStore * create_liststore_book()
{
	store_book = gtk_list_store_new(3, GDK_TYPE_PIXBUF, G_TYPE_STRING,G_TYPE_STRING);
	return store_book;
}

//*********************** ADD BOOKMARK
void add_to_list_book(gchar *str,gchar *str2)
{
	GtkTreeIter    iter;
	GdkPixbuf     *icon;

	icon = gdk_pixbuf_new_from_file("/usr/local/share/griffon/images/griffon_man.png", NULL);

	gtk_list_store_append(store_book, &iter);
	gtk_list_store_set(store_book, &iter, COL_ICON, icon,COL_TEXT, str, COL_TEXT2, str2,-1);	

	griffon_notify(_("The bookmark is available in the tab: \nMark"));
}

//*********************** MATCH SELECT BOOKMARK
void on_changed_book (GtkWidget *widget)
{

	GtkTreeIter iter;
	GtkTreeModel *model_book;
	char *value;

	if (! get_page_text()) return;

	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(widget), &model_book, &iter)) 
	{
		gtk_tree_model_get(model_book, &iter, COL_TEXT2, &value,  -1);

		gchar **a = g_strsplit (value, ":", -1);
		gtk_entry_set_text (GTK_ENTRY (ent_search), a[0]);
		on_mni_goto_line();
		gtk_tree_selection_unselect_all(GTK_TREE_SELECTION(widget));	
	}
}

//*********************** FONCTION DETACHEMENT DONGLET
GtkNotebook* window_creation_function (GtkNotebook *source_notebook)
{
	GtkWidget *window, *notebook;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for(GTK_WINDOW(window),GTK_WINDOW(tea_main_window));
	gtk_window_set_deletable (GTK_WINDOW(window),FALSE);
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_MOUSE);

	notebook = gtk_notebook_new ();
	gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_TOP);

	gtk_notebook_set_group_name (GTK_NOTEBOOK (notebook),
	gtk_notebook_get_group_name (source_notebook));

	g_signal_connect (notebook, "create-window",G_CALLBACK (window_creation_function), NULL);

	gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET(notebook));

	gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);

	gtk_notebook_set_scrollable (GTK_NOTEBOOK (notebook), TRUE);
	gtk_window_set_title (GTK_WINDOW (window), _((_("Tabs"))));
	gtk_window_set_icon_from_file (GTK_WINDOW(window),"/usr/local/share/griffon/images/griffon_ok.png",NULL);

	gtk_widget_show_all (GTK_WIDGET(window));

	return GTK_NOTEBOOK (notebook);
}

//*********************** CLEAR BOOKMARK
void del_book() {gtk_list_store_clear(store_book);}

//*********************** RECHERCHE AVEC GOOGLE
void google_search()
{
	if (! get_page_text()) return;

	if(doc_get_sel (cur_text_doc))
	{
		char search_google[150];
		strcpy(search_google,"https://www.google.fr/#q=");
		strcat(search_google,doc_get_sel (cur_text_doc)); 
		webkit_web_view_load_uri(webView_myadmin_aide, search_google);

		gtk_entry_set_text (GTK_ENTRY (entry_find_web_help), _(doc_get_sel (cur_text_doc)));
		griffon_notify(_("The search result is available in the tab: MyAdmin-> Help/Search"));
	preview_web_popup_search ();
	}
}

//*********************** TRADUCTION AVEC GOOGLE
void google_traduction_fr_en()
{
	if (! get_page_text()) return;

	if(doc_get_sel (cur_text_doc))
	{
		char search_google[650];
		strcpy(search_google,"https://translate.google.fr/?hl=fr&tab=wT#fr/en/");
		strcat(search_google,doc_get_sel (cur_text_doc));
		webkit_web_view_load_uri(webView_myadmin_traduc, search_google);
		preview_web_popup_translate ();
		griffon_notify(_("The result of the translation is available in the tab::\nMyAdmin-> Translation"));
	}
}

//*********************** TRADUCTION EN VERS FR AVEC GOOGLE
void google_traduction_en_fr()
{
	if (! get_page_text()) return;

	if(doc_get_sel (cur_text_doc))
	{
		char search_google[650];
		strcpy(search_google,"https://translate.google.fr/?hl=fr&tab=wT#en/fr/");
		strcat(search_google,doc_get_sel (cur_text_doc));
		webkit_web_view_load_uri(webView_myadmin_traduc, search_google);
		griffon_notify(_("The result of the translation is available in the tab :\nMyAdmin->Translation"));
		preview_web_popup_translate2 ();
	}
}

//*********************** LOAD URL MINIWEB
void enter_web ()
{
	gchar *tampon_web;

	if(gtk_editable_get_chars(GTK_EDITABLE(entry_web),0, -1))
	{	
		tampon_web = gtk_editable_get_chars(GTK_EDITABLE(entry_web),0, -1);
		webkit_web_view_load_uri(webView, tampon_web);

		if (! g_file_test (confile.tea_miniweb_history, G_FILE_TEST_EXISTS))
		{
		create_empty_file (confile.tea_miniweb_history, "");
		}

		if (g_file_test (confile.tea_miniweb_history, G_FILE_TEST_EXISTS))
		{
			FILE *fich;
			char carac;
			char mot[200];
			mot[0]='\0';
			int check=0;
	
			fich=fopen(confile.tea_miniweb_history,"r");
			while ((carac =fgetc(fich)) != EOF)
			{
				if (carac =='\n')
				{
					if (strncmp(mot,tampon_web,strlen(mot))==0 ){check=1;}
					mot[0]='\0';
				}
				else
				{
					strncat(mot,&carac,1);
				}
			}

			fclose(fich);

			if(check==0)
			{
				if (! g_file_test (confile.tea_miniweb_history, G_FILE_TEST_EXISTS))
				{
					create_empty_file (confile.tea_miniweb_history, (gchar *)tampon_web);
					save_string_to_file_add(confile.tea_miniweb_history,"\n");
				}
				else
				{
					save_string_to_file_add(confile.tea_miniweb_history,(gchar *)tampon_web);
					save_string_to_file_add(confile.tea_miniweb_history,"\n");
				}

				GtkTreeIter iter_entry;
				gtk_list_store_append(model_entry_http, &iter_entry);
				gtk_list_store_set(model_entry_http, &iter_entry, CONTACT_NAME_HTTP, gtk_entry_get_text (GTK_ENTRY (entry_web)),  -1);

			}
		}
	}
}

//*********************** LOAD URL MYADMIN
void enter_myweb ()
{
	gchar *tampon_myweb;

	if(gtk_editable_get_chars(GTK_EDITABLE(entry_myadmin),0, -1))
	{
		tampon_myweb = gtk_editable_get_chars(GTK_EDITABLE(entry_myadmin),0, -1);
		webkit_web_view_load_uri(webView_myadmin, tampon_myweb);

		if (! g_file_test (confile.tea_myadmin_history, G_FILE_TEST_EXISTS))
		{
		create_empty_file (confile.tea_myadmin_history, "");
		}

		if (g_file_test (confile.tea_myadmin_history, G_FILE_TEST_EXISTS))
		{
			FILE *fich;
			char carac;
			char mot[200];
			mot[0]='\0';
			int check=0;
	
			fich=fopen(confile.tea_myadmin_history,"r");
			while ((carac =fgetc(fich)) != EOF)
			{
				if (carac =='\n')
				{
					if (strncmp(mot,tampon_myweb,strlen(mot))==0 ){check=1;}
					mot[0]='\0';
				}
				else
				{
					strncat(mot,&carac,1);
				}
			}

			fclose(fich);

			if(check==0)
			{
				if (! g_file_test (confile.tea_myadmin_history, G_FILE_TEST_EXISTS))
				{
					create_empty_file (confile.tea_myadmin_history, (gchar *)tampon_myweb);
					save_string_to_file_add(confile.tea_myadmin_history,"\n");
				}
				else
				{
					save_string_to_file_add(confile.tea_myadmin_history,(gchar *)tampon_myweb);
					save_string_to_file_add(confile.tea_myadmin_history,"\n");
				}

				GtkTreeIter iter_entry;
				gtk_list_store_append(model_entry_http2, &iter_entry);
				gtk_list_store_set(model_entry_http2, &iter_entry, CONTACT_NAME_HTTP2, gtk_entry_get_text (GTK_ENTRY (entry_myadmin)),  -1);

			}
		}


	}
}

//*********************** FONCTION TOOLBAR MYADMIN
void myadmin_reload (){webkit_web_view_reload(webView_myadmin);}
void myadmin_back (){webkit_web_view_go_back(webView_myadmin);}
void myadmin_forward (){webkit_web_view_go_forward(webView_myadmin);}
void myadmin_stop (){webkit_web_view_stop_loading(webView_myadmin);}

void myadmin_reload_win (GtkWidget *widget, gpointer user_data){gtk_widget_get_name(widget);webkit_web_view_reload(user_data);}
void myadmin_back_win (GtkWidget *widget,gpointer user_data){gtk_widget_get_name(widget);webkit_web_view_go_back(user_data);}
void myadmin_forward_win (GtkWidget *widget,gpointer user_data){gtk_widget_get_name(widget);webkit_web_view_go_forward(user_data);}
void myadmin_stop_win (GtkWidget *widget,gpointer user_data){gtk_widget_get_name(widget);webkit_web_view_stop_loading(user_data);}
void myadmin_source_mode_get_url_win (GtkWidget *widget,gpointer user_data){gtk_widget_get_name(widget);webkit_web_view_set_view_source_mode(user_data,TRUE);webkit_web_view_reload(user_data);}
void myadmin_view_mode_get_url_win (GtkWidget *widget,gpointer user_data){gtk_widget_get_name(widget);webkit_web_view_set_view_source_mode(user_data,FALSE);webkit_web_view_reload(user_data);}

//*********************** LOAD URL MYADMIN WINDOW
void enter_myweb_win (GtkWidget* entry, gpointer user_data)
{
	gchar *tampon_myweb;

	if(gtk_editable_get_chars(GTK_EDITABLE(entry),0, -1))
	{
		tampon_myweb = gtk_editable_get_chars(GTK_EDITABLE(entry),0, -1);
		webkit_web_view_load_uri(user_data, tampon_myweb);
	}
}

//*********************** GET URL MYADMIN WINDOW
void myadmin_get_url_win (WebKitWebView  *web,WebKitWebFrame *web_frame,gpointer user_data)
{
	if(web_frame!=NULL){printf(" ");}
	gchar const *url=webkit_web_view_get_uri(web);
	gtk_entry_set_text (GTK_ENTRY (user_data), _(url));

	if (! get_page_text()) return;
	if(doc_get_sel (cur_text_doc))
	{
	webkit_web_view_unmark_text_matches(WEBKIT_WEB_VIEW (web));
	gchar *search=doc_get_sel (cur_text_doc);
	webkit_web_view_mark_text_matches (WEBKIT_WEB_VIEW (web), search, FALSE, 0);
	webkit_web_view_set_highlight_text_matches (WEBKIT_WEB_VIEW (web), TRUE);
//	webkit_web_view_search_text (WEBKIT_WEB_VIEW (web), search, FALSE, TRUE, TRUE);
	}

}

//*********************** FONCTION TOOLBAR MINIWEB
void miniweb_reload (){webkit_web_view_reload(webView);}
void miniweb_back (){webkit_web_view_go_back(webView);}
void miniweb_forward (){webkit_web_view_go_forward(webView);}
void miniweb_stop (){webkit_web_view_stop_loading(webView);}

//*********************** GET URL MYADMIN
void myadmin_get_url ()
{
	gchar const *url=webkit_web_view_get_uri(webView_myadmin);
	gtk_entry_set_text (GTK_ENTRY (entry_myadmin), _(url));
}

//*********************** GET URL MINIWEB
void miniweb_get_url ()
{
	gchar const *url=webkit_web_view_get_uri(webView);
	gtk_entry_set_text (GTK_ENTRY (entry_web), _(url));
}

//*********************** MINIWEB MODE SOURCE
void miniweb_source_mode_get_url (){webkit_web_view_set_view_source_mode(webView,TRUE);webkit_web_view_reload(webView);}
//*********************** MINIWEB VIEW MODE
void miniweb_view_mode_get_url (){webkit_web_view_set_view_source_mode(webView,FALSE);webkit_web_view_reload(webView);}
//*********************** MYADMIN MODE SOURCE
void myadmin_source_mode_get_url (){webkit_web_view_set_view_source_mode(webView_myadmin,TRUE);webkit_web_view_reload(webView_myadmin);}
//*********************** MYADMIN VIEW MODE
void myadmin_view_mode_get_url (){webkit_web_view_set_view_source_mode(webView_myadmin,FALSE);webkit_web_view_reload(webView_myadmin);}

//*********************** COPY TERMINAL
void copy_vte(GtkWidget *tv,GdkEventButton *event,  gpointer user_data)
{
	if(event==NULL){printf("\n");}
	gtk_widget_get_name(tv);

	vte_terminal_copy_clipboard(user_data);
	gtk_widget_destroy(menu_vte);
}

//*********************** PAST TERMINAL
void paste_vte(GtkWidget *tv,GdkEventButton *event,  gpointer user_data)
{
	if(event==NULL){printf("\n");}
	gtk_widget_get_name(tv);
	vte_terminal_paste_clipboard(user_data);
	gtk_widget_destroy(menu_vte);
}

//*********************** RECHERCHE GOOGLE TERMINAL
void term_search_google(gpointer user_data)
{
	vte_terminal_copy_clipboard(user_data);
	char search_google[150];

	GtkClipboard *clipboard = gtk_clipboard_get (GDK_SELECTION_PRIMARY);
	gchar *selection = NULL;
	selection = gtk_clipboard_wait_for_text (clipboard);

	strcpy(search_google,"https://www.google.fr/#q=");
	strcat(search_google,selection);
	webkit_web_view_load_uri(webView_myadmin_aide, search_google);
	gtk_widget_destroy(menu_vte);
	griffon_notify(_("Le résultat de la recherche est disponible dans l'onglet :\nMyAdmin->Aide/Recherche"));
}

//*********************** MENU POPUP TERMINAL
gboolean popup_context_menu_vte(GtkWidget *tv, GdkEventButton *event)
{
	GtkWidget *menu_item,*menu_item2,*menu_item3,*menu_item4,*menu_item5;
	gboolean ret = FALSE;

	if (event->type == GDK_BUTTON_PRESS && event->button == 3)
	{
		ret = TRUE;
		menu_vte = gtk_menu_new();

		gtk_container_set_border_width(GTK_CONTAINER(menu_vte), 2);

		if(vte_terminal_get_has_selection (VTE_TERMINAL(tv)))
		{
			menu_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_COPY,NULL);
			gtk_image_menu_item_set_always_show_image (GTK_IMAGE_MENU_ITEM(menu_item) ,TRUE);
			g_signal_connect(menu_item, "button-release-event", G_CALLBACK(copy_vte),tv);
			gtk_menu_shell_append(GTK_MENU_SHELL(menu_vte), menu_item);
		}

		menu_item2 = gtk_image_menu_item_new_from_stock(GTK_STOCK_PASTE,NULL);
		gtk_image_menu_item_set_always_show_image (GTK_IMAGE_MENU_ITEM(menu_item2) ,TRUE);
		g_signal_connect(menu_item2, "button-release-event", G_CALLBACK(paste_vte), tv);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu_vte), menu_item2);

		menu_item3 = gtk_menu_item_new_with_label (_("Save a text file"));
		g_signal_connect(menu_item3, "button-release-event", G_CALLBACK(new_file_term_edit), tv);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu_vte), menu_item3);

		menu_item4 = gtk_menu_item_new_with_label (_("Google search"));
		g_signal_connect(menu_item4, "button-release-event", G_CALLBACK(term_search_google), tv);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu_vte), menu_item4);

		menu_item5 = gtk_menu_item_new_with_label (_("Help"));
		g_signal_connect(menu_item5, "button-release-event", G_CALLBACK(term_help), tv);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu_vte), menu_item5);

		gtk_widget_show_all(GTK_WIDGET(menu_vte));

		gtk_menu_popup(GTK_MENU(menu_vte), NULL, NULL, NULL, NULL, 3, GDK_CURRENT_TIME);

	}else{gtk_widget_grab_focus (tv);}

	return ret;
}


//*********************** NEW TERMINAL
void new_terminal ()
{
	term_page *page_term = (term_page *) g_malloc (sizeof (term_page));


	page_term->vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_show (GTK_WIDGET(page_term->vbox2));
	gtk_container_add (GTK_CONTAINER (notebook_term), GTK_WIDGET(page_term->vbox2));

	page_term->vte_add = vte_terminal_new();

	vte_terminal_set_background_image_file (VTE_TERMINAL(page_term->vte_add),"/usr/local/share/griffon/images/griffon_bg2.png");
	vte_terminal_set_background_saturation (VTE_TERMINAL(page_term->vte_add),0.3);

	vte_terminal_set_scroll_on_keystroke(VTE_TERMINAL (page_term->vte_add), TRUE);
	gtk_box_pack_start(GTK_BOX(page_term->vbox2), GTK_WIDGET(page_term->vte_add), TRUE, TRUE, 1);

	gtk_widget_show (GTK_WIDGET(page_term->vte_add));

	vte_terminal_set_scrollback_lines(VTE_TERMINAL(page_term->vte_add),-1);

	char** startterm=NULL;
	g_shell_parse_argv("/bin/bash", NULL, &startterm, NULL);

	vte_terminal_fork_command_full(VTE_TERMINAL(page_term->vte_add),VTE_PTY_DEFAULT, NULL, startterm, NULL, (GSpawnFlags)(G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_SEARCH_PATH),
	NULL, 
	NULL, 
	NULL, 
	NULL);

	page_term->num_tab=gtk_notebook_get_n_pages(GTK_NOTEBOOK (notebook_term));
	if(page_term->num_tab!=0){page_term->num_tab--;}

	gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook_term), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_term), page_term->num_tab), TRUE);

	page_term->label_term = gtk_label_new (_("   Terminal   "));
	page_term->hbox_term_tab = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);

	page_term->image_term_tab = gtk_image_new_from_stock ("gtk-yes", GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_box_pack_start (GTK_BOX (page_term->hbox_term_tab), page_term->image_term_tab, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (page_term->hbox_term_tab), page_term->label_term, TRUE, TRUE, 0);
	gtk_widget_show (GTK_WIDGET(page_term->hbox_term_tab));
	gtk_widget_show (GTK_WIDGET(page_term->label_term));
	gtk_widget_show (GTK_WIDGET(page_term->image_term_tab));

	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook_term), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_term), page_term->num_tab), page_term->hbox_term_tab);

	g_signal_connect (page_term->vte_add, "button-press-event", G_CALLBACK (popup_context_menu_vte), NULL);
	g_signal_connect (page_term->vte_add, "child-exited", G_CALLBACK (on_button_close_term), NULL);

	gtk_notebook_set_current_page(GTK_NOTEBOOK (notebook_term), page_term->num_tab);
}

//*********************** CLOSE TERMINAL
void on_button_close_term ()
{
	gint num_tab=gtk_notebook_get_current_page(GTK_NOTEBOOK (notebook_term));
	gtk_notebook_remove_page(GTK_NOTEBOOK (notebook_term),num_tab);
}

//*********************** NEXT/PREV TERMINAL
void next_term (){gtk_notebook_next_page(GTK_NOTEBOOK (notebook_term));}
void prev_term (){gtk_notebook_prev_page(GTK_NOTEBOOK (notebook_term));}

//*********************** BOOKMARK ON/OFF
void myadmin_bookmark_off (){gtk_widget_hide(GTK_WIDGET(view_a));}
void myadmin_bookmark_on (){gtk_widget_show(GTK_WIDGET(view_a));}

//*********************** WINDOW DEBUG RUN SCRIPT/PROG
void window_debug ()
{
	if (! get_page_text()) return;
	gtk_widget_destroy (window_run);

	window_run = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for(GTK_WINDOW(window_run),GTK_WINDOW(tea_main_window));
	gtk_window_resize (GTK_WINDOW (window_run), 900, 420);
	
	GtkWidget * vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (window_run), GTK_WIDGET(vbox2));

	GtkWidget *toolbar_myadmin;
	toolbar_myadmin = gtk_toolbar_new ();

	GtkToolItem *tool_exe = gtk_tool_button_new_from_stock(GTK_STOCK_EXECUTE  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_exe, -1);
	g_signal_connect ((gpointer) tool_exe, "clicked",G_CALLBACK (run_debug),NULL);
	gtk_tool_item_set_tooltip_text(tool_exe,_("Run script/bin"));

	GtkToolItem *tool_chmod = gtk_tool_button_new_from_stock(GTK_STOCK_APPLY  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_chmod, -1);
	g_signal_connect ((gpointer) tool_chmod, "clicked",G_CALLBACK (exe),NULL);
	gtk_tool_item_set_tooltip_text(tool_chmod,_("Chmod u+x script/bin"));

	GtkToolItem *tool_sep=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_sep, -1);


	GtkToolItem *tool_clear = gtk_tool_button_new_from_stock(GTK_STOCK_CLEAR  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_clear, -1);
	g_signal_connect ((gpointer) tool_clear, "clicked",G_CALLBACK (clear_debug),NULL);
	gtk_tool_item_set_tooltip_text(tool_clear,_("Clear terminal"));

	gtk_box_pack_start (GTK_BOX (vbox2), toolbar_myadmin, FALSE, FALSE, 0);
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_myadmin), GTK_TOOLBAR_ICONS);

	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar_myadmin),GTK_ICON_SIZE_SMALL_TOOLBAR);

	vte_add = vte_terminal_new();

	char** startterm=NULL;
	g_shell_parse_argv("/bin/bash", NULL, &startterm, NULL);

	vte_terminal_fork_command_full(VTE_TERMINAL(vte_add),VTE_PTY_DEFAULT,NULL, startterm, NULL,  (GSpawnFlags)(G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_SEARCH_PATH),  
	NULL, 
	NULL, 
	NULL, 
	NULL);

	vte_terminal_set_background_image_file (VTE_TERMINAL(vte_add),"/usr/local/share/griffon/images/griffon_bg2.png");
	vte_terminal_set_background_saturation (VTE_TERMINAL(vte_add),0.3);

	vte_terminal_set_scroll_on_keystroke(VTE_TERMINAL (vte_add), TRUE);
	gtk_container_add (GTK_CONTAINER (vbox2), GTK_WIDGET(vte_add));	

	vte_terminal_feed_child (VTE_TERMINAL(vte_add),"PS1=\"[\\d \\t]\"\n",-1);
	vte_terminal_feed_child (VTE_TERMINAL(vte_add),"clear\n",-1);

	g_signal_connect (vte_add, "button-press-event", G_CALLBACK (popup_context_menu_vte), NULL);
	gtk_widget_show (GTK_WIDGET(vbox2));
	gtk_widget_show (GTK_WIDGET(vte_add));
	gtk_widget_show_all (GTK_WIDGET(window_run));
	gtk_widget_show (GTK_WIDGET(toolbar_myadmin)); 
	gtk_widget_show(GTK_WIDGET(tool_clear));
	gtk_widget_show(GTK_WIDGET(tool_sep));
	gtk_widget_show(GTK_WIDGET(tool_exe));
	gtk_widget_show(GTK_WIDGET(tool_chmod));

	run_debug();
}

//*********************** LANCER LE DEBUG
void run_debug ()
{
	char *extension;
	if(strrchr(cur_text_doc->file_name,'.'))
	{
		extension = strrchr(cur_text_doc->file_name,'.');

			if (strcmp(".sh", extension) == 0)
			{
				clear_debug();  
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),cur_text_doc->file_name,-1);
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),"\n",-1);
			}

			if (strcmp(".pl", extension) == 0)
			{
				clear_debug();
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),cur_text_doc->file_name,-1);
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),"\n",-1);
			}

			if (strcmp(".c", extension) == 0 || strcmp(".h", extension) == 0)
			{
				if (p_dir_makefile)
				{
				clear_debug();
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),"cd ",-1);
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),p_dir_makefile,-1);
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),"\n",-1);
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),p_cmd_make,-1);
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),"\n",-1);
				}

				if (p_file_executable)
				{
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),p_file_executable,-1);
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),"\n",-1);
				}
			}

	}

	statusbar_msg (_("Run Debug"));
}

//*********************** CLEAR DEBUG
void clear_debug ()
{
	vte_terminal_reset (VTE_TERMINAL(vte_add),TRUE,TRUE);
}

//*********************** NEW TERMINAL SSH
void new_terminal_ssh (gchar *serveur,gchar *user,gchar *path)
{
	term_page *page_term = (term_page *) g_malloc (sizeof (term_page));

	page_term->vbox2 =gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (notebook_term), GTK_WIDGET(page_term->vbox2));

	page_term->vte_add = vte_terminal_new();

	char** startterm=NULL;
	g_shell_parse_argv("/bin/bash", NULL, &startterm, NULL);

	vte_terminal_fork_command_full(VTE_TERMINAL(page_term->vte_add),
	VTE_PTY_DEFAULT,
	NULL,  
	startterm,
	NULL, 
	(GSpawnFlags)(G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_SEARCH_PATH),  
	NULL, 
	NULL, 
	NULL, 
	NULL); 

	vte_terminal_set_background_image_file (VTE_TERMINAL(page_term->vte_add),"/usr/local/share/griffon/images/griffon_bg2.png");
	vte_terminal_set_background_saturation (VTE_TERMINAL(page_term->vte_add),0.3);

	vte_terminal_set_scroll_on_keystroke(VTE_TERMINAL (page_term->vte_add), TRUE);
	gtk_box_pack_start(GTK_BOX(page_term->vbox2), GTK_WIDGET(page_term->vte_add), TRUE, TRUE, 1);

	page_term->num_tab=gtk_notebook_get_n_pages(GTK_NOTEBOOK (notebook_term));
	if(page_term->num_tab!=0){page_term->num_tab--;}

	gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook_term), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_term), page_term->num_tab), TRUE);

	page_term->label_term = gtk_label_new (_(serveur));

	page_term->hbox_term_tab = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);

	page_term->image_term_tab = gtk_image_new_from_stock ("gtk-yes", GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_box_pack_start (GTK_BOX (page_term->hbox_term_tab), page_term->image_term_tab, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (page_term->hbox_term_tab), page_term->label_term, TRUE, TRUE, 0);

	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook_term), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_term), page_term->num_tab), page_term->hbox_term_tab);

	vte_terminal_feed_child (VTE_TERMINAL(page_term->vte_add),"ssh ",-1);
	vte_terminal_feed_child (VTE_TERMINAL(page_term->vte_add),user,-1);
	vte_terminal_feed_child (VTE_TERMINAL(page_term->vte_add),"@",-1);
	vte_terminal_feed_child (VTE_TERMINAL(page_term->vte_add),serveur,-1);
	vte_terminal_feed_child (VTE_TERMINAL(page_term->vte_add),"\n",-1);

	vte_terminal_feed_child (VTE_TERMINAL(page_term->vte_add),"cd ",-1);
	vte_terminal_feed_child (VTE_TERMINAL(page_term->vte_add),path,-1);
	vte_terminal_feed_child (VTE_TERMINAL(page_term->vte_add),"\n",-1);

	gtk_widget_set_can_focus (GTK_WIDGET (page_term->label_term), FALSE);
	gtk_widget_set_can_focus (GTK_WIDGET (page_term->hbox_term_tab), FALSE);

	gtk_widget_show (GTK_WIDGET(page_term->vbox2));
	gtk_widget_show (GTK_WIDGET(page_term->vte_add));
	gtk_widget_show (GTK_WIDGET(page_term->label_term));
	gtk_widget_show (GTK_WIDGET(page_term->hbox_term_tab));
	gtk_widget_show (GTK_WIDGET(page_term->image_term_tab));
	gtk_notebook_set_current_page(GTK_NOTEBOOK (notebook_term), page_term->num_tab);

	g_signal_connect (page_term->vte_add, "button-press-event", G_CALLBACK (popup_context_menu_vte), NULL);
	g_signal_connect (page_term->vte_add, "child-exited", G_CALLBACK (on_button_close_term), NULL);
}

//*********************** CREAT REPERTOIRE HELP
void new_dir_cmd_help ()
{

	gchar* dir="";

		char rep_path[200];
		strcpy(rep_path,confile.helps_dir);

	if(strlen (gtk_entry_get_text (GTK_ENTRY (ent_search_help))) != 0)
	{
		dir=gtk_editable_get_chars(GTK_EDITABLE(cmb_famous_help),0, -1);
		strcat(rep_path,dir);
		if (mkdir (rep_path, S_IRUSR | S_IWUSR | S_IXUSR) == -1){log_to_memo (_("Creat Help Custom Error."), NULL, LM_ERROR);statusbar_msg (_("Creat Help Custom [ERROR]"));}
		else{log_to_memo (_("Creat Help Custom %s"), rep_path, LM_NORMAL); statusbar_msg (_("Creat Help Custom [OK]"));
		griffon_notify(_("The Help Custom is created."));

		gtk_widget_destroy(view_help);
		GtkTreeSelection *selection;
		view_help = create_view_and_model_help();
		gtk_widget_show (GTK_WIDGET(view_help));
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(view_help));
		gtk_box_pack_start(GTK_BOX(vbox_help), view_help, TRUE, TRUE, 1);
		g_signal_connect(selection, "changed",  G_CALLBACK(on_changed), statusbar_help);

		return;
				}
	}
	else{log_to_memo (_("Creat Help Custom Error : you must enter the name in the command line"), NULL, LM_ERROR);statusbar_msg (_("Creat Help Custom [ERROR]"));}
}

//*********************** RELOAD REPERTOIRE HELP
void reload_dir_cmd_help ()
{
	gtk_widget_destroy(view_help);
	GtkTreeSelection *selection;
	view_help = create_view_and_model_help();
	gtk_widget_show (GTK_WIDGET(view_help));
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(view_help));
	gtk_box_pack_start(GTK_BOX(vbox_help), view_help, TRUE, TRUE, 1);
	g_signal_connect(selection, "changed",  G_CALLBACK(on_changed), statusbar_help);
}

//*********************** CREAT REPERTOIRE
void new_dir_cmd ()
{
	gchar* path_dir="";
	gchar* dir="";
	char pathT[500];
	pathT[0]='\0';

	if(! gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER(filechooserwidget2))){log_to_memo (_("Mkdir Error: you must select a directory in the File Selector Tool."), NULL, LM_ERROR);statusbar_msg (_("Mkdir ERROR"));return;}

	path_dir=gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER(filechooserwidget2));
	strcpy(pathT,path_dir);
	strcat(pathT,"/");

	if(strlen (gtk_entry_get_text (GTK_ENTRY (ent_search2))) != 0)
	{
		dir=gtk_editable_get_chars(GTK_EDITABLE(cmb_famous2),0, -1);
		strcat(pathT,dir);
		if (mkdir (pathT, S_IRUSR | S_IWUSR | S_IXUSR) == -1){log_to_memo (_("Mkdir Error."), NULL, LM_ERROR);statusbar_msg (_("Mkdir [ERROR]"));}
		else{log_to_memo (_("Mkdir %s"), pathT, LM_NORMAL); gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooserwidget2) ,pathT); statusbar_msg (_("Mkdir [OK]"));
		griffon_notify(_("The directory is created."));

		char commande[350];
		strcpy(commande,"chmod 755 ");
		strcat(commande,pathT);
		int systemRet =system(commande);
		if(systemRet == -1){return;}

		return;
				}
	}
	else{log_to_memo (_("Mkdir Error : you must enter the directory name in the command line"), NULL, LM_ERROR);statusbar_msg (_("Mkdir [ERROR]"));}
}

//*********************** CREAT FICHIER
void new_file_cmd ()
{
	gchar* path_file="";
	gchar* dir="";
	char pathT[500];
	pathT[0]='\0';

	if(! gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER(filechooserwidget2))){log_to_memo (_("File Error: you must select a directory in the File Selector Tool."), NULL, LM_ERROR);statusbar_msg (_("File touch [ERROR]"));return;}

	path_file=gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER(filechooserwidget2));
	strcpy(pathT,path_file);
	strcat(pathT,"/");
	dir = gtk_editable_get_chars(GTK_EDITABLE(ent_search2),0, -1);

	if(strlen(dir)>0)
	{
		strcat(pathT,dir);
		if (g_file_test (pathT, G_FILE_TEST_EXISTS)){log_to_memo (_("File Error."), NULL, LM_ERROR);statusbar_msg (_("File touch [ERROR]"));}
		else{

				create_empty_file (pathT, "");
				log_to_memo (_("File %s"), pathT, LM_NORMAL);
				statusbar_msg (_("File touch [OK]"));
				griffon_notify(_("The file is created."));
				doc_open_file (pathT);

				char *extension;
				if(strrchr(pathT,'.'))
				{
					extension = strrchr(pathT,'.');
					if (strcmp(".pl", extension) == 0){start_perl_script();}
					if (strcmp(".sh", extension) == 0){print_bash();}
					if (strcmp(".php", extension) == 0){start_php_script();}
					if (strcmp(".html", extension) == 0){on_mni_html_default_template();}
					if (strcmp(".htm", extension) == 0){on_mni_html_default_template();}
				}

		char commande[350];
		strcpy(commande,"chmod u+x ");
		strcat(commande,cur_text_doc->file_name);
		int systemRet =system(commande);
		if(systemRet == -1){return;}
			}
	}
	else{log_to_memo (_("File Error: You must enter the file name in the command line"), NULL, LM_ERROR);statusbar_msg (_("File touch [ERROR]"));}
}

//*********************** CREAT NEW WEB WINDOW
void new_web_window ()
{

	win_web *web_win = (win_web *) g_malloc (sizeof (win_web));

	web_win->window_web = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for(GTK_WINDOW(web_win->window_web),GTK_WINDOW(tea_main_window));
	gtk_window_resize (GTK_WINDOW (web_win->window_web), 900, 500);
	gtk_window_maximize (GTK_WINDOW(web_win->window_web));

	web_win->vbox3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (web_win->window_web), GTK_WIDGET(web_win->vbox3));
	gtk_widget_show (GTK_WIDGET(web_win->vbox3));  

	web_win->hbox3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(web_win->hbox3));
	gtk_box_pack_start (GTK_BOX (web_win->vbox3), web_win->hbox3, FALSE, TRUE, 0);

	web_win->webView_w = WEBKIT_WEB_VIEW(webkit_web_view_new());

	GtkToolItem *tool_sep;
	GtkWidget *toolbar_myadmin;
	toolbar_myadmin = gtk_toolbar_new ();

	GtkToolItem *tool_myadmin_reload = gtk_tool_button_new_from_stock(GTK_STOCK_REFRESH  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_reload, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_reload));
	g_signal_connect ((gpointer) tool_myadmin_reload, "clicked",G_CALLBACK (myadmin_reload_win),web_win->webView_w);
	gtk_tool_item_set_tooltip_text(tool_myadmin_reload,_("Reload"));

	GtkToolItem *tool_myadmin_back = gtk_tool_button_new_from_stock(GTK_STOCK_UNDO  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_back, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_back));
	g_signal_connect ((gpointer) tool_myadmin_back, "clicked",G_CALLBACK (myadmin_back_win),web_win->webView_w);
	gtk_tool_item_set_tooltip_text(tool_myadmin_back,_("Undo"));

	GtkToolItem *tool_myadmin_prev = gtk_tool_button_new_from_stock(GTK_STOCK_REDO  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_prev, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_prev));
	g_signal_connect ((gpointer) tool_myadmin_prev, "clicked",G_CALLBACK (myadmin_forward_win),web_win->webView_w);
	gtk_tool_item_set_tooltip_text(tool_myadmin_prev,_("Redo"));

	GtkToolItem *tool_myadmin_stop = gtk_tool_button_new_from_stock(GTK_STOCK_STOP  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_stop, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_stop));
	g_signal_connect ((gpointer) tool_myadmin_stop, "clicked",G_CALLBACK (myadmin_stop_win),web_win->webView_w);
	gtk_tool_item_set_tooltip_text(tool_myadmin_stop,_("Stop"));

	tool_sep=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin ), tool_sep, -1);
	gtk_widget_show(GTK_WIDGET(tool_sep));

	GtkToolItem *tool_myadmin_source = gtk_tool_button_new_from_stock(GTK_STOCK_PROPERTIES   );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_source, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_source));
	g_signal_connect ((gpointer) tool_myadmin_source, "clicked",G_CALLBACK (myadmin_source_mode_get_url_win),web_win->webView_w);
	gtk_tool_item_set_tooltip_text(tool_myadmin_source,_("Source view"));

	GtkToolItem *tool_myadmin_view = gtk_tool_button_new_from_stock(GTK_STOCK_CONVERT    );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_view, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_view));
	g_signal_connect ((gpointer) tool_myadmin_view, "clicked",G_CALLBACK (myadmin_view_mode_get_url_win),web_win->webView_w);
	gtk_tool_item_set_tooltip_text(tool_myadmin_view,_("Web view"));

	gtk_box_pack_start (GTK_BOX (web_win->hbox3), toolbar_myadmin, TRUE, TRUE, 0);
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_myadmin), GTK_TOOLBAR_ICONS);
	gtk_widget_show (GTK_WIDGET(toolbar_myadmin)); 

	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar_myadmin),GTK_ICON_SIZE_SMALL_TOOLBAR);

	web_win->entry_myadmin = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(web_win->entry_myadmin));
	gtk_box_pack_start (GTK_BOX (web_win->hbox3), web_win->entry_myadmin, TRUE, TRUE, 0);

	gtk_widget_show (GTK_WIDGET(web_win->webView_w));

	web_win->scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show (GTK_WIDGET(web_win->scrolledWindow));
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(web_win->scrolledWindow),GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(web_win->scrolledWindow), GTK_WIDGET(web_win->webView_w));

	gtk_box_pack_start(GTK_BOX(web_win->vbox3), GTK_WIDGET(web_win->scrolledWindow), TRUE, TRUE, 1);

	gtk_widget_show_all (GTK_WIDGET(web_win->window_web)); 

	g_signal_connect ((gpointer) web_win->entry_myadmin, "activate",G_CALLBACK (enter_myweb_win), web_win->webView_w);

	gchar *tampon_myweb;

	if(gtk_editable_get_chars(GTK_EDITABLE(entry_myadmin),0, -1))
	{
		tampon_myweb = gtk_editable_get_chars(GTK_EDITABLE(entry_myadmin),0, -1);
		webkit_web_view_load_uri(web_win->webView_w, tampon_myweb);
		gtk_entry_set_text (GTK_ENTRY (web_win->entry_myadmin), _(tampon_myweb));
	}

	g_signal_connect(web_win->webView_w, "document-load-finished",G_CALLBACK(myadmin_get_url_win), web_win->entry_myadmin);
	g_signal_connect(web_win->webView_w, "new-window-policy-decision-requested",  G_CALLBACK(myadmin_new_window), web_win->webView_w);
	g_signal_connect(web_win->webView_w, "create-web-view",G_CALLBACK(web_new_w_click_go), web_win->webView_w);
}

//*********************** CREAT NEW WEB WINDOW
void new_web_window_mini ()
{

	win_web *web_win = (win_web *) g_malloc (sizeof (win_web));

	web_win->window_web = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for(GTK_WINDOW(web_win->window_web),GTK_WINDOW(tea_main_window));
	gtk_window_resize (GTK_WINDOW (web_win->window_web), 900, 500);
	gtk_window_maximize (GTK_WINDOW(web_win->window_web));

	web_win->vbox3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (web_win->window_web), GTK_WIDGET(web_win->vbox3));
	gtk_widget_show (GTK_WIDGET(web_win->vbox3));  

	web_win->hbox3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(web_win->hbox3));
	gtk_box_pack_start (GTK_BOX (web_win->vbox3), web_win->hbox3, FALSE, TRUE, 0);

	web_win->webView_w = WEBKIT_WEB_VIEW(webkit_web_view_new());

	GtkToolItem *tool_sep;
	GtkWidget *toolbar_myadmin;
	toolbar_myadmin = gtk_toolbar_new ();

	GtkToolItem *tool_myadmin_reload = gtk_tool_button_new_from_stock(GTK_STOCK_REFRESH  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_reload, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_reload));
	g_signal_connect ((gpointer) tool_myadmin_reload, "clicked",G_CALLBACK (myadmin_reload_win),web_win->webView_w);
	gtk_tool_item_set_tooltip_text(tool_myadmin_reload,_("Reload"));

	GtkToolItem *tool_myadmin_back = gtk_tool_button_new_from_stock(GTK_STOCK_UNDO  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_back, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_back));
	g_signal_connect ((gpointer) tool_myadmin_back, "clicked",G_CALLBACK (myadmin_back_win),web_win->webView_w);
	gtk_tool_item_set_tooltip_text(tool_myadmin_back,_("Undo"));

	GtkToolItem *tool_myadmin_prev = gtk_tool_button_new_from_stock(GTK_STOCK_REDO  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_prev, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_prev));
	g_signal_connect ((gpointer) tool_myadmin_prev, "clicked",G_CALLBACK (myadmin_forward_win),web_win->webView_w);
	gtk_tool_item_set_tooltip_text(tool_myadmin_prev,_("Redo"));

	GtkToolItem *tool_myadmin_stop = gtk_tool_button_new_from_stock(GTK_STOCK_STOP  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_stop, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_stop));
	g_signal_connect ((gpointer) tool_myadmin_stop, "clicked",G_CALLBACK (myadmin_stop_win),web_win->webView_w);
	gtk_tool_item_set_tooltip_text(tool_myadmin_stop,_("Stop"));

	tool_sep=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin ), tool_sep, -1);
	gtk_widget_show(GTK_WIDGET(tool_sep));

	GtkToolItem *tool_myadmin_source = gtk_tool_button_new_from_stock(GTK_STOCK_PROPERTIES   );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_source, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_source));
	g_signal_connect ((gpointer) tool_myadmin_source, "clicked",G_CALLBACK (myadmin_source_mode_get_url_win),web_win->webView_w);
	gtk_tool_item_set_tooltip_text(tool_myadmin_source,_("Source view"));

	GtkToolItem *tool_myadmin_view = gtk_tool_button_new_from_stock(GTK_STOCK_CONVERT    );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_myadmin_view, -1);
	gtk_widget_show(GTK_WIDGET(tool_myadmin_view));
	g_signal_connect ((gpointer) tool_myadmin_view, "clicked",G_CALLBACK (myadmin_view_mode_get_url_win),web_win->webView_w);
	gtk_tool_item_set_tooltip_text(tool_myadmin_view,_("Web view"));

	gtk_box_pack_start (GTK_BOX (web_win->hbox3), toolbar_myadmin, TRUE, TRUE, 0);
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_myadmin), GTK_TOOLBAR_ICONS);
	gtk_widget_show (GTK_WIDGET(toolbar_myadmin)); 

	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar_myadmin),GTK_ICON_SIZE_SMALL_TOOLBAR);

	web_win->entry_myadmin = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(web_win->entry_myadmin));
	gtk_box_pack_start (GTK_BOX (web_win->hbox3), web_win->entry_myadmin, TRUE, TRUE, 0);

	gtk_widget_show (GTK_WIDGET(web_win->webView_w));

	web_win->scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show (GTK_WIDGET(web_win->scrolledWindow));
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(web_win->scrolledWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(web_win->scrolledWindow), GTK_WIDGET(web_win->webView_w));

	gtk_box_pack_start(GTK_BOX(web_win->vbox3), GTK_WIDGET(web_win->scrolledWindow), TRUE, TRUE, 1);
	gtk_widget_show_all (GTK_WIDGET(web_win->window_web)); 

	g_signal_connect ((gpointer) web_win->entry_myadmin, "activate",G_CALLBACK (enter_myweb_win),web_win->webView_w);

	gchar *tampon_myweb;

	if(gtk_editable_get_chars(GTK_EDITABLE(entry_web),0, -1))
	{
		tampon_myweb = gtk_editable_get_chars(GTK_EDITABLE(entry_web),0, -1);
		webkit_web_view_load_uri(web_win->webView_w, tampon_myweb);
		gtk_entry_set_text (GTK_ENTRY (web_win->entry_myadmin), _(tampon_myweb));
	}

	g_signal_connect(web_win->webView_w, "document-load-finished",G_CALLBACK(myadmin_get_url_win), web_win->entry_myadmin);
	g_signal_connect(web_win->webView_w, "new-window-policy-decision-requested",G_CALLBACK(myadmin_new_window), web_win->webView_w);
	g_signal_connect(web_win->webView_w, "create-web-view", G_CALLBACK(web_new_w_click_go), web_win->webView_w);

}

//*********************** CREAT FICHIER DE LOG
void new_file_log_edit ()
{
	file_new();
	gchar *buf = doc_get_buf (GTK_TEXT_BUFFER(log_memo_textbuffer));
	doc_insert_at_cursor (cur_text_doc,buf);
}

//*********************** CREAT FICHIER LOG TERMINAL
void new_file_term_edit (GtkWidget *tv,GdkEventButton *event,  gpointer user_data)
{
	if(event==NULL){printf("\n");}
	gtk_widget_get_name(tv);
	file_new();
	gchar *buf = vte_terminal_get_text(user_data,NULL,NULL,NULL);
	doc_insert_at_cursor (cur_text_doc,buf);
}

//*********************** FONCTION NOTIFY
void griffon_notify (gchar* txt)
{
	NotifyNotification * notification=notify_notification_new("Griffon IDE",txt,"/usr/local/share/griffon/images/griffon_button.png");
	notify_notification_show (notification,NULL);
}

//******************************** DL WEBKIT
void download_status_cb(GObject* object, GParamSpec* pspec, gpointer data){ 
	WebKitDownload *download;
	WebKitDownloadStatus status;

	if(pspec==NULL){printf(" ");}
	if(data==NULL){printf(" ");}

	download = WEBKIT_DOWNLOAD(object);
	status = webkit_download_get_status(download);

	switch (status) {
		case WEBKIT_DOWNLOAD_STATUS_ERROR:
			griffon_notify(_("Download error"));
		break;
		case WEBKIT_DOWNLOAD_STATUS_CREATED:
			griffon_notify(_("Download created"));
		break;
		case WEBKIT_DOWNLOAD_STATUS_STARTED:
			griffon_notify(_("Download started"));
		break;
		case WEBKIT_DOWNLOAD_STATUS_CANCELLED:
			griffon_notify(_("Download cancelled"));
		break;
		case WEBKIT_DOWNLOAD_STATUS_FINISHED:
			griffon_notify(_("Download finished!"));
		break;
		default:
		break;
		}
}

//******************************** TELECHARGEMENT POUR WEBKIT
void download_requested_cb(WebKitWebView *web_view, WebKitDownload *download, gpointer user_data)
{
	const gchar *filename="";
	gchar* path_dir="";
	gchar *uri1="file://";

	if(web_view==NULL){printf(" ");}
	if(user_data==NULL){printf(" ");}

	if(! gtk_file_chooser_get_current_folder ((GtkFileChooser *)filechooserwidget2)){log_to_memo (_("Download Error : You must select a directory in the file selector."), NULL, LM_ERROR);statusbar_msg (_("Download ERROR"));return;}

	filename = webkit_download_get_suggested_filename(download);
	path_dir=gtk_file_chooser_get_current_folder ((GtkFileChooser *)filechooserwidget2);
	const gchar * dest_uri = g_strdup_printf("%s%s/%s", uri1, path_dir, filename);
	gchar * path = g_strdup_printf("%s/%s", path_dir, filename);

			if (g_file_test (path, G_FILE_TEST_EXISTS)){log_to_memo (_("File Error.download exists"), NULL, LM_ERROR);statusbar_msg (_("File download exists [ERROR]"));return;}
		else{
					webkit_download_set_destination_uri(download,dest_uri);
					g_signal_connect(download, "notify::status",G_CALLBACK(download_status_cb), NULL);
					}

	return; 
}

//*********************** SWITCH DONGLET ET PLACEMENT DANS LE SELECTEUR DE FICHIER
void switch_filechooser ()
{
	if (! get_page_text()) return;

	 if(cur_text_doc->file_name!=NULL)
		{
			gchar **a = g_strsplit (cur_text_doc->file_name, "_", -1);
			if (strcmp("noname", a[0]) != 0 )
			{
				gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooserwidget2) ,g_path_get_dirname (cur_text_doc->file_name));

				//*********** Pour le diff
				GtkTextIter start;
				GtkTextIter end;
				char *text;
				gtk_text_buffer_get_start_iter ((GtkTextBuffer *)cur_text_doc->text_buffer, &start);
				gtk_text_buffer_get_end_iter ((GtkTextBuffer *)cur_text_doc->text_buffer, &end);
				text = gtk_text_buffer_get_text ((GtkTextBuffer *)cur_text_doc->text_buffer, &start, &end, FALSE);       
				g_file_set_contents (confile.file_tmp, text, -1, NULL);
				g_free (text);

			}
	}
}

//*********************** RELOAD HISTORIQUE DES ENTRY
void back_history ()
{
	GtkTreeIter iter_entry;
	FILE *fich;
	char carac;
	char mot[100];
	mot[0]='\0';

	if(fopen(confile.tea_miniweb_history,"rt"))
	{
	fich=fopen(confile.tea_miniweb_history,"r");

		while ((carac =fgetc(fich)) != EOF)
		{
			if (carac =='\n')
			{
				gtk_list_store_append(model_entry_http, &iter_entry);
				gtk_list_store_set(model_entry_http, &iter_entry, CONTACT_NAME_HTTP, mot,  -1);
				mot[0]='\0';
			}
			else
			{
				strncat(mot,&carac,1);
			}
		}
	fclose(fich);
	}

	mot[0]='\0';

	if(fopen(confile.tea_myadmin_history,"rt"))
	{
	fich=fopen(confile.tea_myadmin_history,"r");

		while ((carac =fgetc(fich)) != EOF)
		{
			if (carac =='\n')
			{
				gtk_list_store_append(model_entry_http2, &iter_entry);
				gtk_list_store_set(model_entry_http2, &iter_entry, CONTACT_NAME_HTTP2, mot,  -1);
				mot[0]='\0';
			}
			else
			{
				strncat(mot,&carac,1);
			}
		}
	fclose(fich);
	}

}

//********************* DELETE LOG TODOLIST
void delete_todo()
{
	create_empty_file (confile.tea_todo, "");
	on_mni_file_todolist ();
}

//********************* LOAD PROJECTS LIST
void load_projects_list()
{
	FILE *fich;
	char carac;
	char mot[3000];
	mot[0]='\0';

	gtk_widget_destroy(vbox_proj_main);

	vbox_proj_main = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_pack_start (GTK_BOX (vbox_proj), vbox_proj_main, TRUE, TRUE, 0);
	gtk_widget_show (GTK_WIDGET(vbox_proj_main)); 

	GtkWidget *notebook_proj,*label_note4,*hbox_note,*image2;
	GdkPixbuf *pixbuf_icon;

	notebook_proj = gtk_notebook_new ();  
	gtk_widget_show (GTK_WIDGET(notebook_proj));  
	gtk_box_pack_start (GTK_BOX (vbox_proj_main), notebook_proj, TRUE, TRUE, 0);  
	gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook_proj), GTK_POS_LEFT);
	gtk_notebook_set_group_name (GTK_NOTEBOOK (notebook_proj), "wnote");	
	gtk_notebook_set_scrollable(GTK_NOTEBOOK (notebook_proj),TRUE);

	GtkWidget *vbox_proj_main2;
	GtkWidget *toolbar_proj;
	GtkToolItem *tool_proj_new;
	GtkToolItem *tool_proj_edit;
	GtkToolItem *tool_proj_delete;
	PangoFontDescription *font_desc_projet;
	GtkWidget *sView_projet2;
	GtkSourceBuffer *buffer_projet2;
	GtkWidget *scrolledwindow4;
	int ligne=0;
	int ligne_tab=0;

	GdkPixbuf *pixbuf;
	GtkTextIter itFin;

	if(fopen(confile.projects,"rt"))
	{
	fich=fopen(confile.projects,"r");

		while ((carac =fgetc(fich)) != EOF)
		{
			if (carac =='\n')
			{
			ligne++;
			gchar **a = g_strsplit (mot, ";", -1);
			gchar **b=NULL;
			if(a[0])
			{

			vbox_proj_main2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
			gtk_container_add (GTK_CONTAINER (notebook_proj), (GtkWidget *)vbox_proj_main2);
			gtk_widget_show (GTK_WIDGET(vbox_proj_main2)); 

			toolbar_proj = gtk_toolbar_new ();
			gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_proj), GTK_TOOLBAR_ICONS);
			gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar_proj),GTK_ICON_SIZE_SMALL_TOOLBAR);

			tool_proj_new = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
			gtk_toolbar_insert(GTK_TOOLBAR(toolbar_proj), tool_proj_new, -1);
			gtk_widget_show(GTK_WIDGET(tool_proj_new));
			g_signal_connect_swapped ((gpointer) tool_proj_new, "clicked",G_CALLBACK (open_project),(gpointer)ligne);
			gtk_tool_item_set_tooltip_text(tool_proj_new,_("Open project"));

			tool_proj_edit = gtk_tool_button_new_from_stock(GTK_STOCK_EDIT);
			gtk_toolbar_insert(GTK_TOOLBAR(toolbar_proj), tool_proj_edit, -1);
			gtk_widget_show(GTK_WIDGET(tool_proj_edit));
			g_signal_connect_swapped ((gpointer) tool_proj_edit, "clicked",G_CALLBACK (update_project_window),(gpointer)ligne);
			gtk_tool_item_set_tooltip_text(tool_proj_edit,_("Update project"));

			tool_proj_delete = gtk_tool_button_new_from_stock(GTK_STOCK_REMOVE);
			gtk_toolbar_insert(GTK_TOOLBAR(toolbar_proj), tool_proj_delete, -1);
			gtk_widget_show(GTK_WIDGET(tool_proj_delete));
			g_signal_connect_swapped ((gpointer) tool_proj_delete, "clicked",G_CALLBACK (delete_project),(gpointer)ligne);
			gtk_tool_item_set_tooltip_text(tool_proj_delete,_("Delete project"));

			gtk_box_pack_start (GTK_BOX (vbox_proj_main2), toolbar_proj, FALSE, FALSE, 0);
			gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_proj), GTK_TOOLBAR_ICONS);
			gtk_widget_show (GTK_WIDGET(toolbar_proj)); 

			scrolledwindow4 = gtk_scrolled_window_new (NULL, NULL);
			gtk_widget_show (GTK_WIDGET(scrolledwindow4));
			gtk_box_pack_start(GTK_BOX(vbox_proj_main2), GTK_WIDGET(scrolledwindow4), TRUE, TRUE, 1);
			gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow4), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
			gtk_scrolled_window_set_placement (GTK_SCROLLED_WINDOW (scrolledwindow4), GTK_CORNER_TOP_LEFT);

			buffer_projet2 = GTK_SOURCE_BUFFER (gtk_source_buffer_new (NULL));

			sView_projet2 = gtk_source_view_new_with_buffer(buffer_projet2);
			font_desc_projet = pango_font_description_from_string ("mono 8");
			gtk_widget_modify_font ((GtkWidget *)sView_projet2, font_desc_projet);
			pango_font_description_free (font_desc_projet);

			gtk_text_view_set_editable ((GtkTextView *)sView_projet2, FALSE);
			gtk_text_view_set_cursor_visible((GtkTextView *)sView_projet2,FALSE);

			gtk_source_view_set_show_right_margin((GtkSourceView *)sView_projet2,TRUE);
			gtk_source_view_set_show_line_marks((GtkSourceView *)sView_projet2,TRUE);

			gtk_container_add (GTK_CONTAINER (scrolledwindow4), (GtkWidget *)sView_projet2);

			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_("\n\n\t")), -1);

			if(strlen(a[6])>3)
			{
			pixbuf = gdk_pixbuf_new_from_file(a[6], NULL);
			gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer_projet2), &itFin);
			gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(buffer_projet2),&itFin,pixbuf);
			}

			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_("\n\n======= [ ")), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_(a[0])), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_(" ] ======= \n\n")), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_("[SOURCE DIR] \t: ")), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_(a[1])), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_("\n")), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_("[MAKE DIR] \t: ")), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_(a[2])), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_("\n")), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_("[RUN CMD] \t: ")), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_(a[3])), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_("\n")), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_("[MAKE CMD] \t: ")), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_(a[4])), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_("\n")), -1);

			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_("[SFTP IP] \t: ")), -1);
			if(strlen(a[8])>1){gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_(a[8])), -1);}
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_("\n")), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_("[SFTP USER] \t: ")), -1);
			if(a[9]!=NULL){b = g_strsplit (a[9], ":", -1);}
			if(b[0]!=NULL){gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_(b[0])), -1);}
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_("\n")), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_("[SFTP PATH] \t: ")), -1);
			if(b[1]!=NULL){gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_(b[1])), -1);}
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_("\n")), -1);

			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_("[FTP IP] \t: ")), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_(a[10])), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_("\n")), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_("[FTP USER] \t: ")), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_(a[11])), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_("\n")), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_("[FTP PASSWORD] \t: ")), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_(a[12])), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_("\n")), -1);


			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_("\n_____________________________________________________\n\n===== [ INFO ] ===== \n\n")), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_(a[5])), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_("\n\n[URL/HTTP] : ")), -1);
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet2), (_(a[7])), -1);

			gtk_widget_show (GTK_WIDGET(sView_projet2)); 

			ligne_tab=ligne-1;
			label_note4 = gtk_label_new (_(a[0]));
			gtk_widget_show (GTK_WIDGET(label_note4));

			gtk_widget_set_size_request (label_note4, 200, 20);

			hbox_note = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			gtk_widget_show (GTK_WIDGET(hbox_note));
			gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook_proj), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_proj), ligne_tab), hbox_note);

			if(strlen(a[6])>3)
			{
			pixbuf_icon=gdk_pixbuf_new_from_file(a[6],NULL);
			pixbuf_icon=gdk_pixbuf_scale_simple(pixbuf_icon,20,20,GDK_INTERP_BILINEAR);
			image2=gtk_image_new_from_pixbuf(pixbuf_icon);
			}
			else
			{
			image2 = gtk_image_new_from_stock ("gtk-directory", GTK_ICON_SIZE_SMALL_TOOLBAR);
			}

			gtk_widget_show (GTK_WIDGET(image2));
			gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
			gtk_box_pack_start (GTK_BOX (hbox_note), label_note4, TRUE, TRUE, 0);	

			gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook_proj), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_proj), ligne_tab), TRUE);

			}
			mot[0]='\0';
			}
			else
			{
				strncat(mot,&carac,1);
			}
		}
	fclose(fich);
	}
}

//********************* OPEN PROJECTS
void open_project(gpointer data)
{
	FILE *fich;
	char carac;
	char mot[1000];
	mot[0]='\0';
	int ligne=0;

	if(fopen(confile.projects,"rt"))
	{
	fich=fopen(confile.projects,"r");

		while ((carac =fgetc(fich)) != EOF)
		{
			if (carac =='\n')
			{
			ligne++;
				if(ligne==(int)data)
				{
					gchar **a = g_strsplit (mot, ";", -1);

					p_project_name = a[0];
					p_project_file_name = NULL;
					p_dir_makefile = a[2];
					p_dir_source = a[1];
					p_file_executable = a[3];
					p_cmd_make = a[4];
					p_info = a[5];
					p_icon = a[6];

					gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooserwidget2) ,p_dir_source);
					griffon_notify(_("Opening the project"));

					//********** CHARGE URL
					if(strlen(a[7])>4)
					{
						webkit_web_view_load_uri(webView, a[7]);
					}

					//********** CHARGE SFTP
					if(strlen(a[8])>1 && strlen(a[9])>1)
					{
						char mot2[150];
						char mot3[150];
						gchar *tampon_utilisateur;
						gchar *tampon_chemin;
						gchar *tampon_sftp;

						const char *home_dir = g_getenv ("HOME");

						FILE *fichier = NULL;
						fichier = fopen("/usr/bin/sshfs",  "r");

						if (fichier == NULL)
						{
						icon_stop_logmemo();
						log_to_memo (_("You must install the curlftpfs and sshfs to use the mounting  SFTP"), NULL, LM_ERROR);
						statusbar_msg (_("Mount [ERROR]"));
						}
						else
						{
						fclose(fichier); 
						gchar **b = g_strsplit (a[9], ":", -1);
						tampon_sftp = a[8];
						tampon_utilisateur = b[0];
						tampon_chemin = b[1];
//						tampon_chemin = "/";

						int systemRet=0;
						if(sshadd==NULL){systemRet =system ("ssh-add");sshadd="ok";}
						if(systemRet == -1){return;}

						strcpy(mot2,"mkdir -p ");
						strcat(mot2,home_dir);
						strcat(mot2,"/MOUNT/");

						strcat(mot2,tampon_sftp);
						systemRet =system (mot2);
						if(systemRet == -1){return;}

						strcpy(mot3,"sshfs ");
						strcat(mot3,tampon_utilisateur);
						strcat(mot3,"@");				
						strcat(mot3,tampon_sftp);
						strcat(mot3,":");
						strcat(mot3,tampon_chemin);	

						strcat(liste_mount,"fusermount -u ");
						strcat(liste_mount,home_dir);
						strcat(liste_mount,"/MOUNT/");
						strcat(liste_mount,tampon_sftp);
						strcat(liste_mount," ; ");

						new_terminal_ssh (tampon_sftp,tampon_utilisateur,tampon_chemin);

						strcat(mot3," ");
						strcat(mot3,home_dir);
						strcat(mot3,"/MOUNT/");

						strcat(mot3,tampon_sftp);	
						systemRet =system (mot3);
						if(systemRet == -1){return;}

						char total_path[300];total_path[0]='\0';
						strcat(total_path,home_dir);
						strcat(total_path,"/MOUNT/");
						strcat(total_path,tampon_sftp);		
						gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooserwidget2) ,total_path);
						icon_log_logmemo();
						log_to_memo (_("%s mount SFTP in MOUNT/"), tampon_sftp, LM_NORMAL);
						statusbar_msg (_("Mount [OK]"));
						griffon_notify(_("Mount SFTP"));
						icon_affiche_net ();
						}
					}

					//********** CHARGE FTP
					if(strlen(a[10])>1 && strlen(a[11])>1  && strlen(a[12])>1)
					{
						char mot3[150];
						char mot2[150];
						gchar *tampon_utilisateur_ftp;
						gchar *tampon_passwd_ftp;
						gchar *tampon_sftp;

						const char *home_dir = g_getenv ("HOME");

						FILE *fichier = NULL;
						fichier = fopen("/usr/bin/curlftpfs",  "r");

						if (fichier == NULL)
						{
						icon_stop_logmemo();
						log_to_memo (_("You must install the curlftpfs to use the mounting FTP"), NULL, LM_ERROR);
						statusbar_msg (_("Mount [ERROR]"));
						}
						else
						{
						fclose(fichier); 
						tampon_sftp = a[10];
						tampon_utilisateur_ftp = a[11];
						tampon_passwd_ftp = a[12];

						strcpy(mot2,"mkdir -p ");
						strcat(mot2,home_dir);
						strcat(mot2,"/MOUNT/");
						strcat(mot2,tampon_sftp);
						int systemRet =system (mot2);
						if(systemRet == -1){return;}

						strcpy(mot3,"curlftpfs ");
						strcat(mot3,tampon_utilisateur_ftp);
						strcat(mot3,":");				
						strcat(mot3,tampon_passwd_ftp);
						strcat(mot3,"@");
						strcat(mot3,tampon_sftp);		

						strcat(mot3," ");
						strcat(mot3,home_dir);
						strcat(mot3,"/MOUNT/");
						strcat(mot3,tampon_sftp);
		
						strcat(liste_mount,"fusermount -u ");
						strcat(liste_mount,home_dir);
						strcat(liste_mount,"/MOUNT/");
						strcat(liste_mount,tampon_sftp);
						strcat(liste_mount," ; ");

						systemRet =system (mot3);
						if(systemRet == -1){return;}

						char total_path[300];total_path[0]='\0';
						strcat(total_path,home_dir);
						strcat(total_path,"/MOUNT/");
						strcat(total_path,tampon_sftp);		
						gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooserwidget2) ,total_path);

						icon_log_logmemo();
						log_to_memo (_("%s mount FTP in MOUNT/"), tampon_sftp, LM_NORMAL);
						statusbar_msg (_("Mount [OK]"));
						griffon_notify(_("Mount FTP"));
						icon_affiche_net ();
						ftp_reload();
						}

					}

				}

			mot[0]='\0';
			}
			else
			{
				strncat(mot,&carac,1);
			}
		}
	fclose(fich);
	}

}

//*********************** WINDOW DEBUG MAKE PROJECT
void window_make_project ()
{
	gtk_widget_destroy (window_run);

	window_run = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for(GTK_WINDOW(window_run),GTK_WINDOW(tea_main_window));
	gtk_window_resize (GTK_WINDOW (window_run), 900, 420);
	
	GtkWidget * vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (window_run), GTK_WIDGET(vbox2));

	GtkWidget *toolbar_myadmin;
	toolbar_myadmin = gtk_toolbar_new ();

	GtkToolItem *tool_exe = gtk_tool_button_new_from_stock(GTK_STOCK_EXECUTE  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_exe, -1);
	g_signal_connect ((gpointer) tool_exe, "clicked",G_CALLBACK (run_make_project),NULL);
	gtk_tool_item_set_tooltip_text(tool_exe,_("Run script/bin"));

	GtkToolItem *tool_chmod = gtk_tool_button_new_from_stock(GTK_STOCK_APPLY  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_chmod, -1);
	g_signal_connect ((gpointer) tool_chmod, "clicked",G_CALLBACK (exe),NULL);
	gtk_tool_item_set_tooltip_text(tool_chmod,_("Chmod u+x script/bin"));

	GtkToolItem *tool_sep=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_sep, -1);


	GtkToolItem *tool_clear = gtk_tool_button_new_from_stock(GTK_STOCK_CLEAR  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_clear, -1);
	g_signal_connect ((gpointer) tool_clear, "clicked",G_CALLBACK (clear_debug),NULL);
	gtk_tool_item_set_tooltip_text(tool_clear,_("Clear terminal"));

	gtk_box_pack_start (GTK_BOX (vbox2), toolbar_myadmin, FALSE, FALSE, 0);
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_myadmin), GTK_TOOLBAR_ICONS);

	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar_myadmin),GTK_ICON_SIZE_SMALL_TOOLBAR);

	vte_add = vte_terminal_new();

	char** startterm=NULL;
	g_shell_parse_argv("/bin/bash", NULL, &startterm, NULL);

	vte_terminal_fork_command_full(VTE_TERMINAL(vte_add),VTE_PTY_DEFAULT,NULL, startterm, NULL,  (GSpawnFlags)(G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_SEARCH_PATH),  
	NULL, 
	NULL, 
	NULL, 
	NULL);

	vte_terminal_set_background_image_file (VTE_TERMINAL(vte_add),"/usr/local/share/griffon/images/griffon_bg2.png");
	vte_terminal_set_background_saturation (VTE_TERMINAL(vte_add),0.3);

	vte_terminal_set_scroll_on_keystroke(VTE_TERMINAL (vte_add), TRUE);
	gtk_container_add (GTK_CONTAINER (vbox2), GTK_WIDGET(vte_add));	

	vte_terminal_feed_child (VTE_TERMINAL(vte_add),"PS1=\"[\\d \\t]\"\n",-1);
	vte_terminal_feed_child (VTE_TERMINAL(vte_add),"clear\n",-1);

	g_signal_connect (vte_add, "button-press-event", G_CALLBACK (popup_context_menu_vte), NULL);
	gtk_widget_show (GTK_WIDGET(vbox2));
	gtk_widget_show (GTK_WIDGET(vte_add));
	gtk_widget_show_all (GTK_WIDGET(window_run));
	gtk_widget_show (GTK_WIDGET(toolbar_myadmin)); 
	gtk_widget_show(GTK_WIDGET(tool_clear));
	gtk_widget_show(GTK_WIDGET(tool_sep));
	gtk_widget_show(GTK_WIDGET(tool_exe));
	gtk_widget_show(GTK_WIDGET(tool_chmod));

	run_make_project();
}

//*********************** LANCER LE DEBUG
void run_make_project ()
{
				if (p_dir_makefile)
				{
				clear_debug();
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),"cd ",-1);
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),p_dir_makefile,-1);
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),"\n",-1);
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),p_cmd_make,-1);
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),"\n",-1);
				}

	statusbar_msg (_("Make Debug"));
}

//*********************** WINDOW DEBUG RUN SCRIPT/PROG PROJECT
void window_debug_project ()
{
	gtk_widget_destroy (window_run);

	window_run = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for(GTK_WINDOW(window_run),GTK_WINDOW(tea_main_window));
	gtk_window_resize (GTK_WINDOW (window_run), 900, 420);
	
	GtkWidget * vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (window_run), GTK_WIDGET(vbox2));

	GtkWidget *toolbar_myadmin;
	toolbar_myadmin = gtk_toolbar_new ();

	GtkToolItem *tool_exe = gtk_tool_button_new_from_stock(GTK_STOCK_EXECUTE  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_exe, -1);
	g_signal_connect ((gpointer) tool_exe, "clicked",G_CALLBACK (run_debug_project),NULL);
	gtk_tool_item_set_tooltip_text(tool_exe,_("Run script/bin"));

	GtkToolItem *tool_chmod = gtk_tool_button_new_from_stock(GTK_STOCK_APPLY  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_chmod, -1);
	g_signal_connect ((gpointer) tool_chmod, "clicked",G_CALLBACK (exe),NULL);
	gtk_tool_item_set_tooltip_text(tool_chmod,_("Chmod u+x script/bin"));

	GtkToolItem *tool_sep=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_sep, -1);


	GtkToolItem *tool_clear = gtk_tool_button_new_from_stock(GTK_STOCK_CLEAR  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_clear, -1);
	g_signal_connect ((gpointer) tool_clear, "clicked",G_CALLBACK (clear_debug),NULL);
	gtk_tool_item_set_tooltip_text(tool_clear,_("Clear terminal"));

	gtk_box_pack_start (GTK_BOX (vbox2), toolbar_myadmin, FALSE, FALSE, 0);
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_myadmin), GTK_TOOLBAR_ICONS);

	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar_myadmin),GTK_ICON_SIZE_SMALL_TOOLBAR);

	vte_add = vte_terminal_new();

	char** startterm=NULL;
	g_shell_parse_argv("/bin/bash", NULL, &startterm, NULL);

	vte_terminal_fork_command_full(VTE_TERMINAL(vte_add),VTE_PTY_DEFAULT,NULL, startterm, NULL,  (GSpawnFlags)(G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_SEARCH_PATH),  
	NULL, 
	NULL, 
	NULL, 
	NULL);

	vte_terminal_set_background_image_file (VTE_TERMINAL(vte_add),"/usr/local/share/griffon/images/griffon_bg2.png");
	vte_terminal_set_background_saturation (VTE_TERMINAL(vte_add),0.3);

	vte_terminal_set_scroll_on_keystroke(VTE_TERMINAL (vte_add), TRUE);
	gtk_container_add (GTK_CONTAINER (vbox2), GTK_WIDGET(vte_add));	

	vte_terminal_feed_child (VTE_TERMINAL(vte_add),"PS1=\"[\\d \\t]\"\n",-1);
	vte_terminal_feed_child (VTE_TERMINAL(vte_add),"clear\n",-1);

	g_signal_connect (vte_add, "button-press-event", G_CALLBACK (popup_context_menu_vte), NULL);
	gtk_widget_show (GTK_WIDGET(vbox2));
	gtk_widget_show (GTK_WIDGET(vte_add));
	gtk_widget_show_all (GTK_WIDGET(window_run));
	gtk_widget_show (GTK_WIDGET(toolbar_myadmin)); 
	gtk_widget_show(GTK_WIDGET(tool_clear));
	gtk_widget_show(GTK_WIDGET(tool_sep));
	gtk_widget_show(GTK_WIDGET(tool_exe));
	gtk_widget_show(GTK_WIDGET(tool_chmod));

	run_debug_project();
}

//*********************** LANCER LE DEBUG
void run_debug_project ()
{

				if (p_dir_makefile)
				{
				clear_debug();
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),"cd ",-1);
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),p_dir_makefile,-1);
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),"\n",-1);
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),p_cmd_make,-1);
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),"\n",-1);
				}

				if (p_file_executable)
				{
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),p_file_executable,-1);
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),"\n",-1);
				}

	statusbar_msg (_("Run Debug Project"));
}

//********************* DELETE PROJECTS
void delete_project(gpointer data)
{
	FILE *fich;
	char carac;
	int ligne=1;
	char file[9000];
	file[0]='\0';

	if(fopen(confile.projects,"rt"))
	{
	fich=fopen(confile.projects,"r");

		while ((carac =fgetc(fich)) != EOF)
		{
			if (carac =='\n')
			{
				if(ligne!=(int)data){strncat(file,"\n",1);}
				ligne++;
			}
			else
			{
				if(ligne!=(int)data)
				{
					strncat(file,&carac,1);
				}
			}
		}
	fclose(fich);
	}

	if(strlen(file)>5)
	{
		save_string_to_file_vide(confile.projects,"");
		save_string_to_file_add(confile.projects,file);
	}
	else
	{
		save_string_to_file_vide(confile.projects," ");
	}

	file[0]='\0';

	load_projects_list();
}

//********************* CLEAR LOG INFO
void clear_info()
{
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(log_memo_textbuffer), "", -1);

	GdkPixbuf *pixbuf;
	GtkTextBuffer* buffer_img;
	GtkTextIter itFin;
	pixbuf = gdk_pixbuf_new_from_file("/usr/local/share/griffon/images/griffon_start.png", NULL);
	buffer_img = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv_logmemo));
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer_img), &itFin);
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer_img), &itFin);
	gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(buffer_img),&itFin,pixbuf);
	tv_logmemo_set_pos (0);
	log_to_memo ("\n\n(C)2014 Philippe Muller <pmuller@lasotel.fr>\n Griffon 1.7.2 - http://griffon.lasotel.fr\n\n", NULL, LM_GREET); 
	tv_logmemo_set_pos (0);
	tv_logmemo_set_pos (0);
}

//********************* AFFICHAGE CHANGELOG
void show_changelogs()
{
	if (! get_page_text()) return;
	GtkWidget *window1;  

	window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window1), _((_("ChangeLogs"))));
	gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);
	gtk_widget_show(GTK_WIDGET(window1));
	gtk_window_resize (GTK_WINDOW (window1), 900, 500);

	GtkWidget *vbox3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (window1), GTK_WIDGET(vbox3));
	gtk_widget_show (GTK_WIDGET(vbox3));  

	GtkWidget *toolbar_myadmin;
	toolbar_myadmin = gtk_toolbar_new ();

	GtkToolItem *tool_exe = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE_AS  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_exe, -1);
	g_signal_connect ((gpointer) tool_exe, "clicked",G_CALLBACK (new_file_changelogs),NULL);
	gtk_tool_item_set_tooltip_text(tool_exe,_("Save ChangeLogs"));

	GtkToolItem *tool_chmod = gtk_tool_button_new_from_stock(GTK_STOCK_CLEAR  );
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_myadmin), tool_chmod, -1);
	g_signal_connect ((gpointer) tool_chmod, "clicked",G_CALLBACK (clean_file_changelogs),NULL);
	gtk_tool_item_set_tooltip_text(tool_chmod,_("Clean Changelogs"));

	gtk_box_pack_start (GTK_BOX (vbox3), toolbar_myadmin, FALSE, FALSE, 0);
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_myadmin), GTK_TOOLBAR_ICONS);

	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar_myadmin),GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_show_all (GTK_WIDGET(toolbar_myadmin));  

	GtkWidget *scrolledwindow4 = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (GTK_WIDGET(scrolledwindow4));
	gtk_box_pack_start(GTK_BOX(vbox3), GTK_WIDGET(scrolledwindow4), TRUE, TRUE, 1);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow4), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_placement (GTK_SCROLLED_WINDOW (scrolledwindow4), GTK_CORNER_TOP_LEFT);

	PangoFontDescription *font_desc_note;
	GtkWidget *sView_note;
	GtkSourceLanguageManager *lm_note;
	GtkSourceLanguage *language_note = NULL;

	buffer_note2 = GTK_SOURCE_BUFFER (gtk_source_buffer_new (NULL));

	sView_note = gtk_source_view_new_with_buffer(buffer_note2);
	font_desc_note = pango_font_description_from_string ("mono 8");
	gtk_widget_modify_font (sView_note, font_desc_note);
	pango_font_description_free (font_desc_note);

	gtk_source_view_set_show_right_margin(GTK_SOURCE_VIEW(sView_note),TRUE);
	gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(sView_note),TRUE);
	gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(sView_note),TRUE);
	gtk_source_view_set_show_line_marks(GTK_SOURCE_VIEW(sView_note),TRUE);

	lm_note = gtk_source_language_manager_new();
	g_object_ref (lm_note);
	g_object_set_data_full ( G_OBJECT (buffer_note2), "languages-manager",lm_note, (GDestroyNotify) g_object_unref);

	lm_note = g_object_get_data (G_OBJECT (buffer_note2), "languages-manager");
	language_note = gtk_source_language_manager_get_language (lm_note,"diff");
	gtk_source_buffer_set_language (buffer_note2, language_note);

	gtk_container_add (GTK_CONTAINER (scrolledwindow4), GTK_WIDGET(sView_note));
	gtk_widget_show_all (GTK_WIDGET(scrolledwindow4));

	gchar *fname = g_path_get_basename (cur_text_doc->file_name);
	gchar *changelog_file = g_strconcat (confile.changelog,"/",fname, NULL); 

	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(buffer_note2), "", -1);
	gchar lecture[1024];
	FILE *fichier;
	fichier = fopen(changelog_file,"rt");

	if(fichier!=NULL)
	{
		while(fgets(lecture, 1024, fichier))
		{
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_note2),g_locale_to_utf8(lecture, -1, NULL, NULL, NULL) , -1);
		}
	fclose(fichier);
	}


		g_free (changelog_file);
		g_free (fname);
}

//*********************** CREAT FICHIER DE CHANGELOGS
void new_file_changelogs ()
{
	file_new();
	gchar *buf = doc_get_buf (GTK_TEXT_BUFFER(buffer_note2));
	doc_insert_at_cursor (cur_text_doc,buf);
}

//*********************** CLEAN FICHIER DE CHANGELOGS
void clean_file_changelogs ()
{
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(buffer_note2), "", -1);
	gchar *fname = g_path_get_basename (cur_text_doc->file_name);
	gchar *changelog_file = g_strconcat (confile.changelog,"/",fname, NULL); 
	create_empty_file (changelog_file, "");
}

//*********************** WINDOW UPDATE
void window_update (GtkWidget *widget,gpointer data)
{
	gtk_widget_destroy (window_run);
	gtk_widget_get_name(widget);
	window_run = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for(GTK_WINDOW(window_run),GTK_WINDOW(tea_main_window));
	gtk_window_resize (GTK_WINDOW (window_run), 800, 400);
	
	GtkWidget * vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (window_run), GTK_WIDGET(vbox2));

	vte_add = vte_terminal_new();

	char** startterm=NULL;
	g_shell_parse_argv("/bin/bash", NULL, &startterm, NULL);

	vte_terminal_fork_command_full(VTE_TERMINAL(vte_add),VTE_PTY_DEFAULT,NULL, startterm, NULL,  (GSpawnFlags)(G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_SEARCH_PATH),  
	NULL, 
	NULL, 
	NULL, 
	NULL);

	vte_terminal_set_background_image_file (VTE_TERMINAL(vte_add),"/usr/local/share/griffon/images/griffon_bg2.png");
	vte_terminal_set_background_saturation (VTE_TERMINAL(vte_add),0.3);

	vte_terminal_set_scroll_on_keystroke(VTE_TERMINAL (vte_add), TRUE);
	gtk_container_add (GTK_CONTAINER (vbox2), GTK_WIDGET(vte_add));	

	vte_terminal_feed_child (VTE_TERMINAL(vte_add),"PS1=\"[\\d \\t]\"\n",-1);
	vte_terminal_feed_child (VTE_TERMINAL(vte_add),"clear\n",-1);

	g_signal_connect (vte_add, "button-press-event", G_CALLBACK (popup_context_menu_vte), NULL);
	gtk_widget_show (GTK_WIDGET(vbox2));
	gtk_widget_show (GTK_WIDGET(vte_add));
	gtk_widget_show_all (GTK_WIDGET(window_run));

	if((int)data==1){update_griffon_ubuntu ();}
	if((int)data==2){update_griffon_ubuntu_beta ();}
}

//*********************** UPDATE GRIFFON IDE
void update_griffon_ubuntu ()
{
	vte_terminal_feed_child (VTE_TERMINAL(vte_add),"cd /tmp/ > /dev/null\n",-1);
	vte_terminal_feed_child (VTE_TERMINAL(vte_add),"[ ! -e /tmp/update.sh ] || rm -f /tmp/update.sh\n",-1);
	vte_terminal_feed_child (VTE_TERMINAL(vte_add),"clear && wget http://griffon.lasotel.fr/update.sh\n",-1);
	vte_terminal_feed_child (VTE_TERMINAL(vte_add),"chmod u+x update.sh > /dev/null\n",-1);
	vte_terminal_feed_child (VTE_TERMINAL(vte_add),"clear && ./update.sh\n",-1);
}

//*********************** UPDATE GRIFFON IDE
void update_griffon_ubuntu_beta ()
{
	vte_terminal_feed_child (VTE_TERMINAL(vte_add),"cd /tmp/ > /dev/null\n",-1);
	vte_terminal_feed_child (VTE_TERMINAL(vte_add),"[ ! -e /tmp/update_beta.sh ] || rm -f /tmp/update_beta.sh\n",-1);
	vte_terminal_feed_child (VTE_TERMINAL(vte_add),"clear && wget http://griffon.lasotel.fr/update_beta.sh\n",-1);
	vte_terminal_feed_child (VTE_TERMINAL(vte_add),"chmod u+x update_beta.sh > /dev/null\n",-1);
	vte_terminal_feed_child (VTE_TERMINAL(vte_add),"clear && ./update_beta.sh\n",-1);
}

//*********************** PREVIEW WEB IN POPUP
void preview_web_popup ()
{
	if (! get_page_text()) return;

	gchar *buf = doc_get_sel (cur_text_doc);
	gchar *uri=NULL;

	if (! buf) return;

	 if(cur_text_doc->file_name!=NULL)
		{
			gchar **a = g_strsplit (cur_text_doc->file_name, "_", -1);
			if (strcmp("noname", a[0]) != 0 )
			{
				uri = g_strconcat("file://", cur_text_doc->file_name, NULL);
			}
		}

	window_popup_delete ();
	window1_popup = gtk_window_new (GTK_WINDOW_POPUP);
	gtk_window_set_transient_for(GTK_WINDOW(window1_popup),GTK_WINDOW(tea_main_window));
	gtk_window_set_title (GTK_WINDOW (window1_popup), _((_("Web preview"))));
	gtk_window_set_position (GTK_WINDOW (window1_popup), GTK_WIN_POS_CENTER);
		gtk_window_resize (GTK_WINDOW (window1_popup), 800, 600);
	gtk_widget_show(GTK_WIDGET(window1_popup));

	//gtk_widget_set_opacity(GTK_WIDGET(window1_popup),0.8);

	GtkWidget *vbox1;
	WebKitWebView *webView_doc;

	vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_show (GTK_WIDGET(vbox1));
	gtk_container_add (GTK_CONTAINER (window1_popup), GTK_WIDGET(vbox1));

	GtkWidget *scrolledwindow5 = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (GTK_WIDGET(scrolledwindow5));
	gtk_box_pack_start(GTK_BOX(vbox1), GTK_WIDGET(scrolledwindow5), TRUE, TRUE, 1);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow5), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_placement (GTK_SCROLLED_WINDOW (scrolledwindow5), GTK_CORNER_TOP_LEFT);

	webView_doc = WEBKIT_WEB_VIEW(webkit_web_view_new());
	gtk_widget_show (GTK_WIDGET(webView_doc));

	gtk_container_add(GTK_CONTAINER(scrolledwindow5), GTK_WIDGET(webView_doc));

	webkit_web_view_load_string (webView_doc,buf,NULL,NULL,uri);

	g_signal_connect(webView_doc, "new-window-policy-decision-requested",G_CALLBACK(myadmin_new_window), webView_doc);
	g_signal_connect(webView_doc, "create-web-view",G_CALLBACK(web_new_w_click_go), webView_doc);

	win_popup=1;
}

//*********************** PREVIEW WEB IN POPUP FULL
void preview_web_popup_full ()
{
	if (! get_page_text()) return;

		window_popup_delete ();
		GtkTextIter start;
		GtkTextIter end;
		gchar *buf;
		gtk_text_buffer_get_start_iter ((GtkTextBuffer *)cur_text_doc->text_buffer, &start);
		gtk_text_buffer_get_end_iter ((GtkTextBuffer *)cur_text_doc->text_buffer, &end);
		buf = gtk_text_buffer_get_text ((GtkTextBuffer *)cur_text_doc->text_buffer, &start, &end, FALSE);       

	gchar *uri=NULL;

	if (! buf) return;

	 if(cur_text_doc->file_name!=NULL)
		{
			gchar **a = g_strsplit (cur_text_doc->file_name, "_", -1);
			if (strcmp("noname", a[0]) != 0 )
			{
				uri = g_strconcat("file://", cur_text_doc->file_name, NULL);
			}
		}

	window1_popup = gtk_window_new (GTK_WINDOW_POPUP);
	gtk_window_set_transient_for(GTK_WINDOW(window1_popup),GTK_WINDOW(tea_main_window));
	gtk_window_set_title (GTK_WINDOW (window1_popup), _((_("Web preview"))));
	gtk_window_set_position (GTK_WINDOW (window1_popup), GTK_WIN_POS_CENTER);
	gtk_window_resize (GTK_WINDOW (window1_popup), 900, 700);

	gtk_widget_show(GTK_WIDGET(window1_popup));

	//gtk_widget_set_opacity(GTK_WIDGET(window1_popup),0.8);

	GtkWidget *vbox1;
	WebKitWebView *webView_doc;

	vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_show (GTK_WIDGET(vbox1));
	gtk_container_add (GTK_CONTAINER (window1_popup), GTK_WIDGET(vbox1));

	GtkWidget *scrolledwindow5 = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (GTK_WIDGET(scrolledwindow5));
	gtk_box_pack_start(GTK_BOX(vbox1), GTK_WIDGET(scrolledwindow5), TRUE, TRUE, 1);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow5), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_placement (GTK_SCROLLED_WINDOW (scrolledwindow5), GTK_CORNER_TOP_LEFT);

	webView_doc = WEBKIT_WEB_VIEW(webkit_web_view_new());
	gtk_widget_show (GTK_WIDGET(webView_doc));

	gtk_container_add(GTK_CONTAINER(scrolledwindow5), GTK_WIDGET(webView_doc));

	webkit_web_view_load_string (webView_doc,buf,NULL,NULL,uri);

	g_signal_connect(webView_doc, "new-window-policy-decision-requested",G_CALLBACK(myadmin_new_window), webView_doc);
	g_signal_connect(webView_doc, "create-web-view",G_CALLBACK(web_new_w_click_go), webView_doc);

	win_popup=1;
	g_free (buf);
	g_free (uri);
}

//*********************** WINDOW POPUP HTM PREVIEW DELETE
gboolean window_popup_delete ()
{
	if(win_popup!=0)
	{
	gtk_widget_destroy (window1_popup);
	win_popup=0;
	return TRUE;
	}

	return FALSE;
}

//*********************** WINDOW TWEETER INFO
void window_tweeter_info ()
{
	GtkWidget *window1;
	GtkWidget *vbox1;
	WebKitWebView *webView_doc2;

	window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for(GTK_WINDOW(window1),GTK_WINDOW(tea_main_window));
	gtk_window_set_title (GTK_WINDOW (window1), _((_("Griffon IDE Version"))));
	gtk_window_resize (GTK_WINDOW (window1), 530, 645);
	gtk_widget_show (GTK_WIDGET(window1));

	vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_show (GTK_WIDGET(vbox1));
	gtk_container_add (GTK_CONTAINER (window1), GTK_WIDGET(vbox1));

	GtkWidget *scrolledwindow5 = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (GTK_WIDGET(scrolledwindow5));
	gtk_box_pack_start(GTK_BOX(vbox1), GTK_WIDGET(scrolledwindow5), TRUE, TRUE, 1);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow5), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_placement (GTK_SCROLLED_WINDOW (scrolledwindow5), GTK_CORNER_TOP_LEFT);


	webView_doc2 = WEBKIT_WEB_VIEW(webkit_web_view_new());
	gtk_widget_show (GTK_WIDGET(webView_doc2));

	gtk_container_add(GTK_CONTAINER(scrolledwindow5), GTK_WIDGET(webView_doc2));

	webkit_web_view_load_uri(webView_doc2, "http://griffon.lasotel.fr/tweeter.html");

	g_signal_connect(webView_doc2, "new-window-policy-decision-requested",G_CALLBACK(myadmin_new_window), webView_doc2);
	g_signal_connect(webView_doc2, "create-web-view",G_CALLBACK(web_new_w_click_go), webView_doc2);


}

//*********************** WINDOW DevDoc
void window_devdocs ()
{
	GtkWidget *window1;
	GtkWidget *vbox1;
	WebKitWebView *webView_doc2;

	window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for(GTK_WINDOW(window1),GTK_WINDOW(tea_main_window));
	gtk_window_set_title (GTK_WINDOW (window1), _((_("Griffon IDE Version"))));
	gtk_window_resize (GTK_WINDOW (window1), 900, 700);
	gtk_widget_show (GTK_WIDGET(window1));

	vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_show (GTK_WIDGET(vbox1));
	gtk_container_add (GTK_CONTAINER (window1), GTK_WIDGET(vbox1));

	GtkWidget *scrolledwindow5 = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (GTK_WIDGET(scrolledwindow5));
	gtk_box_pack_start(GTK_BOX(vbox1), GTK_WIDGET(scrolledwindow5), TRUE, TRUE, 1);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow5), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_placement (GTK_SCROLLED_WINDOW (scrolledwindow5), GTK_CORNER_TOP_LEFT);


	webView_doc2 = WEBKIT_WEB_VIEW(webkit_web_view_new());
	gtk_widget_show (GTK_WIDGET(webView_doc2));

	gtk_container_add(GTK_CONTAINER(scrolledwindow5), GTK_WIDGET(webView_doc2));

	webkit_web_view_load_uri(webView_doc2, "http://devdocs.io/");

	g_signal_connect(webView_doc2, "new-window-policy-decision-requested",G_CALLBACK(myadmin_new_window), webView_doc2);
	g_signal_connect(webView_doc2, "create-web-view",G_CALLBACK(web_new_w_click_go), webView_doc2);
}

//*********************** WINDO DIALOG
void quick_message (gchar *message,gchar *message2)
{
	GtkWidget *dialog = gtk_message_dialog_new ((GtkWindow*)tea_main_window,
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_INFO,
			GTK_BUTTONS_CLOSE,
			message,
			NULL);

	gtk_message_dialog_format_secondary_text((GtkMessageDialog *)dialog,message2,NULL);

	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

//*********************** FIND WEB TEXT MYADMIN
void web_find_myadmin ()
{
	webkit_web_view_unmark_text_matches(WEBKIT_WEB_VIEW (webView_myadmin));
	gchar *search=gtk_editable_get_chars(GTK_EDITABLE(entry_find_web),0, -1);
	webkit_web_view_mark_text_matches (WEBKIT_WEB_VIEW (webView_myadmin), search, FALSE, 0);
	webkit_web_view_set_highlight_text_matches (WEBKIT_WEB_VIEW (webView_myadmin), TRUE);
	webkit_web_view_search_text (WEBKIT_WEB_VIEW (webView_myadmin), search, FALSE, TRUE, TRUE);
}

//*********************** FIND WEB TEXT MINIWEB
void web_find_mini ()
{
	webkit_web_view_unmark_text_matches(WEBKIT_WEB_VIEW (webView));
	gchar *search=gtk_editable_get_chars(GTK_EDITABLE(entry_find_web_mini),0, -1);
	webkit_web_view_mark_text_matches (WEBKIT_WEB_VIEW (webView), search, FALSE, 0);
	webkit_web_view_set_highlight_text_matches (WEBKIT_WEB_VIEW (webView), TRUE);
	webkit_web_view_search_text (WEBKIT_WEB_VIEW (webView), search, FALSE, TRUE, TRUE);
}

//*********************** FIND WEB TEXT SEARCH/HELP
void web_find_web_help ()
{
	webkit_web_view_unmark_text_matches(WEBKIT_WEB_VIEW (webView_myadmin_aide));
	gchar *search=gtk_editable_get_chars(GTK_EDITABLE(entry_find_web_help),0, -1);
	webkit_web_view_mark_text_matches (WEBKIT_WEB_VIEW (webView_myadmin_aide), search, FALSE, 0);
	webkit_web_view_set_highlight_text_matches (WEBKIT_WEB_VIEW (webView_myadmin_aide), TRUE);
	webkit_web_view_search_text (WEBKIT_WEB_VIEW (webView_myadmin_aide), search, FALSE, TRUE, TRUE);
}

//*********************** PREVIEW WEB IN POPUP
gboolean preview_web_popup_line ()
{
	if (! get_page_text()) return FALSE;

	GtkTextIter itstart,iter;
	GtkTextIter itend;
	gint row;
	char *extension;
	gint win_x, win_y;
	GdkRectangle buf_loc;
	gint x, y;
	GdkWindow *win;

	gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(cur_text_doc->text_buffer),&iter, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(cur_text_doc->text_buffer)));
	row = gtk_text_iter_get_line(&iter);
	gtk_text_buffer_get_iter_at_line((GtkTextBuffer *)cur_text_doc->text_buffer,&itstart,row);
	gtk_text_buffer_get_iter_at_line((GtkTextBuffer *)cur_text_doc->text_buffer,&itend,row+1);

  gtk_text_view_get_iter_location (GTK_TEXT_VIEW (cur_text_doc->text_view), &iter, &buf_loc);

  gtk_text_view_buffer_to_window_coords (GTK_TEXT_VIEW (cur_text_doc->text_view),
                                         GTK_TEXT_WINDOW_WIDGET,
                                         buf_loc.x, buf_loc.y,
                                         &win_x, &win_y);
  win = gtk_text_view_get_window (GTK_TEXT_VIEW (cur_text_doc->text_view), 
                                  GTK_TEXT_WINDOW_WIDGET);
  gdk_window_get_origin (win, &x, &y);
  gtk_window_move (GTK_WINDOW (window1_popup_line), win_x + x + 25, win_y + y + buf_loc.height - 125);


	gchar *buf=gtk_text_buffer_get_text((GtkTextBuffer *)cur_text_doc->text_buffer,&itstart,&itend,FALSE);
	gchar *uri=NULL;

	if (! buf) return FALSE;

	 if(cur_text_doc->file_name!=NULL)
		{
			extension = strrchr(cur_text_doc->file_name,'.');
			gchar **a = g_strsplit (cur_text_doc->file_name, "_", -1);
			if (strcmp("noname", a[0]) != 0 )
			{
				uri = g_strconcat("file://", cur_text_doc->file_name, NULL);
			}else{gtk_widget_hide(window1_popup_line);return FALSE;}
		}

	if (strcmp(".htm", extension) == 0 || strcmp(".html", extension) == 0 || strcmp(".php", extension) == 0 )
	{
		window_popup_delete ();
		win_popup_line=1;

		gtk_widget_show(GTK_WIDGET(window1_popup_line));
		webkit_web_view_load_string (webView_doc_line,buf,NULL,NULL,uri);

		g_signal_connect(webView_doc_line, "new-window-policy-decision-requested",G_CALLBACK(myadmin_new_window), webView_doc_line);
		g_signal_connect(webView_doc_line, "create-web-view",G_CALLBACK(web_new_w_click_go), webView_doc_line);

		if (g_strrstr(buf,"<")==NULL){gtk_widget_hide(window1_popup_line);return FALSE;}
		if (g_strrstr(buf,">")==NULL){gtk_widget_hide(window1_popup_line);return FALSE;}

	}
	else{gtk_widget_hide(window1_popup_line);win_popup_line=0;}

	return FALSE;
}

//*********************** PREVIEW WEB IN POPUP SEARCH
gboolean preview_web_popup_search ()
{
	if (! get_page_text()) return FALSE;

	GtkTextIter itstart,iter;
	GtkTextIter itend;
	gint row;
	gint win_x, win_y;
	GdkRectangle buf_loc;
	gint x, y;
	GdkWindow *win;

	gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(cur_text_doc->text_buffer),&iter, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(cur_text_doc->text_buffer)));
	row = gtk_text_iter_get_line(&iter);
	gtk_text_buffer_get_iter_at_line((GtkTextBuffer *)cur_text_doc->text_buffer,&itstart,row);
	gtk_text_buffer_get_iter_at_line((GtkTextBuffer *)cur_text_doc->text_buffer,&itend,row+1);

  gtk_text_view_get_iter_location (GTK_TEXT_VIEW (cur_text_doc->text_view), &iter, &buf_loc);

  gtk_text_view_buffer_to_window_coords (GTK_TEXT_VIEW (cur_text_doc->text_view),
                                         GTK_TEXT_WINDOW_WIDGET,
                                         buf_loc.x, buf_loc.y,
                                         &win_x, &win_y);
  win = gtk_text_view_get_window (GTK_TEXT_VIEW (cur_text_doc->text_view), 
                                  GTK_TEXT_WINDOW_WIDGET);
  gdk_window_get_origin (win, &x, &y);
  gtk_window_move (GTK_WINDOW (window1_popup_line), win_x + x, win_y + y + buf_loc.height);

	char search_google[980];

	if(doc_get_sel (cur_text_doc))
	{
		strcpy(search_google,"<table><tr><td><img src=\"http://griffon.lasotel.fr/images/doc.png\"></td><td><a target=\"_blank\" href=\"https://www.google.fr/#q=");
		strcat(search_google,doc_get_sel (cur_text_doc)); 
		strcat(search_google,"\">Google search</a></td></tr></table>"); 
	}

	gchar *uri=NULL;

		window_popup_delete ();
		win_popup_line=1;
		gtk_widget_show(GTK_WIDGET(window1_popup_line));
		webkit_web_view_load_string (webView_doc_line,search_google,NULL,NULL,uri);

		g_signal_connect(webView_doc_line, "new-window-policy-decision-requested",G_CALLBACK(myadmin_new_window), webView_doc_line);
		g_signal_connect(webView_doc_line, "create-web-view",G_CALLBACK(web_new_w_click_go), webView_doc_line);

	return FALSE;
}

//*********************** HIDDEN POPUP
gboolean hidden_popup ()
{
	if(win_popup_line==1)
	{
	gtk_widget_hide(window1_popup_line);
	win_popup_line=0;
	}
	return FALSE;
}

//*********************** PREVIEW WEB IN POPUP TRANSLATE
gboolean preview_web_popup_translate ()
{
	if (! get_page_text()) return FALSE;

	GtkTextIter itstart,iter;
	GtkTextIter itend;
	gint row;
	gint win_x, win_y;
	GdkRectangle buf_loc;
	gint x, y;
	GdkWindow *win;

	gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(cur_text_doc->text_buffer),&iter, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(cur_text_doc->text_buffer)));
	row = gtk_text_iter_get_line(&iter);
	gtk_text_buffer_get_iter_at_line((GtkTextBuffer *)cur_text_doc->text_buffer,&itstart,row);
	gtk_text_buffer_get_iter_at_line((GtkTextBuffer *)cur_text_doc->text_buffer,&itend,row+1);

  gtk_text_view_get_iter_location (GTK_TEXT_VIEW (cur_text_doc->text_view), &iter, &buf_loc);

  gtk_text_view_buffer_to_window_coords (GTK_TEXT_VIEW (cur_text_doc->text_view),
                                         GTK_TEXT_WINDOW_WIDGET,
                                         buf_loc.x, buf_loc.y,
                                         &win_x, &win_y);
  win = gtk_text_view_get_window (GTK_TEXT_VIEW (cur_text_doc->text_view), 
                                  GTK_TEXT_WINDOW_WIDGET);
  gdk_window_get_origin (win, &x, &y);
  gtk_window_move (GTK_WINDOW (window1_popup_line), win_x + x, win_y + y + buf_loc.height);

	char search_google[980];

	if(doc_get_sel (cur_text_doc))
	{
		strcpy(search_google,"<table><tr><td><img src=\"http://griffon.lasotel.fr/images/contribution.png\"></td><td><a target=\"_blank\" href=\"https://translate.google.fr/?hl=fr&tab=wT#fr/en/");
		strcat(search_google,doc_get_sel (cur_text_doc)); 
		strcat(search_google,"\">Google translation</a></td></tr></table>"); 
	}

	gchar *uri=NULL;

		window_popup_delete ();
		win_popup_line=1;
		gtk_widget_show(GTK_WIDGET(window1_popup_line));
		webkit_web_view_load_string (webView_doc_line,search_google,NULL,NULL,uri);

		g_signal_connect(webView_doc_line, "new-window-policy-decision-requested",G_CALLBACK(myadmin_new_window), webView_doc_line);
		g_signal_connect(webView_doc_line, "create-web-view",G_CALLBACK(web_new_w_click_go), webView_doc_line);

	return FALSE;
}

//*********************** PREVIEW WEB IN POPUP TRANSLATE
gboolean preview_web_popup_translate2 ()
{
	if (! get_page_text()) return FALSE;

	GtkTextIter itstart,iter;
	GtkTextIter itend;
	gint row;
	gint win_x, win_y;
	GdkRectangle buf_loc;
	gint x, y;
	GdkWindow *win;

	gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(cur_text_doc->text_buffer),&iter, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(cur_text_doc->text_buffer)));
	row = gtk_text_iter_get_line(&iter);
	gtk_text_buffer_get_iter_at_line((GtkTextBuffer *)cur_text_doc->text_buffer,&itstart,row);
	gtk_text_buffer_get_iter_at_line((GtkTextBuffer *)cur_text_doc->text_buffer,&itend,row+1);

  gtk_text_view_get_iter_location (GTK_TEXT_VIEW (cur_text_doc->text_view), &iter, &buf_loc);

  gtk_text_view_buffer_to_window_coords (GTK_TEXT_VIEW (cur_text_doc->text_view),
                                         GTK_TEXT_WINDOW_WIDGET,
                                         buf_loc.x, buf_loc.y,
                                         &win_x, &win_y);
  win = gtk_text_view_get_window (GTK_TEXT_VIEW (cur_text_doc->text_view), 
                                  GTK_TEXT_WINDOW_WIDGET);
  gdk_window_get_origin (win, &x, &y);
  gtk_window_move (GTK_WINDOW (window1_popup_line), win_x + x, win_y + y + buf_loc.height);

	char search_google[980];

	if(doc_get_sel (cur_text_doc))
	{
		strcpy(search_google,"<table><tr><td><img src=\"http://griffon.lasotel.fr/images/contribution.png\"></td><td><a target=\"_blank\" href=\"https://translate.google.fr/?hl=fr&tab=wT#en/fr/");
		strcat(search_google,doc_get_sel (cur_text_doc)); 
		strcat(search_google,"\">Google translation</a></td></tr></table>"); 
	}

	gchar *uri=NULL;

		window_popup_delete ();
		win_popup_line=1;
		gtk_widget_show(GTK_WIDGET(window1_popup_line));
		webkit_web_view_load_string (webView_doc_line,search_google,NULL,NULL,uri);

		g_signal_connect(webView_doc_line, "new-window-policy-decision-requested",G_CALLBACK(myadmin_new_window), webView_doc_line);
		g_signal_connect(webView_doc_line, "create-web-view",G_CALLBACK(web_new_w_click_go), webView_doc_line);

	return FALSE;
}

//************************ ADD ITEM MENU POPUP
void populate_popup(GtkTextView *view, GtkMenu *menu, gpointer user_data)
{
	GtkWidget *i;

	if(user_data==NULL){printf(" ");}
	if(view==NULL){printf(" ");}

	i = gtk_separator_menu_item_new();
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), i);
	gtk_widget_show(i);

	i = gtk_menu_item_new_with_label("Search with Google [F8]");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), i);
	g_signal_connect(i, "button-release-event",G_CALLBACK(google_search), NULL);
	gtk_widget_show(i);

	i = gtk_menu_item_new_with_label("HTML Preview text selection in a web popup [Ctrl+P]");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), i);
	g_signal_connect(i, "button-release-event",G_CALLBACK(preview_web_popup), NULL);
	gtk_widget_show(i);

	i = gtk_menu_item_new_with_label("HTML Preview full text in a web popup [Ctrl+Alt+P]");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), i);
	g_signal_connect(i, "button-release-event",G_CALLBACK(preview_web_popup_full), NULL);
	gtk_widget_show(i);

	i = gtk_menu_item_new_with_label("Project : Make [Shift+F11]");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), i);
	g_signal_connect(i, "button-release-event",G_CALLBACK(window_make_project), NULL);
	gtk_widget_show(i);

	i = gtk_menu_item_new_with_label("Project : Run [Shift+F12]");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), i);
	g_signal_connect(i, "button-release-event",G_CALLBACK(window_debug_project), NULL);
	gtk_widget_show(i);

	i = gtk_menu_item_new_with_label("[BETA TEST] Code folding ALL");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), i);
	g_signal_connect(i, "button-release-event",G_CALLBACK(code_folding_all), NULL);
	gtk_widget_show(i);

	i = gtk_menu_item_new_with_label("[BETA TEST] Remove ALL code folding");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), i);
	g_signal_connect(i, "button-release-event",G_CALLBACK(clear_code_folding), NULL);
	gtk_widget_show(i);

}

//*********************** CODE FOLDING
void code_folding ()
{
	if (! get_page_text()) return;
	GtkTextIter start,itstart;
	GtkTextIter end;
	int controle=0;

	GtkTextIter tmp1;
	gint c1 = 0;

	GtkTextMark *m1 = gtk_text_buffer_get_insert (GTK_TEXT_BUFFER(cur_text_doc->text_buffer)); 
	gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &tmp1, m1);

	while (c1 != -1)
	{
		controle++;
		if(controle==1800){return;}

		if (gtk_text_iter_backward_char (&tmp1))
		{
			if (gtk_text_iter_get_char (&tmp1) == '}')
			c1++;
			else
			if (gtk_text_iter_get_char (&tmp1) == '{')
			{
				c1--;
				if (c1 == -1)
				if (gtk_text_iter_forward_char (&tmp1))
				{
					start=tmp1;
				}
			}
		}
	}

	controle=0;
	gint c2 = 0;
	GtkTextIter tmp2;
	gint row;

	GtkTextMark *m2 = gtk_text_buffer_get_insert (GTK_TEXT_BUFFER(cur_text_doc->text_buffer)); 
	gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &tmp2, m2);

	while (c2 != -1)
	{
		controle++;
		if(controle==1800){return;}

		if (gtk_text_iter_forward_char (&tmp2))
		{
			if (gtk_text_iter_get_char (&tmp2) == '{')
			c2++;
			else
			if (gtk_text_iter_get_char (&tmp2) == '}')
			{
				c2--;
				if (c2 == -1)
				{
					row = gtk_text_iter_get_line(&tmp2);
					gtk_text_buffer_get_iter_at_line (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &end, row);
				}
			}
		}
	}


	gchar *tampon=NULL;
	gint line=gtk_text_iter_get_line(&start);
	gint line2=line-1;

	if(tab_fold[row]==1)
	{
	clear_code_folding ();
	tab_fold[row]=2;
	return;
	}

	tab_fold[row]=1;

	gtk_text_buffer_get_iter_at_line (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &itstart, line2);
	tampon=g_strdup_printf ("%d", line) ;

	gtk_text_buffer_create_tag (	GTK_TEXT_BUFFER(cur_text_doc->text_buffer), tampon, "invisible", TRUE, NULL);
	gtk_text_buffer_apply_tag_by_name (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), tampon, &start, &end);

}

//*********************** CODE FOLDING REMOVE
void clear_code_folding ()
{
	GtkTextIter itstart, itend;
	gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &itstart, 0);
	gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &itend, gtk_text_buffer_get_char_count (GTK_TEXT_BUFFER(cur_text_doc->text_buffer)));
	gtk_text_buffer_remove_all_tags (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &itstart, &itend);
	gtk_source_buffer_remove_source_marks((GtkSourceBuffer *)cur_text_doc->text_buffer, &itstart, &itend,NULL);
	apply_hl (cur_text_doc);
	code_bg_folding ();
}


//*********************** CODE BG FOLDING
void code_bg_folding ()
{
	if (! get_page_text()) return;
	GtkTextIter start;
	GtkTextIter end;
	GtkTextIter line_iter;
	int controle=0;
	gint row1=NULL,row2=NULL;


	GtkTextIter tmp1;
	gint c1 = 0;

	GtkTextMark *m1 = gtk_text_buffer_get_insert (GTK_TEXT_BUFFER(cur_text_doc->text_buffer)); 
	gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &tmp1, m1);

	while (c1 != -1)
	{
		controle++;
		if(controle==1800){return;}

		if (gtk_text_iter_backward_char (&tmp1))
		{
			if (gtk_text_iter_get_char (&tmp1) == '}')
			c1++;
			else
			if (gtk_text_iter_get_char (&tmp1) == '{')
			{
				c1--;
				if (c1 == -1)
				if (gtk_text_iter_forward_char (&tmp1))
				{
					row1 = gtk_text_iter_get_line(&tmp1);
					start=tmp1;
				}
			}
		}else{return;}
	}

	controle=0;
	gint c2 = 0;
	GtkTextIter tmp2;

	GtkTextMark *m2 = gtk_text_buffer_get_insert (GTK_TEXT_BUFFER(cur_text_doc->text_buffer)); 
	gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &tmp2, m2);

	while (c2 != -1)
	{
		controle++;
		if(controle==1800){return;}

		if (gtk_text_iter_forward_char (&tmp2))
		{
			if (gtk_text_iter_get_char (&tmp2) == '{')
			c2++;
			else
			if (gtk_text_iter_get_char (&tmp2) == '}')
			{
				c2--;
				if (c2 == -1)
				{
					row2 = gtk_text_iter_get_line(&tmp2);
					gtk_text_buffer_get_iter_at_line (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &end, row2);
				}
			}
		}else{return;}
	}

	if(row1==row2)
	{
		return;
	}

	GtkTextIter start_find, end_find;

	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &start_find);
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &end_find);

	gtk_source_buffer_remove_source_marks((GtkSourceBuffer *)cur_text_doc->text_buffer, &start_find, &end_find,NULL);

	gtk_text_buffer_remove_tag_by_name(GTK_TEXT_BUFFER(cur_text_doc->text_buffer), "folding", &start_find, &end_find);
	gtk_text_buffer_apply_tag_by_name(GTK_TEXT_BUFFER(cur_text_doc->text_buffer), "folding", &start, &end);

	row2--;
	gchar *tampon=NULL;

	GdkPixbuf *pixbuf_mark;
	pixbuf_mark = gdk_pixbuf_new_from_file ("/usr/local/share/griffon/images/griffon_list.png", NULL);

	GtkSourceMarkAttributes *attribu=gtk_source_mark_attributes_new();
	gtk_source_mark_attributes_set_pixbuf(attribu,pixbuf_mark );

	GdkPixbuf *pixbuf_mark2;
	pixbuf_mark2 = gdk_pixbuf_new_from_file ("/usr/local/share/griffon/images/griffon_down.png", NULL);

	GtkSourceMarkAttributes *attribu2=gtk_source_mark_attributes_new();
	gtk_source_mark_attributes_set_pixbuf(attribu2,pixbuf_mark2 );

	tampon=g_strdup_printf ("%d", row1) ;

	gtk_source_view_set_mark_attributes(GTK_SOURCE_VIEW(cur_text_doc->text_view),tampon,attribu2,1);
	gtk_text_buffer_get_iter_at_line (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &line_iter, row1);
	gtk_source_buffer_create_source_mark(GTK_SOURCE_BUFFER(cur_text_doc->text_buffer),tampon,tampon,&line_iter);

	while(row1!=row2)
	{
	row1++;
	tampon=g_strdup_printf ("%d", row1) ;

	gtk_source_view_set_mark_attributes(GTK_SOURCE_VIEW(cur_text_doc->text_view),tampon,attribu,1);
	gtk_text_buffer_get_iter_at_line (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &line_iter, row1);
	gtk_source_buffer_create_source_mark(GTK_SOURCE_BUFFER(cur_text_doc->text_buffer),tampon,tampon,&line_iter);
	}

	GdkPixbuf *pixbuf_mark3;
	pixbuf_mark3 = gdk_pixbuf_new_from_file ("/usr/local/share/griffon/images/griffon_up.png", NULL);

	GtkSourceMarkAttributes *attribu3=gtk_source_mark_attributes_new();
	gtk_source_mark_attributes_set_pixbuf(attribu3,pixbuf_mark3 );

	row2++;
	tampon=g_strdup_printf ("%d", row2) ;

	gtk_source_view_set_mark_attributes(GTK_SOURCE_VIEW(cur_text_doc->text_view),tampon,attribu3,1);
	gtk_text_buffer_get_iter_at_line (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &line_iter, row2);
	gtk_source_buffer_create_source_mark(GTK_SOURCE_BUFFER(cur_text_doc->text_buffer),tampon,tampon,&line_iter);

}

//*********************** CODE FOLDING ALL
void code_folding_all ()
{
	if (! get_page_text()) return;
	GtkTextIter start,itstart;
	GtkTextIter end,end_buf,start_buf;
	int controle=0;

	GtkTextIter tmp1;
	gint c1 = 0;

	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &end_buf);
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &start_buf);

	GtkTextMark *m1 = gtk_text_buffer_get_insert (GTK_TEXT_BUFFER(cur_text_doc->text_buffer)); 
	gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &tmp1, m1);

	while (c1 != -1)
	{
		controle++;
		if(controle==1800){return;}

		if (gtk_text_iter_backward_char (&tmp1))
		{
			if (gtk_text_iter_get_char (&tmp1) == '}')
			c1++;
			else
			if (gtk_text_iter_get_char (&tmp1) == '{')
			{
				c1--;
				if (c1 == -1)
				if (gtk_text_iter_forward_char (&tmp1))
				{
					start=tmp1;
				}
			}
		}
	}

	controle=0;
	gint c2 = 0;
	GtkTextIter tmp2;
	gint row;

	GtkTextMark *m2 = gtk_text_buffer_get_insert (GTK_TEXT_BUFFER(cur_text_doc->text_buffer)); 
	gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &tmp2, m2);

	while (c2 != -1)
	{
		controle++;
		if(controle==1800){return;}

		if (gtk_text_iter_forward_char (&tmp2))
		{
			if (gtk_text_iter_get_char (&tmp2) == '{')
			c2++;
			else
			if (gtk_text_iter_get_char (&tmp2) == '}')
			{
				c2--;
				if (c2 == -1)
				{
					row = gtk_text_iter_get_line(&tmp2);
					gtk_text_buffer_get_iter_at_line (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &end, row);
				}
			}
		}
	}


	gchar *tampon=NULL;
	gint line=gtk_text_iter_get_line(&start);
	gint line2=line-1;

	if(tab_fold[row]==1)
	{
	clear_code_folding ();
	tab_fold[row]=2;
	return;
	}

	tab_fold[row]=1;

	gtk_text_buffer_get_iter_at_line (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &itstart, line2);
	tampon=g_strdup_printf ("%d", line) ;

	gtk_text_buffer_create_tag (	GTK_TEXT_BUFFER(cur_text_doc->text_buffer), tampon, "invisible", TRUE, NULL);
	gtk_text_buffer_apply_tag_by_name (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), tampon, &start_buf, &start);
	gtk_text_buffer_apply_tag_by_name (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), tampon, &end, &end_buf);
}

