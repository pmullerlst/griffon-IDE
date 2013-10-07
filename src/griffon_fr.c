
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

#include <glib.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguage.h>
//#include <gtkspell/gtkspell.h>

#include "griffon_fr.h"
#include "griffon_defs.h"
#include "griffon_text_document.h"
#include "interface.h"


static void on_button_find (GtkWidget *wid, gpointer data)
{
  if (! get_page_text()) return;
   
  gchar *t = gtk_entry_get_text (ent_text_to_find);
  if (! t)
      return;
  
  if (gtk_toggle_button_get_active (&chb_case_insensetive->toggle_button))
      doc_search_f_ncase (cur_text_doc, t); 
  else  
      doc_search_f (cur_text_doc, t); 
}


void on_button_find_next_f (GtkWidget *wid, gpointer data)
{
  if (! get_page_text()) return;
    
  if (gtk_toggle_button_get_active (&chb_case_insensetive->toggle_button))
     doc_search_f_next_ncase (cur_text_doc); 
  else  
      doc_search_f_next (cur_text_doc); 
}


void on_button_replace (GtkWidget *wid, gpointer data)
{
  if (! get_page_text()) return;
   
  gchar *t = gtk_entry_get_text (ent_text_to_replace);
  if (! t)
      return;
  
  if (doc_is_sel (cur_text_doc->text_buffer))
     doc_rep_sel (cur_text_doc, t);  
}


void on_button_jmp_top (GtkWidget *wid, gpointer data)
{
  if (! get_page_text()) return;
  editor_set_pos (cur_text_doc, 0);
}


GtkWidget* create_fr (void)
{
  GtkWidget *window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window1), _("Search and Replace window"));

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (window1), vbox1);

  hbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);

  ent_text_to_find = tea_text_entry (hbox1, _("Text to find"), NULL);

  gchar *ft = get_clipboard_text ();

  if (ft)
      gtk_entry_set_text (ent_text_to_find, ft);
 
  g_free (ft);    

  ent_text_to_replace = tea_text_entry (hbox1, _("Replace with"), NULL);

  hbox3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox3, TRUE, TRUE, 0);

  chb_case_insensetive = gtk_check_button_new_with_mnemonic (_("Case insensitive"));
  gtk_widget_show (chb_case_insensetive);
  gtk_box_pack_start (GTK_BOX (hbox3), chb_case_insensetive, FALSE, FALSE, 0);

/*
  chb_whole_words = gtk_check_button_new_with_mnemonic (_("Whole words"));
  gtk_widget_show (chb_whole_words);
  gtk_box_pack_start (GTK_BOX (hbox3), chb_whole_words, FALSE, FALSE, 0);
*/
  //n.p. Fugees - La-Fu-Gee

  GtkWidget *hbox4 = gtk_hbox_new (TRUE, 0);
  gtk_widget_show (hbox4);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox4, TRUE, TRUE, 0);

  GtkWidget *bt_replace = gtk_button_new_with_label (_("Replace")); 
  gtk_widget_show (bt_replace);
  gtk_box_pack_start (GTK_BOX (hbox4), bt_replace, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) bt_replace, "clicked",
                    G_CALLBACK (on_button_replace),
                    NULL);

  GtkWidget *bt_jmp_top = gtk_button_new_with_label (_("Jump to top")); 
  gtk_widget_show (bt_jmp_top);
  gtk_box_pack_start (GTK_BOX (hbox4), bt_jmp_top, FALSE, FALSE, 0);
 
  g_signal_connect ((gpointer) bt_jmp_top, "clicked",
                    G_CALLBACK (on_button_jmp_top),
                    NULL);

  GtkWidget *bt_find = gtk_button_new_with_label (_("Find")); 
  gtk_widget_show (bt_find);
  gtk_box_pack_start (GTK_BOX (hbox4), bt_find, FALSE, FALSE, 0);
  
  g_signal_connect ((gpointer) bt_find, "clicked",
                    G_CALLBACK (on_button_find),
                    NULL);

  GtkWidget *bt_forward = gtk_button_new_with_label (_("Find next")); 
  gtk_widget_show (bt_forward);
  gtk_box_pack_start (GTK_BOX (hbox4), bt_forward, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) bt_forward, "clicked",
                    G_CALLBACK (on_button_find_next_f),
                    NULL);

  g_object_set (G_OBJECT (window1), "default-width", 500, NULL);

  gtk_window_set_focus (window1, ent_text_to_find); 

  g_signal_connect (window1,
                      "destroy",
                      gtk_widget_destroyed,
                      &window1); 

  //n.p. Radiohead - Heil To the thief - 2+2=5
  g_signal_connect (window1, "delete_event",
                    G_CALLBACK (gtk_false), NULL);

  gtk_window_set_modal (window1, TRUE);

  gtk_widget_show (window1);  

  return window1;
}
