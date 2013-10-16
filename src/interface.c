
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <glib.h>
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
#include <gtksourceview/gtksourcecompletion.h>
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

GtkSourceCompletionWords *word_provider;

//**************** STRUCTURE
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

int save_controle=0,nbr_term=0;

GtkTreeSelection *selection_myadmin ;
GtkWidget *toolbar;
GtkTreeSelection *selection_file; 
GtkListStore *store;
GtkWidget* view_list;
GtkListStore *store_book;
GtkWidget* view_list_book;
GtkWidget *view_a;
GtkWidget *button_web_image, *button_web_image2, *window_run ;
GtkWidget *view_myadmin, *view_sftp, *view_ftp;

GtkWidget *window_center, *icon_ok, *fam_hbox, *pWindow, *window_center_perl, *entry_man, *vbox_myadmin, *hpaned1, *vbox_sftp, *vbox_ftp, *vte, *splash_bar;
GList *combo1_items = NULL;
GtkWidget *combo1, *textview1, *hbox2, *scrolledwindow1, *label_note1,*label_note2, *label_note3, *label_note4, *scrolledwindow3, *textview2, *textview3, *vbox3, *scrolledwindow4, *scrolledwindow5, *textview4, *vbox4, *vbox5, *button_help1, *button_custom, *label_note5, *label_note6, *button_help2, *vbox6, *scrolledwindow7, *vbox_proj, *button_proj1,  *button_proj2, *button_proj3, *button_proj4, *button_proj5, *button_include1, *button_note1;
GtkWidget *filechooserwidget1, *label_tol, *icon_proj, *hbox_proj, *vbox2_proj,*scrolledWindow_sftp,*scrolledWindow_ftp,*combo2, *entry_custom, *entry_myadmin, *button_help6, *entry_help_custom, *hseparator_help;
GList *combo2_items = NULL;
GtkWidget *sView_scan;
GtkWidget *vte_add;

//********************************************* AUTOCOMPLEMENTATION 
struct _TestProviderClass
{
	GObjectClass parent_class;
};

static void test_provider_iface_init (GtkSourceCompletionProviderIface *iface);
GType test_provider_get_type (void);

G_DEFINE_TYPE_WITH_CODE (TestProvider,
			 test_provider,
			 G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (GTK_SOURCE_TYPE_COMPLETION_PROVIDER,
				 		test_provider_iface_init))


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

	gtk_source_completion_context_add_proposals (context,
						     completion_provider,
						     proposals,
						     TRUE);
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

	gtk_source_completion_words_register (word_provider,
	                                      gtk_text_view_get_buffer (GTK_TEXT_VIEW (source_view)));

	gtk_source_completion_add_provider (completion,
	                                    GTK_SOURCE_COMPLETION_PROVIDER (word_provider),
	                                    NULL);

	g_object_set (word_provider, "priority", 10, NULL);
}

//******************************** Ouverture de link pour navigateur web en mode link blank
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
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(web_win->scrolledWindow),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(web_win->scrolledWindow), GTK_WIDGET(web_win->webView_w));

			gtk_box_pack_start(GTK_BOX(web_win->vbox3), GTK_WIDGET(web_win->scrolledWindow), TRUE, TRUE, 1);

		gtk_widget_show_all(GTK_WIDGET(web_win->window_web)); 

  g_signal_connect ((gpointer) web_win->entry_myadmin, "activate",
                    G_CALLBACK (enter_myweb_win),
                    web_win->webView_w);


  	 g_signal_connect(web_win->webView_w, "load-finished",  
      G_CALLBACK(myadmin_get_url_win), web_win->entry_myadmin);

  	 g_signal_connect(web_win->webView_w, "new-window-policy-decision-requested",  
      G_CALLBACK(myadmin_new_window), web_win->webView_w);

  	 g_signal_connect(web_win->webView_w, "create-web-view",  
    G_CALLBACK(web_new_w_click_go), web_win->webView_w);

	webkit_web_view_load_uri(web_win->webView_w, uri);

return TRUE;
}

//******************************** AFFICHAGE DUN NAVIGATEUR WEB APRES CLIQUE DROIT
gboolean web_new_w_click(WebKitWebView *web_view,gpointer user_data)
{
	if(web_view==NULL){return FALSE;}
	gtk_widget_show_all (GTK_WIDGET(user_data));
	return TRUE;
}

//******************************** PREPARATION DUN NAVIGATEUR WEB APRES CLIQUE DROIT
WebKitWebView * web_new_w_click_go(WebKitWebView  *web_view, WebKitWebFrame *frame, gpointer user_data)
{

	if(web_view==NULL){return FALSE;}
	if(frame==NULL){return FALSE;}
	if(user_data==NULL){return FALSE;}

	win_web *web_win = (win_web *) g_malloc (sizeof (win_web));

  web_win->window_web = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for(GTK_WINDOW(web_win->window_web),GTK_WINDOW(tea_main_window));
	gtk_window_resize (GTK_WINDOW (web_win->window_web), 900, 500);
	gtk_window_maximize (GTK_WINDOW(web_win->window_web));

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
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(web_win->scrolledWindow),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(web_win->scrolledWindow), GTK_WIDGET(web_win->webView_w));

			gtk_box_pack_start(GTK_BOX(web_win->vbox3), GTK_WIDGET(web_win->scrolledWindow), TRUE, TRUE, 1);

  g_signal_connect ((gpointer) web_win->entry_myadmin, "activate",
                    G_CALLBACK (enter_myweb_win),
                    web_win->webView_w);

  	 g_signal_connect(web_win->webView_w, "load-finished",  
      G_CALLBACK(myadmin_get_url_win), web_win->entry_myadmin);

  	 g_signal_connect(web_win->webView_w, "new-window-policy-decision-requested",  
      G_CALLBACK(myadmin_new_window), web_win->webView_w);

  	 g_signal_connect(web_win->webView_w, "create-web-view",  
    G_CALLBACK(web_new_w_click_go), web_win->webView_w);

			g_signal_connect(web_win->webView_w, "download-requested", G_CALLBACK(download_requested_cb), NULL);

  	 g_signal_connect(web_win->webView_w, "web-view-ready",  
    G_CALLBACK(web_new_w_click), web_win->window_web);

return web_win->webView_w;

}

//******************************* creation de la toolbar
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
  gtk_tool_item_set_tooltip_text(tool_doc,(_("Génération d'un template de documentation du code en HTML")));
  gtk_tool_button_set_label(GTK_TOOL_BUTTON(tool_doc),"Doc Gen");

tool_sep=gtk_separator_tool_item_new();
gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_sep, -1);
gtk_widget_show(GTK_WIDGET(tool_sep));

		gtk_toolbar_set_style (GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS); 
		gtk_style_context_add_class (gtk_widget_get_style_context (toolbar),
                                                "primary-toolbar");
  gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar),GTK_ICON_SIZE_SMALL_TOOLBAR);

 if (confile.do_show_main_toolbar)
     gtk_widget_show (GTK_WIDGET(toolbar));
  else
      gtk_widget_hide (GTK_WIDGET(toolbar));
   
  return toolbar;
}

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

//******************************* fenetre principale
GtkWidget* create_tea_main_window (void)
{
  confile_reload();

	//*************** INIT des notifications
	notify_init ("libnotify");

  FILE *fich;
  char carac;
	char mot[100];
	mot[0]='\0';

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

  accel_group = gtk_accel_group_new ();

  tea_main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_maximize (GTK_WINDOW(tea_main_window));
  gtk_widget_set_name (tea_main_window, "tea_main_window");
  gtk_window_set_title (GTK_WINDOW (tea_main_window), _("Griffon 1.6.4"));

  gtk_window_set_icon_from_file (GTK_WINDOW(tea_main_window),"/usr/local/share/griffon/pixmaps/griffon_button.png",NULL);

  g_signal_connect (tea_main_window, "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  g_signal_connect ((gpointer) tea_main_window, "destroy_event",
                    G_CALLBACK (gtk_false),
                    NULL);

  g_signal_connect ((gpointer) tea_main_window, "delete_event",
                    G_CALLBACK (on_tea_window_delete_event),
                    tea_main_window);

  mainvbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_name (mainvbox, "mainvbox");
  gtk_widget_show (GTK_WIDGET(mainvbox));
  gtk_container_add (GTK_CONTAINER (tea_main_window), GTK_WIDGET(mainvbox));

  menubar1 = gtk_menu_bar_new ();

  gtk_widget_set_name (menubar1, "menubar1");
  gtk_widget_show (GTK_WIDGET(menubar1));
  gtk_box_pack_start (GTK_BOX (mainvbox), menubar1, FALSE, FALSE, 0);


  mni_temp = new_menu_item (_("File"), menubar1, NULL);
  mni_file_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
  mni_temp = new_menu_tof (mni_file_menu);

  mni_temp = new_menu_item (_("New"), mni_file_menu, on_mni_new_file_activate);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_n, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);
      
  mni_temp = new_menu_item (_("Crapbook"), mni_file_menu, on_mni_file_crapbook);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_M, GDK_MOD1_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Add a bookmark"), mni_file_menu, on_mni_file_add_bookmark);

  mni_temp = new_menu_item (_("Doc Gen"), mni_file_menu, gen_doc_html);

  mni_temp = new_menu_sep (mni_file_menu);

  mni_temp = new_menu_item (_("Open"), mni_file_menu, on_mni_file_open_activate);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_o, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Last open files"), mni_file_menu, on_mni_load_last_file);
  
  mni_temp = new_menu_item (_("Open different files"), mni_file_menu, NULL);
  mni_opendifferent_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
  mni_temp = new_menu_item (_("Open a snippet"), mni_opendifferent_menu, on_mni_snippet_file_open_activate);
  mni_temp = new_menu_item (_("Open a template"), mni_opendifferent_menu, on_mni_templ_file_open_activate);
  mni_temp = new_menu_item (_("Open a session file"), mni_opendifferent_menu, on_mni_session_file_open_activate);
  
  mni_temp = new_menu_sep (mni_file_menu);

  mni_recent_list = new_menu_item (_("Recent file"), mni_file_menu, NULL);
  mni_recent_menu = new_menu_submenu (GTK_WIDGET(mni_recent_list));

  mni_bookmarks_root = new_menu_item (_("BookMark"), mni_file_menu, NULL);
  mni_bookmarks_menu = new_menu_submenu (GTK_WIDGET(mni_bookmarks_root));
 
  mni_sessions = new_menu_item (_("Sessions"), mni_file_menu, NULL); 
  mni_sessions_menu = new_menu_submenu (GTK_WIDGET(mni_sessions));

  mni_templates = new_menu_item (_("Templates"), mni_file_menu, NULL); 
  mni_templates_menu = new_menu_submenu (GTK_WIDGET(mni_templates));
    
  mni_temp = new_menu_sep (mni_file_menu);

  mni_temp = new_menu_item (_("Save"), mni_file_menu, on_mni_file_save_activate);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_s, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Save as"), mni_file_menu, on_mni_file_save_as_activate);

  mni_temp = new_menu_item (_("Save different files"), mni_file_menu, NULL);
  mni_savedifferent_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
 
  mni_temp = new_menu_item (_("Save session"), mni_savedifferent_menu, on_mni_file_save_session);
  
  mni_temp = new_menu_item (_("Save as a template"), mni_savedifferent_menu, on_mni_file_save_as_template_activate);
  mni_temp = new_menu_item (_("Save as a snippet"), mni_savedifferent_menu, on_mni_file_save_as_snippet_activate);

  mni_temp = new_menu_item (_("Save Version"), mni_savedifferent_menu, on_mni_file_save_version);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_s, GDK_MOD1_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Backup"), mni_savedifferent_menu, on_mni_file_backup);

  mni_temp = new_menu_item (_("Back to save"), mni_file_menu, on_mni_file_revert_to_saved);

  mni_temp = new_menu_sep (mni_file_menu);

  mni_temp = new_menu_item (_("Manager configuration file"), mni_file_menu, NULL);
  mni_file_configs_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
  mni_temp = new_menu_item (_("File auto replacement"), mni_file_configs_menu, on_mni_autorep_file_open);
  mni_temp = new_menu_item (_("File autocomp"), mni_file_configs_menu, on_mni_autocomp_file_open);
  mni_temp = new_menu_item (_("main configuration"), mni_file_configs_menu , on_mni_config_open);
  mni_temp = new_menu_item (_("Charsets configuration"), mni_file_configs_menu, on_mni_utils_add_encodings_activate);
  mni_temp = new_menu_item (_("Configuring the User Menu"), mni_file_configs_menu, on_mni_user_menu_open);
  mni_temp = new_menu_item (_("Bookmark file"), mni_file_configs_menu, on_mni_file_open_bookmark);
	mni_temp = new_menu_item (_("Config file for todo list"), mni_file_configs_menu, on_mni_file_todo);
	mni_temp = new_menu_item (_("Configuration file MyAdmin"), mni_file_configs_menu, on_mni_file_myadmin);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_n, GDK_MOD1_MASK,
                              GTK_ACCEL_VISIBLE);

	mni_temp = new_menu_item (_("Reload MyAdmin"), mni_file_configs_menu, myadmin);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_h, GDK_MOD1_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Preferences"), mni_file_menu, on_mni_show_options);
 
  mni_temp = new_menu_sep (mni_file_menu);

  mni_temp = new_menu_item (_("Close the file assets"), mni_file_menu, on_mni_file_close_current);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_Q, GDK_MOD1_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Exit"), mni_file_menu, on_mni_out_activate); 

  mni_temp = new_menu_item (_("Edit"), menubar1, NULL);
  mni_edit_menu =  new_menu_submenu (GTK_WIDGET(mni_temp));
  mni_temp = new_menu_tof (mni_edit_menu);

  mni_temp = new_menu_item (_("Indent"), mni_edit_menu, on_mni_indent);
  mni_temp = new_menu_item (_("Undindent"), mni_edit_menu, on_mni_unindent);

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
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_E, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);
  mni_temp = new_menu_sep (mni_edit_menu);

  mni_temp = new_menu_item (_("Select all"), mni_edit_menu, on_mni_edit_select_all);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_A, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);
  
  mni_temp = new_menu_item (_("Delete Row"), mni_edit_menu, on_mni_edit_delete_current_line);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_y, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_sep (mni_edit_menu);

  mni_edit_undo = new_menu_item (_("Undo"), mni_edit_menu, on_mni_undo);
  gtk_widget_add_accelerator (mni_edit_undo, "activate", accel_group,
                              GDK_KEY_Z, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_edit_redo = new_menu_item (_("Redo"), mni_edit_menu, on_mni_redo);
  gtk_widget_add_accelerator (mni_edit_redo, "activate", accel_group,
                              GDK_KEY_Z, GDK_CONTROL_MASK | GDK_SHIFT_MASK,
                              GTK_ACCEL_VISIBLE);


  mni_temp = new_menu_item (_("Find"), menubar1, NULL);

  mni_quest_menu =  new_menu_submenu (GTK_WIDGET(mni_temp));
  mni_temp = new_menu_tof (mni_quest_menu);

  mni_temp = new_menu_item (_("BookMark"), mni_quest_menu, on_format_button_clicked);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_b, GDK_MOD1_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Find and Replace window"), mni_quest_menu, on_mni_find_and_replace_wnd);
    
  mni_temp = new_menu_item (_("Search (in the text box control)"), mni_quest_menu, on_mni_quest_find);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_f, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);
 
  mni_temp = new_menu_item (_("Search case insensitive (the text box control)"), mni_quest_menu, on_mni_quest_find_ncase);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_f, GDK_CONTROL_MASK | GDK_SHIFT_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Search the following (text area control)"), mni_quest_menu, on_mni_quest_find_next);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_F3, 0,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Search according to case insensitive (the text box control)"), mni_quest_menu, on_mni_quest_find_next_ncase);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_F3, GDK_SHIFT_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Go to line"), mni_quest_menu, on_mni_goto_line);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_g, GDK_MOD1_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Replace All (Command Line FIND_TEXT~TEXTE_REMPLACEMENT)"), mni_quest_menu, on_mni_search_repall);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_F1, 0,
                              GTK_ACCEL_VISIBLE);
                              
  mni_temp = new_menu_item (_("Scan local HTML link (HREF)"), mni_quest_menu, on_mni_get_links );

  mni_temp = new_menu_item (_("Scan HTML image (SRC)"), mni_quest_menu, on_mni_get_src);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_F7, GDK_SHIFT_MASK,
                              GTK_ACCEL_VISIBLE); 

  mni_temp = new_menu_item (_("Function"), menubar1, NULL);
  mni_functions_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
  mni_temp = new_menu_tof (mni_functions_menu);

  mni_temp = new_menu_item (_("Insert a file path PATH"), mni_functions_menu, selection_path);

  mni_temp = new_menu_item (_("Analyze text UNIversal"), mni_functions_menu, NULL);
  mni_utils_unitaz_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
  mni_temp = new_menu_tof (mni_utils_unitaz_menu);
  mni_temp = new_menu_item (_("UNITAZ sorts with abc"), mni_utils_unitaz_menu, on_mni_tools_unitaz_sort);
  mni_temp = new_menu_item (_("UNITAZ with sorting by account"), mni_utils_unitaz_menu, on_mni_tools_unitaz_sort_by_count);
  mni_temp = new_menu_item (_("UNITAZ with case insensitive sorts with abc"), mni_utils_unitaz_menu, on_mni_tools_unitaz_sort_caseinsens);
  mni_temp = new_menu_item (_("UNITAZ with case insensitive sorts by counting"), mni_utils_unitaz_menu, on_mni_tools_unitaz_sort_by_count_caseinsens);
  mni_temp = new_menu_item (_("UNITAZ plaine"), mni_utils_unitaz_menu, on_mni_tools_unitaz_plain);
  mni_temp = new_menu_item (_("Extract words"), mni_utils_unitaz_menu, on_mni_extract_words);
  
  mni_temp = new_menu_sep (mni_functions_menu);

  mni_snippets = new_menu_item (_("Snippets"), mni_functions_menu, NULL);
  mni_snippets_menu = new_menu_submenu (GTK_WIDGET(mni_snippets));

  mni_user_menu = new_menu_item (_("User Menu"), mni_functions_menu, NULL);
  mni_user_menu_menu = new_menu_submenu (GTK_WIDGET(mni_user_menu));

  mni_temp = new_menu_sep (mni_functions_menu);

  mni_temp = new_menu_item (_("Insert"), mni_functions_menu, NULL);
  mni_ins_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
  mni_temp = new_menu_tof (mni_ins_menu);

  mni_temp = new_menu_item (_("Date/Time"), mni_ins_menu, on_mni_funx_ins_time);
  mni_temp = new_menu_item (_("Dump menu"), mni_ins_menu, on_mni_dump_menu);

  mni_temp = new_menu_item (_("Char"), mni_functions_menu, NULL);
  mni_functions_case_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
  mni_temp = new_menu_tof (mni_functions_case_menu);
                        
  mni_temp = new_menu_item (_("Capital"), mni_functions_case_menu, on_mni_case_upcase_activate);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_Up, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);
                        
  mni_temp = new_menu_item (_("Tiny"), mni_functions_case_menu, on_mni_case_locase_activate);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_Down, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

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
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_D, GDK_MOD1_MASK,
                              GTK_ACCEL_VISIBLE);

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
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_E, GDK_MOD1_MASK,
                              GTK_ACCEL_VISIBLE);

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
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_F8, 0,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Google translation with text selection tab MyAdmin : FR to EN"), mni_functions_menu, google_traduction_fr_en);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_F6, 0,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Google translation with text selection tab MyAdmin : EN to FR"), mni_functions_menu, google_traduction_en_fr);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_F7, 0,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Html"), menubar1, NULL);
  mni_markup_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
  mni_temp = new_menu_tof (mni_markup_menu);
  
  mni_temp = new_menu_item (_("HTML tools"), mni_markup_menu, NULL);
  mni_html_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
  mni_temp = new_menu_tof (mni_html_menu);

  mni_temp = new_menu_item (_("Basic template"), mni_html_menu, on_mni_html_default_template);

  mni_temp = new_menu_item (_("Basic Table"), mni_html_menu, on_mni_html_make_table);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_T, GDK_CONTROL_MASK | GDK_SHIFT_MASK,
                              GTK_ACCEL_VISIBLE);

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

  mni_temp = new_menu_item (_("Bold"), mni_markup_menu, on_mni_Markup_bold_activate);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_B, GDK_CONTROL_MASK | GDK_MOD1_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Italics"), mni_markup_menu, on_mni_Markup_italic_activate);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_I, GDK_MOD1_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Underline"), mni_markup_menu, on_mni_Markup_underline_activate);
  
  mni_temp = new_menu_sep (mni_markup_menu);

  mni_temp = new_menu_item (_("Comment"), mni_markup_menu, on_mni_Markup_comment);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_slash, GDK_MOD1_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Paragraph"), mni_markup_menu, on_mni_Markup_para_activate);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_P, GDK_MOD1_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Link"), mni_markup_menu, on_mni_Markup_link_activate);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_L, GDK_MOD1_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Color"), mni_markup_menu, on_mni_markup_select_color_activate);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_c, GDK_MOD1_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Image"), mni_markup_menu, on_mni_image_activate);
 
  mni_temp = new_menu_item (_("Carriage return"), mni_markup_menu, on_mni_Markup_br);

  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_Return, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item ("Space", mni_markup_menu,on_mni_Markup_nbsp);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_space, GDK_CONTROL_MASK | GDK_SHIFT_MASK,
                              GTK_ACCEL_VISIBLE);

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

  mni_ide = new_menu_item (_("Projects"), menubar1, NULL);
  mni_ide_menu = new_menu_submenu (GTK_WIDGET(mni_ide));
  mni_temp = new_menu_tof (mni_ide_menu);

  mni_temp = new_menu_item (_("New Project"), mni_ide_menu, on_mni_project_new);
  mni_temp = new_menu_item (_("Open a project"), mni_ide_menu, on_mni_project_open);
  mni_temp = new_menu_item (_("Save the project"), mni_ide_menu, on_mni_show_project_save);
  mni_temp = new_menu_item (_("Save the project under"), mni_ide_menu, on_mni_project_save_as);
  mni_temp = new_menu_item (_("Ownership of the project"), mni_ide_menu, on_mni_show_project_props);
  mni_temp = new_menu_item (_("Make"), mni_ide_menu, on_mni_project_make);
    gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_F11, GDK_SHIFT_MASK,
                              GTK_ACCEL_VISIBLE);
  
	//**** OLD RUN on_mni_show_project_run
  mni_temp = new_menu_item (_("Run"), mni_ide_menu, window_debug);  
    gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_F12, GDK_SHIFT_MASK,
                              GTK_ACCEL_VISIBLE);
  mni_temp = new_menu_item (_("Mount a server SFTP (SSH)"), mni_ide_menu, w_sftp_mount);
     gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_F9, GDK_SHIFT_MASK,
                              GTK_ACCEL_VISIBLE);
                              
  mni_temp = new_menu_item (_("Mount a server FTP (FTPFS)"), mni_ide_menu, w_ftp_mount);
  
  mni_temp = new_menu_item (_("Umount the last mounted server SFTP / FTP"), mni_ide_menu, umount_sftp);
     gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_F10, GDK_SHIFT_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_tabs = new_menu_item (_("Tabs"), menubar1, NULL);
  mni_tabs_menu = new_menu_submenu (GTK_WIDGET(mni_tabs));

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
  
  mni_temp = new_menu_item (_("Navigation"), menubar1, NULL);
  mni_nav_menu = new_menu_submenu (GTK_WIDGET(mni_temp));
  mni_temp = new_menu_tof (mni_nav_menu);

  mni_temp = new_menu_item (_("Scan links in the page"), mni_nav_menu,scan_links);

  mni_links = new_menu_item (_("Links"), mni_nav_menu, NULL);
  mni_links_menu = new_menu_submenu (GTK_WIDGET(mni_links));

  mni_temp = new_menu_item (_("Go to beginning of block {"), mni_nav_menu, on_mni_nav_block_start);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_Left, GDK_MOD1_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Go to end of block}"), mni_nav_menu, on_mni_nav_block_end);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_Right, GDK_MOD1_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Jump to recent tab"), mni_nav_menu, on_mni_nav_goto_recent_tab);

  mni_temp = new_menu_item (_("Go to the next tab"), mni_nav_menu, on_mni_nav_goto_next_tab);
    gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_Page_Up, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);
  mni_temp = new_menu_item (_("Go to the previous tab"), mni_nav_menu, on_mni_nav_goto_prev_tab);
    gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_Page_Down, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Go to first tab"), mni_nav_menu, on_mni_nav_goto_first_tab);
  mni_temp = new_menu_item (_("Go to the last tab"), mni_nav_menu, on_mni_nav_goto_last_tab);
  mni_temp = new_menu_item (_("Go to the selection"), mni_nav_menu, on_mni_nav_goto_selection);

  mni_temp = new_menu_item (_("Focus on the command line"), mni_nav_menu, on_mni_nav_focus_to_famous);
    gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_W, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Reload Mini Web"), mni_nav_menu, focus_web);
    gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_R, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Focus and loading of the current page in the Web tab Mini"), mni_nav_menu, web_current_file);
    gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_D, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Focus on the text"), mni_nav_menu, on_mni_nav_focus_to_text);
    gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_B, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Open the cursor"), mni_nav_menu, on_mni_file_open_at_cursor);

  mni_temp = new_menu_item (_("Edit the cursor"), mni_nav_menu, on_mni_file_edit_at_cursor );
  
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
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_F5, 0,
                              GTK_ACCEL_VISIBLE);

  mni_temp = new_menu_item (_("Wrap"), mni_view_menu, on_mni_view_wrap);
  gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_W, GDK_MOD1_MASK,
                              GTK_ACCEL_VISIBLE);
 
  mni_temp = new_menu_item (_("Display images in the text"), mni_view_menu, on_mni_show_images_in_text);
  mni_temp = new_menu_item (_("Switch between header / source"), mni_view_menu, on_source_header_switch);
    gtk_widget_add_accelerator (mni_temp, "activate", accel_group,
                              GDK_KEY_H, GDK_CONTROL_MASK,
                              GTK_ACCEL_VISIBLE);

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

  mni_temp = new_menu_item (_("Check your version of Griffon IDE"), mni_what_menu, version_window);    
  mni_temp = new_menu_item (_("Read the manual / documentation Griffon IDE"), mni_what_menu, doc_window);
	mni_temp = new_menu_item (_("Send a bug report"), mni_what_menu, rapport_window);
  mni_temp = new_menu_item (_("About Griffon IDE"), mni_what_menu, create_about1);

  mni_temp = new_menu_sep (mni_what_menu);
  
  //***************************** FAMOUS CMB
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

	//***************************************************** NOTEBOOK MAIN
  notebook3 = gtk_notebook_new ();  
  gtk_widget_show (GTK_WIDGET(notebook3));  
	gtk_paned_pack1 (GTK_PANED (vpaned1), notebook3, TRUE, TRUE);
  gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook3), GTK_POS_TOP);
 
	gtk_notebook_set_group_name (GTK_NOTEBOOK (notebook3), "wnote");	

  //********************** FILE CHOSE 
  vbox4 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add (GTK_CONTAINER (notebook3), GTK_WIDGET(vbox4));
  gtk_widget_show (GTK_WIDGET(vbox4));  

	//***************** ONGLET FILE  
  notebook2 = gtk_notebook_new ();  
  gtk_widget_show (GTK_WIDGET(notebook2));  
  gtk_box_pack_start (GTK_BOX (vbox4), notebook2, TRUE, TRUE, 0);  
  gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook2), GTK_POS_LEFT);
 
	gtk_notebook_set_group_name (GTK_NOTEBOOK (notebook2), "wnote");	

  //********************** FILE CHOSE 
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

  scrolledwindow5 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (GTK_WIDGET(scrolledwindow5));
		 gtk_box_pack_start(GTK_BOX(vbox4), GTK_WIDGET(scrolledwindow5), TRUE, TRUE, 1);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow5), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_placement (GTK_SCROLLED_WINDOW (scrolledwindow5), GTK_CORNER_TOP_LEFT);

	//**************************** Include FC Variables LIST
    GtkTreeModel      *model;
    GtkTreeViewColumn *col, *col2;
    GtkCellRenderer   *renderer;
	 GtkTreeSelection *selection_scan;

    model = GTK_TREE_MODEL(create_liststore());
 
    view_list = gtk_tree_view_new_with_model(model);

    col = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(col, (_("List of include / functions     ")));

    col2 = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(col2, "Line");
 
    renderer = gtk_cell_renderer_pixbuf_new();
    gtk_tree_view_column_pack_start(col, renderer, FALSE);
    gtk_tree_view_column_set_attributes(col, renderer,
                                        "pixbuf", COL_ICON,
                                        NULL);
 
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_set_attributes(col, renderer,
                                        "text", COL_TEXT,
                                        NULL);
 
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col2, renderer, TRUE);
    gtk_tree_view_column_set_attributes(col2, renderer,
                                        "text", COL_TEXT2,
                                        NULL);

    gtk_tree_view_append_column(GTK_TREE_VIEW(view_list), col);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view_list), col2);
    gtk_widget_show(GTK_WIDGET(view_list));
	gtk_container_add (GTK_CONTAINER (scrolledwindow5), GTK_WIDGET(view_list));

	gtk_tree_view_set_grid_lines (GTK_TREE_VIEW(view_list),GTK_TREE_VIEW_GRID_LINES_BOTH);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(view_list),TRUE);

	selection_scan = gtk_tree_view_get_selection(GTK_TREE_VIEW(view_list));

	 g_signal_connect(selection_scan, "changed",  
      G_CALLBACK(on_changed_scan), NULL);

	//*******************************
    button_include1 = gtk_button_new_with_label (_("Start the scan"));
    gtk_widget_show(GTK_WIDGET(button_include1));
    gtk_box_pack_start(GTK_BOX(vbox4), button_include1, FALSE, FALSE, 0);

	 GtkWidget *button_include2;
    button_include2 = gtk_button_new_with_label (_("Open files include"));
    gtk_widget_show(GTK_WIDGET(button_include2));
    gtk_box_pack_start(GTK_BOX(vbox4), button_include2, FALSE, FALSE, 0);

    g_signal_connect ((gpointer) button_include2, "clicked",
                    G_CALLBACK (open_include),
                    NULL);  

   label_note4 = gtk_label_new (_("In"));
   gtk_widget_show (GTK_WIDGET(label_note4));

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

  GtkWidget *view;
  GtkTreeSelection *selection; 
  GtkWidget *vbox;
  GtkWidget *statusbar;

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(vbox5), GTK_WIDGET(vbox));
  gtk_widget_show (GTK_WIDGET(vbox));

  view = create_view_and_model_help();
  gtk_widget_show (GTK_WIDGET(view));
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
  gtk_box_pack_start(GTK_BOX(vbox), view, TRUE, TRUE, 1);

  statusbar = gtk_statusbar_new();
  gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, TRUE, 1);
  gtk_widget_show (GTK_WIDGET(statusbar));
	
	 g_signal_connect(selection, "changed",  
    G_CALLBACK(on_changed), statusbar);

  label_note5 = gtk_label_new (_("Help"));
  gtk_widget_show (GTK_WIDGET(label_note5));

	gtk_widget_set_size_request (label_note5, 50, 20);

  hbox_note = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox_note));
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 2), hbox_note);

  image2 = gtk_image_new_from_stock ("gtk-help", GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_widget_show (GTK_WIDGET(image2));
  gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox_note), label_note5, TRUE, TRUE, 0);	

  gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 2), TRUE);
        
	//*********MONTAGED HISTORIQUE SFTP                                    
     vbox_sftp = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	  gtk_container_add (GTK_CONTAINER (notebook2), GTK_WIDGET(vbox_sftp));
	  gtk_widget_show (GTK_WIDGET(vbox_sftp)); 
      
    scrolledWindow_sftp = gtk_scrolled_window_new(NULL, NULL);
	 gtk_widget_show (GTK_WIDGET(scrolledWindow_sftp));
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow_sftp),
    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	 gtk_box_pack_start(GTK_BOX(vbox_sftp), scrolledWindow_sftp, TRUE, TRUE, 1);

  view_sftp = create_view_and_model_sftp();
  gtk_widget_show (GTK_WIDGET(view_sftp));
  selection_sftp = gtk_tree_view_get_selection(GTK_TREE_VIEW(view_sftp));       
  gtk_container_add(GTK_CONTAINER(scrolledWindow_sftp), GTK_WIDGET(view_sftp));

  g_signal_connect(selection_sftp, "changed",  
  G_CALLBACK(on_changed_sftp), statusbar);             

  GtkWidget *button_vide_sftp = gtk_button_new_with_label (_("Clear history"));
  gtk_widget_show(GTK_WIDGET(button_vide_sftp));
  gtk_box_pack_start(GTK_BOX(vbox_sftp), button_vide_sftp, FALSE, FALSE, 0);            

  g_signal_connect ((gpointer) button_vide_sftp, "clicked",
                    G_CALLBACK (vide_configuration_sftp),
                    NULL); 

    GtkWidget *button_m_sftp = gtk_button_new_with_label (_("Mount a server"));
    gtk_widget_show(GTK_WIDGET(button_m_sftp));
    gtk_box_pack_start(GTK_BOX(vbox_sftp), button_m_sftp, FALSE, FALSE, 0);            

  g_signal_connect ((gpointer) button_m_sftp, "clicked",
                    G_CALLBACK (w_sftp_mount),
                    NULL); 

    GtkWidget *button_umount = gtk_button_new_with_label (_("Umount a server"));
    gtk_widget_show(GTK_WIDGET(button_umount));
    gtk_box_pack_start(GTK_BOX(vbox_sftp), button_umount, FALSE, FALSE, 0);            

  g_signal_connect ((gpointer) button_umount, "clicked",
                    G_CALLBACK (umount_sftp),
                    NULL);                                                                        
            
  label_note6 = gtk_label_new (_("SFTP"));
  gtk_widget_show (GTK_WIDGET(label_note6));
	
	gtk_widget_set_size_request (label_note6, 50, 20);

  hbox_note = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox_note));
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 3), hbox_note);

  image2 = gtk_image_new_from_stock ("gtk-network", GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_widget_show (GTK_WIDGET(image2));
  gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox_note), label_note6, TRUE, TRUE, 0);	

  gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 3), TRUE);

	//*********MONTAGED HISTORIQUE FTP                                    
     vbox_ftp = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	  gtk_container_add (GTK_CONTAINER (notebook2), GTK_WIDGET(vbox_ftp));
	  gtk_widget_show (GTK_WIDGET(vbox_ftp)); 
      
    scrolledWindow_ftp = gtk_scrolled_window_new(NULL, NULL);
	 gtk_widget_show (GTK_WIDGET(scrolledWindow_ftp));
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow_ftp),
    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	 gtk_box_pack_start(GTK_BOX(vbox_ftp), scrolledWindow_ftp, TRUE, TRUE, 1);
            
  view_ftp = create_view_and_model_ftp();
  gtk_widget_show (GTK_WIDGET(view_ftp));
  selection_ftp = gtk_tree_view_get_selection(GTK_TREE_VIEW(view_ftp));       
  gtk_container_add(GTK_CONTAINER(scrolledWindow_ftp), GTK_WIDGET(view_ftp));

  	 g_signal_connect(selection_ftp, "changed",  
    G_CALLBACK(on_changed_ftp), statusbar);             

    GtkWidget *button_vide_ftp = gtk_button_new_with_label (_("Clear history"));
    gtk_widget_show(GTK_WIDGET(button_vide_ftp));
    gtk_box_pack_start(GTK_BOX(vbox_ftp), button_vide_ftp, FALSE, FALSE, 0);            

  g_signal_connect ((gpointer) button_vide_ftp, "clicked",
                    G_CALLBACK (vide_configuration_ftp),
                    NULL);

    GtkWidget *button_m_ftp = gtk_button_new_with_label (_("Mount a server"));
    gtk_widget_show(GTK_WIDGET(button_m_ftp));
    gtk_box_pack_start(GTK_BOX(vbox_ftp), button_m_ftp, FALSE, FALSE, 0);            

  g_signal_connect ((gpointer) button_m_ftp, "clicked",
                    G_CALLBACK (w_ftp_mount),
                    NULL);

    GtkWidget *button_umount2 = gtk_button_new_with_label (_("Umount a server"));
    gtk_widget_show(GTK_WIDGET(button_umount2));
    gtk_box_pack_start(GTK_BOX(vbox_ftp), button_umount2, FALSE, FALSE, 0);            

  g_signal_connect ((gpointer) button_umount2, "clicked",
                    G_CALLBACK (umount_sftp),
                    NULL);                                                                        
            
                          label_note6 = gtk_label_new (_("FTP"));
  gtk_widget_show (GTK_WIDGET(label_note6));
  gtk_widget_set_size_request (label_note6, 50, 20);

  hbox_note = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox_note));
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 4), hbox_note);

  image2 = gtk_image_new_from_stock ("gtk-network", GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_widget_show (GTK_WIDGET(image2));
  gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox_note), label_note6, TRUE, TRUE, 0);	

  gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 4), TRUE);

	//**************************** Include FC Variables LIST 
  GtkWidget* vbox_book = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add (GTK_CONTAINER (notebook2), GTK_WIDGET(vbox_book));
  gtk_widget_show (GTK_WIDGET(vbox_book));  

  GtkWidget* scrolledwindow_book = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (GTK_WIDGET(scrolledwindow_book));
		 gtk_box_pack_start(GTK_BOX(vbox_book), GTK_WIDGET(scrolledwindow_book), TRUE, TRUE, 1);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_book), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_placement (GTK_SCROLLED_WINDOW (scrolledwindow_book), GTK_CORNER_TOP_LEFT);


    GtkTreeModel      *model_book;
    GtkTreeViewColumn *col_book, *col_book2;
    GtkCellRenderer   *renderer_book;
 
    model_book = GTK_TREE_MODEL(create_liststore_book());
 
    view_list_book = gtk_tree_view_new_with_model(model_book);
 
    col_book = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(col_book, "   ");
 
    col_book2 = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(col_book2, "Mark");

    renderer = gtk_cell_renderer_pixbuf_new();
    gtk_tree_view_column_pack_start(col_book, renderer, FALSE);
    gtk_tree_view_column_set_attributes(col_book, renderer,
                                        "pixbuf", COL_ICON,
                                        NULL);
 
    renderer_book = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col_book, renderer_book, TRUE);
    gtk_tree_view_column_set_attributes(col_book, renderer_book,
                                        "text", COL_TEXT,
                                        NULL);
 
    renderer_book = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col_book2, renderer_book, TRUE);
    gtk_tree_view_column_set_attributes(col_book2, renderer_book,
                                        "text", COL_TEXT2,
                                        NULL);

    gtk_tree_view_append_column(GTK_TREE_VIEW(view_list_book), col_book);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view_list_book), col_book2);
    gtk_widget_show(GTK_WIDGET(view_list_book));
	 gtk_container_add (GTK_CONTAINER (scrolledwindow_book), GTK_WIDGET(view_list_book));

	GtkTreeSelection *selection_book;

	gtk_tree_view_set_grid_lines (GTK_TREE_VIEW(view_list_book),GTK_TREE_VIEW_GRID_LINES_BOTH);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(view_list_book),TRUE);


	selection_book = gtk_tree_view_get_selection(GTK_TREE_VIEW(view_list_book));

	 g_signal_connect(selection_book, "changed",  
      G_CALLBACK(on_changed_book), NULL);

    GtkWidget *button_del_book = gtk_button_new_with_label (_("Remove BookMark"));
    gtk_widget_show(GTK_WIDGET(button_del_book));
    gtk_box_pack_start(GTK_BOX(vbox_book), button_del_book, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) button_del_book, "clicked",
                    G_CALLBACK (del_book),
                    NULL);

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

  hbox_note = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox_note));
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook3), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook3), 0), hbox_note);

  image2 = gtk_image_new_from_stock ("gtk-directory", GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_widget_show (GTK_WIDGET(image2));
  gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox_note), label_note4, TRUE, TRUE, 0);	

	gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook3), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook3), 0), TRUE);

//*************** ONGLET EDITOR
  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(notebook3), GTK_WIDGET(vbox));
  gtk_widget_show (GTK_WIDGET(vbox));

  hbox_bar = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start (GTK_BOX (vbox), hbox_bar, FALSE, TRUE, 0);
  gtk_widget_show (GTK_WIDGET(hbox_bar));

  tb_main_toolbar = create_hardcoded_toolbar ();
  gtk_box_pack_start (GTK_BOX (hbox_bar), tb_main_toolbar, FALSE, TRUE, 0);

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
	gtk_box_pack_start (GTK_BOX (hbox_bar), toolbar_manager, TRUE , TRUE, 0);
	gtk_widget_show(GTK_WIDGET(toolbar_manager));

GtkToolItem *tool_sep2;
tool_sep2=gtk_separator_tool_item_new();
gtk_toolbar_insert(GTK_TOOLBAR(toolbar_manager), tool_sep2, -1);
gtk_widget_show(GTK_WIDGET(tool_sep2));

	//*************************************
	GtkToolItem *item_entry  = gtk_tool_item_new();

  cmb_famous = gtk_entry_new ();     
  ent_search = cmb_famous;
  gtk_widget_show (GTK_WIDGET(cmb_famous));

	gtk_entry_set_width_chars (GTK_ENTRY(cmb_famous),50);

	//**************************** Autocomp
    GtkEntryCompletion *completion_entry;
			completion_entry = gtk_entry_completion_new();

    gtk_entry_completion_set_text_column(completion_entry, CONTACT_NAME);
    gtk_entry_set_completion(GTK_ENTRY(cmb_famous), completion_entry);

	    model_entry = gtk_list_store_new(1, G_TYPE_STRING);
 
    gtk_entry_completion_set_model(completion_entry, GTK_TREE_MODEL(model_entry));
	//****************************

	gtk_container_add( GTK_CONTAINER(item_entry), GTK_WIDGET(cmb_famous) );
	gtk_toolbar_insert( GTK_TOOLBAR(toolbar_manager), GTK_TOOL_ITEM(item_entry), -1 );
  gtk_widget_show (GTK_WIDGET(item_entry));

  g_signal_connect ((gpointer) ent_search, "key_press_event",
                    G_CALLBACK (on_ent_search_key_press_event),
                    NULL);

  g_signal_connect ((gpointer) ent_search, "key_release_event",
                    G_CALLBACK (keyrelase_search),
                    NULL);

tool_sep2=gtk_separator_tool_item_new();
gtk_toolbar_insert(GTK_TOOLBAR(toolbar_manager), tool_sep2, -1);
gtk_widget_show(GTK_WIDGET(tool_sep2));


  //*************Icon de status
  icon_ok = gtk_image_new_from_file("/usr/local/share/griffon/pixmaps/griffon_ok.png");
  gtk_widget_set_size_request (icon_ok, 35, 10);
  gtk_box_pack_start (GTK_BOX (hbox_bar), icon_ok, FALSE, FALSE, 0);
  gtk_widget_show (GTK_WIDGET(icon_ok));

   webView_editor = WEBKIT_WEB_VIEW(webkit_web_view_new());
	 //gtk_widget_show (webView_editor);

    scrolledWindow_editor = gtk_scrolled_window_new(NULL, NULL);
	  gtk_widget_show (GTK_WIDGET(scrolledWindow_editor));
     gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow_editor),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolledWindow_editor), GTK_WIDGET(webView_editor));
 
			gtk_box_pack_start (GTK_BOX (vbox), scrolledWindow_editor, TRUE, TRUE, 0);

	webkit_web_view_load_uri(webView_editor, "http://griffon.lasotel.fr/main.php?version=1.6.4");

  notebook1 = gtk_notebook_new ();
  gtk_widget_set_name (notebook1, "notebook1");
  gtk_widget_show (GTK_WIDGET(notebook1));
  gtk_notebook_set_scrollable (GTK_NOTEBOOK (notebook1), TRUE);
	gtk_box_pack_start (GTK_BOX (vbox), notebook1, TRUE, TRUE, 0);
	gtk_notebook_set_group_name (GTK_NOTEBOOK (notebook1), "wnote");

	//*****************************
  label_note4 = gtk_label_new (_("Editor"));
  gtk_widget_show (GTK_WIDGET(label_note4));

	gtk_widget_set_size_request (label_note4, 50, 20);

  hbox_note = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox_note));
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook3), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook3), 1), hbox_note);

  image2 = gtk_image_new_from_stock ("gtk-edit", GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_widget_show (GTK_WIDGET(image2));
  gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox_note), label_note4, TRUE, TRUE, 0);	

	//*********************** NOTEBOOK TOOLS
  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(notebook3), GTK_WIDGET(vbox));
  gtk_widget_show (GTK_WIDGET(vbox));


  notebook_down = gtk_notebook_new ();  
  gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook_down), GTK_POS_LEFT);
  gtk_widget_show (GTK_WIDGET(notebook_down));  
	gtk_box_pack_start (GTK_BOX (vbox), notebook_down, TRUE, TRUE, 0);

	gtk_notebook_set_group_name (GTK_NOTEBOOK (notebook_down), "wnote");	
	//*********************** ZONE INFORMATION
  vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (GTK_WIDGET(vbox2));
  gtk_container_add (GTK_CONTAINER (notebook_down), GTK_WIDGET(vbox2));

	//**************************** TOOLBAR INFO
	GtkWidget *toolbar_info;
	toolbar_info = gtk_toolbar_new ();

  GtkToolItem *tool_info_new = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE_AS  );
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar_info ), tool_info_new, -1);
  gtk_widget_show(GTK_WIDGET(tool_info_new));
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_info), GTK_TOOLBAR_ICONS);
  g_signal_connect ((gpointer) tool_info_new, "clicked",G_CALLBACK (new_file_log_edit),NULL);
  gtk_tool_item_set_tooltip_text(tool_info_new,_("Ouvrir les logs dans un fichier"));

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

  hbox_note =gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox_note));
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook_down), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_down), 0), hbox_note);
  gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook_down), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_down), 0), TRUE);

  image2 = gtk_image_new_from_stock ("gtk-info", GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_widget_show (GTK_WIDGET(image2));
  gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox_note), label_note5, TRUE, TRUE, 0);	

	//*********************** ZONE TERMINALE
  vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (GTK_WIDGET(vbox2));
  gtk_container_add (GTK_CONTAINER (notebook_down), GTK_WIDGET(vbox2));

	//**************************** TOOLBAR TERMINALE
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
	//****************************
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

	//********************************* PROJET TAB
  vbox_proj = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_container_add (GTK_CONTAINER (notebook_down), GTK_WIDGET(vbox_proj));
  gtk_widget_show (GTK_WIDGET(vbox_proj)); 

	//********* Button project
  hbox_proj = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start (GTK_BOX (vbox_proj), hbox_proj, FALSE, TRUE, 0);
  gtk_widget_show (GTK_WIDGET(hbox_proj));  

  vbox2_proj = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start (GTK_BOX (hbox_proj), vbox2_proj, TRUE, TRUE, 0);
  gtk_widget_show (GTK_WIDGET(vbox2_proj)); 

	//**************************** TOOLBAR MINIWEB
	GtkWidget *toolbar_projet;
	toolbar_projet = gtk_toolbar_new ();
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_projet), GTK_TOOLBAR_ICONS);
	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar_projet),GTK_ICON_SIZE_SMALL_TOOLBAR);

  GtkToolItem *tool_projet_new = gtk_tool_button_new_from_stock(GTK_STOCK_NEW  );
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar_projet ), tool_projet_new, -1);
  gtk_widget_show(GTK_WIDGET(tool_projet_new));
  g_signal_connect ((gpointer) tool_projet_new, "clicked",G_CALLBACK (on_mni_project_new),NULL);
  gtk_tool_item_set_tooltip_text(tool_projet_new,_("Nouveau projet"));

  GtkToolItem *tool_projet_open = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN  );
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar_projet ), tool_projet_open, -1);
  gtk_widget_show(GTK_WIDGET(tool_projet_open));
  g_signal_connect ((gpointer) tool_projet_open, "clicked",G_CALLBACK (on_mni_project_open),NULL);
  gtk_tool_item_set_tooltip_text(tool_projet_open,_("Ouvrir un projet"));

  GtkToolItem *tool_projet_save_as = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE_AS  );
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar_projet ), tool_projet_save_as, -1);
  gtk_widget_show(GTK_WIDGET(tool_projet_save_as));
  g_signal_connect ((gpointer) tool_projet_save_as, "clicked",G_CALLBACK (on_mni_project_save_as),NULL);
  gtk_tool_item_set_tooltip_text(tool_projet_save_as,_("Saving a project"));

	tool_sep=gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_projet), tool_sep, -1);
	gtk_widget_show(GTK_WIDGET(tool_sep));

  GtkToolItem *tool_projet_make = gtk_tool_button_new_from_stock(GTK_STOCK_CONVERT  );
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar_projet ), tool_projet_make, -1);
  gtk_widget_show(GTK_WIDGET(tool_projet_make));
  g_signal_connect ((gpointer) tool_projet_make, "clicked",G_CALLBACK (on_mni_project_make),NULL);
  gtk_tool_item_set_tooltip_text(tool_projet_make,_("Make the project"));

  GtkToolItem *tool_projet_run = gtk_tool_button_new_from_stock(GTK_STOCK_EXECUTE   );
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar_projet ), tool_projet_run, -1);
  gtk_widget_show(GTK_WIDGET(tool_projet_run));
  g_signal_connect ((gpointer) tool_projet_run, "clicked",G_CALLBACK (on_mni_show_project_run),NULL);
  gtk_tool_item_set_tooltip_text(tool_projet_run,_("Run the project"));

	gtk_box_pack_start (GTK_BOX (vbox2_proj), toolbar_projet, FALSE, FALSE, 0);
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_projet), GTK_TOOLBAR_ICONS);
	gtk_widget_show (GTK_WIDGET(toolbar_projet)); 
  
	//****************************
    button_proj1 = gtk_button_new_with_label (_("Ouvrir un projet"));
    gtk_widget_show(GTK_WIDGET(button_proj1));
    gtk_box_pack_start(GTK_BOX(vbox2_proj), button_proj1, FALSE, FALSE, 0);

    button_proj2 = gtk_button_new_with_label (_("Saving a project"));
    gtk_widget_show(GTK_WIDGET(button_proj2));
    gtk_box_pack_start(GTK_BOX(vbox2_proj), button_proj2, FALSE, FALSE, 0);

    button_proj3 = gtk_button_new_with_label (_("Make the project"));
    gtk_widget_show(GTK_WIDGET(button_proj3));
    gtk_box_pack_start(GTK_BOX(vbox2_proj), button_proj3, FALSE, FALSE, 0);

    button_proj4 = gtk_button_new_with_label (_("Executer le projet"));
    gtk_widget_show(GTK_WIDGET(button_proj4));
    gtk_box_pack_start(GTK_BOX(vbox2_proj), button_proj4, FALSE, FALSE, 0);

    button_proj5 = gtk_button_new_with_label (_("Nouveau projet"));
    gtk_widget_show(GTK_WIDGET(button_proj5));
    gtk_box_pack_start(GTK_BOX(vbox2_proj), button_proj5, FALSE, FALSE, 0);

  scrolledwindow3 = gtk_scrolled_window_new (NULL, NULL);
  gtk_box_pack_start (GTK_BOX (vbox_proj), scrolledwindow3, TRUE, TRUE, 0);

  gtk_widget_show (GTK_WIDGET(scrolledwindow3));
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow3), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_placement (GTK_SCROLLED_WINDOW (scrolledwindow3), GTK_CORNER_TOP_LEFT);

	//******************************* SOURCEVIEW PROJET
  PangoFontDescription *font_desc_projet;
	GtkWidget *sView_projet;
	GtkSourceLanguageManager *lm_projet;
   GtkSourceLanguage *language_projet = NULL;

  buffer_projet = GTK_SOURCE_BUFFER (gtk_source_buffer_new (NULL));

  sView_projet = gtk_source_view_new_with_buffer(buffer_projet);
  font_desc_projet = pango_font_description_from_string ("mono 8");
  gtk_widget_modify_font (sView_projet, font_desc_projet);
  pango_font_description_free (font_desc_projet);

  gtk_text_view_set_editable (GTK_TEXT_VIEW(sView_projet), FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(sView_projet),FALSE);

  gtk_source_view_set_show_right_margin(GTK_SOURCE_VIEW(sView_projet),TRUE);
  gtk_source_view_set_show_line_marks(GTK_SOURCE_VIEW(sView_projet),TRUE);
 	gtk_source_buffer_set_style_scheme(buffer_projet , scheme);
	lm_projet = gtk_source_language_manager_new();
	g_object_ref (lm_projet);
	g_object_set_data_full ( G_OBJECT (buffer_projet), "languages-manager",
                           lm_projet, (GDestroyNotify) g_object_unref);

	lm_projet = g_object_get_data (G_OBJECT (buffer_projet), "languages-manager");
	language_projet = gtk_source_language_manager_get_language (lm_projet,"sh");
	gtk_source_buffer_set_language (buffer_projet, language_projet);

  gtk_container_add (GTK_CONTAINER (scrolledwindow3), GTK_WIDGET(sView_projet));
  gtk_widget_show_all (GTK_WIDGET(scrolledwindow3));

  label_note2 = gtk_label_new (_("Projets"));
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

  vbox3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add (GTK_CONTAINER (notebook_down), GTK_WIDGET(vbox3));
  gtk_widget_show (GTK_WIDGET(vbox3));  
	//gtk_box_set_homogeneous(GTK_BOX(vbox3),FALSE);

  scrolledwindow4 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (GTK_WIDGET(scrolledwindow4));
  //gtk_container_add (GTK_CONTAINER (vbox3), GTK_WIDGET(scrolledwindow4));
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
	g_object_set_data_full ( G_OBJECT (buffer_note), "languages-manager",
                           lm_note, (GDestroyNotify) g_object_unref);

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

	GtkWidget *hbox3, *button2;

  vbox3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add (GTK_CONTAINER (notebook_down), GTK_WIDGET(vbox3));
  gtk_widget_show (GTK_WIDGET(vbox3));  

	//**************************** TOOLBAR MINIWEB
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

	gtk_box_pack_start (GTK_BOX (vbox3), toolbar_miniweb, FALSE, FALSE, 0);
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_miniweb), GTK_TOOLBAR_ICONS);
	gtk_widget_show (GTK_WIDGET(toolbar_miniweb)); 
  
	//****************************
  hbox3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox3));
  gtk_box_pack_start (GTK_BOX (vbox3), hbox3, FALSE, TRUE, 0);

  entry_web = gtk_entry_new ();
  gtk_widget_show (GTK_WIDGET(entry_web));
  gtk_box_pack_start (GTK_BOX (hbox3), entry_web, TRUE, TRUE, 0);

	//**************************** Autocomp
	GtkEntryCompletion *completion_entry_http;
	completion_entry_http = gtk_entry_completion_new();

	gtk_entry_completion_set_text_column(completion_entry_http, CONTACT_NAME_HTTP);
	gtk_entry_set_completion(GTK_ENTRY(entry_web), completion_entry_http);

	model_entry_http = gtk_list_store_new(1, G_TYPE_STRING);
 
	gtk_entry_completion_set_model(completion_entry_http, GTK_TREE_MODEL(model_entry_http));
	//****************************

	gtk_entry_set_text (GTK_ENTRY (entry_web), _("http://griffon.lasotel.fr/main.php?version=1.6.4"));

  button2 = gtk_button_new_from_stock (" Charger l'URL ");
  gtk_widget_show (GTK_WIDGET(button2));
  gtk_box_pack_start (GTK_BOX (hbox3), button2, FALSE, TRUE, 0);

  button_web_current = gtk_button_new_from_stock ((_(" Charger la page courante ")));
  gtk_widget_show (GTK_WIDGET(button_web_current));
  gtk_box_pack_start (GTK_BOX (hbox3), button_web_current, FALSE, TRUE, 0);

  g_signal_connect ((gpointer) button_web_current, "clicked",
                    G_CALLBACK (web_current_file),
                    NULL);

    webView = WEBKIT_WEB_VIEW(webkit_web_view_new());
	 gtk_widget_show (GTK_WIDGET(webView));

  	 g_signal_connect(webView, "new-window-policy-decision-requested",  
    G_CALLBACK(myadmin_new_window), webView);

		g_signal_connect(webView, "download-requested", G_CALLBACK(download_requested_cb), NULL);

  	 g_signal_connect(webView, "create-web-view",  
    G_CALLBACK(web_new_w_click_go), webView);

    GtkWidget *scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	 gtk_widget_show (GTK_WIDGET(scrolledWindow));
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolledWindow), GTK_WIDGET(webView));

			 gtk_box_pack_start(GTK_BOX(vbox3), GTK_WIDGET(scrolledWindow), TRUE, TRUE, 1);

  g_signal_connect ((gpointer) button2, "clicked",
                    G_CALLBACK (focus_web),
                    NULL);

  g_signal_connect ((gpointer) entry_web, "activate",
                    G_CALLBACK (enter_web),
                    entry_web);

  	 g_signal_connect(webView, "load-finished",  
    G_CALLBACK(miniweb_get_url), NULL);

  button_web_image = gtk_button_new_from_stock ((_(" Ouvrir avec Gimp ")));
  gtk_widget_show (GTK_WIDGET(button_web_image));
  gtk_box_pack_start (GTK_BOX (vbox3), button_web_image, FALSE, TRUE, 0);
  gtk_widget_hide (GTK_WIDGET(button_web_image));

  g_signal_connect ((gpointer) button_web_image, "clicked",
                    G_CALLBACK (open_gimp),
                    NULL);

  button_web_image2 = gtk_button_new_from_stock ((_(" Inserer le nom du fichier image dans la page courrante ")));
  gtk_widget_show (GTK_WIDGET(button_web_image2));
  gtk_box_pack_start (GTK_BOX (vbox3), button_web_image2, FALSE, TRUE, 0);
	gtk_widget_hide (GTK_WIDGET(button_web_image2));

  g_signal_connect ((gpointer) button_web_image2, "clicked",
                    G_CALLBACK (insert_image_web),
                    NULL);

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

	//*************************** Notebook myadmin
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

	//**************************** TOOLBAR MYADMIN
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

	gtk_box_pack_start (GTK_BOX (hbox_myadmin), toolbar_myadmin, FALSE, FALSE, 0);
	gtk_toolbar_set_style (GTK_TOOLBAR(toolbar_myadmin), GTK_TOOLBAR_ICONS);
	gtk_widget_show (GTK_WIDGET(toolbar_myadmin)); 

  entry_myadmin = gtk_entry_new ();
  gtk_widget_show (GTK_WIDGET(entry_myadmin));
  gtk_box_pack_start (GTK_BOX (hbox_myadmin), entry_myadmin, FALSE, FALSE, 0);
 
	//**************************** Autocomp
	GtkEntryCompletion *completion_entry_http2;
	completion_entry_http2 = gtk_entry_completion_new();

	gtk_entry_completion_set_text_column(completion_entry_http2, CONTACT_NAME_HTTP2);
	gtk_entry_set_completion(GTK_ENTRY(entry_myadmin), completion_entry_http2);

	model_entry_http2 = gtk_list_store_new(1, G_TYPE_STRING);
 
	gtk_entry_completion_set_model(completion_entry_http2, GTK_TREE_MODEL(model_entry_http2));
	//****************************

	gtk_entry_set_text (GTK_ENTRY (entry_myadmin), _("http://"));

  g_signal_connect ((gpointer) entry_myadmin, "activate",
                    G_CALLBACK (enter_myweb),
                    NULL);

	//****************************
   vbox_myadmin = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
 gtk_box_pack_start(GTK_BOX(hbox_myadmin), GTK_WIDGET(vbox_myadmin), TRUE, TRUE, 1);

	gtk_widget_show (GTK_WIDGET(vbox_myadmin)); 
      
    webView_myadmin = WEBKIT_WEB_VIEW(webkit_web_view_new());
	 gtk_widget_show (GTK_WIDGET(webView_myadmin));

		//************** AJOUT POUR EVITER LE PROBLEME DE DELETE
		view_myadmin = gtk_tree_view_new(); 

    GtkWidget *scrolledWindow_myadmin = gtk_scrolled_window_new(NULL, NULL);
	  gtk_widget_show (GTK_WIDGET(scrolledWindow_myadmin));
     gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow_myadmin),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolledWindow_myadmin), GTK_WIDGET(webView_myadmin));

      gtk_box_pack_start(GTK_BOX(vbox_myadmin), GTK_WIDGET(scrolledWindow_myadmin), TRUE, TRUE, 1);
   
  view_a = create_view_and_model_myadmin();
  gtk_widget_show (GTK_WIDGET(view_a));
  selection_myadmin = gtk_tree_view_get_selection(GTK_TREE_VIEW(view_a));
  gtk_box_pack_start(GTK_BOX(vbox_myadmin), view_a, FALSE, TRUE, 1);        

  	 g_signal_connect(selection_myadmin, "changed",  
      G_CALLBACK(on_changed_myadmin), statusbar);  
 
  	 g_signal_connect(webView_myadmin, "load-finished",  
      G_CALLBACK(myadmin_get_url), NULL);

  	 g_signal_connect(webView_myadmin, "new-window-policy-decision-requested",  
      G_CALLBACK(myadmin_new_window), webView_myadmin);

  	 g_signal_connect(webView_myadmin, "create-web-view",  
    G_CALLBACK(web_new_w_click_go), webView_myadmin);

//***************************
  label_note3 = gtk_label_new (_("Web"));
  gtk_widget_show (GTK_WIDGET(label_note3));

	gtk_widget_set_size_request (label_note3, 150, 20);

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
     gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow_myadmin_traduc),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolledWindow_myadmin_traduc), GTK_WIDGET(webView_myadmin_traduc));

    gtk_container_add(GTK_CONTAINER(notebook_myadmin), GTK_WIDGET(scrolledWindow_myadmin_traduc));  

			//******************************** SIGNALE WEBKIT

  	 g_signal_connect(webView_myadmin, "new-window-policy-decision-requested",  
      G_CALLBACK(myadmin_new_window), webView_myadmin);
			
			g_signal_connect(webView_myadmin, "download-requested", G_CALLBACK(download_requested_cb), NULL);

  	 g_signal_connect(webView_myadmin_traduc, "create-web-view",  
    G_CALLBACK(web_new_w_click_go), webView_myadmin_traduc);

//***************************
  label_note3 = gtk_label_new (_("Traduction"));
  gtk_widget_show (GTK_WIDGET(label_note3));

	gtk_widget_set_size_request (label_note3, 150, 20);

  hbox_note = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox_note));

  image2 = gtk_image_new_from_stock ("gtk-convert", GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_widget_show (GTK_WIDGET(image2));
  gtk_box_pack_start (GTK_BOX (hbox_note), image2, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox_note), label_note3, TRUE, TRUE, 0);		
 
 gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook_myadmin), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_myadmin), 1), hbox_note);
 gtk_notebook_set_tab_detachable (GTK_NOTEBOOK (notebook_myadmin), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook_myadmin), 1), TRUE);

   webView_myadmin_aide = WEBKIT_WEB_VIEW(webkit_web_view_new());
	 gtk_widget_show (GTK_WIDGET(webView_myadmin_aide));

    GtkWidget *scrolledWindow_myadmin_aide = gtk_scrolled_window_new(NULL, NULL);
	  gtk_widget_show (GTK_WIDGET(scrolledWindow_myadmin_aide));
     gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow_myadmin_aide),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolledWindow_myadmin_aide), GTK_WIDGET(webView_myadmin_aide));

    gtk_container_add(GTK_CONTAINER(notebook_myadmin), GTK_WIDGET(scrolledWindow_myadmin_aide));  

  	 g_signal_connect(webView_myadmin, "new-window-policy-decision-requested",  
      G_CALLBACK(myadmin_new_window), webView_myadmin_aide);

  	 g_signal_connect(webView_myadmin, "create-web-view",  
    G_CALLBACK(web_new_w_click_go), webView_myadmin_aide);

//***************************
  label_note3 = gtk_label_new (_("Aide/Recherche"));
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
//***************************

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

//************** notebook tools



  statusbar1 = gtk_statusbar_new ();
  gtk_box_pack_start (GTK_BOX (mainvbox), statusbar1, FALSE, FALSE, 0);
	gtk_widget_show (GTK_WIDGET(statusbar1));

	win_tips_autocomp=NULL;

  g_signal_connect ((gpointer) notebook1, "switch_page",
                    G_CALLBACK (on_notebook1_switch_page),
                    NULL);

  g_signal_connect ((gpointer) notebook3, "focus-in-event",
                    G_CALLBACK (switch_filechooser),
                    NULL);
  
  gtk_window_add_accel_group (GTK_WINDOW (tea_main_window), accel_group);

  gtk_text_buffer_create_tag (GTK_TEXT_BUFFER(log_memo_textbuffer), "lm_greet",
                              "foreground", "#6c0606", NULL);

  gtk_text_buffer_create_tag (GTK_TEXT_BUFFER(log_memo_textbuffer), "gray_bg",
                              "background", "gray", NULL);

  tag_lm_error = gtk_text_buffer_create_tag (GTK_TEXT_BUFFER(log_memo_textbuffer), "lm_error",
                                             "foreground", "red", NULL);

  gtk_text_buffer_create_tag (GTK_TEXT_BUFFER(log_memo_textbuffer), "lm_normal",
                              "foreground", "black", NULL);

  gtk_text_buffer_create_tag (GTK_TEXT_BUFFER(log_memo_textbuffer), "lm_advice",
                              "foreground", "navy", NULL);

  set_lm_colors ();
  widget_apply_colors (tv_logmemo);

  
	//******************************* IMAGE Griffon icon dans zone d'information
	GdkPixbuf *pixbuf;
	GtkTextBuffer* buffer_img;
	GtkTextIter itFin;
	pixbuf = gdk_pixbuf_new_from_file("/usr/local/share/griffon/pixmaps/griffon_start.png", NULL);
	buffer_img = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv_logmemo));
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer_img), &itFin);
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer_img), &itFin);
	gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(buffer_img),&itFin,pixbuf);
	tv_logmemo_set_pos (0);
  log_to_memo ("\n\n(C)2013 Philippe Muller <pmuller@lasotel.fr>\n Griffon 1.6.4 - http://griffon.lasotel.fr\n\n", NULL, LM_GREET); 
	tv_logmemo_set_pos (0);
  ui_init ();

  /*********Signaux et fonctions ratache*********/

  g_signal_connect ((gpointer) button_note1, "clicked",
                    G_CALLBACK (clear_note),
                    NULL);

  g_signal_connect ((gpointer) button_include1, "clicked",
                    G_CALLBACK (scan_include),
                    NULL);

  g_signal_connect ((gpointer) button_proj1, "clicked",
                    G_CALLBACK (on_mni_project_open),
                    NULL);

  g_signal_connect ((gpointer) button_proj2, "clicked",
                    G_CALLBACK (on_mni_project_save_as),
                    NULL);

  g_signal_connect ((gpointer) button_proj3, "clicked",
                    G_CALLBACK (on_mni_project_make),
                    NULL);

  g_signal_connect ((gpointer) button_proj4, "clicked",
                    G_CALLBACK (on_mni_show_project_run),
                    NULL);

  g_signal_connect ((gpointer) button_proj5, "clicked",
                    G_CALLBACK (on_mni_project_new),
                    NULL);

  g_signal_connect (notebook_down, "create-window",
                    G_CALLBACK (window_creation_function), NULL);

  g_signal_connect (notebook2, "create-window",
                    G_CALLBACK (window_creation_function), NULL);

  g_signal_connect (notebook_myadmin, "create-window",
                    G_CALLBACK (window_creation_function), NULL);


  g_signal_connect (notebook_term, "create-window",
                    G_CALLBACK (window_creation_function), NULL);

  g_signal_connect (notebook3, "create-window",
                    G_CALLBACK (window_creation_function), NULL);

		gtk_notebook_set_current_page(GTK_NOTEBOOK (notebook_down),4);
		gtk_notebook_set_current_page(GTK_NOTEBOOK (notebook2),0);
  no_onglet_open() ;  

	webkit_web_view_load_uri(webView, "http://griffon.lasotel.fr/main.php?version=1.6.4");
	webkit_web_view_load_uri(webView_myadmin, "https://www.google.fr/");
	webkit_web_view_load_uri(webView_myadmin_traduc, "https://translate.google.fr/?hl=fr&tab=wT");
	webkit_web_view_load_uri(webView_myadmin_aide, "https://www.google.fr/");
	gtk_notebook_set_current_page(GTK_NOTEBOOK (notebook3),1);
             
	window_run = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  return tea_main_window;
}

//******************************* affichage d'icon en status
GtkWidget* icon_affiche_ok (void)
{
  gtk_widget_destroy(icon_ok);
  icon_ok = gtk_image_new_from_file("/usr/local/share/griffon/pixmaps/griffon_ok.png");
  gtk_widget_set_size_request (icon_ok, 35, 10);
  gtk_box_pack_start (GTK_BOX (hbox_bar), icon_ok, FALSE, FALSE, 0);
  gtk_widget_show (GTK_WIDGET(icon_ok));
  return window_center;
}

//******************************* affichage d'icon en status
GtkWidget* icon_affiche_stop (void)
{
  gtk_widget_destroy(icon_ok);
  icon_ok = gtk_image_new_from_file("/usr/local/share/griffon/pixmaps/griffon_stop.png");
  gtk_widget_set_size_request (icon_ok, 35, 10);
  gtk_box_pack_start (GTK_BOX (hbox_bar), icon_ok, FALSE, FALSE, 0);
  gtk_widget_show (GTK_WIDGET(icon_ok));
  return window_center;
}

//******************************* affichage d'icon en status
GtkWidget* icon_affiche_net (void)
{
  gtk_widget_destroy(icon_ok);
  icon_ok = gtk_image_new_from_file("/usr/local/share/griffon/pixmaps/griffon_net.png");
  gtk_widget_set_size_request (icon_ok, 35, 10);
  gtk_box_pack_start (GTK_BOX (hbox_bar), icon_ok, FALSE, FALSE, 0);
  gtk_widget_show (GTK_WIDGET(icon_ok));
  return window_center;
}

//******************************* affichage d'icon en status
GtkWidget* icon_affiche_bug (void)
{
  gtk_widget_destroy(icon_ok);
  icon_ok = gtk_image_new_from_file("/usr/local/share/griffon/pixmaps/griffon_advance.png");
  gtk_widget_set_size_request (icon_ok, 35, 10);
  gtk_box_pack_start (GTK_BOX (hbox_bar), icon_ok, FALSE, FALSE, 0);
  gtk_widget_show (GTK_WIDGET(icon_ok));
  return window_center;
}

//******************************* affichage d'icon en status
GtkWidget* icon_affiche_save (void)
{
  gtk_widget_destroy(icon_ok);
  icon_ok = gtk_image_new_from_file("/usr/local/share/griffon/pixmaps/griffon_save.png");
  gtk_widget_set_size_request (icon_ok, 35, 10);
  gtk_box_pack_start (GTK_BOX (hbox_bar), icon_ok, FALSE, FALSE, 0);
  gtk_widget_show (GTK_WIDGET(icon_ok));
  save_controle=1;
	controle_save_page_icon();
  return window_center;
}

//******************************* affichage d'icon en status
GtkWidget* icon_affiche_nosave (void)
{
  gtk_widget_destroy(icon_ok);
  icon_ok = gtk_image_new_from_file("/usr/local/share/griffon/pixmaps/griffon_nosave.png");
  gtk_widget_set_size_request (icon_ok, 35, 10);
  gtk_box_pack_start (GTK_BOX (hbox_bar), icon_ok, FALSE, FALSE, 0);
  gtk_widget_show (GTK_WIDGET(icon_ok));
	controle_save_page_icon_no();
  return window_center;
}

//******************************* recherche via devhelp
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
		log_to_memo (_("Vous devez installer le package DevHelp"), NULL, LM_ERROR);
			statusbar_msg (_("Search DevHelp [ERROR]"));
		}
		else
		{	  
		strcpy(mot,"devhelp -s ");
		strcat(mot,titre);
		strcat(mot," &");

		int systemRet =system (mot);
		if(systemRet == -1){return NULL;}
		log_to_memo (_("%s Recherche dans DevHelp"), titre, LM_NORMAL);
			statusbar_msg (_("Search DevHelp [OK]"));
		fclose(fichier);
		}
	return NULL;
}	

//******************************* afficher les voltes droit et gauche
GtkWidget* open_tools (void)
{
gtk_widget_show(GTK_WIDGET(notebook2));
gtk_widget_show(GTK_WIDGET(filechooserwidget2));
return NULL;
}

//******************************* cacher les volets droit et gauche
GtkWidget* close_tools (void)
{
gtk_widget_hide(GTK_WIDGET(notebook2));
gtk_widget_hide(GTK_WIDGET(filechooserwidget2));
return NULL;
}

//******************************* cacher les volets droit
GtkWidget* close_tools2 (void)
{
gtk_widget_hide(GTK_WIDGET(notebook2));
gtk_widget_show(GTK_WIDGET(filechooserwidget2));
return NULL;
}

//******************************* effacer les notes
GtkWidget* clear_note (void)
{
gtk_text_buffer_set_text(GTK_TEXT_BUFFER(buffer_note), "", -1);
gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_note), (_("\nPour afficher les notes [ALT+M]\n")), -1);
gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_note), (_("Pour integrer une séléction dans la note [CTR+E]\n\n____________________________________________________________________________________________________________________________________________________\n\n")), -1);
return NULL;
}

//******************************* fichier selectionné 
GtkWidget* file_ok_sel (void)
{
gchar *file_selected;
file_selected = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER( filechooserwidget2));
doc_open_file (file_selected);
return NULL;
}

//******************************* fenetre a propos de griffon....
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

  image1 = gtk_image_new_from_file("/usr/local/share/griffon/pixmaps/splash_griffon.png");
  gtk_widget_show (GTK_WIDGET(image1));
  gtk_box_pack_start (GTK_BOX (vbox1), image1, TRUE, TRUE, 0);

  label1 = gtk_label_new (_("\nGriffon IDE 1.6.4 \n\nAuteur \t: Philippe Muller \nEmail \t\t: pmuller@lasotel.fr\n\nDéveloppeur et Administrateur système GNU/Linux pour Lasotel ( Lyon ).\n\n"));
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

  g_signal_connect_swapped ((gpointer) button1, "clicked",
                            G_CALLBACK (gtk_widget_destroy),
                            window1);

  return window1;
}

GtkWidget* create_custom_win (void)
{
   gchar *titre;  
	titre = gtk_editable_get_chars(GTK_EDITABLE(entry_help_custom),0, -1);
	doc_insert_at_cursor (cur_text_doc, titre);	 
  return entry_custom;
}

//********************************* TREEVIEW DE TEST
enum
{
  COLUMN = 0,
  NUM_COLS
} ;

//******************************* tree aide composition
static GtkTreeModel * create_and_fill_model_help (void)
{
  GtkTreeStore *treestore;
  GtkTreeIter toplevel, child;

  treestore = gtk_tree_store_new(NUM_COLS,
                  G_TYPE_STRING);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("Langages interprétés")),
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "[BASH]",-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "[PERL]",-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "[PHP]",-1);


  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("Base de données")),
                     -1);
                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "[MYSQL]",-1);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("Web mise en page")),
                     -1);
                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "[HTML]",-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "[CSS]",-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "[JAVASCRIPT/JQUERY]",-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "[HTACCESS]",-1);
                                          
  return GTK_TREE_MODEL(treestore);
}

//******************************* affichage du modele tree pour les aides
GtkWidget * create_view_and_model_help (void)
{
  GtkTreeViewColumn *col;
  GtkCellRenderer *renderer;
  GtkWidget *view;
  GtkTreeModel *model;

  view = gtk_tree_view_new();

  col = gtk_tree_view_column_new();
  gtk_tree_view_column_set_title(col, (_("Langages de programmation")));
  gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);

  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(col, renderer, TRUE);
  gtk_tree_view_column_add_attribute(col, renderer, 
      "text", COLUMN);

	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(view),TRUE);
	gtk_tree_view_set_enable_tree_lines (GTK_TREE_VIEW(view),TRUE);

  model = create_and_fill_model_help();
  gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);
  g_object_unref(model); 

		gtk_tree_view_expand_all (GTK_TREE_VIEW(view));

  return view;
}

//******************************* focus sur le terminal
void focus_term ()
{
gtk_widget_grab_focus (vte);
}

//******************************* rechargement et focus sur longlet mini web
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
	griffon_notify(_("La visualisation est disponible dans l'onglet Mini Web"));
}

//******************************* MyAdmin composition
static GtkTreeModel * create_and_fill_model_myadmin (void)
{
  GtkTreeStore *treestore;
  GtkTreeIter toplevel, child;

  treestore = gtk_tree_store_new(NUM_COLS,
                  G_TYPE_STRING);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("Liste MyAdmin HTTP                                          ")),
                     -1);

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

//******************************* affichage du modele tree pour les Myadmin
GtkWidget * create_view_and_model_myadmin (void)
{
  GtkTreeViewColumn *col;
  GtkCellRenderer *renderer;

  GtkTreeModel *model;

	gtk_widget_destroy(view_myadmin);

  view_myadmin = gtk_tree_view_new();   
  col = gtk_tree_view_column_new();
  gtk_tree_view_column_set_title(col, (_("\n[ALT+N] pour configurer \n[ALT+H] pour recharger\n")));
  gtk_tree_view_append_column(GTK_TREE_VIEW(view_myadmin), col);

 
  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(col, renderer, TRUE);
  gtk_tree_view_column_add_attribute(col, renderer, 
      "text", COLUMN);

  model = create_and_fill_model_myadmin();
  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(view_myadmin),TRUE);
	gtk_tree_view_set_enable_tree_lines (GTK_TREE_VIEW(view_myadmin),TRUE);
  gtk_tree_view_set_model(GTK_TREE_VIEW(view_myadmin), model);
  g_object_unref(model); 

		gtk_tree_view_expand_all (GTK_TREE_VIEW(view_myadmin));

  return view_myadmin;
}

//******************************* match et envoi des fonction d'aide insert
void  on_changed_myadmin(GtkWidget *widget) 
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  char *value;

  if (gtk_tree_selection_get_selected(
      GTK_TREE_SELECTION(widget), &model, &iter)) {		
    gtk_tree_model_get(model, &iter, COLUMN, &value,  -1);            
	webkit_web_view_load_uri(webView_myadmin, value);
    g_free(value);
  }
	
}

//******************************* Reload myadmin
void  myadmin() 
{
  gtk_widget_destroy(view_a);
  
  view_a = create_view_and_model_myadmin();
  gtk_widget_show (GTK_WIDGET(view_a));
  selection_myadmin = gtk_tree_view_get_selection(GTK_TREE_VIEW(view_a));
  gtk_widget_show (GTK_WIDGET(selection_myadmin));
  gtk_box_pack_start(GTK_BOX(vbox_myadmin), view_a, FALSE, TRUE, 1);
  
  	 g_signal_connect(selection_myadmin, "changed",  
      G_CALLBACK(on_changed_myadmin), NULL);  
}

//******************************* si pas d'onglet ouvert
void  no_onglet_open() 
{
	if (! get_page_text())
	{
	gtk_widget_hide(GTK_WIDGET(notebook1));	
	gtk_widget_show(GTK_WIDGET(scrolledWindow_editor));
	gtk_widget_show(GTK_WIDGET(webView_editor));
	}
	else
	{
	gtk_widget_hide(GTK_WIDGET(scrolledWindow_editor));	
	gtk_widget_hide(GTK_WIDGET(webView_editor));	
	gtk_widget_show(GTK_WIDGET(notebook1));
	}
}

//******************************* fenetre a propos de griffon....
GtkWidget* doc_window (void)
{
  GtkWidget *window1;
  GtkWidget *vbox1;
	WebKitWebView *webView_doc;

  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_transient_for(GTK_WINDOW(window1),GTK_WINDOW(tea_main_window));
  gtk_window_set_title (GTK_WINDOW (window1), _((_("Documentation Griffon IDE"))));
  gtk_window_resize (GTK_WINDOW (window1), 930, 600);
  gtk_widget_show (GTK_WIDGET(window1));

  vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (GTK_WIDGET(vbox1));
  gtk_container_add (GTK_CONTAINER (window1), GTK_WIDGET(vbox1));

    webView_doc = WEBKIT_WEB_VIEW(webkit_web_view_new());
	 gtk_widget_show (GTK_WIDGET(webView_doc));

  	 g_signal_connect(webView_doc, "new-window-policy-decision-requested",  
    G_CALLBACK(myadmin_new_window), webView_doc);

  	 g_signal_connect(webView_doc, "create-web-view",  
    G_CALLBACK(web_new_w_click_go), webView_doc);

    GtkWidget *scrolledWindow_doc = gtk_scrolled_window_new(NULL, NULL);
	 gtk_widget_show (GTK_WIDGET(scrolledWindow_doc));
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow_doc),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolledWindow_doc), GTK_WIDGET(webView_doc));

			gtk_box_pack_start(GTK_BOX(vbox1), GTK_WIDGET(scrolledWindow_doc), TRUE, TRUE, 1);

   webkit_web_view_load_uri(webView_doc, "http://griffon.lasotel.fr/DOC/doc.html");

  return window1;
}

//******************************* fenetre de vérification de version
GtkWidget* version_window (void)
{
  GtkWidget *window1;
  GtkWidget *vbox1;
	WebKitWebView *webView_doc;

  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_transient_for(GTK_WINDOW(window1),GTK_WINDOW(tea_main_window));
  gtk_window_set_title (GTK_WINDOW (window1), _((_("Version Griffon IDE"))));
  gtk_window_resize (GTK_WINDOW (window1), 670, 255);
  gtk_widget_show (GTK_WIDGET(window1));

  vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (GTK_WIDGET(vbox1));
  gtk_container_add (GTK_CONTAINER (window1), GTK_WIDGET(vbox1));

   webView_doc = WEBKIT_WEB_VIEW(webkit_web_view_new());
	gtk_widget_show (GTK_WIDGET(webView_doc));

    gtk_container_add(GTK_CONTAINER(vbox1), GTK_WIDGET(webView_doc));

   webkit_web_view_load_uri(webView_doc, "http://griffon.lasotel.fr/version.php?version=1.6.4");

  return window1;
}

//******************************* Bug rapport
GtkWidget* rapport_window (void)
{
  GtkWidget *window1;
  GtkWidget *vbox1;
  WebKitWebView *webView_doc;

  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_transient_for(GTK_WINDOW(window1),GTK_WINDOW(tea_main_window));
  gtk_window_set_title (GTK_WINDOW (window1), _((_("Rapport Griffon IDE"))));
  gtk_window_resize (GTK_WINDOW (window1), 670, 345);
  gtk_widget_show (GTK_WIDGET(window1));

  vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (GTK_WIDGET(vbox1));
  gtk_container_add (GTK_CONTAINER (window1), GTK_WIDGET(vbox1));

    webView_doc = WEBKIT_WEB_VIEW(webkit_web_view_new());
	 gtk_widget_show (GTK_WIDGET(webView_doc));

    gtk_container_add(GTK_CONTAINER(vbox1), GTK_WIDGET(webView_doc));

   webkit_web_view_load_uri(webView_doc, "http://griffon.lasotel.fr/bug_rapport.php?clef=1.6.4");

  return window1;
}

//******************************* Afficher dans le webview la page du fichier courrant
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

void  insert_img_autoconf_var() 
{
	//******************************* IMAGE Griffon icon dans zone d'information autoconf
	GdkPixbuf *pixbuf;
	GtkTextIter itFin;
	pixbuf = gdk_pixbuf_new_from_file("/usr/local/share/griffon/pixmaps/griffon_package.png", NULL);
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer_scan), &itFin);
	gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(buffer_scan),&itFin,pixbuf);
}

void  insert_img_autoconf_mot() 
{
	//******************************* IMAGE Griffon icon dans zone d'information autoconf
	GdkPixbuf *pixbuf;
	GtkTextIter itFin;
	pixbuf = gdk_pixbuf_new_from_file("/usr/local/share/griffon/pixmaps/griffon_man.png", NULL);
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer_scan), &itFin);
	gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(buffer_scan),&itFin,pixbuf);
}

void  insert_img_autoconf_fonc() 
{
	//******************************* IMAGE Griffon icon dans zone d'information autoconf
	GdkPixbuf *pixbuf;
	GtkTextIter itFin;
	pixbuf = gdk_pixbuf_new_from_file("/usr/local/share/griffon/pixmaps/griffon_exe.png", NULL);
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer_scan), &itFin);
	gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(buffer_scan),&itFin,pixbuf);
}

//******************************* Scrol en position bottom
void  fin_de_scroll_pos() 
{
	GtkTextIter itFin;
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer_scan), &itFin);
	gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW(sView_scan), &itFin, 0.0, FALSE, 0.0, 0.0 );
}

//******************************* IMAGE Griffon icon dans zone d'information logmemo
void  icon_log_logmemo() 
{
	GdkPixbuf *pixbuf;
	GtkTextBuffer* buffer_img;
	GtkTextIter itFin;
	pixbuf = gdk_pixbuf_new_from_file("/usr/local/share/griffon/pixmaps/griffon_log.png", NULL);
	buffer_img = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv_logmemo));
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer_img), &itFin);
	gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(buffer_img),&itFin,pixbuf);
}

//******************************* IMAGE Griffon icon dans zone d'information logmemo
void  icon_list_logmemo() 
{
	GdkPixbuf *pixbuf;
	GtkTextBuffer* buffer_img;
	GtkTextIter itFin;
	pixbuf = gdk_pixbuf_new_from_file("/usr/local/share/griffon/pixmaps/griffon_man.png", NULL);
	buffer_img = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv_logmemo));
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer_img), &itFin);
	gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(buffer_img),&itFin,pixbuf);
}

//******************************* IMAGE Griffon icon dans zone d'information logmemo
void  icon_stop_logmemo() 
{
	GdkPixbuf *pixbuf;
	GtkTextBuffer* buffer_img;
	GtkTextIter itFin;
	pixbuf = gdk_pixbuf_new_from_file("/usr/local/share/griffon/pixmaps/griffon_stop2.png", NULL);
	buffer_img = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv_logmemo));
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer_img), &itFin);
	gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(buffer_img),&itFin,pixbuf);
}

//******************************* IMAGE Griffon icon dans zone d'information logmemo
void  icon_ok_logmemo() 
{
	GdkPixbuf *pixbuf;
	GtkTextBuffer* buffer_img;
	GtkTextIter itFin;
	pixbuf = gdk_pixbuf_new_from_file("/usr/local/share/griffon/pixmaps/griffon_ok2.png", NULL);
	buffer_img = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv_logmemo));
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer_img), &itFin);
	gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(buffer_img),&itFin,pixbuf);
}

//******************************* IMAGE Griffon icon dans zone d'information logmemo
void  icon_man_logmemo() 
{
	GdkPixbuf *pixbuf;
	GtkTextBuffer* buffer_img;
	GtkTextIter itFin;
	pixbuf = gdk_pixbuf_new_from_file("/usr/local/share/griffon/pixmaps/griffon_man.png", NULL);
	buffer_img = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv_logmemo));
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer_img), &itFin);
	gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(buffer_img),&itFin,pixbuf);
}

//******************************* IMAGE Griffon icon dans zone d'information logmemo
void  icon_save_logmemo() 
{
	GdkPixbuf *pixbuf;
	GtkTextBuffer* buffer_img;
	GtkTextIter itFin;
	pixbuf = gdk_pixbuf_new_from_file("/usr/local/share/griffon/pixmaps/griffon_save2.png", NULL);
	buffer_img = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv_logmemo));
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer_img), &itFin);
	gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(buffer_img),&itFin,pixbuf);			
}

//******************************* IMAGE Griffon icon dans zone d'information logmemo
void  icon_nosave_logmemo() 
{
	GdkPixbuf *pixbuf;
	GtkTextBuffer* buffer_img;
	GtkTextIter itFin;
	pixbuf = gdk_pixbuf_new_from_file("/usr/local/share/griffon/pixmaps/griffon_nosave2.png", NULL);
	buffer_img = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv_logmemo));
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer_img), &itFin);
	gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(buffer_img),&itFin,pixbuf);
}

//******************************* Controle si le fichier est sauvegardé pour l'affichage d'icons
void  controle_save() 
{
	if(save_controle==1)
	{
	icon_affiche_ok();	
	save_controle=2;	
	}
}

//******************************* Controle si le fichier est sauvegardé pour l'affichage d'icons
void  controle_save_page_icon_no() 
{
	if (! get_page_text()) return;
	if(gtk_widget_get_visible (cur_text_doc->icon)){gtk_widget_set_visible (cur_text_doc->icon,FALSE);}

}

//******************************* Controle si le fichier est sauvegardé pour l'affichage d'icons
void  controle_save_page_icon() 
{
if (! get_page_text()) return;

	gchar **a = g_strsplit (cur_text_doc->file_name, "_", -1);
	if (strcmp("noname", a[0]) == 0) return;

	if(! gtk_widget_get_visible (cur_text_doc->icon)){gtk_widget_set_visible (cur_text_doc->icon,TRUE);}
}

//******************************* SFTP composition
GtkTreeModel * create_and_fill_model_sftp (void)
{
  GtkTreeStore *treestore;
  GtkTreeIter toplevel, child;

  treestore = gtk_tree_store_new(NUM_COLS,
                  G_TYPE_STRING);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("Liste SFTP                                          ")),
                     -1);

  FILE *fich;
  char carac;
	char mot[100];
	mot[0]='\0';
	
  fich=fopen(confile.tea_sftp,"r");
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

//******************************* affichage du modele tree pour les sftp
GtkWidget * create_view_and_model_sftp (void)
{
  GtkTreeViewColumn *col;
  GtkCellRenderer *renderer;

  GtkTreeModel *model;

  view_sftp = gtk_tree_view_new();   
  col = gtk_tree_view_column_new();
  gtk_tree_view_column_set_title(col, (_("\nHistorique SFTP\n")));
  gtk_tree_view_append_column(GTK_TREE_VIEW(view_sftp), col);

 
  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(col, renderer, TRUE);
  gtk_tree_view_column_add_attribute(col, renderer, 
      "text", COLUMN);

  model = create_and_fill_model_sftp();
  gtk_tree_view_set_model(GTK_TREE_VIEW(view_sftp), model);
  g_object_unref(model); 

	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(view_sftp),TRUE);
	gtk_tree_view_set_enable_tree_lines (GTK_TREE_VIEW(view_sftp),TRUE);

	gtk_tree_view_expand_all (GTK_TREE_VIEW(view_sftp));

  return view_sftp;
}

//******************************* match et envoi des fonction sftp
void  on_changed_sftp(GtkWidget *widget) 
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  char *value;
	char mot[150];

const char *home_dir = g_getenv ("HOME");

  if (gtk_tree_selection_get_selected(
      GTK_TREE_SELECTION(widget), &model, &iter)) {

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
    
		new_terminal_ssh (a[0],a[1]);

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
	log_to_memo (_("%s montage SFTP dans le répertoire Griffon_MONTAGE_SFTP"), a[0], LM_NORMAL);
		statusbar_msg (_("Mount SFTP [OK]"));
	griffon_notify(_("Mount SFTP"));
	icon_affiche_net ();                                                                                                 
   g_free(value);
  }
	
}

//******************************* Reload sftp
void  sftp_reload() 
{
  gtk_widget_destroy(view_sftp);

  view_sftp = create_view_and_model_sftp();
  gtk_widget_show (GTK_WIDGET(view_sftp));
  selection_sftp = gtk_tree_view_get_selection(GTK_TREE_VIEW(view_sftp));
  gtk_widget_show (GTK_WIDGET(selection_sftp));
  gtk_container_add(GTK_CONTAINER(scrolledWindow_sftp), GTK_WIDGET(view_sftp));
  
  	 g_signal_connect(selection_sftp, "changed",  
      G_CALLBACK(on_changed_sftp), NULL);  
}

//******************************* SFTP composition
GtkTreeModel * create_and_fill_model_ftp (void)
{
  GtkTreeStore *treestore;
  GtkTreeIter toplevel, child;

  treestore = gtk_tree_store_new(NUM_COLS,
                  G_TYPE_STRING);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("Liste FTP                                          ")),
                     -1);

  FILE *fich;
  char carac;
	char mot[100];
	mot[0]='\0';
	
  fich=fopen(confile.tea_ftp,"r");
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

//******************************* affichage du modele tree pour les sftp
GtkWidget * create_view_and_model_ftp (void)
{

  GtkTreeViewColumn *col;
  GtkCellRenderer *renderer;

  GtkTreeModel *model;

  view_ftp = gtk_tree_view_new();   
  col = gtk_tree_view_column_new();
  gtk_tree_view_column_set_title(col, "\nHistorique FTP\n");
  gtk_tree_view_append_column(GTK_TREE_VIEW(view_ftp), col);

 
  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(col, renderer, TRUE);
  gtk_tree_view_column_add_attribute(col, renderer, 
      "text", COLUMN);

  model = create_and_fill_model_ftp();
  gtk_tree_view_set_model(GTK_TREE_VIEW(view_ftp), model);
  g_object_unref(model); 

	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(view_ftp),TRUE);
	gtk_tree_view_set_enable_tree_lines (GTK_TREE_VIEW(view_ftp),TRUE);


		gtk_tree_view_expand_all (GTK_TREE_VIEW(view_ftp));

  return view_ftp;
}

//******************************* match et envoi des fonction sftp
void  on_changed_ftp(GtkWidget *widget) 
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  char *value;
	char mot[150];

	const char *home_dir = g_getenv ("HOME");

  if (gtk_tree_selection_get_selected(
      GTK_TREE_SELECTION(widget), &model, &iter)) {		
		
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
	log_to_memo (_("%s montage FTP dans le répertoire Griffon_MONTAGE_SFTP"), a[0], LM_NORMAL);
	griffon_notify(_("Mount FTP"));
	icon_affiche_net ();                                                                                                 
    g_free(value);
	statusbar_msg (_("Mount FTP [OK]"));
  }

}

//******************************* Reload ftp
void  ftp_reload() 
{
  gtk_widget_destroy(view_ftp);
  
  view_ftp = create_view_and_model_ftp();
  gtk_widget_show (GTK_WIDGET(view_ftp));
  selection_ftp = gtk_tree_view_get_selection(GTK_TREE_VIEW(view_ftp));
	gtk_widget_show (GTK_WIDGET(selection_ftp));
   gtk_container_add(GTK_CONTAINER(scrolledWindow_ftp), GTK_WIDGET(view_ftp));
  
  	 g_signal_connect(selection_ftp, "changed",  
      G_CALLBACK(on_changed_ftp), NULL);  
}

//******************************* Vide fichier historique
void  vide_configuration_ftp() 
{
save_string_to_file_vide(confile.tea_ftp,"");
ftp_reload();
}

//******************************* Vide fichier historique
void  vide_configuration_sftp() 
{
save_string_to_file_vide(confile.tea_sftp,"");
sftp_reload();
}

//******************************* switch du mini web si page html
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

//**************************** liste vIEW TEST
 GtkListStore * create_liststore()
{ 
    store = gtk_list_store_new(3, GDK_TYPE_PIXBUF, G_TYPE_STRING,G_TYPE_STRING);
    return store;
}
 
void add_to_list(gchar *str,gchar *str2)
{
    GtkTreeIter    iter;
    GdkPixbuf     *icon;

  icon = gdk_pixbuf_new_from_file("/usr/local/share/griffon/pixmaps/griffon_log.png", NULL);
 
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter,
                       COL_ICON, icon,
                       COL_TEXT, str,
							  COL_TEXT2, str2,
                       -1);	
}

void add_to_list_fc(gchar *str,gchar *str2)
{
    GtkTreeIter    iter;
    GdkPixbuf     *icon;

  icon = gdk_pixbuf_new_from_file("/usr/local/share/griffon/pixmaps/griffon_exe.png", NULL);
 
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter,
                       COL_ICON, icon,
                       COL_TEXT, str,
							  COL_TEXT2, str2,
                       -1);	
}

void add_to_list_var(gchar *str,gchar *str2)
{
    GtkTreeIter    iter;
    GdkPixbuf     *icon;

  icon = gdk_pixbuf_new_from_file("/usr/local/share/griffon/pixmaps/griffon_package.png", NULL);
 
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter,
                       COL_ICON, icon,
                       COL_TEXT, str,
							  COL_TEXT2, str2,
                       -1);	
}

void add_to_list_err(gchar *str,gchar *str2)
{
    GtkTreeIter    iter;
    GdkPixbuf     *icon;

  icon = gdk_pixbuf_new_from_file("/usr/local/share/griffon/pixmaps/griffon_bug.png", NULL);
 
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter,
                       COL_ICON, icon,
                       COL_TEXT, str,
							  COL_TEXT2, str2,
                       -1);	
}

void clear_list_include (){gtk_list_store_clear(store);}

void on_changed_scan (GtkWidget *widget)
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  char *value;

  if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(widget), &model, &iter)) 
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

//******************************* Insert du chemin d'une image
void insert_image_web() 
{
	if (! get_page_text()) return;

	gchar *tampon_web;
	tampon_web = gtk_editable_get_chars(GTK_EDITABLE(entry_web),0, -1);

	gchar **a = g_strsplit (tampon_web, "://", -1);
	doc_insert_at_cursor (cur_text_doc, a[1]);

}

//**************************** liste vIEW  bookmark
GtkListStore * create_liststore_book()
{
    store_book = gtk_list_store_new(3, GDK_TYPE_PIXBUF, G_TYPE_STRING,G_TYPE_STRING);
    return store_book;
}

//******************************* Ajout dun bookMark
void add_to_list_book(gchar *str,gchar *str2)
{
    GtkTreeIter    iter;
    GdkPixbuf     *icon;

  icon = gdk_pixbuf_new_from_file("/usr/local/share/griffon/pixmaps/griffon_man.png", NULL);
 
    gtk_list_store_append(store_book, &iter);
    gtk_list_store_set(store_book, &iter,
                       COL_ICON, icon,
                       COL_TEXT, str,
							  COL_TEXT2, str2,
                       -1);	

	griffon_notify(_("Le bookmark est disponible dans l'onglet :\nMark"));

}

//******************************* change sur bokkmark
void on_changed_book (GtkWidget *widget)
{

  GtkTreeIter iter;
  GtkTreeModel *model_book;
  char *value;
  GtkTextMark *recup;
  GtkTextIter  iter2;

  if (! get_page_text()) return;

  if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(widget), &model_book, &iter)) 
	{
	   gtk_tree_model_get(model_book, &iter, COL_TEXT2, &value,  -1);
		
		if(gtk_text_buffer_get_mark(GTK_TEXT_BUFFER(cur_text_doc->text_buffer),value))
		{
		recup=gtk_text_buffer_get_mark(GTK_TEXT_BUFFER(cur_text_doc->text_buffer),value);
		gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(cur_text_doc->text_buffer),&iter2,recup);

	  gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &iter2);
	  gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW(cur_text_doc->text_view), &iter2, 0.0, FALSE, 0.0, 0.0);	
		}else{log_to_memo (_("Le BookMark ne correspond pas au fichier en cours d'édition."), NULL, LM_ERROR);statusbar_msg (_("BookMark File [ERROR]"));}
		gtk_tree_selection_unselect_all(GTK_TREE_SELECTION(widget));	
  }

}

//**************************** Detachable onglet pour notebook
GtkNotebook* window_creation_function (GtkNotebook *source_notebook,gint         x,gint         y)
{
  GtkWidget *window, *notebook;

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for(GTK_WINDOW(window),GTK_WINDOW(tea_main_window));
	gtk_window_set_deletable (GTK_WINDOW(window),FALSE);

  notebook = gtk_notebook_new ();
	gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_LEFT);

  gtk_notebook_set_group_name (GTK_NOTEBOOK (notebook),
           gtk_notebook_get_group_name (source_notebook));

 g_signal_connect (notebook, "create-window",
                    G_CALLBACK (window_creation_function), NULL);

  gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET(notebook));

  gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);
  gtk_window_move (GTK_WINDOW (window), x, y);

  gtk_notebook_set_scrollable (GTK_NOTEBOOK (notebook), TRUE);
  gtk_window_set_title (GTK_WINDOW (window), _((_("Tabs"))));
	gtk_window_set_icon_from_file (GTK_WINDOW(window),"/usr/local/share/griffon/pixmaps/griffon_ok.png",NULL);

  gtk_widget_show_all (GTK_WIDGET(window));

  return GTK_NOTEBOOK (notebook);
}

//******************************* Supprimer les bookmarks
void del_book() {gtk_list_store_clear(store_book);}

//**************************** Recherche avec Google
void google_search()
{   
	if (! get_page_text()) return;

	if(doc_get_sel (cur_text_doc))
	{
		char search_google[150];
		strcpy(search_google,"https://www.google.fr/#q=");
		strcat(search_google,doc_get_sel (cur_text_doc)); 
		webkit_web_view_load_uri(webView_myadmin_aide, search_google);

	//*************** NOTIFY TEST
	griffon_notify(_("Le résultat de la recherche est disponible dans l'onglet :\nMyAdmin->Aide/Recherche"));
	}
}

//**************************** Traduction fr vers en Google
void google_traduction_fr_en()
{   
	if (! get_page_text()) return;

	if(doc_get_sel (cur_text_doc))
	{
		char search_google[650];
		strcpy(search_google,"https://translate.google.fr/?hl=fr&tab=wT#fr/en/");
		strcat(search_google,doc_get_sel (cur_text_doc));
		webkit_web_view_load_uri(webView_myadmin_traduc, search_google);
		griffon_notify(_("Le résultat de la traduction est disponible dans l'onglet :\nMyAdmin->Traduction"));
	}
}

//**************************** Traduction en vers fr Google
void google_traduction_en_fr()
{   
	if (! get_page_text()) return;

	if(doc_get_sel (cur_text_doc))
	{
		char search_google[650];
		strcpy(search_google,"https://translate.google.fr/?hl=fr&tab=wT#en/fr/");
		strcat(search_google,doc_get_sel (cur_text_doc));
		webkit_web_view_load_uri(webView_myadmin_traduc, search_google);
		griffon_notify(_("Le résultat de la traduction est disponible dans l'onglet :\nMyAdmin->Traduction"));
	}
}

//******************************* chargement url miniweb
void enter_web ()
{
gchar *tampon_web;

	if(gtk_editable_get_chars(GTK_EDITABLE(entry_web),0, -1))
	{	
	tampon_web = gtk_editable_get_chars(GTK_EDITABLE(entry_web),0, -1);
	webkit_web_view_load_uri(webView, tampon_web);
	}
}

//******************************* chargement url myweb
void enter_myweb ()
{
gchar *tampon_myweb;

	if(gtk_editable_get_chars(GTK_EDITABLE(entry_myadmin),0, -1))
	{
	tampon_myweb = gtk_editable_get_chars(GTK_EDITABLE(entry_myadmin),0, -1);
	webkit_web_view_load_uri(webView_myadmin, tampon_myweb);
	}
}

//******************************* Toolbar Myadmin
void myadmin_reload (){webkit_web_view_reload(webView_myadmin);}
void myadmin_back (){webkit_web_view_go_back(webView_myadmin);}
void myadmin_forward (){webkit_web_view_go_forward(webView_myadmin);}
void myadmin_stop (){webkit_web_view_stop_loading(webView_myadmin);}

//******************************* Toolbar WIN_WEB
void myadmin_reload_win (gpointer user_data){webkit_web_view_reload(user_data);}
void myadmin_back_win (gpointer user_data){webkit_web_view_go_back(user_data);}
void myadmin_forward_win (gpointer user_data){webkit_web_view_go_forward(user_data);}
void myadmin_stop_win (gpointer user_data){webkit_web_view_stop_loading(user_data);}
void myadmin_source_mode_get_url_win (gpointer user_data){webkit_web_view_set_view_source_mode(user_data,TRUE);webkit_web_view_reload(user_data);}
void myadmin_view_mode_get_url_win (gpointer user_data){webkit_web_view_set_view_source_mode(user_data,FALSE);webkit_web_view_reload(user_data);}

void enter_myweb_win (GtkWidget* entry, gpointer user_data)
{
gchar *tampon_myweb;

	if(gtk_editable_get_chars(GTK_EDITABLE(entry),0, -1))
	{
	tampon_myweb = gtk_editable_get_chars(GTK_EDITABLE(entry),0, -1);
	webkit_web_view_load_uri(user_data, tampon_myweb);
	}
}

void myadmin_get_url_win (WebKitWebView  *web,gpointer user_data)
{
 gchar const *url=webkit_web_view_get_uri(web);
 gtk_entry_set_text (GTK_ENTRY (user_data), _(url));
}


//******************************* Toolbar MiniWeb
void miniweb_reload (){webkit_web_view_reload(webView);}
void miniweb_back (){webkit_web_view_go_back(webView);}
void miniweb_forward (){webkit_web_view_go_forward(webView);}
void miniweb_stop (){webkit_web_view_stop_loading(webView);}

//******************************* MyAdmin get url
void myadmin_get_url ()
{
 gchar const *url=webkit_web_view_get_uri(webView_myadmin);
 gtk_entry_set_text (GTK_ENTRY (entry_myadmin), _(url));

	GtkTreeIter iter_entry;
	gtk_list_store_append(model_entry_http2, &iter_entry);
	gtk_list_store_set(model_entry_http2, &iter_entry, CONTACT_NAME_HTTP2, gtk_entry_get_text (GTK_ENTRY (entry_myadmin)),  -1);
}

//******************************* Miniweb get url
void miniweb_get_url ()
{
 gchar const *url=webkit_web_view_get_uri(webView);
 gtk_entry_set_text (GTK_ENTRY (entry_web), _(url));

	GtkTreeIter iter_entry;
	gtk_list_store_append(model_entry_http, &iter_entry);
	gtk_list_store_set(model_entry_http, &iter_entry, CONTACT_NAME_HTTP, gtk_entry_get_text (GTK_ENTRY (entry_web)),  -1);

}

//******************************* Miniweb source mode
void miniweb_source_mode_get_url (){webkit_web_view_set_view_source_mode(webView,TRUE);webkit_web_view_reload(webView);}
//******************************* Miniweb view mode
void miniweb_view_mode_get_url (){webkit_web_view_set_view_source_mode(webView,FALSE);webkit_web_view_reload(webView);}
//******************************* Myadmin source mode
void myadmin_source_mode_get_url (){webkit_web_view_set_view_source_mode(webView_myadmin,TRUE);webkit_web_view_reload(webView_myadmin);}
//******************************* Myadmin view mode
void myadmin_view_mode_get_url (){webkit_web_view_set_view_source_mode(webView_myadmin,FALSE);webkit_web_view_reload(webView_myadmin);}



void copy_vte(  gpointer user_data)
{
      vte_terminal_copy_clipboard(user_data);
      gtk_widget_destroy(menu_vte);
}

void paste_vte(gpointer user_data)
{
      vte_terminal_paste_clipboard(user_data);
      gtk_widget_destroy(menu_vte);
}

//************** VTE Search google
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


gboolean popup_context_menu_vte(GtkWidget *tv, GdkEventButton *event)
{
      GtkWidget *menu_item,*menu_item2,*menu_item3,*menu_item4,*menu_item5;
      gboolean ret = FALSE;

      if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
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

            menu_item3 = gtk_menu_item_new_with_label (_("Sauvegarder dans un fichier texte"));
            g_signal_connect(menu_item3, "button-release-event", G_CALLBACK(new_file_term_edit), tv);
            gtk_menu_shell_append(GTK_MENU_SHELL(menu_vte), menu_item3);

            menu_item4 = gtk_menu_item_new_with_label (_("Recherche Google"));
            g_signal_connect(menu_item4, "button-release-event", G_CALLBACK(term_search_google), tv);
            gtk_menu_shell_append(GTK_MENU_SHELL(menu_vte), menu_item4);

            menu_item5 = gtk_menu_item_new_with_label (_("Help"));
            g_signal_connect(menu_item5, "button-release-event", G_CALLBACK(term_help), tv);
            gtk_menu_shell_append(GTK_MENU_SHELL(menu_vte), menu_item5);

            gtk_menu_popup(GTK_MENU(menu_vte), NULL, NULL, NULL, NULL, 3, 1);
									gtk_widget_show_all(GTK_WIDGET(menu_vte));

      }else{gtk_widget_grab_focus (tv);}

      return ret;
}


//******************************* New Terminale
void new_terminal ()
{
	term_page *page_term = (term_page *) g_malloc (sizeof (term_page));


  page_term->vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (GTK_WIDGET(page_term->vbox2));
  gtk_container_add (GTK_CONTAINER (notebook_term), GTK_WIDGET(page_term->vbox2));

  	page_term->vte_add = vte_terminal_new();

  vte_terminal_set_background_image_file (VTE_TERMINAL(page_term->vte_add),"/usr/local/share/griffon/pixmaps/griffon_bg2.png");
	vte_terminal_set_background_saturation (VTE_TERMINAL(page_term->vte_add),0.1);

  vte_terminal_set_scroll_on_keystroke(VTE_TERMINAL (page_term->vte_add), TRUE);
	 gtk_box_pack_start(GTK_BOX(page_term->vbox2), GTK_WIDGET(page_term->vte_add), TRUE, TRUE, 1);

	gtk_widget_show (GTK_WIDGET(page_term->vte_add));
 
		vte_terminal_set_scrollback_lines(VTE_TERMINAL(page_term->vte_add),-1);

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

//********************************* CLOSE ONE TERMINALE
void on_button_close_term ()
{
	gint num_tab=gtk_notebook_get_current_page(GTK_NOTEBOOK (notebook_term));
   gtk_notebook_remove_page(GTK_NOTEBOOK (notebook_term),num_tab);
}

//********************************* NEXT et PREV PAGE dans les onglets Terminal
void next_term (){gtk_notebook_next_page(GTK_NOTEBOOK (notebook_term));}
void prev_term (){gtk_notebook_prev_page(GTK_NOTEBOOK (notebook_term));}


//********************************* Bookmark myadmin ON/OFF
void myadmin_bookmark_off (){gtk_widget_hide(GTK_WIDGET(view_a));}

void myadmin_bookmark_on (){gtk_widget_show(GTK_WIDGET(view_a));}

//**************************** Window Debug Tool script
void window_debug ()
{
	if (! get_page_text()) return;
	gtk_widget_destroy (window_run);

  window_run = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_transient_for(GTK_WINDOW(window_run),GTK_WINDOW(tea_main_window));
	gtk_window_resize (GTK_WINDOW (window_run), 900, 500);
	
  GtkWidget * vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add (GTK_CONTAINER (window_run), GTK_WIDGET(vbox2));

	//**************************** TOOLBAR 
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

	vte_terminal_fork_command_full(VTE_TERMINAL(vte_add),
                                       VTE_PTY_DEFAULT,
                                       NULL,  
                                       startterm,
                                       NULL, 
                                       (GSpawnFlags)(G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_SEARCH_PATH),  
                                       NULL, 
                                       NULL, 
                                       NULL, 
                                       NULL);

   vte_terminal_set_background_image_file (VTE_TERMINAL(vte_add),"/usr/local/share/griffon/pixmaps/griffon_bg2.png");
	vte_terminal_set_background_saturation (VTE_TERMINAL(vte_add),0.1);

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

//**************************** Window Debug Tool script run
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

			   if (!cur_tea_project){return;}
				clear_debug();
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),"cd ",-1);
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),cur_tea_project->dir_makefile,-1);
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),"\nmake\n",-1);

				vte_terminal_feed_child (VTE_TERMINAL(vte_add),"cd ",-1);
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),cur_tea_project->dir_source,-1);	
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),"\n./",-1);		
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),cur_tea_project->file_executable,-1);
				vte_terminal_feed_child (VTE_TERMINAL(vte_add),"\n",-1);
			}

		}

statusbar_msg (_("Run Debug"));

}

//**************************** Window Debug Tool script clear
void clear_debug ()
{
	vte_terminal_reset (VTE_TERMINAL(vte_add),TRUE,TRUE);
}

//******************************* New terminale avec ssh
void new_terminal_ssh (gchar *serveur,gchar *user)
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

  vte_terminal_set_background_image_file (VTE_TERMINAL(page_term->vte_add),"/usr/local/share/griffon/pixmaps/griffon_bg2.png");
	vte_terminal_set_background_saturation (VTE_TERMINAL(page_term->vte_add),0.1);

  vte_terminal_set_scroll_on_keystroke(VTE_TERMINAL (page_term->vte_add), TRUE);
	gtk_container_add (GTK_CONTAINER (page_term->vbox2), GTK_WIDGET(page_term->vte_add));	


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

//******************************* Création de répertoire
void new_dir_cmd ()
{
	gchar* path_dir="";
	gchar* dir="";
	char pathT[500];
	pathT[0]='\0';

		if(! gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER(filechooserwidget2))){log_to_memo (_("Mkdir Error : vous devez séléctionner un répertoire dans le sélécteur de fichier."), NULL, LM_ERROR);statusbar_msg (_("Mkdir ERROR"));return;}

	path_dir=gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER(filechooserwidget2));
	strcpy(pathT,path_dir);
	strcat(pathT,"/");

	if(strlen (gtk_entry_get_text (GTK_ENTRY (ent_search2))) != 0)
	{
		 dir=gtk_editable_get_chars(GTK_EDITABLE(cmb_famous2),0, -1);
		strcat(pathT,dir);
		if (mkdir (pathT, S_IRUSR | S_IWUSR | S_IXUSR) == -1){log_to_memo (_("Mkdir Error."), NULL, LM_ERROR);statusbar_msg (_("Mkdir [ERROR]"));}
		else{log_to_memo (_("Mkdir %s"), pathT, LM_NORMAL); gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooserwidget2) ,pathT); statusbar_msg (_("Mkdir [OK]"));
	griffon_notify(_("Le répertoire est créé."));

		char commande[350];
	strcpy(commande,"chmod 755 ");
	strcat(commande,pathT);
	int systemRet =system(commande);
	if(systemRet == -1){return;}

return;}
	}
	else{log_to_memo (_("Mkdir Error : vous devez entrer le nom du répertoire dans la ligne de commande"), NULL, LM_ERROR);statusbar_msg (_("Mkdir [ERROR]"));}
}

//******************************* Création de fichier
void new_file_cmd ()
{
	gchar* path_file="";
	gchar* dir="";
	char pathT[500];
	pathT[0]='\0';

		if(! gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER(filechooserwidget2))){log_to_memo (_("File Error : vous devez séléctionner un répertoire dans le sélécteur de fichier."), NULL, LM_ERROR);statusbar_msg (_("File touch [ERROR]"));return;}

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
					griffon_notify(_("Le fichier est créé."));
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
	else{log_to_memo (_("File Error : vous devez entrer le nom du fichier dans la ligne de commande"), NULL, LM_ERROR);statusbar_msg (_("File touch [ERROR]"));}
}

//******************************* Création de fenêtre web
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
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(web_win->scrolledWindow),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(web_win->scrolledWindow), GTK_WIDGET(web_win->webView_w));

		gtk_box_pack_start(GTK_BOX(web_win->vbox3), GTK_WIDGET(web_win->scrolledWindow), TRUE, TRUE, 1);

		gtk_widget_show_all (GTK_WIDGET(web_win->window_web)); 

  g_signal_connect ((gpointer) web_win->entry_myadmin, "activate",
                    G_CALLBACK (enter_myweb_win),
                    web_win->webView_w);

	gchar *tampon_myweb;

	if(gtk_editable_get_chars(GTK_EDITABLE(entry_myadmin),0, -1))
	{
	tampon_myweb = gtk_editable_get_chars(GTK_EDITABLE(entry_myadmin),0, -1);
	webkit_web_view_load_uri(web_win->webView_w, tampon_myweb);
	gtk_entry_set_text (GTK_ENTRY (web_win->entry_myadmin), _(tampon_myweb));
	}

  	 g_signal_connect(web_win->webView_w, "load-finished",  
      G_CALLBACK(myadmin_get_url_win), web_win->entry_myadmin);

  	 g_signal_connect(web_win->webView_w, "new-window-policy-decision-requested",  
      G_CALLBACK(myadmin_new_window), web_win->webView_w);

  	 g_signal_connect(web_win->webView_w, "create-web-view",  
    G_CALLBACK(web_new_w_click_go), web_win->webView_w);

}

//******************************* Création de fenêtre web
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
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(web_win->scrolledWindow),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(web_win->scrolledWindow), GTK_WIDGET(web_win->webView_w));

		gtk_box_pack_start(GTK_BOX(web_win->vbox3), GTK_WIDGET(web_win->scrolledWindow), TRUE, TRUE, 1);
		gtk_widget_show_all (GTK_WIDGET(web_win->window_web)); 

  g_signal_connect ((gpointer) web_win->entry_myadmin, "activate",
                    G_CALLBACK (enter_myweb_win),
                    web_win->webView_w);

	gchar *tampon_myweb;

	if(gtk_editable_get_chars(GTK_EDITABLE(entry_web),0, -1))
	{
	tampon_myweb = gtk_editable_get_chars(GTK_EDITABLE(entry_web),0, -1);
	webkit_web_view_load_uri(web_win->webView_w, tampon_myweb);
	gtk_entry_set_text (GTK_ENTRY (web_win->entry_myadmin), _(tampon_myweb));
	}

  	 g_signal_connect(web_win->webView_w, "load-finished",  
      G_CALLBACK(myadmin_get_url_win), web_win->entry_myadmin);

  	 g_signal_connect(web_win->webView_w, "new-window-policy-decision-requested",  
      G_CALLBACK(myadmin_new_window), web_win->webView_w);

  	 g_signal_connect(web_win->webView_w, "create-web-view",  
    G_CALLBACK(web_new_w_click_go), web_win->webView_w);

}

//******************************* Création d'un fichier avec les logs en édition
void new_file_log_edit ()
{
	file_new();
	gchar *buf = doc_get_buf (GTK_TEXT_BUFFER(log_memo_textbuffer));
	doc_insert_at_cursor (cur_text_doc,buf);
}

//******************************* Création d'un fichier avec les logs term en édition
void new_file_term_edit (  gpointer user_data)
{
	file_new();
	gchar *buf = vte_terminal_get_text(user_data,NULL,NULL,NULL);
	doc_insert_at_cursor (cur_text_doc,buf);
}

//******************************* Fonction de notification
void griffon_notify (gchar* txt)
{
		NotifyNotification * notification=notify_notification_new("Griffon IDE",txt,"/usr/local/share/griffon/pixmaps/griffon_button.png");
		notify_notification_show (notification,NULL);
}

//******************************** DL WEBKIT
void download_status_cb(GObject* object){ 
    WebKitDownload *download;
    WebKitDownloadStatus status;

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
void download_requested_cb(WebKitDownload *download)
{
	 gchar const *filename="";
	 gchar* path_dir="";

	 gchar *uri1="file://";

	if(! gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER(filechooserwidget2))){log_to_memo (_("Download Error : vous devez séléctionner un répertoire dans le sélécteur de fichier"), NULL, LM_ERROR);statusbar_msg (_("Download ERROR"));return ;}

	filename = webkit_download_get_suggested_filename(download);
	path_dir=gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER(filechooserwidget2));
	const gchar * dest_uri = g_strdup_printf("%s%s/%s", uri1, path_dir, filename);
	 gchar * path = g_strdup_printf("%s/%s", path_dir, filename);

			if (g_file_test (path, G_FILE_TEST_EXISTS)){log_to_memo (_("File Error.download exists"), NULL, LM_ERROR);statusbar_msg (_("File download exists [ERROR]"));return;}
		else{
						webkit_download_set_destination_uri(download,dest_uri);

    g_signal_connect(download, "notify::status",
                     G_CALLBACK(download_status_cb), NULL);
					}

  return ; 
}

void switch_filechooser ()
{
	if (! get_page_text()) return;

	 if(cur_text_doc->file_name!=NULL)
		{
			gchar **a = g_strsplit (cur_text_doc->file_name, "_", -1);
		if (strcmp("noname", a[0]) != 0 ){gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooserwidget2) ,g_path_get_dirname (cur_text_doc->file_name));}
	}
}
