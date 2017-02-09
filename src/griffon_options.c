
/***************************************************************************
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <glib.h>
#include <gtk/gtk.h>
#include <unistd.h>
#include <sys/stat.h>
#include <gdk/gdkkeysyms.h>

#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "griffon_text_document.h"
#include "griffon_defs.h"
#include "griffon_options.h"
#include "griffon_gtk_utils.h"
#include "griffon_config.h"
#include "rox_strings.h"

static void cb_on_bt_apply () 
{
   GList *l = NULL;

   l = add_to_glist_combined_str (l, "fmt_color_function", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_fmt_color_function)));

   l = add_to_glist_combined_str (l, "def_save_dir", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_def_save_dir)));
   l = add_to_glist_combined_str (l, "def_open_dir", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_def_open_dir)));

   l = add_to_glist_combined_str (l, "mplayer", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_cm_mplayer)));
   l = add_to_glist_combined_str (l, "mozilla", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_cm_mozilla)));
   l = add_to_glist_combined_str (l, "konqueror", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_cm_konqueror)));
   l = add_to_glist_combined_str (l, "firefox", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_cm_firefox)));
   l = add_to_glist_combined_str (l, "opera", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_cm_opera)));
   l = add_to_glist_combined_str (l, "links", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_cm_links)));
   l = add_to_glist_combined_str (l, "elinks", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_cm_elinks)));
   l = add_to_glist_combined_str (l, "dillo", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_cm_dillo)));
   l = add_to_glist_combined_str (l, "galeon", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_cm_galeon)));
   l = add_to_glist_combined_str (l, "epiphany", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_cm_epiphany)));

   l = add_to_glist_combined_str (l, "other", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_cm_other)));
   l = add_to_glist_combined_str (l, "lynx", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_cm_lynx)));
   l = add_to_glist_combined_str (l, "def_doc_browser", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_cm_manual_browser)));

   l = add_to_glist_combined_str (l, "date_time", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_date_time)));

   l = add_to_glist_combined_str (l, "tag_color_lm_error_fg", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_tag_color_lm_error_fg)));
   l = add_to_glist_combined_str (l, "tag_color_lm_error_bg", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_tag_color_lm_error_bg)));

   l = add_to_glist_combined_str (l, "tag_spellcheck", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_tag_spellcheck)));

   l = add_to_glist_combined_str (l, "text_foreground", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_text_foreground)));
   l = add_to_glist_combined_str (l, "text_background", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_text_background)));
 
   l = add_to_glist_combined_str (l, "text_sel_foreground", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_text_sel_foreground)));
   l = add_to_glist_combined_str (l, "text_sel_background", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_text_sel_background)));

   l = add_to_glist_combined_str (l, "def_filesave_charset", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_def_filesave_charset)));
   l = add_to_glist_combined_str (l, "default_charset", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_default_charset)));
   l = add_to_glist_combined_str (l, "rtf_enc", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_rtf_enc)));
   l = add_to_glist_combined_str (l, "tag_comment_font", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_tag_comment_font)));

   l = add_to_glist_combined_str (l, "tag_identifier_font", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_tag_identifier_font)));
   l = add_to_glist_combined_str (l, "tag_digit_font", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_tag_digit_font)));

   l = add_to_glist_combined_str (l, "tag_string_font",(gchar *) gtk_entry_get_text (GTK_ENTRY (ent_tag_string_font)));
   l = add_to_glist_combined_str (l, "tag_html_tag_font", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_tag_html_tag_font)));

   l = add_to_glist_combined_str (l, "tag_preprocessor_font",(gchar *) gtk_entry_get_text (GTK_ENTRY (ent_tag_preprocessor_font)));
   l = add_to_glist_combined_str (l, "tag_type_font",(gchar *) gtk_entry_get_text (GTK_ENTRY (ent_tag_type_font)));

   l = add_to_glist_combined_str (l, "editor_font", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_editor_font)));
   l = add_to_glist_combined_str (l, "ext_pic_editor",(gchar *) gtk_entry_get_text (GTK_ENTRY (ent_ext_pic_editor)));
   l = add_to_glist_combined_str (l, "ext_pic_viewer", (gchar *)gtk_entry_get_text (GTK_ENTRY (ent_ext_pic_viewer)));

   l = add_to_glist_combined_bool (l, "prompt_on_not_saved", gtk_toggle_button_get_active ((GtkToggleButton *)cb_prompt_on_not_saved));
   l = add_to_glist_combined_bool (l, "prompt_on_file_exists", gtk_toggle_button_get_active ((GtkToggleButton *)cb_prompt_on_file_exists));
   l = add_to_glist_combined_bool (l, "warn_about_aliens", gtk_toggle_button_get_active ((GtkToggleButton *)cb_warn_about_aliens));

   l = add_to_glist_combined_bool (l, "do_det_scripts_by_content", gtk_toggle_button_get_active ((GtkToggleButton *)cb_do_det_scripts_by_content));
   l = add_to_glist_combined_bool (l, "do_show_main_toolbar", gtk_toggle_button_get_active ((GtkToggleButton *)cb_do_show_main_toolbar));



   l = add_to_glist_combined_bool (l, "xhtml_mode", gtk_toggle_button_get_active ((GtkToggleButton *)cb_xhtml_mode));
   l = add_to_glist_combined_bool (l, "use_snippets", gtk_toggle_button_get_active ((GtkToggleButton *)cb_use_snippets));
   l = add_to_glist_combined_bool (l, "use_ext_image_viewer", gtk_toggle_button_get_active ((GtkToggleButton *)cb_use_ext_image_viewer));

   l = add_to_glist_combined_bool (l, "do_hl_on_fileopen", gtk_toggle_button_get_active ((GtkToggleButton *)cb_do_hl_on_fileopen));
   l = add_to_glist_combined_bool (l, "do_backup", gtk_toggle_button_get_active ((GtkToggleButton *)cb_do_backup));

   l = add_to_glist_combined_bool (l, "start_with_blank_file", gtk_toggle_button_get_active ((GtkToggleButton *)cb_start_with_blank_file));
   l = add_to_glist_combined_bool (l, "start_with_recent_file", gtk_toggle_button_get_active ((GtkToggleButton *)cb_start_with_recent_file));

   l = add_to_glist_combined_bool (l, "use_def_doc_browser", gtk_toggle_button_get_active ((GtkToggleButton *)cb_use_def_doc_browser));

   l = add_to_glist_combined_bool (l, "show_line_nums", gtk_toggle_button_get_active ((GtkToggleButton *)cb_show_line_nums));
   l = add_to_glist_combined_bool (l, "word_wrap", gtk_toggle_button_get_active ((GtkToggleButton *)cb_word_wrap));
   l = add_to_glist_combined_bool (l, "use_def_save_dir", gtk_toggle_button_get_active ((GtkToggleButton *)cb_use_def_save_dir));
   l = add_to_glist_combined_bool (l, "use_def_open_dir", gtk_toggle_button_get_active ((GtkToggleButton *)cb_use_def_open_dir));
   l = add_to_glist_combined_bool (l, "use_auto_indent", gtk_toggle_button_get_active ((GtkToggleButton *)cb_use_auto_indent));
	l = add_to_glist_combined_bool (l, "use_infotext", gtk_toggle_button_get_active ((GtkToggleButton *)cb_use_infotext));
		l = add_to_glist_combined_bool (l, "use_textcompbloc", gtk_toggle_button_get_active ((GtkToggleButton *)cb_use_textcompbloc));

   l = add_to_glist_combined_bool (l, "ins_spaces_on_tab_press", gtk_toggle_button_get_active ((GtkToggleButton *)cb_ins_spaces_on_tab_press));
   l = add_to_glist_combined_bool (l, "scan_for_links_on_doc_open", gtk_toggle_button_get_active ((GtkToggleButton *)cb_scan_for_links_on_doc_open));
   l = add_to_glist_combined_bool (l, "main_wnd_show_full_path", gtk_toggle_button_get_active ((GtkToggleButton *)cb_main_wnd_show_full_path));

   l = add_to_glist_combined_bool (l, "enc_det_cp1251", gtk_toggle_button_get_active ((GtkToggleButton *)cb_enc_det_cp1251));
   l = add_to_glist_combined_bool (l, "enc_det_koi8r", gtk_toggle_button_get_active ((GtkToggleButton *)cb_enc_det_koi8r));

   l = add_to_glist_combined_bool (l, "enc_det_koi8u", gtk_toggle_button_get_active ((GtkToggleButton *)cb_enc_det_koi8u));
   l = add_to_glist_combined_bool (l, "enc_det_866", gtk_toggle_button_get_active ((GtkToggleButton *)cb_enc_det_866));

   l = add_to_glist_combined_bool (l, "enc_det_finnish", gtk_toggle_button_get_active ((GtkToggleButton *)cb_enc_det_finnish));
   l = add_to_glist_combined_bool (l, "enc_det_german", gtk_toggle_button_get_active ((GtkToggleButton *)cb_enc_det_german));

   l = add_to_glist_combined_bool (l, "enc_det_serbian", gtk_toggle_button_get_active ((GtkToggleButton *)cb_enc_det_serbian));
   l = add_to_glist_combined_bool (l, "enc_det_latvian", gtk_toggle_button_get_active ((GtkToggleButton *)cb_enc_det_latvian));

   l = add_to_glist_combined_bool (l, "enc_det_polish", gtk_toggle_button_get_active ((GtkToggleButton *)cb_enc_det_polish));
   l = add_to_glist_combined_bool (l, "enc_det_portuguese", gtk_toggle_button_get_active ((GtkToggleButton *)cb_enc_det_portuguese));

   l = add_to_glist_combined_bool (l, "enc_det_slovak", gtk_toggle_button_get_active ((GtkToggleButton *)cb_enc_det_slovak));
   l = add_to_glist_combined_bool (l, "enc_det_slovenian", gtk_toggle_button_get_active ((GtkToggleButton *)cb_enc_det_slovenian));

   l = add_to_glist_combined_bool (l, "enc_det_spanish", gtk_toggle_button_get_active ((GtkToggleButton *)cb_enc_det_spanish));
   l = add_to_glist_combined_bool (l, "enc_det_turkish", gtk_toggle_button_get_active ((GtkToggleButton *)cb_enc_det_turkish));

   l = add_to_glist_combined_bool (l, "show_hidden_files", gtk_toggle_button_get_active ((GtkToggleButton *)cb_show_hidden_files));
   l = add_to_glist_combined_bool (l, "do_autorep", gtk_toggle_button_get_active ((GtkToggleButton *)cb_do_autorep));

   l = add_to_glist_combined_int (l, "famous_history_max", gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(sb_famous_history_max)));  
   l = add_to_glist_combined_int (l, "max_recent_items", gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(sb_max_recent_items)));  

   l = add_to_glist_combined_int (l, "thumb_width", gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(sb_thumb_width)));  
   l = add_to_glist_combined_int (l, "thumb_height", gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(sb_thumb_height)));  

   l = add_to_glist_combined_int (l, "tab_size", gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(sb_tab_size)));  
 
   glist_save_to_file (l, confile.tea_rc);

   glist_strings_free (l);
   confile_reload ();
   doc_update_all();
//   update_enc_menu();
}


static GtkWidget* wnd_options_encoding (void)
{
  GtkWidget *wnd = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (wnd);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (wnd), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
 
  GtkWidget *page = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (page);
		gtk_container_add((GtkContainer *)wnd,page);

  GtkWidget *fr1 = gtk_frame_new (_("Defaults"));   
  gtk_widget_show (fr1);
  gtk_box_pack_start (GTK_BOX(page), fr1, FALSE, FALSE, 1);

  ent_default_charset = tea_combo (page,  _("Default charset for file open"));
  ent_def_filesave_charset = tea_combo (page,  _("Default charset for file saved as"));
 
  fr1 = gtk_frame_new (_("Misc"));   
  gtk_widget_show (fr1);
  gtk_box_pack_start (GTK_BOX(page), fr1, FALSE, FALSE, 1);
 
  ent_rtf_enc = tea_text_entry (page, _("Default charset for RTF"), confile.rtf_enc);  
 
  GtkWidget *fr2 = gtk_frame_new (_("Autodetect"));   
  gtk_widget_show (fr2);
 
  gtk_box_pack_start (GTK_BOX(page), fr2, FALSE, FALSE, 1);
  
  cb_enc_det_cp1251 = tea_checkbox (page, _("CP 1251"), &confile.enc_det_cp1251); 
  cb_enc_det_koi8r = tea_checkbox (page, _("KOI8-R"), &confile.enc_det_koi8r);
  cb_enc_det_koi8u = tea_checkbox (page, _("KOI8-U"), &confile.enc_det_koi8u);
  cb_enc_det_866 = tea_checkbox (page, _("DOS 866"), &confile.enc_det_866);

  cb_enc_det_finnish = tea_checkbox (page, _("Finnish"), &confile.enc_det_finnish);
  cb_enc_det_german = tea_checkbox (page, _("German"), &confile.enc_det_german);
  cb_enc_det_serbian = tea_checkbox (page, _("Serbian"), &confile.enc_det_serbian);
  cb_enc_det_latvian = tea_checkbox (page, _("Latvian"), &confile.enc_det_latvian);
  cb_enc_det_polish = tea_checkbox (page, _("Polish"), &confile.enc_det_polish);
  cb_enc_det_portuguese = tea_checkbox (page, _("Portuguese"), &confile.enc_det_portuguese);
  cb_enc_det_slovak = tea_checkbox (page, _("Slovak"), &confile.enc_det_slovak);
  cb_enc_det_slovenian = tea_checkbox (page, _("Slovenian"), &confile.enc_det_slovenian);
  cb_enc_det_spanish = tea_checkbox (page, _("Spanish"), &confile.enc_det_spanish);  
  cb_enc_det_turkish = tea_checkbox (page, _("Turkish"), &confile.enc_det_turkish);  

  return wnd;
}


static GtkWidget* wnd_options_commands (void)
{
  GtkWidget *page = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (page);

  ent_ext_pic_editor = tea_text_entry (page, _("Éditeur d'image externe"), confile.ext_pic_editor);
  ent_ext_pic_viewer = tea_text_entry (page, _("External image viewer"), confile.ext_pic_viewer);
  ent_cm_mplayer =  tea_text_entry (page, _("Run MPlayer with subtitles"), confile.cm_mplayer);
 
  return page;
}


static GtkWidget* wnd_options_limits (void)
{
  GtkWidget *page = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (page);

  sb_max_recent_items = tea_spinbutton (page, _("Max of recent files"), confile.max_recent_items); 

  return page;
}


static GtkWidget* wnd_options_functions (void)
{
  GtkWidget *page = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (page);
  ent_date_time = tea_text_entry (page, _("Date format and time"), confile.date_time);
  ent_fmt_color_function = tea_text_entry (page, _("The format of the function Color"), confile.fmt_color_function);

  return page;
}


static GtkWidget* wnd_options_interface (void)
{
  GtkWidget *page = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (page);
  
  cb_main_wnd_show_full_path = tea_checkbox (page, _("Show full path in window title"), &confile.main_wnd_show_full_path); 

  sb_thumb_width = tea_spinbutton (page, _("Width of the image thumbnail"), confile.thumb_width);
  sb_thumb_height = tea_spinbutton (page, _("Thumbnail image height"), confile.thumb_height);

  return page;
}


static GtkWidget* wnd_options_switches (void)
{
  GtkWidget *page = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (page);

  cb_start_with_recent_file = tea_checkbox (page, _("Griffon start with a recent file"), &confile.start_with_recent_file); 
  cb_start_with_blank_file = tea_checkbox (page, _("Griffon start with an empty file"), &confile.start_with_blank_file); 
  cb_do_det_scripts_by_content = tea_checkbox (page, _("Determine the syntax of the file contents"), &confile.do_det_scripts_by_content); 
  cb_xhtml_mode = tea_checkbox (page, _("XHTML Mode for Markup"), &confile.xhtml_mode); 
  cb_use_snippets = tea_checkbox (page, _("Use snippets"), &confile.use_snippets); 
  cb_use_ext_image_viewer = tea_checkbox (page, _("Use external image viewer"), &confile.use_ext_image_viewer);
  cb_do_hl_on_fileopen = tea_checkbox (page, _("Syntax coloring to open the file"), &confile.do_hl_on_fileopen);
  cb_do_backup = tea_checkbox (page, _("File backup"), &confile.do_backup);
  cb_show_line_nums = tea_checkbox (page, _("Afficher les numéros de ligne"), &confile.show_line_nums);
  cb_word_wrap = tea_checkbox (page, _("Word wrap"), &confile.word_wrap);
  cb_scan_for_links_on_doc_open = tea_checkbox (page, _("Analyze the links to open file"), &confile.scan_for_links_on_doc_open);

  return page;
}


static GtkWidget* wnd_options_confirmations (void)
{
  GtkWidget *page = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (page);
  
  cb_prompt_on_not_saved = tea_checkbox (page, _("Warn before closing a modified file"), &confile.prompt_on_not_saved); 
  cb_prompt_on_file_exists = tea_checkbox (page, _("Overwrite existing file"), &confile.prompt_on_file_exists); 
 
  return page;
}


/*static GtkWidget* wnd_options_maintenance (void)
{
  GtkWidget *page = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (page);
  
  gtk_box_set_homogeneous (GTK_BOX(page), FALSE);
    
  return page;
}*/


/*static GtkWidget* wnd_options_colors (void)
{
  GtkWidget *notebook = gtk_notebook_new ();         
  gtk_widget_show (notebook);

  GtkWidget *page = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (page);

  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), page, gtk_label_new (_("Editor")));
 
  ent_text_foreground = tea_color_selector (page, _("Foreground text"), confile.text_foreground);
  ent_text_background = tea_color_selector (page, _("Background text"), confile.text_background);
  ent_text_sel_foreground = tea_color_selector (page, _("selected plane"), confile.text_sel_foreground);
  ent_text_sel_background = tea_color_selector (page, _("Arrière-plan sélectionnée"), confile.text_sel_background);
  ent_tag_spellcheck = tea_color_selector (page, _("Marques de vérification orthographique"), confile.tag_spellcheck);
  
  page = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (page);
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), page, gtk_label_new (_("IDE")));
 
  ent_tag_color_lm_error_fg = tea_color_selector (page, _("Errors avant-plan (logmemo)"), confile.tag_color_lm_error_fg);
  ent_tag_color_lm_error_bg = tea_color_selector (page, _("Errors arrière-plan du texte (logmemo)"), confile.tag_color_lm_error_bg);

  return notebook ;
}*/


/*static GtkWidget* wnd_options_fonts (void)
{
  GtkWidget *page = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (page);

  ent_editor_font = tea_font_selector (page, _("Editor font"), confile.editor_font); 

  ent_tag_comment_font = tea_font_selector (page, _("Comments font"), confile.tag_comment_font); 
  ent_tag_identifier_font = tea_font_selector (page, _("Identifiers font"), confile.tag_identifier_font); 
  ent_tag_digit_font = tea_font_selector (page, _("numbers font"), confile.tag_digit_font); 
  ent_tag_string_font = tea_font_selector (page, _("Chaîne de caractère"), confile.tag_string_font); 
  ent_tag_html_tag_font = tea_font_selector (page, _("HTML tags font"), confile.tag_html_tag_font); 
  ent_tag_preprocessor_font = tea_font_selector (page, _("Preprocessor font"), confile.tag_preprocessor_font); 
  ent_tag_type_font = tea_font_selector (page, _("Types font"), confile.tag_type_font); 
  
  return page;
}*/


static GtkWidget* wnd_options_browsers (void)
{
  GtkWidget *page = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (page);
  ent_cm_opera = tea_text_entry (page, "Opera", confile.cmd_Opera);
  ent_cm_konqueror = tea_text_entry (page, "Konqueror", confile.cmd_Konqueror);
  ent_cm_firefox = tea_text_entry (page, "Firefox", confile.cmd_Firefox);
  ent_cm_mozilla = tea_text_entry (page, "Mozilla",  confile.cmd_Mozilla);
  ent_cm_links = tea_text_entry (page, "Links", confile.cmd_Links); 
  ent_cm_elinks = tea_text_entry (page, "Elinks", confile.cmd_ELinks);
  ent_cm_dillo = tea_text_entry (page, "Dillo", confile.cmd_Dillo);
  ent_cm_galeon = tea_text_entry (page, "Galeon", confile.cmd_Galeon);
  ent_cm_epiphany = tea_text_entry (page, "Epiphany", confile.cmd_Epiphany); 
  ent_cm_other = tea_text_entry (page, "Other", confile.cmd_Other);
  ent_cm_lynx = tea_text_entry (page, "Lynx", confile.cmd_Lynx);
  ent_cm_manual_browser = tea_text_entry (page, _("Browser for the manual"), confile.cmd_def_doc_browser);
  cb_use_def_doc_browser = tea_checkbox (page, _("Use your browser"), &confile.use_def_doc_browser); 

  return page;
}


static GtkWidget* wnd_options_editor (void)
{
  GtkWidget *page = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (page);
  sb_tab_size = tea_spinbutton (page, _("Size tabs to spaces"), confile.tab_size);
  cb_ins_spaces_on_tab_press = tea_checkbox (page, _("Insert spaces the TAB key"), &confile.ins_spaces_on_tab_press);
  cb_use_auto_indent = tea_checkbox (page, _("Indentation automatique"), &confile.use_auto_indent);
	cb_use_infotext = tea_checkbox (page, _("Show information SPACE, TAB, NEWLINE ..."), &confile.use_infotext);
	cb_use_spellcheck = tea_checkbox (page, _("Enable spell check"), &confile.use_spellcheck);

  return page;
}


static GtkWidget* wnd_options_paths (void)
{
  GtkWidget *page =gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (page);
  ent_def_save_dir = tea_dir_selector (page, _("Default file saving directory"), confile.def_save_dir);
  cb_use_def_save_dir = tea_checkbox (page, _("Use default saving directory"), &confile.use_def_save_dir);
  ent_def_open_dir = tea_dir_selector (page, _("Default file opening directory"), confile.def_open_dir);
  cb_use_def_open_dir = tea_checkbox (page, _("Use default file opening directory"), &confile.use_def_open_dir);
  
  return page;
}


static GtkWidget* wnd_options_coauthor (void)
{
  GtkWidget *page = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (page);

  cb_do_autorep = tea_checkbox (page, _("automatic replacement"), &confile.do_autorep);
		cb_use_textcompbloc = tea_checkbox (page, _("Autocompletion of tags and block {} () ..."), &confile.use_textcompbloc);
  return page;
}



GtkWidget* wnd_options_create (void)
{
  wnd_options = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_window_set_title (GTK_WINDOW (wnd_options), _("Options"));
  GtkWidget *vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (wnd_options), vbox1);

  GtkWidget *notebook = gtk_notebook_new ();         
  gtk_notebook_set_tab_pos (GTK_NOTEBOOK(notebook), GTK_POS_LEFT);

  gtk_widget_show (notebook);

  gtk_container_add (GTK_CONTAINER (vbox1), notebook);

  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), wnd_options_switches (), gtk_label_new (_("Main")));
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), wnd_options_encoding (), gtk_label_new (_("Encoding")));
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), wnd_options_functions (), gtk_label_new (_("Fonctions")));
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), wnd_options_interface (), gtk_label_new (_("Interface")));
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), wnd_options_limits (), gtk_label_new (_("Limitations")));
  //gtk_notebook_append_page (GTK_NOTEBOOK(notebook), wnd_options_fonts (), gtk_label_new (_("Fonts")));
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), wnd_options_editor (), gtk_label_new (_("Editeur")));
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), wnd_options_browsers (), gtk_label_new (_("Navigateurs")));
  //gtk_notebook_append_page (GTK_NOTEBOOK(notebook), wnd_options_colors (), gtk_label_new (_("Couleurs")));
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), wnd_options_commands (), gtk_label_new (_("Commandes")));
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), wnd_options_paths (), gtk_label_new (_("Paths")));
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), wnd_options_coauthor (), gtk_label_new (_("complementation")));
  gtk_notebook_append_page (GTK_NOTEBOOK(notebook), wnd_options_confirmations (), gtk_label_new (_("Confirmations")));
  //gtk_notebook_append_page (GTK_NOTEBOOK(notebook), wnd_options_maintenance (), gtk_label_new (_("Maintenance")));

  gtk_notebook_set_current_page (GTK_NOTEBOOK(notebook), confile.last_prefs_tab);

  GtkWidget *hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_show (hbox1);
  gtk_container_add (GTK_CONTAINER (vbox1), hbox1);

  GtkWidget *bt_close = gtk_button_new_with_label (_("Close"));
  gtk_widget_show (bt_close);
  gtk_container_add (GTK_CONTAINER (hbox1), bt_close);
  g_signal_connect (bt_close, "clicked", G_CALLBACK (cb_on_bt_close), wnd_options);

  GtkWidget *bt_apply = gtk_button_new_with_label (_("Save and apply"));
  gtk_widget_show (bt_apply);
  gtk_container_add (GTK_CONTAINER (hbox1), bt_apply);
  g_signal_connect (bt_apply, "clicked", G_CALLBACK (cb_on_bt_apply), wnd_options);

  gtk_window_set_position (GTK_WINDOW(wnd_options), GTK_WIN_POS_CENTER);   

  gtk_widget_show (wnd_options);
  return wnd_options;
}

