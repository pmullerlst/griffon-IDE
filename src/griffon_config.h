
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


void tea_start (void);
void bmx_reload (void);

typedef struct
{
	GList *confdata;
	gchar *date_time;
	gboolean prompt_on_not_saved; 
	gboolean prompt_on_file_exists; 
	gboolean warn_about_aliens; 
	gchar *ext_pic_editor;
	gchar *ext_pic_viewer;
	gchar *tea_main_dir;
	gchar *templates_dir;
	gchar *snippets_dir;
	gchar *crapbook_file;
	gchar *tmp_dir;
	gchar *changelog;
	gchar *tmp_file;
	gchar *user_menu_file;
	gchar *tea_todo;
	gchar *tea_sftp;	
	gchar *tea_miniweb_history;	
	gchar *tea_myadmin_history;	
	gchar *tea_cmd_history;	
	gchar *tea_theme;	
	gchar *tea_ftp;
	gchar *file_tmp;
	gchar *tea_myadmin;
	gchar *current_movie;
	gchar *cm_mplayer;
	gchar *theme;
	gchar *projects;
	gchar *iconv_file;
	gchar *bmx_file;
	gchar *sessions_dir;
	gchar *tea_rc;
	gchar *tea_ui;
	gchar *tea_gl_famous;
	gchar *tea_kwas_history;
	gchar *tea_kwas_bookmarks;
	gchar *tea_autocomp;
	gchar *tea_autoreplace;
	gchar *tea_recent;
	gchar *tea_hotkeys;
	gchar *def_save_dir;
	gchar *def_open_dir;
	gchar *fmt_color_function;
	gchar *cmd_def_doc_browser;
	gchar *cmd_Opera;
	gchar *cmd_Firefox;
	gchar *cmd_Mozilla;
	gchar *cmd_Konqueror;
	gchar *cmd_Galeon;
	gchar *cmd_Dillo;
	gchar *cmd_Links;
	gchar *cmd_ELinks;
	gchar *cmd_Lynx;
	gchar *cmd_Epiphany;
	gchar *cmd_Other;
	gint logmemo_lines_max;
	gint famous_history_max;
	gint autocomp_wordlen_min;    
	gint autocomp_list_items_max;    
	gint saveopen_w;
	gint saveopen_h;
	gchar *text_foreground;
	gchar *text_background;
	gchar *text_sel_foreground;
	gchar *text_sel_background;
	gchar *tag_color_lm_error_fg;
	gchar *tag_color_lm_error_bg;
	gchar *tag_comment;
	gchar *tag_identifier;
	gchar *tag_digit;
	gchar *tag_string;
	gchar *tag_html_tag;
	gchar *tag_preprocessor;
	gchar *tag_type;
	gchar *tag_spellcheck;
	gchar *tag_comment_font;
	gchar *tag_identifier_font;
	gchar *tag_digit_font;
	gchar *tag_string_font;
	gchar *tag_html_tag_font;
	gchar *tag_preprocessor_font;
	gchar *tag_control_font;
	gchar *tag_type_font;
	gint tab_size;
	gint thumb_width;
	gint thumb_height;
	gchar *default_charset;
	gchar *def_filesave_charset;
	gint last_prefs_tab;
	gboolean ins_spaces_on_tab_press;
	gboolean use_auto_indent;
	gboolean use_infotext;
	gboolean use_spellcheck;
	gboolean use_textcompbloc;
	gboolean show_hidden_files;
	gint screen_h;
  gint screen_w;
	gboolean start_with_blank_file;
	gboolean start_with_recent_file;
	gboolean use_def_save_dir;
	gboolean use_def_open_dir;
	gboolean use_autocomp;
	gboolean use_snippets;
	gboolean use_ext_image_viewer;
	gboolean do_hl_on_fileopen;
	gboolean do_backup;
	gboolean do_autorep;
	gboolean do_show_main_toolbar;
	gboolean do_det_scripts_by_content;
	gint top;
	gint left;
	gint height;
	gint width;
	gint max_recent_items;
	gchar *editor_font;
	gboolean main_wnd_show_full_path;
	gboolean show_line_nums;
	gboolean word_wrap;
	gboolean scan_for_links_on_doc_open;
	gboolean use_def_doc_browser;
	gboolean xhtml_mode; 
	gboolean enc_det_cp1251; 
	gboolean enc_det_koi8r;
	gboolean enc_det_koi8u;
	gboolean enc_det_866;
	gboolean enc_det_finnish;
	gboolean enc_det_german;
	gboolean enc_det_serbian;
	gboolean enc_det_latvian;
	gboolean enc_det_polish;
	gboolean enc_det_portuguese;
	gboolean enc_det_slovak;
	gboolean enc_det_slovenian;
	gboolean enc_det_spanish;  
	gboolean enc_det_turkish;  
	gchar *rtf_enc;
	GList *gl_save_charsets;
	GList *iconv_encs;
	gint msg_counter;
} tea_confile;


typedef struct
{
	gchar *selected_enc;
} t_cur_settings;

typedef struct
{
	gpointer a;
	gpointer b;
	gboolean c;
} t_ppair;

tea_confile confile;
t_cur_settings cur_settings;

GList *bookmarks;
GList *recent_list;
GList *gl_famous;
GList *gl_kwas_history;
GList *gl_tea_kwas_bookmarks;

GList *gl_autocomp;  

GHashTable *ht_user_menu;
GHashTable *ht_entities;
GHashTable *ht_config;
GHashTable *ht_autoreplace;


GCompletion *at_editor; 

gint g_argc;
char **g_argv;

void confile_reload (void);
void confile_free (void);
void tabs_reload (void);

GList* parse_conf_file_to_list (gchar *filename);
gchar* conf_get_char_value (GList *confdata, gchar *key, gchar *def);
gint conf_get_int_value (GList *confdata, gchar *key, gint def);

void ui_init (void);
void ui_done (void);

void get_iconv_sup (void);

void autocomp_done (void);
void autocomp_init (void);

void execute_recent_item (gchar const *item);

void update_enc_menu (void);
void reload_snippets (void);
GtkWidget* lookup_widget (GtkContainer *widget, gchar *widget_name);

void  hash_remove_cb (gpointer data);
gchar* get_tmp_file_string (void);
void reload_usermenu (void);
void reload_sessions (void);
void reload_autoreplace (void);
void reload_templates (void);
void update_recent_list_menu (gboolean load_from_file);
void tea_done (void);
void fill_entities (void);
void reload_hotkeys (void);

