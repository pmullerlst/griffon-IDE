/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


typedef struct
{
  int width;
  int height;
} t_image_info;


gchar* get_noname_name (void);

int get_value (int total, int perc);
int get_percent (int total, int value);

GtkWidget* new_menu_item_with_udata (const gchar *label, GtkWidget *parent, gpointer f, gpointer data);

GtkWidget* new_menu_sep (GtkWidget *parent);
GtkWidget* new_menu_item (const gchar *label, GtkWidget *parent, gpointer f);
GtkWidget* new_menu_tof (GtkWidget *parent);
GtkWidget* new_menu_submenu (GtkWidget *parent);

gboolean change_dir (gchar *filename);
gchar* create_full_path (gchar const *filename, gchar *basedir);
void handle_file (gchar const *filename, gint mode);
t_image_info* get_image_info (gchar *filename);
gboolean is_image (const gchar *filename);
gboolean copy_file (gchar *src, gchar *dest);
void create_empty_file (gchar *filename, gchar *first_line);
gchar* get_time (const gchar *format);
gint get_file_size (const gchar *filename);
gchar* get_8_filename (const gchar *filename);
gchar* get_l_filename (const gchar *filename);
gboolean is_rtf (const gchar *filename);
void window_destroy(GtkWidget *windowname);
gchar* get_lang_name (void);
gchar* find_doc_index_name (void);
void run_doc_in_browser (void);
gchar* get_clipboard_text (void);
void kwas_handle_file (gchar *filename, int mode);
void insert_image (gchar *a_filename);
gchar* get_tea_doc_compose_name (gchar const *f);
gchar* get_tea_doc_dir (void);
gboolean is_css (const gchar *filename);
void build_menu_from_glist (GList *list, gpointer menu, gpointer callback);
void build_menu_wudata_from_glist (GList *list, gpointer menu, gpointer callback, gpointer udata);
void build_menu_from_ht (GHashTable *hash_table, gpointer menu, gpointer callback);
GList* read_dir_files (gchar *path);
gchar* get_hl_name (gchar *file_name);
gboolean is_po (const gchar *f);
gboolean is_php (const gchar *f);
void insert_link (gchar *a_filename);
gboolean parse_error_line (const gchar * line, gchar ** filename, gint *lineno);
void handle_file_ide (gchar *filename, gint line);
gboolean check_ext (gchar const *filename, gchar *ext);
void handle_file_enc (gchar *filename, gchar *enc);
void make_stats (t_note_page *doc);
gboolean is_markup (const gchar *f);
gboolean is_c (const gchar *f);
gboolean is_pascal (const gchar *f);
void create_empty_file (gchar *filename, gchar *first_line);
void goto_local_label (const gchar *l);


