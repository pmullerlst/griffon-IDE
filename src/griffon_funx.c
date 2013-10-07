
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

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguage.h>
//#include <gtkspell/gtkspell.h>

#include "griffon_defs.h"
#include "griffon_config.h"
#include "rox_strings.h"
#include "interface.h"
#include "griffon_text_document.h"
#include "griffon_funx.h"
#include "griffon_proj.h"
#include "griffon_gtk_utils.h"


/* from:
 * time.c
 * This file is part of gedit
 *
 * Copyright (C) 2002 Paolo Maggi

changed by roxton
*/

gchar* get_time (const gchar* format)
{
  	gchar *out = NULL;
	gchar *out_utf8 = NULL;
  	time_t clock;
  	struct tm *now;
  	size_t out_length = 0;
	gchar *locale_format;

	if (strlen (format) == 0)
		return g_strdup (" ");

	locale_format = g_locale_from_utf8 (format, -1, NULL, NULL, NULL);

	if (! locale_format)
		return g_strdup (" ");

  	clock = time (NULL);
  	now = localtime (&clock);

	do
	{
		out_length += 255;
		out = g_realloc (out, out_length);
	}
  	while (strftime (out, out_length, locale_format, now) == 0);

	g_free (locale_format);

	if (g_utf8_validate (out, -1, NULL))
		out_utf8 = out;
	else
	{
		out_utf8 = g_locale_to_utf8 (out, -1, NULL, NULL, NULL);
		g_free (out);

		if (! out_utf8)
			out_utf8 = g_strdup (" ");
	}

  	return out_utf8;
}


//from Anjuta ::
//    utilities.c
//  Copyright (C) 2000  Kh. Naba Kumar Singh
#define FILE_BUFFER_SIZE 65535
gboolean
copy_file (gchar * src, gchar * dest)
{
	FILE *input_fp, *output_fp;
	gchar buffer[FILE_BUFFER_SIZE];
	gint bytes_read, bytes_written;
	gboolean error;

	error = TRUE;

	input_fp = fopen (src, "rb");
	if (input_fp == NULL)
	{
		return FALSE;
	}

	output_fp = fopen (dest, "wb");
	if (output_fp == NULL)
	{
		fclose (input_fp);
		return TRUE;
	}

	for (;;)
	{
		bytes_read = fread (buffer, 1, FILE_BUFFER_SIZE, input_fp);
		if (bytes_read != FILE_BUFFER_SIZE && ferror (input_fp))
		{
			error = FALSE;
			break;
		}

		if (bytes_read)
		{
			bytes_written = fwrite (buffer, 1, bytes_read, output_fp);
			if (bytes_read != bytes_written)
			{
				error = FALSE;
				break;
			}
		}

		if (bytes_read != FILE_BUFFER_SIZE && feof (input_fp))
		{
			break;
		}
	}

	fclose (input_fp);
	fclose (output_fp);

	return error;
}
//


//from Bluefish:: bf_lib.c and gtk_easy

/**
 * window_destroy:
 * @windowname: a #GtkWidget pointer to the window
 * 
 * Remove grab and signals and then destroy window
 * 
 * Return value: void
 **/
void window_destroy(GtkWidget * windowname) {
	g_signal_handlers_destroy(G_OBJECT(windowname));
	gtk_grab_remove(windowname);
	gtk_widget_destroy(windowname);
}


/**
 * ending_slash:
 * @dirname: a #const gchar * with a diretory name
 *
 * makes sure the last character of the newly allocated
 * string it returns is a '/'
 *
 * Return value: a newly allocated gchar * dirname that does end on a '/'
 **/
gchar *ending_slash(const gchar *dirname) {
	if (!dirname) {
		return g_strdup("");
	}

	if (dirname[strlen(dirname)-1] == G_DIR_SEPARATOR) {
		return g_strdup(dirname);
	} else {
		return g_strconcat(dirname, G_DIR_SEPARATOR_S, NULL);
	}
}


/**
 * most_efficient_filename:
 * @filename: a gchar * with a possibly inefficient filename like /hello/../tmp/../myfile
 *
 * tries to eliminate any dir/../ combinations in filename
 * this function could do evern better, it should also remove /./ entries
 *
 * Return value: the same gchar * as passed to the function
 **/
gchar *most_efficient_filename(gchar *filename) {
	gint i,j, len;

	len = strlen(filename);
	for (i=0; i < len-4; i++) {
		if (strncmp(&filename[i], "/../", 4) == 0) {
			for (j=1; j < i; j++) {
				if ((filename[i - j] == G_DIR_SEPARATOR) || (i==j)) {
					memmove(&filename[i-j], &filename[i+3], len-i);
					j=i;
					i--;
				}
			}
		}
	}
	return filename;
}

/**
 * create_relative_link_to:
 * @current_filepath: a #gchar * with the current filename
 * @link_to_filepath: a #gchar * with a file to link to
 *
 * creates a newly allocated relative link from current_filepath
 * to link_to_filepath
 *
 * if current_filepath == NULL it returns the most efficient filepath
 * for link_to_filepath
 *
 * if link_to_filepath == NULL it will return NULL
 *
 * Return value: a newly allocated gchar * with the relative link
 **/
gchar *create_relative_link_to(gchar * current_filepath, gchar * link_to_filepath)
{
	gchar *returnstring = NULL;
	gchar *eff_current_filepath, *eff_link_to_filepath;
	gint common_lenght, maxcommonlen;
	gint current_filename_length, link_to_filename_length, current_dirname_length, link_to_dirname_length;
	gint count, deeper_dirs;

	if (!current_filepath){
		returnstring = most_efficient_filename(g_strdup(link_to_filepath));
		return returnstring;
	}
	if (!link_to_filepath) {
		return NULL;
	}
	eff_current_filepath = most_efficient_filename(g_strdup(current_filepath));
	eff_link_to_filepath = most_efficient_filename(g_strdup(link_to_filepath));
	/* get the size of the directory of the current_filename */
	current_filename_length = strlen(strrchr(eff_current_filepath, G_DIR_SEPARATOR))-1;
	link_to_filename_length = strlen(strrchr(eff_link_to_filepath, G_DIR_SEPARATOR))-1;
	current_dirname_length = strlen(eff_current_filepath) - current_filename_length;
	link_to_dirname_length = strlen(eff_link_to_filepath) - link_to_filename_length;

	if (current_dirname_length < link_to_dirname_length) {
		maxcommonlen = current_dirname_length;
	} else {
		maxcommonlen = link_to_dirname_length;
	}

	/* first lets get the common basedir for both dir+file  by comparing the
	   common path in the directories */
	common_lenght = 0;
	while ((strncmp(eff_current_filepath, eff_link_to_filepath, common_lenght + 1)) == 0) {
		common_lenght++;
		if (common_lenght >= maxcommonlen) {
			common_lenght = maxcommonlen;
			break;
		}
	}
	/* this is the common length, but we need the common directories */
	if (eff_current_filepath[common_lenght] != G_DIR_SEPARATOR) {
		gchar *ltmp = &eff_current_filepath[common_lenght];
		while ((*ltmp != G_DIR_SEPARATOR) && (common_lenght > 0)) {
			common_lenght--;
			ltmp--;
		}
	}

	/* now we need to count how much deeper (in directories) the current_filename
	   is compared to the link_to_file, that is the amount of ../ we need to add */
	deeper_dirs = 0;
	for (count = common_lenght+1; count <= current_dirname_length; count++) {
		if (eff_current_filepath[count] == G_DIR_SEPARATOR) {
			deeper_dirs++;
		}
	}

	/* now we know everything we need to know we can create the relative link */
	returnstring = g_malloc0((strlen(link_to_filepath) - common_lenght + 3 * deeper_dirs + 1) * sizeof(gchar *));
	count = deeper_dirs;
	while (count) {
		strcat(returnstring, "../");
		count--;
	}
	strcat(returnstring, &eff_link_to_filepath[common_lenght+1]);
	g_free(eff_current_filepath);
	g_free(eff_link_to_filepath);
	return returnstring;
}

/**
 * create_full_path:
 * @filename: a gchar * with the (relative or not) filename
 * @basedir: a gchar * with a basedir or NULL for current dir
 *
 * if filename is already absolute, it returns it
 * else it will use basedir if available, else the current dir
 * to add to the filename to form the full path
 *
 * it does use most_efficient_filename() to remote unwanted dir/../ entries
 *
 * Return value: a newly allocated gchar * with the full path
 **/


gchar *create_full_path(gchar * filename, gchar *basedir) {
	gchar *absolute_filename;
	gchar *tmpcdir;


	if (g_path_is_absolute(filename)) {
		absolute_filename = g_strdup(filename);
	} else {
		if (basedir) {
			tmpcdir = ending_slash(basedir);
		} else {
			gchar *curdir = g_get_current_dir();
			tmpcdir = ending_slash(curdir);
			g_free(curdir);
		}
		absolute_filename = g_strconcat(tmpcdir, filename, NULL);
		g_free(tmpcdir);
	}
	absolute_filename = most_efficient_filename(absolute_filename);
	return absolute_filename;
}
//



gint noname_name_counter = -1;
gint noname_kwas_counter = -1;

gchar* get_noname_name (void) 
{
  ++noname_name_counter;
  if (noname_name_counter >= G_MAXINT)
     noname_name_counter = 0;
  return g_strdup_printf ("noname_%d", noname_name_counter);
}


gchar* get_kwas_name (void) 
{
  ++noname_kwas_counter;
  if (noname_name_counter >= G_MAXINT)
     noname_name_counter = 0;
  return g_strdup_printf ("kwas_%d", noname_kwas_counter);
}


int get_value (int total, int perc)
{
  return (int) (total / 100) * perc;
}


int get_percent (int total, int value)
{
  return (int) (value / total) * 100;
}


GtkWidget* new_menu_item_with_udata (const gchar *label, GtkWidget *parent, gpointer f, gpointer data)
{
  GtkWidget *item;

  item = gtk_menu_item_new_with_label (label);
  gtk_widget_show (item);
  gtk_container_add (GTK_CONTAINER (parent), item);
  gtk_widget_set_name (item, label);

  if (f)
     g_signal_connect ((gpointer) item, "activate", G_CALLBACK (f), data);

  return item;
}


GtkWidget* new_menu_item (const gchar *label, GtkWidget *parent, gpointer f)
{
  GtkWidget *item;

  item = gtk_menu_item_new_with_label (label);
  gtk_widget_show (item);
  gtk_container_add (GTK_CONTAINER (parent), item);
  gtk_widget_set_name (item, label);

  if (f)
     g_signal_connect ((gpointer) item, "activate", G_CALLBACK (f), NULL);

  return item;
}


GtkWidget* new_menu_sep (GtkWidget *parent)
{
  GtkWidget *mni_sep = gtk_menu_item_new ();
  gtk_widget_show (mni_sep);
  gtk_container_add (GTK_CONTAINER (parent), mni_sep);
  gtk_widget_set_name (mni_sep, "-x-");
  gtk_widget_set_sensitive (mni_sep, FALSE);
  return mni_sep;
}


GtkWidget* new_menu_tof (GtkWidget *parent)
{
  GtkWidget *mni_tof = gtk_tearoff_menu_item_new();
  gtk_widget_set_name (mni_tof, "-x-");
  gtk_widget_show (mni_tof);
  gtk_container_add (GTK_CONTAINER (parent), mni_tof);
}


GtkWidget* new_menu_submenu (GtkWidget *parent)
{
  GtkWidget *mni_menu = gtk_menu_new ();
  gtk_widget_set_name (mni_menu, "-x-");
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (parent), mni_menu);
  return mni_menu;
}


void goto_local_label (const gchar *l)
{
  gchar *t = g_utf8_find_next_char (l, NULL);
  gchar *s = g_strconcat ("<a name=\"", t, NULL);

  GtkTextIter iter;
  GtkTextIter match_start;

  GtkTextMark *m = gtk_text_buffer_get_insert (cur_text_doc->text_buffer); 
  gtk_text_buffer_get_iter_at_mark (cur_text_doc->text_buffer, &iter, m);

  if (gtk_text_iter_forward_search  (&iter, s, GTK_TEXT_SEARCH_TEXT_ONLY, &match_start, NULL, NULL))
     {
      gtk_text_buffer_place_cursor (cur_text_doc->text_buffer, &match_start );
      gtk_text_view_scroll_to_iter (cur_text_doc->text_view, &match_start, 0.0, TRUE, 0.0, 0.0 );
      gtk_text_view_place_cursor_onscreen (GTK_TEXT_VIEW (cur_text_doc->text_view));
     }

  g_free (s);
}


void handle_file (gchar *filename, gint mode)
{
  if (! filename)
     return;
 
  gchar *cmd = NULL;
  
  gint i = get_n_page_by_filename (filename);

  if (i != -1)
     {
      gtk_notebook_set_current_page (notebook1, i);
      return;
     } 

  if (! g_file_test (filename, G_FILE_TEST_EXISTS))
     return;
 
  if (is_image (filename))
     {
      if (mode != 0)
         {
          cmd = g_strdup_printf (confile.ext_pic_editor, filename);
          system (cmd);
          g_free (cmd);
          return;
         }

      if (confile.use_ext_image_viewer)
         {
          cmd = g_strdup_printf (confile.ext_pic_editor, filename);
          system (cmd);
          g_free(cmd);
          return;        
         }
      else  
          create_wnd_imgviewer (filename);

      return;
     }
  
  if (! get_page_text() )
     cur_settings.selected_enc = ch_str (cur_settings.selected_enc, confile.default_charset);
  else
      cur_settings.selected_enc = ch_str (cur_settings.selected_enc, cur_text_doc->encoding);

  open_file_std (filename);
}


void handle_file_enc (gchar *filename, gchar *enc)
{
  if (! filename)
     return;
 
  gchar *cmd = NULL;
  
  gint i = get_n_page_by_filename (filename);

  if (i != -1)
     {
      gtk_notebook_set_current_page (notebook1, i);
      return;
     } 

  if (! g_file_test (filename, G_FILE_TEST_EXISTS))
     return;
 
  
  cur_settings.selected_enc = ch_str (cur_settings.selected_enc, enc);

  open_file_std (filename);
}

      

void kwas_handle_file (gchar *filename, int mode)
{
  if (! filename)
     return;
 
  gchar *cmd = NULL;
  
  gint i = get_n_page_by_filename (filename);

  if (i != -1)
     {
      gtk_notebook_set_current_page (notebook1, i);
      return;
     } 

  if (! g_file_test (filename, G_FILE_TEST_EXISTS))
     return;
 
  if (is_image (filename))
     {
      if (mode != 0)
         {
          cmd = g_strdup_printf (confile.ext_pic_editor, filename);
          system (cmd);
          g_free (cmd);
          return;
         }

      if (confile.use_ext_image_viewer)
         {
          cmd = g_strdup_printf (confile.ext_pic_editor, filename);
          system (cmd);
          g_free(cmd);
          return;        
         }
      else  
          create_wnd_imgviewer (filename);

      return;
     }

  open_file_std (filename);
}


t_image_info* get_image_info (gchar *filename)
{
  if (! filename)
      return NULL;

  t_image_info *result = g_malloc (sizeof (t_image_info));
  GdkPixbuf *buf = gdk_pixbuf_new_from_file (filename, NULL);
  
  if (buf)
     {
      result->height = gdk_pixbuf_get_height (buf);
      result->width = gdk_pixbuf_get_width (buf);
     } 
  else
      g_free (result);

  g_free (buf);
  return result;
}


gboolean is_image (const gchar *filename)
{
  if (! filename)
      return FALSE;

  if (check_ext (filename, ".png") ||
      check_ext (filename, ".gif") ||
      check_ext (filename, ".jpg") ||
      check_ext (filename, ".jpeg")||
      check_ext (filename, ".wbmp")||
      check_ext (filename, ".bmp")||
      check_ext (filename, ".mng") 
     )
     return TRUE;
 
  return FALSE;
}


gboolean is_po (const gchar *f)
{
  if (check_ext (f, ".pot") ||
      check_ext (f, ".po")) 
     return TRUE;
  else
      return FALSE;
}

gboolean is_php (const gchar *f)
{
  if (check_ext (f, ".php")) 
     return TRUE;
  else
      return FALSE;
}

void create_empty_file (gchar *filename, gchar *first_line)
{
  FILE *out = fopen (filename, "w");
  
  if (! out) 
     return FALSE;                     

  if (first_line)
     fprintf (out, "%s", first_line);

  fclose (out);                        
}


gint get_file_size (const gchar *filename)
{
  struct stat s;
  stat (filename, &s);
  return s.st_size;
}


gboolean check_ext (gchar *filename, gchar *ext)
{
  if (! filename || ! ext)
     return FALSE;

  gboolean r = FALSE;
  gchar *f = g_utf8_strdown (filename, -1);
  r = g_str_has_suffix (f, ext);
  g_free (f);
  return r; 
}


gboolean is_markup (const gchar *f)
{
  if ((check_ext (f, ".html")) ||
      (check_ext (f, ".htm")) ||
      (check_ext (f, ".shtml")) ||
      (check_ext (f, ".xml")) ||
      (check_ext (f, ".xhtml")) 
      )
      return TRUE;

  return FALSE;
}


gboolean is_c (const gchar *f)
{

  if (check_ext (f, ".c") ||
      check_ext (f, ".h") ||
      check_ext (f, ".cpp") ||
      check_ext (f, ".cxx") ||
      check_ext (f, ".cc") ||
      check_ext (f, ".cp") ||
      check_ext (f, ".c++")
      )
      return TRUE;

  return FALSE;
}


gboolean is_pascal (const gchar *f)
{
  if (check_ext (f, ".pas") ||
      check_ext (f, ".dpr") ||
      check_ext (f, ".pp") 
      )
      return TRUE;

  return FALSE;
}


gchar* get_hl_name (gchar *file_name)
{
  gchar *b;

  if (check_ext (file_name, ".html") || check_ext (file_name, ".htm"))
     return g_strdup (HL_MARKUP); 

  if (check_ext (file_name, ".xml"))
     return g_strdup (HL_XML); 
       
  if (check_ext (file_name, ".js"))
     return g_strdup (HL_JAVASCRIPT); 

  if (check_ext (file_name, ".c") || check_ext (file_name, ".h"))
     return g_strdup (HL_C); 

  if (check_ext (file_name, ".jar") || check_ext (file_name, ".jav") || check_ext (file_name, ".java") || check_ext (file_name, ".class"))
     return g_strdup (HL_JAVA); 

  if (check_ext (file_name, ".py")) 
     return g_strdup (HL_PYTHON); 

  if (check_ext (file_name, ".rb")) 
     return g_strdup (HL_RUBY);  

  if (check_ext (file_name, ".php")) 
     return g_strdup (HL_PHP); 

  if (check_ext (file_name, ".cpp")) 
     return g_strdup (HL_CPP); 

  if (check_ext (file_name, ".css")) 
     return g_strdup (HL_CSS); 

  if (check_ext (file_name, ".pl")) 
     return g_strdup (HL_PERL); 

  if (check_ext (file_name, ".sh")) 
     return g_strdup (HL_BASH); 

  if (is_po (file_name))
     return g_strdup (HL_PO); 

  if (confile.do_det_scripts_by_content)
     {
      b = str_file_read (file_name);
      if (b)
         { 
          if (strstr (b, "#!/bin/bash") || strstr (b, "#!/bin/sh"))
             {
              g_free (b);
              return g_strdup (HL_BASH); 
             }
             
          if (strstr (b, "#!/bin/perl") || strstr (b, "#!/usr/bin/perl"))
             {
              g_free (b);
              return g_strdup (HL_PERL); 
             }             
             
          g_free (b);
         }
     } 

  return g_strdup (HL_NONE);
}


gchar* get_l_filename (const gchar *filename)
{
  gsize bytes_read;
  gsize bytes_written;
  return g_filename_from_utf8 (filename, -1, &bytes_read, &bytes_written, NULL);  
}


gchar* get_8_filename (const gchar *filename)
{
  gsize bytes_read;
  gsize bytes_written;
  
  return g_filename_to_utf8 (filename, -1, &bytes_read, &bytes_written, NULL);  
}

/*
void make_stats (t_note_page *doc)
{
  gboolean selected = TRUE;
  gchar *text;
  gint words = 0;
  gint chars = 0;
  gint white_chars = 0;
  gint lines = 0;
  gint bytes = 0;
  GtkTextIter start;
  GtkTextIter end;
  gunichar ch;

  if (doc_is_sel (doc->text_buffer))
     gtk_text_buffer_get_selection_bounds (doc->text_buffer, &start, &end);
  else
      {
       gtk_text_buffer_get_start_iter (doc->text_buffer, &start); 
       gtk_text_buffer_get_end_iter (doc->text_buffer, &end); 
       selected = FALSE;
      }  

  lines = gtk_text_buffer_get_line_count (doc->text_buffer);
  text =  gtk_text_buffer_get_text (doc->text_buffer, &start, &end, FALSE);
  bytes = strlen (text);

  do 
    {
     ch = gtk_text_iter_get_char (&start);
     if (g_unichar_isspace (ch))
        white_chars++;
     if (gtk_text_iter_starts_word (&start))
        words++; 

     chars++;
    }
  while ( (gtk_text_iter_forward_char (&start)) && ( ! gtk_text_iter_equal (&start, &end)) ); 


  gchar *s_bytes = g_strdup_printf(_("bytes: %d\n"), bytes);
  gchar *s_lines = g_strdup_printf(_("lines: %d\n"), lines);
  gchar *s_words = g_strdup_printf(_("words: %d\n"), words);
  gchar *s_chars = g_strdup_printf(_("chars: %d\n"), chars);
  gchar *s_charsnsp = g_strdup_printf(_("chars non-space: %d\n"), chars - white_chars);

  gchar *result;

  if (! selected)
     result = g_strconcat (_("stats for "),
                           doc->file_name,
                           ":\n",
                           s_charsnsp,
                           s_chars,
                           s_words,
                           s_lines,
                           s_bytes,
                           NULL);
   else
       result = g_strconcat (_("stats for the selection"),
                             ":\n",
                             s_charsnsp,
                             s_chars,
                             s_words,
                             s_bytes,
                             NULL);

  log_to_memo (result, NULL, LM_NORMAL);

  g_free (s_bytes);
  g_free (s_charsnsp);
  g_free (s_chars);
  g_free (s_words);
  g_free (s_lines);
  g_free (result);
  g_free (text);
}
*/


//from Gedit - plugin
/*
 * docinfo.c
 * This file is part of gedit
 *
 * Copyright (C) 2002 Paolo Maggi

modified by roxton
*/

void make_stats (t_note_page *doc)
{
  if (! doc)
     return;

  if (! doc->text_buffer)
     return;

  gboolean selected = TRUE;

  gchar *text;
  PangoLogAttr *attrs;
  gint words = 0;
  gint chars = 0;
  gint white_chars = 0;
  gint lines = 0;
  gint bytes = 0;
  gint i;
  gchar *tmp_str;

  selected = doc_is_sel (doc->text_buffer);

  if (! selected)
      text = doc_get_buf (doc->text_buffer);
  else
      text = doc_get_sel (doc);

  if (! text)
     return; 

  if (! g_utf8_validate (text, -1, NULL))
     return;

  lines = gtk_text_buffer_get_line_count (doc->text_buffer);

  chars = g_utf8_strlen (text, -1);
  attrs = g_new0 (PangoLogAttr, chars + 1);

  pango_get_log_attrs (text,
                       -1,
                       0,
                       pango_language_from_string ("C"),
                       attrs,
                       chars + 1);

  i = 0;

  while (i < chars)
        {
         if (attrs [i].is_white)
            ++white_chars;

         if (attrs [i].is_word_start)
            ++words;

         ++i;
        }

  if (chars == 0)
     lines = 0;

  bytes = strlen (text);

  gchar *s_bytes = g_strdup_printf(_("bytes: %d\n"), bytes);
  gchar *s_lines = g_strdup_printf(_("lines: %d\n"), lines);
  gchar *s_words = g_strdup_printf(_("words: %d\n"), words);
  gchar *s_chars = g_strdup_printf(_("chars: %d\n"), chars);
  gchar *s_charsnsp = g_strdup_printf(_("chars non-space: %d\n"), chars - white_chars);

  gchar *result;

  if (! selected)
     result = g_strconcat (_("stats for "),
                           doc->file_name,
                           ":\n",
                           s_charsnsp,
                           s_chars,
                           s_words,
                           s_lines,
                           s_bytes,
                           NULL);
   else
       result = g_strconcat (_("stats for selected:\n"),
                             s_charsnsp,
                             s_chars,
                             s_words,
                             s_bytes,
                             NULL);

  //log_to_memo (result, NULL, LM_NORMAL);

  g_free (s_bytes);
  g_free (s_charsnsp);
  g_free (s_chars);
  g_free (s_words);
  g_free (s_lines);
  g_free (result);
  g_free (attrs);
  g_free (text);
}


gboolean is_rtf (const gchar *filename)
{
  if (check_ext (filename, ".rtf"))
      return TRUE;

  return FALSE;
}


gchar* get_lang_name (void)
{
   if (strcmp (g_getenv ("LANG"), "UTF-8") == 0)
      return g_strdup ("UTF-8"); 
   return g_strndup (g_getenv ("LANG"), 2);
}  


gchar* find_good_browser (void)
{
   gchar *t = g_find_program_in_path ("konqueror");
   if (! t)
      t = g_find_program_in_path ("opera");
      if (! t)
         t = g_find_program_in_path ("firefox");
         if (! t)
            t = g_find_program_in_path ("mozilla");
            if (! t)
                t = g_find_program_in_path ("galeon");

   return t;
}


gchar* compose_browser_cmd (gchar *filename)
{
  gchar *t;
  gchar *b; 

  if (confile.use_def_doc_browser)
     {
      if (confile.cmd_def_doc_browser)
         t = rep_all_s (confile.cmd_def_doc_browser, filename); 
      return t;
     }
  
  b = find_good_browser ();
  
  if (! b)
     return NULL;

  t = g_strconcat (b, " ", filename, " &", NULL);

  g_free (b);
  return t;
}


gchar* get_tea_doc_compose_name (gchar *f)
{
  return g_strconcat (TEA_DOC_DIR, f, NULL);
} 


gchar* find_doc_index_name (void)
{
  gchar *l = get_lang_name ();

  gchar *f = g_strconcat (TEA_DOC_DIR, l, "/index.html", NULL);

  if (! g_file_test (f, G_FILE_TEST_EXISTS))
     {
      g_free (f);
      f = g_strconcat (TEA_DOC_DIR, "en/index.html", NULL);
      g_free (l);
      return f;
     }

  g_free (l);
  return f;
}


void run_doc_in_browser (void)
{
  gchar *f = find_doc_index_name ();
  gchar *t = compose_browser_cmd (f);
  if (t)
     system (t); 

  g_free (f);
  g_free (t);
}


gchar* get_clipboard_text (void)
{
  GtkClipboard *c = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
  if (! c)
     return NULL; 

  if (! gtk_clipboard_wait_is_text_available (c))
     return NULL;

  return gtk_clipboard_wait_for_text (c); 
}  


void insert_image (gchar *a_filename)
{
  if (! a_filename)
     return;

  if (! g_file_test (a_filename, G_FILE_TEST_EXISTS))
     return;

  t_image_info *ii = get_image_info (a_filename);

  gchar *dir = g_path_get_dirname (cur_text_doc->file_name);
  gchar *filename = create_relative_link_to (dir, a_filename);
 
  if (! filename) 
     {
      g_free (dir); 
      return;
     }
       
  gchar *t;
  gchar *x = g_strrstr (filename, G_DIR_SEPARATOR_S);
  if (x)
     t = g_utf8_next_char (x);
  else
      t = filename; 

  gchar *temp = g_malloc (7777);

  if (! confile.xhtml_mode)
     {
      if (ii)
         g_snprintf (temp,
                     7777,
                     "<img src=\"%s\" border=\"0\" alt=\"\" width=\"%d\" height=\"%d\">",
                      t, ii->width, ii->height);
      else
          g_snprintf (temp,
                      7777,
                      "<img src=\"%s\" border=\"0\" alt=\"\" width=\"\" height=\"\">",
                      t);
      }
  else
      {
       if (ii)
          g_snprintf (temp,
                      7777,
                      "<img src=\"%s\" border=\"0\" alt=\"\" width=\"%d\" height=\"%d\" />",
                      t, ii->width, ii->height);
      else
          g_snprintf (temp,
                      7777,
                      "<img src=\"%s\" border=\"0\" alt=\"\" width=\"\" height=\"\" />",
                      t); 
      }

  doc_insert_at_cursor (cur_text_doc, temp);

  g_free (ii);
  g_free (temp);
  g_free (filename);
  g_free (dir);
}


void insert_link (gchar *a_filename)
{
  if (! a_filename)
     return;

  if (! g_file_test (a_filename, G_FILE_TEST_EXISTS))
     return;
  
  gchar *dir = g_path_get_dirname (cur_text_doc->file_name);
  gchar *filename = create_relative_link_to (dir, a_filename);
 
  if (! filename) 
     {
      g_free (dir); 
      return;
     }
       
  gchar *t;
  gchar *x = g_strrstr (filename, G_DIR_SEPARATOR_S);
  if (x)
     t = g_utf8_next_char (x);
  else
      t = filename; 

  gchar *z =  g_strdup_printf ("<a href=\"%s\"></a>", t); 

  doc_insert_at_cursor (cur_text_doc, z);
  doc_move_cursor_backw_middle_tags (cur_text_doc);

  g_free (filename);
  g_free (dir);
  g_free (z);
}


gboolean is_css (const gchar *filename)
{
  if (! filename)
      return FALSE;

  gchar *f = g_ascii_strdown (filename, -1);

  if (check_ext (f, ".css"))
    {
     g_free (f);
     return TRUE;
    }

  g_free (f);
  return FALSE;
}


static build_menu_from_ht_cb (gpointer key,
                              gpointer value,
                              gpointer user_data)
{
  t_ppair *t = user_data;
  new_menu_item (key, t->a, t->b);
}


void build_menu_from_ht (GHashTable *hash_table, gpointer menu, gpointer callback)
{
   if (! ht_entities)
       return; 
 
   t_ppair t;
   t.a = menu;
   t.b = callback; 

   g_hash_table_foreach (hash_table,
                         build_menu_from_ht_cb,
                         &t);
}


void build_menu_from_glist (GList *list, gpointer menu, gpointer callback)
{
   if (! list)
       return; 
 
   GList *p = g_list_first (list);
   while (p)
        {
         if (strlen (p->data) >= 1)
            mni_temp = new_menu_item (p->data, menu, callback);
         p = g_list_next (p);
        }
}


//current music: Radiohead - OK Computer - Paranoid Andriod
void build_menu_wudata_from_glist (GList *list, gpointer menu, gpointer callback, gpointer udata)
{
	int nbr=1;
   if (! list)
       return; 
   GList *p = g_list_first (list);
   while (p)
        {
				if(nbr<16)
				{
		         if (p->data)
					{
		         if (strlen (p->data) >= 1){
            mni_temp = new_menu_item_with_udata (p->data, menu, callback, udata);
	         p = g_list_next (p);
				nbr++;
					}else{return;}}else{return;}	
	        }else{return;}
		  }
}



GList* read_dir_files (gchar *path)
{
  GList *l = NULL;

  GDir *d = g_dir_open (path, 0, NULL);  
  gchar *t;

  while (t = g_dir_read_name (d))
        {
         if (! g_file_test (t, G_FILE_TEST_IS_DIR))
             l = g_list_prepend (l, g_strdup (t));
        }

  g_dir_close (d);  
  return g_list_reverse (l);
}


void handle_file_ide (gchar *filename, gint line)
{
  if (! filename || ! cur_tea_project)
     return;
 
  gchar *cmd = g_strconcat (cur_tea_project->dir_source, filename, NULL);
 
  gint i = get_n_page_by_filename (cmd);

  if (i != -1)
     {
      gtk_notebook_set_current_page (notebook1, i);
      if (get_page_text ())
          doc_select_line (cur_text_doc, line); 
      return;
     } 

  if (! g_file_test (cmd, G_FILE_TEST_EXISTS))
     return;
 
  else
      {
       if (! get_page_text() )
          cur_settings.selected_enc = ch_str (cur_settings.selected_enc, "UTF-8");
       else
           cur_settings.selected_enc = ch_str (cur_settings.selected_enc, cur_text_doc->encoding);

       cur_text_doc = open_file_std (cmd);
       doc_select_line (cur_text_doc, line);
      }
}


gboolean parse_error_line (const gchar *line, gchar **filename, gint *lineno)
{
   if (! line)
       return FALSE;

   if (! strstr (line, ":"))
       return FALSE;

   gchar **a = g_strsplit (line, ":", -1);
   
   if (a[0])
      if (a[1])
         if (a[2])
            if (strstr (a[2], "error") || strstr (a[2], "warning")) 
            {
             *filename = g_strstrip (g_strdup (a[0]));
             *lineno = strtol (a[1], NULL, 10);
             g_strfreev (a);   
             return TRUE; 
            }

   g_strfreev (a);   
   return FALSE;
}
