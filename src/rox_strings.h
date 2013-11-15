
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
                           

typedef struct {
               gchar *word;
               gint count;  
              } t_struct_word;

                   
void free_word_data (gpointer data);  

gchar* get_file_ver_name (const gchar *name);
gboolean filename_test_extensions (gchar **extensions, gchar *filename);
size_t tNumber2Roman (guint uiNumber, int bUpperCase, char *szOutput);
gchar *strip_any_whitespace (gchar *string);

GList* parse_string_to_list_clean (gchar *s);
GList* parse_string_to_list (gchar *s);
GList* parse_sep_string_to_list (gchar *s, gchar *sep);
GList* add_to_glist_combined_st (GList *list, gchar* key, gchar *value);
GList* add_to_glist_combined_int (GList *list, gchar* key, gint value);
GList* remove_blank_lines (GList *lines);
GList* glist_from_string (const gchar *string);
void glist_strings_free (GList *list);
GList* glist_strings_sort (GList *list);
gchar* string_from_glist (GList *list);
GList* glist_trim_l (GList* list, gint n);
GList* glist_trim_l_on_char (GList* list, gchar n);
GList* glist_enclose (GList* list, gchar *a, gchar *b);
void glist_print (GList *list);
void glist_save_to_file (GList *list, gchar *filename);
GList* load_file_to_glist (gchar *filename);
GList* add_to_glist_combined_st (GList *list, gchar *key, gchar *value);
GList* add_to_glist_combined_int (GList *list, gchar *key, gint value);
GList* filter_exclude_from_list (GList *list, gchar const *phrase);
GList* filter_antiexclude_from_list (GList *list, gchar const *phrase);
GList* find_item_by_data (GList *list, gchar const *s);
GList* str_in_glist (GList *list, gchar *st);
gchar* rep_all_s (gchar *s, gchar *r);
gchar* str_file_read (const gchar *filename);
GList* glist_shuffle (GList *list);
gchar* glist_enum (gchar *buf, gchar const *format_str);
gboolean save_string_to_file (gchar *filename, gchar *s);
gboolean save_string_to_file_add (gchar *filename, gchar *s);
gchar * str_replace_all(char const * const original,char const * const pattern,char const * const replacement);
GList* add_to_list_with_limit (GList *list, gchar *s, guint limit);
GList* add_to_list_with_limit2 (GList *list, gchar *s, guint limit);
GList* glist_repl (GList* list, gchar const *format);
GHashTable* load_file_to_hashtable (gchar *filename);
void print_ht (GHashTable *ht);
gchar* str_crackerize (gchar *buf);
GList* add_to_glist_combined_bool (GList *list, gchar *key, gboolean value);
GList* add_to_glist_combined_str (GList *list, gchar *key, gchar *value);
gchar* str_eol_to_crlf (gchar *buf);
gint get_gchar_array_size (gchar **a);
gchar* morse_encode (gchar *s);
gchar* morse_decode (gchar *s);
GList* ht_to_glist (GHashTable *hash_table, gint option);
gchar* ch_str (gchar *s, gchar const *new_val);
GList* each_line_remove_lt_spaces (GList *lines, gint mode);
gint glist_get_str_index (GList *l, gchar *s);
gchar* get_c_header_fname (gchar *f);
gchar* get_c_source_fname (gchar *f);
gchar* ht_get_gchar_value (GHashTable *hash_table, gchar *key, gchar *def);
gboolean save_string_to_file_vide (gchar *filename, gchar *s);
gchar* linkify_text (const gchar *text);
gchar *strip_html(const gchar *text);
gchar* str_kill_dups (const gchar *s);
GList* sort_list_case_insensetive (GList *list);
gchar* kill_formatting (gchar *s);
gchar* wrap_raw (gchar *s, gint str_len);
void wrap_on_spaces (gchar *str, gint width);
gchar* kill_formatting_on_each_line (gchar *s);
gchar* kill_lesser (gchar *s, gint max);
gchar* kill_larger (gchar *s, gint max);
GList* glist_word_sort_mode (GList *list, gint mode);
gchar* g_str_replace (const gchar *input, const gchar *replace, const gchar *with);
void glist_strings_free (GList *list);
gchar* text_load_rtf (const gchar *filename);
gchar* enc_guess (const gchar *s);
void str_walk_extract_word (gpointer key);
void walk_by_words (gpointer value);
void cb_print_ht (gpointer key,gpointer value);
GList* load_file_to_glist_with_limit (gchar *filename, gint limit);
