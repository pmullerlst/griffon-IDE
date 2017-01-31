
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
#include <gtk/gtk.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <string.h>
#include "griffon_text_document.h"
#include "griffon_hl.h"
#include "griffon_config.h"
#include "griffon_defs.h"
#include "interface.h"
#include "griffon_funx.h"
#include "rox_strings.h"

static GList *l_words;

void str_walk_extract_word (gpointer key)
{
  l_words = g_list_prepend (l_words, key);
}

void walk_by_words (gpointer value)
{
  l_words = g_list_prepend (l_words, value);
}

void run_unitaz ( t_note_page *page, gint sort_type, gboolean case_insensetive)
{
  GList *list = NULL;
  GtkTextIter a;

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

  if (doc_is_sel (GTK_TEXT_BUFFER(page->text_buffer)))
     gtk_text_buffer_get_selection_bounds (GTK_TEXT_BUFFER(page->text_buffer), &start, &end);
  else
      {
       gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(page->text_buffer), &start); 
       gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(page->text_buffer), &end); 
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
             z = gtk_text_buffer_get_text (GTK_TEXT_BUFFER(page->text_buffer),
                                           &a,
                                           &start,
                                           FALSE);
             s = g_utf8_strdown (z, -1);
             g_free (z);
            }
         else
             s = gtk_text_buffer_get_text (GTK_TEXT_BUFFER(page->text_buffer),
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
    
  gchar *st = NULL;
             
  g_hash_table_foreach (words, (GHFunc)walk_by_words, NULL);
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
   
  gtk_text_buffer_insert_at_cursor (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), sr, -1);
  g_free (sr);
  glist_strings_free (list);
  g_list_free (l_words);
  g_hash_table_destroy (words); 
}


void run_extract_words ( t_note_page *page)
{
  GList *list = NULL;
  GtkTextIter a;
  
  l_words = NULL;
  GHashTable *words = g_hash_table_new (g_str_hash, g_str_equal);
                                         
  gchar *s;
  gchar *t;
  GtkTextIter start;
  GtkTextIter end;

  if (doc_is_sel (GTK_TEXT_BUFFER(page->text_buffer)))
     gtk_text_buffer_get_selection_bounds (GTK_TEXT_BUFFER(page->text_buffer), &start, &end);
  else
      {
       gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(page->text_buffer), &start); 
       gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(page->text_buffer), &end); 
      }  

  do 
    {
     if (gtk_text_iter_starts_word (&start))
        a = start; 
     
     if (gtk_text_iter_ends_word (&start))
        {
         s = gtk_text_buffer_get_text (GTK_TEXT_BUFFER(page->text_buffer),
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
             
  g_hash_table_foreach (words, (GHFunc)str_walk_extract_word, NULL);
  
  GList *tmplist = g_list_first (g_list_reverse (l_words));
  while (tmplist)
        {
         list = g_list_prepend (list, tmplist->data);
         tmplist = g_list_next (tmplist);
        }

  cur_text_doc = doc_clear_new ();
  
  gchar *sr = string_from_glist (g_list_reverse (list));
   
  gtk_text_buffer_insert_at_cursor (GTK_TEXT_BUFFER(cur_text_doc->text_buffer), sr, -1);
  g_free (sr);
  glist_strings_free (list);
  g_list_free (l_words);
  g_hash_table_destroy (words); 
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


GList* add_recent_item_composed (GList *list, t_note_page *doc)
{
  if (! doc) return list;

  GList *l = list;
 
  l = add_to_list_with_limit2 (list, g_strdup_printf ("%s,%s,%d",
                               doc->file_name, doc->encoding, 
                               editor_get_pos (doc)),
                               confile.max_recent_items);
  return l;
}


void add_recent_internal (t_note_page *doc)
{
  recent_list = add_recent_item_composed (recent_list, doc);
}


void assign_tags (t_note_page *doc)
{
  GtkTextTagTable *t = gtk_text_buffer_get_tag_table (GTK_TEXT_BUFFER(doc->text_buffer));

  gtk_text_tag_table_add (t, tag_comment);
  gtk_text_tag_table_add (t, tag_identifier);
  gtk_text_tag_table_add (t, tag_digit);
  gtk_text_tag_table_add (t, tag_string);
  gtk_text_tag_table_add (t, tag_html_tag);
  gtk_text_tag_table_add (t, tag_preprocessor);
  gtk_text_tag_table_add (t, tag_type);
}


void create_tags (void)
{
  tag_comment = gtk_text_tag_new ("comment");
  tag_identifier = gtk_text_tag_new ("identifier");
  tag_digit = gtk_text_tag_new ("digit");
  tag_string = gtk_text_tag_new ("string");
  tag_html_tag = gtk_text_tag_new ("html_tag");
  tag_preprocessor = gtk_text_tag_new ("preprocessor");  
  tag_type = gtk_text_tag_new ("type");  
  tag_spell_err = gtk_text_tag_new ("spell_err");  

  g_object_set (G_OBJECT (tag_spell_err), "foreground", confile.tag_spellcheck, NULL);
  g_object_set (G_OBJECT (tag_spell_err), "underline", PANGO_UNDERLINE_LOW, NULL);
  g_object_set (G_OBJECT (tag_comment), "foreground", confile.tag_comment, "font", confile.tag_comment_font, NULL);
  g_object_set (G_OBJECT (tag_identifier), "foreground", confile.tag_identifier, "font", confile.tag_identifier_font, NULL);
  g_object_set (G_OBJECT (tag_digit), "foreground", confile.tag_digit, "font", confile.tag_digit_font, NULL);
  g_object_set (G_OBJECT (tag_string), "foreground", confile.tag_string, "font", confile.tag_string_font, NULL);
  g_object_set (G_OBJECT (tag_html_tag), "foreground", confile.tag_html_tag, "font", confile.tag_html_tag_font, NULL);
  g_object_set (G_OBJECT (tag_preprocessor), "foreground", confile.tag_preprocessor, "font", confile.tag_preprocessor_font, NULL);
  g_object_set (G_OBJECT (tag_type), "foreground", confile.tag_type, "font", confile.tag_type_font, NULL);

  main_tags_table = gtk_text_tag_table_new ();
  
  gtk_text_tag_table_add (main_tags_table, tag_spell_err);
  gtk_text_tag_table_add (main_tags_table, tag_comment);
  gtk_text_tag_table_add (main_tags_table, tag_identifier);
  gtk_text_tag_table_add (main_tags_table, tag_digit);
  gtk_text_tag_table_add (main_tags_table, tag_string);
  gtk_text_tag_table_add (main_tags_table, tag_html_tag);
  gtk_text_tag_table_add (main_tags_table, tag_preprocessor);
  gtk_text_tag_table_add (main_tags_table, tag_type);
}


void remove_tags (t_note_page *doc)
{
  GtkTextIter itstart, itend;
  gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(doc->text_buffer), &itstart, 0);
  gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(doc->text_buffer), &itend, gtk_text_buffer_get_char_count (GTK_TEXT_BUFFER(doc->text_buffer)));
}                   


void clear_remove_tags (t_note_page *doc)
{
  GtkTextIter itstart, itend;
  gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(doc->text_buffer), &itstart, 0);
  gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(doc->text_buffer), &itend, gtk_text_buffer_get_char_count (GTK_TEXT_BUFFER(doc->text_buffer)));
  gtk_text_buffer_remove_all_tags (GTK_TEXT_BUFFER(doc->text_buffer), &itstart, &itend);
  apply_hl (doc);
} 

void apply_html_hl (t_note_page *doc)
{
  gchar *text = doc_get_buf (GTK_TEXT_BUFFER(doc->text_buffer));
  if (! text)
     return;

  remove_tags (doc);

  GtkTextIter it_start;
  GtkTextIter it_end;
  gint i = -1;

  gchar *p = text;
  gunichar u = g_utf8_get_char (p);

  while ( u )
        {
         i++;
         
         if (u == 60) //<
            gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(doc->text_buffer), &it_start, i);

         if (u == 62) //>
            {
             gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(doc->text_buffer), &it_end, i+1);
             gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_html_tag, &it_start, &it_end);
            }
         
         p = g_utf8_next_char(p);
         u = g_utf8_get_char (p);
        }

  g_free (text);
}


void prepare_hl_py (void)
{
  py_t_keywords = g_hash_table_new (g_str_hash, g_str_equal);
  
  g_hash_table_insert (py_t_keywords, "and", "and");
  g_hash_table_insert (py_t_keywords, "as", "as");
  g_hash_table_insert (py_t_keywords, "assert", "assert");
  g_hash_table_insert (py_t_keywords, "break", "break");
  g_hash_table_insert (py_t_keywords, "class", "class");
  g_hash_table_insert (py_t_keywords, "continue", "continue");
  g_hash_table_insert (py_t_keywords, "def", "def");
  g_hash_table_insert (py_t_keywords, "del", "del");
  g_hash_table_insert (py_t_keywords, "elif", "elif");
  g_hash_table_insert (py_t_keywords, "else", "else");
  g_hash_table_insert (py_t_keywords, "except", "except");
  g_hash_table_insert (py_t_keywords, "exec", "exec");
  g_hash_table_insert (py_t_keywords, "finally", "finally");
  g_hash_table_insert (py_t_keywords, "for", "for");
  g_hash_table_insert (py_t_keywords, "from", "from");
  g_hash_table_insert (py_t_keywords, "global", "global");
  g_hash_table_insert (py_t_keywords, "global", "global");
  g_hash_table_insert (py_t_keywords, "if", "if");
  g_hash_table_insert (py_t_keywords, "import", "import");
  g_hash_table_insert (py_t_keywords, "in", "in");
  g_hash_table_insert (py_t_keywords, "is", "is");
  g_hash_table_insert (py_t_keywords, "lambda", "lambda");
  g_hash_table_insert (py_t_keywords, "None", "None");
  g_hash_table_insert (py_t_keywords, "not", "not");
  g_hash_table_insert (py_t_keywords, "or", "or");
  g_hash_table_insert (py_t_keywords, "pass", "pass");
  g_hash_table_insert (py_t_keywords, "pass", "pass");
  g_hash_table_insert (py_t_keywords, "print", "print");
  g_hash_table_insert (py_t_keywords, "raise", "raise");
  g_hash_table_insert (py_t_keywords, "return", "return");
  g_hash_table_insert (py_t_keywords, "try", "try");
  g_hash_table_insert (py_t_keywords, "while", "while");
  g_hash_table_insert (py_t_keywords, "yield", "yield");  
}


void prepare_hl_bash (void)
{
  bash_t_keywords = g_hash_table_new (g_str_hash, g_str_equal);
  
  g_hash_table_insert (bash_t_keywords, "case", "case");
  g_hash_table_insert (bash_t_keywords, "do", "do");
  g_hash_table_insert (bash_t_keywords, "done", "done");
  g_hash_table_insert (bash_t_keywords, "elif", "elif");
  g_hash_table_insert (bash_t_keywords, "else", "else");
  g_hash_table_insert (bash_t_keywords, "esac", "esac");
  g_hash_table_insert (bash_t_keywords, "fi", "fi");
  g_hash_table_insert (bash_t_keywords, "for", "for");
  g_hash_table_insert (bash_t_keywords, "function", "function");
  g_hash_table_insert (bash_t_keywords, "if", "if");
  g_hash_table_insert (bash_t_keywords, "in", "in");
  g_hash_table_insert (bash_t_keywords, "select", "select");
  g_hash_table_insert (bash_t_keywords, "then", "then");
  g_hash_table_insert (bash_t_keywords, "until", "until");
  g_hash_table_insert (bash_t_keywords, "while", "while");
  g_hash_table_insert (bash_t_keywords, "time", "time");
}


void prepare_hl_po (void)
{
  po_t_keywords = g_hash_table_new (g_str_hash, g_str_equal);
  g_hash_table_insert (po_t_keywords, "msgid", "msgid");
  g_hash_table_insert (po_t_keywords, "msgstr", "msgstr");
}


void prepare_hl_c (void)
{
  c_t_keywords = g_hash_table_new ( g_str_hash, g_str_equal);
  
  g_hash_table_insert (c_t_keywords, "asm", "asm");
  g_hash_table_insert (c_t_keywords, "break", "break");
  g_hash_table_insert (c_t_keywords, "case", "case");
  g_hash_table_insert (c_t_keywords, "continue", "continue");
  g_hash_table_insert (c_t_keywords, "default", "default");
  g_hash_table_insert (c_t_keywords, "delete", "delete");
  g_hash_table_insert (c_t_keywords, "do", "do");
  g_hash_table_insert (c_t_keywords, "else", "else");
  g_hash_table_insert (c_t_keywords, "for", "for");
  g_hash_table_insert (c_t_keywords, "goto", "goto");
  g_hash_table_insert (c_t_keywords, "if", "if");
  g_hash_table_insert (c_t_keywords, "new", "new");
  g_hash_table_insert (c_t_keywords, "return", "return");
  g_hash_table_insert (c_t_keywords, "sizeof", "sizeof");
  g_hash_table_insert (c_t_keywords, "switch", "switch");
  g_hash_table_insert (c_t_keywords, "template", "template");
  g_hash_table_insert (c_t_keywords, "this", "this");
  g_hash_table_insert (c_t_keywords, "typedef", "typedef");
  g_hash_table_insert (c_t_keywords, "typeof", "typeof");
  g_hash_table_insert (c_t_keywords, "while", "while");

  c_t_types = g_hash_table_new ( g_str_hash, g_str_equal);

  g_hash_table_insert (c_t_types, "char", "char");
  g_hash_table_insert (c_t_types, "class", "class");
  g_hash_table_insert (c_t_types, "const", "const");
  g_hash_table_insert (c_t_types, "double", "double");
  g_hash_table_insert (c_t_types, "extern", "extern");
  g_hash_table_insert (c_t_types, "float", "float");
  g_hash_table_insert (c_t_types, "friend", "friend");
  g_hash_table_insert (c_t_types, "inline", "inline");
  g_hash_table_insert (c_t_types, "int", "int");
  g_hash_table_insert (c_t_types, "long", "long");
  g_hash_table_insert (c_t_types, "operator", "operator");
  g_hash_table_insert (c_t_types, "overload", "overload");
  g_hash_table_insert (c_t_types, "public", "public");
  g_hash_table_insert (c_t_types, "register", "register");
  g_hash_table_insert (c_t_types, "short", "short");
  g_hash_table_insert (c_t_types, "signed", "signed");
  g_hash_table_insert (c_t_types, "static", "static");
  g_hash_table_insert (c_t_types, "struct", "struct");    
  g_hash_table_insert (c_t_types, "union", "union");
  g_hash_table_insert (c_t_types, "unsigned", "unsigned");
  g_hash_table_insert (c_t_types, "vector", "vector");
  g_hash_table_insert (c_t_types, "virtual", "virtual");
  g_hash_table_insert (c_t_types, "void", "void");
  g_hash_table_insert (c_t_types, "volatile", "volatile");  
}

void prepare_hl_php (void)
{
  php_t_keywords = g_hash_table_new ( g_str_hash, g_str_equal);
  
  g_hash_table_insert (php_t_keywords, "and", "and");
  g_hash_table_insert (php_t_keywords, "or", "or");
  g_hash_table_insert (php_t_keywords, "xor", "xor");
  g_hash_table_insert (php_t_keywords, "__FILE__", "__FILE__");
  g_hash_table_insert (php_t_keywords, "exception", "exception");
  g_hash_table_insert (php_t_keywords, "php_user_filter", "php_user_filter");
  g_hash_table_insert (php_t_keywords, "__LINE__", "__LINE__");
  g_hash_table_insert (php_t_keywords, "array", "array");
  g_hash_table_insert (php_t_keywords, "as", "as");
  g_hash_table_insert (php_t_keywords, "break", "break");
  g_hash_table_insert (php_t_keywords, "case", "case");
  g_hash_table_insert (php_t_keywords, "cfunction", "cfunction");
  g_hash_table_insert (php_t_keywords, "class", "class");
  g_hash_table_insert (php_t_keywords, "const", "const");
  g_hash_table_insert (php_t_keywords, "continue", "continue");
  g_hash_table_insert (php_t_keywords, "declare", "declare");
  g_hash_table_insert (php_t_keywords, "default", "default");
  g_hash_table_insert (php_t_keywords, "die", "die");
  g_hash_table_insert (php_t_keywords, "do", "do");
  g_hash_table_insert (php_t_keywords, "echo", "echo");
  g_hash_table_insert (php_t_keywords, "else", "else");
  g_hash_table_insert (php_t_keywords, "elseif", "elseif");
  g_hash_table_insert (php_t_keywords, "empty", "empty");
  g_hash_table_insert (php_t_keywords, "enddeclare", "enddeclare");
  g_hash_table_insert (php_t_keywords, "endfor", "endfor");
  g_hash_table_insert (php_t_keywords, "endforeach", "endforeach");
  g_hash_table_insert (php_t_keywords, "endif", "endif");
  g_hash_table_insert (php_t_keywords, "endswitch", "endswitch");
  g_hash_table_insert (php_t_keywords, "endwhile", "endwhile");
  g_hash_table_insert (php_t_keywords, "eval", "eval");
  g_hash_table_insert (php_t_keywords, "exit", "exit");
  g_hash_table_insert (php_t_keywords, "extends", "extends");
  g_hash_table_insert (php_t_keywords, "for", "for");
  g_hash_table_insert (php_t_keywords, "foreach", "foreach");
  g_hash_table_insert (php_t_keywords, "function", "function");
  g_hash_table_insert (php_t_keywords, "global", "global");
  g_hash_table_insert (php_t_keywords, "if", "if");
  g_hash_table_insert (php_t_keywords, "include", "include");
  g_hash_table_insert (php_t_keywords, "include_once", "include_once");
  g_hash_table_insert (php_t_keywords, "isset", "isset");
  g_hash_table_insert (php_t_keywords, "list", "list");
  g_hash_table_insert (php_t_keywords, "new", "new");
  g_hash_table_insert (php_t_keywords, "old_function", "old_function");
  g_hash_table_insert (php_t_keywords, "print", "print");
  g_hash_table_insert (php_t_keywords, "require", "require");
  g_hash_table_insert (php_t_keywords, "require_once", "require_once");
  g_hash_table_insert (php_t_keywords, "return", "return");
  g_hash_table_insert (php_t_keywords, "static", "static");
  g_hash_table_insert (php_t_keywords, "switch", "switch");
  g_hash_table_insert (php_t_keywords, "use", "use");
  g_hash_table_insert (php_t_keywords, "unset", "unset");
  g_hash_table_insert (php_t_keywords, "var", "var");
  g_hash_table_insert (php_t_keywords, "while", "while");
  g_hash_table_insert (php_t_keywords, "__FUNCTION__", "__FUNCTION__");
  g_hash_table_insert (php_t_keywords, "__CLASS__", "__CLASS__");
  g_hash_table_insert (php_t_keywords, "__METHOD_", "__METHOD__");
}

void prepare_hl_pas (void)
{
  pas_t_keywords = g_hash_table_new ( g_str_hash, g_str_equal);

  g_hash_table_insert (pas_t_keywords, "absolute", "absolute");
  g_hash_table_insert (pas_t_keywords, "abstract", "abstract");
  g_hash_table_insert (pas_t_keywords, "and", "and");
  g_hash_table_insert (pas_t_keywords, "array", "array");
  g_hash_table_insert (pas_t_keywords, "as", "as");
  g_hash_table_insert (pas_t_keywords, "asm", "asm");
  g_hash_table_insert (pas_t_keywords, "assembler", "assembler");
  g_hash_table_insert (pas_t_keywords, "assembler", "assembler");
  g_hash_table_insert (pas_t_keywords, "begin", "begin");
  g_hash_table_insert (pas_t_keywords, "break", "break");
  g_hash_table_insert (pas_t_keywords, "case", "case");
  g_hash_table_insert (pas_t_keywords, "class", "class");
  g_hash_table_insert (pas_t_keywords, "const", "const");
  g_hash_table_insert (pas_t_keywords, "constructor", "constructor");
  g_hash_table_insert (pas_t_keywords, "continue", "continue");
  g_hash_table_insert (pas_t_keywords, "destructor", "destructor");
  g_hash_table_insert (pas_t_keywords, "dispose", "dispose");
  g_hash_table_insert (pas_t_keywords, "div", "div");
  g_hash_table_insert (pas_t_keywords, "do", "do");
  g_hash_table_insert (pas_t_keywords, "downto", "downto");
  g_hash_table_insert (pas_t_keywords, "else", "else");
  g_hash_table_insert (pas_t_keywords, "end", "end");
  g_hash_table_insert (pas_t_keywords, "except", "except");
  g_hash_table_insert (pas_t_keywords, "exit", "exit");
  g_hash_table_insert (pas_t_keywords, "export", "export");
  g_hash_table_insert (pas_t_keywords, "exports", "exports");
  g_hash_table_insert (pas_t_keywords, "false", "false");
  g_hash_table_insert (pas_t_keywords, "finalization", "finalization");
  g_hash_table_insert (pas_t_keywords, "finally", "finally");
  g_hash_table_insert (pas_t_keywords, "for", "for");
  g_hash_table_insert (pas_t_keywords, "forward", "forward");
  g_hash_table_insert (pas_t_keywords, "function", "function");
  g_hash_table_insert (pas_t_keywords, "if", "if");
  g_hash_table_insert (pas_t_keywords, "implementation", "implementation");
  g_hash_table_insert (pas_t_keywords, "import", "import");
  g_hash_table_insert (pas_t_keywords, "index", "index");
  g_hash_table_insert (pas_t_keywords, "inherited", "inherited");
  g_hash_table_insert (pas_t_keywords, "initialization", "initialization");
  g_hash_table_insert (pas_t_keywords, "inline", "inline");
  g_hash_table_insert (pas_t_keywords, "interface", "interface");
  g_hash_table_insert (pas_t_keywords, "interrupt", "interrupt");
  g_hash_table_insert (pas_t_keywords, "is", "is");
  g_hash_table_insert (pas_t_keywords, "label", "label");
  g_hash_table_insert (pas_t_keywords, "library", "library");
  g_hash_table_insert (pas_t_keywords, "mod", "mod");
  g_hash_table_insert (pas_t_keywords, "near", "near");
  g_hash_table_insert (pas_t_keywords, "new", "new");
  g_hash_table_insert (pas_t_keywords, "nil", "nil");
  g_hash_table_insert (pas_t_keywords, "not", "not");
  g_hash_table_insert (pas_t_keywords, "object", "object");
  g_hash_table_insert (pas_t_keywords, "of", "of");
  g_hash_table_insert (pas_t_keywords, "on", "on");
  g_hash_table_insert (pas_t_keywords, "operator", "operator");
  g_hash_table_insert (pas_t_keywords, "or", "or");
  g_hash_table_insert (pas_t_keywords, "otherwise", "otherwise");
  g_hash_table_insert (pas_t_keywords, "overload", "overload");
  g_hash_table_insert (pas_t_keywords, "override", "override");
  g_hash_table_insert (pas_t_keywords, "packed", "packed");
  g_hash_table_insert (pas_t_keywords, "private", "private");
  g_hash_table_insert (pas_t_keywords, "procedure", "procedure");
  g_hash_table_insert (pas_t_keywords, "program", "program");
  g_hash_table_insert (pas_t_keywords, "property", "property");
  g_hash_table_insert (pas_t_keywords, "protected", "protected");
  g_hash_table_insert (pas_t_keywords, "public", "public");
  g_hash_table_insert (pas_t_keywords, "raise", "raise");
  g_hash_table_insert (pas_t_keywords, "record", "record");
  g_hash_table_insert (pas_t_keywords, "repeat", "repeat");
  g_hash_table_insert (pas_t_keywords, "resourcestring", "resourcestring");
  g_hash_table_insert (pas_t_keywords, "select", "select");
  g_hash_table_insert (pas_t_keywords, "self", "self");
  g_hash_table_insert (pas_t_keywords, "set", "set");
  g_hash_table_insert (pas_t_keywords, "shl", "shl");
  g_hash_table_insert (pas_t_keywords, "shr", "shr");
  g_hash_table_insert (pas_t_keywords, "sizeof", "sizeof");
  g_hash_table_insert (pas_t_keywords, "then", "then");
  g_hash_table_insert (pas_t_keywords, "to", "to");
  g_hash_table_insert (pas_t_keywords, "true", "true");
  g_hash_table_insert (pas_t_keywords, "try", "try");
  g_hash_table_insert (pas_t_keywords, "type", "type");
  g_hash_table_insert (pas_t_keywords, "unit", "unit");
  g_hash_table_insert (pas_t_keywords, "until", "until");
  g_hash_table_insert (pas_t_keywords, "uses", "uses");
  g_hash_table_insert (pas_t_keywords, "var", "var");
  g_hash_table_insert (pas_t_keywords, "virtual", "virtual");
  g_hash_table_insert (pas_t_keywords, "while", "while");
  g_hash_table_insert (pas_t_keywords, "with", "with");
  g_hash_table_insert (pas_t_keywords, "xor", "xor");

  pas_t_types = g_hash_table_new ( g_str_hash, g_str_equal);

  g_hash_table_insert (pas_t_types, "boolean", "boolean");
  g_hash_table_insert (pas_t_types, "cdecl", "cdecl");
  g_hash_table_insert (pas_t_types, "char", "char");
  g_hash_table_insert (pas_t_types, "external", "external");
  g_hash_table_insert (pas_t_types, "far", "far");
  g_hash_table_insert (pas_t_types, "file", "file");
  g_hash_table_insert (pas_t_types, "integer", "integer");
  g_hash_table_insert (pas_t_types, "longint", "longint");
  g_hash_table_insert (pas_t_types, "name", "name");
  g_hash_table_insert (pas_t_types, "near", "near");
  g_hash_table_insert (pas_t_types, "pointer", "pointer");
  g_hash_table_insert (pas_t_types, "stdcall", "stdcall");
  g_hash_table_insert (pas_t_types, "string", "string");
  g_hash_table_insert (pas_t_types, "word", "word");
}


gboolean find_slash (gunichar ch)
{
  if (ch == '/')
     return TRUE;
  else
      return FALSE;
}


gboolean find_q (gunichar ch)
{
  if (ch == '"')
     return TRUE;
  else
      return FALSE;
}


gboolean find_q2 (gunichar ch)
{
  if (ch == '\'')
     return TRUE;
  else
      return FALSE;
}


gboolean find_ast (gunichar ch)
{
  if (ch == '*')
     return TRUE;
  else
      return FALSE;
}


gboolean is_word_number (gchar *s)
{
   return (g_unichar_isdigit (g_utf8_get_char (s) ));
}


void do_hl_php (t_note_page *doc)
{
	auto_hl_griffon_perl ();
   GtkTextIter iter;
   GtkTextIter a;
   GtkTextIter b;
   GtkTextIter c;

   remove_tags (doc);

   gchar *text;

   gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(doc->text_buffer), &iter, 0);

   if (gtk_text_iter_starts_word (&iter))
      {
       a = iter;
       c = iter;
      }

   do
     {
      if (gtk_text_iter_starts_word (&iter))
         {  
          b = iter;
          if (gtk_text_iter_backward_char (&b))
             if (gtk_text_iter_get_char (&b) != '_')
                a = iter;
              
          if (gtk_text_iter_forward_word_end (&iter))
             if (gtk_text_iter_ends_word (&iter))
                {
                 if (gtk_text_iter_get_char (&iter) != '_')
                    {
                     text = gtk_text_iter_get_slice (&a, &iter);
                     if (text)
                        {
                         if (g_unichar_isdigit (g_utf8_get_char (text))) 
                            {
                             gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_digit, &a, &iter);
                             g_free (text);
                             continue;
                            }

                         if (g_hash_table_lookup (php_t_keywords, text))  
                            {
                             gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_identifier, &a, &iter);
                             g_free (text);
                             continue;
                            }
                        }
                }
              }
          }
        }

  while ( gtk_text_iter_forward_char (&iter));

  gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(doc->text_buffer), &iter, 0);
  a = iter;
  c = iter;
 
  do
    {
  
     a = iter;
        
     switch (gtk_text_iter_get_char (&iter))
            {
             case '#':  
                      {
                       a = iter;
                       if (gtk_text_iter_forward_line (&iter))
                          if (gtk_text_iter_backward_char (&iter)) 
                             { 
                              gtk_text_buffer_remove_all_tags (GTK_TEXT_BUFFER(doc->text_buffer), &a, &iter);
                              gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_comment, &a, &iter);
                             }
                       break;
                      }

             case '/':
                      {
                       b = iter;
                       if (gtk_text_iter_forward_char (&b))
															{
                          if (gtk_text_iter_get_char (&b) == '*')
                             {
                              c = iter;
                              b = c;
                        
                              do 
                                { 
                                 if (gtk_text_iter_forward_find_char (&iter, (GtkTextCharPredicate)find_slash, NULL, NULL))
                                    {
                                     b = iter;
                                     if (gtk_text_iter_backward_char (&b))
                                        if (gtk_text_iter_get_char (&b) == '*')
                                           {
                                            gtk_text_buffer_remove_all_tags (GTK_TEXT_BUFFER(doc->text_buffer), &c, &iter);                
                                            gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_comment, &c, &iter);
                                           }
                                     }          
                                 } 
                               while ( gtk_text_iter_get_char (&b) != '*' );
                              }
                          else      
                              {
                               b = iter;
                               if (gtk_text_iter_forward_char (&b))
                                  if (gtk_text_iter_get_char (&b) == '/')
                                     {
                                      a = iter;
                                      gtk_text_iter_forward_line (&iter);
                                      gtk_text_buffer_remove_all_tags (GTK_TEXT_BUFFER(doc->text_buffer), &a, &iter);                
                                      gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_comment, &a, &iter);
                                     }
                              }
                       break;   
                      }
														}  

             case '"': 
                      {
                       if (gtk_text_iter_forward_find_char (&iter, (GtkTextCharPredicate)find_q, NULL, NULL))
                          if (gtk_text_iter_forward_char (&iter)) 
                             gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_string, &a, &iter);
                       break;
                      }
 
             case '\'':             
                       {
                        if (gtk_text_iter_forward_find_char (&iter, (GtkTextCharPredicate)find_q2, NULL, NULL))
                           if (gtk_text_iter_forward_char (&iter))
                              gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_string, &a, &iter);
                        break;
                       }  
        }        
    }

    while ( gtk_text_iter_forward_char (&iter) );
}


void do_hl_bash (t_note_page *doc)
{

   GtkTextIter iter;
   GtkTextIter a;
   GtkTextIter b;

   remove_tags (doc);

   gchar *text;

   gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(doc->text_buffer), &iter, 0);

   if (gtk_text_iter_starts_word (&iter))
      {
       a = iter;
      }

   do
     {
      if (gtk_text_iter_starts_word (&iter))
         {  
          b = iter;
          if (gtk_text_iter_backward_char (&b))
             if (gtk_text_iter_get_char (&b) != '_')
                a = iter;
              
          if (gtk_text_iter_forward_word_end (&iter))
             if (gtk_text_iter_ends_word (&iter))
                {
                 if (gtk_text_iter_get_char (&iter) != '_')
                    {
                     text = gtk_text_iter_get_slice (&a, &iter);
                     if (text)
                        {
                         if (g_unichar_isdigit (g_utf8_get_char (text))) 
                            {
                             gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_digit, &a, &iter);
                             g_free (text);
                             continue;
                            }

                         if (g_hash_table_lookup (bash_t_keywords, text))  
                            {
                             gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_identifier, &a, &iter);
                             g_free (text);
                             continue;
                            }

                        }
                }
              }
          }
        }

  while ( gtk_text_iter_forward_char (&iter));

  gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(doc->text_buffer), &iter, 0);
  a = iter;
 
  do
    {
     a = iter;
        
     switch (gtk_text_iter_get_char (&iter))
            {
             case '#':  
                     {
                      a = iter;
                      if (gtk_text_iter_forward_line (&iter))
                         if (gtk_text_iter_backward_char (&iter)) 
                            { 
                             gtk_text_buffer_remove_all_tags (GTK_TEXT_BUFFER(doc->text_buffer), &a, &iter);
                             gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_comment, &a, &iter);
                            }
                      break;
                     }

             case '"': 
                      {
                       if (gtk_text_iter_forward_find_char (&iter, (GtkTextCharPredicate)find_q, NULL, NULL))
                          if (gtk_text_iter_forward_char (&iter)) 
                            {
                             gtk_text_buffer_remove_all_tags (GTK_TEXT_BUFFER(doc->text_buffer), &a, &iter);
                             gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_string, &a, &iter);
                            }
                       break;
                      }
 
             case '\'':             
                       {
                        if (gtk_text_iter_forward_find_char (&iter, (GtkTextCharPredicate)find_q2, NULL, NULL))
                           if (gtk_text_iter_forward_char (&iter))
                              {
                               gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_string, &a, &iter);
                               gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_string, &a, &iter);
                              } 
                        break;
                       }  

        }        
    }

    while ( gtk_text_iter_forward_char (&iter) );
}


void do_hl_po (t_note_page *doc)
{
	auto_hl_griffon_po ();

   GtkTextIter iter;
   GtkTextIter a;
   GtkTextIter b;

   remove_tags (doc);

   gchar *text;

   gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(doc->text_buffer), &iter, 0);

   if (gtk_text_iter_starts_word (&iter))
      {
       a = iter;
      }

   do
     {
      if (gtk_text_iter_starts_word (&iter))
         {  
          b = iter;
          if (gtk_text_iter_backward_char (&b))
             if (gtk_text_iter_get_char (&b) != '_')
                a = iter;
              
          if (gtk_text_iter_forward_word_end (&iter))
             if (gtk_text_iter_ends_word (&iter))
                {
                 if (gtk_text_iter_get_char (&iter) != '_')
                    {
                     text = gtk_text_iter_get_slice (&a, &iter);
                     if (text)
                        {
                         if (g_unichar_isdigit (g_utf8_get_char (text))) 
                            {
                             gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_digit, &a, &iter);
                             g_free (text);
                             continue;
                            }

                         if (g_hash_table_lookup (po_t_keywords, text))  
                            {
                             gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_identifier, &a, &iter);
                             g_free (text);
                             continue;
                            }

                        }
                }
              }
          }
        }

  while ( gtk_text_iter_forward_char (&iter));

  gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(doc->text_buffer), &iter, 0);
  a = iter;
 
  do
    {
     a = iter;
        
     switch (gtk_text_iter_get_char (&iter))
            {
             case '#':  
                      {
                       a = iter;
                       if (gtk_text_iter_forward_line (&iter))
                          if (gtk_text_iter_backward_char (&iter)) 
                             { 
                              gtk_text_buffer_remove_all_tags (GTK_TEXT_BUFFER(doc->text_buffer), &a, &iter);
                              gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_comment, &a, &iter);
                             }
                       break;
                      }

             case '"': 
                      {
                       if (gtk_text_iter_forward_find_char (&iter,(GtkTextCharPredicate) find_q, NULL, NULL))
                          if (gtk_text_iter_forward_char (&iter)) 
                            {
                             gtk_text_buffer_remove_all_tags (GTK_TEXT_BUFFER(doc->text_buffer), &a, &iter);
                             gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_string, &a, &iter);
                            }
                       break;
                      }
 
             case '\'':             
                       {
                        if (gtk_text_iter_forward_find_char (&iter, (GtkTextCharPredicate)find_q2, NULL, NULL))
                           if (gtk_text_iter_forward_char (&iter))
                              {
                               gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_string, &a, &iter);
                               gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_string, &a, &iter);
                              } 
                        break;
                       }  
        }        
    }

    while ( gtk_text_iter_forward_char (&iter) );
}



void do_hl_pascal (t_note_page *doc)
{
   GtkTextIter iter;
   GtkTextIter a;
   GtkTextIter b;

   remove_tags (doc);

   gchar *text;
   gchar *text2;
   gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(doc->text_buffer), &iter, 0);

   if (gtk_text_iter_starts_word (&iter))
      {
       a = iter;
      }

   do
     {
      if (gtk_text_iter_starts_word (&iter))
         {  
          b = iter;
          if (gtk_text_iter_backward_char (&b))
             if (gtk_text_iter_get_char (&b) != '_')
                a = iter;
              
          if (gtk_text_iter_forward_word_end (&iter))
             if (gtk_text_iter_ends_word (&iter))
                {
                 if (gtk_text_iter_get_char (&iter) != '_')
                   {
                    text2 = gtk_text_iter_get_slice (&a, &iter);
                    text = g_utf8_strdown (text2, -1); 

                    if (text != NULL)
                       {
                        if (g_unichar_isdigit (g_utf8_get_char (text))) 
                           {
                            gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_digit, &a, &iter);
                            g_free (text);
                            g_free (text2);
                            continue;
                           }

                         if (g_hash_table_lookup (pas_t_keywords, text) != NULL)  
                           {
                            gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_identifier, &a, &iter);
                            g_free (text);
                            g_free (text2);   
                            continue;
                           }

                         if (g_hash_table_lookup (pas_t_types, text) != NULL)  
                            {
                             gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_type, &a, &iter);
                             g_free (text);
                             g_free (text2);
                             continue;
                            }
                   }
                }
              }
          }
        }

  while ( gtk_text_iter_forward_char (&iter));


  gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(doc->text_buffer), &iter, 0);
  a = iter;
 
  do
    {
     a = iter;
        
     switch (gtk_text_iter_get_char (&iter))
            {
     
             case '"': 
                      {
                       if (gtk_text_iter_forward_find_char (&iter,(GtkTextCharPredicate) find_q, NULL, NULL))
                          if (gtk_text_iter_forward_char (&iter)) 
                             gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_string, &a, &iter);
                       break;
                      }
 
             case '\'':             
                       {
                        if (gtk_text_iter_forward_find_char (&iter, (GtkTextCharPredicate)find_q2, NULL, NULL))
                           if (gtk_text_iter_forward_char (&iter))
                              gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_string, &a, &iter);
                        break;
                       } 

             case '/': 
                       {
                        b = iter;
                        if (gtk_text_iter_forward_char (&b))
                           {
                            b = iter;
                            if (gtk_text_iter_forward_char (&b))
                               if (gtk_text_iter_get_char (&b) == '/')
                                  {
                                   a = iter;
                                   gtk_text_iter_forward_line (&iter);
                                      gtk_text_buffer_remove_all_tags (GTK_TEXT_BUFFER(doc->text_buffer), &a, &iter);                
                                      gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_comment, &a, &iter);
                                  }
                            }
                        break;   
                       }  
             }        
    }
    while ( gtk_text_iter_forward_char (&iter) );
}


void do_python_hl (t_note_page *doc)
{
   GtkTextIter iter;
   GtkTextIter a;
   GtkTextIter b;

   remove_tags (doc);

   gchar *text;

   gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(doc->text_buffer), &iter, 0);

   if (gtk_text_iter_starts_word (&iter))
      {
       a = iter;
      }


   do
     {
      if (gtk_text_iter_starts_word (&iter))
         {  
          b = iter;
          if (gtk_text_iter_backward_char (&b))
             if (gtk_text_iter_get_char (&b) != '_')
                a = iter;
              
          if (gtk_text_iter_forward_word_end (&iter))
             if (gtk_text_iter_ends_word (&iter))
                {
                 if (gtk_text_iter_get_char (&iter) != '_')
                    {
                     text = gtk_text_iter_get_slice (&a, &iter);
                     if (text)
                        {
                         if (g_unichar_isdigit (g_utf8_get_char (text))) 
                            {
                             gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_digit, &a, &iter);
                             g_free (text);
                             continue;
                            }

                         if (g_hash_table_lookup (py_t_keywords, text) != NULL)  
                            {
                             gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_identifier, &a, &iter);
                             g_free (text);
                             continue;
                            }
                        }
                }
              }
          }
        }

  while ( gtk_text_iter_forward_char (&iter));


  gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(doc->text_buffer), &iter, 0);
  a = iter;

 
  do
    {
  
     a = iter;
        
     switch (gtk_text_iter_get_char (&iter))
            {
             case '#':  
                      {
                       a = iter;
                       if (gtk_text_iter_forward_line (&iter))
                          if (gtk_text_iter_backward_char (&iter)) 
                             { 
                              gtk_text_buffer_remove_all_tags (GTK_TEXT_BUFFER(doc->text_buffer), &a, &iter);
                              gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_comment, &a, &iter);
                             }
                       break;
                      }

             case '"': 
                      {
                       if (gtk_text_iter_forward_find_char (&iter, (GtkTextCharPredicate)find_q, NULL, NULL))
                          if (gtk_text_iter_forward_char (&iter)) 
                             gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_string, &a, &iter);
                       break;
                      }
 
             case '\'':             
                       {
                        if (gtk_text_iter_forward_find_char (&iter, (GtkTextCharPredicate)find_q2, NULL, NULL))
                           if (gtk_text_iter_forward_char (&iter))
                              gtk_text_buffer_apply_tag (GTK_TEXT_BUFFER(doc->text_buffer), tag_string, &a, &iter);
                        break;
                       }  
        }        
    }

    while ( gtk_text_iter_forward_char (&iter) );
}


void prepare_hl_all (void)
{
  prepare_hl_c ();
  prepare_hl_pas ();
  prepare_hl_py ();
  prepare_hl_php ();
  prepare_hl_bash ();
  prepare_hl_po ();
}


void apply_hl (t_note_page *doc)
{
   if (strcmp (doc->hl_mode, HL_MARKUP) == 0)
      auto_hl_griffon_html ();//apply_html_hl (doc);
   else
   if (strcmp (doc->hl_mode, HL_XML) == 0)
      auto_hl_griffon_xml ();//do_hl_c (doc);
   else
   if (strcmp (doc->hl_mode, HL_JAVA) == 0)
      auto_hl_griffon_java ();//do_hl_c (doc);
   else
   if (strcmp (doc->hl_mode, HL_JAVASCRIPT) == 0)
      auto_hl_griffon_javascript ();//do_hl_c (doc);
   else
   if (strcmp (doc->hl_mode, HL_C) == 0)
      auto_hl_griffon_c ();//do_hl_c (doc);
   else
   if (strcmp (doc->hl_mode, HL_PHP) == 0)
      auto_hl_griffon_php ();//do_hl_php (doc);
   else
   if (strcmp (doc->hl_mode, HL_PYTHON) == 0)
      auto_hl_griffon_python ();//do_hl_php (doc);
   else
   if (strcmp (doc->hl_mode, HL_RUBY) == 0)
      auto_hl_griffon_ruby ();//do_hl_php (doc);
   else
   if (strcmp (doc->hl_mode, HL_CSS) == 0)
      auto_hl_griffon_css ();
   else
   if (strcmp (doc->hl_mode, HL_CPP) == 0)
      auto_hl_griffon_cpp ();
   else
   if (strcmp (doc->hl_mode, HL_PERL) == 0)
      auto_hl_griffon_perl ();//do_hl_php (doc);
   else
   if (strcmp (doc->hl_mode, HL_PASCAL) == 0)
      do_hl_pascal (doc);
   else
   if (strcmp (doc->hl_mode, HL_PYTHON) == 0)
      do_python_hl (doc);
   else
   if (strcmp (doc->hl_mode, HL_NONE) == 0)
      auto_hl_griffon_none();      
   else
   if (strcmp (doc->hl_mode, HL_BASH) == 0)
      auto_hl_griffon_sh ();//do_hl_bash (doc);      
	else
   if (strcmp (doc->hl_mode, HL_PO) == 0)
      auto_hl_griffon_perl ();//do_hl_po (doc);      
	else
   if (strcmp (doc->hl_mode, HL_SQL) == 0)
      auto_hl_griffon_sql ();//do_hl_po (doc); 
}


typedef struct 
              {
               gboolean start; 
               gboolean end;
              } t_s_switcher;
                  

void do_hl_spell_check2 ()
{
	printf("\n");
}


void do_errors_hl (GtkTextView *text_view)
{
  GtkTextIter iter;
  GtkTextIter a;
  gchar *filename = NULL;
  gint lineno = 0;

  GtkTextBuffer *text_buffer = gtk_text_view_get_buffer (text_view);

  gchar *text;

  gint c = gtk_text_buffer_get_line_count (GTK_TEXT_BUFFER(text_buffer));
  gint i;

  gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(text_buffer), &iter); 

  for (i = 0; i <= c; i++)
      {
       gtk_text_iter_set_line (&iter, i); 
       a = iter;

       if (gtk_text_iter_forward_to_line_end (&a))
          {
           text = gtk_text_iter_get_text (&iter, &a);
             
           if (parse_error_line (text, &filename, &lineno))
              {
               gtk_text_buffer_remove_all_tags (GTK_TEXT_BUFFER(text_buffer), &iter, &a);
               gtk_text_buffer_apply_tag_by_name (GTK_TEXT_BUFFER(text_buffer), "lm_error", &iter, &a);
               g_free (filename); 
              } 
           
           g_free (text); 
          } 
       }
}


void set_lm_colors (void)
{
  g_object_set (G_OBJECT (tag_lm_error), "foreground", confile.tag_color_lm_error_fg,
               "background", confile.tag_color_lm_error_bg, NULL);
}
