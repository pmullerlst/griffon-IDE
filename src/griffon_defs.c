#include <gtk/gtk.h>


//************************************************************ AIDE TOOLTIPS DE FONCTION
static char *tips[][2] = 
  {

//EXEMPLE :     {"printf", "(const char *format, ...)"},

//==================================================
//PHP

{"dat", " [PHP] \tstring date ( string $format [, int $timestamp = time() ] ) : Formate une date/heure locale"},
{"fil", " [PHP] \tarray file ( string $filename [, int $flags = 0 [, resource $context ]] ) : Lit le fichier et renvoie le résultat dans un tableau"},
{"fope", " [PHP] \tresource fopen ( string $filename , string $mode [, bool $use_include_path = false [, resource $context ]] ) : Ouvre un fichier ou une URL"},
{"requir", " [PHP] \tinclut et exécute le fichier spécifié en argument."}, 
{"includ", " [PHP] \tinclut et exécute le fichier spécifié en argument. "},
{"ech", " [PHP] \tvoid echo ( string $arg1 [, string $... ] ) : Affiche une chaîne de caractères"},
{"mysql_fetch_objec", " [PHP] \tobject mysql_fetch_object ( resource $result [, string $class_name [, array $params ]] ) : Retourne une ligne de résultat MySQL sous la forme d'un objet"},
{"mysql_quer", " [PHP] \tresource mysql_query ( string $query [, resource $link_identifier = NULL ] ) : Envoie une requête à un serveur MySQL"},
{"mysql_resul", " [PHP] \tstring mysql_result ( resource $result , int $row [, mixed $field = 0 ] ) : Retourne un champ d'un résultat MySQL"},
{"mysql_select_d", " [PHP] \tbool mysql_select_db ( string $database_name [, resource $link_identifier = NULL ] ) : Sélectionne une base de données MySQL"},
{"mysql_num_rows int mysql_num_row", " [PHP] \t( resource $result ) : Retourne le nombre de lignes d'un résultat MySQL"},
{"ftp_connec", " [PHP] \tresource ftp_connect ( string $host [, int $port = 21 [, int $timeout = 90 ]] ) : Ouvre une connexion FTP"},
{"ftp_logi", " [PHP] \tbool ftp_login ( resource $ftp_stream , string $username , string $password ) : Identification sur un serveur FTP"},
{"ftp_rawlis", " [PHP] \tarray ftp_rawlist ( resource $ftp_stream , string $directory [, bool $recursive = false ] ) : Fait une liste détaillée des fichiers d'un dossier"},
{"preg_spli", " [PHP] \tarray preg_split ( string $pattern , string $subject [, int $limit = -1 [, int $flags = 0 ]] ) : Éclate une chaîne par expression rationnelle"},
{"foreac", " [PHP] \tforeach (array_expression as $value) : La structure de langage foreach fournit une façon simple de parcourir des tableaux."},
{"ereg_replac", " [PHP] \tstring ereg_replace ( string $pattern , string $replacement , string $string ) : Remplacement par expression rationnelle"},
{"mai", " [PHP] \tbool mail ( string $to , string $subject , string $message [, string $additional_headers [, string $additional_parameters ]] ) : Envoi de mail"},
{"utf8_encod", " [PHP] \tstring utf8_encode ( string $data ) : Convertit une chaîne ISO-8859-1 en UTF-8"},
{"strst", " [PHP] \tstring strstr ( string $haystack , mixed $needle [, bool $before_needle = false ] ) : Trouve la première occurrence dans une chaîne"},
{"strle", " [PHP] \tint strlen ( string $string ) : Calcule la taille d'une chaîne"},
{"fput", " [PHP] \tint fwrite/fputs ( resource $handle , string $string [, int $length ] ) : Écrit un fichier en mode binaire"},
{"flcos", " [PHP] \tbool fclose ( resource $handle ) : Ferme un fichier "},
{"spli", " [PHP] \t  !OBSOLETE! array split ( string $pattern , string $string [, int $limit = -1 ] ) : Scinde une chaîne en un tableau, grâce à une expression rationnelle"}, 
{"ran", " [PHP] \tint rand ( int $min , int $max ) : Génère une valeur aléatoire"},
{"add", " [PHP] \tstring addslashes ( string $str ) : Ajoute des antislashs dans une chaîne"},
{"sub", " [PHP] \tstring substr ( string $string , int $start [, int $length ] ) : Retourne le segment de string défini par start et length."},
{"uc", " [PHP] \tstring ucfirst ( string $str ) : Met le premier caractère en majuscule"},
{"html", " [PHP] \tstring htmlspecialchars ( string $string [, int $flags = ENT_COMPAT | ENT_HTML401 [, string $encoding = 'UTF-8' [, bool $double_encode = true ]]] ) : Convertit les caractères spéciaux en entités HTML"},
{"html", " [PHP] \tstring htmlspecialchars_decode ( string $string [, int $flags = ENT_COMPAT | ENT_HTML401 ] ) : Convertit les entités HTML spéciales en caractères"},
{"mb_sub", " [PHP] \tint mb_substr_count ( string $haystack , string $needle [, string $encoding ] ) : Compte le nombre d'occurrences d'une sous-chaîne"},

//==================================================== {"flcos", " [PHP] \t"},
//Perl

{"ge", " [PERL]\tget() : retourne le document correspondant à l'URL passé en paramètre"},
{"lengt", " [PERL]\tlength() : Retourne la longueur de la chaîne (nombre de caractères)"},
{"localtim", " [PERL]\tlocaltime() : retourne heure locale"},
{"exi", " [PERL]\texit() : Kill le script"},
{"prin", " [PERL]\tprint() : Affiche une chaîne de caractères"},
{"ope", " [PERL]\topen() : Ouvre un fichier"},
{"clos", " [PERL]\tclose() : Fermer un fichier"},
{"foreac", " [PERL]\tforeach() : La structure de langage foreach fournit une façon simple de parcourir des tableaux."},
{"spli", " [PERL]\tsplit() : Scinde une chaîne en un tableau, grâce à une expression rationnelle "},
{"uri_escap", "\t[PERL] uri_escape() : Encode/Décodage d'url"},
{"exe", " [PERL]\texec() : exécution d'un programme"},
{"slee", " [PERL]\tsleep() : suspend l'exécution pendant une durée"},
{"sor", " [PERL]\tsort(@tab) : trie par ordre croissant les éléments."},
{"uc", " [PERL] \tuc(ch) : Retourne la chaîne en majuscules"},
{"lc", " [PERL] \tlc(ch) : Retourne la chaîne en minuscules"},
{"uc", " [PERL] \tlcfirst(ch) : Retourne la chaîne avec simplement le premier caractère en majuscule"},
{"sub", " [PERL] \tsubstr(ch, indicedébut, longueur) : Retourne la chaîne de caractère contenue dans ch, du caractère indicedébut et de longueur longueur."},
{"us", " [PERL] \tuse LIB : Charge une lib perl"},

//========================================================
//BASH

{"cd", " [BASH]\tcd : Change de répertoire de travail"},
{"pwd", " [BASH]\tpwd : Affiche le répertoire de travail "},
{"chmo", " [BASH]\tchmod : Change les droits du fichier"},
{"tar", " [BASH]\ttar : compression de données"},
{"ech", " [BASH]\techo : Affiche une chaîne de caractères"},
{"cp", " [BASH]\tcp : copie de fichier ou répertoire"},
{"rm", " [BASH]\trm : Supprime la référence"},
{"mv", " [BASH]\tmv : Renomme arg1 en arg2"},
{"ls", " [BASH]\tls : liste le contenu du répertoire de travail"},
{"du", " [BASH]\tdu : taille de fichier ou répertoire"},
{"touc", " [BASH]\ttouch : Crée le fichier"},
{"mkdi", " [BASH]\tmkdir : Crée le répertoire"},
{"df", " [BASH]\tdf : utilisation des ressources"},
{"cat", " [BASH]\tcat : Voir un fichier"},




  };

#define NUM_TIPS (sizeof (tips) / sizeof (tips[0]))

gchar *
get_tip(gchar *text)
{
  gint i;
  gboolean found;
	gchar *totale_tips="";
	gchar *separator="\n";	
	
  found = FALSE;
  for (i = 0; i < NUM_TIPS; i++)
    {
      if (strcmp (text, tips[i][0]) == 0)
        {
          found = TRUE;
          //*************CONCATENATION DE GCHAR 
          totale_tips = g_strdup_printf("%s %s", totale_tips, tips[i][1]);
         totale_tips = g_strdup_printf("%s %s", totale_tips, separator);          
          //break;
        }
    }
  if (!found)
    return NULL;

//  return g_strdup (tips[i][1]);
  return g_strdup (totale_tips);
}

GtkWidget *
tip_window_new (gchar *tip)
{
  GtkWidget *win;
  GtkWidget *label;
  GtkWidget *eb;
  cairo_surface_t *cmap;
  GdkColor color;
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
  cairo_surface_t *cmap;
  GdkColor color;
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


void
buffer_insert_text (GtkTextBuffer *textbuffer, GtkTextIter *arg1,
                    gchar *arg2, gint arg3, gpointer user_data)
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


