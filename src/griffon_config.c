
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

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <glib.h>
#include<stdio.h>
#include<stdlib.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguage.h>
#include "griffon_text_document.h"
#include "callbacks.h"
#include "interface.h"
#include "griffon_defs.h"
#include "griffon_config.h"
#include "griffon_hl.h"
#include "griffon_proj.h"
#include "griffon_gtk_utils.h"
#include "griffon_funx.h"
#include "rox_strings.h"

static GHashTable *ht_tea_hotkeys = NULL;

void ui_init (void)
{
	cur_tea_project = NULL;
	GList *ui = NULL;
	gl_famous = NULL;
	gl_kwas_history = NULL;
	gl_tea_kwas_bookmarks = NULL;
	ui = load_file_to_glist (confile.tea_ui);
	gl_famous = load_file_to_glist (confile.tea_gl_famous);
	gl_kwas_history = load_file_to_glist (confile.tea_kwas_history);
	gl_tea_kwas_bookmarks = load_file_to_glist (confile.tea_kwas_bookmarks);

	confile.last_prefs_tab = conf_get_int_value (ui, "last_prefs_tab", 0); 
	gtk_paned_set_position (GTK_PANED(vpaned1), 50);                  

	if (ui)
	glist_strings_free (ui);
}

void ui_done (void)
{
	GList *list = NULL;

	list = add_to_glist_combined_int (list, "vpaned1_y", gtk_paned_get_position  (GTK_PANED(vpaned1)));

	gint x;
	gint y;

	gtk_window_get_position (GTK_WINDOW (tea_main_window), &x, &y);

	list = add_to_glist_combined_int (list, "last_prefs_tab", confile.last_prefs_tab);
	list = add_to_glist_combined_int (list, "main_wnd_x", x);
	list = add_to_glist_combined_int (list, "main_wnd_y", y);

	gtk_window_get_size (GTK_WINDOW (tea_main_window), &x, &y);

	list = add_to_glist_combined_int (list, "main_wnd_w", x);
	list = add_to_glist_combined_int (list, "main_wnd_h", y);

	glist_save_to_file (list, confile.tea_ui);

	g_list_free (confile.gl_save_charsets);
	g_list_free (list);
	glist_strings_free (gl_famous);
	glist_strings_free (gl_kwas_history);
	glist_strings_free (gl_tea_kwas_bookmarks);
	glist_strings_free (gl_autocomp);

	tea_proj_free (cur_tea_project);
}

GList* parse_conf_file_to_list (gchar *filename)
{
	gsize length;
	gchar *buf;
	GList *result = NULL;

	if  (! g_file_get_contents (filename, &buf, &length, NULL))
		return NULL;

	result = parse_string_to_list (buf);
	g_free (buf);
	return result;
}

gchar* conf_get_char_value (GList *confdata, gchar *key, gchar *def)
{
	if (! confdata)
		return g_strdup (def);

	gchar *result = NULL;
	gchar **a = NULL;

	GList *p = g_list_first (confdata);

	while (p)
	{
		a = g_strsplit ((gchar*) p->data, "=", 2);

		if (a)
		{
			if (a[0])
			if ((g_utf8_collate (a[0], key) == 0))
			{
				if (a[1])
				{
					if (g_utf8_collate (a[1], "") == 0)
						result = strdup (def);
					else
						result = strdup (a[1]);
				}
			g_strfreev (a);
			return result;
			}

		g_strfreev (a);
		p = g_list_next (p);
		}
	}

	if (! def)
	return NULL;

	return strdup (def);
}

gint conf_get_int_value (GList *confdata, gchar *key, gint def)
{
	if (! confdata)
		return def;

	gint result = def;

	gchar *s = g_strdup_printf ("%d", def);  
	gchar *res = conf_get_char_value (confdata, key, s);

	if (! res)
	{
		g_free (s);
		return def;
	}

	result = strtol (res, NULL, 10);

	g_free (res);
	g_free (s);

	return result;
}

void confile_free (void)
{
	glist_strings_free (confile.confdata);

	g_free (confile.def_filesave_charset);
	g_free (confile.tea_kwas_history);
	g_free (confile.tea_kwas_bookmarks);
	g_free (confile.user_menu_file);
	g_free (confile.crapbook_file);
	g_free (confile.snippets_dir);
	g_free (confile.helps_dir);
	g_free (confile.bmx_file);
	g_free (confile.date_time);
	g_free (confile.tea_main_dir);
	g_free (confile.templates_dir);
	g_free (confile.tmp_dir);
	g_free (confile.tmp_file);
	g_free (confile.iconv_file);
	g_free (confile.tea_todo);
	g_free (confile.tea_sftp);
	g_free (confile.tea_miniweb_history);
	g_free (confile.tea_myadmin_history);
	g_free (confile.tea_cmd_history);
	g_free (confile.tea_theme);  
	g_free (confile.theme);
	g_free (confile.projects);
	g_free (confile.tea_ftp);
	g_free (confile.file_tmp);  
	g_free (confile.tea_myadmin);  
	g_free (confile.sessions_dir);
	g_free (confile.tea_rc);
	g_free (confile.tea_ui);
	g_free (confile.tea_gl_famous);
	g_free (confile.tea_recent);
	g_free (confile.tea_hotkeys);
	g_free (confile.def_save_dir);
	g_free (confile.def_open_dir);
	g_free (confile.cmd_def_doc_browser);
	g_free (confile.cmd_Opera);
	g_free (confile.cmd_Firefox);
	g_free (confile.cmd_Mozilla);
	g_free (confile.cmd_Konqueror);
	g_free (confile.cmd_Galeon);
	g_free (confile.cmd_Dillo);
	g_free (confile.cmd_Links);
	g_free (confile.cmd_ELinks);
	g_free (confile.cmd_Lynx);
	g_free (confile.cmd_Epiphany);
	g_free (confile.cmd_Other);
	g_free (confile.fmt_color_function);
	g_free (confile.text_foreground);
	g_free (confile.text_background);
	g_free (confile.text_sel_foreground);
	g_free (confile.text_sel_background);
	g_free (confile.tag_color_lm_error_fg);
	g_free (confile.tag_color_lm_error_bg);
	g_free (confile.tag_comment);
	g_free (confile.tag_identifier);
	g_free (confile.tag_digit);
	g_free (confile.tag_string);
	g_free (confile.tag_html_tag);
	g_free (confile.tag_preprocessor);
	g_free (confile.tag_type);
	g_free (confile.tag_spellcheck);
	g_free (confile.tag_comment_font);
	g_free (confile.tag_identifier_font);
	g_free (confile.tag_digit_font);
	g_free (confile.tag_string_font);
	g_free (confile.tag_html_tag_font);
	g_free (confile.tag_preprocessor_font);
	g_free (confile.tag_type_font);
	g_free (confile.editor_font);
	g_free (confile.rtf_enc);
	g_free (confile.cm_mplayer);
	g_free (confile.default_charset);
	g_free (confile.tea_autocomp);
	g_free (confile.tea_autoreplace);
	g_free (confile.changelog);
}

void get_iconv_sup (void)
{
	gchar *cmd = g_strconcat ("iconv -l > ", confile.iconv_file, NULL);
	if (! g_file_test (confile.iconv_file, G_FILE_TEST_EXISTS))
		if(system (cmd)== -1){printf("\n");}

	g_free (cmd);
	gsize length;
	gchar *buf;

	if (! g_file_get_contents (confile.iconv_file, &buf, &length, NULL))
		return;

	if (confile.iconv_encs)
		g_list_free (confile.iconv_encs);

	if (confile.gl_save_charsets)
		g_list_free (confile.gl_save_charsets);

	confile.iconv_encs = NULL;
	confile.gl_save_charsets = NULL;
	confile.iconv_encs = parse_sep_string_to_list (buf, "//");
	confile.iconv_encs = g_list_prepend (confile.iconv_encs, g_strdup (CURR_LOCALE));
	confile.iconv_encs = g_list_prepend (confile.iconv_encs, g_strdup ("UTF-8"));
	confile.gl_save_charsets = g_list_copy (confile.iconv_encs);
	confile.iconv_encs = g_list_prepend (confile.iconv_encs, g_strdup (CHARSET_JAPANESE));
	confile.iconv_encs = g_list_prepend (confile.iconv_encs, g_strdup ("autodetect"));
	cur_settings.selected_enc = ch_str (cur_settings.selected_enc, "UTF-8");

	g_free (buf);
}


void execute_recent_item (gchar const *item)
{
	if (! item) return;

	if (g_utf8_strlen (item, -1) <= 2)
		return;

	gchar **a = NULL;
	gchar *f = NULL; 
	a = g_strsplit (item, ",", -1);

	if (! a)
		return;

	if (! a[0])
		return;

	if (! a[1])
		return;

	if (! a[2])
		return;

	guint pos = strtol (a[2], NULL, 10);

	f = get_l_filename (a[0]);

	if (! g_file_test ( f, G_FILE_TEST_EXISTS))
	{
		log_to_memo (_("%s is not exists"), f, LM_ERROR);
		g_strfreev (a);
		return;
	}

	cur_settings.selected_enc = ch_str (cur_settings.selected_enc, a[1]);
	cur_text_doc = doc_open_file (f);

	if (cur_text_doc)
	{
		set_title (cur_text_doc);
		editor_set_pos (cur_text_doc, pos);
	}

	g_strfreev (a);
	g_free (f);
}

void update_enc_menu (void)
{
	gtk_widget_destroy (mni_co_menu);
	mni_co_menu = new_menu_submenu (mni_co);
	build_menu_from_glist (confile.iconv_encs, mni_co_menu, on_mni_co_select);
}

void bmx_reload (void)
{
	glist_strings_free (bookmarks);

	bookmarks = load_file_to_glist (confile.bmx_file);

	if (! bookmarks)
		return;

	gtk_widget_destroy (mni_bookmarks_menu);
	mni_bookmarks_menu = new_menu_submenu (mni_bookmarks_root);
	build_menu_from_glist (bookmarks, mni_bookmarks_menu, on_mni_recent_activate);
}

void update_recent_list_menu (gboolean load_from_file)
{
	if (! mni_recent_list)
		return;

	if (load_from_file)
	{
		glist_strings_free (recent_list);
		recent_list = load_file_to_glist_with_limit (confile.tea_recent, confile.max_recent_items); 
	}

	if (! recent_list)
		return;

	gtk_widget_destroy (mni_recent_menu);
	mni_recent_menu = new_menu_submenu (mni_recent_list);

	gint c = -1;
	GList *p = g_list_first (recent_list);

	while (p)
	{
		if (++c == confile.max_recent_items)
			break;

		if (p->data)
			if ( strcmp (p->data, "") != 0 )
				mni_temp = new_menu_item ((gchar*)p->data, mni_recent_menu, on_mni_real_recent_activate);
				p = g_list_next (p);
	}
}

void tea_start (void)
{
	sr_wnd = NULL;
	ht_user_menu = NULL;
	ht_autoreplace = NULL;

	ht_entities = NULL;
	mni_autocomp = NULL;
	dox = NULL;
	bookmarks = NULL;
	confile.msg_counter = 0;
	confile.iconv_encs = NULL;
	confile.current_movie = NULL;
	cur_settings.selected_enc = g_strdup (confile.default_charset);
}

void tea_done (void)
{
	t_note_page *page;
	mni_tabs = NULL;

	GList *p = g_list_first (dox);
	while (p)
	{
		page = (t_note_page*) p->data;
		page_free (page);
		page->text_buffer = NULL;
		p = g_list_next (p);
	}

	glist_save_to_file (recent_list, confile.tea_recent);

	g_list_free (dox);
	glist_strings_free (recent_list);
	glist_strings_free (bookmarks);

	if (ht_tea_hotkeys)
		g_hash_table_destroy (ht_tea_hotkeys); 

	if (ht_user_menu)
		g_hash_table_destroy (ht_user_menu); 

	if (ht_entities)
		g_hash_table_destroy (ht_entities); 

	if (ht_autoreplace)
		g_hash_table_destroy (ht_autoreplace); 

	g_hash_table_destroy (po_t_keywords); 
	g_hash_table_destroy (bash_t_keywords); 
	g_hash_table_destroy (php_t_keywords); 
	g_hash_table_destroy (c_t_keywords); 
	g_hash_table_destroy (c_t_types); 
	g_hash_table_destroy (pas_t_keywords); 
	g_hash_table_destroy (pas_t_types);
	g_hash_table_destroy (py_t_keywords); 

	unlink (confile.tmp_file);

	confile_free ();
}

GdkModifierType string_to_mod (const gchar *s)
{
	if (! s)
		return 0;

	GdkModifierType result = 0;

	if (strstr (s, "SHIFT") != 0)
		result = result | GDK_SHIFT_MASK;

	if (strstr (s, "CTRL") != 0)
		result = result | GDK_CONTROL_MASK;

	if (strstr (s, "ALT") != 0)
		result = result | GDK_MOD1_MASK;

	return result;
}

void lookup_widget_cb (GtkWidget *widget, gpointer data)
{
	if (! widget)
		return;

	if (! ht_tea_hotkeys)
		return;

	gchar *value = NULL;
	gchar *key;
	gchar *t;
	gchar const *s = gtk_widget_get_name (widget);

	if (strcmp ("-x-", s) != 0)
	{
		value = g_hash_table_lookup (ht_tea_hotkeys, s);

		if (value)
		{
			t = g_strrstr (value, " "); 
			if (! t)
				key = g_strdup (value);
			else 
				key = g_strchug (g_strdup (t));
				gtk_widget_add_accelerator (widget, "activate", accel_group,gdk_keyval_from_name (key), string_to_mod (value),GTK_ACCEL_VISIBLE);
				g_free (key); 
		}

		if (GTK_IS_MENU_ITEM (widget))
			lookup_widget ((GtkContainer *)gtk_menu_item_get_submenu (GTK_MENU_ITEM(widget)), data);      
	}
}

GtkWidget* lookup_widget (GtkContainer *widget, gchar *widget_name)
{
	if (widget)
		gtk_container_foreach (widget, lookup_widget_cb, widget_name);

	return NULL;
}

void reload_hotkeys (void){}

void reload_snippets (void)
{
	if (! confile.use_snippets)
		return;

	if (! g_file_test (confile.snippets_dir, G_FILE_TEST_IS_DIR))
		if (mkdir (confile.snippets_dir, S_IRUSR | S_IWUSR | S_IXUSR) == -1)
			return;

	gtk_widget_destroy (mni_snippets_menu);
	mni_snippets_menu = new_menu_submenu (mni_snippets);

	GList *l = read_dir_files (confile.snippets_dir);
	build_menu_from_glist (l, mni_snippets_menu, on_mni_snippet_click);
	glist_strings_free (l);
}

void tabs_reload (void)
{
	if (! mni_tabs)
		return;

	gtk_widget_destroy (mni_tabs_menu);
	mni_tabs_menu = new_menu_submenu (mni_tabs);

	GList *p = g_list_first (dox);
	t_note_page *doc;

	while (p)
	{
		doc = p->data;
			if (doc)                 
				mni_temp = new_menu_item_with_udata (doc->file_name, mni_tabs_menu, on_mni_tabs_menuitem_click, doc);
				p = g_list_next (p);
	}
}

void reload_dicts (void){}

gchar* get_tmp_file_string (void)
{
	return str_file_read (confile.tmp_file); 
}

void reload_usermenu (void)
{
	if (! confile.user_menu_file)
		return;

	if (! g_file_test (confile.user_menu_file, G_FILE_TEST_EXISTS))
		return;

	if (ht_user_menu)
		g_hash_table_destroy (ht_user_menu); 

	ht_user_menu = load_file_to_hashtable (confile.user_menu_file);

	gtk_widget_destroy (mni_user_menu_menu);
	mni_user_menu_menu = new_menu_submenu (mni_user_menu);
	mni_temp = new_menu_tof (mni_user_menu_menu);

	build_menu_from_ht (ht_user_menu, mni_user_menu_menu, on_mni_user_menu_item);
}

void fill_entities (void)
{
	ht_entities = g_hash_table_new (g_str_hash, g_str_equal);

	g_hash_table_insert (ht_entities, _("space"), "160");
	g_hash_table_insert (ht_entities, _("cent sign"), "162");
	g_hash_table_insert (ht_entities, _("pound sign"), "163");
	g_hash_table_insert (ht_entities, _("currency sign"), "164");
	g_hash_table_insert (ht_entities, _("yen sign"), "165");
	g_hash_table_insert (ht_entities, _("section sign"), "167");
	g_hash_table_insert (ht_entities, _("copyright sign"), "169");
	g_hash_table_insert (ht_entities, _("a sign marks"), "8482");
	g_hash_table_insert (ht_entities, _("feminine ordinal indicator \"ª\""), "170");
	g_hash_table_insert (ht_entities, _("left-pointing angle quotation mark double"), "171");
	g_hash_table_insert (ht_entities, _("hyphen"), "173");
	g_hash_table_insert (ht_entities, _("registered sign"), "174");
	g_hash_table_insert (ht_entities, _("degree sign"), "176");
	g_hash_table_insert (ht_entities, _("plus-minus sign"), "177");
	g_hash_table_insert (ht_entities, _("right-pointing angle quotation mark double"), "187");
	g_hash_table_insert (ht_entities, _("left single quotation mark"), "8216");
	g_hash_table_insert (ht_entities, _("right single quote"), "8217");
	g_hash_table_insert (ht_entities, _("euro symbol"), "8364");
	g_hash_table_insert (ht_entities, _("grec symbol pi"), "982");
	g_hash_table_insert (ht_entities, _("left arrow"), "8592");
	g_hash_table_insert (ht_entities, _("up arrow"), "8593");
	g_hash_table_insert (ht_entities, _("right arrow"), "8594");
	g_hash_table_insert (ht_entities, _("down arrow"), "8595");
	g_hash_table_insert (ht_entities, _("square root"), "8730");
	g_hash_table_insert (ht_entities, _("masculine ordinal indicator \"º\""), "186");
	g_hash_table_insert (ht_entities, _("quote"), "34");
	g_hash_table_insert (ht_entities, _("&"), "38");
	g_hash_table_insert (ht_entities, _("less than sign"), "60");
	g_hash_table_insert (ht_entities, _("greater than sign"), "62");
	g_hash_table_insert (ht_entities, _("apostrophe"), "39");
}

void reload_sessions (void)
{
	if (! g_file_test (confile.sessions_dir, G_FILE_TEST_IS_DIR))
		if ( mkdir (confile.sessions_dir, S_IRUSR | S_IWUSR | S_IXUSR) == -1)
			return;

	gtk_widget_destroy (mni_sessions_menu);
	mni_sessions_menu = new_menu_submenu (mni_sessions);

	GList *l = read_dir_files (confile.sessions_dir);
	build_menu_from_glist (l, mni_sessions_menu, on_mni_sessions_click);
	glist_strings_free (l);
}

void reload_templates (void)
{
	if (! g_file_test (confile.templates_dir, G_FILE_TEST_IS_DIR))
		if (mkdir (confile.templates_dir, S_IRUSR | S_IWUSR | S_IXUSR) == -1)
			return;
 
	gtk_widget_destroy (mni_templates_menu);
	mni_templates_menu = new_menu_submenu (mni_templates);

	GList *l = read_dir_files (confile.templates_dir);
	build_menu_from_glist (l, mni_templates_menu, on_mni_templates_click);
	glist_strings_free (l);
}

gchar* ht_get_char_value (gchar *key, gchar *def)
{
	if (! ht_config)
		return g_strdup (def);

	gchar *v = g_hash_table_lookup (ht_config, key);
	if (v)
		return g_strdup (v);

	return strdup (def);
}  

gint ht_get_int_value (gchar *key, gint def)
{
	if (! ht_config)
		return def;

	gint result;

	gchar *s = g_strdup_printf ("%d", def);  
	gchar *res = ht_get_char_value (key, s);

	if (! res)
	{
		g_free (s);
		return def;
	}

	result = strtol (res, NULL, 10);

	g_free (res);
	g_free (s);

	return result;
}

void confile_reload (void)
{
	confile_free ();
	confile.confdata = NULL;

	confile.screen_w = gdk_screen_width ();
	confile.screen_h = gdk_screen_height ();

	gchar const *s = g_getenv ("HOME");

	confile.tea_main_dir = g_strconcat (s, G_DIR_SEPARATOR_S, ".griffon", NULL);
	confile.tmp_dir = g_strdup (g_get_tmp_dir ());
	confile.tmp_file = g_strconcat (confile.tmp_dir, G_DIR_SEPARATOR_S, "griffon_temp", NULL);
	confile.user_menu_file = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "griffon_user_menu", NULL); 
	confile.tea_autocomp = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "griffon_autocomp", NULL); 
	confile.tea_autoreplace = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "griffon_autoreplace", NULL); 
	confile.bmx_file = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "griffon_bmx", NULL); 
	confile.sessions_dir = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "sessions", G_DIR_SEPARATOR_S, NULL);
	confile.snippets_dir = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "snippets", G_DIR_SEPARATOR_S, NULL);
	confile.helps_dir = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "helps_custom", G_DIR_SEPARATOR_S, NULL);
	confile.templates_dir = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "templates", G_DIR_SEPARATOR_S, NULL);
	confile.iconv_file = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "iconv_rc", NULL);
	confile.crapbook_file = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "crapbook.txt", NULL);
	confile.tea_todo = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "griffon_todo", NULL);
	confile.tea_myadmin = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "griffon_myadmin", NULL); 
	confile.tea_sftp = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "griffon_sftp", NULL);
	confile.tea_miniweb_history = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "minieb_history", NULL);
	confile.tea_myadmin_history = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "myadmin_history", NULL);
	confile.tea_cmd_history = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "cmd_history", NULL);
	confile.tea_theme = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "griffon_theme", NULL); 
	confile.projects = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "griffon_projects", NULL); 
	confile.tea_ftp = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "griffon_ftp", NULL);
	confile.file_tmp = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "griffon_tmp", NULL);
	confile.tea_rc = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "griffon_rc", NULL);
	confile.tea_ui = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "griffon_ui", NULL);
	confile.tea_gl_famous = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "griffon_gl_famous", NULL);
	confile.tea_kwas_history = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "griffon_kwas_history", NULL);
	confile.tea_kwas_bookmarks = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "griffon_kwas_bookmarks", NULL);
	confile.tea_recent = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "griffon_recent", NULL);
	confile.tea_hotkeys = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "griffon_hotkeys", NULL);
	confile.changelog = g_strconcat (confile.tea_main_dir, G_DIR_SEPARATOR_S, "changelogs", NULL);

	if (! g_file_test (confile.tea_main_dir, G_FILE_TEST_IS_DIR))
	{
		if (chdir (s)){printf("\n");}

		if (mkdir (".griffon", S_IRUSR | S_IWUSR | S_IXUSR) == -1) 
			DBM ("mkdir griffon_dir failed");

		if (chdir (s)){printf("\n");}

			if (mkdir ("templates", S_IRUSR | S_IWUSR | S_IXUSR) == -1)
				DBM ("mkdir templates failed");

			if (mkdir ("sessions", S_IRUSR | S_IWUSR | S_IXUSR) == -1)
				DBM ("mkdir sessions failed");

			if (mkdir ("todo", S_IRUSR | S_IWUSR | S_IXUSR) == -1)
				DBM ("mkdir todo failed");

			if (chdir (s)){printf("\n");}
	}

	if (! g_file_test (confile.changelog, G_FILE_TEST_IS_DIR))
	{
		if (chdir (confile.tea_main_dir)){printf("\n");}

			if (mkdir ("changelogs", S_IRUSR | S_IWUSR | S_IXUSR) == -1)
				DBM ("mkdir changelogs failed");

			if (chdir (s)){printf("\n");}
	}

	if (! g_file_test (confile.helps_dir, G_FILE_TEST_IS_DIR))
	{
		if (chdir (confile.tea_main_dir)){printf("\n");}

			if (mkdir ("helps_custom", S_IRUSR | S_IWUSR | S_IXUSR) == -1)
				DBM ("mkdir helps_custom failed");

			if (chdir (s)){printf("\n");}
	}


	if (! g_file_test (confile.tea_rc, G_FILE_TEST_EXISTS))
		create_empty_file (confile.tea_rc, "#Fichier de configuration Griffon IDE.");

	if (! g_file_test (confile.tea_todo, G_FILE_TEST_EXISTS))
		create_empty_file (confile.tea_todo, "");

	if (! g_file_test (confile.tea_sftp, G_FILE_TEST_EXISTS))
		create_empty_file (confile.tea_sftp, "");

	if (! g_file_test (confile.tea_theme, G_FILE_TEST_EXISTS))
		{create_empty_file (confile.tea_theme, "classic\n");confile.theme="classic";}

	if (! g_file_test (confile.projects, G_FILE_TEST_EXISTS))
		{create_empty_file (confile.projects, "");}

	if (! g_file_test (confile.tea_ftp, G_FILE_TEST_EXISTS))
		create_empty_file (confile.tea_ftp, "");

	if (! g_file_test (confile.file_tmp, G_FILE_TEST_EXISTS))
		create_empty_file (confile.file_tmp, "");

	if (! g_file_test (confile.tea_cmd_history, G_FILE_TEST_EXISTS))
		create_empty_file (confile.tea_cmd_history, "");

	if (! g_file_test (confile.tea_myadmin, G_FILE_TEST_EXISTS))
		create_empty_file (confile.tea_myadmin, "http://search.cpan.org/\nhttp://www.php.net/manual/fr/index.php\nhttp://developer.gnome.org/gtk2/stable/\nhttp://PhpMyadmin1\nhttp://PhpMyadmin2\nhttp://PhpMyadmin3\n");

	if (! g_file_test (confile.tea_hotkeys, G_FILE_TEST_EXISTS))
		create_empty_file (confile.tea_hotkeys, "#Griffon hotkeys file. It MUST be in UTF-8! Please read the manual");

	if (! g_file_test (confile.tea_autoreplace, G_FILE_TEST_EXISTS))
		create_empty_file (confile.tea_autoreplace, "Linux=GNU/Linux");

	ht_config = NULL;
	ht_config = load_file_to_hashtable (confile.tea_rc);

	gchar *x = g_strconcat (s, G_DIR_SEPARATOR_S, NULL);
	confile.def_save_dir = ht_get_char_value ("def_save_dir", x);
	confile.def_open_dir = ht_get_char_value ("def_open_dir", x);
	g_free (x); 

	confile.editor_font = ht_get_char_value ("editor_font", "Monospace 9");
	confile.ext_pic_editor = ht_get_char_value ("ext_pic_editor", "gimp-remote-2.0 -n %s&");
	confile.ext_pic_viewer = ht_get_char_value ("ext_pic_viewer", "display %s&");
	confile.date_time = ht_get_char_value ("date_time", "%d/%m/%Y %T");
	confile.max_recent_items = ht_get_int_value ("max_recent_items", 7);
	confile.famous_history_max = (ht_get_int_value ("famous_history_max", 7) + 1);
	confile.saveopen_h = ht_get_int_value ("saveopen_h", 75);
	confile.saveopen_w = ht_get_int_value ("saveopen_w", 75);
	confile.prompt_on_not_saved = ht_get_int_value ("prompt_on_not_saved", 0);
	confile.prompt_on_file_exists = ht_get_int_value ("prompt_on_file_exists", 0);
	confile.warn_about_aliens = ht_get_int_value ("warn_about_aliens", 0);
	confile.xhtml_mode = ht_get_int_value ("xhtml_mode", 0);
	confile.start_with_blank_file = ht_get_int_value ("start_with_blank_file", 0); 
	confile.start_with_recent_file = ht_get_int_value ("start_with_recent_file", 0); 
	confile.show_line_nums = ht_get_int_value ("show_line_nums", 0);
	confile.word_wrap = ht_get_int_value ("word_wrap", 1);
	confile.tab_size = ht_get_int_value ("tab_size", 3);
	confile.autocomp_wordlen_min = ht_get_int_value ("autocomp_wordlen_min", 3);
	confile.autocomp_list_items_max = ht_get_int_value ("autocomp_list_items_max", 499);
	confile.fmt_color_function = ht_get_char_value ("fmt_color_function", "<span style=\"color:@color;\">@text</span>");
	confile.text_foreground = ht_get_char_value ("text_foreground", "black");
	confile.text_background = ht_get_char_value ("text_background", "white");
	confile.text_sel_foreground = ht_get_char_value ("text_sel_foreground", "white");
	confile.text_sel_background = ht_get_char_value ("text_sel_background", "black");
	confile.tag_color_lm_error_fg = ht_get_char_value ("tag_color_lm_error_fg", "red");
	confile.tag_color_lm_error_bg = ht_get_char_value ("tag_color_lm_error_bg", confile.text_background);
	confile.tag_comment = ht_get_char_value ("tag_comment", "#605454");
	confile.tag_identifier = ht_get_char_value ("tag_identifier", "black");
	confile.tag_digit = ht_get_char_value ("tag_digit", "navy");
	confile.tag_string = ht_get_char_value ("tag_string", "navy");
	confile.tag_html_tag = ht_get_char_value ("tag_html_tag", "black");
	confile.tag_preprocessor = ht_get_char_value ("tag_preprocessor", "#b522a6");
	confile.tag_type = ht_get_char_value ("tag_type", "#3650bd");
	confile.tag_spellcheck = ht_get_char_value ("tag_spellcheck", "red");
	confile.tag_comment_font = ht_get_char_value ("tag_comment_font", "Monospace Italic 9");
	confile.tag_identifier_font = ht_get_char_value ("tag_identifier_font", "Monospace Bold 9");
	confile.tag_digit_font = ht_get_char_value ("tag_digit_font", confile.editor_font);
	confile.tag_string_font = ht_get_char_value ("tag_string_font", confile.editor_font);
	confile.tag_html_tag_font = ht_get_char_value ("tag_html_tag_font", "Monospace Bold 9");
	confile.tag_preprocessor_font = ht_get_char_value ("tag_preprocessor_font", confile.editor_font);
	confile.tag_type_font = ht_get_char_value ("tag_type_font", confile.editor_font);
	confile.cm_mplayer = ht_get_char_value ("cm_mplayer", "mplayer \"@movie_file\" -subcp @encoding -ss @time -sub \"@sub_file\" &");
	confile.cmd_Opera = ht_get_char_value ("opera","opera -remote 'openURL(%s,new-window)' || opera %s&");
	confile.cmd_Firefox = ht_get_char_value ("firefox", "firefox %s&");
	confile.cmd_Mozilla = ht_get_char_value ("mozilla", "mozilla -remote 'openURL(%s, new-window)' || mozilla %s&");
	confile.cmd_Konqueror = ht_get_char_value ("konqueror", "konqueror %s&");
	confile.cmd_Galeon = ht_get_char_value ("galeon", "galeon -x %s&");
	confile.cmd_Dillo = ht_get_char_value ("dillo", "dillo %s&");
	confile.cmd_Links = ht_get_char_value ("links", "konsole -e links %s&");
	confile.cmd_ELinks = ht_get_char_value ("elinks", "konsole -e elinks %s&");
	confile.cmd_Lynx = ht_get_char_value ("lynx", "konsole -e lynx %s&");
	confile.cmd_Epiphany = ht_get_char_value ("epiphany", "epiphany %s&");
	confile.cmd_Other = ht_get_char_value ("other_browser", "%s&");
	confile.cmd_def_doc_browser = ht_get_char_value ("def_doc_browser", "mozilla %s&");
	confile.use_def_doc_browser = ht_get_int_value ("use_def_doc_browser", 0);
	confile.thumb_width = ht_get_int_value ("thumb_width", 180);
	confile.thumb_height = ht_get_int_value ("thumb_height", 180);
	confile.use_auto_indent = ht_get_int_value ("use_auto_indent", 0);
	confile.use_infotext = ht_get_int_value ("use_infotext", 1);
	confile.use_spellcheck = ht_get_int_value ("use_spellcheck", 1);
	confile.use_textcompbloc = ht_get_int_value ("use_textcompbloc", 1);
	confile.do_autorep = ht_get_int_value ("do_autorep", 0);
	confile.do_show_main_toolbar = ht_get_int_value ("do_show_main_toolbar", 1);
	confile.rtf_enc = ht_get_char_value ("rtf_enc", "CP1251");
	confile.def_filesave_charset = ht_get_char_value ("def_filesave_charset", "UTF-8");
	confile.default_charset = ht_get_char_value ("default_charset", "UTF-8");
	confile.main_wnd_show_full_path = ht_get_int_value ("main_wnd_show_full_path", 1);
	confile.do_backup = ht_get_int_value ("do_backup", 0);
	confile.do_hl_on_fileopen = ht_get_int_value ("do_hl_on_fileopen", 1);
	confile.show_hidden_files = ht_get_int_value ("show_hidden_files", 1);
	confile.ins_spaces_on_tab_press = ht_get_int_value ("ins_spaces_on_tab_press", 0);
	confile.use_snippets = ht_get_int_value ("use_snippets", 1);
	confile.use_autocomp = ht_get_int_value ("use_autocomp", 0);
	confile.use_def_save_dir = ht_get_int_value ("use_def_save_dir", 1);
	confile.use_def_open_dir = ht_get_int_value ("use_def_open_dir", 1);
	confile.use_ext_image_viewer = ht_get_int_value ("use_ext_image_viewer", 0);
	confile.do_det_scripts_by_content = ht_get_int_value ("do_det_scripts_by_content", 0);
	confile.enc_det_cp1251 = ht_get_int_value ("enc_det_cp1251", 0); 
	confile.enc_det_koi8r = ht_get_int_value ("enc_det_koi8r", 0);
	confile.enc_det_koi8u = ht_get_int_value ("enc_det_koi8u", 0);
	confile.enc_det_866 = ht_get_int_value ("enc_det_866", 0);
	confile.enc_det_finnish = ht_get_int_value ("enc_det_finnish", 0);
	confile.enc_det_german = ht_get_int_value ("enc_det_german", 0);
	confile.enc_det_serbian = ht_get_int_value ("enc_det_serbian", 0);
	confile.enc_det_latvian = ht_get_int_value ("enc_det_latvian", 0);
	confile.enc_det_polish = ht_get_int_value ("enc_det_polish", 0);
	confile.enc_det_portuguese = ht_get_int_value ("enc_det_portuguese", 0);
	confile.enc_det_slovak = ht_get_int_value ("enc_det_slovak", 0);
	confile.enc_det_slovenian = ht_get_int_value ("enc_det_slovenian", 0);
	confile.enc_det_spanish = ht_get_int_value ("enc_det_spanish", 0);
	confile.enc_det_turkish = ht_get_int_value ("enc_det_turkish", 0);
	confile.scan_for_links_on_doc_open = ht_get_int_value ("scan_for_links_on_doc_open", 0);
	confile.logmemo_lines_max = ht_get_int_value ("logmemo_lines_max", 13);
	g_hash_table_destroy (ht_config);  

	widget_apply_colors (ent_search);
	widget_apply_colors (tv_logmemo);

	get_iconv_sup();
}

void reload_autoreplace (void)
{
	if (! g_file_test (confile.tea_autoreplace, G_FILE_TEST_EXISTS))
		return;
	ht_autoreplace = load_file_to_hashtable (confile.tea_autoreplace);
}
