
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 **************************************-************************************/                                  

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>

#include <errno.h>
#include <sys/stat.h>
#include <ctype.h>
#include <glib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <vte/vte.h>

#include "griffon_text_document.h"
#include "griffon_funx.h"
#include "callbacks.h"
#include "interface.h"
#include "rox_strings.h"
#include "griffon_config.h"
#include "griffon_defs.h"
#include "griffon_hl.h"
#include "griffon_fr.h"
#include "griffon_proj.h"
#include "griffon_gtk_utils.h"
#include "griffon_options.h"
#include "help_def.h"

#define MAX_BUF 165536

  GtkWidget *checkbutton1,*checkbutton2,*checkbutton3,*checkbutton4,*checkbutton5,*entry1,*entry2,*entry_scp1,*entry_scp2,*entry_scp3,*entry_scp4,*entry_sftp,*entry_utilisateur,*entry_chemin,*entry_ftp,*entry_utilisateur_ftp;
  GtkWidget *entry_passwd_ftp,*entry_ln1,*entry_ln2,*entry_fonction,*entry_fonction2,*entry_compress,*checkbutton1_grep,*checkbutton2_grep,*checkbutton3_grep,*checkbutton4_grep;
  GtkWidget *entry_grep1,*entry_grep2,*entry_grep3,*checkbutton_sed1,*entry_sed4,*entry_sed5,*checkbutton_sed2,*entry_sed6,*entry_sed7,*checkbutton_sed3,*checkbutton_sed4,*checkbutton_sed5;
  GtkWidget *checkbutton_sed6,*checkbutton_sed7,*entry_sed1,*entry_sed2,*entry_sed3,*spinbutton1,*checkbutton_awk1,*entry_awk3,*entry_awk1,*entry_awk4,*entry_awk5,*checkbutton_rouge;
  GtkWidget *checkbutton_vert,*checkbutton_jaune,*entry_echo_couleur;
	int tag_on=0;



void set_fam_text (gchar *def)
{
icon_affiche_ok();
  if (strlen (gtk_entry_get_text (GTK_ENTRY(ent_search))) == 0)
     {
      gtk_entry_set_text (GTK_ENTRY(ent_search), def); 
      icon_man_logmemo(); 
     log_to_memo (_("La documentation est disponible dans le menu \"Aide->Lire le manuel/documentation de Griffon IDE\""), NULL, LM_ERROR);
     } 
}

void on_mni_file_open_at_cursor ()
{
icon_affiche_ok();
  if (! get_page_text()) return;

  gchar *w = get_c_url (cur_text_doc);
  handle_file (w, 0);
  g_free (w);
}


static void aliens_init (void)
{
  /*gint r;

icon_affiche_ok();
  if (confile.warn_about_aliens)
    {
     r = dlg_question_params (tea_main_window,
                              _("Those little green men in a toilet! I saw them! Can you kill them?"),
                              NULL, 
                              _("Yes, I'll do it"),
                              _("Why should I kill them? There is must be a pacific way..."));
    if (r == 1)
       {
        r = dlg_question_params (tea_main_window,
                                 _("But they are... Such green! And small! Mens!"),
                                 NULL, 
                                _("OK, I'll kill them all..."),
                                _("So what?"));

        if (r == 1)
           {
            dlg_info (tea_main_window, _("We are DOOOMED!!!"), NULL);
            return; 
           }
       }

    if (r == 0)
       {
        dlg_info_with_image (tea_main_window, _("We are little green mens from the toilet. You cannot defeat us!"), NULL, TEA_PIX_LGM);
        return; 
       } 
    }*/
}

gboolean tea_init ()
{
  gint i; 
  gchar *fn;

icon_affiche_ok();
  if (g_argc > 1)
  for (i = 1; i <= g_argc; ++i)
      {
       fn = g_malloc (2048); 


       if (realpath (g_argv[i], fn))
          open_file_std (fn); 

       g_free (fn);
      }

  if (g_argc <= 1)
    {
     if (confile.start_with_blank_file)
       on_mni_new_file_activate (NULL, NULL);

     if (confile.start_with_recent_file)
       on_mni_load_last_file (NULL, NULL);
    }

  aliens_init ();
	return TRUE;
}  

void file_new (void)
{
  cur_text_doc = doc_clear_new ();
 
	icon_affiche_ok(); 
  gtk_window_set_title (GTK_WINDOW (tea_main_window), cur_text_doc->file_name);
  gtk_widget_grab_focus (GTK_WIDGET(cur_text_doc->text_view));
  tabs_reload ();
	no_onglet_open();
	statusbar_msg (_("New File [OK]"));
		//gtk_notebook_set_current_page(notebook3,1);
}

void on_mni_new_file_activate ()
{
	icon_affiche_ok();
	file_new ();
	no_onglet_open();
}

void on_mni_new_file_open_select_enc (GtkMenuItem *menuitem)
{
  cur_settings.selected_enc = ch_str (cur_settings.selected_enc, gtk_widget_get_name (GTK_WIDGET(menuitem)));
icon_affiche_ok();
}

void file_open (void)
{
  gchar *t;
 
icon_affiche_ok();
  GtkWidget *file_dialog = gtk_file_chooser_dialog_new (_("File open"),
                                                        GTK_WINDOW(tea_main_window),
                                                        GTK_FILE_CHOOSER_ACTION_OPEN,
                                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                        GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                                        NULL);

  gtk_file_chooser_set_local_only (GTK_FILE_CHOOSER (file_dialog), TRUE);

  if (confile.use_def_open_dir)
     gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (file_dialog), confile.def_open_dir);
  else
      if (get_page_text())
         if (g_file_test (cur_text_doc->file_name, G_FILE_TEST_EXISTS))
            {
             t = g_path_get_dirname (cur_text_doc->file_name);
             gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (file_dialog), t);
             g_free (t);
            }

  t_ppair *p = g_malloc (sizeof (t_ppair));
  p->a = file_dialog;
  g_free (p);

  gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (file_dialog), TRUE);

  if (gtk_dialog_run (GTK_DIALOG (file_dialog)) == GTK_RESPONSE_ACCEPT)
    {
     gchar *filename = NULL;

     GSList *l = gtk_file_chooser_get_filenames (GTK_FILE_CHOOSER (file_dialog));
     if (l)  
        {
         GSList *t = l;

         while (t)
              {
               filename = t->data; 
               open_file_std (filename);
               g_free (filename);
               t = g_slist_next (t);
              }

         g_slist_free (l);
        }
    }

  gtk_widget_destroy (GTK_WIDGET(file_dialog));

}

void on_mni_file_open_activate (){icon_affiche_ok();file_open ();}

void file_save (void)
{
icon_affiche_ok();
  if (! get_page_text()) return;

	//********************************* Affichage des icons mark
	//do_hl_c (cur_text_doc);
  
  if (! cur_text_doc->b_saved)
		{		apply_hl (cur_text_doc);
     on_mni_file_save_as_activate (NULL, NULL);
	 }
  else
      if (text_doc_save (cur_text_doc, cur_text_doc->file_name))
	      icon_save_logmemo();		      	
         log_to_memo (_("%s Sauvegarde [OK]"), cur_text_doc->file_name, LM_NORMAL);
							griffon_notify(_("Save [OK]"));
				statusbar_msg (_("Save [OK]"));
         icon_affiche_save (); 
}

void on_mni_file_save_activate (){icon_affiche_ok();file_save ();}

void file_save_as (void)
{
	icon_affiche_ok();
  if (get_page_text ())
     show_save_as_dlg (0);
}

void on_mni_file_save_as_activate (){icon_affiche_ok();file_save_as ();}

void on_mni_out_activate (){  ui_done();  gtk_main_quit ();}

void on_mni_case_upcase_activate ()
{

icon_affiche_ok();
  if (! get_page_text()) return;
  
  gchar *buf = doc_get_sel (cur_text_doc);
  gchar *t;

  if (buf)
     {
      t = g_utf8_strup (buf, -1);
      doc_rep_sel (cur_text_doc, t);
      g_free (buf);
      g_free (t);
     }
  else
      { 
       GtkTextIter itstart;
       GtkTextIter itend; 
       buf = doc_get_current_word (cur_text_doc, &itstart, &itend);

       if (! buf)
          return;
  
       t = g_utf8_strup (buf, -1);
      
       doc_set_new_text (cur_text_doc, &itstart, &itend, t);        
                                                    
       g_free (buf);
       g_free (t);
      }
}

void on_mni_case_locase_activate () 
{

icon_affiche_ok();
  if (! get_page_text()) return;
  
  gchar *buf = doc_get_sel (cur_text_doc);
  gchar *t;

  if (buf)
     {
      t = g_utf8_strdown (buf, -1);
      doc_rep_sel (cur_text_doc, t);
      g_free (buf);
      g_free (t);
     }
  else
      {
       GtkTextIter itstart;
       GtkTextIter itend; 
       buf = doc_get_current_word (cur_text_doc, &itstart, &itend);

       if (! buf)
          return;

       t = g_utf8_strdown (buf, -1);
      
       doc_set_new_text (cur_text_doc, &itstart, &itend, t);        
                                                    
       g_free (buf);
       g_free (t);
      }
}

void on_mni_reverse () 
{

icon_affiche_ok();
  if (! get_page_text()) return;
  
  gchar *buf = doc_get_sel (cur_text_doc);
  if (! buf) return;

  gchar *t = g_utf8_strreverse (buf, -1);
  doc_rep_sel (cur_text_doc, t);

  g_free (buf);
  g_free (t);
}

void on_mni_file_save_version () 
{
icon_affiche_ok();
  if (! get_page_text()) return;
  
  gchar *s = get_file_ver_name (cur_text_doc->file_name);
  if (! s)
     {
	   icon_stop_logmemo();
      log_to_memo(_("Sauvegardez le fichier avant..."), NULL, LM_ADVICE);
      return;
     }

  text_doc_save (cur_text_doc, s);
	icon_save_logmemo();
	icon_affiche_save ();
  log_to_memo (_("%s Sauvegarde OK..."), s, LM_NORMAL);
	statusbar_msg (_("Save [OK]"));
  g_free (s);
}

void on_mni_Markup_bold_activate ()
{
icon_affiche_ok();
  if (! get_page_text()) return;
  
  gchar *t = NULL;
  gchar *buf = doc_get_sel (cur_text_doc);

  if (! buf)
      {
       doc_insert_at_cursor (cur_text_doc, "<b></b>");
       doc_move_cursor_backw_middle_tags (cur_text_doc);
      }
  else
      {
       t = g_strconcat ("<b>", buf, "</b>", NULL);
       doc_rep_sel (cur_text_doc, t);
      }

  g_free (buf);
  g_free (t);
}


void on_mni_Markup_link_activate () 
{
	icon_affiche_ok();
  if (! get_page_text()) return;
  
  gchar *t = NULL;
  gchar *buf = doc_get_sel (cur_text_doc);

  if (! buf)
     {
      doc_insert_at_cursor (cur_text_doc, "<a href=\"\"></a>");
      doc_move_cursor_backw_middle_tags (cur_text_doc);
     }
  else
      {
       t = g_strconcat ("<a href=\"\">", buf, "</a>", NULL);
       doc_rep_sel (cur_text_doc, t);
       doc_move_to_pos_bw_quote (cur_text_doc);
      }

  g_free (buf);
  g_free (t);
}


void on_mni_Markup_italic_activate () 
{
	icon_affiche_ok();
  if (! get_page_text()) return;
  
  gchar *t = NULL;
  gchar *buf = doc_get_sel (cur_text_doc);

  if (! buf)
     {
      doc_insert_at_cursor (cur_text_doc, "<i></i>");
      doc_move_cursor_backw_middle_tags (cur_text_doc);
     }
  else
      {
       t = g_strconcat ("<i>", buf, "</i>", NULL);
       doc_rep_sel (cur_text_doc, t);
       g_free (t);
      }

  g_free (buf);
}

void on_mni_Markup_underline_activate () 
{
  if (! get_page_text()) return;
  
  gchar *buf = doc_get_sel (cur_text_doc);
  if (! buf)
      {
       doc_insert_at_cursor (cur_text_doc, "<u></u>");
       doc_move_cursor_backw_middle_tags (cur_text_doc);
      }

  gchar *t = g_strconcat ("<u>", buf, "</u>", NULL);
  doc_rep_sel (cur_text_doc, t);

  g_free (buf);
  g_free (t);
}

void on_mni_Markup_para_activate () 
{
  if (! get_page_text()) return;
  
  gchar *t = NULL;
  gchar *buf = doc_get_sel (cur_text_doc);

  if (! buf)
      {
       doc_insert_at_cursor (cur_text_doc, "<p></p>");
       doc_move_cursor_backw_middle_tags (cur_text_doc);
      }
  else
      {
       t = g_strconcat ("<p>", buf, "</p>", NULL);
       doc_rep_sel (cur_text_doc, t);
       g_free (t);
      }

  g_free (buf);
}

void on_mni_Markup_header_activate (GtkMenuItem *menuitem)
{
  if (! get_page_text()) return;
  
  gchar *t = NULL;
  gchar *buf = doc_get_sel (cur_text_doc);

  if (! buf)
     {
      t = g_strconcat ("<", gtk_widget_get_name (GTK_WIDGET(menuitem)), ">",
                       "</", gtk_widget_get_name (GTK_WIDGET(menuitem)), ">",
                       NULL);
      doc_insert_at_cursor (cur_text_doc, t);
      doc_move_cursor_backw_middle_tags (cur_text_doc);
     }
  else
      {
       t = g_strconcat ("<", gtk_widget_get_name (GTK_WIDGET(menuitem)), ">",
                        buf,
                        "</", gtk_widget_get_name (GTK_WIDGET(menuitem)), ">",
                        NULL);
       doc_rep_sel (cur_text_doc, t);
      }

  g_free (buf);
  g_free (t);
}


static gint last_page = -1;

void on_notebook1_switch_page (GtkNotebook *notebook,guint page_num)
{
  t_note_page *dc = get_page_by_index (page_num);
  
	icon_affiche_ok();
  if (dc)
     {
      set_title (dc);
      gtk_widget_grab_focus (GTK_WIDGET(dc->text_view));
      last_page = gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook));
     }
}

void on_mni_file_close_current ()
{
	icon_affiche_ok();
   page_del_by_index (gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook1)));
	no_onglet_open();
}

void on_mni_view_wrap ()
{
	icon_affiche_ok();
  if (! get_page_text()) return;
  
  if (gtk_text_view_get_wrap_mode (GTK_TEXT_VIEW(cur_text_doc->text_view)) != GTK_WRAP_WORD)
      gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW(cur_text_doc->text_view), GTK_WRAP_WORD);
  else
      gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW(cur_text_doc->text_view), GTK_WRAP_NONE);
}

void on_mni_Markup_br ()
{
  if (! get_page_text()) return;
  if (! confile.xhtml_mode)
     doc_insert_at_cursor (cur_text_doc, "<br>");
  else
      doc_insert_at_cursor (cur_text_doc, "<br />"); 
}
                       
void on_mni_Functions_number_arabian2roman ()
{
	icon_affiche_ok();
  if (! get_page_text()) return;
  
  gchar *buf = doc_get_sel (cur_text_doc);
  if (! buf) return;

  gchar *t = g_malloc (1024);

  tNumber2Roman (strtol (buf, NULL, 10), TRUE, t);

  if (t)
     doc_rep_sel (cur_text_doc, t);

  g_free (t);
  g_free (buf);
}

void on_mni_view_show_line_numbers ()
{
	icon_affiche_ok();
  if (! get_page_text()) return;
  
  if (! cur_text_doc->linenums)
      document_set_line_numbers (cur_text_doc, TRUE);
   else
       document_set_line_numbers (cur_text_doc, FALSE);
}

void on_button_close ( gpointer data)
{
	icon_affiche_ok();
  page_del_by_index (find_index_by_page (data));
	no_onglet_open();
	delete_autocomp_tips();
}

void on_mni_html_default_template ()
{
  if (! get_page_text()) return;
  
  gchar *t = g_strconcat (
                          "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/1999/REC-html401-19991224/loose.dtd\">\n", 
                          "<html>\n",
                          "<head>\n",
                          "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n",
                          "<title></title>\n",
                          "</head>\n",
                          "<body>\n",
                          "</body>\n",
                          "</html>",
                          NULL);
                                       
  doc_insert_at_cursor (cur_text_doc, t);
  g_free (t);
}

void on_mni_file_save_session ()
{
	icon_affiche_ok();
  GtkWidget *dialog = gtk_file_chooser_dialog_new (_("Sauvegarde de la session sous:"),
                                                   GTK_WINDOW(tea_main_window),
                                                   GTK_FILE_CHOOSER_ACTION_SAVE,
                                                   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                   GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                                                   NULL);

  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), confile.sessions_dir);
 
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
     gchar *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

     GList *p = g_list_first (dox);
     GList *t = NULL;

     while (p)
          {
           t = add_recent_item_composed (t, (t_note_page*) p->data);
           p = g_list_next (p);
          }

     glist_save_to_file (t, filename);
     g_list_free (t);                      
     reload_sessions ();
     g_free (filename);
    }

  gtk_widget_destroy (GTK_WIDGET(dialog));
}

void on_mni_session_file_open_activate ()
{
  GtkWidget *dialog = gtk_file_chooser_dialog_new (_("Ouverture du fichier de session:"),
                                                   GTK_WINDOW(tea_main_window),
                                                   GTK_FILE_CHOOSER_ACTION_OPEN,
                                                   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                   GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                                   NULL);

  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), confile.sessions_dir);
  
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
     {
      gchar *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
      cur_settings.selected_enc = ch_str (cur_settings.selected_enc, "UTF-8");
      open_file_std (filename);
      g_free (filename);
     }

   gtk_widget_destroy (GTK_WIDGET(dialog));
}

void on_mni_antispam ()
{
  if (! get_page_text()) return;
 
  gchar *buf = doc_get_sel (cur_text_doc);
  if (! buf) return;

  gint c = g_utf8_strlen (buf, STRMAXLEN);
  GString *result = g_string_sized_new (16 * c);

  gint i;

  for (i = 0; i <= c; i++) 
       g_string_append_printf (result, "&#%d;", buf[i]);

  doc_rep_sel (cur_text_doc, result->str);

  g_free (buf);
  g_string_free (result, TRUE);
}

void on_mni_Functions_number_hex2dec ()
{
  if (! get_page_text()) return;
  
  gchar *buf = doc_get_sel (cur_text_doc);
  if (! buf) return;

  gchar *t = g_strdup_printf ("%ld", strtol (buf, NULL, 16));  

  doc_rep_sel (cur_text_doc, t);

  g_free (t);
  g_free (buf);
}

void on_mni_config_open ()
{
  cur_settings.selected_enc = ch_str (cur_settings.selected_enc, "UTF-8");
  open_file_std (confile.tea_rc);
}

void on_mni_open_in_browser (GtkMenuItem *menuitem)
{
icon_affiche_ok();
  if (! get_page_text()) return;
  
  gchar *cmd="";
  gchar const *wn = gtk_widget_get_name (GTK_WIDGET(menuitem));

  if (g_ascii_strncasecmp (wn, "opera", 33) == 0)
     cmd = rep_all_s (confile.cmd_Opera, cur_text_doc->file_name);
  else     
  if (g_ascii_strncasecmp (wn, "firefox", 33) == 0)
     cmd = rep_all_s (confile.cmd_Firefox, cur_text_doc->file_name);
  else
  if (g_ascii_strncasecmp (wn, "mozilla", 33) == 0)
     cmd = rep_all_s (confile.cmd_Mozilla, cur_text_doc->file_name);
  else
  if (g_ascii_strncasecmp (wn, "konqueror", 33) == 0)
     cmd = rep_all_s (confile.cmd_Konqueror, cur_text_doc->file_name);
  else
  if (g_ascii_strncasecmp (wn, "galeon", 33) == 0)
     cmd = rep_all_s (confile.cmd_Galeon, cur_text_doc->file_name);
  else
  if (g_ascii_strncasecmp (wn, "dillo",33) == 0)
     cmd = rep_all_s (confile.cmd_Dillo, cur_text_doc->file_name);
  else
  if (g_ascii_strncasecmp (wn, "elinks",33) == 0)
     cmd = rep_all_s (confile.cmd_ELinks, cur_text_doc->file_name);
  else
  if (g_ascii_strncasecmp (wn, "links",33) == 0)
     cmd = rep_all_s (confile.cmd_Links, cur_text_doc->file_name);
  else
  if (g_ascii_strncasecmp (wn, "lynx", 33) == 0)
     cmd = rep_all_s (confile.cmd_Lynx, cur_text_doc->file_name);
  else
  if (g_ascii_strncasecmp (wn, "epiphany", 33) == 0)
     cmd = rep_all_s (confile.cmd_Epiphany, cur_text_doc->file_name);
  else
  if (g_ascii_strncasecmp (wn, "Other browser", 33) == 0)
     cmd = rep_all_s (confile.cmd_Other, cur_text_doc->file_name);

  int systemRet =system (cmd);
	if(systemRet == -1){return;}

  g_free(cmd);
}

void on_mni_utils_stats ()
{
	icon_affiche_ok();
  if (! get_page_text()) return;
  make_stats (cur_text_doc);
}

void on_mni_utils_add_encodings_activate ()
{
  cur_settings.selected_enc = ch_str (cur_settings.selected_enc, "UTF-8");
  open_file_std (confile.iconv_file);
}

void on_mni_Functions_number_bin2dec ()
{
  if (! get_page_text()) return;
  
  gchar *buf = doc_get_sel (cur_text_doc);
  if (! buf) return;
  
  gchar *t = g_strdup_printf ("%ld", strtol (buf, NULL, 2)); 

  doc_rep_sel (cur_text_doc, t);

  g_free (t);
  g_free (buf);
}
                                     
void on_mni_Markup_nbsp ()
{
  if (! get_page_text()) return;
  doc_insert_at_cursor (cur_text_doc, "&nbsp;");
}

void on_mni_undo ()
{
	icon_affiche_ok();
  if (! get_page_text()) return;
	gtk_source_buffer_undo(GTK_SOURCE_BUFFER(cur_text_doc->text_buffer));
}

void on_mni_redo ()
{
	icon_affiche_ok();
  if (! get_page_text()) return;
	gtk_source_buffer_redo(GTK_SOURCE_BUFFER(cur_text_doc->text_buffer));
}

void on_mni_image_activate ()
{
  if (! get_page_text()) return;
  
  gchar *t = NULL;
  gchar *buf = doc_get_sel (cur_text_doc);

  if (! buf)
     {
      doc_insert_at_cursor (cur_text_doc, "<img src=\"\">");
      doc_move_cursor_backw_middle_tags (cur_text_doc);
     }
  else
      {
       t = g_strconcat ("<img src=\"", buf, "\">", NULL);
       doc_rep_sel (cur_text_doc, t);
       doc_move_to_pos_bw_quote (cur_text_doc);
      }

  g_free (buf);
  g_free (t);
}

void on_mni_html_enclose_link ()
{
  if (! get_page_text()) return;
  
  gchar *buf = doc_get_sel (cur_text_doc);

  if (! buf) return;

  gchar *t = linkify_text (buf);
  doc_rep_sel (cur_text_doc, t);

  g_free (buf);
  free (t);
}

gboolean on_ent_search_key_press_event (GdkEventKey *event)
{
icon_affiche_ok();
  if (event->keyval != GDK_KEY_Return)
     return FALSE;      

  if (get_page_text())
     {
      doc_search_f (cur_text_doc, gtk_entry_get_text (GTK_ENTRY(ent_search)));gtk_widget_grab_focus (GTK_WIDGET(cur_text_doc->text_view));

				GtkTreeIter iter_entry;
      gtk_list_store_append(model_entry, &iter_entry);
      gtk_list_store_set(model_entry, &iter_entry, CONTACT_NAME, gtk_entry_get_text (GTK_ENTRY(ent_search)),  -1);

     }

  return FALSE;
}

void on_mni_func_strings_removeblanks ()
{
icon_affiche_ok();
  if (! get_page_text()) return;
  
  gchar *buf = doc_get_sel (cur_text_doc);

  if (! buf) return;

  GList *temp = glist_from_string (buf);
  GList *temp2 = remove_blank_lines (temp);
  gchar *t = string_from_glist (temp2);

  doc_rep_sel (cur_text_doc, t);

  g_free (buf);
  g_free (t);
  glist_strings_free (temp);
  glist_strings_free (temp2);
}

void on_mni_func_strings_enclose ()
{
  if (! get_page_text()) return;
  
  gchar *buf = doc_get_sel (cur_text_doc);

  if (! buf) return;

  set_fam_text ("<li>%s</li>");

  if (! strstr (gtk_entry_get_text (GTK_ENTRY(ent_search)), "%s"))
     {
      g_free (buf);
      return;
     }

  GList  *temp = glist_from_string (buf);
	gchar const *zz=gtk_entry_get_text (GTK_ENTRY(ent_search));
  temp = glist_repl (temp, zz);
  gchar  *t = string_from_glist (temp);

  doc_rep_sel (cur_text_doc, t);

  g_free (buf);
  g_free (t);
  glist_strings_free (temp);
}

void on_mni_func_strings_rev ()
{
  if (! get_page_text()) return;
  
  gchar *buf = doc_get_sel (cur_text_doc);

  if (! buf) return;

  GList *temp = glist_from_string (buf);
  temp = g_list_reverse (temp);
  gchar *t = string_from_glist (temp);
  doc_rep_sel (cur_text_doc, t);

  g_free (buf);
  g_free (t);
  glist_strings_free (temp);
}

void on_mni_search_repall ()
{
  if (! get_page_text()) return;
  
  set_fam_text ((_("Texte pour trouver le texte à remplacer")));
  gchar const *s = gtk_entry_get_text (GTK_ENTRY(ent_search));

				GtkTreeIter iter_entry;
      gtk_list_store_append(model_entry, &iter_entry);
      gtk_list_store_set(model_entry, &iter_entry, CONTACT_NAME, gtk_entry_get_text (GTK_ENTRY(ent_search)),  -1);

  if (! strstr (s, "~"))
     return; 
 
  gchar **a = g_strsplit (s, "~", -1);

  if (! a) return;

  gchar *x; 
  gchar *y;

  x = a[0];
  if (! x)
     {
      g_strfreev (a);
      return;
     } 

  if (! a[1])
     {
      g_strfreev (a);
      return;
     } 

  y = a[1];

  gchar *buf = doc_get_buf (GTK_TEXT_BUFFER(cur_text_doc->text_buffer));
  gchar *z = str_replace_all (buf, x, y);
 
  gtk_text_buffer_set_text (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), z, -1);

  g_free (z);
  g_free (buf); 
  g_strfreev (a);
}

void on_mni_recent_activate (GtkMenuItem *menuitem)
{
	icon_affiche_ok();
  execute_recent_item (gtk_widget_get_name (GTK_WIDGET(menuitem)));
}

void show_save_as_dlg (int mode)
{
  GtkWidget *file_dialog = gtk_file_chooser_dialog_new (_("Sauvegarde sous..."),
                                                        GTK_WINDOW(tea_main_window),
                                                        GTK_FILE_CHOOSER_ACTION_SAVE,
                                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                        GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                                                        NULL);

  gtk_file_chooser_set_local_only (GTK_FILE_CHOOSER (file_dialog), TRUE);

  t_ppair *p = g_malloc (sizeof (t_ppair));
  p->a = file_dialog;
  g_free (p);

  if (mode == 0)
     {
      if (cur_text_doc)
         {
          if (confile.use_def_save_dir)     
             gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (file_dialog),
                                                  confile.def_save_dir);
          else
              if (g_file_test (cur_text_doc->file_name, G_FILE_TEST_EXISTS))
                 gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (file_dialog), 
                                                cur_text_doc->file_name);
         }
     }

  if (mode == 2)
     gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (file_dialog),
                                          confile.snippets_dir);
  
  if (gtk_dialog_run (GTK_DIALOG (file_dialog)) == GTK_RESPONSE_ACCEPT)
    {
     gchar *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (file_dialog));

     if (confile.prompt_on_file_exists && g_file_test (filename, G_FILE_TEST_EXISTS))
     if (! dlg_question (tea_main_window, (_("%s il existe voulez vous le remplacer?")), filename))
        return;
 
     cur_text_doc->file_name = ch_str (cur_text_doc->file_name, filename);
     cur_text_doc->encoding = ch_str (cur_text_doc->encoding, cur_settings.selected_enc);  

     if (! text_doc_save (cur_text_doc, cur_text_doc->file_name))
        {
          icon_stop_logmemo();
          icon_affiche_nosave ();
         log_to_memo (_("error de la sauvegarde %s!!!"), cur_text_doc->file_name, LM_ERROR);
				statusbar_msg (_("Save [ERROR]"));
				griffon_notify(_("Save [ERROR]"));
         return;
        }
      
      cur_text_doc->b_saved = TRUE;
      gtk_label_set_label (GTK_LABEL(cur_text_doc->tab_label),cur_text_doc->file_name);
      set_title (cur_text_doc);

     if (strstr (cur_text_doc->file_name, confile.snippets_dir))
        reload_snippets();

     if (strstr (cur_text_doc->file_name, confile.templates_dir))
        reload_templates();

     tabs_reload ();
     g_free (filename);
    }

   gtk_widget_destroy (GTK_WIDGET(file_dialog));
}

void on_mni_file_save_as_template_activate (){  show_save_as_dlg (1);}

void on_mni_file_edit_at_cursor ()
{
  if (! get_page_text()) return;
  
  gchar *w = get_c_url (cur_text_doc);
  handle_file (w, 1);
  g_free (w);
}

void on_mni_new_link_select (GtkMenuItem *menuitem){  handle_file (gtk_widget_get_name (GTK_WIDGET(menuitem)), 0);}

void add_link_item (const gchar *fname, const gchar *linkname)
{
  gchar *dir = g_path_get_dirname (fname);
  gchar *filename = create_full_path (linkname, dir);

  if (g_file_test (filename, G_FILE_TEST_EXISTS))
      mni_temp = new_menu_item (filename, GTK_WIDGET(mni_links_menu), on_mni_new_link_select);

  g_free (dir);
  g_free (filename);
}
 
void scan_links (void)
{
  if (! get_page_text()) return;
  
  gchar *f;
  GtkTextIter match_start;
  GtkTextIter match_end;

  gtk_widget_destroy(GTK_WIDGET(mni_links_menu));
	 mni_links_menu = new_menu_submenu (GTK_WIDGET(mni_links));
  mni_temp = new_menu_item (cur_text_doc->file_name, GTK_WIDGET(mni_links_menu), on_mni_new_link_select);
  gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &match_start); 
  gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &match_end); 

  while (gtk_text_iter_forward_search (&match_start, "ref=\"", GTK_TEXT_SEARCH_TEXT_ONLY, &match_start, &match_end, NULL))
       {
        match_start = match_end;
        if (gtk_text_iter_forward_find_char (&match_end, (GtkTextCharPredicate)find_quote, NULL, NULL))   
           { 
            f = gtk_text_buffer_get_text (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &match_start, &match_end, FALSE);
            add_link_item (cur_text_doc->file_name, f);
            g_free (f);
           }
        match_start = match_end;
      }

  gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &match_start); 
  gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &match_end); 

  while (gtk_text_iter_forward_search (&match_start, "REF=\"", GTK_TEXT_SEARCH_TEXT_ONLY, &match_start, &match_end, NULL))
       {
        match_start = match_end;
        if (gtk_text_iter_forward_find_char (&match_end, (GtkTextCharPredicate)find_quote, NULL, NULL))   
           {
            f = gtk_text_buffer_get_text (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &match_start, &match_end, FALSE);
            add_link_item (cur_text_doc->file_name, f);
            g_free (f);
           }
        match_start = match_end;
       }
}

void on_mni_get_links ()
{
  scan_links ();
  icon_log_logmemo();
  log_to_memo (_("Maintenant, regardez dans le menu Navigation->Liens."), NULL, LM_ADVICE);
	statusbar_msg (_("Scan links HTML [OK]"));
}

void on_mni_co_select (GtkMenuItem *menuitem)
{
  if (! get_page_text()) return;
  
  if (! g_file_test (cur_text_doc->file_name, G_FILE_TEST_EXISTS))
     return;

  cur_settings.selected_enc = ch_str (cur_settings.selected_enc, gtk_widget_get_name (GTK_WIDGET(menuitem)));

  if (! doc_reload_text (cur_text_doc, cur_text_doc->file_name, cur_settings.selected_enc))
	{	  icon_stop_logmemo();
     log_to_memo (_("Non, je ne peux pas!"), NULL, LM_ERROR);}
  else{
      cur_text_doc->encoding = g_strdup (cur_settings.selected_enc);
      }
}

void on_mni_html_strip_tags ()
{
  if (! get_page_text()) return;
  
  gchar *buf = doc_get_sel (cur_text_doc);
  if (! buf) return;

  gchar *t = strip_html (buf);
  doc_rep_sel (cur_text_doc, t);

  g_free (buf);
  g_free (t);
}

void on_mni_markup_align (GtkMenuItem *menuitem)
{
  if (! get_page_text()) return;
  
  gchar *t="";
  gchar *buf = doc_get_sel (cur_text_doc);

  if (! buf)
     {
      if (strcmp (gtk_widget_get_name (GTK_WIDGET(menuitem)), "Centrer") == 0)
          doc_insert_at_cursor (cur_text_doc, "<div align=\"center\"></div>");
      else
      if (strcmp (gtk_widget_get_name (GTK_WIDGET(menuitem)), "Gauche") == 0)
          doc_insert_at_cursor (cur_text_doc, "<div align=\"left\"></div>");
      else
      if (strcmp (gtk_widget_get_name (GTK_WIDGET(menuitem)), "Droite") == 0)
          doc_insert_at_cursor (cur_text_doc, "<div align=\"right\"></div>");
      else
      if (strcmp (gtk_widget_get_name (GTK_WIDGET(menuitem)), "Justifier") == 0)
          doc_insert_at_cursor (cur_text_doc, "<div align=\"justify\"></div>");

      g_free (buf);
      doc_move_cursor_backw_middle_tags (cur_text_doc);  

      return;
     }
  else
     {  
      if (strcmp (gtk_widget_get_name (GTK_WIDGET(menuitem)), "Centrer") == 0)
          t = g_strconcat ("<div align=\"center\">", buf, "</div>", NULL);
      else
      if (strcmp (gtk_widget_get_name (GTK_WIDGET(menuitem)), "Gauche") == 0)
         t =  g_strconcat ("<div align=\"left\">", buf, "</div>", NULL);
      else
      if (strcmp (gtk_widget_get_name (GTK_WIDGET(menuitem)), "Droite") == 0)
         t =  g_strconcat ("<div align=\"right\">", buf, "</div>", NULL);
      else
      if (strcmp (gtk_widget_get_name (GTK_WIDGET(menuitem)), "Justifier") == 0)
         t =  g_strconcat ("<div align=\"justify\">", buf, "</div>", NULL);
      }
  
  doc_rep_sel (cur_text_doc, t);

  g_free (buf);
  g_free (t);
}

void on_mni_html_make_table ()
{
  if (! get_page_text()) return;
  
  set_fam_text ("2~2"); 
  gchar const *table = gtk_entry_get_text (GTK_ENTRY(ent_search));
   icon_man_logmemo();
	log_to_memo (_("[INFO] Vous poouvez indiquer le nombre de ligne (TR) et le nombre de colonne (TD) dans la ligne de commande\nExemple :2~2\nPour 2 TR et 2 TD\n\n"), NULL, LM_GREET);
	statusbar_msg (_("Make table HTML"));

  gchar **a = g_strsplit (table, "~", 3);

  if (! a)
     {
      return;
     }

  gint cols = 1;
  gint rows = 1;
  gint c;
  gint r;

  GList *l = NULL;
  l = g_list_prepend (l, strdup ("<table>"));  

  if (a[0])
     rows = strtol (a[0], NULL, 10);
  if (a[1])
     cols = strtol (a[1], NULL, 10);

  for (r = 1; r <= rows; r++)
      {
       l = g_list_prepend (l, strdup ("<tr>")); 
       for (c = 1; c <= cols; c++)
            l = g_list_prepend (l, strdup ("<td></td>")); 
        
       l = g_list_prepend (l, strdup ("</tr>")); 
      }

  l = g_list_prepend (l, strdup ("</table>"));  

  gchar *buf = string_from_glist (g_list_reverse (l));

  doc_insert_at_cursor (cur_text_doc, buf);

  glist_strings_free (l);
  g_strfreev (a);
  g_free (buf);
}

void on_mni_file_revert_to_saved ()
{
  if (! get_page_text()) return;
  
  if (! g_file_test (cur_text_doc->file_name, G_FILE_TEST_EXISTS))
     return;

  if (! doc_reload_text (cur_text_doc, cur_text_doc->file_name, cur_text_doc->encoding))
	{ icon_stop_logmemo();
    log_to_memo (_("Je ne peux pas recharger %s"), cur_text_doc->file_name, LM_ERROR);
    }
  else
  {
	  icon_ok_logmemo();
      log_to_memo (_("%s est rechargé"), cur_text_doc->file_name, LM_NORMAL);
      }
}

void on_mni_funx_ins_time ()
{
  if (! get_page_text()) return;
  
  gchar *buf = get_time (confile.date_time);
  doc_insert_at_cursor (cur_text_doc, buf);
  g_free (buf);
}

void on_mni_goto_line ()
{
  if (! get_page_text()) return;
  set_fam_text ("0");
  doc_select_line (cur_text_doc, strtol (gtk_entry_get_text GTK_ENTRY((ent_search)), NULL, 10));

				GtkTreeIter iter_entry;
      gtk_list_store_append(model_entry, &iter_entry);
      gtk_list_store_set(model_entry, &iter_entry, CONTACT_NAME, gtk_entry_get_text (GTK_ENTRY(ent_search)),  -1);
}

void on_mni_convert_ascii_2_html ()
{
  if (! get_page_text()) return;
  
  gchar *buf = doc_get_sel (cur_text_doc);
  if (! buf) return;

  gchar *t = g_markup_escape_text (buf, -1);
  doc_rep_sel (cur_text_doc, t);

  g_free (buf);
  g_free (t);
}

void on_mni_func_filter_exclude_from_list ()
{
  if (! get_page_text()) return;
  
  set_fam_text ("Chirurgus mente prius et oculis agat, quam armata manu");
  gchar *buf = doc_get_sel (cur_text_doc);

  if (! buf) return;

  GList *temp = glist_from_string (buf);
  GList *temp2 = filter_exclude_from_list (temp, gtk_entry_get_text (GTK_ENTRY(ent_search)));
  gchar *t = string_from_glist (temp2);

  doc_rep_sel (cur_text_doc, t);

  g_free (buf);
  g_free (t);
  glist_strings_free (temp);
  glist_strings_free (temp2);
}

void on_mni_func_filter_antiexclude_from_list ()
{
  if (! get_page_text()) return;
  
  set_fam_text ("Tantum possumus, quantum scimus.");  
  gchar const *s = gtk_entry_get_text (GTK_ENTRY(ent_search));
  gchar *buf = doc_get_sel (cur_text_doc);

  if (! buf) return;

  GList *temp = glist_from_string (buf);
  GList *temp2 = filter_antiexclude_from_list (temp, s);
  gchar *t = string_from_glist (temp2);

  doc_rep_sel (cur_text_doc, t);

  g_free (buf);
  g_free (t);
  glist_strings_free (temp);
  glist_strings_free (temp2);
}

GList *common_temp_list = NULL;

void on_mni_html_calc_weight ()
{
  if (! get_page_text()) return;
  
  if (! g_file_test (cur_text_doc->file_name, G_FILE_TEST_EXISTS))
     return;

  common_temp_list = NULL;
  
  gchar *f;
  GtkTextIter match_start;
  GtkTextIter match_end;

  gtk_widget_destroy (GTK_WIDGET(mni_links_menu));
  mni_links_menu = new_menu_submenu (GTK_WIDGET(mni_links));
  mni_temp = new_menu_item (cur_text_doc->file_name, GTK_WIDGET(mni_links_menu), on_mni_new_link_select);
  gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &match_start); 
  gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &match_end); 

  while (gtk_text_iter_forward_search (&match_start, "src=\"", GTK_TEXT_SEARCH_TEXT_ONLY, &match_start, &match_end, NULL))
       {
        match_start = match_end;
        if (gtk_text_iter_forward_find_char (&match_end,(GtkTextCharPredicate) find_quote, NULL, NULL))   
           {
            f = gtk_text_buffer_get_text (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &match_start, &match_end, FALSE);
            if (g_file_test (f, G_FILE_TEST_EXISTS)) 
                common_temp_list = g_list_append (common_temp_list, f);   
           } 
        match_start = match_end;
       }

  while (gtk_text_iter_forward_search (&match_start, "SRC=\"", GTK_TEXT_SEARCH_TEXT_ONLY, &match_start, &match_end, NULL))
       {
        match_start = match_end;
        if (gtk_text_iter_forward_find_char (&match_end,(GtkTextCharPredicate) find_quote, NULL, NULL))   
           {
            f = gtk_text_buffer_get_text (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &match_start, &match_end, FALSE);
            if ( g_file_test (f, G_FILE_TEST_EXISTS)) 
               common_temp_list = g_list_append (common_temp_list, f);
           } 
        match_start = match_end;
       }

  gint bytes_total = 0;
  gchar *t = NULL;
  gint x;

  GList *p = g_list_first (common_temp_list);

  while (p)
        {
         x = get_file_size ((gchar *) p->data);
         bytes_total = bytes_total + x;
         t = g_strdup_printf (_("%s = %d Kb"), (gchar *) p->data, (gint) x/1024);
         icon_list_logmemo();
         log_to_memo (t, NULL, LM_NORMAL);
         g_free (t);
         p = g_list_next (p);
        }

  x = get_file_size (cur_text_doc->file_name);
  bytes_total = bytes_total + x;
  t = g_strdup_printf (_("%s = %d Kb"), cur_text_doc->file_name, (gint) x/1024);
  log_to_memo (t, NULL, LM_NORMAL);
  g_free (t);     

  glist_strings_free (common_temp_list);

  t = g_strdup_printf (_("Total weight for %s is %d Kb"), cur_text_doc->file_name, bytes_total/1024);
  log_to_memo (t, NULL, LM_NORMAL);
  g_free (t);
}

void on_mni_get_src ()
{
  if (! get_page_text()) return;
  
  if (! g_file_test (cur_text_doc->file_name, G_FILE_TEST_EXISTS))
     return;

  gchar *f;
  GtkTextIter match_start;
  GtkTextIter match_end;

  gtk_widget_destroy(GTK_WIDGET(mni_links_menu));
  mni_links_menu = new_menu_submenu (GTK_WIDGET(mni_links));
  mni_temp = new_menu_item (cur_text_doc->file_name, GTK_WIDGET(mni_links_menu), on_mni_new_link_select);
  gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &match_start); 
  gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &match_end); 

  while (gtk_text_iter_forward_search (&match_start, "src=\"", GTK_TEXT_SEARCH_TEXT_ONLY, &match_start, &match_end, NULL))
       {
        match_start = match_end;
        if (gtk_text_iter_forward_find_char (&match_end,(GtkTextCharPredicate) find_quote, NULL, NULL))   
           {
            f = gtk_text_buffer_get_text (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &match_start, &match_end, FALSE);
            add_link_item (cur_text_doc->file_name, f);
            g_free (f);
           }
        match_start = match_end;
       }

  while (gtk_text_iter_forward_search (&match_start, "SRC=\"", GTK_TEXT_SEARCH_TEXT_ONLY, &match_start, &match_end, NULL))
       {
        match_start = match_end;
        if (gtk_text_iter_forward_find_char (&match_end,(GtkTextCharPredicate) find_quote, NULL, NULL))   
           {
            f = gtk_text_buffer_get_text (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &match_start, &match_end, FALSE);
            add_link_item (cur_text_doc->file_name, f);
            g_free (f); 
           }
        match_start = match_end;
       }
	icon_list_logmemo();
  log_to_memo (_("Maintenant, regardez dans le menu Navigation->Liens"), NULL, LM_ADVICE);
	statusbar_msg (_("Scan links [OK]"));
}

void select_color_cb ()
{
/***** A CORRIGER

  GdkColor color;
  GtkColorChooser *colorsel;
colorsel=gtk_color_selection_dialog_get_color_selection (user_data);
  gtk_color_selection_get_current_color (colorsel, &color);
  gchar *s = g_strdup_printf("#%02x%02x%02x", color.red / 256, color.green / 256, color.blue / 256);

  gchar *cm;
  gchar *t;
  gchar *buf = doc_get_sel (cur_text_doc);

  if (! buf)
     doc_insert_at_cursor (cur_text_doc, s);
  else
      {
       gchar *cm = g_strdup (confile.fmt_color_function);

       if (strstr (cm, "@color"))
          {
           t = g_str_replace (cm, "@color", s);
           g_free (cm);
           cm = t;
          }

       if (strstr (cm, "@text"))
          {
           t = g_str_replace (cm, "@text", buf);
           g_free (cm);
           cm = t;
          }

       doc_rep_sel (cur_text_doc, cm);
       g_free (cm);
       g_free (buf);
      }
	gtk_widget_destroy (GTK_WIDGET(user_data));
  g_free (s);*/
}

void select_color_cb_delete (gpointer user_data){	gtk_widget_destroy (GTK_WIDGET(user_data));}

void on_mni_markup_select_color_activate ()
{
  if (! get_page_text()) return;
  
/**** A CORRIGER

  dlg_colorsel = gtk_color_selection_dialog_new (_("Séléctionnez une couleur"));

GtkWidget *ok_button, *cancel_button;

g_object_get (dlg_colorsel,
	"ok-button", &ok_button,
	"cancel-button", &cancel_button,
	NULL);

  g_signal_connect (ok_button,
                    "clicked",
                    G_CALLBACK (select_color_cb),
                    (gpointer) dlg_colorsel);

  g_signal_connect (cancel_button,
                            "clicked",
                            G_CALLBACK (select_color_cb_delete),
                            (gpointer) dlg_colorsel);

  gtk_widget_show (dlg_colorsel);*/
}

void on_mni_file_backup ()
{
  if (! get_page_text()) return;
  do_backup (cur_text_doc->file_name, FALSE);
}

void on_mni_file_add_bookmark ()
{
  if (! get_page_text()) return;

  //bookmarks = add_recent_item_composed (bookmarks, cur_text_doc);
  //glist_save_to_file (bookmarks, confile.bmx_file);
  //bmx_reload ();
}

void on_mni_file_open_bookmark ()
{
  cur_settings.selected_enc = ch_str (cur_settings.selected_enc, "UTF-8");
  open_file_std (confile.bmx_file);
}

void on_mni_file_todo ()
{
  cur_settings.selected_enc = ch_str (cur_settings.selected_enc, "UTF-8");
  open_file_std (confile.tea_todo);
}

void on_mni_file_myadmin ()
{
  cur_settings.selected_enc = ch_str (cur_settings.selected_enc, "UTF-8");
  open_file_std (confile.tea_myadmin);
}

void on_mni_real_recent_activate (GtkMenuItem *menuitem)
{
  execute_recent_item (gtk_widget_get_name (GTK_WIDGET(menuitem)));

  GList *p = find_item_by_data (recent_list, gtk_widget_get_name (GTK_WIDGET(menuitem)));
  if (p)
     {
      g_free (p->data);
      recent_list = g_list_delete_link (recent_list, p);
      update_recent_list_menu (FALSE);
     }
}

void on_mni_refresh_hl ()
{
  if (! get_page_text()) return;
  clear_remove_tags (cur_text_doc);
}

void on_mni_make_numbering ()
{
  if (! get_page_text()) return;
  set_fam_text ("1~5~10");
  gchar const *s = gtk_entry_get_text (GTK_ENTRY(ent_search));
  
  gchar *buf = doc_get_sel (cur_text_doc);
  if (buf) //exit if selected
     {
      g_free (buf);
      return;
     }

  gchar **a = g_strsplit (s, "~", 4);

  if (! a) return;

  gint c;
  gint x = 0;
  gint y = 0;
  gint step = 1;

  if (a[0])
     {
      x = strtol (a[0], NULL, 10);
      if (a[1])
         {
          y = strtol (a[1], NULL, 10);
          if (a[2])
              step = strtol (a[2], NULL, 10);
         }  
     }

  gchar *counter = NULL;
  GList *list = NULL;

  for (c = x; c <= y; (c++))
      {
       counter = g_strdup_printf ("%d", (c * step));
       list = g_list_append (list, counter);
      }

  gchar *t = string_from_glist (list);
  doc_insert_at_cursor (cur_text_doc, t);

  g_free (t);
  glist_strings_free (list);
}

void on_mni_nav_block_start ()
{
  if (! get_page_text()) return;

  GtkTextIter iter;
  gint c = 0;

  GtkTextMark *m = gtk_text_buffer_get_insert (GTK_TEXT_BUFFER(cur_text_doc->text_buffer)); 
  gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &iter, m);

  while (c != -1)
        {
         if (gtk_text_iter_backward_char (&iter))
						{
         if (gtk_text_iter_get_char (&iter) == '}')
            c++;
         else
             if (gtk_text_iter_get_char (&iter) == '{')
                {
                 c--;
                 if (c == -1)
                 if (gtk_text_iter_forward_char (&iter))
                    {
                     gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &iter );
                     gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW(cur_text_doc->text_view), &iter, 0.0, FALSE, 0.0, 0.0 );
                    }
                }
        }
					}
}

void on_mni_nav_block_end ()
{
  if (! get_page_text()) return;

  GtkTextIter iter;
  gint c = 0;

  GtkTextMark *m = gtk_text_buffer_get_insert (GTK_TEXT_BUFFER(cur_text_doc->text_buffer)); 
  gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &iter, m);

  while (c != -1)
        {
         if (gtk_text_iter_forward_char (&iter))
						{
         if (gtk_text_iter_get_char (&iter) == '{')
            c++;
            else
                if (gtk_text_iter_get_char (&iter) == '}')
                   {
                    c--;
                    if (c == -1)
                       {
                        gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &iter );
                        gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW(cur_text_doc->text_view), &iter, 0.0, FALSE, 0.0, 0.0 );
                       }
                   }
        }
					}
}

void on_mni_open_hotkeys ()
{
  cur_settings.selected_enc = ch_str (cur_settings.selected_enc, "UTF-8");
  open_file_std (confile.tea_hotkeys);
}

enum
{
  TARGET_URILIST
};

static const GtkTargetEntry drag_types[] = {
                                            {"text/uri-list", 0, TARGET_URILIST}
                                           };

void open_files_list (const gchar *s)
{
  GList *l = glist_from_string (s);
  if (! l)
     return;

  gchar *f;
  gchar *z;  

  GList *t = g_list_first (l);
  while (t && t->data)
        {
         g_free (cur_settings.selected_enc);

         cur_settings.selected_enc = g_strdup (confile.default_charset);

         gchar *x = strchr ((gchar *) t->data, '/'); 
         if (x)
            f = g_strdup (x);
          else 
              f = g_strdup ((gchar *) t->data);
       
         if (f)
            {
             g_strchomp (f);
             if (strstr (f, "%20"))
                {
                 z = str_replace_all (f, "%20", " "); 
                 g_free (f);
                 f = z; 
                }

             if (is_image (f))
                {
                 if (get_page_text ())   
                    insert_image (f); 
                }
             else
                 open_file_std (f);

             g_free (f);
            }

         t = g_list_next (t);
        }

   glist_strings_free (l);
}

/*void tea_win_on_drag_data_cb ( GtkSelectionData *data, guint time) 
{
  if (inner_dnd_switch == 1)
     {
      inner_dnd_switch = 0;
      return;
     } 

  gchar *filename;
  inner_dnd_switch++;   
 
  open_files_list (filename);  

  gtk_drag_finish (context, TRUE, TRUE, time);
  g_free (filename);
}

void set_dnd_accept (GtkWidget *widget)
{
  gtk_drag_dest_set (widget, (GTK_DEST_DEFAULT_ALL),
                     drag_types, 1,
                    (GDK_ACTION_DEFAULT | GDK_ACTION_COPY | GDK_ACTION_MOVE));

  g_signal_connect (G_OBJECT (widget), "drag_data_received", G_CALLBACK (tea_win_on_drag_data_cb), widget);
}
*/

gboolean on_tea_window_delete_event ()
{
  ui_done ();
  return FALSE;
}

void on_mni_snippet_click (GtkMenuItem *menuitem)
{
  if (! get_page_text()) return;

  gchar *f = g_strconcat (confile.snippets_dir, gtk_widget_get_name (GTK_WIDGET(menuitem)), NULL);

  gsize length;
  GError *error;
  gchar *buf;
  gchar *t;
 
  if (! g_file_get_contents (f, &buf, &length, &error))
     {
      g_free (f); 
      return;
     }
  else
      if (! g_utf8_validate (buf, -1, NULL))
         {
          g_free (buf);
          g_free (f);
          return;
         }

  gchar *sel = doc_get_sel (cur_text_doc);

  if (! sel)
     doc_insert_at_cursor (cur_text_doc, buf);
  else      
  if (sel)
     {
      if (strstr (buf, "%s"))
        {
         t = rep_all_s (buf, sel);
         doc_rep_sel (cur_text_doc, t); 
         g_free (t);
         g_free (sel);
        }
     }
     else
         doc_insert_at_cursor (cur_text_doc, buf);
  
  g_free (f);
  g_free (buf);
}

void on_mni_snippet_file_open_activate ()
{
  GtkWidget *dialog = gtk_file_chooser_dialog_new (_("Open file:"),
                                                   GTK_WINDOW(tea_main_window),
                                                   GTK_FILE_CHOOSER_ACTION_OPEN,
                                                   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                   GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                                   NULL);

  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), confile.snippets_dir);

  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
     {
      gchar *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
      cur_settings.selected_enc = ch_str (cur_settings.selected_enc, "UTF-8");
      open_file_std (filename);
      g_free (filename);
     }

  gtk_widget_destroy (GTK_WIDGET(dialog));
}

void on_mni_file_save_as_snippet_activate (){show_save_as_dlg (2);}

static GList *gl_menu_item_names;

static void lookup_widget_cb_2 (GtkWidget *widget)
{
  if (! widget) return;
   
  gchar const *s = gtk_widget_get_name (GTK_WIDGET(widget));
  if (strcmp ("-x-", s) != 0)
     {
      gl_menu_item_names = g_list_append (gl_menu_item_names, (gpointer)s);

      if (GTK_IS_MENU_ITEM (widget))
         lookup_widget2 (GTK_CONTAINER(gtk_menu_item_get_submenu (GTK_MENU_ITEM(widget))));      
     }   
}

GtkWidget* lookup_widget2 (GtkContainer *widget)
{
  if (widget)
     gtk_container_foreach (widget, (GtkCallback)lookup_widget_cb_2, NULL);
  return NULL;
}

void on_mni_dump_menu ()
{
  cur_text_doc = doc_clear_new ();
  if (! get_page_text()) return;
   
  g_list_free (gl_menu_item_names);
  lookup_widget2 (GTK_CONTAINER (menubar1));
   
  gchar *t = string_from_glist (gl_menu_item_names); 

  doc_insert_at_cursor (cur_text_doc, t);
  g_free (t);
  g_list_free (gl_menu_item_names);
}

GtkWidget* create_wnd_imgviewer (gchar const *f)
{
  GtkWidget *wnd_imgviewer = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  g_signal_connect ((gpointer) wnd_imgviewer, "destroy_event",
                    G_CALLBACK (gtk_false),
                    NULL);

  GtkWidget *image = gtk_image_new_from_file (f);
  gtk_container_add (GTK_CONTAINER (wnd_imgviewer), image);
  gtk_widget_show (GTK_WIDGET(image));
  gtk_widget_show (GTK_WIDGET(wnd_imgviewer));
  gchar *t = g_path_get_basename (f);
  gtk_window_set_title (GTK_WINDOW(wnd_imgviewer), t);
  g_free (t);
  return wnd_imgviewer;
}

void on_mni_nav_focus_to_famous ()
{ 
	if (! get_page_text()) return;
	gtk_entry_set_text (GTK_ENTRY(ent_search),"");
	gtk_entry_set_text (GTK_ENTRY(ent_search), doc_get_sel (cur_text_doc)); 
  gtk_widget_grab_focus (GTK_WIDGET(ent_search));
}

void on_mni_nav_focus_to_text ()
{
  if (! get_page_text()) return;
  gtk_widget_grab_focus (GTK_WIDGET(cur_text_doc->text_view));    
}

void on_mni_tools_unitaz_sort ()
{
  if (! get_page_text()) return;
  run_unitaz (cur_text_doc, 0, FALSE);
}

void on_mni_tools_unitaz_sort_by_count ()
{
  if (! get_page_text()) return;
  run_unitaz (cur_text_doc, 1, FALSE);
}

void on_mni_tools_unitaz_plain ()
{
  if (! get_page_text()) return;
  run_unitaz (cur_text_doc, -1,TRUE);
}

void on_mni_str_kill_dups ()
{
  if (! get_page_text()) return;

  gchar *buf = doc_get_sel (cur_text_doc);
  if (! buf) return;

  gchar *t = str_kill_dups (buf);
  doc_rep_sel (cur_text_doc, t);

  g_free (buf);
  g_free (t);
}

void on_mni_func_strings_sort ()
{
  if (! get_page_text()) return;

  gchar *buf = doc_get_sel (cur_text_doc);

  if (! buf) return;

  GList *temp = glist_from_string (buf);
  temp = glist_strings_sort (temp);
  gchar *t = string_from_glist (temp);

  doc_rep_sel (cur_text_doc, t);

  g_free (buf);
  g_free (t);
  glist_strings_free (temp);
}

void on_mni_extract_words ()
{
  if (! get_page_text()) return;
  run_extract_words (cur_text_doc);
}

void on_mni_edit_copy_all ()
{
icon_affiche_ok();
  if (! get_page_text()) return;
  
  GtkClipboard *c = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
  if (! c)
     return; 

  gchar *buf = doc_get_buf (GTK_TEXT_BUFFER(cur_text_doc->text_buffer));

  if (buf)
     gtk_clipboard_set_text (c, buf, -1);

  g_free (buf);
}

void on_mni_edit_copy_current_url ()
{
  if (! get_page_text()) return;
  
  GtkClipboard *c = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
  if (! c)
     return; 

  gchar *buf = get_c_url_pure (cur_text_doc);

  if (buf)
     gtk_clipboard_set_text (c, buf, -1);

  g_free (buf);
}

void on_mni_edit_replace_with_clipboard ()
{
  if (! get_page_text()) return;
  
  GtkClipboard *c = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
  if (! c)
     return; 

  if (! gtk_clipboard_wait_is_text_available (c))
     return;

  gchar *buf = doc_get_sel (cur_text_doc);
  if (! buf) return;

  gchar *t = gtk_clipboard_wait_for_text (c); 
 
  doc_rep_sel (cur_text_doc, t);
  gtk_clipboard_set_text (c, buf, -1);

  g_free (buf);
  g_free (t);
}

void on_mni_count_string_list ()
{
  if (! get_page_text()) return;

  gchar *buf = doc_get_sel (cur_text_doc);
  if (! buf) return;

  set_fam_text ("%d.)%s");  
  
  gchar *t = glist_enum (buf, gtk_entry_get_text (GTK_ENTRY(ent_search)));

  if (t)
     doc_rep_sel (cur_text_doc, t);

  g_free (buf);
  g_free (t);
}

void on_mni_paste_to_new ()
{
	icon_affiche_ok();
  GtkClipboard *c = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
  if (! c)
     return; 

  if (! gtk_clipboard_wait_is_text_available (c))
     return;

  cur_text_doc = doc_clear_new ();
  gtk_window_set_title (GTK_WINDOW (tea_main_window), cur_text_doc->file_name);
  gtk_widget_grab_focus (GTK_WIDGET(cur_text_doc->text_view));

  gchar *t = gtk_clipboard_wait_for_text (c); 
  doc_insert_at_cursor (cur_text_doc, t);
  g_free (t); 
}

void on_mni_copy_to_new ()
{
	icon_affiche_ok();
  if (! get_page_text()) return;

  gchar *t = doc_get_sel (cur_text_doc);
  if (! t)
     return;  
 
  cur_text_doc = doc_clear_new ();
  gtk_window_set_title (GTK_WINDOW (tea_main_window), cur_text_doc->file_name);
  gtk_widget_grab_focus (GTK_WIDGET(cur_text_doc->text_view));

  doc_insert_at_cursor (cur_text_doc, t);
  g_free (t); 
}

void on_mni_cut_to_new ()
{
	icon_affiche_ok();
  if (! get_page_text()) return;

  gchar *t = doc_get_sel (cur_text_doc);
  if (! t)
     return;  
 
  doc_rep_sel (cur_text_doc, "");
  doc_ins_to_new (t);
  g_free (t); 
}

void on_mni_shuffle_strings ()
{
  if (! get_page_text()) return;

  gchar *buf = doc_get_sel (cur_text_doc);

  if (! buf) return;

  GList *temp = glist_from_string (buf);
  temp = glist_shuffle (temp);
  gchar *t = string_from_glist (temp);
  doc_rep_sel (cur_text_doc, t);

  g_free (buf);
  g_free (t);
  glist_strings_free (temp);
}

void on_mni_tools_unitaz_sort_caseinsens ()
{
  if (! get_page_text()) return;
  run_unitaz (cur_text_doc, 0, TRUE);
}

void on_mni_tools_unitaz_sort_by_count_caseinsens ()
{
  if (! get_page_text()) return;
  run_unitaz (cur_text_doc, 1, TRUE);
}

void on_mni_cure ()
{
  log_to_memo (_("That is the question!"), NULL, LM_ERROR);  
}

void on_mni_tabs_to_spaces ()
{
  if (! get_page_text()) return;
  set_fam_text ("1");
  doc_tabs_to_spaces (cur_text_doc, strtol (gtk_entry_get_text (GTK_ENTRY(ent_search)), NULL, 10));  
}

void on_mni_spaces_to_tabs ()
{
  if (! get_page_text()) return;
  set_fam_text ("2");
  doc_spaces_to_tabs (cur_text_doc, strtol (gtk_entry_get_text (GTK_ENTRY(ent_search)), NULL, 10));  
}

void on_mni_edit_delete_current_line ()
{
icon_affiche_ok();
  if (! get_page_text()) return;
  
  GtkTextIter ittemp;
  GtkTextIter itend;

  GtkTextMark *mark = gtk_text_buffer_get_insert (GTK_TEXT_BUFFER(cur_text_doc->text_buffer));
 
  gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &ittemp, mark);

  itend = ittemp;

  if (gtk_text_iter_forward_to_line_end (&itend))
     while (! gtk_text_iter_starts_line (&ittemp))
           gtk_text_iter_backward_char (&ittemp); 

  if (gtk_text_iter_forward_char (&itend))
     gtk_text_buffer_delete (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &ittemp, &itend);

}

void on_mni_Markup_comment ()
{
  if (! get_page_text()) return;

  gchar *temp = NULL;
  gchar *buf = doc_get_sel (cur_text_doc);

  if (is_markup (cur_text_doc->file_name))
    {
     if (! buf)
        doc_insert_at_cursor (cur_text_doc, "<!-- -->");
     else
         {
          temp = g_strconcat ("<!-- ", buf, " -->", NULL);
          doc_rep_sel (cur_text_doc, temp);
          g_free (temp);
         }
  
     g_free (buf);
     return;
    }

  if (is_c (cur_text_doc->file_name) || is_css (cur_text_doc->file_name) || is_php (cur_text_doc->file_name))
     {
      if (! buf)
        doc_insert_at_cursor (cur_text_doc, "/* */");
      else
          {
           temp = g_strconcat ("/*", buf, "*/", NULL);
           doc_rep_sel (cur_text_doc, temp);
           g_free (temp);
          }

      g_free (buf);
      return; 
     } 
 
  if (is_pascal (cur_text_doc->file_name))
     {
      if (! buf)
         doc_insert_at_cursor (cur_text_doc, "{ }");
      else
          {
           temp = g_strconcat ("{", buf, "}", NULL);
           doc_rep_sel (cur_text_doc, temp);
           g_free (temp);
          }

      g_free (buf);
      return; 
     } 
}

void edit_paste (void)
{
icon_affiche_ok();
  if (! get_page_text()) return;
  
  GtkClipboard *c = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
  if (! c)
     return; 

  if (! gtk_clipboard_wait_is_text_available (c))
     return;

  gchar *t = gtk_clipboard_wait_for_text (c); 
  doc_insert_at_cursor (cur_text_doc, t);
  g_free (t); 
}

void on_mni_paste (){edit_paste ();}

void edit_copy (void)
{
  if (! get_page_text()) return;
  
  GtkClipboard *c = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
  if (! c)
     return; 

  gchar *buf = doc_get_sel  (cur_text_doc);

  if (buf)
     gtk_clipboard_set_text (c, buf, -1);

  g_free (buf);
}

void on_mni_edit_copy (){  edit_copy ();}

void edit_cut (void)
{
icon_affiche_ok();
  if (! get_page_text()) return;
  
  GtkClipboard *c = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
  if (! c)
     return; 

  gchar *buf = doc_get_sel (cur_text_doc);

  if (buf)
     {
      gtk_clipboard_set_text (c, buf, -1);
      doc_rep_sel (cur_text_doc, "");
     }
      
  g_free (buf);
}

void on_mni_edit_cut (){icon_affiche_ok();  edit_cut ();}

void on_mni_edit_delete ()
{
icon_affiche_ok();
  if (! get_page_text()) return;
  doc_rep_sel (cur_text_doc, "");
}

void on_mni_edit_select_all ()
{
  if (! get_page_text()) return;

  GtkTextIter start_iter;
  GtkTextIter end_iter;

  gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &start_iter);
  gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &end_iter);

  gtk_text_buffer_move_mark_by_name (GTK_TEXT_BUFFER(cur_text_doc->text_buffer),
                                     "insert",
                                     &start_iter);

  gtk_text_buffer_move_mark_by_name (GTK_TEXT_BUFFER(cur_text_doc->text_buffer),
                                     "selection_bound",
                                     &end_iter);
}

void on_mni_sort_case_insensetive ()
{
  if (! get_page_text()) return;

  gchar *buf = doc_get_sel (cur_text_doc);

  if (! buf) return;
  
  GList *temp = glist_from_string (buf);
  temp = sort_list_case_insensetive (temp);
  gchar *t = string_from_glist (temp);

  doc_rep_sel (cur_text_doc, t);

  g_free (buf);
  g_free (t);
  glist_strings_free (temp);
}

void on_mni_kill_formatting ()
{
  if (! get_page_text()) return;
  
  gchar *buf = doc_get_sel (cur_text_doc);

  if (buf)
     {
      gchar *temp = kill_formatting (buf);
      doc_rep_sel (cur_text_doc, temp);
      g_free (buf);
      g_free (temp);
     }
}

void on_mni_wrap_raw ()
{
  if (! get_page_text()) return;

  set_fam_text ("72");
  
  gchar *buf = doc_get_sel (cur_text_doc);
  if (buf)
     {
      gchar *temp = wrap_raw (buf, strtol (gtk_entry_get_text (GTK_ENTRY(ent_search)), NULL, 10));
      doc_rep_sel (cur_text_doc, temp);
      g_free (buf);
      g_free (temp);
     }
}

void on_mni_wrap_on_spaces ()
{
  if (! get_page_text()) return;

  set_fam_text ("72");
  
  gchar *buf = doc_get_sel (cur_text_doc);
  if (buf)
     {
      wrap_on_spaces (buf, strtol (gtk_entry_get_text (GTK_ENTRY(ent_search)), NULL, 10));
      doc_rep_sel (cur_text_doc, buf);
      g_free (buf);
     }
}

void on_doit_button ()
{
  if (! get_page_text())
     return;
  doc_search_f (cur_text_doc, gtk_entry_get_text (GTK_ENTRY(ent_search)));
}

void on_mni_kill_formatting_on_each_line ()
{
  if (! get_page_text()) return;
  
  gchar *buf = doc_get_sel (cur_text_doc);

  if (buf)
     {
      gchar *temp = kill_formatting_on_each_line (buf);
      doc_rep_sel (cur_text_doc, temp);
      g_free (buf);
      g_free (temp);
     }
}

void on_mni_file_crapbook ()
{

  if (! g_file_test (confile.crapbook_file, G_FILE_TEST_EXISTS))
     create_empty_file (confile.crapbook_file, _((_("Déposez vos séléctions ici..."))));

		gtk_text_buffer_set_text(GTK_TEXT_BUFFER(buffer_note), "", -1);
		gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_note), (_("\nPour afficher les notes [ALT+M]\n")), -1);
		gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_note), (_("Pour integrer une séléction dans la note [CTR+E]\n\n____________________________________________________________________________________________________________________________________________________\n\n")), -1);
				
		//**************** OUVERTURE DU FICHIER PROJET
		gchar lecture[1024];
		FILE *fichier;
		fichier = fopen(confile.crapbook_file,"rt");
		while(fgets(lecture, 1024, fichier))
		{
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_note),g_locale_to_utf8(lecture, -1, NULL, NULL, NULL) , -1);
		}

		fclose(fichier);
	statusbar_msg (_("Open Crapbook [OK]"));
 handle_file_enc (confile.crapbook_file, 0);
}

void on_mni_spellcheck (GtkMenuItem *menuitem)
{
  if (! get_page_text()) return;
  do_hl_spell_check2 (cur_text_doc, gtk_widget_get_name (GTK_WIDGET(menuitem)));
}

void on_mni_filter_kill_lesser ()
{
  if (! get_page_text()) return;
  
  set_fam_text ("2");
   
  gchar *buf = doc_get_sel (cur_text_doc);

  if (buf)
     {
      gchar *t = kill_lesser (buf, strtol (gtk_entry_get_text (GTK_ENTRY(ent_search)), NULL, 10));
      doc_rep_sel (cur_text_doc, t);
      g_free (buf);
      g_free (t);
     }
}

void on_mni_filter_kill_more_than ()
{
  if (! get_page_text()) return;
  
  set_fam_text ("2");
   
  gchar *buf = doc_get_sel (cur_text_doc);

  if (buf)
     {
      gchar *t = kill_larger (buf, strtol (gtk_entry_get_text (GTK_ENTRY(ent_search)), NULL, 10));
      doc_rep_sel (cur_text_doc, t);
      g_free (buf);
      g_free (t);
     }
}

void on_mni_func_strings_sort_by_q ()
{
  if (! get_page_text()) return;

  gchar *buf = doc_get_sel (cur_text_doc);

  if (! buf) return;

  GList *temp = glist_from_string (buf);
  temp = glist_word_sort_mode (temp, 2);
  gchar *t = string_from_glist (temp);

  doc_rep_sel (cur_text_doc, t);

  g_free (buf);
  g_free (t);
  glist_strings_free (temp);
}

void on_mni_tabs_menuitem_click (gpointer user_data)
{
  t_note_page *doc = user_data;
  if (doc) 
     handle_file (doc->file_name, 0);
}

gchar* get_full_fname (const gchar *fname, const gchar *linkname)
{
  gchar *dir = g_path_get_dirname (fname);
  gchar *filename = create_full_path (linkname, dir);
  g_free (dir);
  return filename;
}

void on_mni_show_images_in_text ()
{
  if (! get_page_text()) return;
  
  if (! g_file_test (cur_text_doc->file_name, G_FILE_TEST_EXISTS))
     return;

  gchar *f;
  gchar *t;
  GtkWidget *image; 
  GdkPixbuf *p;

  GtkTextIter match_start;
  GtkTextIter match_end;

  gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &match_start); 
  gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &match_end); 

  while (gtk_text_iter_forward_search (&match_start, "src=\"", GTK_TEXT_SEARCH_TEXT_ONLY, &match_start, &match_end, NULL))
       {
        match_start = match_end;
        if (gtk_text_iter_forward_find_char (&match_end,(GtkTextCharPredicate) find_quote, NULL, NULL))   
           {
            f = gtk_text_buffer_get_text (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &match_start, &match_end, FALSE);
            
            t = get_full_fname (cur_text_doc->file_name, f);    
            if (t)
            if (is_image (t))
               {
                p = gdk_pixbuf_new_from_file_at_size (t, confile.thumb_width, confile.thumb_height, NULL);
                image = gtk_image_new_from_pixbuf (p);  
                gtk_widget_show (GTK_WIDGET(image));
                gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(cur_text_doc->text_buffer),
                                               &match_end,
                                               gtk_image_get_pixbuf ((GtkImage *)image));
                gtk_text_buffer_set_modified (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), FALSE);  
                gtk_widget_destroy (GTK_WIDGET(image)); 
               }

            g_free (f);
            g_free (t);
           }
        match_start = match_end;
       }

  while (gtk_text_iter_forward_search (&match_start, "SRC=\"", GTK_TEXT_SEARCH_TEXT_ONLY, &match_start, &match_end, NULL))
       {
        match_start = match_end;
        if (gtk_text_iter_forward_find_char (&match_end,(GtkTextCharPredicate) find_quote, NULL, NULL))   
           {
            f = gtk_text_buffer_get_text (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &match_start, &match_end, FALSE);
            t = get_full_fname (cur_text_doc->file_name, f);    
            if (t)
            if (is_image (t))
               {
                image = gtk_image_new_from_file (t);
                gtk_widget_show (GTK_WIDGET(image));
                gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(cur_text_doc->text_buffer),
                                               &match_end,
                                               gtk_image_get_pixbuf ((GtkImage *)image));

                gtk_widget_destroy (GTK_WIDGET(image)); 
               }

            g_free (f);
            g_free (t);  
           }
        match_start = match_end;
       }
}

void on_mni_nav_goto_recent_tab ()
{
  gint t = gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook1));

  if (last_page != -1)
      gtk_notebook_set_current_page (GTK_NOTEBOOK(notebook1), last_page);

  last_page = t;
}

void on_mni_nav_goto_selection ()
{
  last_page = gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook1));
  gtk_notebook_prev_page (GTK_NOTEBOOK(notebook1));
}

void on_mni_nav_goto_prev_tab ()
{
  last_page = gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook1));
  gtk_notebook_prev_page (GTK_NOTEBOOK(notebook1));
}

void on_mni_nav_goto_next_tab ()
{
  last_page = gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook1));
  gtk_notebook_next_page (GTK_NOTEBOOK(notebook1));
}

void on_mni_view_hide_highlighting ()
{
  if (! get_page_text()) return;
  remove_tags (cur_text_doc);
}

void on_mni_nav_goto_first_tab ()
{
  last_page = gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook1));
  gtk_notebook_set_current_page (GTK_NOTEBOOK(notebook1), 0);
}

void on_mni_nav_goto_last_tab ()
{
  last_page = gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook1));
  gtk_notebook_set_current_page (GTK_NOTEBOOK(notebook1), -1);
}

void on_mni_quest_find ()
{
  if (! get_page_text()) return;
  doc_search_f (cur_text_doc, gtk_entry_get_text (GTK_ENTRY(ent_search)));

				GtkTreeIter iter_entry;
      gtk_list_store_append(model_entry, &iter_entry);
      gtk_list_store_set(model_entry, &iter_entry, CONTACT_NAME, gtk_entry_get_text GTK_ENTRY((ent_search)),  -1);
}

void on_mni_quest_find_next ()
{
  if (! get_page_text()) return;
  doc_search_f_next (cur_text_doc);
}

void on_mni_quest_find_ncase ()
{
  if (! get_page_text()) return;
  doc_search_f_ncase (cur_text_doc, gtk_entry_get_text (GTK_ENTRY(ent_search)));
}

void on_mni_quest_find_next_ncase ()
{
  if (! get_page_text()) return;
  doc_search_f_next_ncase (cur_text_doc);
}

void on_mni_user_menu_item (GtkMenuItem *menuitem)
{
  gchar *t = g_hash_table_lookup (ht_user_menu, gtk_widget_get_name (GTK_WIDGET(menuitem)));
  if (! t) 
     return;

  gchar *s = g_strdup (t);
  gchar *cmd = NULL;

  if (get_page_text()) 
     {
      cmd = str_replace_all (s, "%s", cur_text_doc->file_name);       
      int systemRet =system (cmd);
				if(systemRet == -1){return;}
      g_free (cmd);
     }

  g_free (s);
}

void on_mni_user_menu_open ()
{
  if (! g_file_test (confile.user_menu_file, G_FILE_TEST_EXISTS))
     create_empty_file (confile.user_menu_file, "kwrite=kwrite %s &");

  cur_settings.selected_enc = ch_str (cur_settings.selected_enc, "UTF-8");
  open_file_std (confile.user_menu_file);
}

void on_mni_find_and_replace_wnd ()
{
  if (! get_page_text()) return;

  create_fr ();
}

void on_mni_rtfm (){run_doc_in_browser ();}

void on_mni_crackerize ()
{
  if (! get_page_text()) return;

  gchar *t = doc_get_sel (cur_text_doc);

  if (! t)
     return;

  gchar *x = str_crackerize (t);
  doc_rep_sel (cur_text_doc, x);

  g_free (t);
  g_free (x);
}

void on_mni_markup_stuff (GtkMenuItem *menuitem) 
{
  if (! get_page_text()) return;
  
  gchar const *n = gtk_widget_get_name (GTK_WIDGET(menuitem));

  gchar *t = NULL;
  gchar *buf = doc_get_sel (cur_text_doc);

  if (! buf)
     {
      t = g_strconcat ("<", n, "></", n, ">", NULL);
      doc_insert_at_cursor (cur_text_doc, t);
      doc_move_cursor_backw_middle_tags (cur_text_doc);
     } 
  else
      {
       t = g_strconcat ("<", n, ">", buf, "</", n, ">", NULL);
       doc_rep_sel (cur_text_doc, t);
      }

  g_free (t);
  g_free (buf);
}

void show_options (void){  wnd_options_create ();}

void on_mni_show_options (){  wnd_options_create ();}

void on_mni_eol_to_crlf ()
{
  if (! get_page_text()) return;
  
  gchar *buf = doc_get_buf (GTK_TEXT_BUFFER(cur_text_doc->text_buffer));
  gchar *z = str_eol_to_crlf (buf);

  gtk_text_buffer_set_text (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), z, -1);

  g_free (z);
  g_free (buf); 
}

void on_mni_eol_to_lf ()
{
  if (! get_page_text()) return;
  
  gchar *buf = doc_get_buf (GTK_TEXT_BUFFER(cur_text_doc->text_buffer));
  gchar *z = str_replace_all (buf, "\r\n", "\n");
  
  gtk_text_buffer_set_text (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), z, -1);

  g_free (z);
  g_free (buf); 
  
}

void on_mni_templ_file_open_activate ()
{
  GtkWidget *dialog = gtk_file_chooser_dialog_new (_("Open file:"),
                                                   GTK_WINDOW(tea_main_window),
                                                   GTK_FILE_CHOOSER_ACTION_OPEN,
                                                   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                   GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                                   NULL);

  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), confile.templates_dir);

  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
     {
     gchar *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
     cur_settings.selected_enc = ch_str (cur_settings.selected_enc, "UTF-8");
     open_file_std (filename);
     g_free (filename);
    }

  gtk_widget_destroy (GTK_WIDGET(dialog));
}

void on_mni_upcase_each_first_letter_ofw ()
{
  if (! get_page_text()) return;
  
  gchar *z = upcase_each_first_letter_ofw (cur_text_doc);
  if (z)
     doc_rep_sel (cur_text_doc, z);

  g_free (z);
}

void on_mni_nav_mplayer_jump ()
{
  if (! confile.cm_mplayer)
     return;    

  if (! get_page_text()) 
     return;

  if (! confile.current_movie)
     {
      log_to_memo (_("Before using that, go to File > Open different > Open movie."), NULL, LM_ERROR);
      return;
     }   

  GtkTextIter iter;
  GtkTextMark *m = gtk_text_buffer_get_insert (GTK_TEXT_BUFFER(cur_text_doc->text_buffer));
  gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &iter, m);

  GtkTextIter end;
  GtkTextIter start;

  if (! gtk_text_iter_forward_search (&iter, "\n\n", GTK_TEXT_SEARCH_TEXT_ONLY, &end, NULL, NULL))
     return; 

  if (! gtk_text_iter_backward_search (&end, "-->", GTK_TEXT_SEARCH_TEXT_ONLY, &iter, NULL,NULL))
     return;


  if (! gtk_text_iter_backward_search (&iter, "\n", GTK_TEXT_SEARCH_TEXT_ONLY, &start, NULL, NULL))
     return;

  gchar *t = gtk_text_iter_get_text (&start, &iter);
  if (! t)
     return;

  t = g_strstrip (t);

  gchar *s;

  gchar *cm = g_strdup (confile.cm_mplayer);

  if (strstr (cm, "@encoding"))
     {
      s = g_str_replace (cm, "@encoding", cur_text_doc->encoding);
      g_free (cm);
      cm = s;
     }

  if (strstr (cm, "@time"))
     {
      s = g_str_replace (cm, "@time", t);
      g_free (cm);
      cm = s;
     }

  if (strstr (cm, "@sub_file"))
     {
      s = g_str_replace (cm, "@sub_file", cur_text_doc->file_name);
      g_free (cm);
      cm = s;
     }

  if (strstr (cm, "@movie_file"))
     {
      s = g_str_replace (cm, "@movie_file", confile.current_movie);
      g_free (cm);
      cm = s;
     }
  
  int systemRet =system (cm);
	if(systemRet == -1){return;}

  g_free (t);
  g_free (cm);
}

void on_mni_open_open_movie ()
{
  GtkWidget *dialog = gtk_file_chooser_dialog_new (_("Open a movie:"),
                                                   GTK_WINDOW(tea_main_window),
                                                   GTK_FILE_CHOOSER_ACTION_OPEN,
                                                   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                   GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                                   NULL);

  gchar *t;

  if (get_page_text())
  if (g_file_test (cur_text_doc->file_name, G_FILE_TEST_EXISTS))
     {
      t = g_path_get_dirname (cur_text_doc->file_name);
      gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), t);
      g_free (t);
     }
  

  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
     gchar *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
     confile.current_movie = ch_str (confile.current_movie, filename);
     g_free (filename);
    }

  gtk_widget_destroy (GTK_WIDGET(dialog));
}

void on_mni_help_item (GtkMenuItem *menuitem)
{
  cur_settings.selected_enc = ch_str (cur_settings.selected_enc, "UTF-8");
  gchar *f = get_tea_doc_compose_name (gtk_widget_get_name (GTK_WIDGET(menuitem)));
  open_file_std (f);
  g_free (f);
}

void on_mni_insert_doctype (GtkMenuItem *menuitem)
{
  if (! get_page_text()) return;

  gchar const *n = gtk_widget_get_name (GTK_WIDGET(menuitem));
  gchar *t="";

  if (strcmp (n, "HTML 4.1 Transitional") == 0)
     t = g_strdup ("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/1999/REC-html401-19991224/loose.dtd\">");
  else 
  if (strcmp (n, "HTML 4.1 Strict") == 0)
     t = g_strdup ("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">");
  else
  if (strcmp (n, "HTML 4.1 Frameset") == 0)
     t = g_strdup ("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Frameset//EN\" \"http://www.w3.org/TR/1999/REC-html401-19991224/frameset.dtd\">");
  else
  if (strcmp (n, "XHTML 1.0 Strict") == 0)
     t = g_strdup ("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">");
  else
  if (strcmp (n, "XHTML 1.0 Transitional") == 0)
     t = g_strdup ("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">");
  else
  if (strcmp (n, "XHTML 1.0 Frameset") == 0)
     t = g_strdup ("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Frameset//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-frameset.dtd\">");
  else
  if (strcmp (n, "WML 1.1") == 0)
     t = g_strdup ("<xml version='1.0'> \n<!DOCTYPE wml PUBLIC \"-//WAPFORUM//DTD WML 1.1//EN\" \"http://www.wapforum.org/DTD/wml_1.1.xml\">");
  
  doc_insert_at_cursor (cur_text_doc, t); 

  g_free (t);
}

void on_mni_entity (GtkMenuItem *menuitem)
{
  if (! get_page_text()) return;

  gchar *t = NULL;
  gchar const *n = gtk_widget_get_name (GTK_WIDGET(menuitem));
  gchar *x = g_hash_table_lookup (ht_entities, n);
  if (x)
     t = g_strdup_printf ("&#%s;", x);

  doc_insert_at_cursor (cur_text_doc, t); 

  g_free (t);    
}

void on_mni_sessions_click (GtkMenuItem *menuitem)
{
  gchar *f = g_strconcat (confile.sessions_dir, gtk_widget_get_name (GTK_WIDGET(menuitem)), NULL);

  GList *list = load_file_to_glist (f);
  GList *t = g_list_first (list);

  while (t) 
       {
        execute_recent_item (t->data);
        t = g_list_next (t);
       }

  glist_strings_free (list);
  g_free (f);
}

void on_mni_templates_click (GtkMenuItem *menuitem)
{
  gchar *f = g_strconcat (confile.templates_dir, gtk_widget_get_name (GTK_WIDGET(menuitem)), NULL);
  gchar *t = str_file_read (f);
  doc_ins_to_new (t);
  g_free (f);
  g_free (t);
}

void on_mni_morse_encode ()
{
  if (! get_page_text()) return;

  gchar *t = doc_get_sel (cur_text_doc);

  if (! t)
     return;

  gchar *x = morse_encode (t);
 
  doc_rep_sel (cur_text_doc, x);
  g_free (t);
  g_free (x);
}

void on_mni_morse_decode ()
{
  if (! get_page_text()) return;

  gchar *t = doc_get_sel (cur_text_doc);

  if (! t)
     return;

  gchar *x = morse_decode (t);
 
  doc_rep_sel (cur_text_doc, x);
  g_free (t);
  g_free (x);
}

void on_mni_set_hl_mode (GtkMenuItem *menuitem)
{
  if (! get_page_text()) return;
  cur_text_doc->hl_mode = ch_str (cur_text_doc->hl_mode, gtk_widget_get_name (GTK_WIDGET(menuitem)));
  apply_hl (cur_text_doc);
}

typedef struct 
              {
               t_note_page *page;
               gchar *prefix;    
               GtkTextIter itstart;
               GtkTextIter itend;  
             } t_text_part;


//******************************* DELETE autocomp tips
void delete_autocomp_tips(){if (win_tips_autocomp != NULL){gtk_widget_destroy (GTK_WIDGET(win_tips_autocomp));win_tips_autocomp=NULL;}}

//******************************* Lance la construction des tips au relachement de touche clavier
gboolean on_editor_keyrelease ()
{

	if (! get_page_text()) return FALSE;

	//****************************** Status bar ln cols update
  gchar *msg;
  gint row,row2, col;
  GtkTextIter iter,iter2;

		gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &iter2);
  gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(cur_text_doc->text_buffer),
      &iter, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(cur_text_doc->text_buffer)));

  row = gtk_text_iter_get_line(&iter);
  row2 = gtk_text_iter_get_line(&iter2);
  col = gtk_text_iter_get_line_offset(&iter);

  msg = g_strdup_printf(" [File] : %s\t\t [Col] : %d \t\t[Ln] : %d/%d", cur_text_doc->file_name,col+1, row+1,row2+1);
	statusbar_msg(msg);

  gchar *t;

  //gchar *s;
  GtkTextIter itstart;
  GtkTextIter itend;  

  //t_text_part *p;

	//****************************** bg word backward
	if(row2<1000)
	{
	GtkTextIter start_find, end_find;
	GtkTextIter start_match, end_match;

	t = doc_get_word_at_left (cur_text_doc, &itstart, &itend);
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &start_find);
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &end_find);

	gtk_text_buffer_remove_tag_by_name(GTK_TEXT_BUFFER(cur_text_doc->text_buffer), "gray_bg", &start_find, &end_find);

     if (! t){return FALSE;}else{
	while ( gtk_text_iter_forward_search(&start_find, t, GTK_TEXT_SEARCH_TEXT_ONLY | GTK_TEXT_SEARCH_VISIBLE_ONLY, &start_match, &end_match, NULL) )
					{
          gtk_text_buffer_apply_tag_by_name(GTK_TEXT_BUFFER(cur_text_doc->text_buffer), "gray_bg", 
              &start_match, &end_match);
          int offset = gtk_text_iter_get_offset(&end_match);
          gtk_text_buffer_get_iter_at_offset(GTK_TEXT_BUFFER(cur_text_doc->text_buffer), 
              &start_find, offset);
					}
			}

     g_free (t);
	}

	return TRUE;
}

//******************************* à la pression d'une touche
gboolean on_editor_keypress ( GdkEventKey *event, gpointer data)
{

	if (! get_page_text()) return FALSE;


  t_note_page *page = data;
  gchar *t;
  gchar *s;
  GtkTextIter itstart;
  GtkTextIter itend;  
  t_text_part *p;

         
          t = doc_get_word_at_left (cur_text_doc, &itstart, &itend);
          
          if (! t)
             return FALSE;

  guint32 k = gdk_keyval_to_unicode (event->keyval);

  if (confile.do_autorep && ht_autoreplace)
     {
      if (g_unichar_isspace (k))
	  {
      t = doc_get_word_at_left (cur_text_doc, &itstart, &itend);
      if (! t)
         return FALSE;

      p = g_malloc (sizeof (t_text_part));
      p->page = page;
      p->itstart = itstart; 
      p->itend = itend; 
      p->prefix = g_strdup (t);

      s = g_hash_table_lookup (ht_autoreplace, t);  
      if (s)
        {  
         gtk_text_buffer_delete (GTK_TEXT_BUFFER(p->page->text_buffer),
                                 &p->itstart,
                                 &p->itend);

         gtk_text_buffer_insert (GTK_TEXT_BUFFER(p->page->text_buffer),
                                 &p->itstart,
                                 s,
                                 -1);
     } 
     
     g_free (p);
     g_free (t); 
		}
    }

//********************** AUTOCOMPLEMENTATION
if (event->keyval == '(')
{
doc_insert_at_cursor (cur_text_doc, ")");
doc_move_cursor_backw(cur_text_doc,1);
}

	   char *extension;
	if(strrchr(cur_text_doc->file_name,'.'))
	{
	extension = strrchr(cur_text_doc->file_name,'.');
if (event->keyval == '\"' && confile.use_textcompbloc == 1 && strcmp(".txt", extension) != 0)
{
doc_insert_at_cursor (cur_text_doc, "\"");
doc_move_to_pos_bw_quote (cur_text_doc);
}

if (event->keyval == '{' && confile.use_textcompbloc == 1)
{
	doc_insert_at_cursor (cur_text_doc, "}");
	doc_move_cursor_backw(cur_text_doc,1);
}

if (event->keyval == '[' && confile.use_textcompbloc == 1)
{
doc_insert_at_cursor (cur_text_doc, "]");
doc_move_cursor_backw(cur_text_doc,1);
}

if (event->keyval == '<' && confile.use_textcompbloc == 1)
{
	if(strcmp(".html", extension) == 0 || strcmp(".htm", extension) == 0 || strcmp(".php", extension) == 0)
	{
		doc_insert_at_cursor (cur_text_doc, ">");
		doc_move_cursor_backw(cur_text_doc,1);
	}
}

if (event->keyval == '\'' && confile.use_textcompbloc == 1 && strcmp(".txt", extension) != 0)
{
doc_insert_at_cursor (cur_text_doc, "\'");
doc_move_cursor_backw(cur_text_doc,1);
}
}
  if (event->keyval == GDK_KEY_Return)
     if (confile.use_auto_indent) 
        {
         indent_real(GTK_WIDGET(page->text_view));
         return TRUE;
        }

  if (event->keyval == GDK_KEY_Tab)
     {
      doc_indent_selection (page, FALSE);
      return TRUE; 
     }

//on_mni_utils_stats(NULL,NULL);

 return FALSE;
}

void on_mni_unindent ()
{
  if (! get_page_text()) return;
  doc_indent_selection (cur_text_doc, TRUE);
}


void on_mni_indent ()
{
  if (! get_page_text()) return;
  doc_indent_selection (cur_text_doc, FALSE);
}

void on_mni_autorep_file_open ()
{
  cur_settings.selected_enc = ch_str (cur_settings.selected_enc, "UTF-8");
  open_file_std (confile.tea_autoreplace);
}

void on_mni_autocomp_file_open ()
{
  cur_settings.selected_enc = ch_str (cur_settings.selected_enc, "UTF-8");
  open_file_std (confile.tea_autocomp);
}


void on_mni_strings_remove_leading_whitespaces ()
{
  if (! get_page_text()) return;
  
  gchar *buf = doc_get_sel (cur_text_doc);

  if (! buf) return;

  GList *temp = glist_from_string (buf);
  GList *temp2 = each_line_remove_lt_spaces (temp, 0);
  gchar *t = string_from_glist (temp2);

  doc_rep_sel (cur_text_doc, t);

  g_free (buf);
  g_free (t);
  glist_strings_free (temp);
  glist_strings_free (temp2);
}

void on_mni_strings_remove_trailing_whitespaces ()
{
  if (! get_page_text()) return;
  
  gchar *buf = doc_get_sel (cur_text_doc);

  if (! buf) return;

  GList *temp = glist_from_string (buf);
  GList *temp2 = each_line_remove_lt_spaces (temp, 0);
  gchar *t = string_from_glist (temp2);

  doc_rep_sel (cur_text_doc, t);

  g_free (buf);
  g_free (t);
  glist_strings_free (temp);
  glist_strings_free (temp2);
}

//********************* Editer le crapbook
void on_mni_edit_append_to_crapbook ()
{
  if (! get_page_text()) return;
  
  t_note_page *p;
  GtkTextIter iter;

  gchar *buf = doc_get_sel (cur_text_doc);

  if (! buf) return;

  gchar *t = g_strconcat (buf, "\n", NULL);  

  gint i = get_n_page_by_filename (confile.crapbook_file);

gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_note), t, -1);

  if (i != -1)
     {
      p = get_page_by_index (i);
      gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(p->text_buffer), &iter, 0);
      gtk_text_buffer_insert (GTK_TEXT_BUFFER(p->text_buffer), &iter, t, -1);
     
      g_free (buf);
      g_free (t);
      return;
     } 

  GList *l = load_file_to_glist (confile.crapbook_file);

  l = g_list_prepend (l, t);

  glist_save_to_file (l, confile.crapbook_file);
  glist_strings_free (l);
}

void on_mni_load_last_file ()
{
  if (! recent_list)
    return;

  execute_recent_item (recent_list->data); 
}

//******************************* ouvrir un fichier header .h
void on_source_header_switch ()
{
  if (! get_page_text()) return;
  doc_header_source_switch (cur_text_doc);
}

void on_mni_show_project_props ()
{
  if (cur_tea_project)
     create_proj_props_window (cur_tea_project);  
}

//******************************* Ouvrir un nouveau projet
void on_mni_project_new ()
{
  cur_tea_project = tea_proj_clear_new (cur_tea_project);
  if (cur_tea_project)
     create_proj_props_window (cur_tea_project);
}

//******************************* sauvegarder sous un projet
void on_mni_project_save_as ()
{
  if (! cur_tea_project)
     return;

  gchar *filename;

  GtkWidget *dialog = gtk_file_chooser_dialog_new (_("Save a project"),
                                                   GTK_WINDOW(tea_main_window),
                                                   GTK_FILE_CHOOSER_ACTION_SAVE,
                                                   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                   GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                                                   NULL);
  
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
     {
      filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
      tea_proj_save_as (cur_tea_project, filename);
      g_free (filename);
     }

   gtk_widget_destroy (GTK_WIDGET(dialog));
}

void on_mni_show_project_save ()
{
  if (! cur_tea_project)
     return;

  if (! g_file_test (cur_tea_project->project_file_name, G_FILE_TEST_EXISTS))
     {
      on_mni_project_save_as (NULL, NULL);
      return;
     } 

  tea_proj_save (cur_tea_project);
}

//******************************* Ouvrir un projet
void on_mni_project_open ()
{
  gchar *filename;

  GtkWidget *dialog = gtk_file_chooser_dialog_new (_("Open a project"),
                                                   GTK_WINDOW(tea_main_window),
                                                   GTK_FILE_CHOOSER_ACTION_OPEN,
                                                   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                  GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                                   NULL);

if (confile.use_def_open_dir){gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog),confile.def_open_dir);}

  
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
     {
      filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
      tea_proj_free (cur_tea_project);
      cur_tea_project = tea_proj_open (filename);

	//******************************* IMAGE Griffon icon dans zone d'information 
	GdkPixbuf *pixbuf;
	GtkTextIter itFin;
	pixbuf = gdk_pixbuf_new_from_file("/usr/local/share/griffon/pixmaps/griffon_button.png", NULL);
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer_projet), &itFin);
	gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(buffer_projet),&itFin,pixbuf);


	gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet), (_("   FICHIER PROJET : ")), -1);
	gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet), filename, -1);
	gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet), "\n\n________________________________________________________________________________________________\n\n", -1);
		
		//**************** OUVERTURE DU FICHIER PROJET
		gchar lecture[1024];
		FILE *fichier;
		fichier = fopen(filename,"rt");
		while(fgets(lecture, 1024, fichier))
		{
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet),g_locale_to_utf8(lecture, -1, NULL, NULL, NULL) , -1);
		}

		fclose(fichier);

      g_free (filename);
     }

   gtk_widget_destroy (GTK_WIDGET(dialog));
}

//******************************* compiler un projet
void on_mni_project_make ()
{
icon_affiche_bug();

  if (! cur_tea_project)
      return;

   if (! cur_tea_project->dir_makefile)
      return;

  if (! g_file_test (cur_tea_project->dir_makefile, G_FILE_TEST_EXISTS))
     return;
 
  int systemRet =chdir (cur_tea_project->dir_makefile);
		if(systemRet == -1){return;}

  gchar *standard_output = NULL;
  gchar *standard_error = NULL;
                                                   
  GError *err = NULL;

  gchar *cmd = g_strconcat ("make", NULL); 
  
  gsize bytes_read;
  gsize bytes_written;
  gchar *x = NULL;

  if (! g_spawn_command_line_sync  (cmd, &standard_output, &standard_error, NULL, &err))
    {
     fprintf (stderr, "Error: %s\n", err->message);
     g_error_free (err);
     return;
    }
  else
     {
      x = g_locale_to_utf8 (standard_output, -1, &bytes_read, &bytes_written, NULL);
      log_to_memo (x, NULL, LM_NORMAL);
      g_free (x);

      x = g_locale_to_utf8 (standard_error, -1, &bytes_read, &bytes_written, NULL);
      log_to_memo (x, NULL, LM_NORMAL);

      g_free (x);
  
      do_errors_hl (GTK_TEXT_VIEW(tv_logmemo));
      }   
	statusbar_msg (_("Make [OK]"));
  g_free (cmd);
  g_free (standard_output);
  g_free (standard_error);
	icon_log_logmemo();
}

//******************************* executer un projet
void on_mni_show_project_run (){tea_proj_run (cur_tea_project);  }

void on_mni_test ()
{
  mni_temp = NULL;
  GtkWidget *m = find_menuitem (GTK_CONTAINER (menubar1), "BR");
  if (m)
     //dbm (gtk_widget_get_name (m));
      gtk_menu_item_activate (GTK_MENU_ITEM (m));
}

//******************************* Debut de script Bash
void print_bash (void)
{	
	time_t date;
    date = time(NULL);

  doc_insert_at_cursor (cur_text_doc, (_("#!/bin/bash\n\n# Script BASH edite avec Griffon: http://griffon.lasotel.fr\n# Date de creation du script : "))); 
  doc_insert_at_cursor (cur_text_doc, ctime(&date)); 
}

//******************************* Bash insert help
void et_bash(void){	  doc_insert_at_cursor (cur_text_doc," && ");}
void gen_xmessage             (void){	  doc_insert_at_cursor (cur_text_doc,(_("\n# Message sur l'interface graphique\nxmessage -center -timeout 4 \" Texte a afficher. \"\n")));}
void ou_bash             (void){	  doc_insert_at_cursor (cur_text_doc," || ");}
void reNULL_bash             (void){	  doc_insert_at_cursor (cur_text_doc," > /dev/null");}
void reECRASER_bash             (void){	  doc_insert_at_cursor (cur_text_doc," > ");}
void reRAJOUTER_bash             (void){	  doc_insert_at_cursor (cur_text_doc," >> ");}
void boucle_bash             (void){  doc_insert_at_cursor (cur_text_doc,(_("# Boucle \nwhile\ndo\n\ndone\n")));}
void fichier_ligne_bash             (void){       doc_insert_at_cursor (cur_text_doc,(_("# Lecture d'un fichier ligne par ligne\nwhile read ligne\ndo\n\ndone < fichier\n")));}
void test1             (void){	  doc_insert_at_cursor (cur_text_doc,(_(" [ -e fichier ]")));}
void test12             (void){	  doc_insert_at_cursor (cur_text_doc,(_(" [ -s fichier ]")));}
void gen_rpm             (void){	  doc_insert_at_cursor (cur_text_doc,(_("# Construction d'un package RPM (grace au fichier spec dans /usr/src/redhat/SPEC/)\nrpmbuild -ba fichier.spec\n")));}
void test2             (void){	  doc_insert_at_cursor (cur_text_doc,(_(" [ -d repertoire ]")));}
void test22             (void){	  doc_insert_at_cursor (cur_text_doc,(_(" [ \"chaine1\" != \"chaine2\" ]")));}
void test3             (void){	  doc_insert_at_cursor (cur_text_doc," [ ! ]");}
void test4             (void){     doc_insert_at_cursor (cur_text_doc,(_(" [ -r fichier ]")));}
void test5             (void){     doc_insert_at_cursor (cur_text_doc,(_(" [ -w fichier ]")));}
void test6             (void){     doc_insert_at_cursor (cur_text_doc,(_(" [ -x fichier ]")));}
void test7             (void){  doc_insert_at_cursor (cur_text_doc,(_(" [ -L fichier ]")));}
void test8             (void){  doc_insert_at_cursor (cur_text_doc,(_(" [ -N fichier ]")));}
void test24             (void){  doc_insert_at_cursor (cur_text_doc,(_(" [ -G fichier ]")));}
void test25             (void){  doc_insert_at_cursor (cur_text_doc,(_(" [ -O fichier ]")));}
void test23             (void){  doc_insert_at_cursor (cur_text_doc,(_(" [ -n \"chaine\" ]")));}
void test9             (void){  doc_insert_at_cursor (cur_text_doc,(_(" [ fichier1 -nt fichier2 ]")));}
void gen_smbmount             (void){     doc_insert_at_cursor (cur_text_doc,(_("# Montage de partages samba\nsmbmount //192.168.1.1/partage /point/de/montage username=toto, password=mdp\n")));}
void test10             (void){     doc_insert_at_cursor (cur_text_doc,(_(" [ \"variable1\" -eq \"variable2\" ]")));}
void test11             (void){  doc_insert_at_cursor (cur_text_doc,(_(" [ \"chiffre1\" = \"chiffre2\" ]")));}
void rsync_gen             (void){  doc_insert_at_cursor (cur_text_doc,(_("# Commande Rsync\nrsync -avH --delete /repertoire/source /repertoire/destination >> /fichier/de_log")));}
void echo_bash             (void){  doc_insert_at_cursor (cur_text_doc,(_("# Affichage sur la sortie standare\necho \"\"\n")));}
void gen_read             (void){  doc_insert_at_cursor (cur_text_doc,(_("\n# Interactivite avec l'utilisateur\necho \"entrez une valeur \'variable\' :\"\nread variable\n\necho \"La variable = $variable\"\n")));}
void com             (void){  doc_insert_at_cursor (cur_text_doc,"\n#\n#\n#\n");}
void redi_erreur             (void){  doc_insert_at_cursor (cur_text_doc," 2>");}
void if_bash             (void){  doc_insert_at_cursor (cur_text_doc,(_("# Instruction si...\nif\nthen\n\nfi\n")));}
void redi_2             (void){  doc_insert_at_cursor (cur_text_doc,(_(" > /fichier 2>&1")));}
void fond_bash             (void){	  doc_insert_at_cursor (cur_text_doc," &");}
void else_bash             (void){  doc_insert_at_cursor (cur_text_doc,(_("# Instruction si avec un sinon\nif\nthen\n\nelse\n\nfi\n")));}
void skeleton             (void){	  doc_insert_at_cursor (cur_text_doc,(_("#! /bin/sh\n\n# Script init.d (exemple)\n# Script BASH edite avec Griffon \nPATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin\nDAEMON=/usr/sbin/daemon\nNAME=daemon\nDESC=\"some daemon\"\n\ntest -x $DAEMON || exit 0\n\nset -e\n\ncase \"$1\" in\n  start)\n        echo -n \"Starting $DESC: $NAME\"\n	start-stop-daemon --start --quiet --pidfile /var/run/$NAME.pid --exec $DAEMON\n        echo \".\"\n        ;;\n  stop)\n        echo -n \"Stopping $DESC: $NAME \"\n	start-stop-daemon --stop --quiet --pidfile /var/run/$NAME.pid\n        echo \".\"\n        ;;\n  restart|force-reload)\n        echo -n \"Restarting $DESC: $NAME\"\n        start-stop-daemon --stop --quiet --pidfile /var/run/$NAME.pid --exec $DAEMON\n        sleep 1\n        start-stop-daemon --start --quiet --pidfile /var/run/$NAME.pid --exec $DAEMON\n        echo \".\"\n        ;;\n  *)\n        N=/etc/init.d/$NAME\n	echo \"Usage: $N {start|stop|restart|force-reload}\" >&2\n        ;;\nesac\n")));}

//******************************* fenetre de construction commande grep
GtkWidget* window_grep (void)
{
  GtkWidget *window1,*vbox1,*hbox2,*vbox2,*alignment4,*hbox6,*image5,*label4,*alignment3,*hbox5,*image4,*label3,*alignment2,*hbox4,*image3,*label2,*alignment1;
  GtkWidget *hbox3,*image2,*label1,*vbox3,*vbox4,*label5,*label6,*label7,*hbox1,*button1,*button2;

	icon_affiche_stop();

  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window1), _((_("Aide pour Grep"))));
    gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);
  gtk_window_set_resizable (GTK_WINDOW (window1), FALSE);
  gtk_widget_show(GTK_WIDGET(window1));

  vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (GTK_WIDGET(vbox1));
  gtk_container_add (GTK_CONTAINER (window1), vbox1);

  hbox2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox2));
  gtk_box_pack_start (GTK_BOX (vbox1), hbox2, TRUE, TRUE, 0);

  vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (GTK_WIDGET(vbox2));
  gtk_box_pack_start (GTK_BOX (hbox2), vbox2, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (vbox2), 12);

  checkbutton1_grep = gtk_check_button_new ();
  gtk_widget_show (GTK_WIDGET(checkbutton1_grep));
  gtk_box_pack_start (GTK_BOX (vbox2), checkbutton1_grep, FALSE, FALSE, 0);

  alignment4 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (GTK_WIDGET(alignment4));
  gtk_container_add (GTK_CONTAINER (checkbutton1_grep), alignment4);

  hbox6 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox6));
  gtk_container_add (GTK_CONTAINER (alignment4), hbox6);

  image5 = gtk_image_new_from_stock ("gtk-bold", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (GTK_WIDGET(image5));
  gtk_box_pack_start (GTK_BOX (hbox6), image5, FALSE, FALSE, 0);

  label4 = gtk_label_new_with_mnemonic (_("Indiff\303\251rence Maj/Min"));
  gtk_widget_show (GTK_WIDGET(label4));
  gtk_box_pack_start (GTK_BOX (hbox6), label4, FALSE, FALSE, 0);

  checkbutton2_grep = gtk_check_button_new ();
  gtk_widget_show (GTK_WIDGET(checkbutton2_grep));
  gtk_box_pack_start (GTK_BOX (vbox2), checkbutton2_grep, FALSE, FALSE, 0);

  alignment3 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (GTK_WIDGET(alignment3));
  gtk_container_add (GTK_CONTAINER (checkbutton2_grep), alignment3);

  hbox5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox5));
  gtk_container_add (GTK_CONTAINER (alignment3), hbox5);

  image4 = gtk_image_new_from_stock ("gtk-select-font", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (GTK_WIDGET(image4));
  gtk_box_pack_start (GTK_BOX (hbox5), image4, FALSE, FALSE, 0);

  label3 = gtk_label_new_with_mnemonic (_("Mot complet"));
  gtk_widget_show (GTK_WIDGET(label3));
  gtk_box_pack_start (GTK_BOX (hbox5), label3, FALSE, FALSE, 0);

  checkbutton3_grep = gtk_check_button_new ();
  gtk_widget_show (GTK_WIDGET(checkbutton3_grep));
  gtk_box_pack_start (GTK_BOX (vbox2), checkbutton3_grep, FALSE, FALSE, 0);

  alignment2 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (GTK_WIDGET(alignment2));
  gtk_container_add (GTK_CONTAINER (checkbutton3_grep), alignment2);

  hbox4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox4));
  gtk_container_add (GTK_CONTAINER (alignment2), hbox4);

  image3 = gtk_image_new_from_stock ("gtk-refresh", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (GTK_WIDGET(image3));
  gtk_box_pack_start (GTK_BOX (hbox4), image3, FALSE, FALSE, 0);

  label2 = gtk_label_new_with_mnemonic (_("Inverser la sortie"));
  gtk_widget_show (GTK_WIDGET(label2));
  gtk_box_pack_start (GTK_BOX (hbox4), label2, FALSE, FALSE, 0);

  checkbutton4_grep = gtk_check_button_new ();
  gtk_widget_show (GTK_WIDGET(checkbutton4_grep));
  gtk_box_pack_start (GTK_BOX (vbox2), checkbutton4_grep, FALSE, FALSE, 0);

  alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (GTK_WIDGET(alignment1));
  gtk_container_add (GTK_CONTAINER (checkbutton4_grep), alignment1);

  hbox3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox3));
  gtk_container_add (GTK_CONTAINER (alignment1), hbox3);

  image2 = gtk_image_new_from_stock ("gtk-new", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (GTK_WIDGET(image2));
  gtk_box_pack_start (GTK_BOX (hbox3), image2, FALSE, FALSE, 0);

  label1 = gtk_label_new_with_mnemonic (_("Redirection"));
  gtk_widget_show (GTK_WIDGET(label1));
  gtk_box_pack_start (GTK_BOX (hbox3), label1, FALSE, FALSE, 0);

  vbox3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (GTK_WIDGET(vbox3));
  gtk_box_pack_start (GTK_BOX (hbox2), vbox3, TRUE, TRUE, 14);

  vbox4 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (GTK_WIDGET(vbox4));
  gtk_box_pack_start (GTK_BOX (vbox3), vbox4, FALSE, FALSE, 0);

  label5 = gtk_label_new (_("Fichier"));
  gtk_widget_show (GTK_WIDGET(label5));
  gtk_box_pack_start (GTK_BOX (vbox4), label5, FALSE, FALSE, 0);

  entry_grep1 = gtk_entry_new ();
  gtk_widget_show (GTK_WIDGET(entry_grep1));
  gtk_box_pack_start (GTK_BOX (vbox4), entry_grep1, FALSE, FALSE, 0);

  label6 = gtk_label_new (_("Mot recherch\303\251"));
  gtk_widget_show (GTK_WIDGET(label6));
  gtk_box_pack_start (GTK_BOX (vbox4), label6, FALSE, FALSE, 0);

  entry_grep2 = gtk_entry_new ();
  gtk_widget_show (GTK_WIDGET(entry_grep2));
  gtk_box_pack_start (GTK_BOX (vbox4), entry_grep2, FALSE, FALSE, 0);

  label7 = gtk_label_new (_("Fichier de redirection"));
  gtk_widget_show (GTK_WIDGET(label7));
  gtk_box_pack_start (GTK_BOX (vbox4), label7, FALSE, FALSE, 0);

  entry_grep3 = gtk_entry_new ();
  gtk_widget_show (GTK_WIDGET(entry_grep3));
  gtk_box_pack_start (GTK_BOX (vbox4), entry_grep3, FALSE, FALSE, 0);

  /*hseparator1 = gtk_hseparator_new ();
  gtk_widget_show (GTK_WIDGET(hseparator1));
  gtk_box_pack_start (GTK_BOX (vbox1), hseparator1, TRUE, TRUE, 3);*/

  hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox1));
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);

  button1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (GTK_WIDGET(button1));
  gtk_box_pack_start (GTK_BOX (hbox1), button1, FALSE, TRUE, 23);
  gtk_button_set_relief (GTK_BUTTON (button1), GTK_RELIEF_NONE);

  button2 = gtk_button_new_from_stock ("gtk-apply");
  gtk_widget_show (GTK_WIDGET(button2));
  gtk_box_pack_start (GTK_BOX (hbox1), button2, TRUE, TRUE, 23);
  gtk_button_set_relief (GTK_BUTTON (button2), GTK_RELIEF_NONE);
  
  		 g_signal_connect_swapped ((gpointer) button1, "clicked",
                            G_CALLBACK (icon_affiche_ok),
                            NULL);
					
  
    g_signal_connect_swapped ((gpointer) button1, "clicked",
                            G_CALLBACK (gtk_widget_destroy),
                            window1);
							 
							 g_signal_connect_swapped ((gpointer) button2, "clicked",
                            G_CALLBACK (grep),
                            NULL);
							
		 g_signal_connect_swapped ((gpointer) button2, "clicked",
                            G_CALLBACK (icon_affiche_ok),
                            NULL);
												
							
  g_signal_connect_swapped ((gpointer) button2, "clicked",
                            G_CALLBACK (gtk_widget_destroy),
                            window1);
							
  return window1;
}

//******************************* construction commande grep insert
void grep             (void)
{	
gchar *tampon_fichier;
gchar *tampon_mot;
gchar *tampon_redi;
	
	tampon_fichier = gtk_editable_get_chars(GTK_EDITABLE(entry_grep1),0, -1);
	tampon_mot = gtk_editable_get_chars(GTK_EDITABLE(entry_grep2),0, -1);
	tampon_redi = gtk_editable_get_chars(GTK_EDITABLE(entry_grep3),0, -1);

  doc_insert_at_cursor (cur_text_doc,(_("# Commande Grep (Find dans un fichier)\ngrep")));

if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton1_grep)))
{
  doc_insert_at_cursor (cur_text_doc," -i");
}
if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton2_grep)))
{
  doc_insert_at_cursor (cur_text_doc," -w");
}
if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton3_grep)))
{
  doc_insert_at_cursor (cur_text_doc," -v");
}

  doc_insert_at_cursor (cur_text_doc," ");
  doc_insert_at_cursor (cur_text_doc,tampon_mot);
  doc_insert_at_cursor (cur_text_doc," ");
  doc_insert_at_cursor (cur_text_doc, tampon_fichier);

if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton4_grep)))
{

  doc_insert_at_cursor (cur_text_doc," > ");  
  doc_insert_at_cursor (cur_text_doc, tampon_redi);
}

}

//******************************* fenetre de construction commande sed
GtkWidget* sed (void)
{
	GtkWidget *window1,*vbox1,*hbox2,*vbox5,*notebook1,*vbox6,*label15,*label16,*hbox3,*image2,*label17,*vbox7,*hbox5,*label19,*label20;
  GtkWidget *hbox4,*image3,*label18,*vbox8,*hbox6,*image4,*label21,*vbox3,*vbox4,*label5,*label6,*hbox1,*button1,*button2;

icon_affiche_stop();

  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window1), _((_("Aide Sed"))));
  gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);
  gtk_window_set_resizable (GTK_WINDOW (window1), FALSE);
  gtk_widget_show(GTK_WIDGET(window1));

  vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (GTK_WIDGET(vbox1));
  gtk_container_add (GTK_CONTAINER (window1), vbox1);

  hbox2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox2));
  gtk_box_pack_start (GTK_BOX (vbox1), hbox2, TRUE, TRUE, 0);

  vbox5 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (GTK_WIDGET(vbox5));
  gtk_box_pack_start (GTK_BOX (hbox2), vbox5, TRUE, TRUE, 0);

  notebook1 = gtk_notebook_new ();
  gtk_widget_show (GTK_WIDGET(notebook1));
  gtk_box_pack_start (GTK_BOX (vbox5), notebook1, TRUE, TRUE, 0);

  vbox6 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (GTK_WIDGET(vbox6));
  gtk_container_add (GTK_CONTAINER (notebook1), vbox6);

  checkbutton_sed1 = gtk_check_button_new_with_mnemonic (_("Substitution"));
  gtk_widget_show (GTK_WIDGET(checkbutton_sed1));
  gtk_box_pack_start (GTK_BOX (vbox6), checkbutton_sed1, FALSE, FALSE, 0);

  label15 = gtk_label_new (_("Remplacer par "));
  gtk_widget_show (GTK_WIDGET(label15));
  gtk_box_pack_start (GTK_BOX (vbox6), label15, FALSE, FALSE, 0);

  entry_sed4 = gtk_entry_new ();
  gtk_widget_show (GTK_WIDGET(entry_sed4));
  gtk_box_pack_start (GTK_BOX (vbox6), entry_sed4, FALSE, FALSE, 0);

  label16 = gtk_label_new (_("N\302\260 Occurence (g = toutes)"));
  gtk_widget_show (GTK_WIDGET(label16));
  gtk_box_pack_start (GTK_BOX (vbox6), label16, FALSE, FALSE, 0);

  entry_sed5 = gtk_entry_new ();
  gtk_widget_show (GTK_WIDGET(entry_sed5));
  gtk_box_pack_start (GTK_BOX (vbox6), entry_sed5, FALSE, FALSE, 0);

  hbox3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox3));
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), hbox3);

  image2 = gtk_image_new_from_stock ("gtk-find-and-replace", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (GTK_WIDGET(image2));
  gtk_box_pack_start (GTK_BOX (hbox3), image2, TRUE, TRUE, 0);

  label17 = gtk_label_new (_("Substitution"));
  gtk_widget_show (GTK_WIDGET(label17));
  gtk_box_pack_start (GTK_BOX (hbox3), label17, FALSE, FALSE, 0);

  vbox7 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (GTK_WIDGET(vbox7));
  gtk_container_add (GTK_CONTAINER (notebook1), vbox7);

  checkbutton_sed2 = gtk_check_button_new_with_mnemonic (_("Supprimer les lignes"));
  gtk_widget_show (GTK_WIDGET(checkbutton_sed2));
  gtk_box_pack_start (GTK_BOX (vbox7), checkbutton_sed2, FALSE, FALSE, 0);

  hbox5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox5));
  gtk_box_pack_start (GTK_BOX (vbox7), hbox5, TRUE, TRUE, 0);

  label19 = gtk_label_new (_("de "));
  gtk_widget_show (GTK_WIDGET(label19));
  gtk_box_pack_start (GTK_BOX (hbox5), label19, FALSE, FALSE, 0);

  entry_sed6 = gtk_entry_new ();
  gtk_widget_show (GTK_WIDGET(entry_sed6));
  gtk_box_pack_start (GTK_BOX (hbox5), entry_sed6, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry_sed6, 4, -1);

  label20 = gtk_label_new (_(" \303\240 "));
  gtk_widget_show (GTK_WIDGET(label20));
  gtk_box_pack_start (GTK_BOX (hbox5), label20, FALSE, FALSE, 0);

  entry_sed7 = gtk_entry_new ();
  gtk_widget_show (GTK_WIDGET(entry_sed7));
  gtk_box_pack_start (GTK_BOX (hbox5), entry_sed7, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry_sed7, 2, -1);

  /*hseparator2 = gtk_hseparator_new ();
  gtk_widget_show (GTK_WIDGET(hseparator2));
  gtk_box_pack_start (GTK_BOX (vbox7), hseparator2, TRUE, TRUE, 0);*/

  checkbutton_sed3 = gtk_check_button_new_with_mnemonic (_("Supprimer les lignes avec le mot recherch\303\251"));
  gtk_widget_show (GTK_WIDGET(checkbutton_sed3));
  gtk_box_pack_start (GTK_BOX (vbox7), checkbutton_sed3, FALSE, FALSE, 0);

  checkbutton_sed4 = gtk_check_button_new_with_mnemonic (_("Inversser la suppression"));
  gtk_widget_show (GTK_WIDGET(checkbutton_sed4));
  gtk_box_pack_start (GTK_BOX (vbox7), checkbutton_sed4, FALSE, FALSE, 0);

  hbox4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox4));
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), hbox4);

  image3 = gtk_image_new_from_stock ("gtk-clear", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (GTK_WIDGET(image3));
  gtk_box_pack_start (GTK_BOX (hbox4), image3, TRUE, TRUE, 0);

  label18 = gtk_label_new (_("Suppression"));
  gtk_widget_show (GTK_WIDGET(label18));
  gtk_box_pack_start (GTK_BOX (hbox4), label18, FALSE, FALSE, 0);

  vbox8 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (GTK_WIDGET(vbox8));
  gtk_container_add (GTK_CONTAINER (notebook1), vbox8);

  checkbutton_sed5 = gtk_check_button_new_with_mnemonic (_("Afiche le resultat sur la sortie standare"));
  gtk_widget_show (GTK_WIDGET(checkbutton_sed5));
  gtk_box_pack_start (GTK_BOX (vbox8), checkbutton_sed5, FALSE, FALSE, 0);

  checkbutton_sed6 = gtk_check_button_new_with_mnemonic (_("Donne le numero des lignes"));
  gtk_widget_show (GTK_WIDGET(checkbutton_sed6));
  gtk_box_pack_start (GTK_BOX (vbox8), checkbutton_sed6, FALSE, FALSE, 0);

  hbox6 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox6));
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 2), hbox6);

  image4 = gtk_image_new_from_stock ("gtk-add", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (GTK_WIDGET(image4));
  gtk_box_pack_start (GTK_BOX (hbox6), image4, TRUE, TRUE, 0);

  label21 = gtk_label_new (_("Fonctions"));
  gtk_widget_show (GTK_WIDGET(label21));
  gtk_box_pack_start (GTK_BOX (hbox6), label21, FALSE, FALSE, 0);

  vbox3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (GTK_WIDGET(vbox3));
  gtk_box_pack_start (GTK_BOX (hbox2), vbox3, TRUE, TRUE, 14);

  vbox4 =gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (GTK_WIDGET(vbox4));
  gtk_box_pack_start (GTK_BOX (vbox3), vbox4, FALSE, FALSE, 0);

  label5 = gtk_label_new (_("Fichier"));
  gtk_widget_show (GTK_WIDGET(label5));
  gtk_box_pack_start (GTK_BOX (vbox4), label5, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label5), 0.48, 0.5);

  entry_sed1 = gtk_entry_new ();
  gtk_widget_show (GTK_WIDGET(entry_sed1));
  gtk_box_pack_start (GTK_BOX (vbox4), entry_sed1, FALSE, FALSE, 0);

  label6 = gtk_label_new (_("Mot recherch\303\251"));
  gtk_widget_show (GTK_WIDGET(label6));
  gtk_box_pack_start (GTK_BOX (vbox4), label6, FALSE, FALSE, 0);

  entry_sed2 = gtk_entry_new ();
  gtk_widget_show (GTK_WIDGET(entry_sed2));
  gtk_box_pack_start (GTK_BOX (vbox4), entry_sed2, FALSE, FALSE, 0);

  checkbutton_sed7 = gtk_check_button_new_with_mnemonic (_("Fichier de redirection"));
  gtk_widget_show (GTK_WIDGET(checkbutton_sed7));
  gtk_box_pack_start (GTK_BOX (vbox4), checkbutton_sed7, FALSE, FALSE, 0);

  entry_sed3 = gtk_entry_new ();
  gtk_widget_show (GTK_WIDGET(entry_sed3));
  gtk_box_pack_start (GTK_BOX (vbox4), entry_sed3, FALSE, FALSE, 0);

  /*hseparator1 = gtk_hseparator_new ();
  gtk_widget_show (GTK_WIDGET(hseparator1));
  gtk_box_pack_start (GTK_BOX (vbox1), hseparator1, TRUE, TRUE, 3);*/

  hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox1));
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);

  button1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (GTK_WIDGET(button1));
  gtk_box_pack_start (GTK_BOX (hbox1), button1, FALSE, TRUE, 23);
  gtk_button_set_relief (GTK_BUTTON (button1), GTK_RELIEF_NONE);

  button2 = gtk_button_new_from_stock ("gtk-apply");
  gtk_widget_show (GTK_WIDGET(button2));
  gtk_box_pack_start (GTK_BOX (hbox1), button2, TRUE, TRUE, 23);
  gtk_button_set_relief (GTK_BUTTON (button2), GTK_RELIEF_NONE);

		
  g_signal_connect_swapped ((gpointer) button1, "clicked",
                            G_CALLBACK (icon_affiche_ok),
                            NULL);
					
  g_signal_connect_swapped ((gpointer) button1, "clicked",
                            G_CALLBACK (gtk_widget_destroy),
                            window1);
							
  g_signal_connect_swapped ((gpointer) button2, "clicked",
                            G_CALLBACK (sed_apl),
                            NULL);
							

  g_signal_connect_swapped ((gpointer) button2, "clicked",
                            G_CALLBACK (icon_affiche_ok),
                            NULL);							
							
  g_signal_connect_swapped ((gpointer) button2, "clicked",
                            G_CALLBACK (gtk_widget_destroy),
                            window1);

  return window1;
}

//******************************* construction commande sed insert	
void sed_apl             (void)
{		
gchar *tampon_fichier;
gchar *tampon_mot;
gchar *tampon_redi;
gchar *tampon_ch;
gchar *tampon_ocu;	
gchar *tampon_de;
gchar *tampon_a;	
	
	tampon_fichier = gtk_editable_get_chars(GTK_EDITABLE(entry_sed1),0, -1);
	tampon_mot = gtk_editable_get_chars(GTK_EDITABLE(entry_sed2),0, -1);
	tampon_redi = gtk_editable_get_chars(GTK_EDITABLE(entry_sed3),0, -1);
	tampon_ch = gtk_editable_get_chars(GTK_EDITABLE(entry_sed4),0, -1);
	tampon_ocu = gtk_editable_get_chars(GTK_EDITABLE(entry_sed5),0, -1);
	tampon_de = gtk_editable_get_chars(GTK_EDITABLE(entry_sed6),0, -1);
	tampon_a = gtk_editable_get_chars(GTK_EDITABLE(entry_sed7),0, -1);
	
	doc_insert_at_cursor (cur_text_doc,(_("# Commande Sed (decoupage de fichier)\nsed ")));
	
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_sed1)))
{

doc_insert_at_cursor (cur_text_doc,"\"s");
doc_insert_at_cursor (cur_text_doc,"/");
doc_insert_at_cursor (cur_text_doc,tampon_mot);
doc_insert_at_cursor (cur_text_doc,"/");
doc_insert_at_cursor (cur_text_doc,tampon_ch);
doc_insert_at_cursor (cur_text_doc,"/");
doc_insert_at_cursor (cur_text_doc, tampon_ocu);

}

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_sed2)))
{

doc_insert_at_cursor (cur_text_doc,"\"");
doc_insert_at_cursor (cur_text_doc,tampon_de);
doc_insert_at_cursor (cur_text_doc,",");
doc_insert_at_cursor (cur_text_doc, tampon_a);
doc_insert_at_cursor (cur_text_doc,"d");	

}

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_sed3)))
{
doc_insert_at_cursor (cur_text_doc,"\"/");
doc_insert_at_cursor (cur_text_doc,(_(" tampon_mot")));
doc_insert_at_cursor (cur_text_doc,"/");
	
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_sed4)))
	{
	doc_insert_at_cursor (cur_text_doc,"!");
 	} 
doc_insert_at_cursor (cur_text_doc,"d");
}	

if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_sed5)))
{ 
doc_insert_at_cursor (cur_text_doc,"p");
}

if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_sed6)))
{ 
doc_insert_at_cursor (cur_text_doc,"=");
}
doc_insert_at_cursor (cur_text_doc,"\"");
doc_insert_at_cursor (cur_text_doc," ");
doc_insert_at_cursor (cur_text_doc,tampon_fichier);

if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_sed7)))
{ 
doc_insert_at_cursor (cur_text_doc," > ");
doc_insert_at_cursor (cur_text_doc, tampon_redi); 
}
}

//******************************* fenetre de construction instruction case en bash
GtkWidget* case_window (void)
{	
  GtkWidget *window1,*frame1,*vbox1,*hbox1,*image1,*label2,*button1,*label1;
  GtkAdjustment *spinbutton1_adj;

icon_affiche_stop();
	
  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window1), _((_("Nombre d'options"))));
  gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);
  gtk_window_set_resizable (GTK_WINDOW (window1), FALSE);
  gtk_widget_show(GTK_WIDGET(window1));

  frame1 = gtk_frame_new (NULL);
  gtk_widget_show (GTK_WIDGET(frame1));
  gtk_container_add (GTK_CONTAINER (window1), frame1);

  vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (GTK_WIDGET(vbox1));
  gtk_container_add (GTK_CONTAINER (frame1), vbox1);

  hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox1));
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, FALSE, TRUE, 0);

  image1 = gtk_image_new_from_stock ("gtk-properties", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (GTK_WIDGET(image1));
  gtk_box_pack_start (GTK_BOX (hbox1), image1, TRUE, TRUE, 7);

  label2 = gtk_label_new (_("Nombre d'options du script : "));
  gtk_widget_show (GTK_WIDGET(label2));
  gtk_box_pack_start (GTK_BOX (hbox1), label2, TRUE, FALSE, 0);

  spinbutton1_adj = gtk_adjustment_new (1, 0, 100, 1, 10, 10);
  spinbutton1 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton1_adj), 1, 0);
  gtk_widget_show (GTK_WIDGET(spinbutton1));
  gtk_box_pack_start (GTK_BOX (hbox1), spinbutton1, FALSE, TRUE, 4);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbutton1), TRUE);

  /*hseparator1 = gtk_hseparator_new ();
  gtk_widget_show (GTK_WIDGET(hseparator1));
  gtk_box_pack_start (GTK_BOX (vbox1), hseparator1, FALSE, TRUE, 3);*/

  button1 = gtk_button_new_from_stock ("gtk-apply");
  gtk_widget_show (GTK_WIDGET(button1));
  gtk_box_pack_start (GTK_BOX (vbox1), button1, FALSE, FALSE, 0);
  gtk_button_set_relief (GTK_BUTTON (button1), GTK_RELIEF_NONE);

  label1 = gtk_label_new (_("Case"));
  gtk_widget_show (GTK_WIDGET(label1));
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label1);
  
    g_signal_connect_swapped ((gpointer) button1, "clicked",
                            G_CALLBACK (case_build),
                            NULL);
 
  g_signal_connect_swapped ((gpointer) button1, "clicked",
                            G_CALLBACK (icon_affiche_ok),
                            NULL);

    g_signal_connect_swapped ((gpointer) button1, "clicked",
                            G_CALLBACK (gtk_widget_destroy),
                            window1);				
  return window1;
}	

//******************************* construction instruction case bash insert
void case_build (void)
{
    int num_conteur=0;
	gint tampon_num;
	tampon_num = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinbutton1));
	
	doc_insert_at_cursor (cur_text_doc,"case \"$1\" in\n");
	while (num_conteur!=tampon_num)
	{
	doc_insert_at_cursor (cur_text_doc,(_("\noption)\n#inserer ici le script pour cette option\n\n;;")));
	num_conteur++;	
	}	
doc_insert_at_cursor (cur_text_doc,"\nesac\n");
}

//******************************* fenetre de construction commande awk
GtkWidget* awk (void)
{
  GtkWidget *window1,*vbox1,*hbox2,*vbox5,*vbox6,*hbox3,*image1,*label9,*label29,*label8,*vbox3,*vbox4,*label5,*label7,*hbox1,*button1,*button2,*image2;
	GtkWidget *alignment1;
	icon_affiche_stop();

  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window1), _((_("Aide Awk"))));
  gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);
  gtk_window_set_resizable (GTK_WINDOW (window1), FALSE);
  gtk_widget_show(GTK_WIDGET(window1));

  vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (GTK_WIDGET(vbox1));
  gtk_container_add (GTK_CONTAINER (window1), vbox1);

  hbox2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox2));
  gtk_box_pack_start (GTK_BOX (vbox1), hbox2, TRUE, TRUE, 0);

  vbox5 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (GTK_WIDGET(vbox5));
  gtk_box_pack_start (GTK_BOX (hbox2), vbox5, TRUE, TRUE, 0);

  vbox6 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (GTK_WIDGET(vbox6));
  gtk_box_pack_start (GTK_BOX (vbox5), vbox6, TRUE, TRUE, 0);

  checkbutton_awk1 = gtk_check_button_new ();
  gtk_widget_show (GTK_WIDGET(checkbutton_awk1));
  gtk_box_pack_start (GTK_BOX (vbox6), checkbutton_awk1, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton_awk1), TRUE);

  alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (GTK_WIDGET(alignment1));
  gtk_container_add (GTK_CONTAINER (checkbutton_awk1), alignment1);

  hbox3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox3));
  gtk_container_add (GTK_CONTAINER (alignment1), hbox3);

  image1 = gtk_image_new_from_stock ("gtk-zoom-100", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (GTK_WIDGET(image1));
  gtk_box_pack_start (GTK_BOX (hbox3), image1, FALSE, FALSE, 0);

  label9 = gtk_label_new_with_mnemonic (_("Redirection"));
  gtk_widget_show (GTK_WIDGET(label9));
  gtk_box_pack_start (GTK_BOX (hbox3), label9, FALSE, FALSE, 0);

    label29 = gtk_label_new_with_mnemonic (_("Separateur"));
     gtk_widget_show (GTK_WIDGET(label29));
     gtk_box_pack_start (GTK_BOX (vbox6), label29, FALSE, FALSE, 0);
   
  entry_awk4 = gtk_entry_new ();
  gtk_widget_show (GTK_WIDGET(entry_awk4));
  gtk_box_pack_start (GTK_BOX (vbox6), entry_awk4, FALSE, FALSE, 0);

  label8 = gtk_label_new (_("Colonne"));
  gtk_widget_show (GTK_WIDGET(label8));
  gtk_box_pack_start (GTK_BOX (vbox6), label8, FALSE, FALSE, 0);

  entry_awk5 = gtk_entry_new ();
  gtk_widget_show (GTK_WIDGET(entry_awk5));
  gtk_box_pack_start (GTK_BOX (vbox6), entry_awk5, FALSE, FALSE, 0);

  vbox3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (GTK_WIDGET(vbox3));
  gtk_box_pack_start (GTK_BOX (hbox2), vbox3, TRUE, TRUE, 14);

  vbox4 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (GTK_WIDGET(vbox4));
  gtk_box_pack_start (GTK_BOX (vbox3), vbox4, FALSE, FALSE, 0);

  image2 = gtk_image_new_from_stock ("gtk-copy", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (GTK_WIDGET(image2));
  gtk_container_add (GTK_CONTAINER (vbox4), image2);

  label5 = gtk_label_new (_("Fichier"));
  gtk_widget_show (GTK_WIDGET(label5));
  gtk_box_pack_start (GTK_BOX (vbox4), label5, FALSE, FALSE, 0);
  gtk_misc_set_alignment (GTK_MISC (label5), 0.48, 0.5);

  entry_awk1 = gtk_entry_new ();
  gtk_widget_show (GTK_WIDGET(entry_awk1));
  gtk_box_pack_start (GTK_BOX (vbox4), entry_awk1, FALSE, FALSE, 0);

  label7 = gtk_label_new (_("Fichier de redirection"));
  gtk_widget_show (GTK_WIDGET(label7));
  gtk_box_pack_start (GTK_BOX (vbox4), label7, FALSE, FALSE, 0);

  entry_awk3 = gtk_entry_new ();
  gtk_widget_show (GTK_WIDGET(entry_awk3));
  gtk_box_pack_start (GTK_BOX (vbox4), entry_awk3, FALSE, FALSE, 0);

  /*hseparator1 = gtk_hseparator_new ();
  gtk_widget_show (GTK_WIDGET(hseparator1));
  gtk_box_pack_start (GTK_BOX (vbox1), hseparator1, TRUE, TRUE, 3);*/

  hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox1));
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);

  button1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (GTK_WIDGET(button1));
  gtk_box_pack_start (GTK_BOX (hbox1), button1, FALSE, TRUE, 23);
  gtk_button_set_relief (GTK_BUTTON (button1), GTK_RELIEF_NONE);

  button2 = gtk_button_new_from_stock ("gtk-apply");
  gtk_widget_show (GTK_WIDGET(button2));
  gtk_box_pack_start (GTK_BOX (hbox1), button2, TRUE, TRUE, 23);
  gtk_button_set_relief (GTK_BUTTON (button2), GTK_RELIEF_NONE);


     g_signal_connect_swapped ((gpointer) button1, "clicked",
			       G_CALLBACK (icon_affiche_ok),
			       NULL);

  g_signal_connect_swapped ((gpointer) button1, "clicked",
                            G_CALLBACK (gtk_widget_destroy),
                            window1);

     g_signal_connect_swapped ((gpointer) button2, "clicked",
			       G_CALLBACK (awk_apl),
			       NULL);
   
   
     g_signal_connect_swapped ((gpointer) button2, "clicked",
			       G_CALLBACK (icon_affiche_ok),
			       NULL); 
   
  g_signal_connect_swapped ((gpointer) button2, "clicked",
                            G_CALLBACK (gtk_widget_destroy),
                            window1);
   
  return window1; 
}

//******************************* construction commande awk insert
void  awk_apl             (void)
{
   gchar *tampon_fichier;
   gchar *tampon_redi;
   gchar *tampon_sep;   
   gchar *tampon_col;      
    
   tampon_fichier = gtk_editable_get_chars(GTK_EDITABLE(entry_awk1),0, -1);
   tampon_redi = gtk_editable_get_chars(GTK_EDITABLE(entry_awk3),0, -1);
   tampon_sep = gtk_editable_get_chars(GTK_EDITABLE(entry_awk4),0, -1);
   tampon_col = gtk_editable_get_chars(GTK_EDITABLE(entry_awk5),0, -1);

       doc_insert_at_cursor (cur_text_doc,(_("# Commande Awk (decoupage de fichier par colonne)\nawk -F \"")));
       doc_insert_at_cursor (cur_text_doc, tampon_sep);
       doc_insert_at_cursor (cur_text_doc,"\" '{print $");
          doc_insert_at_cursor (cur_text_doc, tampon_col);
       doc_insert_at_cursor (cur_text_doc,"}' ");
	  doc_insert_at_cursor (cur_text_doc, tampon_fichier);   

           if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton_awk1)))
     {
       doc_insert_at_cursor (cur_text_doc," > ");
       doc_insert_at_cursor (cur_text_doc, tampon_redi);  
     }   
}

//******************************* fonction de sauvegarde
void file_save_bug (void)
{
  if (! get_page_text()) return;

  text_doc_save (cur_text_doc, ".fichierbackup");
		 icon_log_logmemo();
      log_to_memo (_("Scan fichier [START]"), NULL, LM_NORMAL);
}

//******************************* editeur de chemin path
GtkWidget* selection_path (void)
{
  GtkWidget *pFileSelection;
  icon_affiche_stop();

	 icon_man_logmemo();
  log_to_memo (_("[Séléctionnez un  PATH] pour inserer dans le fichier"), NULL, LM_NORMAL);  
  /* Creation du titre de la fenetre */
  //gchar *sUtf8 = g_locale_to_utf8("Sélectionnez un fichier", -1,
		//	   NULL, NULL, NULL);

  /* Creation de la fenetre de selection */
  //pFileSelection = gtk_file_selection_new();
  /*gtk_window_set_modal(GTK_WINDOW(pFileSelection), TRUE);

  g_free(sUtf8);
  sUtf8 = NULL;

  switch(gtk_dialog_run(GTK_DIALOG(pFileSelection)))
    {
    case GTK_RESPONSE_OK:
      sChemin = gtk_file_selection_get_filename(pFileSelection);
      doc_insert_at_cursor (cur_text_doc,sChemin);
      icon_log_logmemo();
      log_to_memo (_("%s PATH insert"), sChemin, LM_NORMAL);
      break;
    default:
      break;
    }
  gtk_widget_destroy(pFileSelection);
   icon_affiche_ok();*/

	//*** pour debug
	pFileSelection=gtk_window_new(GTK_WINDOW_TOPLEVEL);
  return pFileSelection;
}

//******************************* fenetre de construction scp
GtkWidget* win_scp (void)
{
  GtkWidget *window1;
	//GtkWidget *frame1,*vbox4,*vbox5,*table1,*label15,*label16,*label17,*label18,*hbox14,*button3,*button4,*label14;

icon_affiche_net();	
	
  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window1), _((_("Style de variable"))));
  gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);
  gtk_widget_show(GTK_WIDGET(window1));
  gtk_window_set_resizable (GTK_WINDOW (window1), FALSE);
  gtk_window_set_modal (GTK_WINDOW (window1), TRUE);
/*
  frame1 = gtk_frame_new (NULL);
  gtk_widget_show (GTK_WIDGET(frame1));
  gtk_container_add (GTK_CONTAINER (window1), frame1);
  gtk_container_set_border_width (GTK_CONTAINER (frame1), 3);
  gtk_frame_set_label_align (GTK_FRAME (frame1), 0.06, 0.71);

  vbox4 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (GTK_WIDGET(vbox4));
  gtk_container_add (GTK_CONTAINER (frame1), vbox4);

  vbox5 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (GTK_WIDGET(vbox5));
  gtk_box_pack_start (GTK_BOX (vbox4), vbox5, TRUE, TRUE, 0);

  table1 = gtk_table_new (4, 2, FALSE);
  gtk_widget_show (GTK_WIDGET(table1));
  gtk_box_pack_start (GTK_BOX (vbox5), table1, TRUE, TRUE, 0);

  label15 = gtk_label_new (_("Utilisateur : "));
  gtk_widget_show (GTK_WIDGET(label15));
  gtk_table_attach (GTK_TABLE (table1), label15, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label15), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label15), 0, 0.5);

  label16 = gtk_label_new (_("IP Host :"));
  gtk_widget_show (GTK_WIDGET(label16));
  gtk_table_attach (GTK_TABLE (table1), label16, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label16), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label16), 0, 0.5);

  entry_scp1 = gtk_entry_new ();
  gtk_widget_show (GTK_WIDGET(entry_scp1));
  gtk_table_attach (GTK_TABLE (table1), entry_scp1, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entry_scp2 = gtk_entry_new ();
  gtk_widget_show (GTK_WIDGET(entry_scp2));
  gtk_table_attach (GTK_TABLE (table1), entry_scp2, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label17 = gtk_label_new (_("Fichier source sur le host distant : "));
  gtk_widget_show (GTK_WIDGET(label17));
  gtk_table_attach (GTK_TABLE (table1), label17, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label17), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label17), 0, 0.5);

  label18 = gtk_label_new (_("Fichier destination : "));
  gtk_widget_show (GTK_WIDGET(label18));
  gtk_table_attach (GTK_TABLE (table1), label18, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label18), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label18), 0, 0.5);

  entry_scp3 = gtk_entry_new ();
  gtk_widget_show (GTK_WIDGET(entry_scp3));
  gtk_table_attach (GTK_TABLE (table1), entry_scp3, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entry_scp4 = gtk_entry_new ();
  gtk_widget_show (GTK_WIDGET(entry_scp4));
  gtk_table_attach (GTK_TABLE (table1), entry_scp4, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  hbox14 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox14));
  gtk_box_pack_start (GTK_BOX (vbox4), hbox14, FALSE, FALSE, 0);

  button3 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (GTK_WIDGET(button3));
  gtk_box_pack_start (GTK_BOX (hbox14), button3, FALSE, TRUE, 0);
  gtk_button_set_relief (GTK_BUTTON (button3), GTK_RELIEF_NONE);

  button4 = gtk_button_new_from_stock ("gtk-apply");
  gtk_widget_show (GTK_WIDGET(button4));
  gtk_box_pack_start (GTK_BOX (hbox14), button4, FALSE, TRUE, 0);
  gtk_button_set_relief (GTK_BUTTON (button4), GTK_RELIEF_NONE);

  label14 = gtk_label_new (_("Commande SCP"));
  gtk_widget_show (GTK_WIDGET(label14));
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label14);
  gtk_label_set_justify (GTK_LABEL (label14), GTK_JUSTIFY_LEFT);	
	

g_signal_connect_swapped ((gpointer) button4, "clicked",
                            G_CALLBACK (icon_affiche_ok),
                            NULL);	
							
    g_signal_connect_swapped ((gpointer) button4, "clicked",
                            G_CALLBACK (gen_scp),
                            NULL);

    g_signal_connect_swapped ((gpointer) button4, "clicked",
                            G_CALLBACK (gtk_widget_destroy),
                            window1);

g_signal_connect_swapped ((gpointer) button3, "clicked",
                            G_CALLBACK (icon_affiche_ok),
                            NULL);	
	
    g_signal_connect_swapped ((gpointer) button3, "clicked",
                            G_CALLBACK (gtk_widget_destroy),
                            window1);
*/
return window1;	
}	

//******************************* Construction commande scp
void gen_scp            (void)
{
   gchar *tampon1, *tampon2, *tampon3, *tampon4;
        
   tampon1 = gtk_editable_get_chars(GTK_EDITABLE(entry_scp1),0, -1);
   tampon2 = gtk_editable_get_chars(GTK_EDITABLE(entry_scp2),0, -1);
   tampon3 = gtk_editable_get_chars(GTK_EDITABLE(entry_scp3),0, -1);
   tampon4 = gtk_editable_get_chars(GTK_EDITABLE(entry_scp4),0, -1);

doc_insert_at_cursor (cur_text_doc,"\n# Copie de fichier avec SCP\n");
doc_insert_at_cursor (cur_text_doc,"scp ");
doc_insert_at_cursor (cur_text_doc, tampon1);
doc_insert_at_cursor (cur_text_doc,"@");
doc_insert_at_cursor (cur_text_doc, tampon2);
doc_insert_at_cursor (cur_text_doc,":");
doc_insert_at_cursor (cur_text_doc, tampon3);
doc_insert_at_cursor (cur_text_doc," ");	
doc_insert_at_cursor (cur_text_doc, tampon4);

}

//******************************* Debut de script perl
void start_perl_script            (void)
{
time_t date;
    date = time(NULL);

  doc_insert_at_cursor (cur_text_doc, (_("#!/usr/bin/perl\n\n# Script perl Griffon: http://griffon.lasotel.fr\n# Date start script : "))); 
  doc_insert_at_cursor (cur_text_doc, ctime(&date)); 
  doc_insert_at_cursor (cur_text_doc, "\n\n");
  }

//******************************* Term insert help
void term_df(gpointer user_data){vte_terminal_feed_child (user_data,"df -h\n",-1);}
void term_aspiration_web(gpointer user_data){vte_terminal_feed_child (user_data,"wget -r -k -E -np http://website.net",-1);}
void term_long_rm(gpointer user_data){vte_terminal_feed_child (user_data,"find /path/dir/ -name \"name_file*\" -exec rm {} \\;",-1);}
void term_find(gpointer user_data){vte_terminal_feed_child (user_data,"find /path/dir/ -name \"name_file*\"",-1);}
void term_screen(gpointer user_data){vte_terminal_feed_child (user_data,"screen -x -R\n",-1);}
void term_wc(gpointer user_data){vte_terminal_feed_child (user_data,"wc -l",-1);}
void term_wget(gpointer user_data){vte_terminal_feed_child (user_data,"wget 'http://url.web.net' -O /path/dir/file.html",-1);}
void term_ls_color(gpointer user_data){vte_terminal_feed_child (user_data,"ls -l --color | more\n",-1);}
void term_nice(gpointer user_data){vte_terminal_feed_child (user_data,"nice -n 19 ./script",-1);}
void term_comp_targz(gpointer user_data){vte_terminal_feed_child (user_data,"tar -zcvf file.tar.gz /path/dir_or_file",-1);}
void term_decomp_targz(gpointer user_data){vte_terminal_feed_child (user_data,"tar -zxvf file.tar.gz",-1);}
void term_diff(gpointer user_data){vte_terminal_feed_child (user_data,"diff file1 file2",-1);}

//******************************* Perl insert help
void perl_print            (void){  doc_insert_at_cursor (cur_text_doc, "print \" \";\n\n"); }
void perl_if            (void){  doc_insert_at_cursor (cur_text_doc, "if ()\n{\n\n}\n\n"); }
void perl_unless            (void){  doc_insert_at_cursor (cur_text_doc, "unless ()\n{\n\n}\n\n"); }
void perl_if_else            (void){  doc_insert_at_cursor (cur_text_doc, "if ()\n{\n\n}\nelse\n{\n\n}\n\n"); }
void perl_while (void){ doc_insert_at_cursor (cur_text_doc, "while ()\n{\n\n}\n\n");}
void perl_while_infinie (void){ doc_insert_at_cursor (cur_text_doc, "while (1)\n{\n\n}\n\n");}
void perl_for            (void){  doc_insert_at_cursor (cur_text_doc, "for (init;condition;cmd)\n{\n\n}\n\n"); }
void perl_foreach            (void){  doc_insert_at_cursor (cur_text_doc, (_("foreach $element (@tableau)\n{\n\n}\n\n"))); }
void perl_do_while            (void){  doc_insert_at_cursor (cur_text_doc, "do \n{\n\n} while ();\n\n"); }
void perl_fonction            (void){  doc_insert_at_cursor (cur_text_doc, (_("sub fonction\n{\n\n}\n\n"))); }
void perl_fonction_param            (void){  doc_insert_at_cursor (cur_text_doc, (_("sub fonction\n{\n\t# afficher l'argument 0 et 1 de la fonction\n\tprint $_[0];\n\tprint $_[1];\n}\n\n"))); }
void perl_exit           (void){  doc_insert_at_cursor (cur_text_doc, "exit();\n\n"); }
void perl_die            (void){  doc_insert_at_cursor (cur_text_doc, "die'Error';\n\n");}

//******************************* Rendre executable un script
void exe(void)
{
char commande[150];
strcpy(commande,"chmod u+x ");
strcat(commande,cur_text_doc->file_name);

int systemRet =system(commande);
if(systemRet == -1){return;}

icon_ok_logmemo();
 log_to_memo (_("[chmod] %s executable script OK"), cur_text_doc->file_name, LM_NORMAL);
	statusbar_msg (_("Chmod [OK]"));
	griffon_notify(_("Chmod [OK]"));
}

//******************************* Perl insert help
void perl_chomp            (void){  doc_insert_at_cursor (cur_text_doc, "chomp('');\n\n");}
void perl_length            (void){  doc_insert_at_cursor (cur_text_doc, (_("$nombre_de_caracteres=length($chaine);\n\n")));}
void perl_uc            (void){  doc_insert_at_cursor (cur_text_doc, (_("$chaine=uc($chaine);\n\n")));}
void perl_lc            (void){  doc_insert_at_cursor (cur_text_doc, (_("$chaine=lc($chaine);\n\n")));}
void perl_ucfirst            (void){   doc_insert_at_cursor (cur_text_doc, (_("$chaine=ucfirst($chaine);\n\n")));}

//******************************* Scan des fichier include 
void scan_include             (void)
{	
	//save_string_to_file_vide(confile.tea_autocomp,"");
	if (! get_page_text()) return;
  FILE *fich;
  char carac;

  char motrch[100],motrch2[100],motrch3[100],motrch4[100],motrch5[100],motrch6[100],motrch7[100],motrch8[100],motrch9[100],motrch10[100],motrch11[100],motrch12[100],mot_autocomp[500],mot[1000],mot2[1000],ligne[10],mot3[1000];
  int nbapparition=0,nbcarac=0,nbmot=0,counter=0;
  int nbligne=1;
	char *extension;
	int instruction=1;
  if (! get_page_text()) return;
	
     icon_affiche_bug();
	clear_list_include ();

  nbapparition=0,nbcarac=0,nbmot=0,nbligne=1;
  mot[0]='\0';
	mot_autocomp[0]='\0';
	mot2[0]='\0';
	mot3[0]='\0';
  motrch[0]='\0';
  motrch2[0]='\0';
  motrch3[0]='\0';
  motrch4[0]='\0';
  motrch5[0]='\0';
  motrch6[0]='\0';
  motrch7[0]='\0';
  motrch8[0]='\0';
  motrch9[0]='\0';
  motrch10[0]='\0';
  motrch11[0]='\0';
  motrch12[0]='\0';

	strcpy(motrch,"include");
	strcpy(motrch2,"require");
	strcpy(motrch3,"function");
	strcpy(motrch4,"REQUEST");
	strcpy(motrch5,"POST");
	strcpy(motrch6,"GET");
	strcpy(motrch7,"sub");
	strcpy(motrch8,"if");
	strcpy(motrch9,"while");
	strcpy(motrch10,"for");
	strcpy(motrch11,"case");
	strcpy(motrch12,"unless");
	
	GtkTextIter itstart, itend;

	add_to_list_err("",0);

  fich=fopen(cur_text_doc->file_name,"r");
  while ((carac =fgetc(fich)) != EOF)
    {
	//**************************** mot pour autocomp
	/*if (carac =='(' || carac =='=' || carac ==' ' || carac ==',' || carac =='\n')
	{
		if(strlen(mot_autocomp)>3)
		{
			gl_autocomp = glist_add_not_dup (gl_autocomp, mot_autocomp, 3);
			mot_autocomp[0]='\0';

          if (gl_autocomp)
             {
              g_completion_clear_items (at_editor);
              g_completion_add_items (at_editor, gl_autocomp);
             }
		}
	}*/

	if (carac !='\n' && carac !='\r' && carac !='\t' && carac !=' ' && carac !='\'' && carac !='\"' && carac !='#'  && carac !='(' && carac !=')' && carac !='{' && carac !='}' && carac !=';' && carac !=',' && carac !=':' && carac !='/'){strncat(mot_autocomp,&carac,1);}else{mot_autocomp[0]='\0';}
	//******************************* pour include ou require     
  if(counter==1)
  {
	  strncat(mot,&carac,1);
	  
	    if (carac =='\n' || carac =='\r')
	    {

			gchar **a = g_strsplit (mot, "\"", -1);	 
			if(a[1]!='\0')
			{
		    sprintf(ligne,"%d",nbligne); 
		 	 add_to_list(a[1],ligne);
	     }

    mot[0]='\0';
    ligne[0]='\0';
    counter=0;  
	    }   
  }  
	//******************************* pour fonction
  if(counter==2)
  {
	  strncat(mot,&carac,1);
  
	    if (carac =='\n' || carac =='\r')
	    {
			gchar **b = g_strsplit (mot, "\n", -1);
			gchar **a = g_strsplit (b[0], "function", -1);	 
			if(a[1]!='\0')
			{
		    sprintf(ligne,"%d",nbligne); 
		 	 add_to_list_fc(a[1],ligne);
	     }

    mot[0]='\0';
    ligne[0]='\0';
    counter=0;  
	    }
	} 

	//******************************* pour variable php
  if(counter==3)
  {
	  strncat(mot,&carac,1);
  
	    if (carac =='\n' || carac =='\r')
	    {
			gchar **b = g_strsplit (mot, "\n", -1);
			gchar **a = g_strsplit (b[0], "]", -1);
			if(a[0]!='\0')
			{			 
		    sprintf(ligne,"%d",nbligne); 
		 	 add_to_list_var(a[0],ligne);			
	     }

    mot[0]='\0';
    mot2[0]='\0';
    ligne[0]='\0';
    counter=0;  
	    }
   
  }  

	//******************************* pour fonction perl sub
  if(counter==4)
  {
	  strncat(mot,&carac,1);
  
	    if (carac =='\n' || carac =='\r')
	    {
			gchar **b = g_strsplit (mot, "\n", -1);
			gchar **a = g_strsplit (b[0], "sub", -1);	 
			if(a[1]!='\0')
			{
		    sprintf(ligne,"%d",nbligne); 
		 	 add_to_list_fc(a[1],ligne);
	     }

    mot[0]='\0';
    ligne[0]='\0';
    counter=0;  
	    }
	} 
    
if (counter==0)
{    

		if(carac=='_')
		{
			mot2[0]='\0';
		}

    if (carac =='\n' || carac =='\r')
    {   

//********************************* TEST ĈHECK FIN DE LIGNE
	if(strrchr(cur_text_doc->file_name,'.'))
	{
		extension = strrchr(cur_text_doc->file_name,'.');
		if(strcmp(".pl", extension) == 0 || strcmp(".php", extension) == 0 || strcmp(".c", extension) == 0 || strcmp(".h", extension) == 0)
		{
								size_t len = strlen(mot);
								if(len>0)
								{
										if(instruction==1 && mot[len-1]==')')
										{
										
											gtk_text_buffer_get_iter_at_line (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &itstart, nbligne);
											gtk_text_buffer_get_iter_at_line (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &itend, nbligne-1);
										 gtk_text_buffer_apply_tag_by_name (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), "err2", &itstart, &itend);
											sprintf(ligne,"%d",nbligne);
											add_to_list_err(mot,ligne);		
										}

								  if(mot[len-1]!='/' && mot[len-1]!=';' && mot[len-1]!='}' && mot[len-1]!=')' && mot[len-1]!=',' && mot[len-1]!=' ' && mot[len-1]!='.' && mot[len-1]!=')' && mot[len-1]!='}' && mot[0]!='#' && mot[0]!='/' && mot[0]!='*' && mot[len-1]!='\t' && strncmp(mot,"else",strlen(mot))!=0 && mot3[0]!='#' && mot3[0]!='/' && mot3[0]!='*' && mot[len-1]!='{' && mot[len-1]!='(' && mot3[0]!='<' && mot3[0]!='?' && mot3[0]!='p' && strncmp(mot,"case",strlen(mot))!=0)
										{
											gtk_text_buffer_get_iter_at_line (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &itstart, nbligne);
											gtk_text_buffer_get_iter_at_line (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &itend, nbligne-1);
										 gtk_text_buffer_apply_tag_by_name (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), "err2", &itstart, &itend);
										}
								}
		}
	}
		
			instruction=1;
    mot[0]='\0';
	 		mot2[0]='\0';
			mot3[0]='\0';
    nbligne++;
    }

      nbcarac++;
		if(strlen(mot)>900){mot[0]='\0';}
		if(strlen(mot2)>900){mot2[0]='\0';}
		if(strlen(mot3)>900){mot3[0]='\0';}

      if (mot[0] != '\0')
		{
		  if (carac !='\n' || carac !='\r'){strncat(mot,&carac,1);}			  				
		}
		
	if (carac !='\n' && carac !='\r' && carac!='\0' && carac!=' ' && carac!='\t'){strncat(mot3,&carac,1);}	

      if (mot[0] != '\0' && isalnum(carac) != 0)
		{
		  if (carac !='\n' || carac !='\r'){strncat(mot2,&carac,1);	}		
		}
		
		
      if (mot[0] != '\0' && isalnum(carac) == 0)
		{
		  if (strncmp(motrch,mot,strlen(motrch))==0 || strncmp(motrch2,mot,strlen(motrch2))==0) // si motrch=mot pour include 
	    {
	      nbapparition++;
	      if(nbapparition==1)
			{
			  nbcarac--;
			}
	        
			counter=1;      
	    }	

		  if (strncmp(motrch3,mot,strlen(motrch3))==0) 
	    {
	      nbapparition++;
	      if(nbapparition==1)
			{
			  nbcarac--;
			}
	        
			counter=2;// mode fonction      
	    }	

		  if (strncmp(motrch4,mot2,strlen(motrch4))==0 || strncmp(motrch5,mot2,strlen(motrch5))==0 || strncmp(motrch6,mot2,strlen(motrch6))==0) 
	    {
	      nbapparition++;
	      if(nbapparition==1)
			{
			  nbcarac--;
			}
	        
			counter=3;// mode var      
	    }	

		  if (strncmp(motrch7,mot,strlen(motrch7))==0) 
	    {
	      nbapparition++;
	      if(nbapparition==1)
			{
			  nbcarac--;
			}
	        
			counter=4;// mode fonction perl sub      
	    }			

		  if (strncmp(motrch8,mot2,strlen(motrch8))==0 || strncmp(motrch9,mot2,strlen(motrch9))==0 || strncmp(motrch10,mot2,strlen(motrch10))==0 || strncmp(motrch11,mot2,strlen(motrch11))==0 || strncmp(motrch12,mot2,strlen(motrch12))==0) 
	    {	        
				instruction=0;   
	    }			


	}

      if (mot[0] == '\0' && isalnum(carac) != 0)
		{
		  strncat(mot,&carac,1);    
			strncat(mot2,&carac,1); 
			
		  nbmot++;                   
		}
  }
    
    }

  fclose(fich);
}

//******************************* Perl insert help
void perl_read            (void){  doc_insert_at_cursor (cur_text_doc, (_("open (FICHIER, \"nom_du_fichier\");\nwhile ($ligne=<FICHIER>)\n{\nprint $ligne;\n}\n\nclose FICHIER;\n\n"))); }
void perl_writh           (void){  doc_insert_at_cursor (cur_text_doc, (_("open (FICHIER, \">>nom_du_fichier\");\nprint FICHIER \"Text écrit dans le fichier\";\nclose FICHIER;\n\n"))); }
void perl_writh2           (void){  doc_insert_at_cursor (cur_text_doc, (_("open (FICHIER, \">nom_du_fichier\");\nprint FICHIER \"Text écrit dans le fichier\";\nclose FICHIER;\n\n"))); }
void perl_regular           (void){  doc_insert_at_cursor (cur_text_doc, (_("$chaine =~/chaine recherchée/"))); }
void perl_replace           (void){  doc_insert_at_cursor (cur_text_doc, (_("$chaine =~ s/chaine recherchée/chaine de remplacement/g;\n\n"))); }

//******************************* insert debut de script php
void start_php_script            (void)
{
time_t date;
date = time(NULL);

  doc_insert_at_cursor (cur_text_doc, (_("<?\n\n/* \n * Script PHP Griffon: http://griffon.lasotel.fr\n * Date start script : "))); 
  doc_insert_at_cursor (cur_text_doc, ctime(&date)); 
  doc_insert_at_cursor (cur_text_doc, " *\n */\n\n?>");   
}

//******************************* Perl insert help
void perl_tableau (void){doc_insert_at_cursor (cur_text_doc, (_("@tableau = (\"var0\",\"var1\",\"var2\");\n# affiche var0\nprint $tableau[0];\n\n"))); }
void perl_saisie_clavier (void){doc_insert_at_cursor (cur_text_doc, (_("#  <STDIN> attend une saisie clavier\nwhile (<STDIN>)\n{\n\t# évite \\n du dernier champ\n\tchop;\n\t# affiche le caractère saisi sans son \\n\n\tprint;\n}\n\n"))); }
void perl_empiler_tableau (void){doc_insert_at_cursor (cur_text_doc, (_("# Empile valeur en fin de tableau\npush $tableau, (\"valeur\");\n\n"))); }
void perl_empiler_tableau_start (void){doc_insert_at_cursor (cur_text_doc, (_("# Empile valeur en début de tableau\nshift $tableau, (\"valeur\");\n\n"))); }
void perl_arguments (void){doc_insert_at_cursor (cur_text_doc, (_("# le tableau ARGV contient la liste des arguments du programme.\n$argument0 = $ARGV[0];\n\n"))); }
void perl_split (void){doc_insert_at_cursor (cur_text_doc, (_("# découpe la variable $chaine par rapport au séparateur \";\" et place le résultat dans un tableau\n@tableau = split(/;/, $chaine);\n\n"))); }
void perl_dbi_connexion (void){doc_insert_at_cursor (cur_text_doc, (_("# Charger le module DBI\nuse DBI;\n\n# Paramètres de connexion à la base de données\n$bd = 'nom_de_base';\n$serveur = 'localhost';\t\t# Il est possible de mettre une adresse IP\n$identifiant = 'root';\t\t# Identifiant\n$motdepasse  = 'admin';\t\t# Mot de passe\n\n# Connexion à la base de données MySQL\n$dbh = DBI->connect( \"DBI:mysql:$bd:$serveur\", $identifiant, $motdepasse ) or die \"Connexion impossible à la base de données $bd !\";\n\n"))); }
void perl_dbi_select_while (void){doc_insert_at_cursor (cur_text_doc, (_("# Boucle sur une requete MySql\n$query = \"SELECT groupe_attribut FROM table WHERE condition\";\n$sth = $dbh->prepare($query);\n$sth->execute;\n\nwhile($row = $sth->fetchrow_hashref)\n{\n\n\t# Récuperation des valeurs  \n\t$champs_mysql = \"$row->{champs_mysql}\";\n\n}"))); }
void perl_dbi_query (void){doc_insert_at_cursor (cur_text_doc, (_("# simple requete MySql\n$query = \"\";\n$sth = $dbh->do($query);\n\n"))); }
void perl_chop (void){doc_insert_at_cursor (cur_text_doc, (_("chop($chaine);\n\n"))); }
void perl_mime_mail_simple (void){doc_insert_at_cursor (cur_text_doc, (_("use MIME::Lite;\n\n####### ENVOI EMAIL\nmy $Message = new MIME::Lite\nFrom =>\"NOM FROM <from\\@from.net>\",\nTo =>$email_destination,\nSubject =>$sujet,\nType    =>\'TEXT\',\nData =>\"$message\",\nContent-Transfer-Encoding => \'quoted-printable\';\n\n$Message->send;\n\n"))); }
void perl_date (void){doc_insert_at_cursor (cur_text_doc, (_("($sec,$min,$heure,$mjour,$mois,$annee,$sjour,$ajour,$isdst) = localtime(time);\n$mois=$mois+1;\nif($mois<10){$mois=\"0\".$mois;}\nif($mjour<10){$mjour=\"0\".$mjour;}\n$annee=1900+$annee;\n\n$date_sql=$annee.\"-\".$mois.\"-\".$mjour;\n\n"))); }
void perl_mime_mail_pj (void){doc_insert_at_cursor (cur_text_doc, (_("use MIME::Lite;\n\n####### ENVOI EMAIL AVEC PJ\nmy $Message = new MIME::Lite\nFrom =>\"NOM_FROM <from\\@from.net>\",\nTo =>$email_destination,\nSubject =>$sujet,\nType =>\'multipart/mixed\';\n\nattach $Message\nType =>'TEXT',\nData =>$message;\n\nattach $Message\nType =>\'application/txt\',\nPath =>$chemin_local_du_fichier,\nFilename =>$nom_fichier_txt;\n\n$Message->send;\n\n"))); }
void perl_dbi_query_select (void){doc_insert_at_cursor (cur_text_doc, (_("$query2 = \"SELECT groupe_attribut FROM table WHERE condition\";\n$sth2 = $dbh->prepare($query2);\n$sth2->execute;\n\n$row2 = $sth2->fetchrow_hashref;\n$champs_mysql = \"$row2->{champs_mysql}\";\n\n"))); }
void perl_uri_encode (void){doc_insert_at_cursor (cur_text_doc, (_("use URI::Escape;\n\n$url_encode = uri_escape($url_a_encoder);\n\n"))); }
void perl_url_get (void){doc_insert_at_cursor (cur_text_doc, (_("use LWP::Simple;\n\n$doc_retour = get($url);\n\n"))); }
void perl_ftp_connexion (void){doc_insert_at_cursor (cur_text_doc, (_("use Net::FTP;\n\n$FTP_HOST=\"\";\n$FTP_USER=\"\";\n$FTP_PASSWORD=\"\";\n\n$ftp = Net::FTP->new($FTP_HOST, Debug => 0);\n$ftp->login($FTP_USER,$FTP_PASSWORD);\n\n"))); }
void perl_ftp_list (void){doc_insert_at_cursor (cur_text_doc, (_("$ftp->cwd(\"$repertoire_a_lister\");\n@ls = $ftp->ls(\".\");\n\nforeach $file (@ls)\n{\n\n\tprint \"$file\\n\";\n\n}\n\n"))); }
void perl_ftp_get (void){doc_insert_at_cursor (cur_text_doc, (_("$ftp->get($fichier_source_ftp,\"$fichier_destination_local\");\n\n"))); }
void perl_ftp_put (void){doc_insert_at_cursor (cur_text_doc, (_("$ftp->put($fichier_local,\"$fichier_destination_ftp\");\n\n"))); }
void perl_socket_srv (void){doc_insert_at_cursor (cur_text_doc, (_("use IO::Socket::INET ;\n\n$server = new IO::Socket::INET(\nType=>SOCK_STREAM,\n                LocalPort => $port ,\n                Proto => 'tcp' ,\n                Listen => 0 ,\n                Reuse => 1,\n\n);\n\n\nwhile($client = $server->accept)\n{\n\n\tif(fork())\n\t{\n\t$ipclient_connect=sprintf \"%s\", $client->peerhost;\n\t$client->autoflush(1);\n\n\tprint $client \"$hostname\\n\";\n\n\t\twhile (1)\n\t\t{\n\t\t$chaine_client=<$client>;\n\t\tprint $chaine_client.\"\\n\";\n\n\n\t\t######### Envoyer au client\n\t\tprint $client \"$chaine_a_envoyer_au_client\";\n\t\t}\n\tclose $client;\n\texit();\n\n\t}\n}\n\n"))); }
void perl_socket_client (void){doc_insert_at_cursor (cur_text_doc, "use IO::Socket;\n\n\n$socket = IO::Socket::INET->new(Proto    => \"tcp\",\n                                            PeerAddr => $ip_serveur,\n                                            PeerPort => $port)\n\tor die \"Failed : $@\\n\";\n\n\n\nwhile($reponse=<$socket>)\n{\n\t########## Envoi sur la socket\n\tprint $socket \"load\\n\";\n\n}\n\n"); }
void perl_substr (void){doc_insert_at_cursor (cur_text_doc, (_("$chaine_resultat=substr($chaine, $int_indice_debut, $int_longueur);\n\n"))); }
void perl_tube (void){doc_insert_at_cursor (cur_text_doc, (_("$fichier=\"tail -f -n 1 /votre/fichier/tube |\";\nopen(FICHIER, $fichier);\n\nwhile($ligne=<FICHIER>)\n{\n\tprint $ligne;\n}\n\nclose FICHIER;\n\n"))); }
void perl_stdin (void){doc_insert_at_cursor (cur_text_doc, (_("while ($ligne=<STDIN>)\n{\n\n\n}\n\n"))); }
void perl_dbi_lastid (void){doc_insert_at_cursor (cur_text_doc, "$lastid=$dbh->{'mysql_insertid'};\n\n"); }
void perl_rand (void){doc_insert_at_cursor (cur_text_doc, (_("$rand=rand($nombre_max);\n\n"))); }
void perl_stat (void){doc_insert_at_cursor (cur_text_doc, (_("# dev\t\tnuméro de device du système de fichiers\n# ino\t\tnuméro d'inode\n# mode\t\tdroits du fichier (type et permissions)\n# nlink\t\tnombre de liens (hard) sur le fichier\n# uid\t\tID numérique de l'utilisateur propriétaire du fichier\n# gid\t\tID numérique du groupe propriétaire du fichier\n# rdev\t\tl'identificateur de device (fichiers spéciaux uniquement)\n# size\t\ttaille totale du fichier, en octets\n# atime\t\tdate de dernier accès en secondes depuis l'origine des temps\n# mtime\t\tdate de dernière modification en secondes depuis l'origine des temps\n# ctime\t\tdate de dernière modification de l'inode en secondes depuis l'origine des temps (*)\n# blksize\t\ttaille de blocs préférée pour les E/S sur fichiers\n# blocks\t\tnombre de blocs réellement occupés\n\n($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks)= stat($filename);\n\n"))); }
void perl_ftp_cwd (void){doc_insert_at_cursor (cur_text_doc, (_("$ftp->cwd(\"$dir\");\n"))); }
void perl_ftp_delete (void){doc_insert_at_cursor (cur_text_doc, (_("$ftp->delete(\"$file\");\n"))); }
void perl_dbi_rows (void){doc_insert_at_cursor (cur_text_doc, (_("$rows = $sth2->rows;\n"))); }

//******************************* Mysql insert help
void mysql_insert (void){doc_insert_at_cursor (cur_text_doc, (_("INSERT INTO votre_table (nomattribut1,nomattributN) values ('valeur1','valeurN')"))); }
void mysql_update (void){doc_insert_at_cursor (cur_text_doc, (_("UPDATE nomdetable Set nomattribut1=expression1,nomattributN=expressionN WHERE condition"))); }
void mysql_select (void){doc_insert_at_cursor (cur_text_doc, (_("SELECT groupe_attribut FROM table WHERE condition"))); }
void mysql_delete (void){doc_insert_at_cursor (cur_text_doc, (_("DELETE attributN FROM nomdetable WHERE condition"))); }

//******************************* PHP insert help
void php_commentaire (void){doc_insert_at_cursor (cur_text_doc, "/*\n *\n *\n *\n */\n\n"); }
void php_mysql_connexion (void){doc_insert_at_cursor (cur_text_doc, "$dbhost=\"ip_host_name\";\n$dblogin=\"login\";\n$dbpassword=\"password\";\n$dbname=\"nom_de_base\";\n\n$db=mysql_connect($dbhost,$dblogin,$dbpassword);\nmysql_selectdb($dbname,$db);\n\nmysql_query(\"SET NAMES 'utf8'\");\n\n"); }
void php_mysql_while (void){doc_insert_at_cursor (cur_text_doc, (_("$inforeq = \"SELECT champs FROM tables WHERE conditions\";\n$infoq = mysql_query($inforeq);\n\nwhile($ligne = mysql_fetch_object($infoq))\n{\n\t$champs=$ligne->champs;\n\n}\n\n"))); }
void php_mysql_query (void){doc_insert_at_cursor (cur_text_doc, "$requete = \"\";\n$result = mysql_query($requete);\n\n"); }
void php_foreach (void){doc_insert_at_cursor (cur_text_doc, (_("foreach ($tableau as $ligne_num => $ligne)\n{\n\n\n}\n\n"))); }
void php_split (void){doc_insert_at_cursor (cur_text_doc, (_("$tableau = preg_split(\"/[;]+/\", $variable);\n\n"))); }
void php_replace (void){doc_insert_at_cursor (cur_text_doc, (_("$chaine = eregi_replace (\"chaine à remplacer\",\"chaine de remplacement\",$chaine);\n\n"))); }
void php_file (void){doc_insert_at_cursor (cur_text_doc, (_("$tableau = file(\"http://url ou fichier\");\n\n"))); }
void php_find (void){doc_insert_at_cursor (cur_text_doc, (_("if(strstr($chaine, $chaine_recherche))\n{\n\n\n}\n\n"))); }
void php_function (void){doc_insert_at_cursor (cur_text_doc, (_("function nom_de_fonction($var1,$var2)\n{\n\n\n}\n\n"))); }
void php_stripslashes (void){doc_insert_at_cursor (cur_text_doc, (_("$chaine=stripslashes($chaine);\n\n"))); }
void php_mail (void){doc_insert_at_cursor (cur_text_doc, (_("mail(\"email\",\"sujet\",\"message\",\"From: nom <email@qui-envoi.net>\\nContent-Type: text;\\n\");\n\n"))); }
void php_insert_id (void){doc_insert_at_cursor (cur_text_doc, (_("$id_insert=mysql_insert_id();\n"))); }
void php_ftp_connect (void){doc_insert_at_cursor (cur_text_doc, (_("//****** Connexion serveur FTP\n$ftp_connexion = ftp_connect(\"$ip_serveur\");\n$ftp_login = ftp_login($ftp_connexion,\"$login\", \"$password\");\n$ftp_repertoire = ftp_chdir($ftp_connexion,\"/\");\n\n"))); }
void php_ftp_list_rep (void){doc_insert_at_cursor (cur_text_doc, (_("//******* Liste les fichiers de $repertoire dans un tableau\n$tableau = ftp_nlist($ftp_connexion,\"$repertoire\");\n\n"))); }
void php_ftp_put (void){doc_insert_at_cursor (cur_text_doc, (_("ftp_put($ftp_connexion,$path_fichier,$nom_fichier_depose, FTP_BINARY);\n\n"))); }
void php_ftp_delete (void){doc_insert_at_cursor (cur_text_doc, (_("ftp_delete($ftp_connexion, $fichier_supprime);\n\n"))); }
void php_h_utf8 (void){doc_insert_at_cursor (cur_text_doc, "header(\'Content-type: text/plain; charset=UTF-8\');\n\n"); }
void php_h_png (void){doc_insert_at_cursor (cur_text_doc, "Header(\"Content-type: image/png\");\n\n"); }
void php_date_mysql (void){doc_insert_at_cursor (cur_text_doc, (_("$now = date(\"Y-m-d H:i:s\");\n\n"))); }
void php_mysql_query_select (void){doc_insert_at_cursor (cur_text_doc, (_("$inforeq2 = \"SELECT champs FROM tables WHERE conditions\";\n$infoq2 = mysql_query($inforeq2);\n$ligne2 = mysql_fetch_object($infoq2);\n$champs=$ligne2->champs;\n\n"))); }
void php_strlen (void){doc_insert_at_cursor (cur_text_doc, (_("$nbr_caracteres=strlen($chaine);\n\n"))); }
void php_urlencode (void){doc_insert_at_cursor (cur_text_doc, (_("$url=urlencode($chaine);\n\n"))); }
void php_rand (void){doc_insert_at_cursor (cur_text_doc, (_("$valeur=rand($min,$max );\n\n"))); }
void php_mb_substr_count (void){doc_insert_at_cursor (cur_text_doc, (_("$nbr_occurrences=mb_substr_count($chaine, $chaine_a_trouver);\n\n"))); }
void php_var_post (void){doc_insert_at_cursor (cur_text_doc, "print_r($_POST);\n\n"); }
void echo_php           (void){  doc_insert_at_cursor (cur_text_doc, "echo \"\";\n\n"); }
void php_strip_tags           (void){  doc_insert_at_cursor (cur_text_doc, (_("$chaine=strip_tags($chaine_html);\n\n"))); }
void php_num_rows           (void){  doc_insert_at_cursor (cur_text_doc, (_("$num_rows=mysql_num_rows($result);\n"))); }
void php_explode           (void){  doc_insert_at_cursor (cur_text_doc, (_("$tableau = explode(" ", $chaine);\n"))); }
void php_unlink           (void){  doc_insert_at_cursor (cur_text_doc, (_("unlink($file);\n"))); }
void php_fputs           (void){  doc_insert_at_cursor (cur_text_doc, (_("$fp=fopen($file,\"w\");\nfputs($fp, \"text in file\");\nfclose($fp);\n"))); }
void php_count           (void){  doc_insert_at_cursor (cur_text_doc, (_("$nbr=count($tableau);\n"))); }
void php_dir           (void){  doc_insert_at_cursor (cur_text_doc, (_("$mydir = \"/dir/\";\nif ($dir = opendir($mydir)) \n{\n\twhile (($file = readdir($dir)) !== false) \n\t{\n\t\tif($file != \"..\" && $file != \".\")\n\t\t{\n\t\t$filelist[] = $file;\n\t\t}\n\t} \nclosedir($dir);\n}"))); }
void php_addslashes(void){  doc_insert_at_cursor (cur_text_doc, (_("$str=addslashes($str);\n"))); }
void php_dl_csv(void){  doc_insert_at_cursor (cur_text_doc, (_("header(\"Cache-control: private\");\n\n$filename=\"fichier_csv_name.csv\";\n\nif (strstr($_SERVER\[\"HTTP_USER_AGENT\"], \"MSIE\"))\n{header(\"Content-type: text/tab-separated-values; charset=iso-8859-1\");\nheader(\"Content-Disposition: filename=$filename\" );\n}\nelse{\nheader(\"Content-type: text/octet-stream; charset=iso-8859-1\");\nheader(\"Content-Disposition: attachment; filename=$name\");\n}\n\n$csv=\"CSV1;CSV2;CSV3;\";\n\necho $csv;\n"))); }

//********************************* HTML insert help
void html_css (void){doc_insert_at_cursor (cur_text_doc, "<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\" />\n"); }
void html_reload (void){doc_insert_at_cursor (cur_text_doc, "<meta http-equiv=\"refresh\" content=\"30;\">\n"); }
void html_largeur (void){doc_insert_at_cursor (cur_text_doc, "width=\"\" "); }
void html_hauteur (void){doc_insert_at_cursor (cur_text_doc, "height=\"\" "); }
void html_rows (void){doc_insert_at_cursor (cur_text_doc, "rows=\"\" "); }
void html_cols (void){doc_insert_at_cursor (cur_text_doc, "cols=\"\" "); }
void html_style (void){doc_insert_at_cursor (cur_text_doc, "style=\"\" "); }
void html_form (void){doc_insert_at_cursor (cur_text_doc, (_("<form name=\"nom_formulaire\" method=\"post\" action=\"page.php\">\n\n\n<input type=\"submit\" value=\"Valider\">\n</form>"))); }
void html_input_text (void){doc_insert_at_cursor (cur_text_doc, "<input type=\"text\" name=\"nom_text\" value=\"default\">\n"); }
void html_input_textarea (void){doc_insert_at_cursor (cur_text_doc, (_("<textarea name=\"nom_textarea\">\n\n\tTexte affiché dans la balise textarea\n\n</textarea>\n"))); }
void html_input_select (void){doc_insert_at_cursor (cur_text_doc, (_("<select name=\"nom_select\">\n<option selected value=\"valeur\">VALEUR</option>\n<option value=\"valeur2\">VALEUR2</option>\n</select>\n"))); }
void html_favicon (void){doc_insert_at_cursor (cur_text_doc, "<link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"/images/favicon.ico\" />\n"); }



void html_div ()
{
  if (! get_page_text()) return;
  
  gchar *t = NULL;
  gchar *buf = doc_get_sel (cur_text_doc);

  if (! buf){doc_insert_at_cursor (cur_text_doc, "<div id=\"\"></div>");doc_move_cursor_backw_middle_tags (cur_text_doc);}
  else{t = g_strconcat ("<div id=\"\">\n", buf, "\n</div>", NULL);doc_rep_sel (cur_text_doc, t);doc_move_to_pos_bw_quote (cur_text_doc);}

  g_free (buf);
  g_free (t);
}

//********************************* Haccess fc
void htaccess_charset_utf8 (void){doc_insert_at_cursor (cur_text_doc, "AddDefaultCharset utf-8\n"); }
void htaccess_deny_all (void){doc_insert_at_cursor (cur_text_doc, "Deny from all\n"); }
void htaccess_accept_ip (void){doc_insert_at_cursor (cur_text_doc, "Allow from 127.0.0.1\n"); }
void htaccess_redirection_erreur (void){doc_insert_at_cursor (cur_text_doc, "ErrorDocument 403 http://griffon.lasotel.fr\nErrorDocument 404 http://griffon.lasotel.fr\n"); }
void htaccess_urlrw_on (void){doc_insert_at_cursor (cur_text_doc, "RewriteEngine on\n"); }
void htaccess_urlrw (void){doc_insert_at_cursor (cur_text_doc, (_("RewriteRule ^source.html$  destination.php?id=1  [L]\n"))); }
void htaccess_urlrw_exp (void){doc_insert_at_cursor (cur_text_doc, (_("RewriteRule ^source/([a-zA-Z0-9_]+)-([0-9]+).html$  destination.php?option1=$1&option2=$2  [L]\n"))); }

//********************************* HTML insert css
void css_body (void){doc_insert_at_cursor (cur_text_doc, "body\n{\n\n}\n"); }
void css_link (void){doc_insert_at_cursor (cur_text_doc, "a\n{\n\n}\n"); }
void css_link_hover (void){doc_insert_at_cursor (cur_text_doc, "a:hover\n{\n\n}\n"); }
void css_div (void){doc_insert_at_cursor (cur_text_doc, "#ID_DIV\n{\n\n}\n"); }
void css_bg_img (void){doc_insert_at_cursor (cur_text_doc, "background-image: url(images_path/image.png);\n"); }
void css_bg_img_norep (void){doc_insert_at_cursor (cur_text_doc, "background-repeat: no-repeat;\n"); }
void css_bg_couleur (void){doc_insert_at_cursor (cur_text_doc, "background: #FFF;\n"); }
void css_font_size (void){doc_insert_at_cursor (cur_text_doc, "font-size: 10pt;\n"); }
void css_font_style (void){doc_insert_at_cursor (cur_text_doc, "font-family: Abel, sans-serif;\n"); }
void css_font_color (void){doc_insert_at_cursor (cur_text_doc, "color: #B294A0;\n"); }
void css_font_deco_non (void){doc_insert_at_cursor (cur_text_doc, "text-decoration: none;\n"); }
void css_font_deco_und (void){doc_insert_at_cursor (cur_text_doc, "text-decoration: underline;\n"); }
void css_largeur (void){doc_insert_at_cursor (cur_text_doc, "width: 1090px;\n"); }
void css_hauteur (void){doc_insert_at_cursor (cur_text_doc, "height: 168px;\n"); }
void css_gauche (void){doc_insert_at_cursor (cur_text_doc, "left: 10px;\n"); }
void css_droit (void){doc_insert_at_cursor (cur_text_doc, "right: 10px;\n"); }
void css_haut (void){doc_insert_at_cursor (cur_text_doc, "top: 10px;\n"); }
void css_bas (void){doc_insert_at_cursor (cur_text_doc, "bottom: 10px;\n"); }
void css_fixed (void){doc_insert_at_cursor (cur_text_doc, "position:fixed;\n"); }
void css_relative (void){doc_insert_at_cursor (cur_text_doc, "position:relative;\n"); }
void css_absolute (void){doc_insert_at_cursor (cur_text_doc, "position:absolute;\n"); }
void css_none (void){doc_insert_at_cursor (cur_text_doc, "display:none;\n"); }
void css_block (void){doc_insert_at_cursor (cur_text_doc, "display:block;\n"); }
void css_inline (void){doc_insert_at_cursor (cur_text_doc, "display:inline;\n"); }
void css_valign_top (void){doc_insert_at_cursor (cur_text_doc, "vertical-align:top;\n"); }
void css_valign_bottom (void){doc_insert_at_cursor (cur_text_doc, "vertical-align:bottom;\n"); }

//********************************* insert javascript
void javascript_html (void){doc_insert_at_cursor (cur_text_doc, "<script type=\"text/javascript\">\n\n\n</script>\n"); }
void javascript_fonction (void){doc_insert_at_cursor (cur_text_doc, (_("function Nom_De_Votre_Fonction()\n{\n\n\n}\n\n"))); }
void javascript_value_par_id (void){doc_insert_at_cursor (cur_text_doc, (_("var Value=document.getElementById('VOTRE_ID').value;\n"))); }
void javascript_focus_id (void){doc_insert_at_cursor (cur_text_doc, (_("document.getElementById('VOTRE_ID').focus();\n"))); }
void javascript_value_select (void){doc_insert_at_cursor (cur_text_doc, (_("var selectValue = document.getElementById('VOTRE_ID').options[document.getElementById('VOTRE_ID').selectedIndex].value;\n"))); }
void javascript_id_load_file (void){doc_insert_at_cursor (cur_text_doc, (_("$('#VOTRE_ID').load('FICHIER_PHP_HTML');\n"))); }
void javascript_html_change (void){doc_insert_at_cursor (cur_text_doc, (_("document.getElementById('VOTRE_ID').innerHTML =\"VOTRE TEXT\";\n"))); }
void javascript_id_change (void){doc_insert_at_cursor (cur_text_doc, (_("document.getElementById('VOTRE_ID').value = \"VOTRE_TEXTE\";\n"))); }
void javascript_id_change_add (void){doc_insert_at_cursor (cur_text_doc, (_("document.getElementById('VOTRE_ID').value += \"VOTRE_TEXT\";\n"))); }
void javascript_onblur (void){doc_insert_at_cursor (cur_text_doc, "onBlur=\"\" "); }
void javascript_onchange (void){doc_insert_at_cursor (cur_text_doc, "onChange=\"\" "); }
void javascript_onclick (void){doc_insert_at_cursor (cur_text_doc, "onClick=\"\" "); }
void javascript_onfocus (void){doc_insert_at_cursor (cur_text_doc, "onFocus=\"\" "); }
void javascript_onload (void){doc_insert_at_cursor (cur_text_doc, "onLoad=\"\" "); }
void javascript_onmouseover (void){doc_insert_at_cursor (cur_text_doc, "onMouseOver=\"\" "); }
void javascript_onselect (void){doc_insert_at_cursor (cur_text_doc, "onSelect=\"\" "); }
void javascript_onsubmit (void){doc_insert_at_cursor (cur_text_doc, "onSubmit=\"\" "); }
void javascript_split (void){doc_insert_at_cursor (cur_text_doc, (_("//split VOTRE_CHAINE dans un tableau\nvar reg=new RegExp(\"[;]+\", \"g\");\nvar tableau=VOTRE_CHAINE.split(reg);"))); }
void javascript_file_contenu (void){doc_insert_at_cursor (cur_text_doc, (_("var Fichier = function Fichier(fichier)\n{\n\tif(window.XMLHttpRequest) obj = new XMLHttpRequest(); //Pour Firefox, Opera,...\n\n\telse if(window.ActiveXObject) obj = new ActiveXObject(\"Microsoft.XMLHTTP\"); //Pour Internet Explorer\n \n\telse return(false);\n\n\tif (obj.overrideMimeType) obj.overrideMimeType(\"text/xml\"); //Évite un bug de Safari\n\tobj.open(\"GET\", fichier, false);\n\tobj.send(null);\n\n\tif(obj.readyState == 4) return(obj.responseText);\n\telse return(false);\n}\n\nvar Contenu = Fichier('page.php');\n\n"))); }

void javascript_urlencode (void){doc_insert_at_cursor (cur_text_doc, "Url = \"http://example.com/index.html?url=\" + encodeURIComponent(myUrl);\n"); }

//******************************* BookMark tooltips
/*tooltip_bookmark (GtkSourceMarkAttributes *attributes,GtkSourceMark *mark,gpointer user_data)
{
		gchar *text=g_strconcat ("______________________________________________\n\n",user_data," \n",NULL);return text;
}*/

//******************************* surlignement style stabylo jaune fluo
void on_format_button_clicked ()
{	 
	if (! get_page_text()) return;

	GtkTextIter start;
	GtkTextIter end;
	gchar * txt;
	
 gtk_text_buffer_get_selection_bounds (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), & start, & end);
	
	txt=gtk_text_buffer_get_text(GTK_TEXT_BUFFER(cur_text_doc->text_buffer),&start,&end,FALSE);

	if(! strcmp("", txt) == 0)
	{
	gtk_text_buffer_create_tag (
   GTK_TEXT_BUFFER(cur_text_doc->text_buffer), txt, 
   "foreground", "black",
   "background", "yellow",
   "weight", PANGO_WEIGHT_BOLD,
   NULL);

	//*****************
	GtkTextIter itstart, itend;
	gint line=gtk_text_iter_get_line(&start);

gtk_text_buffer_get_iter_at_line (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &itstart, line);
gtk_text_buffer_get_iter_at_line (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &itend, line + 1);
	//*****************
	gtk_text_buffer_apply_tag_by_name (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), "search", &start, &end);  
	gtk_text_buffer_apply_tag_by_name (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), txt, &itstart, &start);  
	gtk_text_buffer_apply_tag_by_name (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), txt, &end, &itend); 

	GdkPixbuf *pixbuf_mark;

	pixbuf_mark = gdk_pixbuf_new_from_file ("/usr/local/share/griffon/pixmaps/griffon_note.png", NULL);

GtkSourceMarkAttributes *attribu=gtk_source_mark_attributes_new();
gtk_source_mark_attributes_set_pixbuf(attribu,pixbuf_mark );
gtk_source_view_set_mark_attributes(GTK_SOURCE_VIEW(cur_text_doc->text_view),"icon_note",attribu,1);
gtk_source_buffer_create_source_mark(GTK_SOURCE_BUFFER(cur_text_doc->text_buffer),txt,"icon_note",&itstart);

//g_signal_connect(attribu, "query-tooltip-text",G_CALLBACK(tooltip_bookmark),"BookMark");

	add_to_list_book("",txt);
	statusbar_msg (_("BookMark [OK]"));
	//gtk_notebook_set_current_page(notebook2,5);
	}else{log_to_memo (_("Vous devez séléctionner un mot ou du texte pour utiliser un BookMark"), NULL, LM_ERROR);statusbar_msg (_("BookMark [ERROR]"));}
}

//******************************* Montage SFTP
void mount_sftp (void)
{
	char mot[150];
	char mot2[150];
	gchar *tampon_utilisateur;
	gchar *tampon_chemin;

	const char *home_dir = g_getenv ("HOME");

	FILE *fichier = NULL;
	fichier = fopen("/usr/bin/sshfs",  "r");

		if (fichier == NULL)
		{
		icon_stop_logmemo();
		//gtk_notebook_set_current_page(notebook_down,0);
		log_to_memo (_("Vous devez installer le package sshfs et avoir une clef ssh valide pour utiliser la fonction montage SFTP"), NULL, LM_ERROR);
		statusbar_msg (_("Mount [ERROR]"));
		}
		else
		{
		tampon_sftp = gtk_editable_get_chars(GTK_EDITABLE(entry_sftp),0, -1);
		tampon_utilisateur = gtk_editable_get_chars(GTK_EDITABLE(entry_utilisateur),0, -1);
		tampon_chemin = gtk_editable_get_chars(GTK_EDITABLE(entry_chemin),0, -1);

		int systemRet=0;
		if(sshadd==NULL){systemRet =system ("ssh-add");sshadd="ok";}
		if(systemRet == -1){return;}

		strcpy(mot2,"mkdir -p ");
		strcat(mot2,home_dir);
		strcat(mot2,"/MOUNT/");

		strcat(mot2,tampon_sftp);
	 systemRet =system (mot2);
		if(systemRet == -1){return;}

		strcpy(mot,"sshfs ");
		strcat(mot,tampon_utilisateur);
		strcat(mot,"@");				
		strcat(mot,tampon_sftp);
		strcat(mot,":");
		strcat(mot,tampon_chemin);	

		strcat(liste_mount,"fusermount -u ");
		strcat(liste_mount,home_dir);
		strcat(liste_mount,"/MOUNT/");
		strcat(liste_mount,tampon_sftp);
		strcat(liste_mount," ; ");
		
		new_terminal_ssh (tampon_sftp,tampon_utilisateur);

		//***** Sauvegarde pour historique	
		save_string_to_file_add(confile.tea_sftp,tampon_sftp);	
		save_string_to_file_add(confile.tea_sftp," ");
		save_string_to_file_add(confile.tea_sftp,tampon_utilisateur);
		save_string_to_file_add(confile.tea_sftp," ");
		save_string_to_file_add(confile.tea_sftp,tampon_chemin);
		save_string_to_file_add(confile.tea_sftp," \n");
									
		strcat(mot," ");
		strcat(mot,home_dir);
		strcat(mot,"/MOUNT/");

		strcat(mot,tampon_sftp);	
	  	systemRet =system (mot);
			if(systemRet == -1){return;}

	char total_path[300];total_path[0]='\0';
		strcat(total_path,home_dir);
		strcat(total_path,"/MOUNT/");
		strcat(total_path,tampon_sftp);		
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooserwidget2) ,total_path);

		 icon_log_logmemo();
		log_to_memo (_("%s montage SFTP dans le répertoire Griffon_MONTAGE_SFTP"), tampon_sftp, LM_NORMAL);
		statusbar_msg (_("Mount [OK]"));
		griffon_notify(_("Mount SFTP"));
		icon_affiche_net ();
		sftp_reload();
		fclose(fichier);
		//gtk_notebook_set_current_page(notebook3,0);
		//gtk_notebook_set_current_page(notebook2,0);
		}
			
}

//******************************* Montage FTP
void mount_ftp (void)
{

	char mot[150];
	char mot2[150];
	gchar *tampon_utilisateur_ftp;
	gchar *tampon_passwd_ftp;

	const char *home_dir = g_getenv ("HOME");

	FILE *fichier = NULL;
	fichier = fopen("/usr/bin/curlftpfs",  "r");
	
		if (fichier == NULL)
		{
		icon_stop_logmemo();
		log_to_memo (_("Vous devez installer le package curlftpfs pour utiliser la fonction montage FTP"), NULL, LM_ERROR);
		statusbar_msg (_("Mount [ERROR]"));
		}
		else
		{
		tampon_sftp = gtk_editable_get_chars(GTK_EDITABLE(entry_ftp),0, -1);
		tampon_utilisateur_ftp = gtk_editable_get_chars(GTK_EDITABLE(entry_utilisateur_ftp),0, -1);
		tampon_passwd_ftp = gtk_editable_get_chars(GTK_EDITABLE(entry_passwd_ftp),0, -1);

		strcpy(mot2,"mkdir -p ");
		strcat(mot2,home_dir);
		strcat(mot2,"/MOUNT/");
		strcat(mot2,tampon_sftp);
		int systemRet =system (mot2);
		if(systemRet == -1){return;}

		strcpy(mot,"curlftpfs ");
		strcat(mot,tampon_utilisateur_ftp);
		strcat(mot,":");				
		strcat(mot,tampon_passwd_ftp);
		strcat(mot,"@");
		strcat(mot,tampon_sftp);		

		strcat(mot," ");
		strcat(mot,home_dir);
		strcat(mot,"/MOUNT/");
		strcat(mot,tampon_sftp);
		
		strcat(liste_mount,"fusermount -u ");
		strcat(liste_mount,home_dir);
		strcat(liste_mount,"/MOUNT/");
		strcat(liste_mount,tampon_sftp);
		strcat(liste_mount," ; ");

		//***** Sauvegarde pour historique	
		save_string_to_file_add(confile.tea_ftp,tampon_sftp);	
		save_string_to_file_add(confile.tea_ftp," ");
		save_string_to_file_add(confile.tea_ftp,tampon_utilisateur_ftp);
		save_string_to_file_add(confile.tea_ftp," ");
		save_string_to_file_add(confile.tea_ftp,tampon_passwd_ftp);
		save_string_to_file_add(confile.tea_ftp," \n");		
		
	  systemRet =system (mot);
		if(systemRet == -1){return;}

	char total_path[300];total_path[0]='\0';
		strcat(total_path,home_dir);
		strcat(total_path,"/MOUNT/");
		strcat(total_path,tampon_sftp);		
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooserwidget2) ,total_path);

		 icon_log_logmemo();
		log_to_memo (_("%s montage FTP dans le répertoire Griffon_MONTAGE_SFTP"), tampon_sftp, LM_NORMAL);
		statusbar_msg (_("Mount [OK]"));
		griffon_notify(_("Mount FTP"));
		icon_affiche_net ();
		ftp_reload();
		fclose(fichier);

		//gtk_notebook_set_current_page(notebook3,0);
		//gtk_notebook_set_current_page(notebook2,0);

	}		

}

//******************************* Umount pour montage SFTP
void umount_sftp (void)
{
	if(tampon_sftp!=NULL)
	{
	int systemRet =system (liste_mount);
	if(systemRet == -1){return;}

	liste_mount[0]='\0';

	icon_log_logmemo();
	log_to_memo (_("%s démontage FTP/SFTP"), tampon_sftp, LM_NORMAL);
	icon_affiche_net ();
	tampon_sftp=NULL;
	statusbar_msg (_("Umount [OK]"));
	griffon_notify(_("Umount SFTP/FTP"));
	}
	else
	{
	icon_stop_logmemo();

	log_to_memo (_("Aucun serveur monté en FTP/SFTP"), NULL, LM_ERROR);
	statusbar_msg (_("Umount [Error]"));
	}

	gtk_tree_selection_unselect_all(GTK_TREE_SELECTION(selection_sftp)); 
	gtk_tree_selection_unselect_all(GTK_TREE_SELECTION(selection_ftp)); 
}

//******************************* fenetre de configuration SFTP
GtkWidget* w_sftp_mount (void)
{
  GtkWidget *window1,*frame1,*vbox1,*hbox2,*label2,*hbox1,*button1,*button2,*label1,*hbox3,*label3,*hbox4,*label4;  
  
	icon_affiche_stop();
	
  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window1), _((_("Monter un serveur en SFTP"))));
  gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);
  gtk_window_set_resizable (GTK_WINDOW (window1), FALSE);
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

  label2 = gtk_label_new (_("Nom du serveur ou ip : "));
  gtk_widget_show (GTK_WIDGET(label2));
  gtk_box_pack_start (GTK_BOX (hbox2), label2, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);

  entry_sftp = gtk_entry_new ();
  gtk_widget_show (GTK_WIDGET(entry_sftp));
  gtk_box_pack_start (GTK_BOX (hbox2), entry_sftp, FALSE, FALSE, 0);
  gtk_widget_grab_focus(GTK_WIDGET(entry_sftp));

  hbox3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox3));
  gtk_box_pack_start (GTK_BOX (vbox1), hbox3, TRUE, TRUE, 0);

  label3 = gtk_label_new (_("Utilisateur : "));
  gtk_widget_show (GTK_WIDGET(label3));
  gtk_box_pack_start (GTK_BOX (hbox3), label3, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);

  entry_utilisateur = gtk_entry_new ();
  gtk_widget_show (GTK_WIDGET(entry_utilisateur));
  gtk_box_pack_start (GTK_BOX (hbox3), entry_utilisateur, FALSE, FALSE, 0);
	gtk_entry_set_text (GTK_ENTRY (entry_utilisateur), _("root"));

  hbox4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox4));
  gtk_box_pack_start (GTK_BOX (vbox1), hbox4, TRUE, TRUE, 0);

  label4 = gtk_label_new (_("Chemin sur le serveur : "));
  gtk_widget_show (GTK_WIDGET(label4));
  gtk_box_pack_start (GTK_BOX (hbox4), label4, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_LEFT);

  entry_chemin = gtk_entry_new ();
  gtk_widget_show (GTK_WIDGET(entry_chemin));
  gtk_box_pack_start (GTK_BOX (hbox4), entry_chemin, FALSE, FALSE, 0);
	gtk_entry_set_text (GTK_ENTRY (entry_chemin), _("/"));

  hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox1));
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, FALSE, FALSE, 0);

  button1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (GTK_WIDGET(button1));
  gtk_box_pack_start (GTK_BOX (hbox1), button1, TRUE, TRUE, 0);
  gtk_button_set_relief (GTK_BUTTON (button1), GTK_RELIEF_NONE);


  button2 = gtk_button_new_from_stock ("gtk-apply");
  gtk_widget_show (GTK_WIDGET(button2));
  gtk_box_pack_start (GTK_BOX (hbox1), button2, TRUE, TRUE, 0);
  gtk_button_set_relief (GTK_BUTTON (button2), GTK_RELIEF_NONE);

  label1 = gtk_label_new (_("Montage SFTP"));
  gtk_widget_show (GTK_WIDGET(label1));
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label1);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

g_signal_connect_swapped ((gpointer) button1, "clicked",
                            G_CALLBACK (icon_affiche_ok),
                            NULL);	
    g_signal_connect_swapped ((gpointer) button1, "clicked",
                            G_CALLBACK (gtk_widget_destroy),
                            window1);

g_signal_connect_swapped ((gpointer) button2, "clicked",
                            G_CALLBACK (icon_affiche_ok),
                            NULL);	

    g_signal_connect_swapped ((gpointer) button2, "clicked",
                            G_CALLBACK (mount_sftp),
                            NULL);	

    g_signal_connect_swapped ((gpointer) button2, "clicked",
                            G_CALLBACK (gtk_widget_destroy),
                            window1);

    g_signal_connect_swapped ((gpointer) button1, "clicked",
                            G_CALLBACK (gtk_widget_destroy),
                            window1);

  return window1;
}

//******************************* fenetre de configuration FTP
GtkWidget* w_ftp_mount (void)
{
  GtkWidget *window1,*frame1,*vbox1,*hbox2,*label2,*hbox1,*button1,*button2,*label1,*hbox3,*label3,*hbox4,*label4;  
  
	icon_affiche_stop();
	
  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window1), _((_("Monter un serveur en FTP"))));
  gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);
  gtk_window_set_resizable (GTK_WINDOW (window1), FALSE);
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

  label2 = gtk_label_new (_("Nom du serveur ou ip : "));
  gtk_widget_show (GTK_WIDGET(label2));
  gtk_box_pack_start (GTK_BOX (hbox2), label2, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);

  entry_ftp = gtk_entry_new ();
  gtk_widget_show (GTK_WIDGET(entry_ftp));
  gtk_box_pack_start (GTK_BOX (hbox2), entry_ftp, FALSE, FALSE, 0);
  gtk_widget_grab_focus(GTK_WIDGET(entry_ftp));

  hbox3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox3));
  gtk_box_pack_start (GTK_BOX (vbox1), hbox3, TRUE, TRUE, 0);

  label3 = gtk_label_new (_("Utilisateur : "));
  gtk_widget_show (GTK_WIDGET(label3));
  gtk_box_pack_start (GTK_BOX (hbox3), label3, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);

  entry_utilisateur_ftp = gtk_entry_new ();
  gtk_widget_show (GTK_WIDGET(entry_utilisateur_ftp));
  gtk_box_pack_start (GTK_BOX (hbox3), entry_utilisateur_ftp, FALSE, FALSE, 0);

  hbox4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox4));
  gtk_box_pack_start (GTK_BOX (vbox1), hbox4, TRUE, TRUE, 0);

  label4 = gtk_label_new (_("Password : "));
  gtk_widget_show (GTK_WIDGET(label4));
  gtk_box_pack_start (GTK_BOX (hbox4), label4, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_LEFT);

  entry_passwd_ftp = gtk_entry_new ();
  gtk_widget_show (GTK_WIDGET(entry_passwd_ftp));
  gtk_box_pack_start (GTK_BOX (hbox4), entry_passwd_ftp, FALSE, FALSE, 0);

  hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (GTK_WIDGET(hbox1));
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, FALSE, FALSE, 0);

  button1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (GTK_WIDGET(button1));
  gtk_box_pack_start (GTK_BOX (hbox1), button1, TRUE, TRUE, 0);
  gtk_button_set_relief (GTK_BUTTON (button1), GTK_RELIEF_NONE);


  button2 = gtk_button_new_from_stock ("gtk-apply");
  gtk_widget_show (GTK_WIDGET(button2));
  gtk_box_pack_start (GTK_BOX (hbox1), button2, TRUE, TRUE, 0);
  gtk_button_set_relief (GTK_BUTTON (button2), GTK_RELIEF_NONE);

  label1 = gtk_label_new (_("Montage FTP"));
  gtk_widget_show (GTK_WIDGET(label1));
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label1);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

g_signal_connect_swapped ((gpointer) button1, "clicked",
                            G_CALLBACK (icon_affiche_ok),
                            NULL);	
    g_signal_connect_swapped ((gpointer) button1, "clicked",
                            G_CALLBACK (gtk_widget_destroy),
                            window1);

g_signal_connect_swapped ((gpointer) button2, "clicked",
                            G_CALLBACK (icon_affiche_ok),
                            NULL);	

    g_signal_connect_swapped ((gpointer) button2, "clicked",
                            G_CALLBACK (mount_ftp),
                            NULL);	

    g_signal_connect_swapped ((gpointer) button2, "clicked",
                            G_CALLBACK (gtk_widget_destroy),
                            window1);

    g_signal_connect_swapped ((gpointer) button1, "clicked",
                            G_CALLBACK (gtk_widget_destroy),
                            window1);

  return window1;
}

//******************************* Executer un script
void on_execut_script (void)
{
	if (! get_page_text()) return;

	if(cur_text_doc->file_name==NULL)
	{
	icon_stop_logmemo();

	 log_to_memo (_("Les script n'est pas sauvegardé ou n'est pas séléctionné"), NULL, LM_ERROR);
	 statusbar_msg (_("Script [Error]"));
	}
	else
	{
	icon_log_logmemo();
  log_to_memo (_("Execution du script %s [OK]"), cur_text_doc->file_name, LM_NORMAL);
	statusbar_msg (_("Run script [OK]"));
	icon_affiche_bug();
 
  gchar *standard_output = NULL;
  gchar *standard_error = NULL;
                                                   
  GError *err = NULL;

  gchar *cmd = g_strconcat (cur_text_doc->file_name, NULL); 

  gsize bytes_read;
  gsize bytes_written;
  gchar *x = NULL;

  if (! g_spawn_command_line_sync  (cmd, &standard_output, &standard_error, NULL, &err))
    {

     g_error_free (err);
     return;
    }
  else
     {
      x = g_locale_to_utf8 (standard_output, -1, &bytes_read, &bytes_written, NULL);
      log_to_memo (x, NULL, LM_NORMAL);
      g_free (x);

      x = g_locale_to_utf8 (standard_error, -1, &bytes_read, &bytes_written, NULL);
      log_to_memo (x, NULL, LM_NORMAL);

      g_free (x);
  
      do_errors_hl (GTK_TEXT_VIEW(tv_logmemo));
      }   

  g_free (cmd);
  g_free (standard_output);
  g_free (standard_error);
	}
}

//******************************* mise en valeur du texte après le cursor
void on_format_line_bg ()
{
	if (! get_page_text()) return;

	GtkTextIter end,start;
	
	if(tag_on==0)
	{	
	gtk_text_buffer_create_tag (
   GTK_TEXT_BUFFER(cur_text_doc->text_buffer), "bg_ligne", 
   "background", "#EDEFFF",
   NULL);
	tag_on=1;
	}

	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(cur_text_doc->text_buffer), & end);
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(cur_text_doc->text_buffer), & start);

	gtk_text_buffer_remove_tag_by_name (GTK_TEXT_BUFFER(cur_text_doc->text_buffer),
                                             "bg_ligne",
                                             &start,
                                             &end);

  GtkTextIter iter;

  gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(cur_text_doc->text_buffer),
      &iter, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(cur_text_doc->text_buffer)));

	gtk_text_buffer_apply_tag_by_name (
	GTK_TEXT_BUFFER(cur_text_doc->text_buffer), "bg_ligne", &iter, & end);  
     
}

//******************************* Ouverture d'image avec gimp depuis le mini web
void open_gimp (void)
{

	FILE *fichier = NULL;
	fichier = fopen("/usr/bin/gimp",  "r");
	
		if (fichier == NULL)
		{
		icon_stop_logmemo();
		log_to_memo (_("Vous devez installer le package gimp pour utiliser la fonction edition d'image"), NULL, LM_ERROR);
		statusbar_msg (_("Vous devez installer le package gimp pour utiliser la fonction edition d'image"));
		}
		else{
	char mot[150];
	gchar *tampon_web;
	tampon_web = gtk_editable_get_chars(GTK_EDITABLE(entry_web),0, -1);
	strcpy(mot,"gimp ");
	strcat(mot,tampon_web);
	strcat(mot," &");
	fclose(fichier);
	int systemRet =system (mot);
		if(systemRet == -1){return;}
	}
}

//******************************* Ouvrir les fichiers include  
void open_include             (void)
{	
	if (! get_page_text()) return;

  FILE *fich;
  char carac;

  char motrch[100],motrch2[100], mot[2000],path[100];
  int nbapparition=0,nbcarac=0,nbmot=0,counter=0;
  int nbligne=1;	
	gchar *t;
	t = g_path_get_dirname (cur_text_doc->file_name);
	strcat(t,"/");

  nbapparition=0,nbcarac=0,nbmot=0,nbligne=1;
  mot[0]='\0';
  motrch[0]='\0';
  motrch2[0]='\0';
  strcpy(motrch,"include");
  strcpy(motrch2,"require");
  fich=fopen(cur_text_doc->file_name,"r");
 while ((carac =fgetc(fich)) != EOF)
 {
    
  if(counter==1)
  {
  strncat(mot,&carac,1);
  
    if (carac =='\n' || carac =='\r')
    {

	 gchar **a = g_strsplit (mot, "\"", -1);
	 
			if(a[1]!='\0')
			{
				strcpy(path,t);
				strcat(path,a[1]);
		     if(g_file_test (path, G_FILE_TEST_EXISTS)){doc_open_file (path);}
			}

	    path[0]='\0';
	    mot[0]='\0';
	    counter=0;		  
    }   
  }  
    
 if (counter==0)
 {    
    if (carac =='\n' || carac =='\r')
    {   
    mot[0]='\0';
    nbligne++;
    }
      nbcarac++;
      if (mot[0] != '\0' && isalnum(carac) != 0){strncat(mot,&carac,1);}

      if (mot[0] != '\0' && isalnum(carac) == 0)
		{
		  if (strncmp(motrch,mot,strlen(motrch))==0 || strncmp(motrch2,mot,strlen(motrch2))==0)
	    {
	      nbapparition++;
	      if(nbapparition==1){nbcarac--;}	        
			counter=1;      
	    }	
		}

      if (mot[0] == '\0' && isalnum(carac) != 0)
		{
		  strncat(mot,&carac,1);     
		  nbmot++;                   
		}
  }    
 }
  fclose(fich);
}

//******************************* Ajout de mot de base pour l'autocomp par rapport au type de fichier  
void add_word_autocomp_file(void)
{

/*	if (! get_page_text()) return;
	gchar* file_autocomp=NULL;
 
   char *extension;

   if(strrchr(cur_text_doc->file_name,'.'))
	{
	extension = strrchr(cur_text_doc->file_name,'.');
	if (strcmp(".pl", extension) == 0){file_autocomp="/usr/local/share/griffon/doc/perl";}
	if (strcmp(".sh", extension) == 0){file_autocomp="/usr/local/share/griffon/doc/bash";}
	if (strcmp(".c", extension) == 0 || strcmp(".h", extension) == 0){file_autocomp="/usr/local/share/griffon/doc/c";}
	if (strcmp(".htm", extension) == 0 || strcmp(".html", extension) == 0){file_autocomp="/usr/local/share/griffon/doc/html";}
	if (strcmp(".php", extension) == 0){file_autocomp="/usr/local/share/griffon/doc/php";}
	}

	if(file_autocomp!=NULL)
	{
	  FILE *fich;
	  char carac;
	  char mot[800];
		mot[0]='\0';

  fich=fopen(file_autocomp,"r");


  if (fich==NULL){return ;}
  else
  {
	  while ((carac =fgetc(fich)) != EOF)
	  {    
		  if (carac =='\n')
		  {			

		 	gl_autocomp = glist_add_not_dup (gl_autocomp, mot, 3);
			mot[0]='\0';

	          if (gl_autocomp)
             {
              g_completion_clear_items (at_editor);
              g_completion_add_items (at_editor, gl_autocomp);
             }	

		  }
		  else
		  {
		   strncat(mot,&carac,1);
		   }
		}   
	}

	fclose(fich);

	}*/
}

//******************************* Keyrelase for search
void keyrelase_search(void)
{
	 if (! get_page_text()) return;
	gchar const *text=gtk_entry_get_text (GTK_ENTRY(ent_search));

	GtkTextIter start_find, end_find;
	GtkTextIter start_match, end_match;

	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &start_find);
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &end_find);
	gint row2 = gtk_text_iter_get_line(&end_find);

	if(row2<1000)
	{
	gtk_text_buffer_remove_tag_by_name(GTK_TEXT_BUFFER(cur_text_doc->text_buffer), "search", &start_find, &end_find);

     if (! text){return;}else{
	while ( gtk_text_iter_forward_search(&start_find, text, GTK_TEXT_SEARCH_TEXT_ONLY | GTK_TEXT_SEARCH_VISIBLE_ONLY, &start_match, &end_match, NULL) )
					{
          gtk_text_buffer_apply_tag_by_name(GTK_TEXT_BUFFER(cur_text_doc->text_buffer), "search", 
              &start_match, &end_match);
          int offset = gtk_text_iter_get_offset(&end_match);
          gtk_text_buffer_get_iter_at_offset(GTK_TEXT_BUFFER(cur_text_doc->text_buffer), 
              &start_find, offset);
					}
			}
	}
}



