//GtkEntry *ent_hotkey; 
GtkWidget *wnd_options;

GtkEntry *ent_def_save_dir; 
GtkEntry *ent_def_open_dir; 


GtkEntry *ent_fmt_color_function; 


GtkEntry *ent_cm_mozilla; 
GtkEntry *ent_cm_konqueror;
GtkEntry *ent_cm_firefox;
GtkEntry *ent_cm_opera;
GtkEntry *ent_cm_links; 
GtkEntry *ent_cm_elinks;
GtkEntry *ent_cm_dillo;
GtkEntry *ent_cm_galeon;
GtkEntry *ent_cm_epiphany; 
GtkEntry *ent_cm_other;
GtkEntry *ent_cm_lynx;
GtkEntry *ent_cm_manual_browser;

GtkEntry *ent_cm_mplayer;

GtkEntry *ent_tag_color_lm_error_fg;
GtkEntry *ent_tag_color_lm_error_bg;


GtkEntry *ent_tag_comment;
GtkEntry *ent_tag_identifier;
GtkEntry *ent_tag_digit; 
GtkEntry *ent_tag_string;
GtkEntry *ent_tag_html_tag;
GtkEntry *ent_tag_preprocessor;
GtkEntry *ent_tag_type;
GtkEntry *ent_tag_spellcheck;

GtkEntry *ent_text_foreground;
GtkEntry *ent_text_background;
GtkEntry *ent_text_sel_foreground;
GtkEntry *ent_text_sel_background;
  
GtkCheckButton *cb_use_snippets;
GtkCheckButton *cb_use_ext_image_viewer;
GtkCheckButton *cb_do_hl_on_fileopen;
GtkCheckButton *cb_do_backup;
//GtkCheckButton *cb_do_enc_autodetect;
GtkCheckButton *cb_ins_spaces_on_tab_press;
GtkCheckButton *cb_do_autorep;
GtkCheckButton *cb_do_show_main_toolbar;


GtkCheckButton *cb_prompt_on_not_saved;
GtkCheckButton *cb_prompt_on_file_exists;
GtkCheckButton *cb_warn_about_aliens;


GtkCheckButton *cb_use_def_doc_browser;

GtkCheckButton *cb_do_det_scripts_by_content;


GtkCheckButton *cb_xhtml_mode;
GtkCheckButton *cb_show_hidden_files;
GtkCheckButton *cb_show_line_nums;
GtkCheckButton *cb_word_wrap;
GtkCheckButton *cb_use_autocomp;
GtkCheckButton *cb_use_def_save_dir;
GtkCheckButton *cb_use_def_open_dir;
GtkCheckButton *cb_use_auto_indent;
GtkCheckButton *cb_use_infotext;
GtkCheckButton *cb_use_spellcheck;
GtkCheckButton *cb_use_textcompbloc;

GtkCheckButton *cb_scan_for_links_on_doc_open;

GtkCheckButton *cb_main_wnd_show_full_path;
GtkCheckButton *cb_start_with_blank_file;
GtkCheckButton *cb_start_with_recent_file;


GtkWidget *sb_logmemo_lines_max;
GtkWidget *sb_famous_history_max;
GtkWidget *sb_max_recent_items;


GtkWidget *sb_autocomp_wordlen_min;
GtkWidget *sb_autocomp_list_items_max;


GtkWidget *sb_thumb_width;
GtkWidget *sb_thumb_height;

 
GtkWidget *sb_saveopen_w;
GtkWidget *sb_saveopen_h;

GtkWidget *sb_tab_size;

GtkEntry *ent_date_time;

GtkEntry *ent_tag_comment_font;
GtkEntry *ent_tag_identifier_font;
GtkEntry *ent_tag_digit_font;
GtkEntry *ent_tag_string_font;
GtkEntry *ent_tag_html_tag_font;
GtkEntry *ent_tag_preprocessor_font;
GtkEntry *ent_tag_type_font;
//GtkEntry *ent_tag_spellcheck_font;
GtkEntry *ent_editor_font;

GtkEntry *ent_ext_pic_editor;
GtkEntry *ent_ext_pic_viewer;

/*
GtkCheckButton *cb_enc_det_shift_jis; 
GtkCheckButton *cb_enc_det_euc_jp;
GtkCheckButton *cb_enc_det_gb18030;
GtkCheckButton *cb_enc_det_big5;
GtkCheckButton *cb_enc_det_iso_2022_jp;
*/

GtkCheckButton *cb_enc_det_cp1251; 
GtkCheckButton *cb_enc_det_koi8r;
GtkCheckButton *cb_enc_det_koi8u;
GtkCheckButton *cb_enc_det_866;
GtkCheckButton *cb_enc_det_finnish;
GtkCheckButton *cb_enc_det_german;
GtkCheckButton *cb_enc_det_serbian;
GtkCheckButton *cb_enc_det_latvian;
GtkCheckButton *cb_enc_det_polish;
GtkCheckButton *cb_enc_det_portuguese;
GtkCheckButton *cb_enc_det_slovak;
GtkCheckButton *cb_enc_det_slovenian;
GtkCheckButton *cb_enc_det_spanish;  
GtkCheckButton *cb_enc_det_turkish;  

GtkEntry *ent_rtf_enc;
GtkEntry *ent_default_charset;
GtkEntry *ent_def_filesave_charset;

GtkWidget* wnd_options_create (void);
GtkWidget* create_wnd_about (void);
