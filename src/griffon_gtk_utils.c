
 /***************************************************************************
 *
 *   This program is free software; you can redistribute it and/or modify 
 *   it under the terms of the GNU General Public License as published by 
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 ***************************************************************************/


#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif


#include <glib.h>
#include <gtk/gtk.h>
#include <unistd.h>
#include <sys/stat.h>
#include <gdk/gdkkeysyms.h>

#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "griffon_text_document.h"
#include "griffon_defs.h"
#include "griffon_config.h"
#include "griffon_gtk_utils.h"
#include "griffon_funx.h"


GtkWidget* tea_text_entry (GtkWidget *container, gchar *caption, gchar *value)
{
  GtkWidget *b = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(b));
  gtk_box_pack_start (GTK_BOX(container), GTK_WIDGET(b), FALSE, TRUE, 1);
 
  GtkWidget *x = gtk_label_new (caption);  
  gtk_widget_show (GTK_WIDGET(x));
  gtk_box_pack_start (GTK_BOX(b), GTK_WIDGET(x), FALSE, FALSE, 1);
  x = gtk_entry_new ();   
  gtk_widget_show (GTK_WIDGET(x));
  gtk_box_pack_start (GTK_BOX(b), GTK_WIDGET(x), TRUE, TRUE, 1);

  if (value)
     gtk_entry_set_text (GTK_ENTRY(x), value);

  return x;
}


static void cb_select_font (GtkFontButton *widget,
                            gpointer user_data) 
{
  gtk_entry_set_text (GTK_ENTRY(user_data), gtk_font_button_get_font_name (widget));  
}


GtkWidget* tea_font_selector (GtkWidget *container, gchar *caption, gchar *value)
{
  GtkWidget *b = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(b));
  gtk_box_pack_start (GTK_BOX(container), GTK_WIDGET(b), FALSE, TRUE, 1);

  GtkWidget *x = gtk_label_new (caption);  
  gtk_widget_show (GTK_WIDGET(x));
  gtk_box_pack_start (GTK_BOX(b), GTK_WIDGET(x), FALSE, FALSE, 1);

  x = gtk_entry_new ();   
  gtk_widget_show (GTK_WIDGET(x));
  gtk_box_pack_start (GTK_BOX(b), GTK_WIDGET(x), TRUE, TRUE, 1);

  if (value)
     gtk_entry_set_text (GTK_ENTRY(x), value);


  GtkWidget *bt = gtk_font_button_new_with_font (value);
  gtk_widget_show (GTK_WIDGET(bt));
  gtk_box_pack_start (GTK_BOX(b),GTK_WIDGET(bt), FALSE, FALSE, 1);
  g_signal_connect (bt, "font-set", G_CALLBACK (cb_select_font), x);
  
  return x;
}


GtkWidget* tea_color_selector (GtkWidget *container, gchar *caption, gchar *value)
{
  GtkWidget *b =gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(b));
  gtk_box_pack_start (GTK_BOX(container), GTK_WIDGET(b), FALSE, TRUE, 1);

  GtkWidget *x = gtk_label_new (caption);  
  gtk_widget_show (GTK_WIDGET(x));
  gtk_box_pack_start (GTK_BOX(b),GTK_WIDGET( x), FALSE, FALSE, 1);

  x = gtk_entry_new ();   
  gtk_widget_show (GTK_WIDGET(x));
  gtk_box_pack_start (GTK_BOX(b), GTK_WIDGET(x), TRUE, TRUE, 1);

  if (value)
     gtk_entry_set_text (GTK_ENTRY(x), value);

//  GdkColor color;
//  gdk_color_parse (value, &color);

  return x;
}


GtkWidget* tea_checkbox (GtkWidget *container, gchar *caption, gboolean *value)
{
  GtkWidget *b = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(b));
  gtk_box_pack_start (GTK_BOX(container), GTK_WIDGET(b), FALSE, TRUE, 1);

  GtkWidget *x = gtk_check_button_new_with_label (caption);
  gtk_widget_show (GTK_WIDGET(x));
  gtk_box_pack_start (GTK_BOX(b), GTK_WIDGET(x), TRUE, TRUE, 1);
  gtk_toggle_button_set_active ((GtkToggleButton *)x, *value);
    
  return x;
}


GtkWidget* tea_spinbutton (GtkWidget *container, gchar *caption, gdouble value)
{
  GtkWidget *b = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(b));
  gtk_box_pack_start (GTK_BOX(container), GTK_WIDGET(b), FALSE, TRUE, 1);

  GtkWidget *l = gtk_label_new (caption);  
  gtk_widget_show (GTK_WIDGET(l));
  gtk_box_pack_start (GTK_BOX(b), GTK_WIDGET(l), FALSE, FALSE, 1);
 
  GtkWidget *x = gtk_spin_button_new (gtk_adjustment_new (value, 1.0, 1080.0, 1.0,
          5.0, 0.0),
                                           1, 0); 

  gtk_widget_show (GTK_WIDGET(x));
  gtk_box_pack_start (GTK_BOX(b), GTK_WIDGET(x), TRUE, TRUE, 1);
  return x;
}


void logmemo_set_pos (GtkTextView *v, guint pos)
{
  GtkTextIter it;
  GtkTextBuffer *b = gtk_text_view_get_buffer (v);
  gtk_text_buffer_get_iter_at_offset (b, &it, pos);
  gtk_text_buffer_place_cursor (b, &it);
  gtk_text_view_scroll_to_iter (v, &it, 0.0, FALSE, 0.0, 0.0);
}


GList* add_combo_hist_with_def (GList *l)
{

  return l;
}


GtkWidget* tea_dir_selector (GtkWidget *container, gchar *caption, gchar *value)
{
  GtkWidget *b = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(b));
  gtk_box_pack_start (GTK_BOX(container), GTK_WIDGET(b), FALSE, TRUE, 1);

  GtkWidget *x = gtk_label_new (caption);  
  gtk_widget_show (GTK_WIDGET(x));
  gtk_box_pack_start (GTK_BOX(b), GTK_WIDGET(x), FALSE, FALSE, 1);

  x = gtk_entry_new ();   
  gtk_widget_show (GTK_WIDGET(x));
  gtk_box_pack_start (GTK_BOX(b), GTK_WIDGET(x), TRUE, TRUE, 1);

  if (value)
     gtk_entry_set_text (GTK_ENTRY(x), value);

  GtkWidget *bt = gtk_button_new_with_label (_("Choose"));
  gtk_widget_show (GTK_WIDGET(bt));
  gtk_box_pack_start (GTK_BOX(b), GTK_WIDGET(bt), FALSE, FALSE, 1);

  return x;
}


GtkWidget* tea_combo (GtkWidget *container, gchar *caption)
{
  GtkWidget *b = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(b));
  gtk_box_pack_start (GTK_BOX(container), GTK_WIDGET(b), FALSE, TRUE, 1);
 
  GtkWidget *x = gtk_label_new (caption);  
  gtk_widget_show (GTK_WIDGET(x));
  gtk_box_pack_start (GTK_BOX(b), GTK_WIDGET(x), FALSE, FALSE, 1);

	return FALSE;
}

void widget_set_colors (GtkWidget *w, GdkColor *background, GdkColor *text,
                        GdkColor *selection, GdkColor *sel_text)
{
	if(w==NULL){printf(" ");}
	if(background==NULL){printf(" ");}
	if(selection==NULL){printf(" ");}
	if(sel_text==NULL){printf(" ");}
	if(text==NULL){printf(" ");}

/*  if (background)
     gtk_widget_modify_base (GTK_WIDGET (w), GTK_STATE_NORMAL, background);

  if (text)
     gtk_widget_modify_text (GTK_WIDGET (w), GTK_STATE_NORMAL, text);

  if (selection)
    {
     gtk_widget_modify_base (GTK_WIDGET (w), GTK_STATE_SELECTED, selection);
     gtk_widget_modify_base (GTK_WIDGET (w), GTK_STATE_ACTIVE, selection);
    }

  if (sel_text)
    {
     gtk_widget_modify_text (GTK_WIDGET (w), GTK_STATE_SELECTED, sel_text);
     gtk_widget_modify_text (GTK_WIDGET (w), GTK_STATE_ACTIVE, sel_text);
    }*/
}


gboolean dlg_question (GtkWidget *parent, gchar *prompt, gchar *val)
{
   gboolean result = FALSE;  
   GtkWidget *dialog = gtk_dialog_new_with_buttons (NULL, 
                                                    GTK_WINDOW (parent),
                                                    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                    "_OK",
                                                    GTK_RESPONSE_ACCEPT,
                                                    "_Cancel",
                                                    GTK_RESPONSE_REJECT, 
                                                    NULL);
  gchar *s; 
  if (val)
     s = g_strdup_printf (prompt, val);
  else    
      s = g_strdup (prompt);

  GtkWidget *label = gtk_label_new (s);
  gtk_widget_show (GTK_WIDGET(label));

	GtkWidget *content_area;
   content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
   gtk_container_add (GTK_CONTAINER (content_area), label);

  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
     result = TRUE;
   
  gtk_widget_destroy (dialog);
  g_free (s);

  return result;
}


static void dlg_question_params_button_cb (gpointer user_data)
{
  t_ppair *p = user_data;

  if (p->c)
     gtk_dialog_response (GTK_DIALOG (p->a), GTK_RESPONSE_ACCEPT);
  else
      gtk_dialog_response (GTK_DIALOG (p->a), GTK_RESPONSE_REJECT);

  g_free (p);
}


gint dlg_question_params (gchar *prompt, gchar *val, gchar *b1, gchar *b2)
{
   gint result = FALSE;  
   
  GtkWidget *dialog = gtk_dialog_new ();
  
  gchar *s; 
  if (val)
     s = g_strdup_printf (prompt, val);
  else    
      s = g_strdup (prompt);

  GtkWidget *label = gtk_label_new (s);
  gtk_label_set_line_wrap (GTK_LABEL(label), TRUE); 
  gtk_widget_show (GTK_WIDGET(label));

  GtkWidget *bt1 = gtk_button_new_with_label (b1) ;
  gtk_widget_show (GTK_WIDGET(bt1));

  t_ppair *p = g_malloc (sizeof (t_ppair));
  p->a = dialog;
  p->c = TRUE;

  g_signal_connect (bt1,
                    "clicked",
                    G_CALLBACK (dlg_question_params_button_cb),
                    p);
  
  GtkWidget *bt2 = gtk_button_new_with_label (b2) ;
  gtk_widget_show (GTK_WIDGET(bt2));

  p = g_malloc (sizeof (t_ppair));
  p->a = dialog;
  p->c = FALSE;

  g_signal_connect (bt2,
                    "clicked",
                    G_CALLBACK (dlg_question_params_button_cb),
                    p);

  gtk_window_set_modal (GTK_WINDOW(dialog), TRUE);

  result = gtk_dialog_run (GTK_DIALOG (dialog));
     
  gtk_widget_destroy (dialog);
  g_free (s);

  if (result == GTK_RESPONSE_ACCEPT)
     result = 0;

  if (result == GTK_RESPONSE_REJECT)
     result = 1;

  return result;
}


void dlg_info (GtkWidget *parent, gchar *prompt, gchar *val)
{
  GtkWidget *dialog = gtk_dialog_new_with_buttons (NULL, 
                                                   GTK_WINDOW (parent),
                                                   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   "_OK",
                                                   GTK_RESPONSE_ACCEPT,
                                                   NULL);
  gchar *s; 
  if (val)
     s = g_strdup_printf (prompt, val);
  else    
      s = g_strdup (prompt);

  GtkWidget *label = gtk_label_new (s);
  gtk_widget_show (GTK_WIDGET(label));
  gtk_label_set_line_wrap (GTK_LABEL(label), TRUE); 
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
  g_free (s);
}


void dlg_info_with_image (GtkWidget *parent, gchar *prompt, gchar *val, gchar *image)
{
  GtkWidget *dialog = gtk_dialog_new_with_buttons (NULL, 
                                                   GTK_WINDOW (parent),
                                                   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   "_OK",
                                                   GTK_RESPONSE_ACCEPT,
                                                   NULL);
  gchar *s; 
  if (val)
     s = g_strdup_printf (prompt, val);
  else    
      s = g_strdup (prompt);
  
  if (g_file_test (image, G_FILE_TEST_EXISTS))
    {
     GtkWidget *img = gtk_image_new_from_file (image);
     gtk_widget_show (GTK_WIDGET(img));
    }

  GtkWidget *label = gtk_label_new (s);
  gtk_widget_show (GTK_WIDGET(label));
  gtk_label_set_line_wrap (GTK_LABEL(label), TRUE); 
  
  gtk_dialog_run (GTK_DIALOG (dialog));
  
  gtk_widget_destroy (dialog);
  g_free (s);
}


GtkWidget* tea_button_at_box (GtkWidget *container, gchar *caption, gpointer callback, gpointer user_data)
{
  GtkWidget *b = gtk_button_new_with_label (caption);
  gtk_widget_show (GTK_WIDGET(b));

  gtk_box_pack_start (GTK_BOX (container),
                      GTK_WIDGET(b),
                      FALSE,
                      FALSE,
                      0);

  g_signal_connect (b,
                    "clicked",
                    G_CALLBACK (callback),
                    user_data);
  return b;
}


//void cb_on_bt_close (GtkWidget *w) 
void cb_on_bt_close (GtkWidget *w2,gpointer w)
{
	if(w2==NULL){printf(" ");}
  window_destroy (w);
}


static void find_menuitem_cb (GtkWidget *widget, gpointer data)
{
  if (! widget)
     return;

  gchar const *s = gtk_widget_get_name (widget);

  if (g_utf8_collate (data, s) == 0)
     {
      mni_temp = widget;       
      return;
     }

  if (GTK_IS_MENU_ITEM (widget))
     find_menuitem (GTK_CONTAINER (gtk_menu_item_get_submenu (GTK_MENU_ITEM(widget))), data);      
        
}

GtkWidget* find_menuitem (GtkContainer *widget, gchar *widget_name)
{
  if (widget)
     gtk_container_foreach (widget, find_menuitem_cb, widget_name);

  return mni_temp;
}
