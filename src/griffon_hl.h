
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

GtkTextTag *tag_spell_err;
GtkTextTag *tag_comment;
GtkTextTag *tag_identifier;
GtkTextTag *tag_digit;
GtkTextTag *tag_string;
GtkTextTag *tag_html_tag;
GtkTextTag *tag_preprocessor;
GtkTextTag *tag_type;

GtkTextTagTable *main_tags_table;

GHashTable *c_t_keywords;
GHashTable *c_t_types;

GHashTable *pas_t_keywords;
GHashTable *pas_t_types;

GHashTable *py_t_keywords;

GHashTable *php_t_keywords;
GHashTable *bash_t_keywords;
GHashTable *po_t_keywords;

void do_errors_hl (GtkTextView *text_view);
void set_lm_colors (void);
void clear_remove_tags (t_note_page *doc);
void apply_hl (t_note_page *doc);
void remove_tags (t_note_page *doc);
void create_tags (void);
void prepare_hl_all (void);
void add_recent_internal (t_note_page *doc);
GList* add_recent_item_composed (GList *list, t_note_page *doc);
void do_hl_spell_check2 ();
void doc_spaces_to_tabs (t_note_page *doc, gint tabsize);
void doc_tabs_to_spaces (t_note_page *doc, gint tabsize) ;
void run_extract_words ( t_note_page *page);
void run_unitaz ( t_note_page *page, gint sort_type, gboolean case_insensetive);
