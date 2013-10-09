/***************************************************************************
                          rox_strings.c  -  description
                             -------------------
    begin                : Fri Oct 17 12:08:36 EEST 2003
    copyright            : (C) 2003, 2004 by Peter 'Roxton' Semiletov
    email                : roxton@ua.fm
***************************************************************************/

/*
Copyright (C) 1999 Antti-Juhani Kaijanaho
Copyright (C) 1998-2003 A.J. van Os
Copyright (C) 1998-1999, Mark Spencer <markster@marko.net>
2003, Nathan Walp <faceprint@faceprint.com>
(C) 2002 Olivier Sessink
Copyright (C) 1997-2003 Stuart Parmenter //Balsa::misc.c, quote_color.c
Chris Phelps <chicane@reninet.com>
Alejandro Dubrovsky <s328940@student.uq.edu.au> 

*/

/***************************************************************************
 *                                                                          *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stddef.h>
#include <errno.h>
#include <regex.h>
#include <glib.h>
#include <gtk/gtk.h>
//#include <gtkspell/gtkspell.h>

#include "rox_strings.h"
#include "griffon_defs.h"
#include "griffon_config.h"
#include "griffon_text_document.h"
#include "griffon_funx.h"
#include "griffon_enc.h"


//*  Glimmer - misc.c
/*
Chris Phelps <chicane@reninet.com>

Alejandro Dubrovsky <s328940@student.uq.edu.au>
modified by roxton
*/
gchar* g_str_replace (const gchar *input, const gchar *replace, const gchar *with)
{
   gchar *text = g_strdup (input);
   gchar *start = strstr (text, replace);
    
   if (! start)
      return (NULL);

   gchar *end = start + strlen (replace);
   *start = '\0';
   gchar *value = g_strconcat (text, with, end, NULL);
   g_free (text);
   return (value);
}
//

//from gaim:: html.c
// *
// * Copyright (C) 1998-1999, Mark Spencer <markster@marko.net>
// *               2003, Nathan Walp <faceprint@faceprint.com>

gchar *strip_html(const gchar *text)
{
	int i, j, k;
	int visible = 1;
	gchar *text2 = g_strdup(text);

	if(!text)
		return NULL;

	for (i = 0, j = 0; text2[i]; i++) {
		if (text2[i] == '<') {
			k = i + 1;
			if(g_ascii_isspace(text2[k])) {
				visible = 1;
			} else {
				while (text2[k]) {
					if (text2[k] == '<') {
						visible = 1;
						break;
					}
					if (text2[k] == '>') {
						visible = 0;
						break;
					}
					k++;
				}
			}
		} else if (text2[i] == '>' && !visible) {
			visible = 1;
			continue;
		}
		if (text2[i] == '&' && strncasecmp(text2+i,"&quot;",6) == 0) {
		    text2[j++] = '\"';
		    i = i+5;
		    continue;
		}
		if (visible) {
			text2[j++] = text2[i];
		}
	}
	text2[j] = '\0';
	return text2;
}
//end of gaim code


/*
 * tNumber2Roman - convert a number to Roman Numerals
 *
 * returns the number of characters written
 */

//taken from misc.c of Antiword
//Copyright (C) 1998-2003 A.J. van Os; Released under GNU GPL
//mod. by me, roxton
size_t tNumber2Roman(guint uiNumber, int bUpperCase, char *szOutput)
{
	char	*outp, *p, *q;
	guint	uiNextVal, uiValue;

	uiNumber %= 4000;	/* Very high numbers can't be represented */
	if (uiNumber == 0) {
		szOutput[0] = '\0';
		return 0;
	}

	outp = szOutput;
	p = bUpperCase ? "M\2D\5C\2L\5X\2V\5I" : "m\2d\5c\2l\5x\2v\5i";
	uiValue = 1000;
	for (;;) {
		while (uiNumber >= uiValue) {
			*outp++ = *p;
			uiNumber -= uiValue;
		}
		if (uiNumber == 0) {
			*outp = '\0';
			return (size_t)(outp - szOutput);
		}
		q = p + 1;
		uiNextVal = uiValue / (uint)(char)*q;
		if ((int)*q == 2) {		/* magic */
			uiNextVal /= (guint)(char)*(q += 2);
		}
		if (uiNumber + uiNextVal >= uiValue) {
			*outp++ = *++q;
			uiNumber += uiNextVal;
		} else {
			p++;
			uiValue /= (guint)(char)(*p++);
		}
	}
} /* end of tNumber2Roman */


gchar* get_file_ver_name (const gchar *name)
{
  if (! g_file_test (name, G_FILE_TEST_EXISTS))
     return NULL;

  gchar *fname = g_path_get_basename (name);
  gchar *dname = g_path_get_dirname (name);
               
  time_t t = time (NULL);
  gchar *st = g_strdup (ctime (&t));
  
  st = g_strdelimit (st, ": ", '_');
  st = g_strstrip (st); 

  gchar *result = g_strconcat (dname, G_DIR_SEPARATOR_S, st, "_", fname, NULL);

  g_free (st);
  g_free (dname);
  g_free (fname);

  return result;
}


GList* parse_string_to_list (gchar *s)
{
  gchar **a = NULL;
  GList *result = NULL;
  
  a = g_strsplit (s, "\n", -1);
  gint c = -1;

  if (a)
     while (a[++c])
           if (! g_str_has_prefix (a[c], "#"))
              result = g_list_prepend (result, g_strdup (a[c])); 
         
  g_strfreev (a);
  return g_list_reverse (result);
}


GList* parse_sep_string_to_list (gchar *s, gchar *sep)
{
  gchar **a = NULL;
  GList *result = NULL;
 
  gint c = -1;
  a = g_strsplit (s, sep, -1);

  if (a)
     while (a[++c])
           {
            gchar *st = g_strdup (a[c]);
            st = g_strchug (st);
            if (st[0] == '+')  
               result = g_list_prepend (result, g_strdup (++st));
           }

  g_strfreev (a);
  return g_list_reverse (result);
}


GList* add_to_glist_combined_bool (GList *list, gchar *key, gboolean value)
{
  gint v;
  if (value)
     v = TRUE;
   else 
       v = FALSE;

  return g_list_prepend (list, g_strdup_printf ("%s=%d", key, v));  
}                                                     


GList* add_to_glist_combined_int (GList *list, gchar *key, gint value)
{
  return g_list_prepend (list, g_strdup_printf ("%s=%d", key, value));  
}                                                     


GList* add_to_glist_combined_str (GList *list, gchar *key, gchar *value)
{
  if (! key || ! value)
     return list;

  return g_list_prepend (list, g_strdup_printf ("%s=%s", key, value));  
}                                                     


gint sort_node (gchar* a, gchar *b)
{
  if (! a && ! b)
     return 0;
  else if (! a) 
      return -1;
  else if (! b) 
      return 1;

  gchar *t1 = g_utf8_collate_key (a, -1);  
  gchar *t2 = g_utf8_collate_key (b, -1);  
  gint result = strcmp (t1, t2);         

  g_free (t1); 
  g_free (t2);
 
  return result;
}


GList* glist_strings_sort (GList *list)
{
  return g_list_sort (list, (GCompareFunc) sort_node);
}


GList* glist_from_string (const gchar *string)
{
  gchar **a = NULL;
  GList *result = NULL;

  a = g_strsplit (string, "\n", -1);
  gint c = -1;
  if (a)
     while (a[++c])
           result = g_list_prepend (result, g_strdup (a[c]));
    
  g_strfreev (a);
  return g_list_reverse (result);
}


gchar* string_from_glist (GList *list)
{
  gchar *result = NULL;
  GList *p = g_list_first (list);
  gchar *t = NULL;
  GList *l = g_list_last (list);
  
  GString *s = g_string_sized_new (1024); 

  while (p)
        {
         if ((p != l) && (p->data))
            t = g_strconcat ((gchar*) p->data, "\n", NULL);
         else
             t = g_strdup ((gchar*) p->data);

         s = g_string_append (s, t);
         p = g_list_next (p);
         g_free (t);
        }

  result = s->str;
  g_string_free (s, FALSE);
  
  return result;
}


gchar* string_from_glist_raw (GList *list)
{
  gchar *result = NULL;
  GList *p = g_list_first (list);
  gchar *t = NULL;
  GList *l = g_list_last (list);
  
  GString *s = g_string_sized_new (1024); 

  while (p)
        {
         if (p->data)
           {
            if (p != l)
               t = g_strconcat ((gchar*) p->data, " ", NULL);
            else
                t = g_strdup ((gchar*) p->data);

            s = g_string_append (s, t);
            g_free (t);
           }  

         p = g_list_next (p);
        }

  result = s->str;
  g_string_free (s, FALSE);
  
  return result;
}


/*GList* glist_enclose (GList* list, gchar *a, gchar *b)
{
  GList *p = g_list_first (list);
  gchar *t = NULL;

  while (p)
        {
         if (a)
            t = g_strconcat (a, (gchar*) p->data, b, NULL);
          else 
              t = g_strconcat ((gchar*) p->data, b, NULL);
        
         g_free (p->data);
         (gchar*) p->data = t;
         p = g_list_next (p);
        }

  return list;
}*/


void glist_print (GList* list)
{
  GList *p = g_list_first (list);
  gchar *t = NULL;

  while (p)
        {
         g_printf ("%s\n", (gchar*) p->data); 
         p = g_list_next (p);
        }
}


GList* load_file_to_glist (gchar *filename)
{
  if (! g_file_test (filename, G_FILE_TEST_EXISTS))
     return NULL;
    
  gsize length;
  gchar *buf;

  if (! g_file_get_contents (filename, &buf, &length, NULL))
     return NULL;

  GList *t = glist_from_string (buf);
  g_free (buf);
  return t;
}


GList* glist_from_string_with_limit (const gchar *string, gint limit)
{
  gchar **a = NULL;
  GList *result = NULL;

  a = g_strsplit (string, "\n", -1);
  gint c = -1;
  if (a)
     while (a [++c] && c != limit)
           result = g_list_prepend (result, g_strdup (a[c]));
    
  g_strfreev (a);
  return g_list_reverse (result);
}


GList* load_file_to_glist_with_limit (gchar *filename, gint limit)
{
  gsize length;
  gchar *buf;

  if (! g_file_get_contents (filename, &buf, &length, NULL))
     return NULL;

  GList *t = glist_from_string_with_limit (buf, limit);
  g_free (buf);
  return t;
}


void glist_save_to_file (GList *list, gchar *filename)
{
  FILE *out = fopen (filename, "w+");
  if (! out)
      return;

  GList *p = g_list_first (list);

  while (p)
       {
        if (p->data)
           fprintf (out, "%s\n", (gchar*) p->data);
        p = g_list_next (p);
       }

  fclose (out);
}


GList* filter_exclude_from_list (const GList *list, gchar const *phrase)
{
  if (! phrase)
     return list;

  GList *t = NULL;
  GList *p = g_list_first (list);

  while (p)
        {
         if (! strstr (phrase, (gchar *) p->data))
            t = g_list_prepend (t, (gchar *) p->data);
         p = g_list_next (p);
        }

  return g_list_reverse (t);
}


GList* filter_antiexclude_from_list (const GList *list, gchar const *phrase)
{
  if (! phrase)
      return list;

  GList *t = NULL;
  GList *p = g_list_first (list);

  while (p)
        {
         if (strstr (phrase, (gchar *) p->data))
            t = g_list_prepend (t, (gchar *) p->data);
         p = g_list_next (p);
        }

  return g_list_reverse (t);
}


GList* find_item_by_data (GList *list, gchar const *s)
{
  if (! s)
      return NULL;

  GList *p = g_list_first (list);

  while (p)
        {
         if (strcmp (s, (gchar *) p->data) == 0)
            return p;
         p = g_list_next (p);
        }

  return NULL;
}


GList* str_in_glist (GList *list, gchar *st)
{
  if (! st)
      return NULL;

  GList *result = NULL;
  GList *p = g_list_first (list);

  while (p)
        {
         if (g_utf8_collate (st, (gchar *) p->data) == 0)
            result = p;
         p = g_list_next (p);
        }

  return result;
}


GList* str_in_glist2 (GList *list, gchar *st)
{
  if (! st)
     return NULL;

  GList *result = NULL;
  GList *p = g_list_first (list);

  while (p)
        {
         if (p->data)
            if (strstr ((gchar*) p->data, st))
               result = p;
         p = g_list_next (p);
        }

  return result;
}


gchar* linkify_text (const gchar *text)
{
  if (! text)
     return NULL;

  gboolean need_for_protocol = TRUE;
  gboolean is_email = FALSE;
  gchar *protocol = NULL;  
  gchar *result = NULL;  

  if (strstr (text, "http://"))
      need_for_protocol = FALSE;
      
  if (strstr (text, "ftp://"))
      need_for_protocol = FALSE;

  if (strstr (text, "mailto:"))
      need_for_protocol = FALSE;

  if (strstr (text, "ftp."))
     protocol = g_strdup ("ftp://");
  else
      if (strstr (text, "@"))
         {
          protocol = g_strdup ("mailto:");
          is_email = TRUE; 
         }
      else  
          protocol = g_strdup ("http://");
 
  if (need_for_protocol)
     {
      if (! is_email)
         result = g_strdup_printf ("<a href=\"%s%s\">%s</a>", protocol, text, text);
      else
          result = g_strdup_printf ("<a href=\"%s%s\">%s</a>", protocol, text, text);
     }
  else 
      result = g_strdup_printf ("<a href=\"%s\">%s</a>", text, text);

  g_free (protocol);
  return result;
}


GList* remove_blank_lines (GList *lines)
{
  GList *l = NULL;
  GList *t = g_list_first (lines);
  while (t)
       {
        if (t->data)
        if (strlen ((gchar*) t->data) > 0)
           l = g_list_prepend (l, g_strdup ((gchar*) t->data));

        t = g_list_next (t);
       }

  return g_list_reverse (l);
}


void glist_strings_free (GList *list)
{
  GList *t = list;
  while (t)
        {
         if (t->data)
            g_free (t->data);
         t = g_list_next (t);
        }

  g_list_free (t);
}



static gchar* enc_guess_value = NULL;

gchar* enc_guess (const gchar *s)
{
  if (! s)
     return NULL;

  g_free (enc_guess_value);
  
  if (g_utf8_validate (s, -1, NULL))
     {
      enc_guess_value = g_strdup ("UTF-8");
      return enc_guess_value;
     }
        
  if (confile.enc_det_koi8u)
  if (enc_is_koi8u (s))
     {
      enc_guess_value = g_strdup ("KOI8-U");
      return enc_guess_value;
     }   

  if (confile.enc_det_koi8r)
  if (enc_is_koi8r (s))
     {
      enc_guess_value = g_strdup ("KOI8-R");
      return enc_guess_value;
     }   

  if (confile.enc_det_cp1251)
  if (enc_is_cp1251 (s))
     {
      enc_guess_value = g_strdup ("CP1251");
      return enc_guess_value;
     }   

  if (confile.enc_det_866)
  if (enc_is_dos866 (s))
     {
      enc_guess_value = g_strdup ("866");
      return enc_guess_value;
     }   

  if (confile.enc_det_finnish)
  if (enc_is_finnish (s))
     {
      enc_guess_value = g_strdup ("ISO-8859-15");
      return enc_guess_value;
     }   

  if (confile.enc_det_portuguese)
  if (enc_is_portuguese (s))
     {
      enc_guess_value = g_strdup ("ISO-8859-1");
      return enc_guess_value;
     }   

  if (confile.enc_det_slovak)
  if (enc_is_slovak (s))
     {
      enc_guess_value = g_strdup ("ISO-8859-2");
      return enc_guess_value;
     }   

  if (confile.enc_det_slovenian)
  if (enc_is_slovenian (s))
     {
      enc_guess_value = g_strdup ("ISO-8859-2");
      return enc_guess_value;
     }   

  if (confile.enc_det_turkish)
  if (enc_is_turkish (s))
     {
      enc_guess_value = g_strdup ("ISO-8859-9");
      return enc_guess_value;
     }   
  
  if (confile.enc_det_spanish)
  if (enc_is_spanish (s))
     {
      enc_guess_value = g_strdup ("ISO-8859-1");
      return enc_guess_value;
     }   

  if (confile.enc_det_polish)
  if (enc_is_polish (s))
     {
      enc_guess_value = g_strdup ("ISO-8859-2");
      return enc_guess_value;
     }   

  if (confile.enc_det_latvian)
  if (enc_is_latvian (s))
     {
      enc_guess_value = g_strdup ("ISO-8859-13");
      return enc_guess_value;
     }   
  
  if (confile.enc_det_german)
  if (enc_is_german (s))
     {
      enc_guess_value = g_strdup ("ISO-8859-15");
      return enc_guess_value;
     }   

  if (confile.enc_det_serbian)
  if (enc_is_serbian (s))
     {
      enc_guess_value = g_strdup ("ISO-8859-2");
      return enc_guess_value;
     }   
 
  //enc_guess_value = g_strdup ("UTF-8");
  return NULL;
} 


gchar* str_file_read (const gchar *filename)
{
  gsize length;
  gchar *buf;

  if (! g_file_get_contents (filename, &buf, &length, NULL))
     return NULL;
  else
      return buf; 
} 


typedef struct {
               gchar *word;
               gint count;  
              } t_struct_word;


static GList *l_words;

gint sort_node_m1 (t_struct_word *a, t_struct_word *b)
{
  gchar *t1 = g_utf8_collate_key (a->word, -1);  
  gchar *t2 = g_utf8_collate_key (b->word, -1);  
  gint result = strcmp (t1, t2);         

  g_free (t1); 
  g_free (t2);
 
  return result;
}


gint sort_node_m2 (t_struct_word *a, t_struct_word *b)
{
  if (a->count == b->count)
     return 0;    

  if (a->count < b->count)
     return -1;    

  if (a->count > b->count)
     return 1;    
}


gint sort_node_m4 (gchar *a, gchar *b)
{
  glong x = g_utf8_strlen (a, - 1);  
  glong z = g_utf8_strlen (b, - 1);   

  if (x == z) 
     return 0;

  if (x < z) 
     return -1;

  if (x > z) 
     return 1;
}


GList* glist_word_sort_mode (GList *list, gint mode)
{
  if (mode == 0) return g_list_sort (list, (GCompareFunc) sort_node_m1);
  if (mode == 1) return g_list_reverse (g_list_sort (list, (GCompareFunc) sort_node_m2));
  if (mode == 2) return g_list_reverse (g_list_sort (list, (GCompareFunc) sort_node_m4));
  if (mode == -1) return list;
}


void free_word_data (gpointer data)
{
  t_struct_word *ts = data;
  g_free (ts->word);
  g_free (ts);
}


void str_walk_extract_word (gpointer key,
                            gpointer value,
                            gpointer user_data)
{
  l_words = g_list_prepend (l_words, key);
}


void run_extract_words ( t_note_page *page)
{
  GList *list = NULL;
  GtkTextIter a;
  GtkTextIter b;
  
  l_words = NULL;
  GHashTable *words = g_hash_table_new (g_str_hash, g_str_equal);
                                         
  gchar *s;
  gchar *t;
  GtkTextIter start;
  GtkTextIter end;

  if (doc_is_sel (page->text_buffer))
     gtk_text_buffer_get_selection_bounds (page->text_buffer, &start, &end);
  else
      {
       gtk_text_buffer_get_start_iter (page->text_buffer, &start); 
       gtk_text_buffer_get_end_iter (page->text_buffer, &end); 
      }  

  do 
    {
     if (gtk_text_iter_starts_word (&start))
        a = start; 
     
     if (gtk_text_iter_ends_word (&start))
        {
         s = gtk_text_buffer_get_text (page->text_buffer,
                                       &a,
                                       &start,
                                       FALSE);
     
         t = g_hash_table_lookup (words, s);
         if (! t)
            g_hash_table_insert (words, s, s);
         else
             g_free (s);
         }
      }

  while (gtk_text_iter_forward_char (&start)); 
             
  g_hash_table_foreach (words, str_walk_extract_word, NULL);
  
  GList *tmplist = g_list_first (g_list_reverse (l_words));
  while (tmplist)
        {
         list = g_list_prepend (list, tmplist->data);
         tmplist = g_list_next (tmplist);
        }

  cur_text_doc = doc_clear_new ();
  
  gchar *sr = string_from_glist (g_list_reverse (list));
   
  gtk_text_buffer_insert_at_cursor (cur_text_doc->text_buffer, sr, -1);
  g_free (sr);
  glist_strings_free (list);
  g_list_free (l_words);
  g_hash_table_destroy (words); 
}


void walk_by_words (gpointer key,
                    gpointer value,
                    gpointer user_data)
{
  l_words = g_list_prepend (l_words, value);
}


void run_unitaz ( t_note_page *page, gint sort_type, gboolean case_insensetive)
{
  GList *list = NULL;
  GtkTextIter a;
  GtkTextIter b;

  int w_total = 0;
  int w_unique = 0;  
  l_words = NULL;

  GHashTable *words = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, free_word_data);
  
  t_struct_word *ts = NULL;
  t_struct_word *t = NULL;
  gchar *s;
  gchar *z;  

  GtkTextIter start;
  GtkTextIter end;

  if (doc_is_sel (page->text_buffer))
     gtk_text_buffer_get_selection_bounds (page->text_buffer, &start, &end);
  else
      {
       gtk_text_buffer_get_start_iter (page->text_buffer, &start); 
       gtk_text_buffer_get_end_iter (page->text_buffer, &end); 
      }  

  do 
    {
     if (gtk_text_iter_starts_word (&start))
        a = start; 
     
     if (gtk_text_iter_ends_word (&start))
        {
         w_total++;  
         if (case_insensetive)
            {          
             z = gtk_text_buffer_get_text (page->text_buffer,
                                           &a,
                                           &start,
                                           FALSE);
             s = g_utf8_strdown (z, -1);
             g_free (z);
            }
         else
             s = gtk_text_buffer_get_text (page->text_buffer,
                                           &a,
                                           &start,
                                           FALSE);

         t = g_hash_table_lookup (words, s);
         if (! t)
              {
               ts = (t_struct_word *) g_malloc (sizeof (t_struct_word));
               ts->count = 1;  
               ts->word = s;
               g_hash_table_insert (words, s, ts);
               w_unique++;
              }
            else
                { 
                 t->count++;
                 g_free (s);
                }
           }
    }

  while ( gtk_text_iter_forward_char (&start)); 
    
  gint i;
  gchar *st = NULL;
             
  g_hash_table_foreach (words, walk_by_words, NULL);
  GList *tl;

  st = g_strdup_printf ("%s %s\n", _("Text analysis results for "), page->file_name);  
  list = g_list_prepend (list, st);

  st = g_strdup_printf ("Words total: %d", w_total);
  list = g_list_prepend (list, st);
  st = g_strdup_printf ("Words unique: %d", w_unique);
  list = g_list_prepend (list, st);

  st = g_strdup_printf ("Words total / Words unique = %.6f\n", (float)w_total/w_unique);
  list = g_list_prepend (list, st);
  st = g_strdup_printf ("%-10s %s", _("Count:"), _("Word:"));  
  list = g_list_prepend (list, st);

  l_words = glist_word_sort_mode (l_words, sort_type);
  
  GList *tmplist = g_list_first (l_words);
  while (tmplist)
        {
         t = (t_struct_word *) tmplist->data;
         st = g_strdup_printf ("%-10d %s", t->count, t->word);  
         list = g_list_prepend (list, st);
         tmplist = g_list_next (tmplist);
        }
  
  cur_text_doc = doc_clear_new ();
  
  gchar *sr = string_from_glist (g_list_reverse (list));
   
  gtk_text_buffer_insert_at_cursor (cur_text_doc->text_buffer, sr, -1);
  g_free (sr);
  glist_strings_free (list);
  g_list_free (l_words);
  g_hash_table_destroy (words); 
}


GList* glist_shuffle (GList *list)
{
  GList *i1 = NULL;
  gchar *t;
  GRand *r;
  gint32 i;

  GList *p = g_list_first (list);
  gint c = (g_list_length (p) - 1);

  while (p)
        {
         r = g_rand_new ();
         i = g_rand_int_range (r, 0, c);  
         i1 = g_list_nth (list, i);
         g_rand_free (r);  
         
         if (i1 != p)
            { 
             t = g_strdup (p->data);
             g_free (p->data);
             p->data = g_strdup (i1->data); 
             g_free (i1->data);
             i1->data = t;   
            }
          
         p = g_list_next (p);
        }

  return list;
}


gchar* parse_rtf_hex_str (const gchar *s)
{
  GString *gs = g_string_sized_new (1048576);   

  gint i;
  gint c;
  gint code; 
  gchar *st;
  gchar *hex;
  
  st = s;
  
  for (i = 0; st[i]; i++)
      {
       if ((st[i] == '\\') && (st[i+1]) == '\'')
          //hex char
          {
           hex = g_malloc (3);
           hex[0] = st [i+2];
           hex[1] = st[i+3];
           hex[3] = '\0';
           i = i+3;   
           code = strtol (hex, NULL, 16); 
           gs = g_string_append_c (gs, (gchar) code);
           g_free (hex); 
          }
       else //non-hex                 
           if ((st[i] == '\\') && (st[i+1] == 'p') &&  (st[i+2] == 'a'))                  
              {
               i = i + 4;
               gs = g_string_append_c (gs, '\n');
              }
           else
              if ((st[i] == '\\') && (st[i+1] == 't') &&  (st[i+2] == 'a'))                  
              {
               i = i + 4;
               gs = g_string_append_c (gs, '\t');
              }
           else
               if ((st[i] == '{') && (st[i+1] == '\\'))                  
                   while (st[++i] != ' ')
                   ;
               else  
                   if (st[i] > 31)
                      gs = g_string_append_c (gs, st [i]);
      }
 

  gchar *result = g_convert (gs->str, strlen (gs->str), "UTF-8", confile.rtf_enc, NULL, NULL, NULL);

  g_string_free (gs, TRUE);
  return result;
} 


gchar* text_load_rtf (const gchar *filename)
{
  gchar *t = str_file_read (filename);
  gchar *s = parse_rtf_hex_str (t);
  g_free (t); 
  return s;
}


void doc_tabs_to_spaces (t_note_page *doc, gint tabsize) 
{
  if (tabsize < 1)
     tabsize = 1; 
  
  gchar *new_text = g_strnfill (tabsize, ' ');
  gchar *text = g_strdup ("\t");

  if (doc_search_f (doc, text))
     {
      doc_rep_sel (doc, new_text);
      while (doc_search_f_next (doc))
            doc_rep_sel (doc, new_text);
     }  
  
  g_free (text);
  g_free (new_text);
}


void doc_spaces_to_tabs (t_note_page *doc, gint tabsize) 
{
  if (tabsize < 1)
     tabsize = 1; 
  
  gchar *text = g_strnfill (tabsize, ' ');
  gchar *new_text = g_strdup ("\t");

  if (doc_search_f (doc, text))
     {
      doc_rep_sel (doc, new_text);
      while (doc_search_f_next (doc))
            doc_rep_sel (doc, new_text);
     }  
  
  g_free (text);
  g_free (new_text);
}


gint sort_node_m3 (gchar *sa, gchar *sb)
{
  if (! sa && ! sb)
     return 0;
  else if (! sa) 
      return -1;
  else if (! sb) 
      return 1;

  gchar *a = g_utf8_casefold (sa, -1);
  gchar *b = g_utf8_casefold (sb, -1);

  gchar *t1 = g_utf8_collate_key (a, -1);  
  gchar *t2 = g_utf8_collate_key (b, -1);  

  gint result = strcmp (t1, t2);         

  g_free (t1); 
  g_free (t2);
  g_free (a);
  g_free (b);

  return result;
}


GList* sort_list_case_insensetive (GList *list)
{
  return g_list_sort (list, (GCompareFunc) sort_node_m3);
}


gchar *arr_to_glist (gchar **a)
{
  GList *l = NULL; 
  gint i = 0;
  while (a[i])
        l = g_list_prepend (l, g_strdup (a[i++]));

  return g_list_reverse (l);
}


gchar* kill_formatting (gchar *s)
{
  gchar **a = g_strsplit (s, " ", -1);
  GList *l = arr_to_glist (a);  

  GList *t = g_list_first (l);
  while (t)
        {
         g_strstrip (t->data);
         if (g_utf8_strlen (t->data, -1) <= 0)
            t->data = NULL;
         t = g_list_next(t);
        }
       
  gchar *r = string_from_glist_raw (l);
  glist_strings_free (l);
  g_strfreev (a);  
  return r;
}


gchar* wrap_raw (gchar *s, gint str_len)
{
  GString* gs = g_string_sized_new (strlen (s));

  gchar *p = s;
  gchar *t;
  gunichar unichar = g_utf8_get_char (p);
  gint c = -1;

  while ( unichar )
        {
         if (unichar == '\n')
            {
             p = g_utf8_next_char (p);
             unichar = g_utf8_get_char (p);
             continue;
            }
 
         if (++c == str_len)  
            {
             gs = g_string_append_unichar (gs, '\n');       
             gs = g_string_append_unichar (gs, unichar);  
             c = -1;
            }     
         else
             gs = g_string_append_unichar (gs, unichar);       
        
         p = g_utf8_next_char (p);
         unichar = g_utf8_get_char (p);
        }
  
   t = gs->str; 
   g_string_free (gs, FALSE);
   return t;
} 


void wrap_on_spaces (gchar *str, gint width)
/* 
Copyright (C) 1997-2003 Stuart Parmenter //Balsa::misc.c

libbalsa_wrap_string
   wraps given string replacing spaces with '\n'.  do changes in place.
   lnbeg - line beginning position, sppos - space position, 
   te - tab's extra space.
modified by roxton

*/
{
  const gint minl = width / 2;

  gchar *space_pos, *ptr;
  gint te = 0;

  gint ptr_offset, line_begin_offset, space_pos_offset;

  line_begin_offset = ptr_offset = space_pos_offset = 0;
  space_pos = ptr = str;

  while (*ptr) 
        {
         switch (*ptr) 
                {
                 case '\t':
                           te += 7;
                           break;
                 case '\n':
                           line_begin_offset = ptr_offset + 1;
                           te = 0;
                           break;
                 case ' ':
                          space_pos = ptr;
                          space_pos_offset = ptr_offset;
                          break;
                 }

         if (ptr_offset - line_begin_offset >= width - te 
             && space_pos_offset >= line_begin_offset + minl) 
            {
             *space_pos = '\n';
             line_begin_offset = space_pos_offset + 1;
             te = 0;
            }
        ptr=g_utf8_next_char(ptr);
        ptr_offset++;
    }
}


gchar* kill_formatting_on_each_line (gchar *s)
{
  GList *l = glist_from_string (s);
  GList *tl = g_list_first (l);
  gchar *t = NULL; 

  while (tl)
        {
         t = kill_formatting (tl->data);
         g_free (tl->data);
         tl->data = t;
         tl = g_list_next (tl);
        }

   t = string_from_glist (l); 
   glist_strings_free (l);
   return t;
} 


gchar* kill_lesser (gchar *s, gint max)
{
  GList *l = glist_from_string (s);
  GList *tl = g_list_first (l);
  gchar *t = NULL; 
  GList *n = NULL; 

  while (tl)
        {
         if (tl->data) 
         if (g_utf8_strlen (tl->data, -1) > max )
            n = g_list_prepend (n, g_strdup (tl->data)); 
         tl = g_list_next (tl);
        }

   t = string_from_glist (g_list_reverse (n)); 
   glist_strings_free (l);
   glist_strings_free (n);
   return t;
} 


gchar* kill_larger (gchar *s, gint max)
{
  GList *l = glist_from_string (s);
  GList *tl = g_list_first (l);
  gchar *t = NULL; 
  GList *n = NULL; 

  while (tl)
        {
         if (tl->data) 
         if (g_utf8_strlen (tl->data, -1) <= max )
            n = g_list_prepend (n, g_strdup (tl->data)); 
         tl = g_list_next (tl);
        }

   t = string_from_glist (g_list_reverse (n)); 
   glist_strings_free (l);
   glist_strings_free (n);
   return t;
} 


gboolean save_string_to_file (gchar *filename, gchar *s)
{
  FILE *f = fopen (filename, "w");
  if (! f)
     return FALSE;

  fputs (s, f);
  fclose (f);
  return TRUE;
}

gboolean save_string_to_file_add (gchar *filename, gchar *s)
{
	FILE *f = fopen (filename, "a");
	if (! f)
	  return FALSE;

	fputs (s, f);
	fclose (f);
   
  return TRUE;
}

gboolean save_string_to_file_vide (gchar *filename, gchar *s)
{
	FILE *f = fopen (filename, "w");
	if (! f)
	  return FALSE;

	fputs (s, f);
	fclose (f);
   
  return TRUE;
}


gchar* rep_all_s (gchar *s, gchar *r)
{
  if (! strstr (s, "%s") )
     return g_strdup (s);
  return (str_replace_all (s, "%s", r));
}

gchar * str_replace_all(char const * const original,char const * const pattern,char const * const replacement) 
{
   
     size_t const replen = strlen(replacement);
     size_t const patlen = strlen(pattern);
     size_t const orilen = strlen(original);
   
     size_t patcnt = 0;
     const char * oriptr;
     const char * patloc;
   
     // find how many times the pattern occurs in the original string
        for (oriptr = original; patloc = strstr(oriptr, pattern); oriptr = patloc + patlen)
        {
          patcnt++;
        }
      
        {
          // allocate memory for the new string
          size_t const retlen = orilen + patcnt * (replen - patlen);
          char * const returned = (char *) malloc( sizeof(char) * (retlen + 1) );
      
          if (returned != NULL)
          {
            // copy the original string, 
            // replacing all the instances of the pattern
            char * retptr = returned;
            for (oriptr = original; patloc = strstr(oriptr, pattern); oriptr = patloc + patlen)
            {
              size_t const skplen = patloc - oriptr;
              // copy the section until the occurence of the pattern
              strncpy(retptr, oriptr, skplen);
              retptr += skplen;
              // copy the replacement 
              strncpy(retptr, replacement, replen);
              retptr += replen;
            }
            // copy the rest of the string.
            strcpy(retptr, oriptr);
          }
          return returned;
        }
      }
     

GList* add_to_list_with_limit (GList *list, gchar *s, gint limit)
{
  if (! s)
     return list;

  if (! list)
     {
      list = g_list_prepend (list, g_strdup (s));
      return list;  
     }

  GList *t = str_in_glist (list, s); 
  if (t)
     {
      g_free (t->data);
      list = g_list_remove_link (list, t);
      g_list_free_1 (t);
     }

  GList *l;
   
  list = g_list_prepend (list, g_strdup (s));
  if (g_list_length (list) >= limit)
     {
      l = g_list_last (list);
      g_free (l->data);
      list = g_list_remove_link (list, l);
      g_list_free_1 (l);
     } 
   
  return list;
} 


GList* add_to_list_with_limit2 (GList *list, gchar *s, gint limit)
{
  if (! s)
     return list;

  GList *l = NULL;
   
  list = g_list_prepend (list, g_strdup (s));
 
  if (g_list_length (list) >= limit)
     {
      l = g_list_last (list);
      g_free (l->data);
      list = g_list_remove_link (list, l);
      g_list_free_1 (l);
     } 

  return list;
} 


GList* glist_repl (GList* list, gchar const *format)
{
  if (! format)
     return NULL;
  
  GList *p = g_list_first (list);
  gchar *t = NULL;

  while (p)
        {
         t = str_replace_all (format, "%s", p->data); 
         g_free (p->data);
         p->data = t;
         p = g_list_next (p);
        }

  return list;
}


gchar* glist_enum (gchar *buf, gchar const *format_str)
{
  if (! buf || ! format_str)
    return NULL;

  GList *list = parse_string_to_list (buf);
  
  gchar **a = g_strsplit (format_str, "~", -1);

  if (! a)
      return NULL;
  
  gchar *s_fmt = a[0];

  gchar *t = NULL;
  gchar *t2 = NULL;
  gchar *t3 = NULL;
  gint step = 1;
  gint counter = 0;

  GList *temp = NULL;

  if (a[1])
     {
      step = strtol (a[1], NULL, 10);  
      if (a[2])
         counter = strtol (a[2], NULL, 10);  
     }

  GList *p = g_list_first (list);
  
  while (p)
        {
         if (p->data)
           {
            t = str_replace_all (s_fmt, "%s", (gchar *) p->data);
            t3 = g_strdup_printf ("%d",(++counter * step));
            t2 = str_replace_all (t, "%d", t3);
            temp = g_list_prepend (temp, t2);
            g_free (t);           
            g_free (t3);           
           }
         p = g_list_next (p);
        }

  t = string_from_glist (g_list_reverse (temp));

  glist_strings_free (list);
  glist_strings_free (temp);
  g_strfreev (a);

  return t;
}


GHashTable* load_file_to_hashtable (gchar *filename)
{
  GList *l = load_file_to_glist (filename);

  if (! l)
     return FALSE; 

  GHashTable *ht = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);

  GList *tl;
  gchar *key = NULL;
  gchar **pair = NULL;

  tl = g_list_first (l);
  while (tl)
        {
         if (! g_str_has_prefix ((gchar*) tl->data, "#"))
            {
             pair = g_strsplit ((gchar*) tl->data, "=", 2);
             if (pair)   
                if (pair[0])
                   {
                    key = pair [0];
                    if (pair [1])
                       g_hash_table_insert (ht, g_strdup (key), g_strdup (pair[1])); 
                   } 

             g_strfreev (pair);
            }

         tl = g_list_next (tl);
        }

  glist_strings_free (l);
  return ht;
}


static void cb_print_ht (gpointer key,
                         gpointer value,
                         gpointer user_data)
{
  g_print ("%s=%s\n", (gchar*)key, (gchar*)value);
}


void print_ht (GHashTable *ht)
{
  g_hash_table_foreach (ht, cb_print_ht, NULL);
}


gchar* str_crackerize (gchar *buf)
{
  gint32 i;     
  GString* gs = g_string_sized_new (strlen (buf));
  
  gchar *p = buf;
  gunichar u = g_utf8_get_char (p);

  while ( u )
        {
         GRand *r = g_rand_new ();     
         
         i = g_rand_int_range (r, 0, 3);

         switch (i)
                {
                 case 0: u = g_unichar_toupper (u); 
                         break; 
                 case 1: u = g_unichar_tolower (u); 
                         break; 
                 case 2: u = g_unichar_toupper (u); 
                         break; 
                 case 3: u = g_unichar_tolower (u); 
                         break; 

                }

         gs = g_string_append_unichar (gs, u);        
  
         g_rand_free (r);
 
         p = g_utf8_next_char (p);
         u = g_utf8_get_char (p);
        }

  p = gs->str;
  g_string_free (gs, FALSE);
  return p;
}


gchar* str_eol_to_crlf (gchar *buf)
{
  GString* gs = g_string_sized_new (strlen (buf));

  gchar *p = buf;
  gunichar unichar = g_utf8_get_char (p);
  gchar *s;

  while ( unichar )
        {
         if ( unichar == '\n')       
             gs = g_string_append (gs, "\r\n");            
         else 

         gs = g_string_append_unichar (gs, unichar);        
   
         p = g_utf8_next_char (p);
         unichar = g_utf8_get_char (p);
        }

  s = gs->str;
  g_string_free (gs, FALSE);

  return s;
}


gint get_gchar_array_size (gchar **a)
{
   gint c = 0;
   while (a[++c])
         ;
   return c;
}


gchar* morse_encode (gchar *s)
{
  if (! s)
     return NULL;

  GHashTable *ht = g_hash_table_new (g_str_hash, g_str_equal);

  g_hash_table_insert (ht, "A", ".-");
  g_hash_table_insert (ht, "B", "-...");
  g_hash_table_insert (ht, "C", "-.-.");
  g_hash_table_insert (ht, "D", "-..");
  g_hash_table_insert (ht, "E", ".");
  g_hash_table_insert (ht, "F", "..-.");
  g_hash_table_insert (ht, "G", "--.");
  g_hash_table_insert (ht, "H", "....");
  g_hash_table_insert (ht, "I", "..");
  g_hash_table_insert (ht, "J", ".---");
  g_hash_table_insert (ht, "K", "-.-");
  g_hash_table_insert (ht, "L", ".-..");
  g_hash_table_insert (ht, "M", "--");
  g_hash_table_insert (ht, "N", "-.");
  g_hash_table_insert (ht, "O", "---");
  g_hash_table_insert (ht, "P", ".--.");
  g_hash_table_insert (ht, "Q", "--.-");
  g_hash_table_insert (ht, "R", ".-.");
  g_hash_table_insert (ht, "S", "...");
  g_hash_table_insert (ht, "T", "-");
  g_hash_table_insert (ht, "U", "..-");
  g_hash_table_insert (ht, "V", "...-");
  g_hash_table_insert (ht, "W", ".--");
  g_hash_table_insert (ht, "X", "-..-");
  g_hash_table_insert (ht, "Y", "-.--");
  g_hash_table_insert (ht, "Z", "--..");
  g_hash_table_insert (ht, "0", "-----");
  g_hash_table_insert (ht, "1", ".----");
  g_hash_table_insert (ht, "2", "..---");
  g_hash_table_insert (ht, "3", "...--");
  g_hash_table_insert (ht, "4", "....-");
  g_hash_table_insert (ht, "5", ".....");
  g_hash_table_insert (ht, "6", "-....");
  g_hash_table_insert (ht, "7", "--...");
  g_hash_table_insert (ht, "8", "---..");
  g_hash_table_insert (ht, "9", "----.");
  g_hash_table_insert (ht, ",", "--..--");
  g_hash_table_insert (ht, ".", ".-.-.-"); //fullstop
  g_hash_table_insert (ht, "?", "..--.."); //query
  g_hash_table_insert (ht, ";", "-.-.-");
  g_hash_table_insert (ht, ":", "---...");
  g_hash_table_insert (ht, "/", "-..-.");
  g_hash_table_insert (ht, "-", "-....-");
  g_hash_table_insert (ht, "\'", ".----.");
  g_hash_table_insert (ht, "(", "-.--.-");
  g_hash_table_insert (ht, "_", "..--.-");
  g_hash_table_insert (ht,  ")", "-.--.-");
  g_hash_table_insert (ht, "/", "-..-.");
  g_hash_table_insert (ht, "+", ".-.-.");
  g_hash_table_insert (ht, "=", "-...-");
  g_hash_table_insert (ht, "\"", ".-..-.");
  g_hash_table_insert (ht, "$", "...-..-");
  
  gchar *t;
  gint size;     
  GString* gs = g_string_sized_new (strlen (s));
  
  gchar *p = s;
  gunichar u = g_utf8_get_char (p);

  gchar *x;
  gchar *x2;
  
  while (u)
        {
         u = g_unichar_toupper (u);
        
         x = g_malloc (6);
         g_unichar_to_utf8 (u, x);
         x2 = g_strndup (x, 1); 
          
         t = g_hash_table_lookup (ht, x2);    
       
         if (t)
             gs = g_string_append (gs, t);

         gs = g_string_append (gs, " ");

         g_free (x);        
         g_free (x2);        
       
         p = g_utf8_next_char (p);
         u = g_utf8_get_char (p);
        }
  
  p = gs->str;
  g_string_free (gs, FALSE);
  g_hash_table_destroy (ht);

  return p;
}


gchar* morse_decode (gchar *s)
{
  if (! s)
     return NULL;

  gchar** a = g_strsplit (s, " ", -1);
  if (! a)
      return NULL;

  GHashTable *ht = g_hash_table_new (g_str_hash, g_str_equal);

  g_hash_table_insert (ht, ".-", "A");
  g_hash_table_insert (ht, "-...", "B");
  g_hash_table_insert (ht, "-.-.", "C");
  g_hash_table_insert (ht, "-..", "D");
  g_hash_table_insert (ht, ".", "E");
  g_hash_table_insert (ht, "..-.", "F");
  g_hash_table_insert (ht, "--.", "G");
  g_hash_table_insert (ht, "....", "H");
  g_hash_table_insert (ht, "..", "I");
  g_hash_table_insert (ht, ".---", "J");
  g_hash_table_insert (ht, "-.-", "K");
  g_hash_table_insert (ht, ".-..", "L");
  g_hash_table_insert (ht, "--", "M");
  g_hash_table_insert (ht, "-.", "N");
  g_hash_table_insert (ht, "---", "O");
  g_hash_table_insert (ht, ".--.", "P");
  g_hash_table_insert (ht, "--.-", "Q");
  g_hash_table_insert (ht, ".-.", "R");
  g_hash_table_insert (ht, "...", "S");
  g_hash_table_insert (ht, "-", "T");
  g_hash_table_insert (ht, "..-", "U");
  g_hash_table_insert (ht, "...-", "V");
  g_hash_table_insert (ht, ".--", "W");
  g_hash_table_insert (ht, "-..-", "X");
  g_hash_table_insert (ht, "-.--", "Y");
  g_hash_table_insert (ht, "--..", "Z");
  g_hash_table_insert (ht, "-----", "0");
  g_hash_table_insert (ht, ".----", "1");
  g_hash_table_insert (ht, "..---", "2");
  g_hash_table_insert (ht, "...--", "3");
  g_hash_table_insert (ht, "....-", "4");
  g_hash_table_insert (ht, ".....", "5");
  g_hash_table_insert (ht, "-....", "6");
  g_hash_table_insert (ht, "--...", "7");
  g_hash_table_insert (ht, "---..", "8");
  g_hash_table_insert (ht, "----.", "9");
  g_hash_table_insert (ht, "--..--", ",");
  g_hash_table_insert (ht, ".-.-.-", "."); //fullstop
  g_hash_table_insert (ht, "..--..", "?"); //query
  g_hash_table_insert (ht, "-.-.-", ";");
  g_hash_table_insert (ht, "---...", ":");
  g_hash_table_insert (ht, "-..-.", "/");
  g_hash_table_insert (ht, "-....-", "-");
  g_hash_table_insert (ht, ".----.", "'");
  g_hash_table_insert (ht, "-.--.-", ")");
  g_hash_table_insert (ht, "---", "(");
  g_hash_table_insert (ht, "..--.-", "_");
  g_hash_table_insert (ht, "-..-.", "/");
  g_hash_table_insert (ht, ".-.-.", "+");
  g_hash_table_insert (ht, "-...-", "=");
  g_hash_table_insert (ht, ".-..-.", "\"");
  g_hash_table_insert (ht, "...-..-", "$");
  
  GString* gs = g_string_sized_new (strlen (s));
  
  gint c = -1;
  gchar *t;
  gchar *p;

  while (a[++c])
        {
         t = g_hash_table_lookup (ht, a[c]);    

         if (t)
             gs = g_string_append (gs, t);
         else
             gs = g_string_append (gs, " ");
        }
  
  p = gs->str;
  g_string_free (gs, FALSE);
  g_hash_table_destroy (ht);

  g_strfreev (a);   

  return p;
} 


static GList *temp_glist;

static void ht_to_glist_cb (gpointer key,
                            gpointer value,
                            gpointer user_data)
{
  temp_glist = g_list_prepend (temp_glist, g_strconcat (key, "=", value, NULL));
}


static void ht_to_glist_keys_cb (gpointer key,
                                 gpointer value,
                                 gpointer user_data)
{
  temp_glist = g_list_prepend (temp_glist, g_strconcat (key, NULL));
}


GList* ht_to_glist (GHashTable *hash_table, gint option)
{
  glist_strings_free (temp_glist);
  
  if (option == opt_ht_to_glist_full)
     g_hash_table_foreach (hash_table, ht_to_glist_cb, NULL);
  else
      if (option == opt_ht_to_glist_keys)
         g_hash_table_foreach (hash_table, ht_to_glist_keys_cb, NULL);

  return temp_glist;
}


gchar* ch_str (gchar *s, gchar const *new_val)
{
   if (! s)
      return g_strdup (new_val);

   if (new_val)
      {
       g_free (s);
       return g_strdup (new_val);
      }
   else
       return s;
} 


//current music: Radiohead - OK Computer - Karma Police
GList* glist_add_not_dup (GList *l, gchar *s, gint min)
{

	  GList *e = NULL;
	e= g_list_alloc();

  if (! s)
     return l;
  if (s==NULL)
     return l;
  if (min==NULL)
     return l;
  if (l==NULL)
     return l;


  if (g_utf8_strlen (s, -1) <= min || strlen (s) <= min)
     return l;

  GList *p = g_list_first (l);

  while (p)
        {
         if (g_strcmp0 (p->data, s) == 0)
            return l;
         p = g_list_next (p);
        }

  gchar *t = strdup (s);
  t = g_strchomp (t);

	if (t!=NULL && strlen (t) >= min)
	{
		//printf("IF size\n");
		e=g_list_prepend (l, t);
	}
 g_free(p);

	//printf("string %s\n",t);
	if(e!=NULL){/*printf("Return E\n");*/return e;}else{/*printf("Return L\n");*/return l;}

} 


GList* each_line_remove_lt_spaces (GList *lines, gint mode)
{
  GList *l = NULL;
  GList *t = g_list_first (lines);
  while (t)
       {
        if (t->data)
          {
           if (strlen ((gchar*) t->data) <= 1)
              l = g_list_prepend (l, g_strdup ((gchar*) t->data));
           else
              {
               if (mode == 1)   
                  l = g_list_prepend (l, g_strchomp (g_strdup ((gchar*) t->data)));
               else
                   if (mode == 0)   
                      l = g_list_prepend (l, g_strchug (g_strdup ((gchar*) t->data)));
              }
          } 
        t = g_list_next (t);
       }

  return g_list_reverse (l);
}


gint glist_get_str_index (GList *l, gchar *s)
{
  gint c = 0;
  GList *t = g_list_first (l);
  while (t)
       {
        if (g_utf8_collate (t->data, s) == 0)        
           return c;

        ++c;
        t = g_list_next (t);
       }
   return 0;
} 


gchar* get_c_source_fname (gchar *f)
{
  if (! f)
     return NULL; 

  gchar *t = NULL;

  if (g_str_has_suffix (f, ".h")) 
     {
      t = g_str_replace (f, ".h", ".cpp");
      if (g_file_test (t, G_FILE_TEST_EXISTS))
          return t;

      g_free (t);     
      t = g_str_replace (f, ".h", ".c");
      if (g_file_test (t, G_FILE_TEST_EXISTS))
          return t;
     } 

  g_free (t);
  return NULL; 
}


gchar* get_c_header_fname (gchar *f)
{
  gchar *t = NULL;

  if (g_str_has_suffix (f, ".cpp")) 
     t = g_str_replace (f, ".cpp", ".h");
  else
      if (g_str_has_suffix (f, ".c")) 
         t = g_str_replace (f, ".c", ".h");

  if (g_file_test (t, G_FILE_TEST_EXISTS))
     return t;

  g_free (t);
  return NULL; 
}


gchar* ht_get_gchar_value (GHashTable *hash_table, gchar *key, gchar *def)
{
  if (! hash_table)
      return g_strdup (def);
 
  gchar *v = g_hash_table_lookup (hash_table, key);
  if (v)
     return g_strdup (v);
 
  return strdup (def);
}  


gchar* str_kill_dups (const gchar *s)
{
  GList *l = glist_from_string (s);
  GList *t = g_list_first (l);
  GList *x = NULL;

  while (t)
        {
         if (! str_in_glist (x, t->data))
            x = g_list_prepend (x, g_strdup (t->data));             

         t = g_list_next (t);
        }

  gchar *result = string_from_glist (g_list_reverse (x));

  glist_strings_free (x);
  glist_strings_free (l);
  return result;
}
