
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
#include <gtk/gtk.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguage.h>
//#include <gtkspell/gtkspell.h>

#include "griffon_text_document.h"
#include "griffon_hl.h"
#include "griffon_config.h"
#include "griffon_defs.h"
#include "interface.h"

tooltip_instruction (GtkSourceMarkAttributes *attributes,GtkSourceMark *mark,gpointer user_data)
{
	if(strcmp("if", user_data) == 0 || strcmp("while", user_data) == 0 || strcmp("unless", user_data) == 0)
	{
		gchar *text=g_strconcat ("______________________________________________\n\n",user_data," (expression)\n","{\n\ninstruction\n\n}\n",NULL);return text;
	}

	if(strcmp("break", user_data) == 0)
	{
		gchar *text=g_strconcat ("______________________________________________\n\n",user_data,";\n\n",NULL);return text;
	}

	if(strcmp("last", user_data) == 0)
	{
		gchar *text=g_strconcat ("______________________________________________\n\n",user_data,";\n\n",NULL);return text;
	}

	if(strcmp("exit", user_data) == 0)
	{
		gchar *text=g_strconcat ("______________________________________________\n\n",user_data," ();\n\n",NULL);return text;
	}

	if(strcmp("sleep", user_data) == 0)
	{
		gchar *text=g_strconcat ("______________________________________________\n\n",user_data," (seconds);\n\n",NULL);return text;
	}

	if(strcmp("else", user_data) == 0)
	{
		gchar *text=g_strconcat ("______________________________________________\n\n",user_data,"\n{\n\ninstruction\n\n}\n\n",NULL);return text;
	}

	if(strcmp("for", user_data) == 0)
	{
		gchar *text=g_strconcat ("______________________________________________\n\n",user_data," (expression)\n{\n\ninstruction\n\n}\n\n",NULL);return text;
	}

	if(strcmp("foreach", user_data) == 0)
	{
		gchar *text=g_strconcat ("______________________________________________\n\n",user_data," (expression)\n{\n\ninstruction\n\n}\n\n",NULL);return text;
	}

	if(strcmp("include", user_data) == 0)
	{
		gchar *text=g_strconcat ("______________________________________________\n\n",user_data," ('file_path');\n\n",NULL);return text;
	}

	if(strcmp("require", user_data) == 0)
	{
		gchar *text=g_strconcat ("______________________________________________\n\n",user_data," ('file_path');\n\n",NULL);return text;
	}

	if(strcmp("function", user_data) == 0)
	{
		gchar *text=g_strconcat ("______________________________________________\n\n",user_data,"(value)\n{\n\ncode\n\n}\n\n",NULL);return text;
	}

	if(strcmp("sub", user_data) == 0)
	{
		gchar *text=g_strconcat ("______________________________________________\n\n",user_data,"(value)\n{\n\ncode\n\n}\n\n",NULL);return text;
	}

}

void assign_tags (t_note_page *doc)
{
  GtkTextTagTable *t = gtk_text_buffer_get_tag_table (doc->text_buffer);

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
  gtk_text_buffer_get_iter_at_offset (doc->text_buffer, &itstart, 0);
  gtk_text_buffer_get_iter_at_offset (doc->text_buffer, &itend, gtk_text_buffer_get_char_count (doc->text_buffer));
  //gtk_text_buffer_remove_all_tags (doc->text_buffer, &itstart, &itend);
}                   


void clear_remove_tags (t_note_page *doc)
{
  GtkTextIter itstart, itend;
  gtk_text_buffer_get_iter_at_offset (doc->text_buffer, &itstart, 0);
  gtk_text_buffer_get_iter_at_offset (doc->text_buffer, &itend, gtk_text_buffer_get_char_count (doc->text_buffer));
  gtk_text_buffer_remove_all_tags (doc->text_buffer, &itstart, &itend);
  apply_hl (doc);
} 

void apply_html_hl (t_note_page *doc)
{
  gchar *text = doc_get_buf (doc->text_buffer);
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
            gtk_text_buffer_get_iter_at_offset (doc->text_buffer, &it_start, i);

         if (u == 62) //>
            {
             gtk_text_buffer_get_iter_at_offset (doc->text_buffer, &it_end, i+1);
             gtk_text_buffer_apply_tag (doc->text_buffer, tag_html_tag, &it_start, &it_end);
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

//n.p. Scorn - Vae Solis - On Ice
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

//Object and Free Pascal keywords
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


gboolean find_slash (gunichar ch, gpointer user_data)
{
  if (ch == '/')
     return TRUE;
  else
      return FALSE;
}


gboolean find_q (gunichar ch, gpointer user_data)
{
  if (ch == '"')
     return TRUE;
  else
      return FALSE;
}


gboolean find_q2 (gunichar ch, gpointer user_data)
{
  if (ch == '\'')
     return TRUE;
  else
      return FALSE;
}


gboolean find_ast (gunichar ch, gpointer user_data)
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


	GdkPixbuf *pixbuf_mark;


void do_hl_c (t_note_page *doc)
{
//**************************** FAIRE CORRECTION POUR BUG MEMOIR	
return;

//	auto_hl_griffon_c ();
   GtkTextIter iter;
   GtkTextIter a;
   GtkTextIter b;
   GtkTextIter c;
   GtkTextIter d,start,stop;
	GtkTextIter itstart;
	gint lineit;
	gchar *text2;

	GtkSourceMark * mark;
	GtkSourceMarkAttributes *attribu;

	gtk_text_buffer_get_bounds(cur_text_doc->text_buffer,&start,&stop);
gtk_source_buffer_remove_source_marks(cur_text_doc->text_buffer,&start,&stop,"icon_function");
gtk_source_buffer_remove_source_marks(cur_text_doc->text_buffer,&start,&stop,"icon_sub");
gtk_source_buffer_remove_source_marks(cur_text_doc->text_buffer,&start,&stop,"icon_include");
gtk_source_buffer_remove_source_marks(cur_text_doc->text_buffer,&start,&stop,"icon_require");
gtk_source_buffer_remove_source_marks(cur_text_doc->text_buffer,&start,&stop,"icon_exit");
gtk_source_buffer_remove_source_marks(cur_text_doc->text_buffer,&start,&stop,"icon_last");
gtk_source_buffer_remove_source_marks(cur_text_doc->text_buffer,&start,&stop,"icon_break");
gtk_source_buffer_remove_source_marks(cur_text_doc->text_buffer,&start,&stop,"icon_sleep");
gtk_source_buffer_remove_source_marks(cur_text_doc->text_buffer,&start,&stop,"icon_if");
gtk_source_buffer_remove_source_marks(cur_text_doc->text_buffer,&start,&stop,"icon_else");
gtk_source_buffer_remove_source_marks(cur_text_doc->text_buffer,&start,&stop,"icon_while");
gtk_source_buffer_remove_source_marks(cur_text_doc->text_buffer,&start,&stop,"icon_for");
gtk_source_buffer_remove_source_marks(cur_text_doc->text_buffer,&start,&stop,"icon_unless");
gtk_source_buffer_remove_source_marks(cur_text_doc->text_buffer,&start,&stop,"icon_foreach");
gtk_source_buffer_remove_source_marks(cur_text_doc->text_buffer,&start,&stop,"icon_down");
gtk_source_buffer_remove_source_marks(cur_text_doc->text_buffer,&start,&stop,"icon_down2");
gtk_source_buffer_remove_source_marks(cur_text_doc->text_buffer,&start,&stop,"icon_up");
gtk_source_buffer_remove_source_marks(cur_text_doc->text_buffer,&start,&stop,"icon_up2");
gtk_source_buffer_remove_source_marks(cur_text_doc->text_buffer,&start,&stop,"icon_com");
gtk_text_buffer_remove_tag_by_name  (cur_text_doc->text_buffer,"err2",&start,&stop);

	//*********************************
  gchar *text;

   gtk_text_buffer_get_iter_at_offset (doc->text_buffer, &iter, 0);

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

								
                         if (strcmp("function", text) == 0 || strcmp("sub", text) == 0) 
                         {
																					//*********** prise en compte de iter de debut de ligne
																					
																					gint lineit=gtk_text_iter_get_line(&a);
																					gtk_text_buffer_get_iter_at_line (cur_text_doc->text_buffer, &itstart, lineit);

																						pixbuf_mark = gdk_pixbuf_new_from_file ("/usr/local/share/griffon/pixmaps/griffon_exe.png", NULL);
																						GtkSourceMarkAttributes *attribu=gtk_source_mark_attributes_new();
																						gtk_source_mark_attributes_set_pixbuf(attribu,pixbuf_mark );
																						gtk_source_view_set_mark_attributes(cur_text_doc->text_view,"icon_function",attribu,0);
																						gtk_source_buffer_create_source_mark(cur_text_doc->text_buffer,NULL,"icon_function",&itstart);

																						//text2=g_strconcat ("",text,NULL);
																						//g_signal_connect(attribu, "query-tooltip-text",G_CALLBACK(tooltip_instruction),text2);

                             g_free (text);gdk_pixbuf_unref( pixbuf_mark);g_free (text2);
                             continue;
                          }


                         if (strcmp("sub", text) == 0) 
                         {
																					//*********** prise en compte de iter de debut de ligne
																				
																				 lineit=gtk_text_iter_get_line(&a);
																					gtk_text_buffer_get_iter_at_line (cur_text_doc->text_buffer, &itstart, lineit);

																						pixbuf_mark = gdk_pixbuf_new_from_file ("/usr/local/share/griffon/pixmaps/griffon_exe.png", NULL);
																						attribu=gtk_source_mark_attributes_new();
																						gtk_source_mark_attributes_set_pixbuf(attribu,pixbuf_mark );
																						gtk_source_view_set_mark_attributes(cur_text_doc->text_view,"icon_sub",attribu,0);
																						gtk_source_buffer_create_source_mark(cur_text_doc->text_buffer,NULL,"icon_sub",&itstart);

																						//text2=g_strconcat ("",text,NULL);
																						//g_signal_connect(attribu, "query-tooltip-text",G_CALLBACK(tooltip_instruction),text2);

                             g_free (text);gdk_pixbuf_unref( pixbuf_mark);g_free (text2);
                             continue;
                          }

								
                         if (strcmp("include", text) == 0) 
                         {

																					//*********** prise en compte de iter de debut de ligne
																					
																					 lineit=gtk_text_iter_get_line(&a);
																					gtk_text_buffer_get_iter_at_line (cur_text_doc->text_buffer, &itstart, lineit);

																						pixbuf_mark = gdk_pixbuf_new_from_file ("/usr/local/share/griffon/pixmaps/griffon_log.png", NULL);
																						attribu=gtk_source_mark_attributes_new();
																						gtk_source_mark_attributes_set_pixbuf(attribu,pixbuf_mark );
																						gtk_source_view_set_mark_attributes(cur_text_doc->text_view,"icon_include",attribu,0);
																						gtk_source_buffer_create_source_mark(cur_text_doc->text_buffer,NULL,"icon_include",&itstart);
							
																						//text2=g_strconcat ("",text,NULL);
																						//g_signal_connect(attribu, "query-tooltip-text",G_CALLBACK(tooltip_instruction),text2);

                             g_free (text);gdk_pixbuf_unref( pixbuf_mark);g_free (text2);
                             continue;
                          }


                         if (strcmp("require", text) == 0) 
                         {

																					//*********** prise en compte de iter de debut de ligne
																					
																					 lineit=gtk_text_iter_get_line(&a);
																					gtk_text_buffer_get_iter_at_line (cur_text_doc->text_buffer, &itstart, lineit);

																						pixbuf_mark = gdk_pixbuf_new_from_file ("/usr/local/share/griffon/pixmaps/griffon_log.png", NULL);
																						attribu=gtk_source_mark_attributes_new();
																						gtk_source_mark_attributes_set_pixbuf(attribu,pixbuf_mark );
																						gtk_source_view_set_mark_attributes(cur_text_doc->text_view,"icon_sub",attribu,0);
																						gtk_source_buffer_create_source_mark(cur_text_doc->text_buffer,NULL,"icon_sub",&itstart);
							
																						//text2=g_strconcat ("",text,NULL);
																						//g_signal_connect(attribu, "query-tooltip-text",G_CALLBACK(tooltip_instruction),text2);

                             g_free (text);gdk_pixbuf_unref( pixbuf_mark);g_free (text2);
                             continue;
                          }

								
                         if (strcmp("exit", text) == 0) 
                          {
																					//*********** prise en compte de iter de debut de ligne
																					
																					 lineit=gtk_text_iter_get_line(&a);
																					gtk_text_buffer_get_iter_at_line (cur_text_doc->text_buffer, &itstart, lineit);

																					pixbuf_mark = gdk_pixbuf_new_from_file ("/usr/local/share/griffon/pixmaps/griffon_stop.png", NULL);
																					attribu=gtk_source_mark_attributes_new();
																					gtk_source_mark_attributes_set_pixbuf(attribu,pixbuf_mark );
																					gtk_source_view_set_mark_attributes(cur_text_doc->text_view,"icon_exit",attribu,0);
																					gtk_source_buffer_create_source_mark(cur_text_doc->text_buffer,NULL,"icon_exit",&itstart);	
							
																					//text2=g_strconcat ("",text,NULL);
																					//g_signal_connect(attribu, "query-tooltip-text",G_CALLBACK(tooltip_instruction),text2);

                             g_free (text);gdk_pixbuf_unref( pixbuf_mark);g_free (text2);
                             continue;
                           }


                         if (strcmp("last", text) == 0) 
                          {

																					//*********** prise en compte de iter de debut de ligne
																					
																					 lineit=gtk_text_iter_get_line(&a);
																					gtk_text_buffer_get_iter_at_line (cur_text_doc->text_buffer, &itstart, lineit);

																					pixbuf_mark = gdk_pixbuf_new_from_file ("/usr/local/share/griffon/pixmaps/griffon_stop.png", NULL);
																					attribu=gtk_source_mark_attributes_new();
																					gtk_source_mark_attributes_set_pixbuf(attribu,pixbuf_mark );
																					gtk_source_view_set_mark_attributes(cur_text_doc->text_view,"icon_last",attribu,0);
																					gtk_source_buffer_create_source_mark(cur_text_doc->text_buffer,NULL,"icon_last",&itstart);	
							
																					//text2=g_strconcat ("",text,NULL);
																					//g_signal_connect(attribu, "query-tooltip-text",G_CALLBACK(tooltip_instruction),text2);

                             g_free (text);gdk_pixbuf_unref( pixbuf_mark);g_free (text2);
                             continue;
                           }

                         if (strcmp("break", text) == 0) 
                          {
																					//*********** prise en compte de iter de debut de ligne
																					
																					 lineit=gtk_text_iter_get_line(&a);
																					gtk_text_buffer_get_iter_at_line (cur_text_doc->text_buffer, &itstart, lineit);

																					pixbuf_mark = gdk_pixbuf_new_from_file ("/usr/local/share/griffon/pixmaps/griffon_stop.png", NULL);
																					attribu=gtk_source_mark_attributes_new();
																					gtk_source_mark_attributes_set_pixbuf(attribu,pixbuf_mark );
																					gtk_source_view_set_mark_attributes(cur_text_doc->text_view,"icon_break",attribu,0);
																					gtk_source_buffer_create_source_mark(cur_text_doc->text_buffer,NULL,"icon_break",&itstart);	
							
																					//text2=g_strconcat ("",text,NULL);
																					//g_signal_connect(attribu, "query-tooltip-text",G_CALLBACK(tooltip_instruction),text2);

                             g_free (text);gdk_pixbuf_unref( pixbuf_mark);g_free (text2);
                             continue;
                           }

                         if (strcmp("sleep", text) == 0) 
                          {

																					//*********** prise en compte de iter de debut de ligne
																					
																					 lineit=gtk_text_iter_get_line(&a);
																					gtk_text_buffer_get_iter_at_line (cur_text_doc->text_buffer, &itstart, lineit);

																					pixbuf_mark = gdk_pixbuf_new_from_file ("/usr/local/share/griffon/pixmaps/griffon_stop.png", NULL);
																					attribu=gtk_source_mark_attributes_new();
																					gtk_source_mark_attributes_set_pixbuf(attribu,pixbuf_mark );
																					gtk_source_view_set_mark_attributes(cur_text_doc->text_view,"icon_sleep",attribu,0);
																					gtk_source_buffer_create_source_mark(cur_text_doc->text_buffer,NULL,"icon_sleep",&itstart);	
							
																				//	text2=g_strconcat ("",text,NULL);
																				//	g_signal_connect(attribu, "query-tooltip-text",G_CALLBACK(tooltip_instruction),text2);

                             g_free (text);gdk_pixbuf_unref( pixbuf_mark);g_free (text2);
                             continue;
                           }


								
                         if (strcmp("if", text) == 0) 
                         {

																					//*********** prise en compte de iter de debut de ligne
																					
																					 lineit=gtk_text_iter_get_line(&a);
																					gtk_text_buffer_get_iter_at_line (cur_text_doc->text_buffer, &itstart, lineit);

																				pixbuf_mark = gdk_pixbuf_new_from_file ("/usr/local/share/griffon/pixmaps/griffon_list.png", NULL);
																				attribu=gtk_source_mark_attributes_new();
																				gtk_source_mark_attributes_set_pixbuf(attribu,pixbuf_mark );
																				gtk_source_view_set_mark_attributes(cur_text_doc->text_view,"icon_if",attribu,0);
																				gtk_source_buffer_create_source_mark(cur_text_doc->text_buffer,NULL,"icon_if",&itstart);

																			//	text2=g_strconcat ("",text,NULL);
																			//	g_signal_connect(attribu, "query-tooltip-text",G_CALLBACK(tooltip_instruction),text2); 

                             g_free (text);gdk_pixbuf_unref( pixbuf_mark);g_free (text2);
                             continue;
                         }


                         if (strcmp("else", text) == 0) 
                         {

																					//*********** prise en compte de iter de debut de ligne
																					
																					 lineit=gtk_text_iter_get_line(&a);
																					gtk_text_buffer_get_iter_at_line (cur_text_doc->text_buffer, &itstart, lineit);

																				pixbuf_mark = gdk_pixbuf_new_from_file ("/usr/local/share/griffon/pixmaps/griffon_list.png", NULL);
																				attribu=gtk_source_mark_attributes_new();
																				gtk_source_mark_attributes_set_pixbuf(attribu,pixbuf_mark );
																				gtk_source_view_set_mark_attributes(cur_text_doc->text_view,"icon_else",attribu,0);
																				gtk_source_buffer_create_source_mark(cur_text_doc->text_buffer,NULL,"icon_else",&itstart);

																			//text2=g_strconcat ("",text,NULL);
																			//	g_signal_connect(attribu, "query-tooltip-text",G_CALLBACK(tooltip_instruction),text2); 

                             g_free (text);gdk_pixbuf_unref( pixbuf_mark);g_free (text2);
                             continue;
                         }


                         if (strcmp("while", text) == 0 ) 
                         {

																					//*********** prise en compte de iter de debut de ligne
																					
																					gint lineit=gtk_text_iter_get_line(&a);
																					gtk_text_buffer_get_iter_at_line (cur_text_doc->text_buffer, &itstart, lineit);

																				pixbuf_mark = gdk_pixbuf_new_from_file ("/usr/local/share/griffon/pixmaps/griffon_list.png", NULL);
																				attribu=gtk_source_mark_attributes_new();
																				gtk_source_mark_attributes_set_pixbuf(attribu,pixbuf_mark );
																				gtk_source_view_set_mark_attributes(cur_text_doc->text_view,"icon_while",attribu,0);
																				gtk_source_buffer_create_source_mark(cur_text_doc->text_buffer,NULL,"icon_while",&itstart);

																			//	text2=g_strconcat ("",text,NULL);
																			//	g_signal_connect(attribu, "query-tooltip-text",G_CALLBACK(tooltip_instruction),text2); 

                             g_free (text);gdk_pixbuf_unref( pixbuf_mark);g_free (text2);
                             continue;
                         }


                         if (strcmp("for", text) == 0) 
                         {

																					//*********** prise en compte de iter de debut de ligne
																					
																					 lineit=gtk_text_iter_get_line(&a);
																					gtk_text_buffer_get_iter_at_line (cur_text_doc->text_buffer, &itstart, lineit);

																				pixbuf_mark = gdk_pixbuf_new_from_file ("/usr/local/share/griffon/pixmaps/griffon_list.png", NULL);
																				attribu=gtk_source_mark_attributes_new();
																				gtk_source_mark_attributes_set_pixbuf(attribu,pixbuf_mark );
																				gtk_source_view_set_mark_attributes(cur_text_doc->text_view,"icon_for",attribu,0);
																				gtk_source_buffer_create_source_mark(cur_text_doc->text_buffer,NULL,"icon_for",&itstart);

																			//	text2=g_strconcat ("",text,NULL);
																			//	g_signal_connect(attribu, "query-tooltip-text",G_CALLBACK(tooltip_instruction),text2); 

                             g_free (text);gdk_pixbuf_unref( pixbuf_mark); g_free (text2);
                             continue;
                         }


                         if (strcmp("unless", text) == 0) 
                         {

																					//*********** prise en compte de iter de debut de ligne
																					
																					 lineit=gtk_text_iter_get_line(&a);
																					gtk_text_buffer_get_iter_at_line (cur_text_doc->text_buffer, &itstart, lineit);

																				pixbuf_mark = gdk_pixbuf_new_from_file ("/usr/local/share/griffon/pixmaps/griffon_list.png", NULL);
																				attribu=gtk_source_mark_attributes_new();
																				gtk_source_mark_attributes_set_pixbuf(attribu,pixbuf_mark );
																				gtk_source_view_set_mark_attributes(cur_text_doc->text_view,"icon_unless",attribu,0);
																				gtk_source_buffer_create_source_mark(cur_text_doc->text_buffer,NULL,"icon_unless",&itstart);

																			//	text2=g_strconcat ("",text,NULL);
																			//	g_signal_connect(attribu, "query-tooltip-text",G_CALLBACK(tooltip_instruction),text2); 

                             g_free (text);gdk_pixbuf_unref( pixbuf_mark);g_free (text2);
                             continue;
                         }


                         if (strcmp("foreach", text) == 0) 
                         {

																					//*********** prise en compte de iter de debut de ligne
																					
																					 lineit=gtk_text_iter_get_line(&a);
																					gtk_text_buffer_get_iter_at_line (cur_text_doc->text_buffer, &itstart, lineit);

																				pixbuf_mark = gdk_pixbuf_new_from_file ("/usr/local/share/griffon/pixmaps/griffon_list.png", NULL);
																				attribu=gtk_source_mark_attributes_new();
																				gtk_source_mark_attributes_set_pixbuf(attribu,pixbuf_mark );
																				gtk_source_view_set_mark_attributes(cur_text_doc->text_view,"icon_foreach",attribu,0);
																				gtk_source_buffer_create_source_mark(cur_text_doc->text_buffer,NULL,"icon_foreach",&itstart);

																			//	text2=g_strconcat ("",text,NULL);
																			//	g_signal_connect(attribu, "query-tooltip-text",G_CALLBACK(tooltip_instruction),text2); 

                             g_free (text);gdk_pixbuf_unref( pixbuf_mark);g_free (text2);
                             continue;
                         }







                        }
                }
              }
          }
        }

  while ( gtk_text_iter_forward_char (&iter));

  gtk_text_buffer_get_iter_at_offset (doc->text_buffer, &iter, 0);
  a = iter;
  c = iter;
 
  do
    {
  
     a = iter;
        
     switch (gtk_text_iter_get_char (&iter))
            {

				//********************************* Debut de bloque
             case '{':  
                      {
	                       a = iter;
	                       if (gtk_text_iter_forward_line (&iter))
                          if (gtk_text_iter_backward_char (&iter)) 
                             { 

																					//*********** prise en compte de iter de debut de ligne
																					
																					 lineit=gtk_text_iter_get_line(&a);
																					gtk_text_buffer_get_iter_at_line (cur_text_doc->text_buffer, &itstart, lineit);

									if(gtk_text_iter_equal(&a,&itstart)==TRUE)
									{
									pixbuf_mark = gdk_pixbuf_new_from_file ("/usr/local/share/griffon/pixmaps/griffon_down.png", NULL);
										attribu=gtk_source_mark_attributes_new();
										gtk_source_mark_attributes_set_pixbuf(attribu,pixbuf_mark );
										gtk_source_view_set_mark_attributes(cur_text_doc->text_view,"icon_down",attribu,0);
										gtk_source_buffer_create_source_mark(cur_text_doc->text_buffer,NULL,"icon_down",&itstart);gdk_pixbuf_unref( pixbuf_mark);
									}
									else
									{
									pixbuf_mark = gdk_pixbuf_new_from_file ("/usr/local/share/griffon/pixmaps/griffon_down2.png", NULL);
										attribu=gtk_source_mark_attributes_new();
										gtk_source_mark_attributes_set_pixbuf(attribu,pixbuf_mark );
										gtk_source_view_set_mark_attributes(cur_text_doc->text_view,"icon_down2",attribu,0);
										gtk_source_buffer_create_source_mark(cur_text_doc->text_buffer,NULL,"icon_down2",&itstart);gdk_pixbuf_unref( pixbuf_mark);
									}

                             }
	                       break;
	                      }

				//********************************* FIn de bloque
             case '}':  
                      {
	                       a = iter;
	                       if (gtk_text_iter_forward_line (&iter))
                          if (gtk_text_iter_backward_char (&iter)) 
                             { 

																					//*********** prise en compte de iter de debut de ligne
																					
																					 lineit=gtk_text_iter_get_line(&a);
																					gtk_text_buffer_get_iter_at_line (cur_text_doc->text_buffer, &itstart, lineit);

									if(gtk_text_iter_equal(&a,&itstart)==TRUE)
									{
									pixbuf_mark = gdk_pixbuf_new_from_file ("/usr/local/share/griffon/pixmaps/griffon_up.png", NULL);
										attribu=gtk_source_mark_attributes_new();
										gtk_source_mark_attributes_set_pixbuf(attribu,pixbuf_mark );
										gtk_source_view_set_mark_attributes(cur_text_doc->text_view,"icon_up",attribu,0);
										gtk_source_buffer_create_source_mark(cur_text_doc->text_buffer,NULL,"icon_up",&itstart);gdk_pixbuf_unref( pixbuf_mark);
									}
									else
									{
									pixbuf_mark = gdk_pixbuf_new_from_file ("/usr/local/share/griffon/pixmaps/griffon_up2.png", NULL);
										attribu=gtk_source_mark_attributes_new();
										gtk_source_mark_attributes_set_pixbuf(attribu,pixbuf_mark );
										gtk_source_view_set_mark_attributes(cur_text_doc->text_view,"icon_up2",attribu,0);
										gtk_source_buffer_create_source_mark(cur_text_doc->text_buffer,NULL,"icon_up2",&itstart);gdk_pixbuf_unref( pixbuf_mark);
									}

                             }
	                      break;
	                      }

				//********************************* Commentaire
             /*case '#':  
                      {
	                       a = iter;
	                       if (gtk_text_iter_forward_line (&iter))
                          if (gtk_text_iter_backward_char (&iter)) 
                             { 
																					GtkTextIter itstart;
																					gint lineit=gtk_text_iter_get_line(&a);
																					gtk_text_buffer_get_iter_at_line (cur_text_doc->text_buffer, &itstart, lineit);

										pixbuf_mark = gdk_pixbuf_new_from_file ("/usr/local/share/griffon/pixmaps/griffon_man.png", NULL);
										GtkSourceMarkAttributes *attribu=gtk_source_mark_attributes_new();
										gtk_source_mark_attributes_set_pixbuf(attribu,pixbuf_mark );
										gtk_source_view_set_mark_attributes(cur_text_doc->text_view,"icon_com",attribu,0);
										gtk_source_buffer_create_source_mark(cur_text_doc->text_buffer,NULL,"icon_com",&itstart);
                             }
	                       break;
	                      }*/
            }  
        }        

    while ( gtk_text_iter_forward_char (&iter) );
}


void do_hl_php (t_note_page *doc)
{
	auto_hl_griffon_perl ();
   GtkTextIter iter;
   GtkTextIter a;
   GtkTextIter b;
   GtkTextIter c;
   GtkTextIter d;

   remove_tags (doc);

   gchar *text;

   gtk_text_buffer_get_iter_at_offset (doc->text_buffer, &iter, 0);

   if (gtk_text_iter_starts_word (&iter))
      {
       a = iter;
       c = iter;
      }

   //keywords
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
                             gtk_text_buffer_apply_tag (doc->text_buffer, tag_digit, &a, &iter);
                             g_free (text);
                             continue;
                            }

                         if (g_hash_table_lookup (php_t_keywords, text))  
                            {
                             gtk_text_buffer_apply_tag (doc->text_buffer, tag_identifier, &a, &iter);
                             g_free (text);
                             continue;
                            }
                        }
                }
              }
          }
        }

  while ( gtk_text_iter_forward_char (&iter));

  //end keywords

  gtk_text_buffer_get_iter_at_offset (doc->text_buffer, &iter, 0);
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
                              gtk_text_buffer_remove_all_tags (doc->text_buffer, &a, &iter);
                              gtk_text_buffer_apply_tag (doc->text_buffer, tag_comment, &a, &iter);
                             }
                       break;
                      }

             case '/':
                      {
                       b = iter;
                       if (gtk_text_iter_forward_char (&b))
                          if (gtk_text_iter_get_char (&b) == '*')
                             {
                              c = iter;
                              b = c;
                        
                              do 
                                { 
                                 if (gtk_text_iter_forward_find_char (&iter, find_slash, NULL, NULL))
                                    {
                                     b = iter;
                                     if (gtk_text_iter_backward_char (&b))
                                        if (gtk_text_iter_get_char (&b) == '*')
                                           {
                                            gtk_text_buffer_remove_all_tags (doc->text_buffer, &c, &iter);                
                                            gtk_text_buffer_apply_tag (doc->text_buffer, tag_comment, &c, &iter);
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
                                      gtk_text_buffer_remove_all_tags (doc->text_buffer, &a, &iter);                
                                      gtk_text_buffer_apply_tag (doc->text_buffer, tag_comment, &a, &iter);
                                     }
                              }
                       break;   
                      }  

             case '"': 
                      {
                       if (gtk_text_iter_forward_find_char (&iter, find_q, NULL, NULL))
                          if (gtk_text_iter_forward_char (&iter)) 
                             gtk_text_buffer_apply_tag (doc->text_buffer, tag_string, &a, &iter);
                       break;
                      }
 
             case '\'':             
                       {
                        if (gtk_text_iter_forward_find_char (&iter, find_q2, NULL, NULL))
                           if (gtk_text_iter_forward_char (&iter))
                              gtk_text_buffer_apply_tag (doc->text_buffer, tag_string, &a, &iter);
                        break;
                       }  
        }        
    }

    while ( gtk_text_iter_forward_char (&iter) );
}


void do_hl_bash (t_note_page *doc)
{
	//auto_hl_griffon_bash ();

   GtkTextIter iter;
   GtkTextIter a;
   GtkTextIter b;
   GtkTextIter c;
   GtkTextIter d;

   remove_tags (doc);

   gchar *text;

   gtk_text_buffer_get_iter_at_offset (doc->text_buffer, &iter, 0);

   if (gtk_text_iter_starts_word (&iter))
      {
       a = iter;
       c = iter;
      }

   //keywords
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
                             gtk_text_buffer_apply_tag (doc->text_buffer, tag_digit, &a, &iter);
                             g_free (text);
                             continue;
                            }

                         if (g_hash_table_lookup (bash_t_keywords, text))  
                            {
                             gtk_text_buffer_apply_tag (doc->text_buffer, tag_identifier, &a, &iter);
                             g_free (text);
                             continue;
                            }

                        }
                }
              }
          }
        }

  while ( gtk_text_iter_forward_char (&iter));

  //end keywords

  gtk_text_buffer_get_iter_at_offset (doc->text_buffer, &iter, 0);
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
                             gtk_text_buffer_remove_all_tags (doc->text_buffer, &a, &iter);
                             gtk_text_buffer_apply_tag (doc->text_buffer, tag_comment, &a, &iter);
                            }
                      break;
                     }

             case '"': 
                      {
                       if (gtk_text_iter_forward_find_char (&iter, find_q, NULL, NULL))
                          if (gtk_text_iter_forward_char (&iter)) 
                            {
                             gtk_text_buffer_remove_all_tags (doc->text_buffer, &a, &iter);
                             gtk_text_buffer_apply_tag (doc->text_buffer, tag_string, &a, &iter);
                            }
                       break;
                      }
 
             case '\'':             
                       {
                        if (gtk_text_iter_forward_find_char (&iter, find_q2, NULL, NULL))
                           if (gtk_text_iter_forward_char (&iter))
                              {
                               gtk_text_buffer_apply_tag (doc->text_buffer, tag_string, &a, &iter);
                               gtk_text_buffer_apply_tag (doc->text_buffer, tag_string, &a, &iter);
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
   GtkTextIter c;
   GtkTextIter d;

   remove_tags (doc);

   gchar *text;

   gtk_text_buffer_get_iter_at_offset (doc->text_buffer, &iter, 0);

   if (gtk_text_iter_starts_word (&iter))
      {
       a = iter;
       c = iter;
      }

   //keywords
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
                             gtk_text_buffer_apply_tag (doc->text_buffer, tag_digit, &a, &iter);
                             g_free (text);
                             continue;
                            }

                         if (g_hash_table_lookup (po_t_keywords, text))  
                            {
                             gtk_text_buffer_apply_tag (doc->text_buffer, tag_identifier, &a, &iter);
                             g_free (text);
                             continue;
                            }

                        }
                }
              }
          }
        }

  while ( gtk_text_iter_forward_char (&iter));

  //end keywords

  gtk_text_buffer_get_iter_at_offset (doc->text_buffer, &iter, 0);
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
                              gtk_text_buffer_remove_all_tags (doc->text_buffer, &a, &iter);
                              gtk_text_buffer_apply_tag (doc->text_buffer, tag_comment, &a, &iter);
                             }
                       break;
                      }

             case '"': 
                      {
                       if (gtk_text_iter_forward_find_char (&iter, find_q, NULL, NULL))
                          if (gtk_text_iter_forward_char (&iter)) 
                            {
                             gtk_text_buffer_remove_all_tags (doc->text_buffer, &a, &iter);
                             gtk_text_buffer_apply_tag (doc->text_buffer, tag_string, &a, &iter);
                            }
                       break;
                      }
 
             case '\'':             
                       {
                        if (gtk_text_iter_forward_find_char (&iter, find_q2, NULL, NULL))
                           if (gtk_text_iter_forward_char (&iter))
                              {
                               gtk_text_buffer_apply_tag (doc->text_buffer, tag_string, &a, &iter);
                               gtk_text_buffer_apply_tag (doc->text_buffer, tag_string, &a, &iter);
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
   GtkTextIter c;
   GtkTextIter d;

   remove_tags (doc);

   gchar *text;
   gchar *text2;
   gtk_text_buffer_get_iter_at_offset (doc->text_buffer, &iter, 0);

   if (gtk_text_iter_starts_word (&iter))
      {
       a = iter;
       c = iter;
      }

   //keywords
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
                            gtk_text_buffer_apply_tag (doc->text_buffer, tag_digit, &a, &iter);
                            g_free (text);
                            g_free (text2);
                            continue;
                           }

                         if (g_hash_table_lookup (pas_t_keywords, text) != NULL)  
                           {
                            gtk_text_buffer_apply_tag (doc->text_buffer, tag_identifier, &a, &iter);
                            g_free (text);
                            g_free (text2);   
                            continue;
                           }

                         if (g_hash_table_lookup (pas_t_types, text) != NULL)  
                            {
                             gtk_text_buffer_apply_tag (doc->text_buffer, tag_type, &a, &iter);
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

  //end keywords

  gtk_text_buffer_get_iter_at_offset (doc->text_buffer, &iter, 0);
  a = iter;
  c = iter;
 
  do
    {
     a = iter;
        
     switch (gtk_text_iter_get_char (&iter))
            {
     
             case '"': 
                      {
                       if (gtk_text_iter_forward_find_char (&iter, find_q, NULL, NULL))
                          if (gtk_text_iter_forward_char (&iter)) 
                             gtk_text_buffer_apply_tag (doc->text_buffer, tag_string, &a, &iter);
                       break;
                      }
 
             case '\'':             
                       {
                        if (gtk_text_iter_forward_find_char (&iter, find_q2, NULL, NULL))
                           if (gtk_text_iter_forward_char (&iter))
                              gtk_text_buffer_apply_tag (doc->text_buffer, tag_string, &a, &iter);
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
                                      gtk_text_buffer_remove_all_tags (doc->text_buffer, &a, &iter);                
                                      gtk_text_buffer_apply_tag (doc->text_buffer, tag_comment, &a, &iter);
                                  }
                            }
                        break;   
                       }  
             }        
    }
    while ( gtk_text_iter_forward_char (&iter) );
}


//n.p. Scorn - Vae Solis - Suck And Eat You
void do_python_hl (t_note_page *doc)
{
   GtkTextIter iter;
   GtkTextIter a;
   GtkTextIter b;
   GtkTextIter c;
   GtkTextIter d;

   remove_tags (doc);

   gchar *text;

   gtk_text_buffer_get_iter_at_offset (doc->text_buffer, &iter, 0);

   if (gtk_text_iter_starts_word (&iter))
      {
       a = iter;
       c = iter;
      }

   //keywords
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
                             gtk_text_buffer_apply_tag (doc->text_buffer, tag_digit, &a, &iter);
                             g_free (text);
                             continue;
                            }

                         if (g_hash_table_lookup (py_t_keywords, text) != NULL)  
                            {
                             gtk_text_buffer_apply_tag (doc->text_buffer, tag_identifier, &a, &iter);
                             g_free (text);
                             continue;
                            }
                        }
                }
              }
          }
        }

  while ( gtk_text_iter_forward_char (&iter));

  //end keywords

  gtk_text_buffer_get_iter_at_offset (doc->text_buffer, &iter, 0);
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
                              gtk_text_buffer_remove_all_tags (doc->text_buffer, &a, &iter);
                              gtk_text_buffer_apply_tag (doc->text_buffer, tag_comment, &a, &iter);
                             }
                       break;
                      }

             case '"': 
                      {
                       if (gtk_text_iter_forward_find_char (&iter, find_q, NULL, NULL))
                          if (gtk_text_iter_forward_char (&iter)) 
                             gtk_text_buffer_apply_tag (doc->text_buffer, tag_string, &a, &iter);
                       break;
                      }
 
             case '\'':             
                       {
                        if (gtk_text_iter_forward_find_char (&iter, find_q2, NULL, NULL))
                           if (gtk_text_iter_forward_char (&iter))
                              gtk_text_buffer_apply_tag (doc->text_buffer, tag_string, &a, &iter);
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
   if (strcmp (doc->hl_mode, HL_PO) == 0)
      auto_hl_griffon_perl ();//do_hl_po (doc);      
}


typedef struct 
              {
               gboolean start; 
               gboolean end;
              } t_s_switcher;
                  

static gboolean find_tag_end (gunichar ch, gpointer user_data)
{
  if (ch == '>')
     return TRUE;
  else
      return FALSE;
}

//n.p. Public Image Limited "Another"
void do_hl_spell_check2 (t_note_page *doc, gchar *lang)
{
/*#ifdef HAVE_LIBASPELL

  AspellConfig *spell_config = new_aspell_config(); 

  AspellCanHaveError *ret;
  AspellSpeller *speller;
  
  aspell_config_replace (spell_config, "lang", lang); 
  aspell_config_replace (spell_config, "encoding", "UTF-8"); 

  ret = new_aspell_speller(spell_config);
  if (aspell_error (ret) != 0)
     {
      g_print ("Error: %s\n", aspell_error_message(ret));
      delete_aspell_config (spell_config); 
      delete_aspell_can_have_error(ret);
      return;
     }

  speller = to_aspell_speller (ret);

  GtkTextIter iter;
  GtkTextIter a;
  GtkTextIter b;
  GtkTextIter c;
  GtkTextIter d;

  gchar *p = NULL; 

  remove_tags (doc);

  gchar *text;
  gchar *text2;
  gtk_text_buffer_get_iter_at_offset (doc->text_buffer, &iter, 0);

  if (gtk_text_iter_starts_word (&iter))
     {
      a = iter;
      c = iter;
     }

   //keywords
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

                    if (g_utf8_strlen (text, -1) > 1) 
                    if (text)
                       {
                        if (aspell_speller_check (speller, text, -1) == 0)
                            {
                             gtk_text_buffer_apply_tag (doc->text_buffer, tag_spell_err, &a, &iter);
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
 
  delete_aspell_speller (speller);
  delete_aspell_config (spell_config);

 #endif
 */
}


void do_errors_hl (GtkTextView *text_view)
{
  GtkTextIter iter;
  GtkTextIter a;
  gchar *filename = NULL;
  gint lineno = 0;

  GtkTextBuffer *text_buffer = gtk_text_view_get_buffer (text_view);

  gchar *text;

  gint c = gtk_text_buffer_get_line_count (text_buffer);
  gint i;

  gtk_text_buffer_get_start_iter (text_buffer, &iter); 

  for (i = 0; i <= c; i++)
      {
       gtk_text_iter_set_line (&iter, i); 
       a = iter;

       if (gtk_text_iter_forward_to_line_end (&a))
          {
           text = gtk_text_iter_get_text (&iter, &a);
             
           if (parse_error_line (text, &filename, &lineno))
              {
               gtk_text_buffer_remove_all_tags (text_buffer, &iter, &a);
               gtk_text_buffer_apply_tag_by_name (text_buffer, "lm_error", &iter, &a);
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
