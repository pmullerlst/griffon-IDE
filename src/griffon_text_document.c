
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

#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <glib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <gtksourceview/gtksource.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguage.h>
#include <gtksourceview/gtksourcelanguagemanager.h>
#include <gtksourceview/gtksourcestyleschememanager.h>
#include <gtksourceview/completion-providers/words/gtksourcecompletionwords.h>
#include <gtksourceview/gtksourcecompletion.h>
#include "griffon_text_document.h"
#include "griffon_defs.h"
#include "callbacks.h"
#include "interface.h"
#include "griffon_config.h"
#include "griffon_funx.h"
#include "griffon_hl.h"
#include "griffon_gtk_utils.h"
#include "rox_strings.h"
#include "griffon_enc.h"

//static GtkSourceCompletionWords *word_provider,*word_provider2;
static GtkSourceCompletionWords *word_provider2;
static GtkSourceCompletionWords *word_provider3;
GtkSourceCompletion *completion;

static gchar *compute_indentation(GtkTextBuffer *buffer, gint line) // from gedit
{
	GtkTextIter start_iter, end_iter;
	gunichar ch;
	
	gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer), &start_iter, line);
	end_iter = start_iter;
	ch = gtk_text_iter_get_char(&end_iter);
	while (g_unichar_isspace(ch) && ch != '\n') {
		if (!gtk_text_iter_forward_char(&end_iter))
			break;
		ch = gtk_text_iter_get_char(&end_iter);
	}
	if (gtk_text_iter_equal(&start_iter, &end_iter))
		return NULL;
	
	return gtk_text_iter_get_text(&start_iter, &end_iter);
}

void indent_real(GtkWidget *text_view)
{
	GtkTextIter iter;
	gchar *ind, *str;

	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
	
	gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(buffer), &iter, gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer)));
	ind = compute_indentation(buffer, gtk_text_iter_get_line(&iter));
	str = g_strconcat("\n", ind, NULL);
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &iter, str, -1);
	g_free(str);
	g_free(ind);

	gtk_text_view_scroll_mark_onscreen(
	GTK_TEXT_VIEW(text_view),
	gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer)));
}

void doc_indent_selection (t_note_page *doc, gboolean unindent)
 {
	GtkTextIter itstart,itend;
	if (gtk_text_buffer_get_selection_bounds(GTK_TEXT_BUFFER(doc->text_buffer),&itstart,&itend)) {
		GtkTextMark *end;

		end = gtk_text_buffer_create_mark(GTK_TEXT_BUFFER(doc->text_buffer),NULL,&itend,TRUE);
		if (gtk_text_iter_get_line_offset(&itstart)>0) {
			gtk_text_iter_set_line_index(&itstart,0);
		}	
		while(gtk_text_iter_compare(&itstart,&itend) < 0) {
			GtkTextMark *cur;

			cur = gtk_text_buffer_create_mark(GTK_TEXT_BUFFER(doc->text_buffer),NULL,&itstart,TRUE);
			if (unindent) {

				gboolean cont=TRUE;
				gunichar cchar = gtk_text_iter_get_char(&itstart);
				if (cchar == 9) { 
					itend = itstart;
					cont = gtk_text_iter_forward_char(&itend);
				} else if (cchar == 32) { 
					gchar *tmpstr;
					gint i=0;
					itend = itstart;
					gtk_text_iter_forward_chars(&itend,confile.tab_size);
					tmpstr = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(doc->text_buffer),&itstart,&itend,FALSE);

					while (cont && tmpstr[i] != '\0') {
						cont = (tmpstr[i] == ' ');
						i++;
					}
					if (cont) {
						printf("\n");
					} else {
						g_free(tmpstr);
					}
				} else {
					cont = FALSE;
				}
				if (cont) {
					gtk_text_buffer_delete(GTK_TEXT_BUFFER(doc->text_buffer),&itstart,&itend);
				}
#ifdef DEBUG
				else {

				}
#endif
			} else {
				gchar *indentstring;
				gint indentlen;
				if (confile.ins_spaces_on_tab_press) {
                                          indentstring = g_strnfill (confile.tab_size, ' '); 
                  

					indentlen = confile.tab_size;
				} else {
					indentstring = g_strdup("\t");
					indentlen=1;
				}
				gtk_text_buffer_insert(GTK_TEXT_BUFFER(doc->text_buffer),&itstart,indentstring,indentlen);

				g_free(indentstring);
			}
			gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(doc->text_buffer),&itstart,cur);
			gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(doc->text_buffer),&itend,end);
			gtk_text_buffer_delete_mark(GTK_TEXT_BUFFER(doc->text_buffer),cur);
			gtk_text_iter_forward_line(&itstart);
		}
		gtk_text_buffer_delete_mark(GTK_TEXT_BUFFER(doc->text_buffer),end);
	} else {

		GtkTextIter iter;
		gtk_text_buffer_get_iter_at_mark(GTK_TEXT_BUFFER(doc->text_buffer),&iter,gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(doc->text_buffer)));
		gtk_text_iter_set_line_offset(&iter,0);
		if (unindent) {
			gint deletelen = 0;
			gchar *tmpstr, *tmp2str;
			GtkTextIter itend = iter;
			gtk_text_iter_forward_chars(&itend,confile.tab_size);
			tmpstr = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(doc->text_buffer),&iter,&itend,FALSE);
                        tmp2str = g_strnfill  (confile.tab_size, ' '); 
			if (tmpstr[0] == '\t') {
				deletelen = 1;
			} else if (tmpstr && strncmp(tmpstr,tmp2str,confile.tab_size)==0) {
				deletelen = confile.tab_size;
			}
			g_free(tmpstr);
			g_free(tmp2str);
			if (deletelen) {
				itend = iter;
				gtk_text_iter_forward_chars(&itend,deletelen);
				gtk_text_buffer_delete(GTK_TEXT_BUFFER(doc->text_buffer),&iter,&itend);
			}
		} else {
			gchar *indentstring;
			gint indentlen;
			if (confile.ins_spaces_on_tab_press) {
                                indentstring = g_strnfill  (confile.tab_size, ' '); 
				indentlen = confile.tab_size;
			} else {
				indentstring = g_strdup("\t");
				indentlen=1;
			}
			gtk_text_buffer_insert(GTK_TEXT_BUFFER(doc->text_buffer),&iter,indentstring,indentlen);
			g_free(indentstring);
		}
	}
}

gint widget_get_string_size (GtkWidget *w, gchar *s) 
{
	gint r = -1;

	PangoLayout *l = gtk_widget_create_pango_layout (w, s);
	if (l)
	{
		pango_layout_get_pixel_size (l, &r, NULL);
		g_object_unref (G_OBJECT (l));
	}

	return r;
}

static gint textview_calculate_real_tab_width(GtkWidget *textview, gint tab_size) {
	gchar *tab_string;
	gint counter = 0;
	gint tab_width = 0;

	if (tab_size <= 0)
		return 0;

	tab_string = g_malloc (tab_size + 1);
	while (counter < tab_size) {
		tab_string[counter] = ' ';
		counter++;
	}
	tab_string[tab_size] = '\0';
	tab_width =  widget_get_string_size(textview, tab_string);
	g_free(tab_string);

	return tab_width;
}


void doc_set_tabsize(t_note_page *doc, gint tabsize) {
	PangoTabArray *tab_array;
	gint pixels = textview_calculate_real_tab_width(GTK_WIDGET(doc->text_view), tabsize);

	tab_array = pango_tab_array_new (1, TRUE);
	pango_tab_array_set_tab (tab_array, 0, PANGO_TAB_LEFT, pixels);
	gtk_text_view_set_tabs (GTK_TEXT_VIEW (doc->text_view), tab_array);
	pango_tab_array_free(tab_array);
}


gboolean doc_textview_expose_event_lcb(GtkWidget * widget, GdkEventExpose * event) {
	GtkTextView *view = (GtkTextView*)widget;
	GdkRectangle rect;
	GdkWindow *win;
	GtkTextIter l_start,l_end,it;
	gint l_top1,l_top2;
	PangoLayout *l;
	gchar *pomstr;
	gint numlines,w,i;

	win = gtk_text_view_get_window(GTK_TEXT_VIEW(view),GTK_TEXT_WINDOW_LEFT);
	if (win!=event->window) return FALSE;

	gtk_text_view_get_visible_rect(GTK_TEXT_VIEW(view),&rect);
	gtk_text_view_get_line_at_y(GTK_TEXT_VIEW(view),&l_start,rect.y,&l_top1);
	gtk_text_view_get_line_at_y(GTK_TEXT_VIEW(view),&l_end,rect.y+rect.height,&l_top2);
	l = gtk_widget_create_pango_layout(widget,"");

	numlines = gtk_text_buffer_get_line_count(GTK_TEXT_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view))));
	pomstr = g_strdup_printf("%d",MAX(99,numlines));
	pango_layout_set_text(l,pomstr,-1);
	g_free(pomstr);
	pango_layout_get_pixel_size(l,&w,NULL);
	gtk_text_view_set_border_window_size(GTK_TEXT_VIEW(view),GTK_TEXT_WINDOW_LEFT,w+4);
	it = l_start;
	for(i=gtk_text_iter_get_line(&l_start);i<=gtk_text_iter_get_line(&l_end);i++) {
		gtk_text_iter_set_line(&it,i);
		gtk_text_view_get_line_yrange(GTK_TEXT_VIEW(view),&it,&w,NULL);
		gtk_text_view_buffer_to_window_coords(GTK_TEXT_VIEW(view),GTK_TEXT_WINDOW_LEFT,0,w,NULL,&w);

		pomstr = g_strdup_printf("%d",i+1);
		pango_layout_set_text(l,pomstr,-1);

		g_free(pomstr);
	}
	g_object_unref(G_OBJECT(l));
	return TRUE;
}


void document_set_line_numbers(t_note_page* doc, gboolean value) {
	if (value) {
		gtk_text_view_set_left_margin(GTK_TEXT_VIEW(doc->text_view),2);
		gtk_text_view_set_border_window_size(GTK_TEXT_VIEW(doc->text_view),GTK_TEXT_WINDOW_LEFT,20);
		g_signal_connect(G_OBJECT(doc->text_view),"expose-event",G_CALLBACK(doc_textview_expose_event_lcb),doc);
	} else {
		gtk_text_view_set_left_margin(GTK_TEXT_VIEW(doc->text_view),0);
		gtk_text_view_set_border_window_size(GTK_TEXT_VIEW(doc->text_view),GTK_TEXT_WINDOW_LEFT,0);
	}
	doc->linenums = value;
}


static void msg_wrong_encoding (gchar *filename)
{	
	icon_stop_logmemo();
	log_to_memo (_("Sorry, but I can not determine the character set %s. try again and select it manually."), filename, LM_ERROR);
}


void do_backup (gchar *file_name, gboolean do_check)
{
	if (do_check)
		if (! confile.do_backup)
			return;

	if (! g_file_test (file_name, G_FILE_TEST_EXISTS))
		return;

	gchar *bak = g_strconcat (file_name, ".bak", NULL);

	if (copy_file (file_name, bak))
	{
		icon_ok_logmemo();
		log_to_memo (_("%s is created"), bak, LM_NORMAL);
	}
	else
	{
		icon_nosave_logmemo();
		icon_affiche_nosave ();
		controle_save_page_icon_no();
		log_to_memo (_("Can not save to %s"), file_name, LM_ERROR);
	}
	g_free (bak);
}


gboolean text_doc_save (t_note_page *doc, gchar *a_filename)
{
	if (doc->readonly)
	{
		icon_log_logmemo();
		icon_affiche_nosave ();
		controle_save_page_icon_no();
		log_to_memo (_("The file is read-only."), NULL, LM_ERROR);
		return FALSE;
	}

	gboolean result;
	gchar *filename = get_l_filename (a_filename);

	if (! filename)
		return FALSE;

	if (is_rtf (filename))
	{
		icon_nosave_logmemo();
		log_to_memo (_("Griffon can't save RTF. Because."), filename, LM_ERROR);
		g_free (filename);
		return FALSE;
	}
 
	if (g_file_test (filename, G_FILE_TEST_EXISTS))
		if (access (filename, W_OK) != 0)
		{
			icon_nosave_logmemo();
			icon_affiche_nosave ();
			controle_save_page_icon_no();
			log_to_memo (_("%s is not writable for you!"), filename, LM_ERROR);
			g_free (filename); 
			return FALSE;
		}

	if (g_file_test (filename, G_FILE_TEST_IS_DIR))
	{
		icon_nosave_logmemo();
		icon_affiche_nosave ();
		controle_save_page_icon_no();

		log_to_memo (_("And how I can save this text file as a directory?!"), NULL, LM_ERROR);
		g_free (filename);
		return FALSE;
	}

	if (g_utf8_collate (doc->encoding, "UTF-8") == 0)
	{
		do_backup (filename, TRUE);
		result = doc_save_buffer_to_file (GTK_TEXT_BUFFER(doc->text_buffer), filename);
	}
	else
	if (g_utf8_collate (doc->encoding, CURR_LOCALE) == 0)
	{
		do_backup (filename, TRUE);
		result = doc_save_buffer_to_file_l (GTK_TEXT_BUFFER(doc->text_buffer), filename);
	}
	else
	{
		do_backup (filename, TRUE);
		result = doc_save_buffer_to_file_iconv (GTK_TEXT_BUFFER(doc->text_buffer), filename, doc->encoding);
	}

	if (g_utf8_collate (filename, confile.tea_rc) == 0)
	{
		confile_reload();
		doc_update_all();
		update_enc_menu();
		icon_log_logmemo();
		log_to_memo (_("config reloaded"), NULL, LM_NORMAL);
	}

	if (g_utf8_collate (filename, confile.tea_hotkeys) == 0)
	{
		reload_hotkeys ();
		icon_log_logmemo();
		log_to_memo (_("hotkeys reloaded"), NULL, LM_NORMAL);
	}

	if (g_utf8_collate (filename, confile.user_menu_file) == 0)
	{
		reload_usermenu ();
		icon_log_logmemo();
		log_to_memo (_("user menu reloaded"), NULL, LM_NORMAL);
	}

	if (g_utf8_collate (filename, confile.tea_kwas_bookmarks) == 0)
	{
		glist_strings_free (gl_tea_kwas_bookmarks); 
		if (g_file_test (confile.tea_kwas_bookmarks, G_FILE_TEST_EXISTS))
		gl_tea_kwas_bookmarks = load_file_to_glist (confile.tea_kwas_bookmarks);
		icon_log_logmemo();
		log_to_memo (_("kwas bookmarks reloaded"), NULL, LM_NORMAL);
	}

	if (g_utf8_collate (filename, confile.tea_autoreplace) == 0)
		reload_autoreplace ();

	if (g_utf8_collate (filename, confile.iconv_file) == 0)
	{
		get_iconv_sup();
		update_enc_menu();
		icon_log_logmemo();
		log_to_memo (_("%s reloaded"), confile.iconv_file, LM_NORMAL);
	}

	if (g_utf8_collate (filename, confile.bmx_file) == 0)
	{
		bmx_reload ();
		icon_log_logmemo();
		log_to_memo (_("%s reloaded"), confile.bmx_file, LM_NORMAL);
	}

	doc->hl_mode = ch_str (doc->hl_mode, get_hl_name (doc->file_name));
	gtk_text_buffer_set_modified (GTK_TEXT_BUFFER(doc->text_buffer), FALSE);  

	char *extension;
	if(strrchr(doc->file_name,'.'))
	{
		extension = strrchr(doc->file_name,'.');

		if (strcmp(".htm", extension) == 0 || strcmp(".html", extension) == 0 || strcmp(".php", extension) == 0)
		{
		focus_web ();
		}
	}

	controle_save_page_icon();
	g_free (filename);

	return result;
}


gchar* doc_get_sel (t_note_page *doc) 
{
	GtkTextIter start;
	GtkTextIter end;
	if (gtk_text_buffer_get_selection_bounds (GTK_TEXT_BUFFER(doc->text_buffer), &start, &end))
		return (gtk_text_buffer_get_text (GTK_TEXT_BUFFER(doc->text_buffer), &start, &end, FALSE));
	else
	return NULL;
}


gboolean doc_is_sel (GtkTextBuffer *text_buffer)
{
	return gtk_text_buffer_get_selection_bounds (GTK_TEXT_BUFFER(text_buffer), NULL, NULL);
}


gchar* doc_get_buf (GtkTextBuffer *text_buffer)
{
	GtkTextIter itstart, itend;

	gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(text_buffer), &itstart, 0);
	gtk_text_buffer_get_iter_at_offset(GTK_TEXT_BUFFER(text_buffer), &itend, gtk_text_buffer_get_char_count (GTK_TEXT_BUFFER(text_buffer)));

	return gtk_text_buffer_get_text (GTK_TEXT_BUFFER(text_buffer), &itstart, &itend, TRUE);
}


void doc_select_line (t_note_page *doc, gint line)
{
	GtkTextIter itstart, itend;
	gtk_text_buffer_get_iter_at_line (GTK_TEXT_BUFFER(doc->text_buffer), &itstart, line - 1);
	itend = itstart;
	if (gtk_text_iter_forward_to_line_end (&itend))
	{
		gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER(doc->text_buffer), &itstart);
		GtkTextMark *m = gtk_text_buffer_get_mark (GTK_TEXT_BUFFER(doc->text_buffer), "insert");
			if (m)
			{
				gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW(doc->text_view), m, 0.0, TRUE, 0.0, 0.0);
				gtk_text_buffer_move_mark_by_name (GTK_TEXT_BUFFER(doc->text_buffer), "selection_bound", &itend);
			}
	} 
}


gboolean doc_save_buffer_to_file (GtkTextBuffer *text_buffer, gchar *filename)
{
	gchar *t = doc_get_buf (text_buffer);
	if (! t)
		return FALSE;
	gboolean r = save_string_to_file (filename, t);
	g_free (t);
	return r;
}


gboolean doc_save_buffer_to_file_l (GtkTextBuffer *text_buffer, gchar *filename)
{
	gsize bytes_read;
	gsize bytes_written;
	gchar *buf = doc_get_buf (text_buffer);

	if (! buf)
		return FALSE;

	gchar *lbuf = g_locale_from_utf8 (buf, -1, &bytes_read, &bytes_written, NULL);
	gboolean result = save_string_to_file (filename, lbuf);

	g_free (buf);
	g_free (lbuf);

	return result;
}


gboolean doc_save_buffer_to_file_iconv (GtkTextBuffer *text_buffer, gchar *filename, gchar *enc)
{
	gchar *t;
	gboolean result = FALSE;
	gsize bytes_read;
	gsize bytes_written;

	gchar *buf = doc_get_buf (text_buffer);
	if (! buf)
		return FALSE;

	t  = g_convert (buf, -1, enc, "UTF-8", &bytes_read, &bytes_written, NULL);

	if (! t)
	{
		g_free (buf);
		return FALSE;
	}

	result = save_string_to_file (filename, t);

	g_free (buf);
	g_free (t);

	return result;
}

void on_button_close (GtkWidget *wid, gpointer data)
{
	gtk_widget_get_name(wid);
	icon_affiche_ok();
	page_del_by_index (find_index_by_page (data));
	no_onglet_open();
	delete_autocomp_tips();
}


t_note_page* page_create_new (void)
{
	t_note_page *page = (t_note_page *) g_malloc (sizeof (t_note_page));

	page->position = 1;
	page->readonly = FALSE;
	page->current_path = NULL;
	page->last_searched_text = NULL;
	page->hl_mode = g_strdup (HL_PO);

	page->scrolledwindow = gtk_scrolled_window_new (NULL, NULL);

	page->text_buffer = GTK_SOURCE_BUFFER (gtk_source_buffer_new (NULL));
	page->text_view = gtk_source_view_new_with_buffer(page->text_buffer);
	gtk_source_view_set_highlight_current_line((GtkSourceView *)page->text_view,TRUE);
	gtk_source_view_set_show_right_margin((GtkSourceView *)page->text_view,TRUE);

	if(confile.use_infotext == 1){gtk_source_view_set_draw_spaces((GtkSourceView *)page->text_view,GTK_SOURCE_DRAW_SPACES_ALL);}

	completion = gtk_source_view_get_completion ((GtkSourceView *)page->text_view);

		//word_provider = gtk_source_completion_words_new (NULL, NULL);

	gtk_source_completion_words_register (word_provider,
	                                      gtk_text_view_get_buffer (GTK_TEXT_VIEW (page->text_view)));

	gtk_source_completion_add_provider (completion,
	                                    GTK_SOURCE_COMPLETION_PROVIDER (word_provider),
	                                    NULL);

	g_object_set (word_provider, "priority", 10, NULL);

	GtkSourceBuffer *tmpbuffer = GTK_SOURCE_BUFFER (gtk_source_buffer_new (NULL));
	GtkWidget *srctmp= gtk_source_view_new_with_buffer(tmpbuffer);


		gchar lecture[2024];
		FILE *fichier;
		if(fopen(confile.tea_autocomp,"rt"))
		{
			fichier = fopen(confile.tea_autocomp,"rt");
			while(fgets(lecture, 2024, fichier))
			{
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(tmpbuffer),g_locale_to_utf8(lecture, -1, NULL, NULL, NULL) , -1);
			}
		fclose(fichier);

		word_provider2 = gtk_source_completion_words_new ("My AutoComp", NULL);

	gtk_source_completion_words_register (word_provider2,
	                                      gtk_text_view_get_buffer (GTK_TEXT_VIEW (srctmp)));

	gtk_source_completion_add_provider (completion,
	                                    GTK_SOURCE_COMPLETION_PROVIDER (word_provider2),
	                                    NULL);

	g_object_set (word_provider2, "priority", 10, NULL);
	}
	
	page->encoding = g_strdup ("UTF-8");
	page->linenums = FALSE;
 
	if (confile.word_wrap == 1)
		gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (page->text_view), GTK_WRAP_WORD);
	else
		gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (page->text_view), GTK_WRAP_NONE);

	gtk_container_add (GTK_CONTAINER (page->scrolledwindow), GTK_WIDGET(page->text_view));

	gtk_text_buffer_create_tag (GTK_TEXT_BUFFER(page->text_buffer), "folding","weight", PANGO_WEIGHT_HEAVY,"style",PANGO_STYLE_ITALIC,NULL);
	gtk_text_buffer_create_tag (GTK_TEXT_BUFFER(page->text_buffer), "search","foreground", "white","background", "blue","weight", PANGO_WEIGHT_BOLD,NULL);
	gtk_text_buffer_create_tag (GTK_TEXT_BUFFER(page->text_buffer), "err2","foreground", "black","background", "pink","weight", PANGO_WEIGHT_BOLD,NULL);
	gtk_text_buffer_create_tag (GTK_TEXT_BUFFER(page->text_buffer), "gray_bg","foreground", "white","background", "gray","weight", PANGO_WEIGHT_BOLD,NULL);

	page->tab_label = gtk_label_new (NULL);

	gtk_widget_set_can_focus (GTK_WIDGET (page->tab_label), FALSE);

	page->hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	page->but = gtk_button_new_with_label (_("x"));


	gtk_widget_set_can_focus (GTK_WIDGET (page->hbox), FALSE);
	gtk_widget_set_can_focus (GTK_WIDGET (page->but), FALSE);

//	page->icon = gtk_image_new_from_stock (GTK_STOCK_SAVE,GTK_ICON_SIZE_MENU);
	page->icon = gtk_image_new_from_file("/usr/local/share/griffon/images/empty.png");

	gtk_box_pack_start (GTK_BOX (page->hbox), page->icon, FALSE, FALSE, 0);
	gtk_widget_show (GTK_WIDGET(page->icon));

	gtk_box_pack_start (GTK_BOX(page->hbox), page->tab_label, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(page->hbox), page->but, FALSE, FALSE, 0);

	gtk_widget_show (GTK_WIDGET(page->hbox));  
	gtk_widget_show (GTK_WIDGET(page->tab_label));
	gtk_widget_set_has_tooltip(page->but,FALSE);

	g_signal_connect ((gpointer) page->but, "clicked",
                    G_CALLBACK (on_button_close),
                    page);
  
	page->b_saved = FALSE;
	page->file_name = g_strdup ("noname");

	g_signal_connect (G_OBJECT (page->text_view), "line-mark-activated",G_CALLBACK (code_folding), NULL);

	g_signal_connect (G_OBJECT (page->text_view), "key_press_event",
                    G_CALLBACK (on_editor_keypress), page);

	g_signal_connect (G_OBJECT (page->text_view), "key_press_event",
                    G_CALLBACK (window_popup_delete), page);

	g_signal_connect (G_OBJECT (page->text_view), "key_release_event",
                    G_CALLBACK (on_editor_keyrelease), page);

	//g_signal_connect (G_OBJECT (page->text_view), "key_press_event",G_CALLBACK (preview_web_popup_line), page);

	g_signal_connect (G_OBJECT (page->text_view), "focus-out-event",
                    G_CALLBACK (hidden_popup), page);

	g_signal_connect (G_OBJECT (page->text_view), "focus-in-event",
                    G_CALLBACK (map_move), page);


/*	g_signal_connect (G_OBJECT (page->text_view), "window_popup_delete",
                    G_CALLBACK (hidden_popup), page);*/

/*
* BUG : test pour trouver le problème
*/

/*	g_signal_connect (G_OBJECT (page->text_buffer), "insert_text", 
                    G_CALLBACK (controle_save_page_icon_no), page->text_view);*/
                    		                                      		                    		

	g_signal_connect (G_OBJECT (page->text_view), "populate-popup", 
                    G_CALLBACK (populate_popup), NULL);

	dox = g_list_append (dox, page);
	gtk_notebook_append_page_menu (GTK_NOTEBOOK(notebook1), page->scrolledwindow, page->hbox, NULL);

	gtk_source_view_set_show_line_marks((GtkSourceView *)page->text_view,TRUE);
	gtk_source_view_set_show_line_numbers((GtkSourceView *)page->text_view,TRUE);		


	FILE *fich;
	char carac;
	char mot[1000];
	mot[0]='\0';

	if(fopen(confile.tea_theme,"r"))
	{
		fich=fopen(confile.tea_theme,"r");
		while ((carac =fgetc(fich)) != EOF)
		{    
			if (carac =='\n')
			{
			break;
			}
			else
			{
				strncat(mot,&carac,1);
			}
		}
	fclose(fich);
	}
	else{strcpy(mot, "classic");}

	GtkSourceStyleSchemeManager* sm = gtk_source_style_scheme_manager_new();
	GtkSourceStyleScheme* scheme = gtk_source_style_scheme_manager_get_scheme(sm, mot);
	gtk_source_buffer_set_style_scheme(page->text_buffer , scheme);

	gtk_box_pack_start (GTK_BOX (page->hbox), srctmp, FALSE, TRUE, 0);

	gtk_widget_show (GTK_WIDGET(page->but)); 
	gtk_widget_show (GTK_WIDGET(page->text_view));
	gtk_widget_show (GTK_WIDGET(page->scrolledwindow));
	gtk_widget_grab_focus (GTK_WIDGET(page->text_view));

		gtk_source_map_set_view(GTK_SOURCE_MAP(source_map1),GTK_SOURCE_VIEW(page->text_view));

	return page;
}

void theme_cobalt ()
{
	if (! get_page_text()) return;
	GtkSourceStyleSchemeManager* sm = gtk_source_style_scheme_manager_new();
	GtkSourceStyleScheme* scheme = gtk_source_style_scheme_manager_get_scheme(sm, "cobalt");
	gtk_source_buffer_set_style_scheme(cur_text_doc->text_buffer , scheme);

	gtk_source_buffer_set_style_scheme(log_memo_textbuffer , scheme);
	gtk_source_buffer_set_style_scheme(buffer_note , scheme);

	create_empty_file (confile.tea_theme, "cobalt\n");
	confile.theme = "cobalt";
	gtk_source_buffer_set_style_scheme(buffer_projet , scheme);
}

void theme_classic ()
{
	if (! get_page_text()) return;
	GtkSourceStyleSchemeManager* sm = gtk_source_style_scheme_manager_new();
	GtkSourceStyleScheme* scheme = gtk_source_style_scheme_manager_get_scheme(sm, "classic");
	gtk_source_buffer_set_style_scheme(cur_text_doc->text_buffer , scheme);
	create_empty_file (confile.tea_theme, "classic\n");
	confile.theme = "classic";
	gtk_source_buffer_set_style_scheme(log_memo_textbuffer , scheme);
	gtk_source_buffer_set_style_scheme(buffer_note , scheme);
	gtk_source_buffer_set_style_scheme(buffer_projet , scheme);
}

void theme_kate  ()
{
	if (! get_page_text()) return;
	GtkSourceStyleSchemeManager* sm = gtk_source_style_scheme_manager_new();
	GtkSourceStyleScheme* scheme = gtk_source_style_scheme_manager_get_scheme(sm, "kate");
	gtk_source_buffer_set_style_scheme(cur_text_doc->text_buffer , scheme);
	create_empty_file (confile.tea_theme, "kate\n");
	confile.theme = "kate";
	gtk_source_buffer_set_style_scheme(log_memo_textbuffer , scheme);
	gtk_source_buffer_set_style_scheme(buffer_note , scheme);
	gtk_source_buffer_set_style_scheme(buffer_projet , scheme);
}

void theme_oblivion  ()
{
	if (! get_page_text()) return;
	GtkSourceStyleSchemeManager* sm = gtk_source_style_scheme_manager_new();
	GtkSourceStyleScheme* scheme = gtk_source_style_scheme_manager_get_scheme(sm, "oblivion");
	gtk_source_buffer_set_style_scheme(cur_text_doc->text_buffer , scheme);
	create_empty_file (confile.tea_theme, "oblivion\n");
	confile.theme = "oblivion";
	gtk_source_buffer_set_style_scheme(log_memo_textbuffer , scheme);
	gtk_source_buffer_set_style_scheme(buffer_note , scheme);
	gtk_source_buffer_set_style_scheme(buffer_projet , scheme);
}

void theme_tango  ()
{
	if (! get_page_text()) return;
	GtkSourceStyleSchemeManager* sm = gtk_source_style_scheme_manager_new();
	GtkSourceStyleScheme* scheme = gtk_source_style_scheme_manager_get_scheme(sm, "tango");
	gtk_source_buffer_set_style_scheme(cur_text_doc->text_buffer , scheme);
	create_empty_file (confile.tea_theme, "tango\n");
	confile.theme = "tango";
	gtk_source_buffer_set_style_scheme(buffer_projet , scheme);
}

void theme_solarizedd  ()
{
	if (! get_page_text()) return;
	GtkSourceStyleSchemeManager* sm = gtk_source_style_scheme_manager_new();
	GtkSourceStyleScheme* scheme = gtk_source_style_scheme_manager_get_scheme(sm, "solarized-dark");
	gtk_source_buffer_set_style_scheme(cur_text_doc->text_buffer , scheme);
	create_empty_file (confile.tea_theme, "solarized-dark\n");
	confile.theme = "solarized-dark";
	gtk_source_buffer_set_style_scheme(buffer_projet , scheme);
}

void theme_solarizedl  ()
{
	if (! get_page_text()) return;
	GtkSourceStyleSchemeManager* sm = gtk_source_style_scheme_manager_new();
	GtkSourceStyleScheme* scheme = gtk_source_style_scheme_manager_get_scheme(sm, "solarized-light");
	gtk_source_buffer_set_style_scheme(cur_text_doc->text_buffer , scheme);
	create_empty_file (confile.tea_theme, "solarized-light\n");
	confile.theme = "solarized-light";
	gtk_source_buffer_set_style_scheme(buffer_projet , scheme);
}

void auto_hl_griffon_perl ()
{
	if (! get_page_text()) return;
	GtkSourceLanguageManager *lm_perl;
	GtkSourceLanguage *language_perl = NULL;

	lm_perl = gtk_source_language_manager_new();
	g_object_ref (lm_perl);
	g_object_set_data_full ( G_OBJECT (cur_text_doc->text_buffer), "languages-manager",
                           lm_perl, (GDestroyNotify) g_object_unref);

	lm_perl = g_object_get_data (G_OBJECT (cur_text_doc->text_buffer), "languages-manager");
	language_perl = gtk_source_language_manager_get_language (lm_perl,"perl");
	gtk_source_buffer_set_language (cur_text_doc->text_buffer, language_perl);	
}

void auto_hl_griffon_sql ()
{
	if (! get_page_text()) return;
	GtkSourceLanguageManager *lm_perl;
	GtkSourceLanguage *language_perl = NULL;

	lm_perl = gtk_source_language_manager_new();
	g_object_ref (lm_perl);
	g_object_set_data_full ( G_OBJECT (cur_text_doc->text_buffer), "languages-manager",
                           lm_perl, (GDestroyNotify) g_object_unref);

	lm_perl = g_object_get_data (G_OBJECT (cur_text_doc->text_buffer), "languages-manager");
	language_perl = gtk_source_language_manager_get_language (lm_perl,"sql");
	gtk_source_buffer_set_language (cur_text_doc->text_buffer, language_perl);	
}

void auto_hl_griffon_csv ()
{
	if (! get_page_text()) return;
	GtkSourceLanguageManager *lm_perl;
	GtkSourceLanguage *language_perl = NULL;

	lm_perl = gtk_source_language_manager_new();
	g_object_ref (lm_perl);
	g_object_set_data_full ( G_OBJECT (cur_text_doc->text_buffer), "languages-manager",
                           lm_perl, (GDestroyNotify) g_object_unref);

	lm_perl = g_object_get_data (G_OBJECT (cur_text_doc->text_buffer), "languages-manager");
	language_perl = gtk_source_language_manager_get_language (lm_perl,"csv");
	gtk_source_buffer_set_language (cur_text_doc->text_buffer, language_perl);	
}

void auto_hl_griffon_php ()
{
	if (! get_page_text()) return;
	GtkSourceLanguageManager *lm_php;
	GtkSourceLanguage *language_php = NULL;

	lm_php = gtk_source_language_manager_new();
	g_object_ref (lm_php);
	g_object_set_data_full ( G_OBJECT (cur_text_doc->text_buffer), "languages-manager",
                           lm_php, (GDestroyNotify) g_object_unref);

	lm_php = g_object_get_data (G_OBJECT (cur_text_doc->text_buffer), "languages-manager");
	language_php = gtk_source_language_manager_get_language (lm_php,"php");
	gtk_source_buffer_set_language (cur_text_doc->text_buffer, language_php);	
}

void auto_hl_griffon_c ()
{
	if (! get_page_text()) return;
	GtkSourceLanguageManager *lm_c;
	GtkSourceLanguage *language_c = NULL;

	lm_c = gtk_source_language_manager_new();
	g_object_ref (lm_c);
	g_object_set_data_full ( G_OBJECT (cur_text_doc->text_buffer), "languages-manager",
                           lm_c, (GDestroyNotify) g_object_unref);

	lm_c = g_object_get_data (G_OBJECT (cur_text_doc->text_buffer), "languages-manager");
	language_c = gtk_source_language_manager_get_language (lm_c,"c");
	gtk_source_buffer_set_language (cur_text_doc->text_buffer, language_c);
}

void auto_hl_griffon_python ()
{
	if (! get_page_text()) return;
	GtkSourceLanguageManager *lm_c;
	GtkSourceLanguage *language_c = NULL;

	lm_c = gtk_source_language_manager_new();
	g_object_ref (lm_c);
	g_object_set_data_full ( G_OBJECT (cur_text_doc->text_buffer), "languages-manager",
                           lm_c, (GDestroyNotify) g_object_unref);

	lm_c = g_object_get_data (G_OBJECT (cur_text_doc->text_buffer), "languages-manager");
	language_c = gtk_source_language_manager_get_language (lm_c,"python");
	gtk_source_buffer_set_language (cur_text_doc->text_buffer, language_c);
}

void auto_hl_griffon_java ()
{
	if (! get_page_text()) return;
	GtkSourceLanguageManager *lm_c;
	GtkSourceLanguage *language_c = NULL;

	lm_c = gtk_source_language_manager_new();
	g_object_ref (lm_c);
	g_object_set_data_full ( G_OBJECT (cur_text_doc->text_buffer), "languages-manager",
                           lm_c, (GDestroyNotify) g_object_unref);

	lm_c = g_object_get_data (G_OBJECT (cur_text_doc->text_buffer), "languages-manager");
	language_c = gtk_source_language_manager_get_language (lm_c,"java");
	gtk_source_buffer_set_language (cur_text_doc->text_buffer, language_c);
}
void auto_hl_griffon_ruby ()
{
	if (! get_page_text()) return;
	GtkSourceLanguageManager *lm_c;
	GtkSourceLanguage *language_c = NULL;

	lm_c = gtk_source_language_manager_new();
	g_object_ref (lm_c);
	g_object_set_data_full ( G_OBJECT (cur_text_doc->text_buffer), "languages-manager",
                           lm_c, (GDestroyNotify) g_object_unref);

	lm_c = g_object_get_data (G_OBJECT (cur_text_doc->text_buffer), "languages-manager");
	language_c = gtk_source_language_manager_get_language (lm_c,"ruby");
	gtk_source_buffer_set_language (cur_text_doc->text_buffer, language_c);
}

void auto_hl_griffon_xml ()
{
	if (! get_page_text()) return;
	GtkSourceLanguageManager *lm_c;
	GtkSourceLanguage *language_c = NULL;

	lm_c = gtk_source_language_manager_new();
	g_object_ref (lm_c);
	g_object_set_data_full ( G_OBJECT (cur_text_doc->text_buffer), "languages-manager",
                           lm_c, (GDestroyNotify) g_object_unref);

	lm_c = g_object_get_data (G_OBJECT (cur_text_doc->text_buffer), "languages-manager");
	language_c = gtk_source_language_manager_get_language (lm_c,"xml");
	gtk_source_buffer_set_language (cur_text_doc->text_buffer, language_c);
}

void auto_hl_griffon_javascript ()
{
	if (! get_page_text()) return;
	GtkSourceLanguageManager *lm_c;
	GtkSourceLanguage *language_c = NULL;

	lm_c = gtk_source_language_manager_new();
	g_object_ref (lm_c);
	g_object_set_data_full ( G_OBJECT (cur_text_doc->text_buffer), "languages-manager",
                           lm_c, (GDestroyNotify) g_object_unref);

	lm_c = g_object_get_data (G_OBJECT (cur_text_doc->text_buffer), "languages-manager");
	language_c = gtk_source_language_manager_get_language (lm_c,"javascript");
	gtk_source_buffer_set_language (cur_text_doc->text_buffer, language_c);
}

void auto_hl_griffon_none ()
{
	if (! get_page_text()) return;
	GtkSourceLanguageManager *lm_c;

	lm_c = gtk_source_language_manager_new();
	g_object_ref (lm_c);
	g_object_set_data_full ( G_OBJECT (cur_text_doc->text_buffer), "languages-manager",
                           lm_c, (GDestroyNotify) g_object_unref);

	lm_c = g_object_get_data (G_OBJECT (cur_text_doc->text_buffer), "languages-manager");
	gtk_source_buffer_set_language (cur_text_doc->text_buffer, NULL);
}

void auto_hl_griffon_css ()
{
	if (! get_page_text()) return;
	GtkSourceLanguageManager *lm_css;
	GtkSourceLanguage *language_css = NULL;
	lm_css = gtk_source_language_manager_new();
	g_object_ref (lm_css);
	g_object_set_data_full ( G_OBJECT (cur_text_doc->text_buffer), "languages-manager",
                           lm_css, (GDestroyNotify) g_object_unref);

	lm_css = g_object_get_data (G_OBJECT (cur_text_doc->text_buffer), "languages-manager");
	language_css = gtk_source_language_manager_get_language (lm_css,"css");
	gtk_source_buffer_set_language (cur_text_doc->text_buffer, language_css);
}

void auto_hl_griffon_cpp ()
{
	if (! get_page_text()) return;
	GtkSourceLanguageManager *lm_css;
	GtkSourceLanguage *language_css = NULL;
	lm_css = gtk_source_language_manager_new();
	g_object_ref (lm_css);
	g_object_set_data_full ( G_OBJECT (cur_text_doc->text_buffer), "languages-manager",
                           lm_css, (GDestroyNotify) g_object_unref);

	lm_css = g_object_get_data (G_OBJECT (cur_text_doc->text_buffer), "languages-manager");
	language_css = gtk_source_language_manager_get_language (lm_css,"cpp");
	gtk_source_buffer_set_language (cur_text_doc->text_buffer, language_css);
}

void auto_hl_griffon_sh ()
{
	if (! get_page_text()) return;
	GtkSourceLanguageManager *lm_sh;
	GtkSourceLanguage *language_sh = NULL;
	lm_sh = gtk_source_language_manager_new();
	g_object_ref (lm_sh);
	g_object_set_data_full ( G_OBJECT (cur_text_doc->text_buffer), "languages-manager",
                           lm_sh, (GDestroyNotify) g_object_unref);

	lm_sh = g_object_get_data (G_OBJECT (cur_text_doc->text_buffer), "languages-manager");
	language_sh = gtk_source_language_manager_get_language (lm_sh,"sh");
	gtk_source_buffer_set_language (cur_text_doc->text_buffer, language_sh);
}

void auto_hl_griffon_po ()
{
	if (! get_page_text()) return;
	GtkSourceLanguageManager *lm_po;
	GtkSourceLanguage *language_po = NULL;
	lm_po = gtk_source_language_manager_new();
	g_object_ref (lm_po);
	g_object_set_data_full ( G_OBJECT (cur_text_doc->text_buffer), "languages-manager",
                           lm_po, (GDestroyNotify) g_object_unref);

	lm_po = g_object_get_data (G_OBJECT (cur_text_doc->text_buffer), "languages-manager");
	language_po = gtk_source_language_manager_get_language (lm_po,"po");
	gtk_source_buffer_set_language (cur_text_doc->text_buffer, language_po);
}

void auto_hl_griffon_html ()
{
	if (! get_page_text()) return;
	GtkSourceLanguageManager *lm_html;
	GtkSourceLanguage *language_html = NULL;
	lm_html = gtk_source_language_manager_new();
	g_object_ref (lm_html);
	g_object_set_data_full ( G_OBJECT (cur_text_doc->text_buffer), "languages-manager",
                           lm_html, (GDestroyNotify) g_object_unref);

	lm_html = g_object_get_data (G_OBJECT (cur_text_doc->text_buffer), "languages-manager");
	language_html = gtk_source_language_manager_get_language (lm_html,"html");
	gtk_source_buffer_set_language (cur_text_doc->text_buffer, language_html);
}

gpointer glist_find_by_index (GList *list, int i)
{
	return g_list_nth_data (list, i);
}

t_note_page* get_page (void)
{
	cur_text_doc = NULL;

	gint i = gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook1));
	if (i == -1)
		return NULL;
	else
		cur_text_doc = (t_note_page *) g_list_nth_data (dox, i);

	return cur_text_doc;
}


t_note_page* get_page_by_index (int i)
{
	return (t_note_page *) g_list_nth_data (dox, i);
}


void page_del_by_index (int i)
{
	t_note_page *page;

	if (i != -1)
	{
		page = glist_find_by_index (dox, i);
		icon_log_logmemo();
		log_to_memo (_("%s is closed"), page->file_name, LM_NORMAL);
		dox = g_list_remove (dox, page);
		page_free (page);
		gtk_notebook_remove_page (GTK_NOTEBOOK(notebook1), i);
		update_recent_list_menu (FALSE);
	}
}


t_note_page* doc_clear_new (void) 
{
	t_note_page *doc = page_create_new ();
	doc->file_name = get_noname_name ();
	gtk_label_set_label (GTK_LABEL(doc->tab_label), doc->file_name);
	doc_apply_settings (doc);
	gtk_notebook_set_current_page (GTK_NOTEBOOK(notebook1), g_list_length (dox) - 1);
	return doc;
}


t_note_page* doc_open_file (gchar *a_filename)
{
	if (! a_filename)
		return NULL;

	gsize length;
	GError *error;
	gchar *buf;
	gchar *newlabel;
	gsize bytes_read;
	gsize bytes_written;
	gchar *enc;
	gchar *t;
	gchar *filename;

	t_note_page* page = NULL;

	if (! g_file_test (a_filename, G_FILE_TEST_EXISTS))
	{
		icon_stop_logmemo();
		log_to_memo (_("%s does not exist!"), a_filename, LM_ERROR);
		return NULL;       
	}
    
	if (access (a_filename, R_OK) != 0)
	{
		icon_stop_logmemo();
		log_to_memo (_("You do not have permission to %s"), a_filename, LM_ERROR);
		return NULL;
	}


	filename = g_strdup (a_filename);

	if (g_file_test (a_filename, G_FILE_TEST_IS_SYMLINK))
	{
		g_free (filename);
		filename = (gchar *) g_malloc (2048);
		if(readlink (a_filename, filename, 2048)){printf("\n");}

		if (access (filename, R_OK) != 0)
		{
			icon_stop_logmemo();      
			log_to_memo (_("You do not have permission to %s"), filename, LM_ERROR);
			g_free (filename);
			return NULL;
		}
	}

	char *extension;
	if(strrchr(filename,'.'))
	{ 
		extension = strrchr(filename,'.');
	if (strcmp(".jpg", extension) == 0 || strcmp(".png", extension) == 0 || strcmp(".gif", extension) == 0  || strcmp(".jpeg", extension) == 0)
	{
		char type[150];
		strcpy(type,"file://");
		strcat(type,filename);
		gtk_entry_set_text (GTK_ENTRY (entry_web), _(type));
		focus_web ();
		return NULL;
	}
	
	if (strcmp(".htm", extension) == 0 || strcmp(".html", extension) == 0 )
	{
		char type[150];
		strcpy(type,"file://");
		strcat(type,filename);
		gtk_entry_set_text (GTK_ENTRY (entry_web), _(type));
		focus_web ();
	}
	}

	if (is_rtf (filename))
	{
		buf = text_load_rtf (filename);
		page = doc_ins_to_new (buf); 
		g_free (buf);
		g_free (page->file_name);
		page->file_name = get_8_filename (filename);
		page->b_saved = TRUE;

		newlabel = g_path_get_basename (page->file_name);
		gtk_label_set_label (GTK_LABEL(page->tab_label), newlabel);

		doc_apply_settings (page);
		editor_set_pos (page, 0);
		gtk_text_buffer_set_modified (GTK_TEXT_BUFFER(page->text_buffer), FALSE);
		icon_ok_logmemo();
		log_to_memo (_("%s is opened"), page->file_name, LM_NORMAL);

		g_free (filename);
		g_free (newlabel);
		tabs_reload ();

		return page;
	}   

	if (! g_file_get_contents (filename, &buf, &length, &error))
	{
		g_free (filename);
		return NULL;  
	}


	page = doc_clear_new ();

	if (page->encoding)
		g_free (page->encoding);

	if (cur_settings.selected_enc)
		page->encoding = g_strdup (cur_settings.selected_enc);
	else
		page->encoding = g_strdup (confile.default_charset);

	if (g_utf8_collate (page->encoding, CHARSET_JAPANESE) == 0)
	{
		enc = detect_charset_ja (buf);
		page->encoding = ch_str (page->encoding, enc);
	}

	if (g_utf8_collate (page->encoding, CHARSET_AUTODETECT) == 0)
	{
		enc = enc_guess (buf);
		if (enc)
		page->encoding = ch_str (page->encoding, enc);
		else
		{
			g_free (buf);
			page_del_by_index (gtk_notebook_get_current_page ((GtkNotebook *) notebook1));
			msg_wrong_encoding (filename); 
			g_free (filename);
			return NULL; 
		}
	}

	if (g_utf8_collate (page->encoding, "UTF-8") == 0)
	{
		if (! g_utf8_validate (buf, -1, NULL))
		{
			g_free (buf);
			page_del_by_index (gtk_notebook_get_current_page ((GtkNotebook *) notebook1));
			msg_wrong_encoding (filename); 
			g_free (filename);
			return NULL; 
		}

	gtk_text_buffer_set_text (GTK_TEXT_BUFFER(page->text_buffer), buf, length);
	}
	else
		if (g_utf8_collate (page->encoding, CURR_LOCALE) == 0)
		{
			t = g_locale_to_utf8 (buf, length, &bytes_read, &bytes_written, &error);

		if (! t)
		{
			g_free (buf);
			page_del_by_index (gtk_notebook_get_current_page ((GtkNotebook *) notebook1));
			msg_wrong_encoding (filename); 
			g_free (filename);
			return NULL; 
		}

		gtk_text_buffer_set_text (GTK_TEXT_BUFFER(page->text_buffer), t, bytes_written);
		g_free (t);
		}
		else
		{ 
			t = g_convert (buf, length, "UTF-8", page->encoding, &bytes_read, &bytes_written, &error);
			if (! t)
			{
				g_free (buf);
				page_del_by_index (gtk_notebook_get_current_page ((GtkNotebook *) notebook1));
				msg_wrong_encoding (filename); 
				g_free (filename);
				return NULL; 
			}

		gtk_text_buffer_set_text (GTK_TEXT_BUFFER(page->text_buffer), t, bytes_written);
		g_free (t);
		}

	g_free (page->file_name);
	page->file_name = get_8_filename (filename);

	page->b_saved = TRUE;

	newlabel = g_path_get_basename (page->file_name);
	gtk_label_set_label (GTK_LABEL(page->tab_label), newlabel);

	doc_apply_settings (page);
	editor_set_pos (page, 0);

	page->hl_mode = ch_str (page->hl_mode, get_hl_name (page->file_name));
	icon_ok_logmemo();
	log_to_memo (_("%s is opened"), page->file_name, LM_NORMAL);


	if (confile.scan_for_links_on_doc_open == 1)
		scan_links();

	if (confile.do_hl_on_fileopen)
		apply_hl (page);

	gtk_text_buffer_set_modified (GTK_TEXT_BUFFER(page->text_buffer), FALSE);

	tabs_reload ();
	no_onglet_open() ;
	controle_save_page_icon();

	clear_remove_tags (cur_text_doc);

	gtk_window_set_title (GTK_WINDOW (tea_main_window), page->file_name);

	GtkSourceBuffer *tmpbuffer = GTK_SOURCE_BUFFER (gtk_source_buffer_new (NULL));
	GtkWidget *srctmp= gtk_source_view_new_with_buffer(tmpbuffer);

	gchar* file_autocomp=NULL;
	gchar* titre_autocomp="";

	if(strrchr(page->file_name,'.'))
	{
		extension = strrchr(page->file_name,'.');
		if (strcmp(".pl", extension) == 0){file_autocomp="/usr/local/share/griffon/autocomp/perl";titre_autocomp="[PERL] AutoComp";}
		if (strcmp(".sh", extension) == 0){file_autocomp="/usr/local/share/griffon/autocomp/bash";titre_autocomp="[BASH] AutoComp";}
		if (strcmp(".c", extension) == 0 || strcmp(".h", extension) == 0){file_autocomp="/usr/local/share/griffon/autocomp/c";titre_autocomp="[C] AutoComp";}
		if (strcmp(".htm", extension) == 0 || strcmp(".html", extension) == 0){file_autocomp="/usr/local/share/griffon/autocomp/html";titre_autocomp="[HTML] AutoComp";preview_web_popup_full ();}
		if (strcmp(".php", extension) == 0){file_autocomp="/usr/local/share/griffon/autocomp/php";titre_autocomp="[PHP] AutoComp";}
		if (strcmp(".css", extension) == 0){file_autocomp="/usr/local/share/griffon/autocomp/css";titre_autocomp="[CSS] AutoComp";}

		//********************** ICON FILE MIME
		gtk_widget_destroy(page->icon);

		if (strcmp(".c", extension) == 0){page->icon = gtk_image_new_from_file("/usr/local/share/griffon/images/c.png");}
		if (strcmp(".cpp", extension) == 0){page->icon = gtk_image_new_from_file("/usr/local/share/griffon/images/cpp.png");}
		if (strcmp(".css", extension) == 0){page->icon = gtk_image_new_from_file("/usr/local/share/griffon/images/css.png");}
		if (strcmp(".csv", extension) == 0){page->icon = gtk_image_new_from_file("/usr/local/share/griffon/images/csv.png");}
		if (strcmp(".f", extension) == 0){page->icon = gtk_image_new_from_file("/usr/local/share/griffon/images/f.png");}
		if (strcmp(".h", extension) == 0){page->icon = gtk_image_new_from_file("/usr/local/share/griffon/images/h.png");}
		if (strcmp(".html", extension) == 0){page->icon = gtk_image_new_from_file("/usr/local/share/griffon/images/html.png");}
		if (strcmp(".htm", extension) == 0){page->icon = gtk_image_new_from_file("/usr/local/share/griffon/images/html.png");}
		if (strcmp(".j", extension) == 0){page->icon = gtk_image_new_from_file("/usr/local/share/griffon/images/j.png");}
		if (strcmp(".java", extension) == 0){page->icon = gtk_image_new_from_file("/usr/local/share/griffon/images/java.png");}
		if (strcmp(".l", extension) == 0){page->icon = gtk_image_new_from_file("/usr/local/share/griffon/images/l.png");}
		if (strcmp(".o", extension) == 0){page->icon = gtk_image_new_from_file("/usr/local/share/griffon/images/o.png");}
		if (strcmp(".p", extension) == 0){page->icon = gtk_image_new_from_file("/usr/local/share/griffon/images/p.png");}
		if (strcmp(".php", extension) == 0){page->icon = gtk_image_new_from_file("/usr/local/share/griffon/images/php.png");}
		if (strcmp(".pl", extension) == 0){page->icon = gtk_image_new_from_file("/usr/local/share/griffon/images/pl.png");}
		if (strcmp(".py", extension) == 0){page->icon = gtk_image_new_from_file("/usr/local/share/griffon/images/py.png");}
		if (strcmp(".s", extension) == 0){page->icon = gtk_image_new_from_file("/usr/local/share/griffon/images/s.png");}
		if (strcmp(".sh", extension) == 0){page->icon = gtk_image_new_from_file("/usr/local/share/griffon/images/sh.png");}
		if (strcmp(".txt", extension) == 0){page->icon = gtk_image_new_from_file("/usr/local/share/griffon/images/txt.png");}
		if (strcmp(".y", extension) == 0){page->icon = gtk_image_new_from_file("/usr/local/share/griffon/images/y.png");}
		if (strcmp(".js", extension) == 0){page->icon = gtk_image_new_from_file("/usr/local/share/griffon/images/sh.png");}

	
		gtk_box_pack_start (GTK_BOX (page->hbox), page->icon, FALSE, FALSE, 0);
		gtk_box_reorder_child((GtkBox *)page->hbox, page->icon,0);
		gtk_widget_show (GTK_WIDGET(page->icon));

	}

	if(file_autocomp!=NULL)
	{
		gchar lecture[2024];
		FILE *fichier;
		if(fopen(file_autocomp,"rt"))
		{
			fichier = fopen(file_autocomp,"rt");
			while(fgets(lecture, 2024, fichier))
			{
			gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(tmpbuffer),g_locale_to_utf8(lecture, -1, NULL, NULL, NULL) , -1);
			}
			fclose(fichier);

		word_provider3 = gtk_source_completion_words_new (titre_autocomp, NULL);

		gtk_source_completion_words_register (word_provider3,
	                                      gtk_text_view_get_buffer (GTK_TEXT_VIEW (srctmp)));

		gtk_source_completion_add_provider (completion,
	                                    GTK_SOURCE_COMPLETION_PROVIDER (word_provider3),
	                                    NULL);

		g_object_set (word_provider3, "priority", 10, NULL);
		}
	}

	gtk_box_pack_start (GTK_BOX (page->hbox), srctmp, FALSE, TRUE, 0);
	g_free (filename);
	g_free (newlabel);
	g_free (buf);
	newlabel = g_path_get_basename (page->file_name);
	griffon_notify(_(g_strconcat ("File open : ",newlabel,NULL)));

		//*********** Pour le diff
		GtkTextIter start;
		GtkTextIter end;
		char *text;
		gtk_text_buffer_get_start_iter ((GtkTextBuffer *)page->text_buffer, &start);
		gtk_text_buffer_get_end_iter ((GtkTextBuffer *)page->text_buffer, &end);
		text = gtk_text_buffer_get_text ((GtkTextBuffer *)page->text_buffer, &start, &end, FALSE);       
		g_file_set_contents (confile.file_tmp, text, -1, NULL);
		g_free (text);

	//scan_include();
	scan_var_include_all();

	//************ ADD IN HISTORY DEFAULT GTK
  gchar *uri = g_strconcat("file://", page->file_name, NULL);
	gtk_recent_manager_add_item (manager, uri);
	g_free(uri);

		gtk_source_map_set_view(GTK_SOURCE_MAP(source_map1),GTK_SOURCE_VIEW(page->text_view));

	return page;
}


void doc_apply_settings (t_note_page *doc)
{
	if (! doc) return;

	PangoFontDescription *font_desc = pango_font_description_from_string (confile.editor_font);
	//gtk_widget_override_font (GTK_WIDGET(doc->text_view), font_desc);
	pango_font_description_free (font_desc);

	g_object_set (G_OBJECT (tag_comment), "foreground", confile.tag_comment, "font", confile.tag_comment_font, NULL);
	g_object_set (G_OBJECT (tag_identifier), "foreground", confile.tag_identifier, "font", confile.tag_identifier_font, NULL);
	g_object_set (G_OBJECT (tag_digit), "foreground", confile.tag_digit, "font", confile.tag_digit_font, NULL);
	g_object_set (G_OBJECT (tag_string), "foreground", confile.tag_string, "font", confile.tag_string_font, NULL);
	g_object_set (G_OBJECT (tag_html_tag), "foreground", confile.tag_html_tag, "font", confile.tag_html_tag_font, NULL);
	g_object_set (G_OBJECT (tag_preprocessor), "foreground", confile.tag_preprocessor, "font", confile.tag_preprocessor_font, NULL);
	g_object_set (G_OBJECT (tag_type), "foreground", confile.tag_type, "font", confile.tag_type_font, NULL);

	GdkColor background;
	GdkColor text;
	GdkColor selection;
	GdkColor sel_text;

/*	gdk_color_parse (confile.text_foreground, &text);
	gdk_color_parse (confile.text_background, &background);
	gdk_color_parse (confile.text_sel_background, &selection);
	gdk_color_parse (confile.text_sel_foreground, &sel_text);
*/
	widget_set_colors (GTK_WIDGET(doc->text_view), &background, &text, &selection, &sel_text);  
	doc_set_tabsize (doc, confile.tab_size);
}


void widget_apply_colors (GtkWidget *w)
{
	if (! w) return;

	GdkColor background;
	GdkColor text;
	GdkColor selection;
	GdkColor sel_text;

/*	gdk_color_parse (confile.text_foreground, &text);
	gdk_color_parse (confile.text_background, &background);
	gdk_color_parse (confile.text_sel_background, &selection);
	gdk_color_parse (confile.text_sel_foreground, &sel_text);
*/  
	widget_set_colors (GTK_WIDGET(w), &background, &text, &selection, &sel_text);  
}


void page_free (t_note_page *page)
{
	gchar *f = get_l_filename (page->file_name);

	if (strcmp (f, confile.crapbook_file) != 0)
		if (page->b_saved && confile.prompt_on_not_saved && gtk_text_buffer_get_modified (GTK_TEXT_BUFFER(page->text_buffer))) 
			if (dlg_question (tea_main_window, _("%s is modified but not saved, save?"), page->file_name))
				text_doc_save (page, page->file_name);

	if (strcmp (f, confile.crapbook_file) == 0)
		doc_save_buffer_to_file (GTK_TEXT_BUFFER(page->text_buffer), confile.crapbook_file); 
	else 
		if (g_file_test (f, G_FILE_TEST_EXISTS))
			add_recent_internal (page);

	g_free (f); 

	g_free (page->last_searched_text);
	g_free (page->encoding);
	g_free (page->hl_mode);
	g_free (page->file_name);
	g_free (page->current_path);

	g_free (page);

	tabs_reload ();
}


void doc_insert_at_cursor (t_note_page* doc, gchar *text)
{
	if (! text)
		return;
	gtk_text_buffer_begin_user_action (GTK_TEXT_BUFFER(doc->text_buffer));
	gtk_text_buffer_insert_at_cursor (GTK_TEXT_BUFFER(doc->text_buffer), text, -1);
	gtk_text_buffer_end_user_action (GTK_TEXT_BUFFER(doc->text_buffer));
}


void doc_update_cb (gpointer data)
{
	if (data)
		doc_apply_settings ((t_note_page*) data);
}


void doc_update_all (void)
{
	set_lm_colors ();
	if (confile.do_show_main_toolbar)
		gtk_widget_show (GTK_WIDGET(tb_main_toolbar));
	//else
		//gtk_widget_hide (tb_main_toolbar);
}


gint editor_get_pos (t_note_page* doc)
{
	if (! doc->text_buffer && ! doc->text_view)
		return 0;

	GtkTextMark *mark;
	GtkTextIter it;
	mark = gtk_text_buffer_get_mark (GTK_TEXT_BUFFER(doc->text_buffer), "insert");
	gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER(doc->text_buffer), &it, mark);
	return gtk_text_iter_get_offset (&it);
}


void tv_logmemo_set_pos (guint pos )
{
	GtkTextIter it;
	gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(log_memo_textbuffer), &it, pos);
	gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER(log_memo_textbuffer), &it);
	gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW(tv_logmemo), &it, 0.0, FALSE, 0.0, 0.0);
}


gboolean doc_reload_text (t_note_page *doc, gchar *filename, gchar *enc)
{
	gsize *length=NULL;
	GError *error;
	gchar *buf;
	gchar *t;
	gsize bytes_read;
	gsize bytes_written;

	if (! g_file_get_contents (filename, &buf, length, &error))
		return FALSE;
	else
		if (g_utf8_collate (enc, CURR_LOCALE) == 0)
		{
			gchar *t = g_locale_to_utf8 (buf, (gssize )length, &bytes_read, &bytes_written, NULL);

		if (! t)
		{
			g_free (buf);
			return FALSE;
		}

		gtk_text_buffer_set_text (GTK_TEXT_BUFFER(doc->text_buffer), t, bytes_written);
		g_free (buf);
		g_free (t);

		return TRUE;
		}

	else
		if (g_utf8_collate (enc, "UTF-8") == 0)
		{
			if (! g_utf8_validate (buf, -1, NULL))
			{
				g_free (buf);
				return FALSE;
			}

		gtk_text_buffer_set_text (GTK_TEXT_BUFFER(doc->text_buffer), buf, *length);

		g_free (buf);
		return TRUE;
		}
		else
           {//not UTF-8
		t = g_convert (buf, (gssize)length, "UTF-8", enc, &bytes_read, &bytes_written, &error);
		if (! t)
		{
			g_free (buf);
			return FALSE;
		}

		gtk_text_buffer_set_text (GTK_TEXT_BUFFER(doc->text_buffer), t, bytes_written);

		g_free (buf);
		g_free (t);

		return TRUE;
	}
}


gint get_n_page_by_filename (gchar const *filename)
{
  gint i = 0;
  t_note_page *t;

  GList *tl = g_list_first (dox);

  while (tl)
        {
         t = (t_note_page*) tl->data;
         if (strcmp (filename, t->file_name) == 0)
            return i;
         i++;
       
         tl = g_list_next (tl);
        }

  return -1;
}


void log_to_memo (gchar *m1, gchar* m2, gint mode)
{
	if (! log_memo_textbuffer)
		return;

	gchar *prefix = g_strdup_printf  (" ");
	gchar *st;

	gchar *buf = get_time (confile.date_time);

	if (! m2)
	{
		st = g_strconcat (prefix, m1, "\n________________________________________________________\n", NULL);

		GtkTextIter it;
		gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(log_memo_textbuffer), &it, 0);
		gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER(log_memo_textbuffer), &it);

		if (mode == LM_NORMAL)
			gtk_text_buffer_insert_with_tags_by_name (GTK_TEXT_BUFFER(log_memo_textbuffer), &it, st, -1, "lm_normal", NULL);
		else
			if (mode == LM_ERROR)
				{gtk_text_buffer_insert_with_tags_by_name (GTK_TEXT_BUFFER(log_memo_textbuffer), &it, st, -1, "lm_error", NULL);
				quick_message ("ERROR",st);}
		else
			if (mode == LM_ADVICE)
				gtk_text_buffer_insert_with_tags_by_name (GTK_TEXT_BUFFER(log_memo_textbuffer), &it, st, -1, "lm_advice", NULL);
		else
			if (mode == LM_GREET)
				gtk_text_buffer_insert_with_tags_by_name (GTK_TEXT_BUFFER(log_memo_textbuffer), &it, st, -1, "lm_greet", NULL);

				g_free (st);
				g_free (prefix);
				g_free (buf);
				tv_logmemo_set_pos (0);
				return;
	}
 
	gtk_text_buffer_insert_at_cursor (GTK_TEXT_BUFFER(log_memo_textbuffer), "\t", -1);
	gtk_text_buffer_insert_at_cursor (GTK_TEXT_BUFFER(log_memo_textbuffer), buf, -1);
	gtk_text_buffer_insert_at_cursor (GTK_TEXT_BUFFER(log_memo_textbuffer), "\t", -1);

	gulong len = strlen (m1) + strlen (m2) + 32024;
	gchar *message = g_malloc (len);

	g_snprintf (message, len, m1, m2);
	gchar *s = g_strconcat (prefix, message, "\n________________________________________________________\n", NULL);

	gtk_text_buffer_insert_at_cursor (GTK_TEXT_BUFFER(log_memo_textbuffer), s, -1);

	g_free (message);
	g_free (s);
	g_free (prefix);
	tv_logmemo_set_pos (0);
  
	return;
}


gboolean find_space (gunichar ch)
{
  if (g_unichar_isspace (ch) || (ch == '\0') || (ch == ')') || (ch == '\'') || (ch == '=')  || (ch == ',') || (ch == '(') || (ch == '\"') || (ch == '`')  || (ch == '{')  || (ch == '[')  || (ch == '<'  || (ch == '>')  || (ch == '?') || (ch == '-')  || (ch == ':')  || (ch == '}')  || (ch == ']') || (ch == '!') || (ch == '+') || (ch == ';') )) 
     return TRUE;
  else
      return FALSE; 
}




gboolean find_quote (gunichar ch)
{
	if ((ch == '"') || (ch == '\'')) 
		return TRUE;
	else
		return FALSE; 
}


gchar* doc_get_current_word (t_note_page *doc, GtkTextIter *itstart, GtkTextIter *itend) 
{
	GtkTextIter ittemp;
	GtkTextMark *mark = gtk_text_buffer_get_insert (GTK_TEXT_BUFFER(doc->text_buffer));
	gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER(doc->text_buffer), &ittemp, mark);

	*itstart = ittemp;
	*itend = ittemp;

	if (! gtk_text_iter_forward_find_char (itend, (GtkTextCharPredicate) find_space, NULL, NULL))
		gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(doc->text_buffer), itend);

	if (! gtk_text_iter_backward_find_char (itstart, (GtkTextCharPredicate)find_space, NULL, NULL))
		gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(doc->text_buffer), itstart); 
	else     
		gtk_text_iter_forward_char (itstart);  
		return (gtk_text_buffer_get_text (GTK_TEXT_BUFFER(doc->text_buffer), itstart, itend, FALSE));
}


void doc_set_new_text (t_note_page *doc, GtkTextIter *itstart, GtkTextIter *itend, gchar *text) 
{
	gtk_text_buffer_begin_user_action (GTK_TEXT_BUFFER(doc->text_buffer));

	gtk_text_buffer_delete (GTK_TEXT_BUFFER(doc->text_buffer), itstart, itend);   
	gtk_text_buffer_insert (GTK_TEXT_BUFFER(doc->text_buffer), itstart, text, -1);

	gtk_text_buffer_end_user_action (GTK_TEXT_BUFFER(doc->text_buffer));
}


void doc_rep_sel (t_note_page *page, const gchar *newstring)
{
	if (! newstring)
		return; 

	GtkTextIter start;
	GtkTextIter end;
	if (gtk_text_buffer_get_selection_bounds (GTK_TEXT_BUFFER(page->text_buffer), &start, &end))
		doc_set_new_text (page, &start, &end,(gchar *) newstring);  
}


gchar* get_c_url (t_note_page *doc) 
{
	GtkTextIter ittemp;
	GtkTextIter itstart;
	GtkTextIter itend;
	gchar *filename;

	GtkTextMark *mark = gtk_text_buffer_get_insert (GTK_TEXT_BUFFER(doc->text_buffer));
	gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER(doc->text_buffer), &ittemp, mark);

	itstart = ittemp;
	itend = ittemp;

	if (! gtk_text_iter_forward_find_char (&itend, (GtkTextCharPredicate)find_quote, NULL, NULL))
		gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(doc->text_buffer), &itend);

	if (! gtk_text_iter_backward_find_char (&itstart, (GtkTextCharPredicate)find_quote, NULL, NULL))
		gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(doc->text_buffer), &itstart); 
		else     
			gtk_text_iter_forward_char (&itstart);  

	gchar *result = gtk_text_buffer_get_text (GTK_TEXT_BUFFER(doc->text_buffer), &itstart, &itend, FALSE);

	if (g_utf8_get_char (result) == '#')
	{
		goto_local_label (result);
		g_free (result);
		return NULL;
	}

	gchar *dir = g_path_get_dirname (doc->file_name);

	if (g_utf8_get_char (result) == G_DIR_SEPARATOR)  
		filename = create_full_path (g_utf8_find_next_char (result, NULL), dir);
	else
		filename = create_full_path (result, dir);

	g_free (dir);
	g_free (result);
	gchar *result2 = strdup (filename);
	g_free (filename);

	return result2;
}


gboolean doc_search_f (t_note_page *doc, const gchar *text) 
{
	if (! text)
		return FALSE; 

	GtkTextIter ittemp;
	GtkTextIter end;
	GtkTextIter match_start;
	GtkTextIter match_end;
	gboolean result = FALSE;

	gtk_text_buffer_get_bounds (GTK_TEXT_BUFFER(doc->text_buffer), &ittemp, &end);

	if (gtk_text_iter_forward_search (&ittemp, text, GTK_TEXT_SEARCH_TEXT_ONLY, &match_start, &match_end, &end))
	{
		gtk_text_buffer_move_mark_by_name (GTK_TEXT_BUFFER(doc->text_buffer), "insert", &match_end);
		gtk_text_buffer_move_mark_by_name (GTK_TEXT_BUFFER(doc->text_buffer), "selection_bound", &match_start);
		gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW(doc->text_view), &match_start, 0.0, TRUE, 0.0, 0.0 );

		GtkTextIter itstart, itend;
		gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(doc->text_buffer), &itstart, 0);
		gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(doc->text_buffer), &itend, gtk_text_buffer_get_char_count (GTK_TEXT_BUFFER(doc->text_buffer)));

		g_free (doc->last_searched_text);
		doc->last_searched_text = g_strdup (text); 
		result = TRUE;
	}


	GtkTextIter start_find, end_find;
	GtkTextIter start_match, end_match;

	gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(doc->text_buffer), &start_find);
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(doc->text_buffer), &end_find);

	gtk_text_buffer_remove_tag_by_name(GTK_TEXT_BUFFER(doc->text_buffer), "search", &start_find, &end_find);

	if (! text){return FALSE;}else{
		while ( gtk_text_iter_forward_search(&start_find, text, GTK_TEXT_SEARCH_TEXT_ONLY | GTK_TEXT_SEARCH_VISIBLE_ONLY, &start_match, &end_match, NULL) )
		{
			gtk_text_buffer_apply_tag_by_name(GTK_TEXT_BUFFER(doc->text_buffer), "search", 
			&start_match, &end_match);
			int offset = gtk_text_iter_get_offset(&end_match);
			gtk_text_buffer_get_iter_at_offset(GTK_TEXT_BUFFER(doc->text_buffer), 
			&start_find, offset);
		}
	}

	gtk_widget_grab_focus (GTK_WIDGET(doc->text_view));

	return result;
}


gboolean doc_search_f_next (t_note_page *doc) 
{
	gboolean result = FALSE;
	if (! doc->last_searched_text)
		return FALSE;

	GtkTextIter ittemp;
	GtkTextIter end;
	GtkTextIter match_start;
	GtkTextIter match_end;
	GtkTextMark *mark = gtk_text_buffer_get_insert (GTK_TEXT_BUFFER(doc->text_buffer));
	gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER(doc->text_buffer), &ittemp, mark);

	gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(doc->text_buffer), &end); 

	if (gtk_text_iter_forward_search (&ittemp, doc->last_searched_text, GTK_TEXT_SEARCH_TEXT_ONLY, &match_start, &match_end, &end))
	{
		gtk_text_buffer_move_mark_by_name (GTK_TEXT_BUFFER(doc->text_buffer), "insert", &match_end);
		gtk_text_buffer_move_mark_by_name (GTK_TEXT_BUFFER(doc->text_buffer), "selection_bound", &match_start);
		gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW(doc->text_view), &match_start, 0.0, TRUE, 0.0, 0.0 );

		result = TRUE;
	}

	return result;
}


void doc_replace_all (t_note_page *doc, const gchar *text, const gchar *new_text) 
{
	if (! text)
		return;

	if (! new_text)
		return;

	if (doc_search_f (doc, text))
	{
		doc_rep_sel (doc, new_text);
			while (doc_search_f_next (doc))
				doc_rep_sel (doc, new_text);
	}  
}


gint find_index_by_page (t_note_page *page)
{
	gint i = -1;

	GList *p = g_list_first (dox);

	while (p)
	{
		i++;  
		if (page == p->data){return i;}
		p = g_list_next (p);  
	}

	return i;
}


gchar* get_c_url_pure (t_note_page *doc) 
{
	GtkTextIter ittemp;
	GtkTextIter itstart;
	GtkTextIter itend;

	GtkTextMark *mark = gtk_text_buffer_get_insert (GTK_TEXT_BUFFER(doc->text_buffer));
	gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER(doc->text_buffer), &ittemp, mark);

	itstart = ittemp;
	itend = ittemp;

	if (! gtk_text_iter_forward_find_char (&itend, (GtkTextCharPredicate)find_quote, NULL, NULL))
		gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(doc->text_buffer), &itend);

	if (! gtk_text_iter_backward_find_char (&itstart, (GtkTextCharPredicate)find_quote, NULL, NULL))
		gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(doc->text_buffer), &itstart); 
	else     
		gtk_text_iter_forward_char (&itstart);  

	gchar *result = gtk_text_buffer_get_text (GTK_TEXT_BUFFER(doc->text_buffer), &itstart, &itend, FALSE);

	return result;
}


t_note_page* doc_ins_to_new (const gchar *newstring)
{
	if (! newstring) 
		return NULL;

	t_note_page *page = doc_clear_new ();
	gtk_window_set_title (GTK_WINDOW (tea_main_window), page->file_name);
	gtk_widget_grab_focus (GTK_WIDGET(page->text_view));
	doc_insert_at_cursor (page, (gchar *)newstring);
	tabs_reload ();
	return page;
}


t_note_page* get_page_text (void)
{
	cur_text_doc = NULL;

	gint i = gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook1));
	if (i == -1)
		return NULL;
	else
		cur_text_doc = (t_note_page *) g_list_nth_data (dox, i);

	return cur_text_doc;
}


void set_title (t_note_page *t)
{
	if (! t)
		return;

	if (! t->file_name)
		return;      
 //strlen (t->file_name)
	if (! g_utf8_validate (t->file_name, -1, NULL))
		return;

	gchar *nc = NULL;

	if (confile.main_wnd_show_full_path)
	{
		nc = g_strdup (t->file_name);
	}
	else
	{ 
		nc = g_path_get_basename (t->file_name);
	}

	gtk_window_set_title (GTK_WINDOW (tea_main_window), nc);
	statusbar_msg (t->encoding);

	g_free (nc);

	gchar **a = g_strsplit (t->file_name, "_", -1);
	if (strcmp("noname", a[0]) == 0){ return;}else{gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooserwidget2) ,g_path_get_dirname (t->file_name));	}

}
  

t_note_page* open_file_std (gchar const *f)
{
	cur_text_doc = doc_open_file ((gchar *)f);
	tabs_reload ();
	set_title (cur_text_doc);
	return cur_text_doc;
}


void doc_move_to_pos_bw (t_note_page* doc, gint pos)
{
	if (! doc->text_buffer && ! doc->text_view)
		return;

	GtkTextMark *mark;
	GtkTextIter it;
	mark = gtk_text_buffer_get_mark (GTK_TEXT_BUFFER(doc->text_buffer), "insert");
	gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER(doc->text_buffer), &it, mark);

	if (gtk_text_iter_backward_chars (&it, pos))
		gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER(doc->text_buffer), &it);
}

void doc_move_to_pos_bw_quote (t_note_page* doc)
{
	if (! doc->text_buffer && ! doc->text_view)
		return;

	GtkTextMark *mark;
	GtkTextIter it;
	mark = gtk_text_buffer_get_mark (GTK_TEXT_BUFFER(doc->text_buffer), "insert");
	gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER(doc->text_buffer), &it, mark);

	if (gtk_text_iter_backward_find_char (&it, (GtkTextCharPredicate)find_quote, NULL, NULL))
		gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER(doc->text_buffer), &it);
}


void editor_set_pos (t_note_page *doc, guint pos)
{
	GtkTextIter it;
	GtkTextMark* m; 

	gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(doc->text_buffer), &it, pos);
	gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER(doc->text_buffer), &it);
	m = gtk_text_buffer_get_mark (GTK_TEXT_BUFFER(doc->text_buffer), "insert");
	if (m)
		gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW(doc->text_view), m, 0.0, TRUE, 0.0, 0.0);
}
 

void editor_goto_selection (t_note_page *doc)
{
	GtkTextIter start;
	GtkTextIter end;

	if (gtk_text_buffer_get_selection_bounds (GTK_TEXT_BUFFER(doc->text_buffer), &start, &end))
		gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW(doc->text_view), &start, 0.0, TRUE, 0.0, 0.0); 
}


gboolean doc_search_f_ncase (t_note_page *doc, const gchar *text) 
{
	if (! text)
		return FALSE;

	gboolean result = FALSE;

	gchar *f = g_utf8_strdown (text, -1);

	GtkTextIter start;
	GtkTextIter end;
	GtkTextIter ittemp;
	GtkTextIter b_end;

	GtkTextMark *mark = gtk_text_buffer_get_insert (GTK_TEXT_BUFFER(doc->text_buffer));
	gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER(doc->text_buffer), &ittemp, mark);

	gtk_text_iter_forward_chars (&ittemp, g_utf8_strlen (f, -1));

	gint int_char_offset = gtk_text_iter_get_offset (&ittemp);

	gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(doc->text_buffer), &b_end);

	gchar *t = gtk_text_buffer_get_slice (GTK_TEXT_BUFFER(doc->text_buffer), &ittemp, &b_end, FALSE);
	gchar *x = g_utf8_strdown (t, -1);

	gchar *found = strstr (x, f);
 
	if (found)
	{
		glong i = g_utf8_pointer_to_offset (x, found) + int_char_offset;
		gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(doc->text_buffer), &start, i);
		gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(doc->text_buffer), &end,
                                          i + g_utf8_strlen (f, -1));

		gtk_text_buffer_move_mark_by_name (GTK_TEXT_BUFFER(doc->text_buffer), "insert", &start);
		gtk_text_buffer_move_mark_by_name (GTK_TEXT_BUFFER(doc->text_buffer), "selection_bound", &end);
		g_free (doc->last_searched_text);
		doc->last_searched_text = g_strdup (text); 
		result = TRUE;
	}     

	g_free (t);
	g_free (x);
	g_free (f);

	return result;
}


gboolean doc_search_f_next_ncase (t_note_page *doc) 
{
	if (! doc->last_searched_text)
		return FALSE;

	GtkTextIter start;
	GtkTextIter end;

	gchar *f = g_utf8_strdown (doc->last_searched_text, -1);

	gboolean result = FALSE;
	GtkTextIter ittemp;
	GtkTextIter b_end;
	GtkTextMark *mark = gtk_text_buffer_get_insert (GTK_TEXT_BUFFER(doc->text_buffer));
	gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER(doc->text_buffer), &ittemp, mark);

	gtk_text_iter_forward_chars (&ittemp, g_utf8_strlen (f, -1));
	gint int_char_offset = gtk_text_iter_get_offset (&ittemp);
	gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER(doc->text_buffer), &b_end);

	gchar *t = gtk_text_buffer_get_slice (GTK_TEXT_BUFFER(doc->text_buffer), &ittemp, &b_end, FALSE);
	gchar *x = g_utf8_strdown (t, -1);
	gchar *found = strstr (x, f);

	if (found)
	{
		glong i = g_utf8_pointer_to_offset (x, found) + int_char_offset;

		gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(doc->text_buffer), &start, i);
		gtk_text_buffer_get_iter_at_offset (GTK_TEXT_BUFFER(doc->text_buffer), &end, i + g_utf8_strlen (f, -1));
		gtk_text_buffer_move_mark_by_name (GTK_TEXT_BUFFER(doc->text_buffer), "insert", &start);
		gtk_text_buffer_move_mark_by_name (GTK_TEXT_BUFFER(doc->text_buffer), "selection_bound", &end);

		result = TRUE;
	}     

	g_free (t);
	g_free (x);
	g_free (f);
	return result;
}


gchar* upcase_each_first_letter_ofw (t_note_page *doc)
{
	gunichar u;
	GtkTextIter start;
	GtkTextIter end;

	if (! gtk_text_buffer_get_selection_bounds (GTK_TEXT_BUFFER(doc->text_buffer), &start, &end))
		return NULL; 

	GString* gs = g_string_sized_new (1024);

	do
	{
		if (gtk_text_iter_equal (&start, &end))
			break;

		u = gtk_text_iter_get_char (&start);

		if ( gtk_text_iter_starts_word (&start))        
			u = g_unichar_toupper (u); 

			gs = g_string_append_unichar (gs, u);        
	}  
		while ( gtk_text_iter_forward_char (&start) );

	gchar *s = gs->str;
	g_string_free (gs, FALSE);

	return s;
}


gchar* doc_get_word_at_left (t_note_page *doc, GtkTextIter *itstart, GtkTextIter *itend) 
{
	GtkTextIter ittemp;
	GtkTextMark *mark = gtk_text_buffer_get_insert (GTK_TEXT_BUFFER(doc->text_buffer));
	gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER(doc->text_buffer), &ittemp, mark);

	*itstart = ittemp;
	*itend = ittemp;

	if (! gtk_text_iter_backward_find_char (itstart,(GtkTextCharPredicate) find_space, NULL, NULL))
		gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(doc->text_buffer), itstart); 
	else     
		gtk_text_iter_forward_char (itstart);  

	return (gtk_text_buffer_get_text (GTK_TEXT_BUFFER(doc->text_buffer), itstart, itend, FALSE));
}

void doc_move_cursor_forw (t_note_page *doc, gint i) 
{
	GtkTextIter ittemp;
	GtkTextMark *mark = gtk_text_buffer_get_insert (GTK_TEXT_BUFFER(doc->text_buffer));
	gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER(doc->text_buffer), &ittemp, mark);

	if (gtk_text_iter_forward_chars (&ittemp, i))
	{
		gtk_text_buffer_move_mark_by_name (GTK_TEXT_BUFFER(doc->text_buffer), "insert", &ittemp);
		gtk_text_buffer_move_mark_by_name (GTK_TEXT_BUFFER(doc->text_buffer), "selection_bound", &ittemp);
	} 
}


void doc_move_cursor_backw (t_note_page *doc, gint i) 
{
	GtkTextIter ittemp;
	GtkTextMark *mark = gtk_text_buffer_get_insert (GTK_TEXT_BUFFER(doc->text_buffer));
	gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER(doc->text_buffer), &ittemp, mark);

	if (gtk_text_iter_backward_chars (&ittemp, i))
	{
		gtk_text_buffer_move_mark_by_name (GTK_TEXT_BUFFER(doc->text_buffer), "insert", &ittemp);
		gtk_text_buffer_move_mark_by_name (GTK_TEXT_BUFFER(doc->text_buffer), "selection_bound", &ittemp);
	}
}


gboolean predicate_find_tag_end (gunichar ch)
{
	if (ch == '>')
		return TRUE;
	else 
		return FALSE;
}


void doc_move_cursor_backw_middle_tags (t_note_page *doc) 
{
	GtkTextIter ittemp;
	GtkTextMark *mark = gtk_text_buffer_get_insert (GTK_TEXT_BUFFER(doc->text_buffer));
	gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER(doc->text_buffer), &ittemp, mark);

	if (gtk_text_iter_backward_find_char (&ittemp, (GtkTextCharPredicate)predicate_find_tag_end, NULL, NULL))
	if (gtk_text_iter_backward_find_char (&ittemp, (GtkTextCharPredicate)predicate_find_tag_end, NULL, NULL))
	if (gtk_text_iter_forward_char (&ittemp))
	{
		gtk_text_buffer_move_mark_by_name (GTK_TEXT_BUFFER(doc->text_buffer), "insert", &ittemp);
		gtk_text_buffer_move_mark_by_name (GTK_TEXT_BUFFER(doc->text_buffer), "selection_bound", &ittemp);
	}
}


void test_save_onglet ()
{

	if (! get_page_text()) return;

	gchar *f = get_l_filename (cur_text_doc->file_name);

	if (strcmp (f, confile.crapbook_file) != 0)
	{
		if (cur_text_doc->b_saved && confile.prompt_on_not_saved && gtk_text_buffer_get_modified (GTK_TEXT_BUFFER(cur_text_doc->text_buffer))) 
		{icon_affiche_nosave ();controle_save_page_icon_no();}
		else{icon_affiche_save ();}
	}   

}

void doc_header_source_switch ()
{
	gchar *f = get_c_header_fname (cur_text_doc->file_name);
	if (f)
	{
		handle_file (f, 0);
		g_free (f);
		return;
	}

	f = get_c_source_fname (cur_text_doc->file_name);

	if (f)
	{
		handle_file (f, 0);
		g_free (f);
		return;
	}
}


