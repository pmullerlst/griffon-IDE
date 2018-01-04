gboolean tea_init ();

void scan_links (void);
void on_mni_funx_ins_time ();
void on_mni_Markup_comment ();
void on_mni_Markup_br ();
void on_mni_view_wrap ();
void on_notebook1_switch_page (GtkNotebook *notebook, guint page_num);
void on_mni_file_save_version ();
void on_mni_reverse ();
void on_mni_case_locase_activate ();
void on_mni_case_upcase_activate (); 
void on_mni_new_file_activate ();
void on_mni_file_open_activate ();
void on_mni_file_save_activate ();
void on_mni_file_save_as_activate ();
void on_mni_out_activate ();
void on_mni_nav_block_start ();
void on_mni_nav_block_end ();
void on_mni_Markup_bold_activate ();
void on_mni_Markup_italic_activate ();
void on_mni_Markup_underline_activate ();
void on_mni_Markup_para_activate ();
void on_mni_Markup_header_activate (GtkMenuItem *menuitem);
void on_mni_file_close_current ();
void on_mni_Functions_number_arabian2roman ();
void on_mni_view_show_line_numbers ();
void on_mni_html_default_template ();
void on_mni_file_save_session ();
void on_mni_open_session_activate (GtkMenuItem *menuitem, gpointer user_data);
void on_mni_session_file_open_activate ();
void on_mni_antispam ();
void on_mni_Functions_number_hex2dec ();
void on_mni_config_open ();
void on_mni_open_in_browser (GtkMenuItem *menuitem);
void on_mni_Markup_link_activate ();
void on_mni_utils_stats ();
void on_mni_utils_add_encodings_activate ();
void on_mni_Functions_number_bin2dec ();
void on_mni_file_open_at_cursor ();
void on_mni_Markup_nbsp ();
void on_mni_undo ();
void on_mni_redo ();
void on_mni_markup_select_color_activate ();
void on_mni_image_activate ();
void on_mni_html_strip_tags ();
void on_mni_html_enclose_link ();
gboolean on_ent_search_key_press_event ( GdkEventKey *event);
void on_mni_quest_find ();
void on_mni_quest_find_next ();
void on_mni_func_strings_removeblanks ();
void on_mni_func_strings_sort ();
void on_mni_func_strings_enclose ();
void on_mni_func_strings_rev ();
void on_mni_search_repall ();
void on_mni_recent_activate (GtkMenuItem *menuitem);
void on_mni_about_activate (GtkMenuItem *menuitem, gpointer user_data);
void on_mni_file_new_from_template (GtkMenuItem *menuitem, gpointer user_data);
void on_mni_file_save_as_template_activate ();
void show_save_as_dlg (int mode);
void on_mni_file_edit_at_cursor ();
void on_mni_get_links ();
void on_mni_new_link_select (GtkMenuItem *menuitem);
void on_mni_co_select (GtkMenuItem *menuitem);
void on_mni_markup_align (GtkMenuItem *menuitem);
void on_mni_html_make_table ();
void on_mni_file_revert_to_saved ();
void on_mni_goto_line ();
void on_mni_convert_ascii_2_html ();
void on_mni_func_filter_exclude_from_list ();
void on_mni_func_filter_antiexclude_from_list ();
void on_mni_html_calc_weight ();
void on_mni_get_src ();
void on_mni_file_backup ();
void on_mni_file_add_bookmark ();
void on_mni_file_open_bookmark ();
void on_mni_real_recent_activate (GtkMenuItem *menuitem);
void on_mni_test ();
void on_mni_refresh_hl ();
void on_mni_make_numbering ();
void on_mni_open_hotkeys ();
void tea_win_on_drag_data_lcb(GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *data, guint info, guint time); 
gboolean on_tea_window_delete_event ();
void on_mni_snippet_click (GtkMenuItem *menuitem);
void on_mni_snippet_file_open_activate ();
void on_mni_file_save_as_snippet_activate ();
void on_mni_dump_menu ();
GtkWidget* create_wnd_imgviewer (gchar const *f);
void add_link_item (const gchar *fname, const gchar *linkname);
void on_mni_nav_goto_prev_tab ();
void on_mni_nav_focus_to_text ();
void on_mni_nav_focus_to_famous ();
void on_mni_tools_unitaz_sort ();
void on_mni_tools_unitaz_plain ();
void on_mni_str_kill_dups ();
void on_mni_extract_words ();
void on_mni_tools_unitaz_sort_by_count ();
void on_mni_edit_copy_all ();
void on_mni_edit_copy_current_url ();
void on_mni_edit_replace_with_clipboard ();
void on_mni_count_string_list ();
void on_mni_paste_to_new ();
void on_mni_copy_to_new ();
void on_mni_cut_to_new ();
void on_mni_shuffle_strings ();
void on_mni_tools_unitaz_sort_caseinsens ();
void on_mni_tools_unitaz_sort_by_count_caseinsens ();
void on_mni_cure ();
void on_mni_tabs_to_spaces ();
void on_mni_spaces_to_tabs ();
void on_mni_edit_delete_current_line ();
void on_mni_paste ();
void on_mni_edit_copy ();
void on_mni_edit_cut ();
void on_mni_edit_delete ();
void on_mni_edit_select_all ();
void on_mni_sort_case_insensetive ();
void on_mni_new_filenav (GtkMenuItem *menuitem, gpointer user_data);
void on_but_refresh (void);
void on_mni_new_file_open_select_enc (GtkMenuItem *menuitem);
void on_mni_kill_formatting ();
void on_mni_wrap_raw ();
void on_mni_wrap_on_spaces ();
void on_doit_button ();
void on_mni_kill_formatting_on_each_line ();
void on_mni_file_crapbook ();
void on_mni_spellcheck (GtkMenuItem *menuitem);
void on_mni_filter_kill_lesser ();
void on_mni_filter_kill_more_than ();
void on_mni_func_strings_sort_by_q ();
void on_mni_tabs_menuitem_click (gpointer user_data);
void on_mni_show_images_in_text ();
GtkWidget* lookup_widget2 (GtkContainer *widget);
void on_mni_nav_goto_recent_tab ();
void on_mni_nav_goto_next_tab ();
void on_mni_view_hide_highlighting ();
void on_mni_nav_goto_first_tab ();
void on_mni_nav_goto_last_tab ();
void on_mni_nav_goto_selection ();
void on_mni_quest_find_ncase ();
void on_mni_quest_find_next_ncase ();
void on_mni_user_menu_open ();
void on_mni_user_menu_item (GtkMenuItem *menuitem);
void on_mni_find_and_replace_wnd ();
void on_mni_rtfm ();
void on_mni_crackerize ();
void on_mni_markup_stuff (GtkMenuItem *menuitem); 
void on_mni_show_options ();
void on_mni_help_item (GtkMenuItem *menuitem);
void on_mni_eol_to_crlf ();
void on_mni_eol_to_lf ();
void on_mni_templ_file_open_activate ();
void on_mni_upcase_each_first_letter_ofw ();
void on_mni_nav_mplayer_jump ();
void on_mni_open_open_movie ();
void on_mni_open_base64_activate (GtkMenuItem *menuitem, gpointer user_data);
void on_mni_insert_doctype (GtkMenuItem *menuitem);
void on_mni_entity (GtkMenuItem *menuitem);
void on_mni_sessions_click (GtkMenuItem *menuitem);
void on_mni_templates_click (GtkMenuItem *menuitem);
void on_mni_morse_encode ();
void on_mni_morse_decode ();
void on_mni_set_hl_mode (GtkMenuItem *menuitem);
gboolean on_editor_keypress ( GtkWidget *widget, GdkEventKey *event, gpointer data);
void on_mni_unindent ();
void on_mni_indent ();
void on_mni_autorep_file_open ();
void on_mni_strings_remove_trailing_whitespaces ();
void on_mni_strings_remove_leading_whitespaces ();
void on_mni_edit_append_to_crapbook ();
void on_mni_load_last_file ();
void on_source_header_switch ();
void on_mni_show_project_props ();
void on_mni_project_new ();
void on_mni_project_save_as ();
void on_mni_project_open ();
void on_mni_show_project_save ();
void on_mni_project_make ();
void on_mni_show_project_run ();
void file_new (void);
void file_save (void);
void file_save_as (void);
void file_open (void);
void edit_copy (void);
void edit_cut (void);
void edit_paste (void);
void show_options (void);
void print_bash (void);
void et_bash(void);
void gen_xmessage(void);
void ou_bash(void);
void reNULL_bash(void);
void reECRASER_bash(void);
void reRAJOUTER_bash(void);
void boucle_bash(void);
void fichier_ligne_bash(void);
void test1(void);
void test12(void);
void gen_rpm(void);
void test2(void);
void test22(void);
void test3(void);
void test4(void);
void test5(void);
void test6(void);
void test7(void);
void test8(void);
void test24(void);
void test25(void);
void test23(void);
void test9(void);
void gen_smbmount(void);
void test10(void);
void test11(void);
void rsync_gen(void);
void echo_bash(void);
void gen_read(void);
void com(void);
void redi_erreur(void);
void if_bash(void);
void fond_bash(void);
void redi_2(void);
void else_bash(void);
void skeleton(void);
GtkWidget* window_grep (void);
void grep(void);
GtkWidget* sed (void);
void sed_apl(void);
GtkWidget* case_window (void);
void case_build (void);
GtkWidget* awk (void);
void  awk_apl(void);
void file_save_bug (void);
GtkWidget* selection_path (void);
GtkWidget* win_scp (void);
void gen_scp (void);
void start_perl_script(void);
void perl_print(void);
void perl_if(void);
void perl_if_else(void);
void perl_while(void);
void perl_for(void);
void perl_foreach(void);
void perl_do_while(void);
void perl_fonction(void);
void perl_fonction_param(void);
void perl_exit(void);
void perl_die(void);
void exe(void);
void perl_chop(void);
void perl_chomp(void);
void perl_lenght(void);
void perl_uc(void);
void perl_lc(void);
void perl_ucfirst (void);
void perl_writh(void);
void perl_read(void);
void perl_regular(void);
void perl_replace(void);
void echo_php(void);
GtkWidget* help_php_window (void);
GtkWidget* help_mysql(void);
GtkWidget* Aide_BASH (void);
GtkWidget* w_sftp_mount (void);
void on_format_button_clicked ();
void umount_sftp (void);
void on_mni_file_todo ();
void on_mni_file_myadmin ();
void  on_changed(GtkWidget *widget, gpointer statusbar) ;
GtkWidget* centre_perl (void);
void on_execut_script (void);
void on_format_line_bg ();
void mount_ftp (void);
GtkWidget* w_ftp_mount (void);
gchar *tampon_sftp;
void gen_doc_html (void);
gboolean on_editor_keyrelease (GtkWidget *widget, GdkEventKey *event, gpointer data);
void delete_autocomp_tips();
void on_mni_autocomp_file_open ();
void keyrelase_search(void);
void start_php_script            (void);
void javascript_urlencode (void);
void javascript_file_contenu (void);
void javascript_split (void);
void javascript_onsubmit (void);
void javascript_onselect (void);
void javascript_onmouseover (void);
void javascript_onload (void);
void javascript_onfocus (void);
void javascript_onclick (void);
void javascript_onchange (void);
void javascript_onblur (void);
void javascript_id_change_add (void);
void javascript_id_change(void);
void javascript_html_change (void);
void javascript_id_load_file (void);
void javascript_value_select (void);
void javascript_focus_id (void);
void javascript_value_par_id(void);
void javascript_fonction (void);
void javascript_html (void);
void css_valign_bottom (void);
void css_valign_top (void);
void css_inline (void);
void css_block (void);
void css_none (void);
void css_fixed (void);
void css_absolute (void);
void css_relative (void);
void css_bas (void);
void css_haut (void);
void css_droit (void);
void css_gauche (void);
void css_hauteur (void);
void css_largeur (void);
void css_font_deco_non (void);
void css_font_deco_und (void);
void css_font_color (void);
void css_font_style (void);
void css_font_size (void);
void css_bg_couleur (void);
void css_bg_img_norep (void);
void css_bg_img (void);
void css_div (void);
void css_link_hover (void);
void css_link (void);
void css_body (void);
void html_favicon (void);
void html_style(void);
void html_div (void);
void html_input_select (void);
void html_input_textarea (void);
void html_input_text (void);
void html_form (void);
void html_cols (void);
void html_rows (void);
void html_hauteur (void);
void html_largeur (void);
void html_reload (void);
void html_css (void);
void mysql_delete (void);
void mysql_update (void);
void mysql_select (void);
void mysql_insert (void);
void perl_ftp_delete (void);
void perl_ftp_cwd (void);
void perl_stat (void);
void perl_dbi_rows (void);
void perl_dbi_lastid (void);
void perl_stdin (void);
void perl_tube (void);
void perl_substr (void);
void perl_socket_client (void);
void perl_socket_srv (void);
void perl_ftp_put (void);
void perl_ftp_get (void);
void perl_ftp_list (void);
void perl_ftp_connexion (void);
void perl_url_get (void);
void perl_uri_encode (void);
void perl_dbi_query_select (void);
void perl_mime_mail_pj (void);
void perl_date (void);
void perl_mime_mail_simple (void);
void perl_length (void);
void perl_dbi_query (void);
void perl_dbi_select_while (void);
void perl_dbi_connexion (void);
void perl_split (void);
void perl_writh2 (void);
void perl_while_infinie (void);
void perl_unless (void);
void perl_arguments (void);
void perl_empiler_tableau_start (void);
void perl_empiler_tableau (void);
void perl_saisie_clavier (void);
void perl_tableau (void);
void perl_rand (void);
void php_dl_csv (void);
void php_addslashes (void);
void php_dir (void);
void php_count (void);
void php_fputs (void);
void php_unlink (void);
void php_explode (void);
void php_num_rows (void);
void php_strip_tags (void);
void php_var_post (void);
void php_mb_substr_count (void);
void php_rand (void);
void php_urlencode (void);
void php_strlen (void);
void php_date_mysql (void);
void php_h_png(void);
void php_h_utf8 (void);
void php_ftp_delete (void);
void php_ftp_put (void);
void php_ftp_list_rep (void);
void php_ftp_connect (void);
void php_insert_id (void);
void php_mail (void);
void php_stripslashes (void);
void php_function (void);
void php_find(void);
void php_file(void);
void php_replace (void);
void php_split(void);
void php_foreach (void);
void php_mysql_query_select (void);
void php_mysql_query (void);
void php_mysql_while (void);
void php_mysql_connexion (void);
void htaccess_urlrw_exp (void);
void htaccess_urlrw_on (void);
void htaccess_redirection_erreur (void);
void htaccess_accept_ip (void);
void htaccess_deny_all (void);
void htaccess_charset_utf8 (void);
void htaccess_urlrw (void);
void term_diff (gpointer user_data);
void term_decomp_targz (gpointer user_data);
void term_nice (gpointer user_data);
void term_ls_color (gpointer user_data);
void term_wget (gpointer user_data);
void term_wc (gpointer user_data);
void term_screen (gpointer user_data);
void term_find (gpointer user_data);
void term_long_rm (gpointer user_data);
void term_aspiration_web (gpointer user_data);
void term_df (gpointer user_data);
void term_comp_targz (gpointer user_data);
void add_todo_com(void);
void add_todo_bug(void);
void add_todo_fixme(void);
void close_color_select();
void term_git_status(gpointer user_data);
void term_git_commit(gpointer user_data);
void term_git_push(gpointer user_data);
void term_iptables_drop(gpointer user_data);
void term_iptables_clean(gpointer user_data);
void iptables_drop_all (void);
void iptables_accept_lo (void);
void iptables_accept_port (void);
void iptables_accept_port_ip (void);
void iptables_log(void);
void iptables_clean (void);
void on_mni_file_todolist ();
void insert_dialog_path();
void insert_dialog_path_dir();
void on_execut_diff (void);
void on_mni_custom_term_file_open ();
void open_todo_combo (void);
void on_mni_html5_header ();
void on_mni_html5_footer ();
void on_mni_html5_menu ();
void on_mni_html5_section ();
void on_mni_html5_article ();
void on_mni_html5_aside ();
void on_mni_html5_nav ();
void html5_input_range (void);
void html5_input_number (void);
void html5_input_email (void);
void html5_input_tel (void);
void html5_input_required (void);
void html5_input_search (void);
void html5_input_url (void);
void css_box_shadow (void);
void css_border_radius (void);
void css_media_screen (void);
void html5_audio ();
void html5_video ();
void css_template_table (void);
void css_template_submit (void);
void css_template_menu (void);
void css_template_box (void);
void css_template_textarea (void);
void css_template_label (void);
void css_template_footer (void);
void jquery_head (void);
void tablefilter_head (void);
void tablefilter_source (void);
void jquery_autocomp (void);
void jquery_datepicker (void);
void php_htmlentities(void);
void php_nl2br(void);
void csv_to_mysql(void);
void csv_to_mysql_insert(void);
void edit_todo (void);
void on_mni_mywebhistory_file_open ();
void on_mni_miniwebhistory_file_open ();
gboolean scan_include_view (gchar *text,int id) ;
void scan_var_include_all ();

