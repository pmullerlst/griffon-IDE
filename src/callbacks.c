
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
#include <gtksourceview/gtksourcemarkattributes.h>
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

//*********************** DECLARATION
GtkWidget *checkbutton1,*checkbutton2,*checkbutton3,*checkbutton4,*checkbutton5,*entry1,*entry2,*entry_scp1,*entry_scp2,*entry_scp3,*entry_scp4,*entry_sftp,*entry_utilisateur,*entry_chemin,*entry_ftp,*entry_utilisateur_ftp;
  GtkWidget *entry_passwd_ftp,*entry_ln1,*entry_ln2,*entry_fonction,*entry_fonction2,*entry_compress,*checkbutton1_grep,*checkbutton2_grep,*checkbutton3_grep,*checkbutton4_grep;
  GtkWidget *entry_grep1,*entry_grep2,*entry_grep3,*checkbutton_sed1,*entry_sed4,*entry_sed5,*checkbutton_sed2,*entry_sed6,*entry_sed7,*checkbutton_sed3,*checkbutton_sed4,*checkbutton_sed5;
  GtkWidget *checkbutton_sed6,*checkbutton_sed7,*entry_sed1,*entry_sed2,*entry_sed3,*spinbutton1,*checkbutton_awk1,*entry_awk3,*entry_awk1,*entry_awk4,*entry_awk5,*checkbutton_rouge;
  GtkWidget *checkbutton_vert,*checkbutton_jaune,*entry_echo_couleur;
	int tag_on=0;

//*********************** SET CMD LIGNE EDITOR
void set_fam_text (gchar *def)
{
	icon_affiche_ok();
	if (strlen (gtk_entry_get_text (GTK_ENTRY(ent_search))) == 0)
	{
		gtk_entry_set_text (GTK_ENTRY(ent_search), def); 
		icon_man_logmemo(); 
		log_to_memo (_("Documentation is available in the menu \"Help-> Read the manual/Griffon IDE documentation\""), NULL, LM_ERROR);
	} 
}

//*********************** FILE OPEN AT CURSOR
void on_mni_file_open_at_cursor ()
{
	icon_affiche_ok();
	if (! get_page_text()) return;

	gchar *w = get_c_url (cur_text_doc);
	handle_file (w, 0);
	g_free (w);
}

//*********************** INIT DE GRIFFON
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

	return TRUE;
}  

//*********************** NEW FICHIER
void file_new (void)
{
	cur_text_doc = doc_clear_new ();

	icon_affiche_ok(); 
	gtk_window_set_title (GTK_WINDOW (tea_main_window), cur_text_doc->file_name);
	gtk_widget_grab_focus (GTK_WIDGET(cur_text_doc->text_view));
	tabs_reload ();
	no_onglet_open();
	statusbar_msg (_("New File [OK]"));
}

//*********************** NEW FICHIER MENU CLICK
void on_mni_new_file_activate ()
{
	icon_affiche_ok();
	file_new ();
	no_onglet_open();
}

//*********************** OPEN FILE ENCODAGE
void on_mni_new_file_open_select_enc (GtkMenuItem *menuitem)
{
	cur_settings.selected_enc = ch_str (cur_settings.selected_enc, gtk_widget_get_name (GTK_WIDGET(menuitem)));
	icon_affiche_ok();
}

//*********************** OPEN FICHIER
void file_open (void)
{
	gchar *t;

	icon_affiche_ok();
	GtkWidget *file_dialog = gtk_file_chooser_dialog_new (_("File open"),GTK_WINDOW(tea_main_window),
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

//*********************** FILE OPEN MENU CLICK
void on_mni_file_open_activate (){icon_affiche_ok();file_open ();}

//*********************** FILE SAVE
void file_save (void)
{
	icon_affiche_ok();
	if (! get_page_text()) return;
	if (! cur_text_doc->b_saved)
	{
		apply_hl (cur_text_doc);
		on_mni_file_save_as_activate (NULL, NULL);
	}
	else
	{
		if (text_doc_save (cur_text_doc, cur_text_doc->file_name))
		icon_save_logmemo();		      	
		log_to_memo (_("%s Save File [OK]"), cur_text_doc->file_name, LM_NORMAL);
		griffon_notify(_("Save [OK]"));
		statusbar_msg (_("Save [OK]"));
		icon_affiche_save (); 
		on_execut_diff ();

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

//*********************** FILE SAVE MENU CLICK
void on_mni_file_save_activate (){icon_affiche_ok();file_save ();}

//*********************** FILE SAVE AS
void file_save_as (void)
{
	icon_affiche_ok();
	if (get_page_text ())
	show_save_as_dlg (0);
}

void on_mni_file_save_as_activate (){icon_affiche_ok();file_save_as ();}
void on_mni_out_activate (){  ui_done();  gtk_main_quit ();}

//*********************** MISE EN MAJUSCULE DE LA SELECTION
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

//*********************** MISE EN MINUSCULE DE LA SELECTION
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

//*********************** REVERSE SELECTION
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

//*********************** SAVE FILE VERSION
void on_mni_file_save_version ()
{
	icon_affiche_ok();
	if (! get_page_text()) return;

	gchar *s = get_file_ver_name (cur_text_doc->file_name);
	if (! s)
	{
		icon_stop_logmemo();
		log_to_memo(_("Save the file before ..."), NULL, LM_ADVICE);
		return;
	}

	text_doc_save (cur_text_doc, s);
	icon_save_logmemo();
	icon_affiche_save ();
	log_to_memo (_("%s Save OK..."), s, LM_NORMAL);
	statusbar_msg (_("Save [OK]"));
	g_free (s);
}

//*********************** BALISE HTML BOLD
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

//*********************** BALISE HTML LINK
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

//*********************** BALISE HTML ITALIC
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

//*********************** BALISE HTML UNDERLINE
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

//*********************** BALISE HTML PARAGRAPHE
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

//*********************** BALISE HTML HEADER
void on_mni_Markup_header_activate (GtkMenuItem *menuitem)
{
	if (! get_page_text()) return;

	gchar *t = NULL;
	gchar *buf = doc_get_sel (cur_text_doc);

	if (! buf)
	{
		t = g_strconcat ("<", gtk_widget_get_name (GTK_WIDGET(menuitem)), ">","</", gtk_widget_get_name (GTK_WIDGET(menuitem)), ">",NULL);
		doc_insert_at_cursor (cur_text_doc, t);
		doc_move_cursor_backw_middle_tags (cur_text_doc);
	}
	else
	{
		t = g_strconcat ("<", gtk_widget_get_name (GTK_WIDGET(menuitem)), ">",buf,"</", gtk_widget_get_name (GTK_WIDGET(menuitem)), ">",NULL);
		doc_rep_sel (cur_text_doc, t);
	}

	g_free (buf);
	g_free (t);
}

static gint last_page = -1;

//*********************** SWITCH PAGE EDITOR
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

//*********************** CLOSE CURRENT FILE
void on_mni_file_close_current ()
{
	icon_affiche_ok();
	page_del_by_index (gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook1)));
	no_onglet_open();
}

//*********************** VIEW WRAP
void on_mni_view_wrap ()
{
	icon_affiche_ok();
	if (! get_page_text()) return;

	if (gtk_text_view_get_wrap_mode (GTK_TEXT_VIEW(cur_text_doc->text_view)) != GTK_WRAP_WORD)
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW(cur_text_doc->text_view), GTK_WRAP_WORD);
	else
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW(cur_text_doc->text_view), GTK_WRAP_NONE);
}

//*********************** BALISE HTML BR
void on_mni_Markup_br ()
{
	if (! get_page_text()) return;
	if (! confile.xhtml_mode)
	doc_insert_at_cursor (cur_text_doc, "<br>");
	else
	doc_insert_at_cursor (cur_text_doc, "<br />"); 
}

//*********************** FONCTION NUM
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

//*********************** SHOW LINE NUMMBERS
void on_mni_view_show_line_numbers ()
{
	icon_affiche_ok();
	if (! get_page_text()) return;

	if (! cur_text_doc->linenums)
		document_set_line_numbers (cur_text_doc, TRUE);
	else
		document_set_line_numbers (cur_text_doc, FALSE);
}

//*********************** TEMPLATE HTML DEFAULT
void on_mni_html_default_template ()
{
	if (! get_page_text()) return;

	gchar *t = g_strconcat ("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/1999/REC-html401-19991224/loose.dtd\">\n", 
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

//*********************** SAVE SESSION
void on_mni_file_save_session ()
{
	icon_affiche_ok();
	GtkWidget *dialog = gtk_file_chooser_dialog_new (_("Save session as"),
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

//*********************** OPEN SESSION FILE
void on_mni_session_file_open_activate ()
{
	GtkWidget *dialog = gtk_file_chooser_dialog_new (_("Open file session"),
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

//*********************** ANTISPAM
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

//*********************** CONVERT HEX TO DEC
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

//*********************** CONFIG OPEN
void on_mni_config_open ()
{
	cur_settings.selected_enc = ch_str (cur_settings.selected_enc, "UTF-8");
	open_file_std (confile.tea_rc);
}

//*********************** OPEN IN BROWSER
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

//*********************** STATS FILE
void on_mni_utils_stats ()
{
	icon_affiche_ok();
	if (! get_page_text()) return;
	make_stats (cur_text_doc);
}

//*********************** ADD ENCODINGS
void on_mni_utils_add_encodings_activate ()
{
	cur_settings.selected_enc = ch_str (cur_settings.selected_enc, "UTF-8");
	open_file_std (confile.iconv_file);
}

//*********************** CONVERT BIN TO DEC
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

//*********************** BALISE HTML ESPACE
void on_mni_Markup_nbsp ()
{
	if (! get_page_text()) return;
	doc_insert_at_cursor (cur_text_doc, "&nbsp;");
}

//*********************** UNDO
void on_mni_undo ()
{
	icon_affiche_ok();
	if (! get_page_text()) return;
	gtk_source_buffer_undo(GTK_SOURCE_BUFFER(cur_text_doc->text_buffer));
}

//*********************** REDO
void on_mni_redo ()
{
	icon_affiche_ok();
	if (! get_page_text()) return;
	gtk_source_buffer_redo(GTK_SOURCE_BUFFER(cur_text_doc->text_buffer));
}

//*********************** BALISE HTML IMAGE
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

//*********************** HTML LINK
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

//*********************** CMD PRESS EVENT
gboolean on_ent_search_key_press_event (GdkEventKey *event)
{
	icon_affiche_ok();
	if (event->keyval != GDK_KEY_Return)
	return FALSE;

	if (get_page_text())
	{
		doc_search_f (cur_text_doc, gtk_entry_get_text (GTK_ENTRY(ent_search)));gtk_widget_grab_focus (GTK_WIDGET(cur_text_doc->text_view));

			FILE *fich;
			char carac;
			char mot[100];
			mot[0]='\0';
			int check=0;
	
			fich=fopen(confile.tea_cmd_history,"r");
			while ((carac =fgetc(fich)) != EOF)
			{
				if (carac =='\n')
				{
					if (strncmp(mot,gtk_entry_get_text (GTK_ENTRY(ent_search)),strlen(mot))==0 ){check=1;}
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
		GtkTreeIter iter_entry;
		gtk_list_store_append(model_entry, &iter_entry);
		gtk_list_store_set(model_entry, &iter_entry, CONTACT_NAME, gtk_entry_get_text (GTK_ENTRY(ent_search)),  -1);
		save_string_to_file_add(confile.tea_cmd_history,(gchar *)gtk_entry_get_text (GTK_ENTRY(ent_search)));
		save_string_to_file_add(confile.tea_cmd_history,"\n");
		}

	}

	return FALSE;
}

//*********************** STRINGS REMOVE BLANKS
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

//*********************** STRINGS ENCLOSE
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

//*********************** STRINGS REV
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

//*********************** SEARCH REP ALL
void on_mni_search_repall ()
{
	if (! get_page_text()) return;

	set_fam_text ((_("Text to find the text to replace")));
	gchar const *s = gtk_entry_get_text (GTK_ENTRY(ent_search));

			FILE *fich;
			char carac;
			char mot[100];
			mot[0]='\0';
			int check=0;
	
			fich=fopen(confile.tea_cmd_history,"r");
			while ((carac =fgetc(fich)) != EOF)
			{
				if (carac =='\n')
				{
					if (strncmp(mot,gtk_entry_get_text (GTK_ENTRY(ent_search)),strlen(mot))==0 ){check=1;}
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
		GtkTreeIter iter_entry;
		gtk_list_store_append(model_entry, &iter_entry);
		gtk_list_store_set(model_entry, &iter_entry, CONTACT_NAME, gtk_entry_get_text (GTK_ENTRY(ent_search)),  -1);
		save_string_to_file_add(confile.tea_cmd_history,(gchar *)gtk_entry_get_text (GTK_ENTRY(ent_search)));
		save_string_to_file_add(confile.tea_cmd_history,"\n");
		}

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

//*********************** RECENT ACTIVATE
void on_mni_recent_activate (GtkMenuItem *menuitem)
{
	icon_affiche_ok();
	execute_recent_item (gtk_widget_get_name (GTK_WIDGET(menuitem)));
}

//*********************** SHOW SAVE AS DIALOG
void show_save_as_dlg (int mode)
{
	GtkWidget *file_dialog = gtk_file_chooser_dialog_new (_("Save as ..."),
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
			gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (file_dialog),confile.def_save_dir);
			else
				if (g_file_test (cur_text_doc->file_name, G_FILE_TEST_EXISTS))
				gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (file_dialog), cur_text_doc->file_name);
		}
	}

	if (mode == 2)
	gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (file_dialog),confile.snippets_dir);

	if (gtk_dialog_run (GTK_DIALOG (file_dialog)) == GTK_RESPONSE_ACCEPT)
	{
		gchar *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (file_dialog));

		if (confile.prompt_on_file_exists && g_file_test (filename, G_FILE_TEST_EXISTS))
		if (! dlg_question (tea_main_window, (_("%s there want to replace it?")), filename))
		return;

		cur_text_doc->file_name = ch_str (cur_text_doc->file_name, filename);
		cur_text_doc->encoding = ch_str (cur_text_doc->encoding, cur_settings.selected_enc);  

		if (! text_doc_save (cur_text_doc, cur_text_doc->file_name))
		{
			icon_stop_logmemo();
			icon_affiche_nosave ();
			log_to_memo (_("Save error %s!!!"), cur_text_doc->file_name, LM_ERROR);
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

//*********************** FILE SAVE AS TEMPLATE
void on_mni_file_save_as_template_activate (){  show_save_as_dlg (1);}

//*********************** FILE EDIT AT CURSOR
void on_mni_file_edit_at_cursor ()
{
	if (! get_page_text()) return;

	gchar *w = get_c_url (cur_text_doc);
	handle_file (w, 1);
	g_free (w);
}

//*********************** NEW LINK SELECT
void on_mni_new_link_select (GtkMenuItem *menuitem){  handle_file (gtk_widget_get_name (GTK_WIDGET(menuitem)), 0);}

//*********************** ADD LINK ITEM
void add_link_item (const gchar *fname, const gchar *linkname)
{
	gchar *dir = g_path_get_dirname (fname);
	gchar *filename = create_full_path (linkname, dir);

	if (g_file_test (filename, G_FILE_TEST_EXISTS))
	mni_temp = new_menu_item (filename, GTK_WIDGET(mni_links_menu), on_mni_new_link_select);

	g_free (dir);
	g_free (filename);
}

//*********************** SCAN LINKS
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

//*********************** GET LINKS
void on_mni_get_links ()
{
	scan_links ();
	icon_log_logmemo();
	log_to_memo (_("Now look in the Navigation menu-> Links."), NULL, LM_ADVICE);
	statusbar_msg (_("Scan links HTML [OK]"));
}

//*********************** COLOR SELECT
void on_mni_co_select (GtkMenuItem *menuitem)
{
	if (! get_page_text()) return;

	if (! g_file_test (cur_text_doc->file_name, G_FILE_TEST_EXISTS))
	return;

	cur_settings.selected_enc = ch_str (cur_settings.selected_enc, gtk_widget_get_name (GTK_WIDGET(menuitem)));

	if (! doc_reload_text (cur_text_doc, cur_text_doc->file_name, cur_settings.selected_enc))
	{
		icon_stop_logmemo();
		log_to_memo (_("No, I can not!"), NULL, LM_ERROR);}
	else
	{
		cur_text_doc->encoding = g_strdup (cur_settings.selected_enc);
	}
}

//*********************** HTML STRIP TAGS
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

//*********************** MARKUP ALIGN
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

//*********************** HTML MAKE TABLE
void on_mni_html_make_table ()
{
	if (! get_page_text()) return;

	set_fam_text ("2~2"); 
	gchar const *table = gtk_entry_get_text (GTK_ENTRY(ent_search));
	icon_man_logmemo();
	log_to_memo (_("[INFO] You can specify the number of rows (TR) and the number of columns (TD) in the command line\nExample :2~2\nFor 2 TR and 2 TD\n\n"), NULL, LM_GREET);
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

//*********************** FILE REVERT TO SAVED
void on_mni_file_revert_to_saved ()
{
	if (! get_page_text()) return;

	if (! g_file_test (cur_text_doc->file_name, G_FILE_TEST_EXISTS))
	return;

	if (! doc_reload_text (cur_text_doc, cur_text_doc->file_name, cur_text_doc->encoding))
	{
		icon_stop_logmemo();
		log_to_memo (_("I can not reload %s"), cur_text_doc->file_name, LM_ERROR);
	}
	else
	{
		icon_ok_logmemo();
		log_to_memo (_("%s is reloaded"), cur_text_doc->file_name, LM_NORMAL);
	}
}

//*********************** INSERT TIME
void on_mni_funx_ins_time ()
{
	if (! get_page_text()) return;

	gchar *buf = get_time (confile.date_time);
	doc_insert_at_cursor (cur_text_doc, buf);
	g_free (buf);
}

//*********************** GOTO LINE
void on_mni_goto_line ()
{
	if (! get_page_text()) return;
	set_fam_text ("0");
	doc_select_line (cur_text_doc, strtol (gtk_entry_get_text GTK_ENTRY((ent_search)), NULL, 10));

			FILE *fich;
			char carac;
			char mot[100];
			mot[0]='\0';
			int check=0;
	
			fich=fopen(confile.tea_cmd_history,"r");
			while ((carac =fgetc(fich)) != EOF)
			{
				if (carac =='\n')
				{
					if (strncmp(mot,gtk_entry_get_text (GTK_ENTRY(ent_search)),strlen(mot))==0 ){check=1;}
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
		GtkTreeIter iter_entry;
		gtk_list_store_append(model_entry, &iter_entry);
		gtk_list_store_set(model_entry, &iter_entry, CONTACT_NAME, gtk_entry_get_text (GTK_ENTRY(ent_search)),  -1);
		save_string_to_file_add(confile.tea_cmd_history,(gchar *)gtk_entry_get_text (GTK_ENTRY(ent_search)));
		save_string_to_file_add(confile.tea_cmd_history,"\n");
		}
}

//*********************** ASCII TO HTML
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

//*********************** FILTRE EXCLUDE FROM LIST
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

//*********************** GET SRC
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
	log_to_memo (_("Now look in the Navigation menu-> Links"), NULL, LM_ADVICE);
	statusbar_msg (_("Scan links [OK]"));
}

//*********************** MARKUP SELECT COLOR
void on_mni_markup_select_color_activate ()
{
	if (! get_page_text()) return;

	dlg_colorsel = gtk_color_chooser_dialog_new("Select Color",(GtkWindow *)tea_main_window);
	gtk_widget_show (dlg_colorsel);

	g_signal_connect ((gpointer) dlg_colorsel, "response",G_CALLBACK (close_color_select),NULL);
}

#define TO_HEX(x) (int) ((gdouble) x * 255.0)

//*********************** COLOR SELECT HTML
void close_color_select()
{
	GdkRGBA color;
	gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(dlg_colorsel), &color);
	gchar *s = g_strdup_printf ("#%02x%02x%02x",TO_HEX(color.red),TO_HEX(color.green),TO_HEX(color.blue));

	doc_insert_at_cursor (cur_text_doc, s);
	gtk_widget_destroy(dlg_colorsel);
}

//*********************** FILE BACKUP
void on_mni_file_backup ()
{
	if (! get_page_text()) return;
	do_backup (cur_text_doc->file_name, FALSE);
}

//*********************** FILE ADD BOOKMARK
void on_mni_file_add_bookmark ()
{
	if (! get_page_text()) return;
}

//*********************** FILE OPEN BOOKMARK
void on_mni_file_open_bookmark ()
{
	cur_settings.selected_enc = ch_str (cur_settings.selected_enc, "UTF-8");
	open_file_std (confile.bmx_file);
}

//*********************** FILE TODO
void on_mni_file_todo ()
{
	cur_settings.selected_enc = ch_str (cur_settings.selected_enc, "UTF-8");
	open_file_std (confile.tea_todo);
}

//*********************** FILE MYADMIN
void on_mni_file_myadmin ()
{
	cur_settings.selected_enc = ch_str (cur_settings.selected_enc, "UTF-8");
	open_file_std (confile.tea_myadmin);
}

//*********************** REAL RECENT ACTIVATE
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

//*********************** RELOAD HL COLOR
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
	if (buf)
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

//*********************** NAV BLOCK START
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

//*********************** NAV BLOCK END
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

//*********************** OPEN HOTKEYS
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

//*********************** OPEN FILES LIST
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

//*********************** GRIFFON WINDOW DELETE EVENT
gboolean on_tea_window_delete_event ()
{
	ui_done ();
	return FALSE;
}

//*********************** INSERT SNIPPET
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

//*********************** SNIPPET FILE OPEN
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

	g_signal_connect ((gpointer) wnd_imgviewer, "destroy_event",G_CALLBACK (gtk_false),NULL);

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

//*********************** STRINGS SORT
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

//*********************** EXTRACT WORDS
void on_mni_extract_words ()
{
	if (! get_page_text()) return;
	run_extract_words (cur_text_doc);
}

//*********************** EDIT COPY ALL
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

//*********************** EDIT COPY CURRENT URL
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

//*********************** EDIT REPLACE WITH CLIPBOARD
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

//*********************** COUNT STRING LIST
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

//*********************** PASTE TO NEW
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

//*********************** COPY TO NEW
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

//*********************** CUT TO NEW
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

//*********************** SHUFFLE STRINGS
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

//*********************** UNITAZ
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

//*********************** DELETE CURRENT LINE
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

//*********************** TAG COMMENT
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

//*********************** EDIT PASTE
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

//*********************** EDIT COPY
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

//*********************** EDIT CUT
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

//*********************** EDIT DELETE
void on_mni_edit_delete ()
{
	icon_affiche_ok();
	if (! get_page_text()) return;
	doc_rep_sel (cur_text_doc, "");
}

//*********************** EDIT SELECT ALL
void on_mni_edit_select_all ()
{
	if (! get_page_text()) return;

	GtkTextIter start_iter;
	GtkTextIter end_iter;

	gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &start_iter);
	gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &end_iter);

	gtk_text_buffer_move_mark_by_name (GTK_TEXT_BUFFER(cur_text_doc->text_buffer),"insert",&start_iter);

	gtk_text_buffer_move_mark_by_name (GTK_TEXT_BUFFER(cur_text_doc->text_buffer),"selection_bound",&end_iter);
}

//*********************** SORT CASE INSENSETIVE
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

//*********************** KILL FORMATTING
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

//*********************** WRAP ON SPACES
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

//*********************** FILE CRAPBOOK
void on_mni_file_crapbook ()
{

	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(buffer_note), "", -1);
	gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_note), (_("\nTo view notes [ALT+M]\n")), -1);
	gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_note), (_("To integrate a selection in the note  [CTR+E]\n\n____________________________________________________________________________________________________________________________________________________\n\n")), -1);

	gchar lecture[1024];
	FILE *fichier;
	fichier = fopen(confile.crapbook_file,"rt");

	if(fichier!=NULL)
	{
		while(fgets(lecture, 1024, fichier))
		{
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_note),g_locale_to_utf8(lecture, -1, NULL, NULL, NULL) , -1);
		}

		fclose(fichier);
	}
}

//************************* TODOLIST HISTORY
void on_mni_file_todolist ()
{
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(buffer_todo), "", -1);
	gchar lecture[1024];
	FILE *fichier;
	fichier = fopen(confile.tea_todo,"rt");

	if(fichier!=NULL)
	{
		while(fgets(lecture, 1024, fichier))
		{
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_todo),g_locale_to_utf8(lecture, -1, NULL, NULL, NULL) , -1);
		}

		fclose(fichier);
	}
}

//*********************** SPELLCHECK
void on_mni_spellcheck (GtkMenuItem *menuitem)
{
	if (! get_page_text()) return;
	do_hl_spell_check2 (cur_text_doc, gtk_widget_get_name (GTK_WIDGET(menuitem)));
}

//*********************** FILTER KILL LESSER
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

//*********************** FILTER KILL MORE THAN
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

//*********************** STRING SORT BY Q
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

//*********************** GET FULL NAME FILE
gchar* get_full_fname (const gchar *fname, const gchar *linkname)
{
	gchar *dir = g_path_get_dirname (fname);
	gchar *filename = create_full_path (linkname, dir);
	g_free (dir);
	return filename;
}

//*********************** SHOW IMAGES IN TEXT
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
				gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(cur_text_doc->text_buffer),&match_end,gtk_image_get_pixbuf ((GtkImage *)image));

				gtk_widget_destroy (GTK_WIDGET(image)); 
			}

			g_free (f);
			g_free (t);
		}
		match_start = match_end;
	}
}

//*********************** GOTO RECENT TAB
void on_mni_nav_goto_recent_tab ()
{
	gint t = gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook1));

	if (last_page != -1)
		gtk_notebook_set_current_page (GTK_NOTEBOOK(notebook1), last_page);

	last_page = t;
}

//*********************** NAV GOTO SELECTION
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

			FILE *fich;
			char carac;
			char mot[100];
			mot[0]='\0';
			int check=0;
	
			fich=fopen(confile.tea_cmd_history,"r");
			while ((carac =fgetc(fich)) != EOF)
			{
				if (carac =='\n')
				{
					if (strncmp(mot,gtk_entry_get_text (GTK_ENTRY(ent_search)),strlen(mot))==0 ){check=1;}
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
		GtkTreeIter iter_entry;
		gtk_list_store_append(model_entry, &iter_entry);
		gtk_list_store_set(model_entry, &iter_entry, CONTACT_NAME, gtk_entry_get_text (GTK_ENTRY(ent_search)),  -1);
		save_string_to_file_add(confile.tea_cmd_history,(gchar *)gtk_entry_get_text (GTK_ENTRY(ent_search)));
		save_string_to_file_add(confile.tea_cmd_history,"\n");
		}
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

//*********************** DELETE AUTOCOMP TIPS
void delete_autocomp_tips(){if (win_tips_autocomp != NULL){gtk_widget_destroy (GTK_WIDGET(win_tips_autocomp));win_tips_autocomp=NULL;}}

//*********************** RELACHEMENT DUNE TOUCHE EN MODE EDITOR
gboolean on_editor_keyrelease ()
{
	if (! get_page_text()) return FALSE;

	gchar *msg;
	gint row,row2, col;
	GtkTextIter iter,iter2;

	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &iter2);
	gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(cur_text_doc->text_buffer),&iter, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(cur_text_doc->text_buffer)));

	row = gtk_text_iter_get_line(&iter);
	row2 = gtk_text_iter_get_line(&iter2);
	col = gtk_text_iter_get_line_offset(&iter);

	msg = g_strdup_printf(" [File] : %s\t\t [Col] : %d \t\t[Ln] : %d/%d", cur_text_doc->file_name,col+1, row+1,row2+1);
	statusbar_msg(msg);

	g_free (msg);

/*	gchar *t;

	GtkTextIter itstart;
	GtkTextIter itend;  

	if(row2<1000)
	{
	GtkTextIter start_find, end_find;
	GtkTextIter start_match, end_match;

	t = doc_get_word_at_left (cur_text_doc, &itstart, &itend);
	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &start_find);
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &end_find);

	gtk_text_buffer_remove_tag_by_name(GTK_TEXT_BUFFER(cur_text_doc->text_buffer), "gray_bg", &start_find, &end_find);

		if (! t){return FALSE;}
		else
		{
			while ( gtk_text_iter_forward_search(&start_find, t, GTK_TEXT_SEARCH_TEXT_ONLY | GTK_TEXT_SEARCH_VISIBLE_ONLY, &start_match, &end_match, NULL) )
			{
				gtk_text_buffer_apply_tag_by_name(GTK_TEXT_BUFFER(cur_text_doc->text_buffer), "gray_bg", 
				&start_match, &end_match);
				int offset = gtk_text_iter_get_offset(&end_match);
				gtk_text_buffer_get_iter_at_offset(GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &start_find, offset);
			}
		}
		g_free (t);
	}
*/
	return TRUE;
}

//*********************** PRESSION DUNE TOUCHE EN MODE EDITOR
gboolean on_editor_keypress ( GtkWidget *widget, GdkEventKey *event, gpointer data)
{
	gtk_widget_get_name(widget);
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
				gtk_text_buffer_delete (GTK_TEXT_BUFFER(p->page->text_buffer),&p->itstart,&p->itend);
				gtk_text_buffer_insert (GTK_TEXT_BUFFER(p->page->text_buffer),&p->itstart,s,-1);
			}

		g_free (p);
		g_free (t); 
		}
	}

	char *extension;
	if (event->keyval == '(')
	{
		if(strcmp("if", t) == 0 || strcmp("unless", t) == 0 || strcmp("for", t) == 0 || strcmp("while", t) == 0)
		{
			if(confile.use_textcompbloc == 1){doc_insert_at_cursor (cur_text_doc, "()\n{\n\n\n}");	doc_move_cursor_backw(cur_text_doc,7);return TRUE;}
			else{
							doc_insert_at_cursor (cur_text_doc, ")");
							doc_move_cursor_backw(cur_text_doc,1);
						}
		}
		else
		{
				doc_insert_at_cursor (cur_text_doc, ")");
				doc_move_cursor_backw(cur_text_doc,1);
		}
	}

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

	int nbrcarac;

	if (event->keyval == '>' && confile.use_textcompbloc == 1)
	{
		if(strcmp(".html", extension) == 0 || strcmp(".htm", extension) == 0 || strcmp(".php", extension) == 0)
		{
				if(strcmp("body", t) == 0 || strcmp("table", t) == 0 || strcmp("form", t) == 0 || strcmp("td", t) == 0 || strcmp("BODY", t) == 0 || strcmp("TABLE", t) == 0 || strcmp("FORM", t) == 0 || strcmp("TD", t) == 0)
				{
					nbrcarac = strlen(t);
					nbrcarac=nbrcarac+3; 
					if(confile.use_textcompbloc == 1){doc_insert_at_cursor (cur_text_doc, "></");doc_insert_at_cursor (cur_text_doc, t);doc_insert_at_cursor (cur_text_doc, ">");doc_move_cursor_backw(cur_text_doc,nbrcarac);return TRUE;	}
				}

		if(strcmp("head", t) == 0 || strcmp("script", t) == 0 || strcmp("h1", t) == 0 || strcmp("h2", t) == 0 || strcmp("HEAD", t) == 0 || strcmp("SCRIPT", t) == 0 || strcmp("H1", t) == 0 || strcmp("H2", t) == 0)
		{
			nbrcarac = strlen(t);
			nbrcarac=nbrcarac+3; 
			if(confile.use_textcompbloc == 1){doc_insert_at_cursor (cur_text_doc, "></");doc_insert_at_cursor (cur_text_doc, t);doc_insert_at_cursor (cur_text_doc, ">");doc_move_cursor_backw(cur_text_doc,nbrcarac);return TRUE;	}
		}

		if(strcmp("p", t) == 0 || strcmp("html", t) == 0 || strcmp("li", t) == 0 || strcmp("ul", t) == 0 || strcmp("P", t) == 0 || strcmp("HTML", t) == 0 || strcmp("LI", t) == 0 || strcmp("UL", t) == 0)
		{
			nbrcarac = strlen(t);
			nbrcarac=nbrcarac+3; 
			if(confile.use_textcompbloc == 1){doc_insert_at_cursor (cur_text_doc, "></");doc_insert_at_cursor (cur_text_doc, t);doc_insert_at_cursor (cur_text_doc, ">");doc_move_cursor_backw(cur_text_doc,nbrcarac);return TRUE;	}
		}

		if(strcmp("h3", t) == 0 || strcmp("div", t) == 0 || strcmp("th", t) == 0 || strcmp("span", t) == 0 || strcmp("H3", t) == 0 || strcmp("DIV", t) == 0 || strcmp("TH", t) == 0 || strcmp("SPAN", t) == 0)
		{
			nbrcarac = strlen(t);
			nbrcarac=nbrcarac+3; 
			if(confile.use_textcompbloc == 1){doc_insert_at_cursor (cur_text_doc, "></");doc_insert_at_cursor (cur_text_doc, t);doc_insert_at_cursor (cur_text_doc, ">");doc_move_cursor_backw(cur_text_doc,nbrcarac);return TRUE;	}
		}

		if(strcmp("font", t) == 0 || strcmp("b", t) == 0 || strcmp("option", t) == 0 || strcmp("select", t) == 0 || strcmp("FONT", t) == 0 || strcmp("B", t) == 0 || strcmp("OPTION", t) == 0 || strcmp("SELECT", t) == 0)
		{
			nbrcarac = strlen(t);
			nbrcarac=nbrcarac+3; 
			if(confile.use_textcompbloc == 1){doc_insert_at_cursor (cur_text_doc, "></");doc_insert_at_cursor (cur_text_doc, t);doc_insert_at_cursor (cur_text_doc, ">");doc_move_cursor_backw(cur_text_doc,nbrcarac);return TRUE;	}
		}

		if(strcmp("u", t) == 0 || strcmp("textarea", t) == 0 || strcmp("s", t) == 0 || strcmp("ol", t) == 0 || strcmp("U", t) == 0 || strcmp("TEXTAREA", t) == 0 || strcmp("S", t) == 0 || strcmp("OL", t) == 0 || strcmp("HR", t) == 0 || strcmp("hr", t) == 0)
			{
			nbrcarac = strlen(t);
			nbrcarac=nbrcarac+3; 
			if(confile.use_textcompbloc == 1){doc_insert_at_cursor (cur_text_doc, "></");doc_insert_at_cursor (cur_text_doc, t);doc_insert_at_cursor (cur_text_doc, ">");doc_move_cursor_backw(cur_text_doc,nbrcarac);return TRUE;	}
			}

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

	return FALSE;
}

//*********************** UNINDENT
void on_mni_unindent ()
{
	if (! get_page_text()) return;
	doc_indent_selection (cur_text_doc, TRUE);
}

//*********************** INDENT
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

//*********************** AUTOCOMP FILE OPEN
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

//*********************** EDITER LE CRAPBOOK
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

//*********************** LOAD LAST FILE
void on_mni_load_last_file ()
{
	if (! recent_list)
		return;

	execute_recent_item (recent_list->data); 
}

//*********************** OPEN FICHIER HEADER .H
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

//*********************** OPEN NEW PROJET
void on_mni_project_new ()
{
	cur_tea_project = tea_proj_clear_new (cur_tea_project);
	if (cur_tea_project)
		create_proj_props_window (cur_tea_project);
}

//*********************** SAVE AS PROJET
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

//*********************** OPEN PROJET
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

	GdkPixbuf *pixbuf;
	GtkTextIter itFin;
	pixbuf = gdk_pixbuf_new_from_file("/usr/local/share/griffon/images/griffon_button.png", NULL);
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer_projet), &itFin);
	gtk_text_buffer_insert_pixbuf (GTK_TEXT_BUFFER(buffer_projet),&itFin,pixbuf);


	gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet), (_("   FILE PROJECT")), -1);
	gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet), filename, -1);
	gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer_projet), "\n\n________________________________________________________________________________________________\n\n", -1);
		
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

//*********************** MAKE PROJET
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

//*********************** EXECUTER UN PROJET
void on_mni_show_project_run (){tea_proj_run (cur_tea_project);  }

void on_mni_test ()
{
	mni_temp = NULL;
	GtkWidget *m = find_menuitem (GTK_CONTAINER (menubar1), "BR");
	if (m)
		gtk_menu_item_activate (GTK_MENU_ITEM (m));
}

//*********************** DEBUTER UN SCRIPT BASH
void print_bash (void)
{	
	time_t date;
	date = time(NULL);

	doc_insert_at_cursor (cur_text_doc, (_("#!/bin/bash\n\n# BASH script edited with Griffon: http://griffon.lasotel.fr\n# Date of creation of the script : "))); 
	doc_insert_at_cursor (cur_text_doc, ctime(&date)); 
}

//*********************** BASH HELP
void et_bash(void){doc_insert_at_cursor (cur_text_doc," && ");}
void gen_xmessage(void){doc_insert_at_cursor (cur_text_doc,(_("\n# Message on the GUI\nxmessage -center -timeout 4 \" Text display. \"\n")));}
void ou_bash(void){doc_insert_at_cursor (cur_text_doc," || ");}
void reNULL_bash(void){doc_insert_at_cursor (cur_text_doc," > /dev/null");}
void reECRASER_bash (void){doc_insert_at_cursor (cur_text_doc," > ");}
void reRAJOUTER_bash(void){doc_insert_at_cursor (cur_text_doc," >> ");}
void boucle_bash(void){doc_insert_at_cursor (cur_text_doc,(_("# Boucle \nwhile\ndo\n\ndone\n")));}
void fichier_ligne_bash (void){doc_insert_at_cursor (cur_text_doc,(_("# Reading a file line by line\nwhile read ligne\ndo\n\ndone < file_path\n")));}
void test1(void){doc_insert_at_cursor (cur_text_doc,(_(" [ -e file_path ]")));}
void test12(void){doc_insert_at_cursor (cur_text_doc,(_(" [ -s file_path ]")));}
void gen_rpm (void){doc_insert_at_cursor (cur_text_doc,(_("# Building an RPM package (thanks to the spec file in /usr/src/redhat/SPEC/)\nrpmbuild -ba fichier.spec\n")));}
void test2(void){doc_insert_at_cursor (cur_text_doc,(_(" [ -d directory ]")));}
void test22(void){doc_insert_at_cursor (cur_text_doc,(_(" [ \"string1\" != \"string2\" ]")));}
void test3(void){doc_insert_at_cursor (cur_text_doc," [ ! ]");}
void test4(void){doc_insert_at_cursor (cur_text_doc,(_(" [ -r file ]")));}
void test5(void){doc_insert_at_cursor (cur_text_doc,(_(" [ -w file ]")));}
void test6 (void){doc_insert_at_cursor (cur_text_doc,(_(" [ -x file ]")));}
void test7(void){doc_insert_at_cursor (cur_text_doc,(_(" [ -L file ]")));}
void test8(void){doc_insert_at_cursor (cur_text_doc,(_(" [ -N file ]")));}
void test24(void){doc_insert_at_cursor (cur_text_doc,(_(" [ -G file ]")));}
void test25(void){doc_insert_at_cursor (cur_text_doc,(_(" [ -O file ]")));}
void test23(void){doc_insert_at_cursor (cur_text_doc,(_(" [ -n \"string\" ]")));}
void test9 (void){doc_insert_at_cursor (cur_text_doc,(_(" [ file1 -nt file2 ]")));}
void gen_smbmount(void){doc_insert_at_cursor (cur_text_doc,(_("# Mount samba shares\nsmbmount //192.168.1.1/share /point/of/montage username=login, password=***\n")));}
void test10(void){doc_insert_at_cursor (cur_text_doc,(_(" [ \"variable1\" -eq \"variable2\" ]")));}
void test11(void){doc_insert_at_cursor (cur_text_doc,(_(" [ \"number1\" = \"number2\" ]")));}
void rsync_gen(void){doc_insert_at_cursor (cur_text_doc,(_("# Rsync command\nrsync -avH --delete /directory/source /directory/destination >> /file/of_log")));}
void echo_bash(void){doc_insert_at_cursor (cur_text_doc,(_("# Display output standare\necho \"\"\n")));}
void gen_read(void){doc_insert_at_cursor (cur_text_doc,(_("\n# Interactivity with the user\necho \"entrez une valeur \'variable\' :\"\nread variable\n\necho \"La variable = $variable\"\n")));}
void com(void){doc_insert_at_cursor (cur_text_doc,"\n#\n#\n#\n");}
void redi_erreur(void){doc_insert_at_cursor (cur_text_doc," 2>");}
void if_bash(void){doc_insert_at_cursor (cur_text_doc,(_("# Instruction if...\nif\nthen\n\nfi\n")));}
void redi_2(void){doc_insert_at_cursor (cur_text_doc,(_(" > /file 2>&1")));}
void fond_bash(void){doc_insert_at_cursor (cur_text_doc," &");}
void else_bash(void){doc_insert_at_cursor (cur_text_doc,(_("# Instruction if ...else\nif\nthen\n\nelse\n\nfi\n")));}
void skeleton(void){doc_insert_at_cursor (cur_text_doc,(_("#! /bin/sh\n\n# Script init.d (exemple)\n# Script BASH edite avec Griffon \nPATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin\nDAEMON=/usr/sbin/daemon\nNAME=daemon\nDESC=\"some daemon\"\n\ntest -x $DAEMON || exit 0\n\nset -e\n\ncase \"$1\" in\n  start)\n        echo -n \"Starting $DESC: $NAME\"\n	start-stop-daemon --start --quiet --pidfile /var/run/$NAME.pid --exec $DAEMON\n        echo \".\"\n        ;;\n  stop)\n        echo -n \"Stopping $DESC: $NAME \"\n	start-stop-daemon --stop --quiet --pidfile /var/run/$NAME.pid\n        echo \".\"\n        ;;\n  restart|force-reload)\n        echo -n \"Restarting $DESC: $NAME\"\n        start-stop-daemon --stop --quiet --pidfile /var/run/$NAME.pid --exec $DAEMON\n        sleep 1\n        start-stop-daemon --start --quiet --pidfile /var/run/$NAME.pid --exec $DAEMON\n        echo \".\"\n        ;;\n  *)\n        N=/etc/init.d/$NAME\n	echo \"Usage: $N {start|stop|restart|force-reload}\" >&2\n        ;;\nesac\n")));}

//*********************** WINDOW GREP
GtkWidget* window_grep (void)
{
	GtkWidget *window1,*vbox1,*hbox2,*vbox2,*alignment4,*hbox6,*image5,*label4,*alignment3,*hbox5,*image4,*label3,*alignment2,*hbox4,*image3,*label2,*alignment1;
	GtkWidget *hbox3,*image2,*label1,*vbox3,*vbox4,*label5,*label6,*label7,*hbox1,*button1,*button2;

	icon_affiche_stop();

	window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window1), _((_("Grep help"))));
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

	label3 = gtk_label_new_with_mnemonic (_("full word"));
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

	label2 = gtk_label_new_with_mnemonic (_("Invert the output"));
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

	label7 = gtk_label_new (_("Redirection file"));
	gtk_widget_show (GTK_WIDGET(label7));
	gtk_box_pack_start (GTK_BOX (vbox4), label7, FALSE, FALSE, 0);

	entry_grep3 = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_grep3));
	gtk_box_pack_start (GTK_BOX (vbox4), entry_grep3, FALSE, FALSE, 0);

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

	g_signal_connect_swapped ((gpointer) button1, "clicked",G_CALLBACK (icon_affiche_ok), NULL);

	g_signal_connect_swapped ((gpointer) button1, "clicked",G_CALLBACK (gtk_widget_destroy), window1);
	g_signal_connect_swapped ((gpointer) button2, "clicked",G_CALLBACK (grep),NULL);

	g_signal_connect_swapped ((gpointer) button2, "clicked",G_CALLBACK (icon_affiche_ok),NULL);
	g_signal_connect_swapped ((gpointer) button2, "clicked",G_CALLBACK (gtk_widget_destroy),window1);

	return window1;
}

//*********************** BUILD CMD GREP
void grep (void)
{	
	gchar *tampon_fichier;
	gchar *tampon_mot;
	gchar *tampon_redi;

	tampon_fichier = gtk_editable_get_chars(GTK_EDITABLE(entry_grep1),0, -1);
	tampon_mot = gtk_editable_get_chars(GTK_EDITABLE(entry_grep2),0, -1);
	tampon_redi = gtk_editable_get_chars(GTK_EDITABLE(entry_grep3),0, -1);

	doc_insert_at_cursor (cur_text_doc,(_("# Grep Command (Find in a file)\ngrep")));

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

//*********************** WINDOW SED
GtkWidget* sed (void)
{
	GtkWidget *window1,*vbox1,*hbox2,*vbox5,*notebook1,*vbox6,*label15,*label16,*hbox3,*image2,*label17,*vbox7,*hbox5,*label19,*label20;
	GtkWidget *hbox4,*image3,*label18,*vbox8,*hbox6,*image4,*label21,*vbox3,*vbox4,*label5,*label6,*hbox1,*button1,*button2;

	icon_affiche_stop();

	window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window1), _((_("Sed help"))));
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

	label15 = gtk_label_new (_("Replace "));
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

	checkbutton_sed2 = gtk_check_button_new_with_mnemonic (_("Delete Rows"));
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

	checkbutton_sed3 = gtk_check_button_new_with_mnemonic (_("Delete rows with the search word"));
	gtk_widget_show (GTK_WIDGET(checkbutton_sed3));
	gtk_box_pack_start (GTK_BOX (vbox7), checkbutton_sed3, FALSE, FALSE, 0);

	checkbutton_sed4 = gtk_check_button_new_with_mnemonic (_("Invert delete"));
	gtk_widget_show (GTK_WIDGET(checkbutton_sed4));
	gtk_box_pack_start (GTK_BOX (vbox7), checkbutton_sed4, FALSE, FALSE, 0);

	hbox4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox4));
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), hbox4);

	image3 = gtk_image_new_from_stock ("gtk-clear", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show (GTK_WIDGET(image3));
	gtk_box_pack_start (GTK_BOX (hbox4), image3, TRUE, TRUE, 0);

	label18 = gtk_label_new (_("Delete"));
	gtk_widget_show (GTK_WIDGET(label18));
	gtk_box_pack_start (GTK_BOX (hbox4), label18, FALSE, FALSE, 0);

	vbox8 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_show (GTK_WIDGET(vbox8));
	gtk_container_add (GTK_CONTAINER (notebook1), vbox8);

	checkbutton_sed5 = gtk_check_button_new_with_mnemonic (_("Displays the result on the standard output"));
	gtk_widget_show (GTK_WIDGET(checkbutton_sed5));
	gtk_box_pack_start (GTK_BOX (vbox8), checkbutton_sed5, FALSE, FALSE, 0);

	checkbutton_sed6 = gtk_check_button_new_with_mnemonic (_("Gives the number of lines"));
	gtk_widget_show (GTK_WIDGET(checkbutton_sed6));
	gtk_box_pack_start (GTK_BOX (vbox8), checkbutton_sed6, FALSE, FALSE, 0);

	hbox6 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox6));
	gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 2), hbox6);

	image4 = gtk_image_new_from_stock ("gtk-add", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show (GTK_WIDGET(image4));
	gtk_box_pack_start (GTK_BOX (hbox6), image4, TRUE, TRUE, 0);

	label21 = gtk_label_new (_("Function"));
	gtk_widget_show (GTK_WIDGET(label21));
	gtk_box_pack_start (GTK_BOX (hbox6), label21, FALSE, FALSE, 0);

	vbox3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_show (GTK_WIDGET(vbox3));
	gtk_box_pack_start (GTK_BOX (hbox2), vbox3, TRUE, TRUE, 14);

	vbox4 =gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_show (GTK_WIDGET(vbox4));
	gtk_box_pack_start (GTK_BOX (vbox3), vbox4, FALSE, FALSE, 0);

	label5 = gtk_label_new (_("File"));
	gtk_widget_show (GTK_WIDGET(label5));
	gtk_box_pack_start (GTK_BOX (vbox4), label5, FALSE, FALSE, 0);
	gtk_misc_set_alignment (GTK_MISC (label5), 0.48, 0.5);

	entry_sed1 = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_sed1));
	gtk_box_pack_start (GTK_BOX (vbox4), entry_sed1, FALSE, FALSE, 0);

	label6 = gtk_label_new (_("search word"));
	gtk_widget_show (GTK_WIDGET(label6));
	gtk_box_pack_start (GTK_BOX (vbox4), label6, FALSE, FALSE, 0);

	entry_sed2 = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_sed2));
	gtk_box_pack_start (GTK_BOX (vbox4), entry_sed2, FALSE, FALSE, 0);

	checkbutton_sed7 = gtk_check_button_new_with_mnemonic (_("Redirection file"));
	gtk_widget_show (GTK_WIDGET(checkbutton_sed7));
	gtk_box_pack_start (GTK_BOX (vbox4), checkbutton_sed7, FALSE, FALSE, 0);

	entry_sed3 = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_sed3));
	gtk_box_pack_start (GTK_BOX (vbox4), entry_sed3, FALSE, FALSE, 0);

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

	g_signal_connect_swapped ((gpointer) button1, "clicked",G_CALLBACK (icon_affiche_ok),NULL);
	g_signal_connect_swapped ((gpointer) button1, "clicked", G_CALLBACK (gtk_widget_destroy), window1);
	g_signal_connect_swapped ((gpointer) button2, "clicked",G_CALLBACK (sed_apl),NULL);
	g_signal_connect_swapped ((gpointer) button2, "clicked",G_CALLBACK (icon_affiche_ok),NULL);
	g_signal_connect_swapped ((gpointer) button2, "clicked",G_CALLBACK (gtk_widget_destroy),window1);

	return window1;
}

//*********************** BUILD SED
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

	doc_insert_at_cursor (cur_text_doc,(_("# Sed command (cutting file)\nsed ")));
	
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
		doc_insert_at_cursor (cur_text_doc,(_(" buffer_word")));
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

//*********************** WINDOW BASH CASE OPTIONS
GtkWidget* case_window (void)
{	
	GtkWidget *window1,*frame1,*vbox1,*hbox1,*image1,*label2,*button1,*label1;
	GtkAdjustment *spinbutton1_adj;

	icon_affiche_stop();

	window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window1), _((_("Number of options"))));
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

	label2 = gtk_label_new (_("Number of options in the script:"));
	gtk_widget_show (GTK_WIDGET(label2));
	gtk_box_pack_start (GTK_BOX (hbox1), label2, TRUE, FALSE, 0);

	spinbutton1_adj = gtk_adjustment_new (1, 0, 100, 1, 10, 10);
	spinbutton1 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton1_adj), 1, 0);
	gtk_widget_show (GTK_WIDGET(spinbutton1));
	gtk_box_pack_start (GTK_BOX (hbox1), spinbutton1, FALSE, TRUE, 4);
	gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinbutton1), TRUE);

	button1 = gtk_button_new_from_stock ("gtk-apply");
	gtk_widget_show (GTK_WIDGET(button1));
	gtk_box_pack_start (GTK_BOX (vbox1), button1, FALSE, FALSE, 0);
	gtk_button_set_relief (GTK_BUTTON (button1), GTK_RELIEF_NONE);

	label1 = gtk_label_new (_("Case"));
	gtk_widget_show (GTK_WIDGET(label1));
	gtk_frame_set_label_widget (GTK_FRAME (frame1), label1);

	g_signal_connect_swapped ((gpointer) button1, "clicked",G_CALLBACK (case_build), NULL);
	g_signal_connect_swapped ((gpointer) button1, "clicked", G_CALLBACK (icon_affiche_ok),NULL);
	g_signal_connect_swapped ((gpointer) button1, "clicked", G_CALLBACK (gtk_widget_destroy),window1);
	return window1;
}

//*********************** BUILD BASH CASE
void case_build (void)
{
	int num_conteur=0;
	gint tampon_num;
	tampon_num = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinbutton1));
	
	doc_insert_at_cursor (cur_text_doc,"case \"$1\" in\n");
	while (num_conteur!=tampon_num)
	{
		doc_insert_at_cursor (cur_text_doc,(_("\noption)\n#insert here the script for this option\n\n;;")));
		num_conteur++;	
	}	
	doc_insert_at_cursor (cur_text_doc,"\nesac\n");

}

//*********************** WINDOW AWK
GtkWidget* awk (void)
{
	GtkWidget *window1,*vbox1,*hbox2,*vbox5,*vbox6,*hbox3,*image1,*label9,*label29,*label8,*vbox3,*vbox4,*label5,*label7,*hbox1,*button1,*button2,*image2;
	GtkWidget *alignment1;
	icon_affiche_stop();

	window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window1), _((_("Awk help"))));
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

	label29 = gtk_label_new_with_mnemonic (_("Separator"));
	gtk_widget_show (GTK_WIDGET(label29));
	gtk_box_pack_start (GTK_BOX (vbox6), label29, FALSE, FALSE, 0);

	entry_awk4 = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_awk4));
	gtk_box_pack_start (GTK_BOX (vbox6), entry_awk4, FALSE, FALSE, 0);

	label8 = gtk_label_new (_("Column"));
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

	g_signal_connect_swapped ((gpointer) button1, "clicked",G_CALLBACK (icon_affiche_ok), NULL);
	g_signal_connect_swapped ((gpointer) button1, "clicked", G_CALLBACK (gtk_widget_destroy), window1);
	g_signal_connect_swapped ((gpointer) button2, "clicked", G_CALLBACK (awk_apl),NULL);
	g_signal_connect_swapped ((gpointer) button2, "clicked",G_CALLBACK (icon_affiche_ok),NULL); 
	g_signal_connect_swapped ((gpointer) button2, "clicked",G_CALLBACK (gtk_widget_destroy), window1);

	return window1; 
}

//*********************** BUILD AWK
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

	doc_insert_at_cursor (cur_text_doc,(_("# Awk command (cutting file column)\nawk -F \"")));
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

//*********************** SAVE BUG
void file_save_bug (void)
{
	if (! get_page_text()) return;

	text_doc_save (cur_text_doc, ".fichierbackup");
	icon_log_logmemo();
	log_to_memo (_("Scan file [START]"), NULL, LM_NORMAL);
}

//*********************** PATH SELECT
GtkWidget* selection_path (void)
{
	GtkWidget *pFileSelection;
	icon_affiche_stop();

	icon_man_logmemo();
	log_to_memo (_("[Select a PATH] to insert the file"), NULL, LM_NORMAL);
	pFileSelection=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	return pFileSelection;
}

//*********************** SCP
GtkWidget* win_scp (void)
{
	GtkWidget *window1;
	icon_affiche_net();	

	window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window1), _((_("Style variable"))));
	gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);
	gtk_widget_show(GTK_WIDGET(window1));
	gtk_window_set_resizable (GTK_WINDOW (window1), FALSE);
	gtk_window_set_modal (GTK_WINDOW (window1), TRUE);
return window1;	
}	

//*********************** BUILD SCP
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

//*********************** START SCRIPT PERL
void start_perl_script            (void)
{
	time_t date;
	date = time(NULL);

	doc_insert_at_cursor (cur_text_doc, (_("#!/usr/bin/perl\n\n# Script perl Griffon: http://griffon.lasotel.fr\n# Date start script : "))); 
	doc_insert_at_cursor (cur_text_doc, ctime(&date)); 
	doc_insert_at_cursor (cur_text_doc, "\n\n");
}

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
void term_git_status(gpointer user_data){vte_terminal_feed_child (user_data,"git status\n",-1);}
void term_git_commit(gpointer user_data){vte_terminal_feed_child (user_data,"git commit -a\n",-1);}
void term_git_push(gpointer user_data){vte_terminal_feed_child (user_data,"git push origin master\n",-1);}
void term_iptables_drop(gpointer user_data){vte_terminal_feed_child (user_data,"iptables -A INPUT -s 192.192.192.1 -j DROP",-1);}
void term_iptables_clean(gpointer user_data){vte_terminal_feed_child (user_data,"iptables -F\niptables -t nat -F\niptables -t mangle -F\niptables -X\n",-1);}

void perl_print            (void){  doc_insert_at_cursor (cur_text_doc, "print \" \";\n\n"); }
void perl_if            (void){  doc_insert_at_cursor (cur_text_doc, "if ()\n{\n\n}\n\n"); }
void perl_unless            (void){  doc_insert_at_cursor (cur_text_doc, "unless ()\n{\n\n}\n\n"); }
void perl_if_else            (void){  doc_insert_at_cursor (cur_text_doc, "if ()\n{\n\n}\nelse\n{\n\n}\n\n"); }
void perl_while (void){ doc_insert_at_cursor (cur_text_doc, "while ()\n{\n\n}\n\n");}
void perl_while_infinie (void){ doc_insert_at_cursor (cur_text_doc, "while (1)\n{\n\n}\n\n");}
void perl_for            (void){  doc_insert_at_cursor (cur_text_doc, "for (init;condition;cmd)\n{\n\n}\n\n"); }
void perl_foreach            (void){  doc_insert_at_cursor (cur_text_doc, (_("foreach $value (@array)\n{\n\n}\n\n"))); }
void perl_do_while            (void){  doc_insert_at_cursor (cur_text_doc, "do \n{\n\n} while ();\n\n"); }
void perl_fonction            (void){  doc_insert_at_cursor (cur_text_doc, (_("sub function_name\n{\n\n}\n\n"))); }
void perl_fonction_param            (void){  doc_insert_at_cursor (cur_text_doc, (_("sub function_name\n{\n\t# display the argument 0 and 1 function\n\tprint $_[0];\n\tprint $_[1];\n}\n\n"))); }
void perl_exit           (void){  doc_insert_at_cursor (cur_text_doc, "exit();\n\n"); }
void perl_die            (void){  doc_insert_at_cursor (cur_text_doc, "die'Error';\n\n");}

//*********************** RENDRE EXECUTABLE UN SCRIPT
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

void perl_chomp(void){doc_insert_at_cursor (cur_text_doc, "chomp('');\n\n");}
void perl_length(void){doc_insert_at_cursor (cur_text_doc, (_("$number_of_characters=length($string);\n\n")));}
void perl_uc(void){doc_insert_at_cursor (cur_text_doc, (_("$string=uc($string);\n\n")));}
void perl_lc(void){doc_insert_at_cursor (cur_text_doc, (_("$string=lc($string);\n\n")));}
void perl_ucfirst(void){doc_insert_at_cursor (cur_text_doc, (_("$string=ucfirst($string);\n\n")));}

//*********************** SCAN DE FICHIER INCLUDE
void scan_include             (void)
{	

	if (! get_page_text()) return;
	FILE *fich;
	char carac;

	char motrch[100],motrch2[100],motrch3[100],motrch4[100],motrch5[100],motrch6[100],motrch7[100],motrch8[100],motrch9[100],motrch10[100],motrch11[100],motrch12[100],mot_autocomp[500],mot[1000],mot2[1000],ligne[10],mot3[1000],motrch13[100],motrch14[100],motrch15[100];
	int nbapparition=0,nbcarac=0,nbmot=0,counter=0;
	int nbligne=1;
	char *extension;
	int instruction=1;
	if (! get_page_text()) return;
	
	icon_affiche_bug();
	clear_list_include ();
	clear_list_todo ();

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
	motrch13[0]='\0';
	motrch14[0]='\0';
	motrch15[0]='\0';

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
	strcpy(motrch13,"TODO");
	strcpy(motrch14,"FIXME");
	strcpy(motrch15,"BUG");
	
	GtkTextIter itstart, itend;

	add_to_list_err("",0);

	fich=fopen(cur_text_doc->file_name,"r");
	while ((carac =fgetc(fich)) != EOF)
	{

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

	//******************************* pour todolist
	if(counter==5)
	{
		strncat(mot,&carac,1);

		if (carac =='\n' || carac =='\r')
		{
			gchar **b = g_strsplit (mot, "\n", -1);

			if(b[0]!='\0')
			{
				sprintf(ligne,"%d",nbligne); 
				add_to_list_todo(b[0],ligne);
			}

		mot[0]='\0';
		ligne[0]='\0';
		counter=0;  
		}
	}

	//******************************* pour todolist
	if(counter==6)
	{
		strncat(mot,&carac,1);

		if (carac =='\n' || carac =='\r')
		{
			gchar **b = g_strsplit (mot, "\n", -1);

			if(b[0]!='\0')
			{
				sprintf(ligne,"%d",nbligne); 
				add_to_list_todo_fixme(b[0],ligne);
			}

		mot[0]='\0';
		ligne[0]='\0';
		counter=0;  
		}
	}

	//******************************* pour todolist
	if(counter==7)
	{
		strncat(mot,&carac,1);

			if (carac =='\n' || carac =='\r')
			{
				gchar **b = g_strsplit (mot, "\n", -1);

				if(b[0]!='\0')
				{
					sprintf(ligne,"%d",nbligne); 
					add_to_list_todo_bug(b[0],ligne);
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

				//******TODO 
			if (strncmp(motrch13,mot,strlen(motrch13))==0) 
			{
				nbapparition++;
				if(nbapparition==1)
				{
					nbcarac--;
				}

			counter=5;// mode fonction      
	    }	

				//******TODO 
				if (strncmp(motrch14,mot,strlen(motrch14))==0) 
				{
					nbapparition++;
					if(nbapparition==1)
					{
						nbcarac--;
					}

					counter=6;// mode fonction      
				}	

				//******TODO 
				if (strncmp(motrch15,mot,strlen(motrch15))==0) 
				{
					nbapparition++;
					if(nbapparition==1)
					{
						nbcarac--;
					}

					counter=7;// mode fonction      
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

//*********************** HELP PERL
void perl_read (void){  doc_insert_at_cursor (cur_text_doc, (_("open (FILE, \"file_path\");\nwhile ($line=<FILE>)\n{\nprint $line;\n}\n\nclose FILE;\n\n"))); }
void perl_writh (void){  doc_insert_at_cursor (cur_text_doc, (_("open (FILE, \">>file_path\");\nprint FILE \"Text written to the file\";\nnclose FILE;\n\n"))); }
void perl_writh2 (void){  doc_insert_at_cursor (cur_text_doc, (_("open (FILE, \">file_name\");\nprint FILE \"Text written to the file\";\nnclose FILE;\n\n"))); }
void perl_regular(void){  doc_insert_at_cursor (cur_text_doc, (_("$string =~/string search/"))); }
void perl_replace (void){  doc_insert_at_cursor (cur_text_doc, (_("$string =~ s/string search/string replacement/g;\n\n"))); }

void start_php_script            (void)
{
	time_t date;
	date = time(NULL);

	doc_insert_at_cursor (cur_text_doc, (_("<?\n\n/* \n * Script PHP Griffon: http://griffon.lasotel.fr\n * Date start script : "))); 
	doc_insert_at_cursor (cur_text_doc, ctime(&date)); 
	doc_insert_at_cursor (cur_text_doc, " *\n */\n\n?>");   
}

void perl_tableau (void){doc_insert_at_cursor (cur_text_doc, (_("@array = (\"var0\",\"var1\",\"var2\");\n# display var0\nprint $array[0];\n\n"))); }
void perl_saisie_clavier (void){doc_insert_at_cursor (cur_text_doc, (_("#  <STDIN> expects keyboard input\nwhile (<STDIN>)\n{\n\t# avoids \\n the last field\n\tchop;\n\t# displays the character entered without his  \\n\n\tprint;\n}\n\n"))); }
void perl_empiler_tableau (void){doc_insert_at_cursor (cur_text_doc, (_("# Stacks value at end of table\npush $array, (\"value\");\n\n"))); }
void perl_empiler_tableau_start (void){doc_insert_at_cursor (cur_text_doc, (_("# Stacks value at beginning of table\nshift $array, (\"value\");\n\n"))); }
void perl_arguments (void){doc_insert_at_cursor (cur_text_doc, (_("# the ARGV array contains the list of program arguments.\n$argument0 = $ARGV[0];\n\n"))); }
void perl_split (void){doc_insert_at_cursor (cur_text_doc, (_("# cutting the $string variable with respect to the separator \";\" and places the result in a array\n@array = split(/;/, $string);\n\n"))); }
void perl_dbi_connexion (void){doc_insert_at_cursor (cur_text_doc, (_("\nuse DBI;\n\n$bd = 'base_name';\n$server = 'localhost';\t\t\n$login = 'root';\t\t\n$passwd  = 'admin';\t\t\n\n\n$dbh = DBI->connect( \"DBI:mysql:$bd:$server\", $login, $passwd) or die \"Connexion impossible à la base de données $bd !\";\n\n"))); }
void perl_dbi_select_while (void){doc_insert_at_cursor (cur_text_doc, (_("# Loop on a MySql query\n$query = \"SELECT group_attribute FROM table WHERE condition\";\n$sth = $dbh->prepare($query);\n$sth->execute;\n\nwhile($row = $sth->fetchrow_hashref)\n{\n\n\t# Recovery values   \n\t$champs_mysql = \"$row->{champs_mysql}\";\n\n}"))); }
void perl_dbi_query (void){doc_insert_at_cursor (cur_text_doc, (_("# single MySql query\n$query = \"\";\n$sth = $dbh->do($query);\n\n"))); }
void perl_chop (void){doc_insert_at_cursor (cur_text_doc, (_("chop($string);\n\n"))); }
void perl_mime_mail_simple (void){doc_insert_at_cursor (cur_text_doc, (_("use MIME::Lite;\n\n####### SEND EMAIL\nmy $Message = new MIME::Lite\nFrom =>\"NAME FROM <from\\@from.net>\",\nTo =>$email_to,\nSubject =>$subject,\nType    =>\'TEXT\',\nData =>\"$message\",\nContent-Transfer-Encoding => \'quoted-printable\';\n\n$Message->send;\n\n"))); }
void perl_date (void){doc_insert_at_cursor (cur_text_doc, (_("($sec,$min,$hour,$day,$month,$year,$sday,$aday,$isdst) = localtime(time);\n$month=$month+1;\nif($month<10){$month=\"0\".$month;}\nif($day<10){$day=\"0\".$day;}\n$year=1900+$year;\n\n$date_sql=$year.\"-\".$month.\"-\".$day;\n\n"))); }
void perl_mime_mail_pj (void){doc_insert_at_cursor (cur_text_doc, (_("use MIME::Lite;\n\n####### SEND EMAIL ATTACH\nmy $Message = new MIME::Lite\nFrom =>\"NAME_FROM <from\\@from.net>\",\nTo =>$email_to,\nSubject =>$subject,\nType =>\'multipart/mixed\';\n\nattach $Message\nType =>'TEXT',\nData =>$message;\n\nattach $Message\nType =>\'application/txt\',\nPath =>$local_path_file,\nFilename =>$name_file_txt;\n\n$Message->send;\n\n"))); }
void perl_dbi_query_select (void){doc_insert_at_cursor (cur_text_doc, (_("$query2 = \"SELECT group_attribut FROM table WHERE condition\";\n$sth2 = $dbh->prepare($query2);\n$sth2->execute;\n\n$row2 = $sth2->fetchrow_hashref;\n$value_mysql = \"$row2->{value_mysql}\";\n\n"))); }
void perl_uri_encode (void){doc_insert_at_cursor (cur_text_doc, (_("use URI::Escape;\n\n$url_encode = uri_escape($url_encoder);\n\n"))); }
void perl_url_get (void){doc_insert_at_cursor (cur_text_doc, (_("use LWP::Simple;\n\n$return = get($url);\n\n"))); }
void perl_ftp_connexion (void){doc_insert_at_cursor (cur_text_doc, (_("use Net::FTP;\n\n$FTP_HOST=\"\";\n$FTP_USER=\"\";\n$FTP_PASSWORD=\"\";\n\n$ftp = Net::FTP->new($FTP_HOST, Debug => 0);\n$ftp->login($FTP_USER,$FTP_PASSWORD);\n\n"))); }
void perl_ftp_list (void){doc_insert_at_cursor (cur_text_doc, (_("$ftp->cwd(\"$a_directory_listing\");\n@ls = $ftp->ls(\".\");\n\nforeach $file (@ls)\n{\n\n\tprint \"$file\\n\";\n\n}\n\n"))); }
void perl_ftp_get (void){doc_insert_at_cursor (cur_text_doc, (_("$ftp->get($file_source_ftp,\"$file_destination_local\");\n\n"))); }
void perl_ftp_put (void){doc_insert_at_cursor (cur_text_doc, (_("$ftp->put($file_local,\"$file_destination_ftp\");\n\n"))); }
void perl_socket_srv (void){doc_insert_at_cursor (cur_text_doc, (_("use IO::Socket::INET ;\n\n$server = new IO::Socket::INET(\nType=>SOCK_STREAM,\n                LocalPort => $port ,\n                Proto => 'tcp' ,\n                Listen => 0 ,\n                Reuse => 1,\n\n);\n\n\nwhile($client = $server->accept)\n{\n\n\tif(fork())\n\t{\n\t$ipclient_connect=sprintf \"%s\", $client->peerhost;\n\t$client->autoflush(1);\n\n\tprint $client \"$hostname\\n\";\n\n\t\twhile (1)\n\t\t{\n\t\t$chaine_client=<$client>;\n\t\tprint $chaine_client.\"\\n\";\n\n\n\t\t######### Envoyer au client\n\t\tprint $client \"$chaine_a_envoyer_au_client\";\n\t\t}\n\tclose $client;\n\texit();\n\n\t}\n}\n\n"))); }
void perl_socket_client (void){doc_insert_at_cursor (cur_text_doc, "use IO::Socket;\n\n\n$socket = IO::Socket::INET->new(Proto    => \"tcp\",\n                                            PeerAddr => $ip_serveur,\n                                            PeerPort => $port)\n\tor die \"Failed : $@\\n\";\n\n\n\nwhile($reponse=<$socket>)\n{\n\t########## Envoi sur la socket\n\tprint $socket \"load\\n\";\n\n}\n\n"); }
void perl_substr (void){doc_insert_at_cursor (cur_text_doc, (_("$string_return=substr($string, $int_indice_start, $int_length);\n\n"))); }
void perl_tube (void){doc_insert_at_cursor (cur_text_doc, (_("$file=\"tail -f -n 1 /path/file/pipe |\";\nopen(FILE, $file);\n\nwhile($line=<FILE>)\n{\n\tprint $line;\n}\n\nclose FILE;\n\n"))); }
void perl_stdin (void){doc_insert_at_cursor (cur_text_doc, (_("while ($line=<STDIN>)\n{\n\n\n}\n\n"))); }
void perl_dbi_lastid (void){doc_insert_at_cursor (cur_text_doc, "$lastid=$dbh->{'mysql_insertid'};\n\n"); }
void perl_rand (void){doc_insert_at_cursor (cur_text_doc, (_("$rand=rand($number_max);\n\n"))); }
void perl_stat (void){doc_insert_at_cursor (cur_text_doc, (_("# dev\t\tnuméro de device du système de fichiers\n# ino\t\tnuméro d'inode\n# mode\t\tdroits du fichier (type et permissions)\n# nlink\t\tnombre de liens (hard) sur le fichier\n# uid\t\tID numérique de l'utilisateur propriétaire du fichier\n# gid\t\tID numérique du groupe propriétaire du fichier\n# rdev\t\tl'identificateur de device (fichiers spéciaux uniquement)\n# size\t\ttaille totale du fichier, en octets\n# atime\t\tdate de dernier accès en secondes depuis l'origine des temps\n# mtime\t\tdate de dernière modification en secondes depuis l'origine des temps\n# ctime\t\tdate de dernière modification de l'inode en secondes depuis l'origine des temps (*)\n# blksize\t\ttaille de blocs préférée pour les E/S sur fichiers\n# blocks\t\tnombre de blocs réellement occupés\n\n($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks)= stat($filename);\n\n"))); }
void perl_ftp_cwd (void){doc_insert_at_cursor (cur_text_doc, (_("$ftp->cwd(\"$dir\");\n"))); }
void perl_ftp_delete (void){doc_insert_at_cursor (cur_text_doc, (_("$ftp->delete(\"$file\");\n"))); }
void perl_dbi_rows (void){doc_insert_at_cursor (cur_text_doc, (_("$rows = $sth2->rows;\n"))); }

//*********************** MYSQL HELP
void mysql_insert (void){doc_insert_at_cursor (cur_text_doc, (_("INSERT INTO your_table (name_attribut1,name_attributN) values ('value1','valueN')"))); }
void mysql_update (void){doc_insert_at_cursor (cur_text_doc, (_("UPDATE name_table Set name_attribut1=value1,name_attributN=valueN WHERE condition"))); }
void mysql_select (void){doc_insert_at_cursor (cur_text_doc, (_("SELECT group_attribut FROM table WHERE condition"))); }
void mysql_delete (void){doc_insert_at_cursor (cur_text_doc, (_("DELETE attributN FROM name_table WHERE condition"))); }

//*********************** HELP PHP
void php_commentaire (void){doc_insert_at_cursor (cur_text_doc, "/*\n *\n *\n *\n */\n\n"); }
void php_mysql_connexion (void){doc_insert_at_cursor (cur_text_doc, "$dbhost=\"ip_host_name\";\n$dblogin=\"login\";\n$dbpassword=\"password\";\n$dbname=\"nom_de_base\";\n\n$db=mysql_connect($dbhost,$dblogin,$dbpassword);\nmysql_selectdb($dbname,$db);\n\nmysql_query(\"SET NAMES 'utf8'\");\n\n"); }
void php_mysql_while (void){doc_insert_at_cursor (cur_text_doc, (_("$inforeq = \"SELECT values FROM tables WHERE conditions\";\n$infoq = mysql_query($inforeq);\n\nwhile($line = mysql_fetch_object($infoq))\n{\n\t$value=$line->value;\n\n}\n\n"))); }
void php_mysql_query (void){doc_insert_at_cursor (cur_text_doc, "$requete = \"\";\n$result = mysql_query($requete);\n\n"); }
void php_foreach (void){doc_insert_at_cursor (cur_text_doc, (_("foreach ($array as $line_num => $line)\n{\n\n\n}\n\n"))); }
void php_split (void){doc_insert_at_cursor (cur_text_doc, (_("$tableau = preg_split(\"/[;]+/\", $variable);\n\n"))); }
void php_replace (void){doc_insert_at_cursor (cur_text_doc, (_("$chaine = eregi_replace (\"chaine à remplacer\",\"chaine de remplacement\",$chaine);\n\n"))); }
void php_file (void){doc_insert_at_cursor (cur_text_doc, (_("$array = file(\"http://url or file\");\n\n"))); }
void php_find (void){doc_insert_at_cursor (cur_text_doc, (_("if(strstr($string, $find_string))\n{\n\n\n}\n\n"))); }
void php_function (void){doc_insert_at_cursor (cur_text_doc, (_("function function_name($var1,$var2)\n{\n\n\n}\n\n"))); }
void php_stripslashes (void){doc_insert_at_cursor (cur_text_doc, (_("$string=stripslashes($string);\n\n"))); }
void php_mail (void){doc_insert_at_cursor (cur_text_doc, (_("mail(\"email\",\"subject\",\"message\",\"From: name_frome <email@sender_domaine.net>\\nContent-Type: text;\\n\");\n\n"))); }
void php_insert_id (void){doc_insert_at_cursor (cur_text_doc, (_("$id_insert=mysql_insert_id();\n"))); }
void php_ftp_connect (void){doc_insert_at_cursor (cur_text_doc, (_("//****** Connexion serveur FTP\n$ftp_connexion = ftp_connect(\"$ip_serveur\");\n$ftp_login = ftp_login($ftp_connexion,\"$login\", \"$password\");\n$ftp_repertoire = ftp_chdir($ftp_connexion,\"/\");\n\n"))); }
void php_ftp_list_rep (void){doc_insert_at_cursor (cur_text_doc, (_("//******* List files in a $directory in array\n$array = ftp_nlist($ftp_connection,\"$directory\");\n\n"))); }
void php_ftp_put (void){doc_insert_at_cursor (cur_text_doc, (_("ftp_put($ftp_connection,$name_file_put,$path_file, FTP_BINARY);\n\n"))); }
void php_ftp_delete (void){doc_insert_at_cursor (cur_text_doc, (_("ftp_delete($ftp_connection, $file_delete);\n\n"))); }
void php_h_utf8 (void){doc_insert_at_cursor (cur_text_doc, "header(\'Content-type: text/plain; charset=UTF-8\');\n\n"); }
void php_h_png (void){doc_insert_at_cursor (cur_text_doc, "Header(\"Content-type: image/png\");\n\n"); }
void php_date_mysql (void){doc_insert_at_cursor (cur_text_doc, (_("$now = date(\"Y-m-d H:i:s\");\n\n"))); }
void php_mysql_query_select (void){doc_insert_at_cursor (cur_text_doc, (_("$inforeq2 = \"SELECT values FROM tables WHERE conditions\";\n$infoq2 = mysql_query($inforeq2);\n$ligne2 = mysql_fetch_object($infoq2);\n$champs=$ligne2->champs;\n\n"))); }
void php_strlen (void){doc_insert_at_cursor (cur_text_doc, (_("$number_of_characters=strlen($string);\n\n"))); }
void php_urlencode (void){doc_insert_at_cursor (cur_text_doc, (_("$url=urlencode($string);\n\n"))); }
void php_rand (void){doc_insert_at_cursor (cur_text_doc, (_("$value=rand($min,$max );\n\n"))); }
void php_mb_substr_count (void){doc_insert_at_cursor (cur_text_doc, (_("$occurrence_count=mb_substr_count($string, $find_string);\n\n"))); }
void php_var_post (void){doc_insert_at_cursor (cur_text_doc, "print_r($_POST);\n\n"); }
void echo_php           (void){  doc_insert_at_cursor (cur_text_doc, "echo \"\";\n\n"); }
void php_strip_tags           (void){  doc_insert_at_cursor (cur_text_doc, (_("$string=strip_tags($string_html);\n\n"))); }
void php_num_rows           (void){  doc_insert_at_cursor (cur_text_doc, (_("$num_rows=mysql_num_rows($result);\n"))); }
void php_explode           (void){  doc_insert_at_cursor (cur_text_doc, (_("$array = explode(\" \", $string);\n"))); }
void php_unlink           (void){  doc_insert_at_cursor (cur_text_doc, (_("unlink($file);\n"))); }
void php_fputs           (void){  doc_insert_at_cursor (cur_text_doc, (_("$fp=fopen($file,\"w\");\nfputs($fp, \"text in file\");\nfclose($fp);\n"))); }
void php_count           (void){  doc_insert_at_cursor (cur_text_doc, (_("$nbr=count($array);\n"))); }
void php_dir           (void){  doc_insert_at_cursor (cur_text_doc, (_("$mydir = \"/dir/\";\nif ($dir = opendir($mydir)) \n{\n\twhile (($file = readdir($dir)) !== false) \n\t{\n\t\tif($file != \"..\" && $file != \".\")\n\t\t{\n\t\t$filelist[] = $file;\n\t\t}\n\t} \nclosedir($dir);\n}"))); }
void php_addslashes(void){  doc_insert_at_cursor (cur_text_doc, (_("$str=addslashes($str);\n"))); }
void php_dl_csv(void){  doc_insert_at_cursor (cur_text_doc, (_("header(\"Cache-control: private\");\n\n$filename=\"fichier_csv_name.csv\";\n\nif (strstr($_SERVER\[\"HTTP_USER_AGENT\"], \"MSIE\"))\n{header(\"Content-type: text/tab-separated-values; charset=iso-8859-1\");\nheader(\"Content-Disposition: filename=$filename\" );\n}\nelse{\nheader(\"Content-type: text/octet-stream; charset=iso-8859-1\");\nheader(\"Content-Disposition: attachment; filename=$name\");\n}\n\n$csv=\"CSV1;CSV2;CSV3;\";\n\necho $csv;\n"))); }

//*********************** HTML HELP
void html_css (void){doc_insert_at_cursor (cur_text_doc, "<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\" />\n"); }
void html_reload (void){doc_insert_at_cursor (cur_text_doc, "<meta http-equiv=\"refresh\" content=\"30;\">\n"); }
void html_largeur (void){doc_insert_at_cursor (cur_text_doc, "width=\"\" "); }
void html_hauteur (void){doc_insert_at_cursor (cur_text_doc, "height=\"\" "); }
void html_rows (void){doc_insert_at_cursor (cur_text_doc, "rows=\"\" "); }
void html_cols (void){doc_insert_at_cursor (cur_text_doc, "cols=\"\" "); }
void html_style (void){doc_insert_at_cursor (cur_text_doc, "style=\"\" "); }
void html_form (void){doc_insert_at_cursor (cur_text_doc, (_("<form name=\"nom_formulaire\" method=\"post\" action=\"page.php\">\n\n\n<input type=\"submit\" value=\"Valider\">\n</form>"))); }
void html_input_text (void){doc_insert_at_cursor (cur_text_doc, "<input type=\"text\" name=\"name_text\" value=\"default\">\n"); }
void html_input_textarea (void){doc_insert_at_cursor (cur_text_doc, (_("<textarea name=\"name_textarea\">\n\n\tText displayed in the textarea tag a\n\n</textarea>\n"))); }
void html_input_select (void){doc_insert_at_cursor (cur_text_doc, (_("<select name=\"name_select\">\n<option selected value=\"value\">VALUE</option>\n<option value=\"value2\">VALUE2</option>\n</select>\n"))); }
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

//*********************** IPTABLES HELP
void iptables_drop_all (void){doc_insert_at_cursor (cur_text_doc, "iptables -P INPUT DROP\niptables -P OUTPUT DROP\niptables -P FORWARD DROP\n"); }
void iptables_accept_lo (void){doc_insert_at_cursor (cur_text_doc, "iptables -A INPUT -i eth0 -p icmp -j ACCEPT\niptables -A OUTPUT -o eth0 -p icmp -j ACCEPT\n"); }
void iptables_accept_port (void){doc_insert_at_cursor (cur_text_doc, "iptables -A INPUT -p tcp --source-port 1000 -j ACCEPT\niptables -A INPUT -p tcp --destination-port 1000 -j ACCEPT\niptables -A OUTPUT -p tcp --source-port 1000 -j ACCEPT\niptables -A OUTPUT -p tcp --destination-port 1000 -j ACCEPT\n"); }
void iptables_accept_port_ip (void){doc_insert_at_cursor (cur_text_doc, "iptables -A INPUT --src 192.192.192.1 -p tcp --destination-port 1000 -j ACCEPT\niptables -A OUTPUT --dst 192.192.192.1 -p tcp --source-port 1000 -j ACCEPT\n"); }
void iptables_log(void){doc_insert_at_cursor (cur_text_doc, "iptables -N log_drop\niptables -A log_drop -j LOG --log-prefix '[IpTables DROP]'\niptables -A log_drop -j DROP\n\niptables -A FORWARD -j log_drop\niptables -A INPUT -j log_drop\niptables -A OUTPUT -j log_drop\n"); }
void iptables_clean (void){doc_insert_at_cursor (cur_text_doc, "iptables -F\niptables -t nat -F\niptables -t mangle -F\niptables -X\n"); }

//*********************** HTACCESS HELP
void htaccess_charset_utf8 (void){doc_insert_at_cursor (cur_text_doc, "AddDefaultCharset utf-8\n"); }
void htaccess_deny_all (void){doc_insert_at_cursor (cur_text_doc, "Deny from all\n"); }
void htaccess_accept_ip (void){doc_insert_at_cursor (cur_text_doc, "Allow from 127.0.0.1\n"); }
void htaccess_redirection_erreur (void){doc_insert_at_cursor (cur_text_doc, "ErrorDocument 403 http://griffon.lasotel.fr\nErrorDocument 404 http://griffon.lasotel.fr\n"); }
void htaccess_urlrw_on (void){doc_insert_at_cursor (cur_text_doc, "RewriteEngine on\n"); }
void htaccess_urlrw (void){doc_insert_at_cursor (cur_text_doc, (_("RewriteRule ^source.html$  destination.php?id=1  [L]\n"))); }
void htaccess_urlrw_exp (void){doc_insert_at_cursor (cur_text_doc, (_("RewriteRule ^source/([a-zA-Z0-9_]+)-([0-9]+).html$  destination.php?option1=$1&option2=$2  [L]\n"))); }

//*********************** CSS HELP
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

//*********************** JAVASCRIPT HELP
void javascript_html (void){doc_insert_at_cursor (cur_text_doc, "<script type=\"text/javascript\">\n\n\n</script>\n"); }
void javascript_fonction (void){doc_insert_at_cursor (cur_text_doc, (_("function name_function()\n{\n\n\n}\n\n"))); }
void javascript_value_par_id (void){doc_insert_at_cursor (cur_text_doc, (_("var Value=document.getElementById('VOTRE_ID').value;\n"))); }
void javascript_focus_id (void){doc_insert_at_cursor (cur_text_doc, (_("document.getElementById('YOUR_ID').focus();\n"))); }
void javascript_value_select (void){doc_insert_at_cursor (cur_text_doc, (_("var selectValue = document.getElementById('YOUR_ID').options[document.getElementById('YOUR_ID').selectedIndex].value;\n"))); }
void javascript_id_load_file (void){doc_insert_at_cursor (cur_text_doc, (_("$('#YOUR_ID').load('FILE_PHP_HTML');\n"))); }
void javascript_html_change (void){doc_insert_at_cursor (cur_text_doc, (_("document.getElementById('YOUR_ID').innerHTML =\"YOUR TEXT\";\n"))); }
void javascript_id_change (void){doc_insert_at_cursor (cur_text_doc, (_("document.getElementById('YOUR_ID').value = \"YOUR_TEXTE\";\n"))); }
void javascript_id_change_add (void){doc_insert_at_cursor (cur_text_doc, (_("document.getElementById('YOUR_ID').value += \"YOUR_TEXT\";\n"))); }
void javascript_onblur (void){doc_insert_at_cursor (cur_text_doc, "onBlur=\"\" "); }
void javascript_onchange (void){doc_insert_at_cursor (cur_text_doc, "onChange=\"\" "); }
void javascript_onclick (void){doc_insert_at_cursor (cur_text_doc, "onClick=\"\" "); }
void javascript_onfocus (void){doc_insert_at_cursor (cur_text_doc, "onFocus=\"\" "); }
void javascript_onload (void){doc_insert_at_cursor (cur_text_doc, "onLoad=\"\" "); }
void javascript_onmouseover (void){doc_insert_at_cursor (cur_text_doc, "onMouseOver=\"\" "); }
void javascript_onselect (void){doc_insert_at_cursor (cur_text_doc, "onSelect=\"\" "); }
void javascript_onsubmit (void){doc_insert_at_cursor (cur_text_doc, "onSubmit=\"\" "); }
void javascript_split (void){doc_insert_at_cursor (cur_text_doc, (_("//split YOUR_STRING in array \nvar reg=new RegExp(\"[;]+\", \"g\");\nvar array=YOUR_STRING.split(reg);"))); }
void javascript_file_contenu (void){doc_insert_at_cursor (cur_text_doc, (_("var Fichier = function Fichier(fichier)\n{\n\tif(window.XMLHttpRequest) obj = new XMLHttpRequest(); //Pour Firefox, Opera,...\n\n\telse if(window.ActiveXObject) obj = new ActiveXObject(\"Microsoft.XMLHTTP\"); //Pour Internet Explorer\n \n\telse return(false);\n\n\tif (obj.overrideMimeType) obj.overrideMimeType(\"text/xml\"); //Évite un bug de Safari\n\tobj.open(\"GET\", fichier, false);\n\tobj.send(null);\n\n\tif(obj.readyState == 4) return(obj.responseText);\n\telse return(false);\n}\n\nvar Contenu = Fichier('page.php');\n\n"))); }

void javascript_urlencode (void){doc_insert_at_cursor (cur_text_doc, "Url = \"http://example.com/index.html?url=\" + encodeURIComponent(myUrl);\n"); }

//*********************** BOOKMARK
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

	GtkTextIter itstart, itend;
	gint line=gtk_text_iter_get_line(&start);

	gtk_text_buffer_get_iter_at_line (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &itstart, line);
	gtk_text_buffer_get_iter_at_line (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), &itend, line + 1);
	gtk_text_buffer_apply_tag_by_name (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), "search", &start, &end);  
	gtk_text_buffer_apply_tag_by_name (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), txt, &itstart, &start);  
	gtk_text_buffer_apply_tag_by_name (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), txt, &end, &itend); 

	GdkPixbuf *pixbuf_mark;

	pixbuf_mark = gdk_pixbuf_new_from_file ("/usr/local/share/griffon/images/griffon_note.png", NULL);

	GtkSourceMarkAttributes *attribu=gtk_source_mark_attributes_new();
	gtk_source_mark_attributes_set_pixbuf(attribu,pixbuf_mark );
	gtk_source_view_set_mark_attributes(GTK_SOURCE_VIEW(cur_text_doc->text_view),"icon_note",attribu,1);
	gtk_source_buffer_create_source_mark(GTK_SOURCE_BUFFER(cur_text_doc->text_buffer),txt,"icon_note",&itstart);

	add_to_list_book("",txt);
	statusbar_msg (_("BookMark [OK]"));
	}else{log_to_memo (_("You must select a word or text to use a BookMark"), NULL, LM_ERROR);statusbar_msg (_("BookMark [ERROR]"));}
}

//*********************** MONTAGE SFTP
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
		log_to_memo (_("You must install the curlftpfs to use the mounting  SFTP"), NULL, LM_ERROR);
		statusbar_msg (_("Mount [ERROR]"));
		}
		else
		{
		fclose(fichier); 
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
		log_to_memo (_("%s mount SFTP in MOUNT/"), tampon_sftp, LM_NORMAL);
		statusbar_msg (_("Mount [OK]"));
		griffon_notify(_("Mount SFTP"));
		icon_affiche_net ();
		sftp_reload();

		}
}

//*********************** MONTAGE FTP
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
		log_to_memo (_("You must install the curlftpfs to use the mounting FTP"), NULL, LM_ERROR);
		statusbar_msg (_("Mount [ERROR]"));
	}
	else
	{
		fclose(fichier); 
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
		log_to_memo (_("%s mount FTP in MOUNT/"), tampon_sftp, LM_NORMAL);
		statusbar_msg (_("Mount [OK]"));
		griffon_notify(_("Mount FTP"));
		icon_affiche_net ();
		ftp_reload();

	}
}

//*********************** UMOUNT SFTP/FTP
void umount_sftp (void)
{

	if(strlen(liste_mount)>4)
	{
		int systemRet =system (liste_mount);
		if(systemRet == -1){return;}

		liste_mount[0]='\0';

		icon_log_logmemo();
		log_to_memo (_("%s umount FTP/SFTP"), tampon_sftp, LM_NORMAL);
		icon_affiche_net ();
		tampon_sftp=NULL;
		statusbar_msg (_("Umount [OK]"));
		griffon_notify(_("Umount SFTP/FTP"));
	}
	else
	{
		icon_stop_logmemo();

		log_to_memo (_("No mounted server FTP / SFTP"), NULL, LM_ERROR);
		statusbar_msg (_("Umount [Error]"));
	}

		gtk_tree_selection_unselect_all(GTK_TREE_SELECTION(selection_sftp)); 
		gtk_tree_selection_unselect_all(GTK_TREE_SELECTION(selection_ftp)); 
}

//*********************** CONFIGURATION WINDOW SFTP
GtkWidget* w_sftp_mount (void)
{
	GtkWidget *window1,*frame1,*vbox1,*hbox2,*label2,*hbox1,*button1,*button2,*label1,*hbox3,*label3,*hbox4,*label4;  

	icon_affiche_stop();
	
	window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window1), _((_("Mount a SFTP server"))));
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

	label2 = gtk_label_new (_("Server name or ip : \t"));
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

	label3 = gtk_label_new (_("User : \t\t\t\t\t"));
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

	label4 = gtk_label_new (_("Path on the server : \t"));
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

	label1 = gtk_label_new (_("Mount SFTP"));
	gtk_widget_show (GTK_WIDGET(label1));
	gtk_frame_set_label_widget (GTK_FRAME (frame1), label1);
	gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

	g_signal_connect_swapped ((gpointer) button1, "clicked",G_CALLBACK (icon_affiche_ok),NULL);	
	g_signal_connect_swapped ((gpointer) button1, "clicked",G_CALLBACK (gtk_widget_destroy),window1);
	g_signal_connect_swapped ((gpointer) button2, "clicked",G_CALLBACK (icon_affiche_ok),NULL);	
	g_signal_connect_swapped ((gpointer) button2, "clicked",G_CALLBACK (mount_sftp),NULL);	
	g_signal_connect_swapped ((gpointer) button2, "clicked",G_CALLBACK (gtk_widget_destroy),window1);
	g_signal_connect_swapped ((gpointer) button1, "clicked",G_CALLBACK (gtk_widget_destroy),window1);

	return window1;
}

//*********************** CONFIGURATION WINDOW FTP
GtkWidget* w_ftp_mount (void)
{
	GtkWidget *window1,*frame1,*vbox1,*hbox2,*label2,*hbox1,*button1,*button2,*label1,*hbox3,*label3,*hbox4,*label4;  

	icon_affiche_stop();
	
	window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window1), _((_("Mount a FTP server"))));
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

	label2 = gtk_label_new (_("Server name or ip :\t"));
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

	label3 = gtk_label_new (_("User : \t\t\t\t\t"));
	gtk_widget_show (GTK_WIDGET(label3));
	gtk_box_pack_start (GTK_BOX (hbox3), label3, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);

	entry_utilisateur_ftp = gtk_entry_new ();
	gtk_widget_show (GTK_WIDGET(entry_utilisateur_ftp));
	gtk_box_pack_start (GTK_BOX (hbox3), entry_utilisateur_ftp, FALSE, FALSE, 0);

	hbox4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_show (GTK_WIDGET(hbox4));
	gtk_box_pack_start (GTK_BOX (vbox1), hbox4, TRUE, TRUE, 0);

	label4 = gtk_label_new (_("Password :\t\t\t"));
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

	label1 = gtk_label_new (_("Mount FTP"));
	gtk_widget_show (GTK_WIDGET(label1));
	gtk_frame_set_label_widget (GTK_FRAME (frame1), label1);
	gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

	g_signal_connect_swapped ((gpointer) button1, "clicked",G_CALLBACK (icon_affiche_ok),NULL);	
	g_signal_connect_swapped ((gpointer) button1, "clicked",G_CALLBACK (gtk_widget_destroy), window1);
	g_signal_connect_swapped ((gpointer) button2, "clicked",G_CALLBACK (icon_affiche_ok), NULL);	
	g_signal_connect_swapped ((gpointer) button2, "clicked",G_CALLBACK (mount_ftp),NULL);	
	g_signal_connect_swapped ((gpointer) button2, "clicked",G_CALLBACK (gtk_widget_destroy), window1);
	g_signal_connect_swapped ((gpointer) button1, "clicked",G_CALLBACK (gtk_widget_destroy), window1);

	return window1;
}

//*********************** EXECUTER UN SCRIPT
void on_execut_script (void)
{
	if (! get_page_text()) return;

	if(cur_text_doc->file_name==NULL)
	{
		icon_stop_logmemo();

		log_to_memo (_("The script is not saved or is not selected"), NULL, LM_ERROR);
		statusbar_msg (_("Script [Error]"));
	}
	else
	{
		icon_log_logmemo();
		log_to_memo (_("Execution of script %s [OK]"), cur_text_doc->file_name, LM_NORMAL);
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

//*********************** MISE EN VALEUR DU TEXTE APRES LE CURSOR
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

//*********************** OUVERTURE DIMAGE AVEC GIMP DEPUIS LE MINIWEB
void open_gimp (void)
{

	FILE *fichier = NULL;
	fichier = fopen("/usr/bin/gimp",  "r");
	
	if (fichier == NULL)
	{
		icon_stop_logmemo();
		log_to_memo (_("You must install the gimp to use the edition image"), NULL, LM_ERROR);
		statusbar_msg (_("You must install the gimp to use the edition image"));
	}
	else
	{
		fclose(fichier); 
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

//*********************** OPEN LES FICHIERS EN INCLUDE
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

//*********************** KEY RELACHE POUR RECHERCHE
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

//*********************** AJOUT DE TODO DANS UN FICHIER + LOG
void add_todo_com(void)
{
	time_t date;
	date = time(NULL);

	if (! get_page_text()) return;

	char *extension;
	if(strrchr(cur_text_doc->file_name,'.'))
	{
		extension = strrchr(cur_text_doc->file_name,'.');
		if (strcmp(".pl", extension) == 0){doc_insert_at_cursor (cur_text_doc, "\n\n#\n# TODO : \n#\n");}
		if (strcmp(".sh", extension) == 0){doc_insert_at_cursor (cur_text_doc, "\n\n#\n# TODO : \n#\n");}
		if (strcmp(".c", extension) == 0 || strcmp(".h", extension) == 0){doc_insert_at_cursor (cur_text_doc, "\n\n/*\n* TODO : \n*/\n");}
		if (strcmp(".htm", extension) == 0 || strcmp(".html", extension) == 0){doc_insert_at_cursor (cur_text_doc, "\n<!-- TODO -->\n");}
		if (strcmp(".php", extension) == 0){doc_insert_at_cursor (cur_text_doc, "\n\n/*\n* TODO : \n*/\n");}
		if (strcmp(".txt", extension) == 0){doc_insert_at_cursor (cur_text_doc, "\n\n/*\n* TODO : \n*/\n");}
		}else{doc_insert_at_cursor (cur_text_doc, "\n\n/*\n* TODO : \n*/\n");}

		gchar *msg;
		gint row;
		GtkTextIter iter;
		gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(cur_text_doc->text_buffer),&iter, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(cur_text_doc->text_buffer)));
		row = gtk_text_iter_get_line(&iter);
		msg = g_strdup_printf("LINE %d", row+1);

		save_string_to_file_add(confile.tea_todo,"[TODO] : ");
		save_string_to_file_add(confile.tea_todo, cur_text_doc->file_name);
		save_string_to_file_add(confile.tea_todo," ");
		save_string_to_file_add(confile.tea_todo,msg);
		save_string_to_file_add(confile.tea_todo," ");
		save_string_to_file_add(confile.tea_todo,ctime(&date));
		save_string_to_file_add(confile.tea_todo,"\n");
		on_mni_file_todolist ();
}

void add_todo_bug(void)
{
	time_t date;
	date = time(NULL);

	if (! get_page_text()) return;

	char *extension;

	if(strrchr(cur_text_doc->file_name,'.'))
	{
		extension = strrchr(cur_text_doc->file_name,'.');
		if (strcmp(".pl", extension) == 0){doc_insert_at_cursor (cur_text_doc, "\n\n#\n# BUG : \n#\n");}
		if (strcmp(".sh", extension) == 0){doc_insert_at_cursor (cur_text_doc, "\n\n#\n# BUG : \n#\n");}
		if (strcmp(".c", extension) == 0 || strcmp(".h", extension) == 0){doc_insert_at_cursor (cur_text_doc, "\n\n/*\n* BUG : \n*/\n");}
		if (strcmp(".htm", extension) == 0 || strcmp(".html", extension) == 0){doc_insert_at_cursor (cur_text_doc, "\n<!-- BUG -->\n");}
		if (strcmp(".php", extension) == 0){doc_insert_at_cursor (cur_text_doc, "\n\n/*\n* BUG : \n*/\n");}
		if (strcmp(".txt", extension) == 0){doc_insert_at_cursor (cur_text_doc, "\n\n/*\n* BUG : \n*/\n");}
		}else{doc_insert_at_cursor (cur_text_doc, "\n\n/*\n* BUG : \n*/\n");}

		gchar *msg;
		gint row;
		GtkTextIter iter;
		gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(cur_text_doc->text_buffer),&iter, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(cur_text_doc->text_buffer)));
		row = gtk_text_iter_get_line(&iter);
		msg = g_strdup_printf("LINE %d", row+1);

		save_string_to_file_add(confile.tea_todo,"[BUG] : ");
		save_string_to_file_add(confile.tea_todo, cur_text_doc->file_name);
		save_string_to_file_add(confile.tea_todo," ");
		save_string_to_file_add(confile.tea_todo,msg);
		save_string_to_file_add(confile.tea_todo," ");
		save_string_to_file_add(confile.tea_todo,ctime(&date));
		save_string_to_file_add(confile.tea_todo,"\n");
		on_mni_file_todolist ();
}

void add_todo_fixme(void)
{
	time_t date;
	date = time(NULL);

	if (! get_page_text()) return;

	char *extension;

	if(strrchr(cur_text_doc->file_name,'.'))
	{
		extension = strrchr(cur_text_doc->file_name,'.');
		if (strcmp(".pl", extension) == 0){doc_insert_at_cursor (cur_text_doc, "\n\n#\n# FIXME : \n#\n");}
		if (strcmp(".sh", extension) == 0){doc_insert_at_cursor (cur_text_doc, "\n\n#\n# FIXME : \n#\n");}
		if (strcmp(".c", extension) == 0 || strcmp(".h", extension) == 0){doc_insert_at_cursor (cur_text_doc, "\n\n/*\n* FIXME : \n*/\n");}
		if (strcmp(".htm", extension) == 0 || strcmp(".html", extension) == 0){doc_insert_at_cursor (cur_text_doc, "\n<!-- FIXME -->\n");}
		if (strcmp(".php", extension) == 0){doc_insert_at_cursor (cur_text_doc, "\n\n/*\n* FIXME : \n*/\n");}
		if (strcmp(".txt", extension) == 0){doc_insert_at_cursor (cur_text_doc, "\n\n/*\n* FIXME : \n*/\n");}
		}else{doc_insert_at_cursor (cur_text_doc, "\n\n/*\n* FIXME : \n*/\n");}

		gchar *msg;
		gint row;
		GtkTextIter iter;
		gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(cur_text_doc->text_buffer),&iter, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(cur_text_doc->text_buffer)));
		row = gtk_text_iter_get_line(&iter);
		msg = g_strdup_printf("LINE %d", row+1);

		save_string_to_file_add(confile.tea_todo,"[FIXME] : ");
		save_string_to_file_add(confile.tea_todo, cur_text_doc->file_name);
		save_string_to_file_add(confile.tea_todo," ");
		save_string_to_file_add(confile.tea_todo,msg);
		save_string_to_file_add(confile.tea_todo," ");
		save_string_to_file_add(confile.tea_todo,ctime(&date));
		save_string_to_file_add(confile.tea_todo,"\n");
		on_mni_file_todolist ();
}

//********************* INSERT DIALOG FILE PATH
void insert_dialog_path()
{
	if (! get_page_text()) return;
	GtkWidget *dialog;
	dialog = gtk_file_chooser_dialog_new ("Open File",
				      (GtkWindow *)tea_main_window,
				      GTK_FILE_CHOOSER_ACTION_OPEN,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				      NULL);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		doc_insert_at_cursor (cur_text_doc, filename);
		g_free (filename);
	}
	gtk_widget_destroy (dialog);
}

//********************* INSERT DIALOG FILE PATH DIR
void insert_dialog_path_dir()
{
	if (! get_page_text()) return;
	GtkWidget *dialog;
	dialog = gtk_file_chooser_dialog_new ("Open File",
				      (GtkWindow *)tea_main_window,
				       GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				      NULL);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		doc_insert_at_cursor (cur_text_doc, filename);
		g_free (filename);
	}
	gtk_widget_destroy (dialog);
}

//*********************** EXECUTER UN DIFF
void on_execut_diff (void)
{
	if (! get_page_text()) return;

	gchar *fname = g_path_get_basename (cur_text_doc->file_name);
	gchar *changelog_file = g_strconcat (confile.changelog,"/",fname, NULL); 

	gchar *datetime = get_time (confile.date_time);
	save_string_to_file_add(changelog_file,datetime);
	save_string_to_file_add(changelog_file,"______________________________________\n");

	if (! g_file_test (changelog_file, G_FILE_TEST_EXISTS))
		create_empty_file (changelog_file, "");

	FILE *fichier = NULL;
	fichier = fopen("/usr/bin/diff",  "r");
	
	if (fichier == NULL){return;}
	fclose(fichier); 

	if(cur_text_doc->file_name==NULL)
	{
		icon_stop_logmemo();

		log_to_memo (_("The script is not saved or is not selected"), NULL, LM_ERROR);
		statusbar_msg (_("Diff [Error]"));
	}
	else
	{
		icon_log_logmemo();
		log_to_memo (_("Diff %s [OK]"), cur_text_doc->file_name, LM_NORMAL);

		gchar *standard_output = NULL;
		gchar *standard_error = NULL;
		GError *err = NULL;

		gchar *diff="diff ";
		gchar *tmp=" ";
		gchar *cmd = g_strconcat (diff,cur_text_doc->file_name,tmp,confile.file_tmp, NULL); 

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
			save_string_to_file_add(changelog_file,(gchar *)x);
			g_free (x);

			x = g_locale_to_utf8 (standard_error, -1, &bytes_read, &bytes_written, NULL);
			save_string_to_file_add(changelog_file,(gchar *)x);
			log_to_memo (x, NULL, LM_NORMAL);

			g_free (x);
			do_errors_hl (GTK_TEXT_VIEW(tv_logmemo));
		}

		g_free (cmd);
		g_free (standard_output);
		g_free (standard_error);
		g_free (changelog_file);
		g_free (fname);
	}
}
