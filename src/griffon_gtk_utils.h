GtkWidget *mni_temp;

GtkWidget* tea_text_entry (GtkWidget *container, gchar *caption, gchar *value);
GtkWidget* tea_font_selector (GtkWidget *container, gchar *caption, gchar *value);
GtkWidget* tea_color_selector (GtkWidget *container, gchar *caption, gchar *value);
GtkWidget* tea_checkbox (GtkWidget *container, gchar *caption, gboolean *value);
GtkWidget* tea_spinbutton (GtkWidget *container, gchar *caption, gdouble value);
void logmemo_set_pos (GtkTextView *v, guint pos);
GList* add_combo_hist_with_def ( GList *l);
GtkWidget* tea_dir_selector (GtkWidget *container, gchar *caption, gchar *value);
GtkWidget* tea_combo (GtkWidget *container, gchar *caption);
void widget_set_colors (GtkWidget *w, GdkColor *background, GdkColor *text, GdkColor *selection, GdkColor *sel_text);
gboolean dlg_question (GtkWidget *parent, gchar *prompt, gchar *val);
gint dlg_question_params (gchar *prompt, gchar *val, gchar *b1, gchar *b2);
void dlg_info (GtkWidget *parent, gchar *prompt, gchar *val);
GtkWidget* tea_button_at_box (GtkWidget *container, gchar *caption, gpointer callback, gpointer user_data);
void dlg_info_with_image (GtkWidget *parent, gchar *prompt, gchar *val, gchar *image);
void cb_on_bt_close (GtkWidget *w); 
GtkWidget* find_menuitem (GtkContainer *widget, gchar *widget_name);
