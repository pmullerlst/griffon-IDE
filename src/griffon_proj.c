
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

#include <glib.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <string.h>

#include "interface.h"
#include "callbacks.h"
#include "griffon_defs.h"
#include "griffon_gtk_utils.h"
#include "griffon_proj.h"
#include "rox_strings.h"
#include "griffon_config.h"
#include "griffon_text_document.h"

static GtkWidget *ent_project_name;
static GtkWidget *ent_dir_makefile;
static GtkWidget *ent_dir_source;
static GtkWidget *ent_file_executable;

GtkWidget *entry_proj_path,*entry_proj_command,*entry_proj_make_path,*entry_proj_name,*entry_proj_run,*entry_proj_info,*entry_proj_icon,*entry_proj_url;
GtkWidget *entry_proj_sftp_ip,*entry_proj_sftp_user,*entry_proj_sftp_path,*entry_proj_sftp_port;
GtkWidget *entry_proj_ftp_ip,*entry_proj_ftp_user,*entry_proj_ftp_pass;
GtkFileFilter* filefilter_img;

static GtkWidget* mk_page_main (t_tea_project *p)
{
  GtkWidget *page = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

  gtk_widget_show (page);
  ent_project_name = tea_text_entry (GTK_WIDGET(page), _("Project name"), p->project_name);
  ent_dir_makefile = tea_dir_selector (page, _("Makefile directory"), p->dir_makefile);
  ent_dir_source = tea_dir_selector (page, _("Source directory"), p->dir_source);
  ent_file_executable = tea_text_entry (page, _("Target executable"), p->file_executable);
  
gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet), "\n\n", -1);
gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet), "\nfile_executable=", -1);
gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet), p->file_executable, -1);
gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet), "\nproject_name=", -1);
gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet), p->project_name, -1);
gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet), "\ndir_makefile=", -1);
gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet), p->dir_makefile, -1);
gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet), "\ndir_source=", -1);
gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet), p->dir_source, -1);

  return page;
}


t_tea_project* tea_proj_new (void)
{
  t_tea_project *p = g_malloc (sizeof (t_tea_project)); 

  p->project_name = g_strdup ("noname");
  p->project_file_name = NULL;
  p->dir_makefile = NULL;
  p->dir_source = NULL;
  p->file_executable = NULL;

  return p;
} 


t_tea_project* tea_proj_clear_new (t_tea_project *p)
{
  tea_proj_free (p);
  return tea_proj_new ();
} 


t_tea_project* tea_proj_open (gchar *name)
{
  t_tea_project *r = tea_proj_new ();
  GHashTable *ht = load_file_to_hashtable (name);

  r->project_name = ht_get_gchar_value (ht, "project_name", "noname");
  r->project_file_name = ch_str (r->project_file_name, name);
  r->dir_makefile = ht_get_gchar_value (ht, "dir_makefile", "noname");
  r->dir_source = ht_get_gchar_value (ht, "dir_source", "noname");
  r->file_executable = ht_get_gchar_value (ht, "file_executable", "noname");

	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooserwidget2) ,r->dir_source);

  g_hash_table_destroy (ht); 
  return r;
}


void tea_proj_save (t_tea_project *p)
{
  if (! p)
     return;

  if (! p->project_file_name)
     return; 

  GList *l = NULL;
  
  l = add_to_glist_combined_str (l, "file_executable", p->file_executable);
  l = add_to_glist_combined_str (l, "project_name", p->project_name);
  l = add_to_glist_combined_str (l, "project_file_name", p->project_file_name);
  l = add_to_glist_combined_str (l, "dir_makefile", p->dir_makefile);
  l = add_to_glist_combined_str (l, "dir_source", p->dir_source);

  glist_save_to_file (l, p->project_file_name);
  g_list_free (l); 
}


void tea_proj_save_as (t_tea_project *p, gchar *name)
{
  if (! p)
     return;

  if (! name)
     return;

  p->project_file_name = ch_str (p->project_file_name, name);

  GList *l = NULL;

gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet), "\nProeject file name :\n", -1);
gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet), p->project_file_name, -1);
gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet), "\n\n", -1);
gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet), "\nfile_executable=", -1);
gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet), p->file_executable, -1);
gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet), "\nproject_name=", -1);
gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet), p->project_name, -1);
gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet), "\ndir_makefile=", -1);
gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet), p->dir_makefile, -1);
gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet), "\ndir_source=", -1);
gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet), p->dir_source, -1);
  
  l = add_to_glist_combined_str (l, "file_executable", p->file_executable);
  l = add_to_glist_combined_str (l, "project_name", p->project_name);
  l = add_to_glist_combined_str (l, "project_file_name", p->project_file_name);
  l = add_to_glist_combined_str (l, "dir_makefile", p->dir_makefile);
  l = add_to_glist_combined_str (l, "dir_source", p->dir_source);

  glist_save_to_file (l, p->project_file_name);
  g_list_free (l); 
}


void tea_proj_free (t_tea_project *p)
{
  if (! p)
     return;

  g_free (p->project_name);
  g_free (p->project_file_name);
  g_free (p->dir_makefile);
  g_free (p->dir_source);
  g_free (p->file_executable);
  g_free (p);
}

void tea_proj_run (t_tea_project *p)
{
  if (! p)
     return;

  if (! p->file_executable)
     return; 

  gchar *f = g_strconcat (p->dir_source, p->file_executable, NULL);
  g_spawn_command_line_async (f, NULL);
  g_free (f);
}


GtkWidget* create_proj_props_window (t_tea_project *p)
{
  if (! p)
    return NULL; 

  wnd_proj_props = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_window_set_title (GTK_WINDOW (wnd_proj_props), _("Project properties"));

  GtkWidget *vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (wnd_proj_props), vbox1);

  GtkWidget *notebook = gtk_notebook_new ();         
  gtk_notebook_set_tab_pos (GTK_NOTEBOOK(notebook), GTK_POS_LEFT);

  gtk_widget_show (notebook);

  gtk_container_add (GTK_CONTAINER (vbox1), notebook);

  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), mk_page_main (p), gtk_label_new (_("Main")));
  
  GtkWidget *hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (hbox1);
  gtk_container_add (GTK_CONTAINER (vbox1), hbox1);

  GtkWidget *bt_close = gtk_button_new_with_label("_Close");
  gtk_widget_show (bt_close);
  gtk_container_add (GTK_CONTAINER (hbox1), bt_close);
  g_signal_connect (bt_close, "clicked", G_CALLBACK (cb_on_bt_close), wnd_proj_props);

  t_ppair *pair = g_malloc (sizeof (t_ppair)); 
  pair->a = wnd_proj_props;
  pair->b = p;

  GtkWidget *bt_apply = gtk_button_new_with_label("_OK");
  gtk_widget_show (bt_apply);
  gtk_container_add (GTK_CONTAINER (hbox1), bt_apply);

  gtk_window_set_position (GTK_WINDOW(wnd_proj_props), GTK_WIN_POS_CENTER);   
  gtk_window_set_modal (GTK_WINDOW(wnd_proj_props), TRUE);

  gtk_widget_show (wnd_proj_props);
  return wnd_proj_props;
}

//*********************** NEW PROJECT
GtkWidget* new_project_window (void)
{
	GtkWidget *window1,*frame1,*vbox1,*hbox2,*label2,*hbox1,*button1,*button2,*label1,*hbox3,*label3,*hbox4,*label4,*hbox5,*label5,*hbox6,*label6,*hbox7,*label7,*hbox8,*label8,*hbox9,*label9;  
	GtkWidget *button_icon,*button_command,*button_make_path,*button_path,*button_run;
	icon_affiche_stop();
	
	window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window1), _((_("New Project"))));
	gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);
	gtk_window_set_transient_for(GTK_WINDOW(window1),GTK_WINDOW(tea_main_window));
  gtk_widget_show(GTK_WIDGET(window1));

	frame1 = gtk_frame_new (NULL);
	gtk_widget_show (GTK_WIDGET(frame1));
	gtk_container_add (GTK_CONTAINER (window1), frame1);
	gtk_container_set_border_width (GTK_CONTAINER (frame1), 4);
	gtk_frame_set_label_align (GTK_FRAME (frame1), 0.02, 0.55);

	vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_show (GTK_WIDGET(vbox1));
	gtk_container_add (GTK_CONTAINER (frame1), vbox1);

	hbox2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox2));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox2, TRUE, TRUE, 0);

	label2 = gtk_label_new (_("Project name : \t\t"));
	gtk_widget_show (GTK_WIDGET(label2));
	gtk_box_pack_start (GTK_BOX (hbox2), label2, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);

	entry_proj_name = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_name));
	gtk_box_pack_start (GTK_BOX (hbox2), entry_proj_name, FALSE, FALSE, 0);
	gtk_widget_grab_focus(GTK_WIDGET(entry_proj_name));

	hbox3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox3));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox3, TRUE, TRUE, 0);

	label3 = gtk_label_new (_("Path : \t\t\t"));
	gtk_widget_show (GTK_WIDGET(label3));
	gtk_box_pack_start (GTK_BOX (hbox3), label3, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);

	entry_proj_path = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_path));
	gtk_box_pack_start (GTK_BOX (hbox3), entry_proj_path, FALSE, FALSE, 0);

	button_path = gtk_button_new_with_label ("_Open");
	gtk_widget_show (GTK_WIDGET(button_path));
	gtk_box_pack_start (GTK_BOX (hbox3), button_path, TRUE, TRUE, 0);

	hbox4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox4));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox4, TRUE, TRUE, 0);

	label4 = gtk_label_new (_("Make path : \t\t"));
	gtk_widget_show (GTK_WIDGET(label4));
	gtk_box_pack_start (GTK_BOX (hbox4), label4, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_LEFT);

	entry_proj_make_path = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_make_path));
	gtk_box_pack_start (GTK_BOX (hbox4), entry_proj_make_path, FALSE, FALSE, 0);

	button_make_path = gtk_button_new_with_label ("_Open");
	gtk_widget_show (GTK_WIDGET(button_make_path));
	gtk_box_pack_start (GTK_BOX (hbox4), button_make_path, TRUE, TRUE, 0);

	//**************
	hbox5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox5));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox5, TRUE, TRUE, 0);

	label5 = gtk_label_new (_("Make command : \t"));
	gtk_widget_show (GTK_WIDGET(label5));
	gtk_box_pack_start (GTK_BOX (hbox5), label5, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label5), GTK_JUSTIFY_LEFT);

	entry_proj_command = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_command));
	gtk_box_pack_start (GTK_BOX (hbox5), entry_proj_command, FALSE, FALSE, 0);

	button_command = gtk_button_new_with_label ("_Open");
	gtk_widget_show (GTK_WIDGET(button_command));
	gtk_box_pack_start (GTK_BOX (hbox5), button_command, TRUE, TRUE, 0);

	//**************
	hbox6 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox6));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox6, TRUE, TRUE, 0);

	label6 = gtk_label_new (_("Run target : \t\t"));
	gtk_widget_show (GTK_WIDGET(label6));
	gtk_box_pack_start (GTK_BOX (hbox6), label6, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label6), GTK_JUSTIFY_LEFT);

	entry_proj_run = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_run));
	gtk_box_pack_start (GTK_BOX (hbox6), entry_proj_run, FALSE, FALSE, 0);

	button_run = gtk_button_new_with_label ("_Open");
	gtk_widget_show (GTK_WIDGET(button_run));
	gtk_box_pack_start (GTK_BOX (hbox6), button_run, TRUE, TRUE, 0);

	//**************
	hbox7 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox7));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox7, TRUE, TRUE, 0);

	label7 = gtk_label_new (_("Info/Note : \t\t"));
	gtk_widget_show (GTK_WIDGET(label7));
	gtk_box_pack_start (GTK_BOX (hbox7), label7, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label7), GTK_JUSTIFY_LEFT);

	entry_proj_info = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_info));
	gtk_box_pack_start (GTK_BOX (hbox7), entry_proj_info, FALSE, FALSE, 0);
	gtk_entry_set_width_chars (GTK_ENTRY(entry_proj_info),50);
	//**************
	hbox8 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox8));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox8, TRUE, TRUE, 0);

	label8 = gtk_label_new (_("Icon/IMG : \t\t"));
	gtk_widget_show (GTK_WIDGET(label8));
	gtk_box_pack_start (GTK_BOX (hbox8), label8, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label8), GTK_JUSTIFY_LEFT);

	entry_proj_icon = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_icon));
	gtk_box_pack_start (GTK_BOX (hbox8), entry_proj_icon, FALSE, FALSE, 0);

	button_icon = gtk_button_new_with_label ("_Open");
	gtk_widget_show (GTK_WIDGET(button_icon));
	gtk_box_pack_start (GTK_BOX (hbox8), button_icon, TRUE, TRUE, 0);

	//**************
	hbox9 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox9));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox9, TRUE, TRUE, 0);

	label9 = gtk_label_new (_("URL/HTTP : \t\t"));
	gtk_widget_show (GTK_WIDGET(label9));
	gtk_box_pack_start (GTK_BOX (hbox9), label9, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);

	entry_proj_url = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_url));
	gtk_box_pack_start (GTK_BOX (hbox9), entry_proj_url, FALSE, FALSE, 0);
	gtk_entry_set_text (GTK_ENTRY (entry_proj_url), _("http://"));

	//**************
	hbox9 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox9));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox9, TRUE, TRUE, 0);

	label9 = gtk_label_new (_("SFTP IP : \t\t\t"));
	gtk_widget_show (GTK_WIDGET(label9));
	gtk_box_pack_start (GTK_BOX (hbox9), label9, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);

	entry_proj_sftp_ip = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_sftp_ip));
	gtk_box_pack_start (GTK_BOX (hbox9), entry_proj_sftp_ip, FALSE, FALSE, 0);

	//**************
	hbox9 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox9));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox9, TRUE, TRUE, 0);

	label9 = gtk_label_new (_("SFTP USER : \t\t"));
	gtk_widget_show (GTK_WIDGET(label9));
	gtk_box_pack_start (GTK_BOX (hbox9), label9, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);

	entry_proj_sftp_user = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_sftp_user));
	gtk_box_pack_start (GTK_BOX (hbox9), entry_proj_sftp_user, FALSE, FALSE, 0);
	gtk_entry_set_text (GTK_ENTRY (entry_proj_sftp_user), _("root"));

	//**************
	hbox9 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox9));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox9, TRUE, TRUE, 0);

	label9 = gtk_label_new (_("SFTP PATH : \t\t"));
	gtk_widget_show (GTK_WIDGET(label9));
	gtk_box_pack_start (GTK_BOX (hbox9), label9, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);

	entry_proj_sftp_path = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_sftp_path));
	gtk_box_pack_start (GTK_BOX (hbox9), entry_proj_sftp_path, FALSE, FALSE, 0);
	gtk_entry_set_text (GTK_ENTRY (entry_proj_sftp_path), _("/"));

	//**************
	hbox9 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox9));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox9, TRUE, TRUE, 0);

	label9 = gtk_label_new (_("SFTP PORT : \t\t"));
	gtk_widget_show (GTK_WIDGET(label9));
	gtk_box_pack_start (GTK_BOX (hbox9), label9, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);

	entry_proj_sftp_port = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_sftp_port));
	gtk_box_pack_start (GTK_BOX (hbox9), entry_proj_sftp_port, FALSE, FALSE, 0);
	gtk_entry_set_text (GTK_ENTRY (entry_proj_sftp_port), _("22"));

	//**************
	hbox9 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox9));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox9, TRUE, TRUE, 0);

	label9 = gtk_label_new (_("FTP IP : \t\t\t"));
	gtk_widget_show (GTK_WIDGET(label9));
	gtk_box_pack_start (GTK_BOX (hbox9), label9, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);

	entry_proj_ftp_ip = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_ftp_ip));
	gtk_box_pack_start (GTK_BOX (hbox9), entry_proj_ftp_ip, FALSE, FALSE, 0);

	//**************
	hbox9 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox9));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox9, TRUE, TRUE, 0);

	label9 = gtk_label_new (_("FTP USER : \t\t"));
	gtk_widget_show (GTK_WIDGET(label9));
	gtk_box_pack_start (GTK_BOX (hbox9), label9, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);

	entry_proj_ftp_user = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_ftp_user));
	gtk_box_pack_start (GTK_BOX (hbox9), entry_proj_ftp_user, FALSE, FALSE, 0);

	//**************
	hbox9 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox9));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox9, TRUE, TRUE, 0);

	label9 = gtk_label_new (_("FTP PASSWORD : \t"));
	gtk_widget_show (GTK_WIDGET(label9));
	gtk_box_pack_start (GTK_BOX (hbox9), label9, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);

	entry_proj_ftp_pass = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_ftp_pass));
	gtk_box_pack_start (GTK_BOX (hbox9), entry_proj_ftp_pass, FALSE, FALSE, 0);



	hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox1));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox1, FALSE, FALSE, 0);

	button1 = gtk_button_new_with_label ("_Cancel");
	gtk_widget_show (GTK_WIDGET(button1));
	gtk_box_pack_start (GTK_BOX (hbox1), button1, TRUE, TRUE, 0);
	gtk_button_set_relief (GTK_BUTTON (button1), GTK_RELIEF_NONE);


	button2 = gtk_button_new_with_label ("_Apply");
	gtk_widget_show (GTK_WIDGET(button2));
	gtk_box_pack_start (GTK_BOX (hbox1), button2, TRUE, TRUE, 0);
	gtk_button_set_relief (GTK_BUTTON (button2), GTK_RELIEF_NONE);

	label1 = gtk_label_new (_("New Project"));
	gtk_widget_show (GTK_WIDGET(label1));
	gtk_frame_set_label_widget (GTK_FRAME (frame1), label1);
	gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

	g_signal_connect_swapped ((gpointer) button_icon, "clicked",G_CALLBACK (open_dialog_path_icon),entry_proj_icon);
	g_signal_connect_swapped ((gpointer) button_command, "clicked",G_CALLBACK (open_dialog_path),entry_proj_command);
	g_signal_connect_swapped ((gpointer) button_make_path, "clicked",G_CALLBACK (open_dialog_path_dir),entry_proj_make_path);
	g_signal_connect_swapped ((gpointer) button_path, "clicked",G_CALLBACK (open_dialog_path_dir),entry_proj_path);
	g_signal_connect_swapped ((gpointer) button_run, "clicked",G_CALLBACK (open_dialog_path),entry_proj_run);
	g_signal_connect_swapped ((gpointer) button1, "clicked",G_CALLBACK (icon_affiche_ok),NULL);	
	g_signal_connect_swapped ((gpointer) button1, "clicked",G_CALLBACK (gtk_widget_destroy), window1);
	g_signal_connect_swapped ((gpointer) button2, "clicked",G_CALLBACK (icon_affiche_ok), NULL);	
	g_signal_connect_swapped ((gpointer) button2, "clicked",G_CALLBACK (save_projects),NULL);	
	g_signal_connect_swapped ((gpointer) button2, "clicked",G_CALLBACK (gtk_widget_destroy), window1);
	g_signal_connect_swapped ((gpointer) button1, "clicked",G_CALLBACK (gtk_widget_destroy), window1);

	return window1;
}

//*********************** SAVE PROJECTS
void save_projects (void)
{
	gchar *tmp_proj_command;
	gchar *tmp_proj_make_path;
	gchar *tmp_proj_name;
	gchar *tmp_proj_path;
	gchar *tmp_proj_run;
	gchar *tmp_proj_info;
	gchar *tmp_proj_icon;
	gchar *tmp_proj_url;
	gchar *tmp_proj_sftp_ip;
	gchar *tmp_proj_sftp_user;
	gchar *tmp_proj_sftp_path;
	gchar *tmp_proj_sftp_port;
	gchar *tmp_proj_ftp_ip;
	gchar *tmp_proj_ftp_user;
	gchar *tmp_proj_ftp_pass;

	tmp_proj_command = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_command),0, -1);
	tmp_proj_make_path = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_make_path),0, -1);
	tmp_proj_name = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_name),0, -1);
	tmp_proj_path = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_path),0, -1);
	tmp_proj_run = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_run),0, -1);
	tmp_proj_info = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_info),0, -1);
	tmp_proj_icon = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_icon),0, -1);
	tmp_proj_url = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_url),0, -1);
	tmp_proj_sftp_ip = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_sftp_ip),0, -1);
	tmp_proj_sftp_user = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_sftp_user),0, -1);
	tmp_proj_sftp_path = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_sftp_path),0, -1);
	tmp_proj_sftp_port = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_sftp_port),0, -1);
	tmp_proj_ftp_ip = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_ftp_ip),0, -1);
	tmp_proj_ftp_user = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_ftp_user),0, -1);
	tmp_proj_ftp_pass = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_ftp_pass),0, -1);


	save_string_to_file_add(confile.projects,tmp_proj_name);
	save_string_to_file_add(confile.projects,";");
	save_string_to_file_add(confile.projects,tmp_proj_path);
	save_string_to_file_add(confile.projects,";");
	save_string_to_file_add(confile.projects,tmp_proj_make_path);
	save_string_to_file_add(confile.projects,";");
	save_string_to_file_add(confile.projects,tmp_proj_run);
	save_string_to_file_add(confile.projects,";");
	save_string_to_file_add(confile.projects,tmp_proj_command);
	save_string_to_file_add(confile.projects,";");
	save_string_to_file_add(confile.projects,tmp_proj_info);
	save_string_to_file_add(confile.projects,";");
	save_string_to_file_add(confile.projects,tmp_proj_icon);
	save_string_to_file_add(confile.projects,";");
	save_string_to_file_add(confile.projects,tmp_proj_url);
	save_string_to_file_add(confile.projects,";");
	save_string_to_file_add(confile.projects,tmp_proj_sftp_ip);
	save_string_to_file_add(confile.projects,";");
	save_string_to_file_add(confile.projects,tmp_proj_sftp_user);
	save_string_to_file_add(confile.projects,":");
	save_string_to_file_add(confile.projects,tmp_proj_sftp_path);
	save_string_to_file_add(confile.projects,";");
	save_string_to_file_add(confile.projects,tmp_proj_ftp_ip);
	save_string_to_file_add(confile.projects,";");
	save_string_to_file_add(confile.projects,tmp_proj_ftp_user);
	save_string_to_file_add(confile.projects,";");
	save_string_to_file_add(confile.projects,tmp_proj_ftp_pass);
	save_string_to_file_add(confile.projects,";");
	save_string_to_file_add(confile.projects,tmp_proj_sftp_port);
	save_string_to_file_add(confile.projects,";;;;;;;;;;;;;;;;;;;\n");

	load_projects_list();
}

//********************* OPEN DIALOG FILE PATH
void open_dialog_path(gpointer data)
{
	GtkWidget *dialog;
	dialog = gtk_file_chooser_dialog_new ("Open File",
				      (GtkWindow *)tea_main_window,
				      GTK_FILE_CHOOSER_ACTION_OPEN,
				      "_Cancel", GTK_RESPONSE_CANCEL,
				      "_Open", GTK_RESPONSE_ACCEPT,
				      NULL);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		gtk_entry_set_text (GTK_ENTRY (data), _(filename));
		g_free (filename);
	}
	gtk_widget_destroy (dialog);
}

void update_preview_cb (GtkFileChooser *file_chooser, gpointer data)
{
	GtkWidget *preview;
	char *filename;
	GdkPixbuf *pixbuf;
	gboolean have_preview;

	preview = GTK_WIDGET (data);
	filename = gtk_file_chooser_get_preview_filename (file_chooser);

	pixbuf = gdk_pixbuf_new_from_file_at_size (filename, 128, 128, NULL);
	have_preview = (pixbuf != NULL);
	g_free (filename);

	gtk_image_set_from_pixbuf (GTK_IMAGE (preview), pixbuf);
	if (pixbuf)
		g_object_unref (pixbuf);

	gtk_file_chooser_set_preview_widget_active (file_chooser, have_preview);
}

//********************* OPEN DIALOG FILE PATH
void open_dialog_path_icon(gpointer data)
{
	GtkWidget *dialog;
	dialog = gtk_file_chooser_dialog_new ("Open File",
				      (GtkWindow *)tea_main_window,
				      GTK_FILE_CHOOSER_ACTION_OPEN,
				      "_Cancel", GTK_RESPONSE_CANCEL,
				      "_Open", GTK_RESPONSE_ACCEPT,
				      NULL);

	gtk_file_chooser_set_current_folder((GtkFileChooser *)dialog,"/usr/local/share/griffon/images/projects/");

	filefilter_img = NULL;
 
	filefilter_img = gtk_file_filter_new();
	gtk_file_filter_add_mime_type (GTK_FILE_FILTER(filefilter_img), "image/png");
	gtk_file_filter_set_name(filefilter_img,"Filter PNG");
	gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), GTK_FILE_FILTER(filefilter_img));

	GtkWidget *preview;
	preview = gtk_image_new ();

	gtk_file_chooser_set_preview_widget ((GtkFileChooser *)dialog, preview);
	g_signal_connect ((GtkFileChooser *)dialog, "update-preview",G_CALLBACK (update_preview_cb), preview);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		gtk_entry_set_text (GTK_ENTRY (data), _(filename));
		g_free (filename);
	}
	gtk_widget_destroy (dialog);
}


//********************* OPEN DIALOG FILE PATH DIR
void open_dialog_path_dir(gpointer data)
{
	GtkWidget *dialog;
	dialog = gtk_file_chooser_dialog_new ("Open File",
				      (GtkWindow *)tea_main_window,
				       GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
				      "_Cancel", GTK_RESPONSE_CANCEL,
				      "_Open", GTK_RESPONSE_ACCEPT,
				      NULL);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		gtk_entry_set_text (GTK_ENTRY (data), _(filename));
		g_free (filename);
	}
	gtk_widget_destroy (dialog);
}

//*********************** UPDATE INTERFACE PROJECT
void update_project_window (gpointer data)
{

	gchar **a=NULL;
	gchar **b=NULL;

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
				if(ligne==(intptr_t)data)
				{
					a = g_strsplit (mot, ";", -1);
					b = g_strsplit (a[9], ":", -1);
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

		if(strlen(a[13])<1){a[13]="22";}

	GtkWidget *window1,*frame1,*vbox1,*hbox2,*label2,*hbox1,*button1,*button2,*label1,*hbox3,*label3,*hbox4,*label4,*hbox5,*label5,*hbox6,*label6,*hbox7,*label7,*hbox8,*label8,*hbox9,*label9;  
	GtkWidget *button_icon,*button_command,*button_make_path,*button_path,*button_run;
	icon_affiche_stop();
	
	window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window1), _((_("Update Project"))));
	gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);
	gtk_window_set_transient_for(GTK_WINDOW(window1),GTK_WINDOW(tea_main_window));
  gtk_widget_show(GTK_WIDGET(window1));

	frame1 = gtk_frame_new (NULL);
	gtk_widget_show (GTK_WIDGET(frame1));
	gtk_container_add (GTK_CONTAINER (window1), frame1);
	gtk_container_set_border_width (GTK_CONTAINER (frame1), 4);
	gtk_frame_set_label_align (GTK_FRAME (frame1), 0.02, 0.55);

	vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_show (GTK_WIDGET(vbox1));
	gtk_container_add (GTK_CONTAINER (frame1), vbox1);

	hbox2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox2));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox2, TRUE, TRUE, 0);

	label2 = gtk_label_new (_("Project name : \t\t"));
	gtk_widget_show (GTK_WIDGET(label2));
	gtk_box_pack_start (GTK_BOX (hbox2), label2, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);

	entry_proj_name = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_name));
	gtk_box_pack_start (GTK_BOX (hbox2), entry_proj_name, FALSE, FALSE, 0);
	gtk_widget_grab_focus(GTK_WIDGET(entry_proj_name));
	gtk_entry_set_text (GTK_ENTRY (entry_proj_name), _(a[0]));

	hbox3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox3));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox3, TRUE, TRUE, 0);

	label3 = gtk_label_new (_("Path : \t\t\t"));
	gtk_widget_show (GTK_WIDGET(label3));
	gtk_box_pack_start (GTK_BOX (hbox3), label3, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);

	entry_proj_path = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_path));
	gtk_box_pack_start (GTK_BOX (hbox3), entry_proj_path, FALSE, FALSE, 0);
	gtk_entry_set_text (GTK_ENTRY (entry_proj_path), _(a[1]));

	button_path = gtk_button_new_with_label ("_Open");
	gtk_widget_show (GTK_WIDGET(button_path));
	gtk_box_pack_start (GTK_BOX (hbox3), button_path, TRUE, TRUE, 0);

	hbox4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox4));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox4, TRUE, TRUE, 0);

	label4 = gtk_label_new (_("Make path : \t\t"));
	gtk_widget_show (GTK_WIDGET(label4));
	gtk_box_pack_start (GTK_BOX (hbox4), label4, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_LEFT);

	entry_proj_make_path = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_make_path));
	gtk_box_pack_start (GTK_BOX (hbox4), entry_proj_make_path, FALSE, FALSE, 0);
	gtk_entry_set_text (GTK_ENTRY (entry_proj_make_path), _(a[2]));

	button_make_path = gtk_button_new_with_label ("_Open");
	gtk_widget_show (GTK_WIDGET(button_make_path));
	gtk_box_pack_start (GTK_BOX (hbox4), button_make_path, TRUE, TRUE, 0);

	//**************
	hbox5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox5));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox5, TRUE, TRUE, 0);

	label5 = gtk_label_new (_("Make command : \t"));
	gtk_widget_show (GTK_WIDGET(label5));
	gtk_box_pack_start (GTK_BOX (hbox5), label5, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label5), GTK_JUSTIFY_LEFT);

	entry_proj_command = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_command));
	gtk_box_pack_start (GTK_BOX (hbox5), entry_proj_command, FALSE, FALSE, 0);
	gtk_entry_set_text (GTK_ENTRY (entry_proj_command), _(a[4]));

	button_command = gtk_button_new_with_label ("_Open");
	gtk_widget_show (GTK_WIDGET(button_command));
	gtk_box_pack_start (GTK_BOX (hbox5), button_command, TRUE, TRUE, 0);

	//**************
	hbox6 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox6));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox6, TRUE, TRUE, 0);

	label6 = gtk_label_new (_("Run target : \t\t"));
	gtk_widget_show (GTK_WIDGET(label6));
	gtk_box_pack_start (GTK_BOX (hbox6), label6, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label6), GTK_JUSTIFY_LEFT);

	entry_proj_run = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_run));
	gtk_box_pack_start (GTK_BOX (hbox6), entry_proj_run, FALSE, FALSE, 0);
	gtk_entry_set_text (GTK_ENTRY (entry_proj_run), _(a[3]));

	button_run = gtk_button_new_with_label ("_Open");
	gtk_widget_show (GTK_WIDGET(button_run));
	gtk_box_pack_start (GTK_BOX (hbox6), button_run, TRUE, TRUE, 0);

	//**************
	hbox7 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox7));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox7, TRUE, TRUE, 0);

	label7 = gtk_label_new (_("Info/Note : \t\t"));
	gtk_widget_show (GTK_WIDGET(label7));
	gtk_box_pack_start (GTK_BOX (hbox7), label7, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label7), GTK_JUSTIFY_LEFT);

	entry_proj_info = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_info));
	gtk_box_pack_start (GTK_BOX (hbox7), entry_proj_info, FALSE, FALSE, 0);
	gtk_entry_set_width_chars (GTK_ENTRY(entry_proj_info),50);
	gtk_entry_set_text (GTK_ENTRY (entry_proj_info), _(a[5]));

	//**************
	hbox8 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox8));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox8, TRUE, TRUE, 0);

	label8 = gtk_label_new (_("Icon/IMG : \t\t"));
	gtk_widget_show (GTK_WIDGET(label8));
	gtk_box_pack_start (GTK_BOX (hbox8), label8, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label8), GTK_JUSTIFY_LEFT);

	entry_proj_icon = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_icon));
	gtk_box_pack_start (GTK_BOX (hbox8), entry_proj_icon, FALSE, FALSE, 0);
	gtk_entry_set_text (GTK_ENTRY (entry_proj_icon), _(a[6]));

	button_icon = gtk_button_new_with_label ("_Open");
	gtk_widget_show (GTK_WIDGET(button_icon));
	gtk_box_pack_start (GTK_BOX (hbox8), button_icon, TRUE, TRUE, 0);

	//**************
	hbox9 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox9));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox9, TRUE, TRUE, 0);

	label9 = gtk_label_new (_("URL/HTTP : \t\t"));
	gtk_widget_show (GTK_WIDGET(label9));
	gtk_box_pack_start (GTK_BOX (hbox9), label9, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);

	entry_proj_url = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_url));
	gtk_box_pack_start (GTK_BOX (hbox9), entry_proj_url, FALSE, FALSE, 0);
	gtk_entry_set_text (GTK_ENTRY (entry_proj_url), _(a[7]));

	//**************
	hbox9 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox9));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox9, TRUE, TRUE, 0);

	label9 = gtk_label_new (_("SFTP IP : \t\t\t"));
	gtk_widget_show (GTK_WIDGET(label9));
	gtk_box_pack_start (GTK_BOX (hbox9), label9, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);

	entry_proj_sftp_ip = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_sftp_ip));
	gtk_box_pack_start (GTK_BOX (hbox9), entry_proj_sftp_ip, FALSE, FALSE, 0);
	gtk_entry_set_text (GTK_ENTRY (entry_proj_sftp_ip), _(a[8]));

	//**************
	hbox9 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox9));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox9, TRUE, TRUE, 0);

	label9 = gtk_label_new (_("SFTP USER : \t\t"));
	gtk_widget_show (GTK_WIDGET(label9));
	gtk_box_pack_start (GTK_BOX (hbox9), label9, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);

	entry_proj_sftp_user = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_sftp_user));
	gtk_box_pack_start (GTK_BOX (hbox9), entry_proj_sftp_user, FALSE, FALSE, 0);
	gtk_entry_set_text (GTK_ENTRY (entry_proj_sftp_user), _(b[0]));

	//**************
	hbox9 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox9));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox9, TRUE, TRUE, 0);

	label9 = gtk_label_new (_("SFTP PATH : \t\t"));
	gtk_widget_show (GTK_WIDGET(label9));
	gtk_box_pack_start (GTK_BOX (hbox9), label9, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);

	entry_proj_sftp_path = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_sftp_path));
	gtk_box_pack_start (GTK_BOX (hbox9), entry_proj_sftp_path, FALSE, FALSE, 0);
	gtk_entry_set_text (GTK_ENTRY (entry_proj_sftp_path), _(b[1]));

	//**************
	hbox9 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox9));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox9, TRUE, TRUE, 0);

	label9 = gtk_label_new (_("SFTP PORT : \t\t"));
	gtk_widget_show (GTK_WIDGET(label9));
	gtk_box_pack_start (GTK_BOX (hbox9), label9, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);

	entry_proj_sftp_port = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_sftp_port));
	gtk_box_pack_start (GTK_BOX (hbox9), entry_proj_sftp_port, FALSE, FALSE, 0);
	gtk_entry_set_text (GTK_ENTRY (entry_proj_sftp_port), _(a[13]));

	//**************
	hbox9 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox9));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox9, TRUE, TRUE, 0);

	label9 = gtk_label_new (_("FTP IP : \t\t\t"));
	gtk_widget_show (GTK_WIDGET(label9));
	gtk_box_pack_start (GTK_BOX (hbox9), label9, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);

	entry_proj_ftp_ip = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_ftp_ip));
	gtk_box_pack_start (GTK_BOX (hbox9), entry_proj_ftp_ip, FALSE, FALSE, 0);
	gtk_entry_set_text (GTK_ENTRY (entry_proj_ftp_ip), _(a[10]));

	//**************
	hbox9 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox9));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox9, TRUE, TRUE, 0);

	label9 = gtk_label_new (_("FTP USER : \t\t"));
	gtk_widget_show (GTK_WIDGET(label9));
	gtk_box_pack_start (GTK_BOX (hbox9), label9, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);

	entry_proj_ftp_user = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_ftp_user));
	gtk_box_pack_start (GTK_BOX (hbox9), entry_proj_ftp_user, FALSE, FALSE, 0);
	gtk_entry_set_text (GTK_ENTRY (entry_proj_ftp_user), _(a[11]));

	//**************
	hbox9 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox9));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox9, TRUE, TRUE, 0);

	label9 = gtk_label_new (_("FTP PASSWORD : \t"));
	gtk_widget_show (GTK_WIDGET(label9));
	gtk_box_pack_start (GTK_BOX (hbox9), label9, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label9), GTK_JUSTIFY_LEFT);

	entry_proj_ftp_pass = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_proj_ftp_pass));
	gtk_box_pack_start (GTK_BOX (hbox9), entry_proj_ftp_pass, FALSE, FALSE, 0);
	gtk_entry_set_text (GTK_ENTRY (entry_proj_ftp_pass), _(a[12]));

	hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox1));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox1, FALSE, FALSE, 0);

	button1 = gtk_button_new_with_label ("_Cancel");
	gtk_widget_show (GTK_WIDGET(button1));
	gtk_box_pack_start (GTK_BOX (hbox1), button1, TRUE, TRUE, 0);
	gtk_button_set_relief (GTK_BUTTON (button1), GTK_RELIEF_NONE);


	button2 = gtk_button_new_with_label ("_Apply");
	gtk_widget_show (GTK_WIDGET(button2));
	gtk_box_pack_start (GTK_BOX (hbox1), button2, TRUE, TRUE, 0);
	gtk_button_set_relief (GTK_BUTTON (button2), GTK_RELIEF_NONE);

	label1 = gtk_label_new (_("New Project"));
	gtk_widget_show (GTK_WIDGET(label1));
	gtk_frame_set_label_widget (GTK_FRAME (frame1), label1);
	gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

	gchar *file_session = g_strconcat(confile.sessions,a[0], NULL);
	cur_settings.selected_enc = ch_str (cur_settings.selected_enc, "UTF-8");
	open_file_std (file_session);

	g_signal_connect_swapped ((gpointer) button_icon, "clicked",G_CALLBACK (open_dialog_path_icon),entry_proj_icon);
	g_signal_connect_swapped ((gpointer) button_command, "clicked",G_CALLBACK (open_dialog_path),entry_proj_command);
	g_signal_connect_swapped ((gpointer) button_make_path, "clicked",G_CALLBACK (open_dialog_path_dir),entry_proj_make_path);
	g_signal_connect_swapped ((gpointer) button_path, "clicked",G_CALLBACK (open_dialog_path_dir),entry_proj_path);
	g_signal_connect_swapped ((gpointer) button_run, "clicked",G_CALLBACK (open_dialog_path),entry_proj_run);
	g_signal_connect_swapped ((gpointer) button1, "clicked",G_CALLBACK (icon_affiche_ok),NULL);	
	g_signal_connect_swapped ((gpointer) button1, "clicked",G_CALLBACK (gtk_widget_destroy), window1);
	g_signal_connect_swapped ((gpointer) button2, "clicked",G_CALLBACK (icon_affiche_ok), NULL);	
	g_signal_connect_swapped ((gpointer) button2, "clicked",G_CALLBACK (update_projects),data);	
	g_signal_connect_swapped ((gpointer) button2, "clicked",G_CALLBACK (gtk_widget_destroy), window1);
	g_signal_connect_swapped ((gpointer) button1, "clicked",G_CALLBACK (gtk_widget_destroy), window1);

}


//*********************** UPDATE PROJECTS
void update_projects (gpointer data)
{
	gchar *tmp_proj_command;
	gchar *tmp_proj_make_path;
	gchar *tmp_proj_name;
	gchar *tmp_proj_path;
	gchar *tmp_proj_run;
	gchar *tmp_proj_info;
	gchar *tmp_proj_icon;
	gchar *tmp_proj_url;
	gchar *tmp_proj_sftp_ip;
	gchar *tmp_proj_sftp_user;
	gchar *tmp_proj_sftp_path;
	gchar *tmp_proj_sftp_port;
	gchar *tmp_proj_ftp_ip;
	gchar *tmp_proj_ftp_user;
	gchar *tmp_proj_ftp_pass;

	tmp_proj_command = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_command),0, -1);
	tmp_proj_make_path = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_make_path),0, -1);
	tmp_proj_name = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_name),0, -1);
	tmp_proj_path = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_path),0, -1);
	tmp_proj_run = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_run),0, -1);
	tmp_proj_info = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_info),0, -1);
	tmp_proj_icon = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_icon),0, -1);
	tmp_proj_url = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_url),0, -1);
	tmp_proj_sftp_ip = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_sftp_ip),0, -1);
	tmp_proj_sftp_user = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_sftp_user),0, -1);
	tmp_proj_sftp_path = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_sftp_path),0, -1);
	tmp_proj_sftp_port = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_sftp_port),0, -1);
	tmp_proj_ftp_ip = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_ftp_ip),0, -1);
	tmp_proj_ftp_user = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_ftp_user),0, -1);
	tmp_proj_ftp_pass = gtk_editable_get_chars(GTK_EDITABLE(entry_proj_ftp_pass),0, -1);

	delete_project((gpointer) data);

	save_string_to_file_add(confile.projects,tmp_proj_name);
	save_string_to_file_add(confile.projects,";");
	save_string_to_file_add(confile.projects,tmp_proj_path);
	save_string_to_file_add(confile.projects,";");
	save_string_to_file_add(confile.projects,tmp_proj_make_path);
	save_string_to_file_add(confile.projects,";");
	save_string_to_file_add(confile.projects,tmp_proj_run);
	save_string_to_file_add(confile.projects,";");
	save_string_to_file_add(confile.projects,tmp_proj_command);
	save_string_to_file_add(confile.projects,";");
	save_string_to_file_add(confile.projects,tmp_proj_info);
	save_string_to_file_add(confile.projects,";");
	save_string_to_file_add(confile.projects,tmp_proj_icon);
	save_string_to_file_add(confile.projects,";");
	save_string_to_file_add(confile.projects,tmp_proj_url);
	save_string_to_file_add(confile.projects,";");
	save_string_to_file_add(confile.projects,tmp_proj_sftp_ip);
	save_string_to_file_add(confile.projects,";");
	save_string_to_file_add(confile.projects,tmp_proj_sftp_user);
	save_string_to_file_add(confile.projects,":");
	save_string_to_file_add(confile.projects,tmp_proj_sftp_path);
	save_string_to_file_add(confile.projects,";");
	save_string_to_file_add(confile.projects,tmp_proj_ftp_ip);
	save_string_to_file_add(confile.projects,";");
	save_string_to_file_add(confile.projects,tmp_proj_ftp_user);
	save_string_to_file_add(confile.projects,";");
	save_string_to_file_add(confile.projects,tmp_proj_ftp_pass);
	save_string_to_file_add(confile.projects,";");
	save_string_to_file_add(confile.projects,tmp_proj_sftp_port);
	save_string_to_file_add(confile.projects,";;;;;;;;;;;;;;;;;;;\n");

	load_projects_list();
}
