#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <string.h>


#define NUM_TIPS (sizeof (tips) / sizeof (tips[0]))

gchar *get_tip()
{
  //gint i;
  //gboolean found;
	//gchar *totale_tips="";
	//gchar *separator="\n";	
	
  //found = FALSE;
 /* for (i = 0; i < NUM_TIPS; i++)
    {
      if (strcmp (text, tips[i][0]) == 0)
        {
          found = TRUE;
   
          totale_tips = g_strdup_printf("%s %s", totale_tips, tips[i][1]);
         totale_tips = g_strdup_printf("%s %s", totale_tips, separator);          
        }
    }
  if (!found)
    return NULL;
*/
//  return g_strdup (tips[i][1]);
  //return g_strdup (totale_tips);
	return "1";
}

GtkWidget *
tip_window_new (gchar *tip)
{
  GtkWidget *win;
  GtkWidget *label;
  GtkWidget *eb;
  PangoFontDescription *pfd;

  win = gtk_window_new (GTK_WINDOW_POPUP);
  gtk_container_set_border_width (GTK_CONTAINER (win), 0);

  eb = gtk_event_box_new ();
  gtk_container_set_border_width (GTK_CONTAINER (eb), 1);
  gtk_container_add (GTK_CONTAINER (win), eb);

  label = gtk_label_new (tip);  
  gtk_container_add (GTK_CONTAINER (eb), label);

  pfd = pango_font_description_from_string ("courier");
  gtk_widget_modify_font (label, pfd);
  
/*  cmap = gtk_widget_get_colormap (win);
  color.red = 0;
  color.green = 0;
  color.blue = 0;
  if (gdk_colormap_alloc_color (cmap, &color, FALSE, TRUE))
    gtk_widget_modify_bg (win, GTK_STATE_NORMAL, &color);
  else
    g_warning ("Color allocation failed!\n");

  cmap = gtk_widget_get_colormap (eb);
  color.red = 65535;
  color.green = 65535;
  color.blue = 45535;
  if (gdk_colormap_alloc_color (cmap, &color, FALSE, TRUE))
    gtk_widget_modify_bg (eb, GTK_STATE_NORMAL, &color);
  else
    g_warning ("Color allocation failed!\n");

*/

  return win;
}


GtkWidget *
tip_window_new_autocomp (gchar *tip)
{
  GtkWidget *win;
  GtkWidget *label;
  GtkWidget *eb;
  PangoFontDescription *pfd;

  win = gtk_window_new (GTK_WINDOW_POPUP);
  gtk_container_set_border_width (GTK_CONTAINER (win), 0);

  eb = gtk_event_box_new ();
  gtk_container_set_border_width (GTK_CONTAINER (eb), 1);
  gtk_container_add (GTK_CONTAINER (win), eb);

  label = gtk_label_new (tip);  
  gtk_container_add (GTK_CONTAINER (eb), label);

  pfd = pango_font_description_from_string ("courier");
  gtk_widget_modify_font (label, pfd);
  
/*  cmap = gtk_widget_get_colormap (win);
  color.red = 0;
  color.green = 0;
  color.blue = 0;
  if (gdk_colormap_alloc_color (cmap, &color, FALSE, TRUE))
    gtk_widget_modify_bg (win, GTK_STATE_NORMAL, &color);
  else
    g_warning ("Color allocation failed!\n");

  cmap = gtk_widget_get_colormap (eb);
  color.red = 55535;
  color.green = 65535;
  color.blue = 65535;
  if (gdk_colormap_alloc_color (cmap, &color, FALSE, TRUE))
    gtk_widget_modify_bg (eb, GTK_STATE_NORMAL, &color);
  else
    g_warning ("Color allocation failed!\n");
*/


  return win;
}

void
insert_open_brace(GtkWidget **tip_win, GtkWidget *text_view, GtkTextIter *arg1)
{
  GdkWindow *win;
  GtkTextIter start;
  GdkRectangle buf_loc;
  gint x, y;
  gint win_x, win_y;
  gchar *text;
  gchar *tip_text;
  
  /* Get the word at cursor. */
  start = *arg1;
  if (!gtk_text_iter_backward_word_start (&start))
    return;
  text = gtk_text_iter_get_text (&start, arg1);
  g_strstrip (text);

  /* Get the corresponding tooltip. */
  tip_text = get_tip(text);  
  if (tip_text == NULL)
    return;
  
  /* Calculate the tool tip window location. */
  gtk_text_view_get_iter_location (GTK_TEXT_VIEW (text_view), arg1, 
                                   &buf_loc);
  g_printf ("Buffer: %d, %d\n", buf_loc.x, buf_loc.y);
  gtk_text_view_buffer_to_window_coords (GTK_TEXT_VIEW (text_view),
                                         GTK_TEXT_WINDOW_WIDGET,
                                         buf_loc.x, buf_loc.y,
                                         &win_x, &win_y);
  g_printf ("Window: %d, %d\n", win_x, win_y);
  win = gtk_text_view_get_window (GTK_TEXT_VIEW (text_view), 
                                  GTK_TEXT_WINDOW_WIDGET);
  gdk_window_get_origin (win, &x, &y);

  /* Destroy any previous tool tip window. */
  if (*tip_win != NULL)
    gtk_widget_destroy (GTK_WIDGET (*tip_win));  
  
  /* Create a new tool tip window and place it at the caculated
     location. */
  *tip_win = tip_window_new (tip_text);
  g_free(tip_text);
  gtk_window_move (GTK_WINDOW (*tip_win), win_x + x, 
                   win_y + y + buf_loc.height);
  gtk_widget_show_all (*tip_win);
}

void
insert_close_brace (GtkWidget **tip_win)
{
	
  if (*tip_win != NULL)
    {
      gtk_widget_destroy (GTK_WIDGET (*tip_win));
      *tip_win = NULL;
    }
}


void buffer_insert_text (GtkTextIter *arg1, gpointer user_data)
{
  static GtkWidget *tip_win = NULL;


		insert_close_brace(&tip_win);
      insert_open_brace(&tip_win, GTK_WIDGET (user_data), arg1);

/*  if (strcmp (arg2, "(") == 0)
    {
      insert_open_brace(&tip_win, GTK_WIDGET (user_data), arg1);
    }

  if (strcmp (arg2, ")") == 0)
    {
      insert_close_brace(&tip_win);
    } */


}


