/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <gtksourceview/gtksourcebuffer.h>

typedef struct
{
  gint position;
  gboolean *readonly;
  GtkWidget *treeview;
  gchar *hl_mode;

  gint dclicked_y;
  gchar *current_path;
  GtkTextTag *basic_tag;
  gboolean b_saved;
  gchar *file_name;
  gchar *encoding;
  gboolean linenums;

  GtkWidget *icon;
  GtkSourceBuffer* text_buffer;
  GtkWidget *tab_label;
  GtkWidget *but;
  GtkWidget *hbox;

  GtkWidget *scrolledwindow;
  GtkWidget *text_view;
  gchar *last_searched_text;


} t_note_page;


GList *dox;

void do_backup (gchar *file_name, gboolean do_check);

t_note_page* doc_clear_new (void);
t_note_page* page_create_new (void);
void page_free (t_note_page *page);
void page_del_by_index (int i);
gpointer glist_find_by_index (GList *list, int i);
t_note_page* get_page_by_index (int i);

gboolean text_doc_save (t_note_page *doc ,char *a_filename);
t_note_page* doc_open_file (gchar *a_filename);
void on_button_close ( GtkWidget *wid,gpointer data);
//cur_text_doc points to current text document (in any case)
t_note_page *cur_text_doc;
gchar* get_c_url (t_note_page *doc) ;
gchar* doc_get_buf (GtkTextBuffer *text_buffer);
int doc_is_sel (GtkTextBuffer *text_buffer);
gboolean doc_get_selection (GtkTextBuffer *text_buffer, gint *start, gint *end);
gchar *doc_get_text (GtkTextBuffer *text_buffer, gint start, gint end);
gboolean doc_save_buffer_to_file (GtkTextBuffer *text_buffer, gchar *filename);
gboolean doc_read_file_to_buffer (gchar *filename);
void doc_select_line (t_note_page *doc, gint line);
void doc_rep_sel (t_note_page *page, const gchar *newstring);
t_note_page* get_page (void);
void doc_apply_settings (t_note_page *doc);
void doc_insert_at_cursor (t_note_page *doc, gchar *text);
void document_set_line_numbers(t_note_page* doc, gboolean value);
void doc_update_all (void);
gint editor_get_pos (t_note_page *doc);
void editor_set_pos (t_note_page *doc, guint pos);
gchar* doc_get_word(t_note_page *doc);
void log_to_memo (gchar *m1, gchar* m2, gint mode);
void widget_apply_colors (GtkWidget *w);
gboolean doc_reload_text (t_note_page *doc, gchar *filename, gchar *enc);
gint get_n_page_by_filename (gchar const *filename);
gboolean find_quote (gunichar ch);
gchar* get_c_url_pure (t_note_page *doc); 
t_note_page* doc_ins_to_new (const gchar *newstring);
void create_new_fnav (void);
t_note_page* get_page_text (void);
void set_title (t_note_page *t);
t_note_page* open_file_std (gchar const *f);
gboolean doc_search_f_next_ncase (t_note_page *doc); 
gboolean doc_search_f_ncase (t_note_page *doc, const gchar *text); 
gchar* upcase_each_first_letter_ofw (t_note_page *doc);
gchar* doc_get_word_at_left (t_note_page *doc, GtkTextIter *itstart, GtkTextIter *itend); 
void doc_move_cursor_backw (t_note_page *doc, gint i); 
void doc_move_cursor_forw (t_note_page *doc, gint i); 
void doc_move_cursor_backw_middle_tags (t_note_page *doc); 
gchar* doc_get_current_word (t_note_page *doc, GtkTextIter *itstart, GtkTextIter *itend);
void indent_real(GtkWidget *text_view);
void doc_header_source_switch ();
gchar* doc_get_sel (t_note_page *doc);
gint cb_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data);
void test_save_onglet ();
void auto_hl_griffon_perl           (void);	
void auto_hl_griffon_c           (void);
void auto_hl_griffon_sh           (void);
void auto_hl_griffon_po           (void);
void auto_hl_griffon_html           (void);
void auto_hl_griffon_php (void);
void auto_hl_griffon_css (void);
void auto_hl_griffon_cpp (void);
void auto_hl_griffon_none (void);
void auto_hl_griffon_xml (void);
void auto_hl_griffon_javascript (void);
void auto_hl_griffon_ruby (void);
void auto_hl_griffon_python (void);
void auto_hl_griffon_java (void);
void theme_cobalt ();
void theme_classic ();
void theme_kate ();
void theme_oblivion ();
void theme_tango ();
void theme_solarizedd  ();
void theme_solarizedl  ();
void doc_set_new_text (t_note_page *doc, GtkTextIter *itstart, GtkTextIter *itend, gchar *text);
void doc_move_to_pos_bw_quote (t_note_page* doc);
gint find_index_by_page (t_note_page *page);
gboolean doc_search_f (t_note_page *doc, const gchar *text);
gboolean doc_search_f_next (t_note_page *doc);
void doc_indent_selection (t_note_page *doc, gboolean unindent);
void tv_logmemo_set_pos (guint pos );
gboolean popup_context_menu_vte(GtkWidget *tv, GdkEventButton *event);
gboolean doc_textview_expose_event_lcb(GtkWidget * widget, GdkEventExpose * event);
gboolean doc_save_buffer_to_file_l (GtkTextBuffer *text_buffer, gchar *filename);
gboolean doc_save_buffer_to_file_iconv (GtkTextBuffer *text_buffer, gchar *filename, gchar *enc);
void doc_update_cb (gpointer data);
gboolean find_space (gunichar ch);
gboolean predicate_find_tag_end (gunichar ch);
void auto_hl_griffon_sql ();

