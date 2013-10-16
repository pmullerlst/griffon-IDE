#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>

#include <errno.h>
#include <sys/stat.h>
#include <glib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguage.h>
//#include <gtkspell/gtkspell.h>
#include <vte/vte.h>
                 
#include "griffon_text_document.h"
#include "griffon_funx.h"
#include "callbacks.h"
#include "interface.h"
#include "griffon_config.h"
#include "griffon_defs.h"
#include "griffon_hl.h"
#include "griffon_fr.h"
#include "griffon_proj.h"
#include "griffon_gtk_utils.h"
#include "help_def.h"

enum
{
  COLUMN = 0,
  NUM_COLS
} ;
 
  GtkTreeSelection *selection2; 
	  GtkWidget *window1;

//******************************* template tree aide php
GtkTreeModel *create_and_fill_model_php (void)
{
  GtkTreeStore *treestore;
  GtkTreeIter toplevel, child;

  treestore = gtk_tree_store_new(NUM_COLS,
                  G_TYPE_STRING);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, "1 Fonctions base [PHP]",
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.1 (php) débuter un script PHP")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.2 (php) echo : Affiche une chaîne de caractères")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.3 (php) ajouter un commentaire")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.4 (php) FUNCTION : créer une fonction")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.5 (php) MAIL : envoyer un email")),-1);  
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.6 (php) DATE format MySql YYYY-mm-jj hh:mm:ss")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.7 (php) RAND : Génère une valeur aléatoire")),-1);  
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.8 (php) POST : Afficher les variables POST")),-1); 
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.9 (php) COUNT : Compte tous les éléments d'un tableau")),-1);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("2 Instructions")),
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "2.1 (php) IF",-1);  
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "2.2 (php) IF ELSE",-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "2.3 (php) WHILE",-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.4 (php) FOREACH : lire un tableau ligne par ligne")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, "3 Fichier",
                     -1);
                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.1 (php) FILE : place le contenu d'un fichier (url possible) dans un tableau")),-1);        
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.2 (php) UNLINK : supprimer un fichier")),-1);  
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.3 (php) FPUTS : écrire dans un fichier")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.4 (php) DIR : Lister les fichiers d'un répertoire dans un tableau")),-1);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("4 Chaîne de carctères")),
                     -1);
                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.1 (php) SPLIT : découper une chaîne avec séparateur")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.2 (php) REMPLACEMENT : trouver une chaîne dans une variable et la remplacer")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.3 (php) IF chaîne : instruction rechercher une chaîne dans une autre chaîne")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.4 (php) STRIPSLASHES : Supprime les antislashs d'une chaîne")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.5 (php) STRLEN : Retourne la taille de la chaîne")),-1);  
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.6 (php) URLENCODE : Encode une chaîne en URL")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.7 (php) MB_SUBSTR_COUNT : Compte le nombre d'occurrences d'une sous-chaîne")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.8 (php) STRIP_TAGS : Supprime les balises HTML et PHP d'une chaîne")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.9 (php) EXPLODE : découpe une chaîne avec séparateur et retourne un tableau")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.10 (php) ADDSLASHES : Ajoute des antislashs dans une chaîne")),-1);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, "5 MySql",
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.1 (php) connexion à une base MySql")),-1);                         
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.2 (php) SELECT BOUCLE : select mysql avec boucle de récuperation des données")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.3 (php) QUERY : simple requete Mysql (UPDATE,DELETE,INSERT)")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.4 (php) QUERY : simple requete Mysql (SELECT)")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.5 (php) LAST_INSERT_ID : récuperation d'un id après un INSERT")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.6 (php) NUM_ROWS : récuperation du nombre de résultats d'une requete")),-1);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, "6 FTP",
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.1 (php) connexion à un serveur FTP")),-1); 
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.2 (php) FTP_NLIST : lister les fichier d'un répertoire")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.3 (php) FTP_PUT : déposer un fichier")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.4 (php) FTP_DELETE : supprimer un fichier")),-1);
 
  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, "7 Header",
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "7.1 (php) TEXT UTF8",-1); 
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "7.2 (php) IMAGE PNG",-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "7.3 (php) Téléchargement d'un fichier CSV",-1); 
                                                                              
  return GTK_TREE_MODEL(treestore);
}

//******************************* template tree aide HTML
GtkTreeModel *create_and_fill_model_html (void)
{
  GtkTreeStore *treestore;
  GtkTreeIter toplevel, child;

  treestore = gtk_tree_store_new(NUM_COLS,
                  G_TYPE_STRING);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("1 Base de page [HTML]")),
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.1 (html) : Template de base")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.2 (html) : Commenter")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.3 (html) : Tableau [Option CMD : \"{NBR_TR}-{NBR_TD}\"]")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.4 (html) : Lien HREF autour de la sélection")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.5 (html) : Intergation d'une feuille de style css")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.6 (html) : Rechargement (refresh) d'une page toutes les 30 secondes")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.7 (html) : Balise image autour de la séléction")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.8 (html) : Ajouter un div autour de la sélection")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.9 (html) : Ajouter un style à une balise")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.10 (html) : Ajouter un favicon 16x16 format .ico")),-1);
       
  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("2 Taille")),
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.1 (html) : [width] Largeur")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.2 (html) : [height] Hauteur")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.3 (html) : [cols] nombre de colonne (textarea)")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.4 (html) : [rows] nombre de ligne (textarea)")),-1);
     
  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("3 Formulaire")),
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.1 (html) : Formulaire de base")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.2 (html) : Zone de saisie texte")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.3 (html) : Zone de saisie textarea")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.4 (html) : Zone de sélection d'option (SELECT)")),-1);


  return GTK_TREE_MODEL(treestore);
}

//******************************* template tree aide css
GtkTreeModel *create_and_fill_model_css (void)
{
  GtkTreeStore *treestore;
  GtkTreeIter toplevel, child;

  treestore = gtk_tree_store_new(NUM_COLS,
                  G_TYPE_STRING);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("1 Base de feuille de style [CSS]")),
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.1 (css) : Commenter")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.2 (css) : Style pour balise body")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.3 (css) : Style pour lien")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.4 (css) : Style pour lien hover")),-1);    
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.5 (css) : Style pour balise div avec ID")),-1);  
    
  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, "2 Base background",
                     -1);  
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.1 (css) : [background] Couleur de fond")),-1);                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.2 (css) : [background-image] Image de fond")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.3 (css) : [background-repeat] Option image de fond non répétée")),-1);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, "3 Base font",
                     -1);  
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.1 (css) : [font-family] style de font")),-1); 
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.2 (css) : [font-size] Taille de la font")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.3 (css) : [color] Courleur de la font")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.4 (css) : [decoration] Pas de décoration de texte")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.5 (css) : [decoration] Soulignement du texte")),-1);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("4 Base taille")),
                     -1);  
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.1 (css) : [largeur] width largeur")),-1); 
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.2 (css) : [hauteur] height hauteur")),-1); 

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("5 Base position")),
                     -1);  
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.1 (css) : [position] relative Positionnement relatif par rapport à la position de l'élément précédent")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.2 (css) : [position] absolute Positionnement absolu par rapport au coin supérieur gauche du document")),-1);   
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.3 (css) : [position] fixed Positionnement fixe par rapport au coin supérieur gauche de la fenêtre du navigateur")),-1);                       
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.4 (css) : [gauche] left Position à partire du bord gauche")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.5 (css) : [droite] right position à partire du bord droit")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.6 (css) : [haut] top position à partire du bord haut")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.7 (css) : [bas] bottom position à partire du bord bas")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.8 (css) : [valign] alignement vertical en bas bottom")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.9 (css) : [valign] alignement vertical en haut")),-1);
  
  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("6 Base affichage")),
                     -1);  
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.1 (css) : [none] display L'élément est masqué")),-1);   
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.2 (css) : [block] display Affichage l'un en-dessous de l'autre")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.3 (css) : [inline] display Affichage en ligne des div")),-1);  
                                                    
  return GTK_TREE_MODEL(treestore);
}

//******************************* template tree aide mysql
GtkTreeModel *create_and_fill_model_mysql (void)
{
  GtkTreeStore *treestore;
  GtkTreeIter toplevel, child;

  treestore = gtk_tree_store_new(NUM_COLS,
                  G_TYPE_STRING);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("1 Requete base [MYSQL]")),
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "1.1 (mysql) : SELECT",-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "1.2 (mysql) : INSERT",-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "1.3 (mysql) : UPDATE",-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "1.4 (mysql) : DELETE",-1);  
      
  return GTK_TREE_MODEL(treestore);
}

//******************************* template tree aide javascript
GtkTreeModel *create_and_fill_model_javascript (void)
{
  GtkTreeStore *treestore;
  GtkTreeIter toplevel, child;

  treestore = gtk_tree_store_new(NUM_COLS,
                  G_TYPE_STRING);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, "1 Base [Javascript]",
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.1 (javascript) : Balise HTML pour javascript/jquery")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.2 (javascript) : Créer une fonction")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.3 (javascript) : Split d'une chaîne en tableau par séparateur")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.4 (javascript) : Encodage d'une URL")),-1);
     
  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("2 Intéraction avec un ID")),
                     -1);
 
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.1 (javascript) : Récuperation d'un value par id")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.2 (javascript) : Récuperation d'un value par id sur un select HTML")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.3 (javascript) : Focus sur un id")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.4 (javascript) : Charger un fichier php/html dans un div id")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.5 (javascript) : Remplacer le contenu d'un div id par un autre texte")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.6 (javascript) : Remplacer le contenu d'un value html id par un autre texte")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.7 (javascript) : Ajouter un texte au contenu d'un value html id")),-1);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("3 Liste des évenements")),
                     -1);
 
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.1 (javascript) : [onBlur] action en sortie de focus")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.2 (javascript) : [onChange] action pour un changement")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.3 (javascript) : [onClick] action pour un click")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.4 (javascript) : [onFocus] action sur prise de focus")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.5 (javascript) : [onLoad] action en fin de chargement")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.6 (javascript) : [onMouseOver] action au passage de la souris")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.7 (javascript) : [onSelect] action pour une séléction")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.8 (javascript) : [onSubmit] action en validation de formulaire")),-1);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("4 Fonctions")),
                     -1);
 
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.1 (javascript) : Fonction récuperation du contenu d'un fichier")),-1);

  return GTK_TREE_MODEL(treestore);
}


//******************************* template tree aide htaccess
GtkTreeModel *create_and_fill_model_htaccess (void)
{
  GtkTreeStore *treestore;
  GtkTreeIter toplevel, child;

  treestore = gtk_tree_store_new(NUM_COLS,
                  G_TYPE_STRING);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, "1 Base [Htaccess]",
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.1 (htaccess) : Charset par défaut (UTF8)")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.2 (htaccess) : bloquer toutes les ip")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.3 (htaccess) : Autoriser une ip")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.4 (htaccess) : Redirection des erreurs")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.5 (htaccess) : Activation de l'url rewriting")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.6 (htaccess) : ligne exemple simple url rewriting")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.7 (htaccess) : ligne exemple avec option et expression régulière url rewriting")),-1);

  return GTK_TREE_MODEL(treestore);
}

//******************************* template tree aide perl
GtkTreeModel *create_and_fill_model_perl (void)
{
  GtkTreeStore *treestore;
  GtkTreeIter toplevel, child, child2;

  treestore = gtk_tree_store_new(NUM_COLS,
                  G_TYPE_STRING);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("1 Fonctions base [PERL]")),
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.1 (perl) : Débuter un script")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.2 (perl) PRINT : afficher une chaîne de caractères")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);  
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.3 (perl) ARGUMENTS : utiliser les arguments du programme")),-1);


	  gtk_tree_store_append(treestore, &child2, &toplevel);  
	  gtk_tree_store_set(treestore, &child2,COLUMN, (_("TABLEAU")),-1);
		  gtk_tree_store_append(treestore, &child, &child2);    
		  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.4 (perl) TABLEAU : déclaration d'un tableau")),-1);  
		  gtk_tree_store_append(treestore, &child, &child2);
		  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.5 (perl) TABLEAU : empiler une valeur en fin de tableau")),-1);
		  gtk_tree_store_append(treestore, &child, &child2);
		  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.6 (perl) TABLEAU : empiler une valeur en début de tableau")),-1);  
		  gtk_tree_store_append(treestore, &child, &child2);
		  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.7 (perl) FOREACH : lire un tableau ligne par ligne")),-1);  

  gtk_tree_store_append(treestore, &child, &toplevel);  
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.8 (perl) SAISIE : attend une saisie clavier")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);  
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.9 (perl) FONCTION : créer une fonction")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);  
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.10 (perl) EXIT : sortir (killer) le programme")),-1);    
  gtk_tree_store_append(treestore, &child, &toplevel);  
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.11 (perl) LOCALTIME : Récuperation de la date")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);  
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.12 (perl) RAND : génération d'un nombre aléatoire")),-1);
  
  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("2 Instructions")),
                     -1);    

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "2.1 (perl) IF",-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.2 (perl) UNLESS : test négatif")),-1);  
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.3 (perl) IF ELSE")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.4 (perl) WHILE")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.5 (perl) WHILE infinie")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.6 (perl) FOREACH : lire un tableau ligne par ligne")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("3 Fichier lecture écriture")),
                     -1);                    
                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.1 (perl) Lire un fichier ligne par ligne")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.2 (perl) ouvrir un fichier pour y ajouter des données")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.3 (perl) ouvrir un fichier pour y ajouter des données et écraser l'éxistant")),-1);    
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.4 (perl) ouvrir un tube sur un fichier")),-1);  
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.5 (perl) STDIN Lecture de l'entrée standard")),-1); 
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.6 (perl) STAT : récuperation des stats d'un fichier ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks)")),-1); 

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("4 Chaîne de carctères")),
                     -1);
                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.1 (perl) SPLIT : découper une chaîne avec séparateur")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.2 (perl) IF UNLESS chaîne : instruction rechercher une chaîne dans une autre chaîne")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.3 (perl) REMPLACEMENT : trouver une chaîne dans une variable et la remplacer")),-1);    
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.4 (perl) CHOP : Enlève le dernier caractère de la chaîne")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.5 (perl) LENGTH : Retourne la longueur de la chaîne (nombre de caractères)")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.6 (perl) UC : Retourne la chaîne en majuscules")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.7 (perl) LC : Retourne la chaîne en minuscules")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.8 (perl) UCFIRST : Retourne la chaîne avec le premier caractère en majuscule")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.9 (perl) SUBSTR : retourne une chaîne contenue entre indice de début et la longueur")),-1);
          
  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, "5 Lib DBI MySql",
                     -1);
                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.1 (lib_dbi) CONNEXION : se connecter à une base MySql")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.2 (lib_dbi) QUERY : simple requete Mysql (UPDATE,DELETE,INSERT)")),-1); 
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.3 (lib_dbi) QUERY : simple requete Mysql (SELECT)")),-1);   
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.4 (lib_dbi) SELECT BOUCLE : select mysql avec boucle de récuperation des données")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.5 (lib_dbi) LASTID : récuperation du dernier id en insert")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.6 (lib_dbi) ROWS : rows récupération du nombre de résultats pour une requête")),-1);


  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, "6 Lib MIME Lite",
                     -1);
                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.1 (lib_mime) Envoi d\'un mail simple")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.2 (lib_mime) Envoi d\'un mail avec fichier PJ")),-1);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("7 HTTP Lib URI et LWP")),
                     -1);
                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("7.1 (lib_URI) encodage d\'une URL")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("7.2 (lib_LWP) get sur une URL")),-1);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, "8 FTP Lib NET::FTP",
                     -1);
                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("8.1 (lib_net) connexion à un serveur FTP")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("8.2 (lib_net) lister le contenu d'un répertoire FTP")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("8.3 (lib_net) GET télécharger un fichier depuis un FTP")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("8.4 (lib_net) PUT uploader un fichier sur un FTP")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("8.5 (lib_net) CWD Changer de répertoire")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("8.6 (lib_net) DELETE Supprimer un fichier")),-1);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, "9 SOCKET IO::Socket::INET",
                     -1);
                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("9.1 (lib_socket) socket en mode écoute (serveur) avec fork")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("9.2 (lib_socket) socket en mode client")),-1);

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
  return GTK_TREE_MODEL(treestore);
}

//******************************* template tree aide bash
GtkTreeModel *create_and_fill_model_bash (void)
{
  GtkTreeStore *treestore;
  GtkTreeIter toplevel, child;

  treestore = gtk_tree_store_new(NUM_COLS,
                  G_TYPE_STRING);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, "1 Base bash",
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.1 (bash) Débuter un script")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.2 (bash) ECHO : afficher une chaîne de caractères")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.3 (bash) Ajouter un commentaire")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.4 (bash) Commande en tache de fond")),-1);
                     
  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("2 Instructions")),
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "2.1 (bash) IF",-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "2.2 (bash) IF ELSE bash",-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "2.3 (bash) WHILE bash",-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.4 (bash) CASE : multi choix")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.5 (bash) READ : interactivite avec l'utilisateur")),-1);
 
  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("3 Opérateur logique")),
                     -1);                     

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.1 (bash) ET")),-1);                                                            
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.2 (bash) OU")),-1);
 
 
  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("4 Commandes de base")),
                     -1);
                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "4.1 (bash) GREP",-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "4.2 (bash) AWK",-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "4.3 (bash) SED",-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "4.4 (bash) SCP",-1);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("5 Redirection de fichier")),
                     -1);
                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.1 (bash) Redirection de la sortie dans le vide")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.2 (bash) La sortie écrase un fichier")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.3 (bash) La sortie est ajoutée à un fichier")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.4 (bash) Redirection des erreurs")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.5 (bash) Redirection des erreurs et des messages")),-1);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("6 Construction de [TEST]")),
                     -1);
                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.1 (bash) TEST : si un fichier existe")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.2 (bash) TEST : si un répertoire existe")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.3 (bash) TEST : inverser un test")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.4 (bash) TEST : droit en lecture")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.5 (bash) TEST : droit en écriture")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.6 (bash) TEST : droit en excution")),-1);                                                                                                                                                                                                                                                                 
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN,(_("6.7 (bash) TEST : sur deux variables")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.8 (bash) TEST : sur deux chiffres")),-1);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
  return GTK_TREE_MODEL(treestore);
}

//******************************* match et envoi des fonction d'aide insert
void  on_changed(GtkWidget *widget, gpointer statusbar) 
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  char *value;

 if (! get_page_text()){ log_to_memo (_("Vous devez ouvrir un fichier pour pouvoir utiliser l\'aide."), NULL, LM_ERROR);return;}

  if (gtk_tree_selection_get_selected(
      GTK_TREE_SELECTION(widget), &model, &iter)) {

		
    gtk_tree_model_get(model, &iter, COLUMN, &value,  -1);
    gtk_statusbar_push(GTK_STATUSBAR(statusbar),
        gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), 
            value), value);

				//**************** OPEN WINDOWS HELP
            if (strcmp("[BASH]", value) == 0){Aide_BASH();}     
            if (strcmp("[PERL]", value) == 0){centre_perl();}
            if (strcmp("[PHP]", value) == 0){help_php_window();}
            if (strcmp("[MYSQL]", value) == 0){help_mysql();} 
				if (strcmp("[HTML]", value) == 0){help_html();}  
				if (strcmp("[CSS]", value) == 0){help_css();}
				if (strcmp("[JAVASCRIPT/JQUERY]", value) == 0){help_javascript();}      
				if (strcmp("[HTACCESS]", value) == 0){centre_htaccess();}
                                                        
	 gtk_tree_selection_unselect_all(GTK_TREE_SELECTION(widget));                                        
    g_free(value);
  }
	
}


//******************************* match et envoi des fonction d'aide insert
void  on_changed2(gpointer *user_data) 
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  char *value;

  if (gtk_tree_selection_get_selected(
      GTK_TREE_SELECTION(selection2), &model, &iter)) {

		
    gtk_tree_model_get(model, &iter, COLUMN, &value,  -1);
      
				gchar **a = g_strsplit (value, ")", -1);   
				gchar **a2 = g_strsplit (a[0], "(", -1);                      

            //*************** APPEL AU FONCTION TERM
				if (strcmp("1.1 (term", a[0]) == 0){term_df(user_data);return;}
				if (strcmp("1.2 (term", a[0]) == 0){term_aspiration_web(user_data);return;}
				if (strcmp("1.3 (term", a[0]) == 0){term_long_rm(user_data);return;}
				if (strcmp("1.4 (term", a[0]) == 0){term_find(user_data);return;}
				if (strcmp("1.5 (term", a[0]) == 0){term_screen(user_data);return;}
				if (strcmp("1.6 (term", a[0]) == 0){term_wc(user_data);return;}
				if (strcmp("1.7 (term", a[0]) == 0){term_wget(user_data);return;}
				if (strcmp("1.8 (term", a[0]) == 0){term_ls_color(user_data);return;}
				if (strcmp("1.9 (term", a[0]) == 0){term_nice(user_data);return;}
				if (strcmp("2.0 (term", a[0]) == 0){term_comp_targz(user_data);return;}
				if (strcmp("2.1 (term", a[0]) == 0){term_decomp_targz(user_data);return;}
				if (strcmp("2.2 (term", a[0]) == 0){term_diff(user_data);return;}

			 if (! get_page_text()){ log_to_memo (_("Vous devez ouvrir un fichier pour pouvoir utiliser l\'aide."), NULL, LM_ERROR);return;}

            //*************** APPEL AU FONCTION HTACCESS
				if (strcmp("1.1 (htaccess", a[0]) == 0){htaccess_charset_utf8();}
				if (strcmp("1.2 (htaccess", a[0]) == 0){htaccess_deny_all();}
				if (strcmp("1.3 (htaccess", a[0]) == 0){htaccess_accept_ip();}
				if (strcmp("1.4 (htaccess", a[0]) == 0){htaccess_redirection_erreur();}
				if (strcmp("1.5 (htaccess", a[0]) == 0){htaccess_urlrw_on();}
				if (strcmp("1.6 (htaccess", a[0]) == 0){htaccess_urlrw();}
				if (strcmp("1.7 (htaccess", a[0]) == 0){htaccess_urlrw_exp();}
           
            //*************** APPEL AU FONCTION PHP
				if (strcmp("1.1 (php", a[0]) == 0){start_php_script();}
            if (strcmp("1.2 (php", a[0]) == 0){echo_php();}            
            if (strcmp("1.3 (php", a[0]) == 0){on_mni_Markup_comment(NULL,NULL);}
            if (strcmp("5.1 (php", a[0]) == 0){php_mysql_connexion();}
            if (strcmp("5.2 (php", a[0]) == 0){php_mysql_while();}
            if (strcmp("5.3 (php", a[0]) == 0){php_mysql_query();}
            if (strcmp("5.4 (php", a[0]) == 0){php_mysql_query_select();}
            if (strcmp("2.1 (php", a[0]) == 0){perl_if();}
            if (strcmp("2.2 (php", a[0]) == 0){perl_if_else();}
            if (strcmp("2.3 (php", a[0]) == 0){perl_while();}
            if (strcmp("2.4 (php", a[0]) == 0){php_foreach();}
            if (strcmp("4.1 (php", a[0]) == 0){php_split();}
            if (strcmp("4.2 (php", a[0]) == 0){php_replace();}
            if (strcmp("3.1 (php", a[0]) == 0){php_file();}
            if (strcmp("4.3 (php", a[0]) == 0){php_find();}
            if (strcmp("1.4 (php", a[0]) == 0){php_function();}
            if (strcmp("4.4 (php", a[0]) == 0){php_stripslashes();}
            if (strcmp("1.5 (php", a[0]) == 0){php_mail();}
				if (strcmp("5.5 (php", a[0]) == 0){php_insert_id();}
				if (strcmp("6.1 (php", a[0]) == 0){php_ftp_connect();}
				if (strcmp("6.2 (php", a[0]) == 0){php_ftp_list_rep();}
				if (strcmp("6.3 (php", a[0]) == 0){php_ftp_put();}
				if (strcmp("6.4 (php", a[0]) == 0){php_ftp_delete();}
				if (strcmp("7.1 (php", a[0]) == 0){php_h_utf8();}
				if (strcmp("7.2 (php", a[0]) == 0){php_h_png();}
				if (strcmp("1.6 (php", a[0]) == 0){php_date_mysql();}
				if (strcmp("4.5 (php", a[0]) == 0){php_strlen();}
				if (strcmp("4.6 (php", a[0]) == 0){php_urlencode();}
				if (strcmp("1.7 (php", a[0]) == 0){php_rand();}
				if (strcmp("4.7 (php", a[0]) == 0){php_mb_substr_count();}
				if (strcmp("1.8 (php", a[0]) == 0){php_var_post();}
				if (strcmp("4.8 (php", a[0]) == 0){php_strip_tags();}
				if (strcmp("5.6 (php", a[0]) == 0){php_num_rows();}
				if (strcmp("4.9 (php", a[0]) == 0){php_explode();}
				if (strcmp("3.2 (php", a[0]) == 0){php_unlink();}
				if (strcmp("3.3 (php", a[0]) == 0){php_fputs();}
				if (strcmp("1.9 (php", a[0]) == 0){php_count();}
				if (strcmp("3.4 (php", a[0]) == 0){php_dir();}
				if (strcmp("4.10 (php", a[0]) == 0){php_addslashes();}
				if (strcmp("7.3 (php", a[0]) == 0){php_dl_csv();}
            
            //*************** APPEL AU FONCTION BASH
            if (strcmp("1.1 (bash", a[0]) == 0){print_bash();}
            if (strcmp("1.2 (bash", a[0]) == 0){echo_bash();}
            if (strcmp("1.3 (bash", a[0]) == 0){com();}
            if (strcmp("1.4 (bash", a[0]) == 0){fond_bash();}
            if (strcmp("2.1 (bash", a[0]) == 0){if_bash();}
            if (strcmp("2.2 (bash", a[0]) == 0){else_bash();}
            if (strcmp("2.3 (bash", a[0]) == 0){boucle_bash();}
            if (strcmp("2.4 (bash", a[0]) == 0){case_window();}
            if (strcmp("2.5 (bash", a[0]) == 0){gen_read();}
            if (strcmp("3.1 (bash", a[0]) == 0){et_bash();}
            if (strcmp("3.2 (bash", a[0]) == 0){ou_bash();}
            if (strcmp("4.1 (bash", a[0]) == 0){window_grep();}
            if (strcmp("4.2 (bash", a[0]) == 0){awk();}
            if (strcmp("4.3 (bash", a[0]) == 0){sed();}
            if (strcmp("4.4 (bash", a[0]) == 0){win_scp();}
            if (strcmp("5.1 (bash", a[0]) == 0){reNULL_bash();}
            if (strcmp("5.2 (bash", a[0]) == 0){reECRASER_bash();}
            if (strcmp("5.3 (bash", a[0]) == 0){reRAJOUTER_bash();}
            if (strcmp("5.4 (bash", a[0]) == 0){redi_erreur();}
            if (strcmp("5.5 (bash", a[0]) == 0){redi_2();}
            if (strcmp("6.1 (bash", a[0]) == 0){test1();}
            if (strcmp("6.2 (bash", a[0]) == 0){test2();}
            if (strcmp("6.3 (bash", a[0]) == 0){test3();}
            if (strcmp("6.4 (bash", a[0]) == 0){test4();}
            if (strcmp("6.5 (bash", a[0]) == 0){test5();}
            if (strcmp("6.6 (bash", a[0]) == 0){test6();}
            if (strcmp("6.7 (bash", a[0]) == 0){test10();}
            if (strcmp("6.8 (bash", a[0]) == 0){test11();}
            
            //*************** APPEL AU FONCTION PERL
            if (strcmp("1.1 (perl", a[0]) == 0){start_perl_script();}
            if (strcmp("1.12 (perl", a[0]) == 0){perl_rand();}
            if (strcmp("1.2 (perl", a[0]) == 0){perl_print();}
            if (strcmp("1.4 (perl", a[0]) == 0){perl_tableau();}
            if (strcmp("1.8 (perl", a[0]) == 0){perl_saisie_clavier();}
            if (strcmp("1.5 (perl", a[0]) == 0){perl_empiler_tableau();}
            if (strcmp("1.6 (perl", a[0]) == 0){perl_empiler_tableau_start();}
            if (strcmp("1.3 (perl", a[0]) == 0){perl_arguments();}
            if (strcmp("2.1 (perl", a[0]) == 0){perl_if();}
            if (strcmp("2.2 (perl", a[0]) == 0){perl_unless();}
            if (strcmp("2.3 (perl", a[0]) == 0){perl_if_else();}
            if (strcmp("2.4 (perl", a[0]) == 0){perl_while();}
            if (strcmp("2.5 (perl", a[0]) == 0){perl_while_infinie();}
            if (strcmp("1.7 (perl", a[0]) == 0){perl_foreach();}
				if (strcmp("2.6 (perl", a[0]) == 0){perl_foreach();}
            if (strcmp("1.9 (perl", a[0]) == 0){perl_fonction_param();}
            if (strcmp("1.10 (perl", a[0]) == 0){perl_exit();}
            if (strcmp("3.1 (perl", a[0]) == 0){perl_read();}
            if (strcmp("3.2 (perl", a[0]) == 0){perl_writh();}
            if (strcmp("3.3 (perl", a[0]) == 0){perl_writh2();}
            if (strcmp("4.1 (perl", a[0]) == 0){perl_split();}
            if (strcmp("5.1 (lib_dbi", a[0]) == 0){perl_dbi_connexion();}
            if (strcmp("5.4 (lib_dbi", a[0]) == 0){perl_dbi_select_while();}
            if (strcmp("5.2 (lib_dbi", a[0]) == 0){perl_dbi_query();}
            if (strcmp("4.2 (perl", a[0]) == 0){perl_regular();}
            if (strcmp("4.3 (perl", a[0]) == 0){perl_replace();}
            if (strcmp("4.4 (perl", a[0]) == 0){perl_chop();}
            if (strcmp("4.5 (perl", a[0]) == 0){perl_length();}
            if (strcmp("4.6 (perl", a[0]) == 0){perl_uc();}
            if (strcmp("4.7 (perl", a[0]) == 0){perl_lc();}
            if (strcmp("4.8 (perl", a[0]) == 0){perl_ucfirst();}
				if (strcmp("6.1 (lib_mime", a[0]) == 0){perl_mime_mail_simple();}
				if (strcmp("1.11 (perl", a[0]) == 0){perl_date();}
				if (strcmp("6.2 (lib_mime", a[0]) == 0){perl_mime_mail_pj();}
            if (strcmp("5.3 (lib_dbi", a[0]) == 0){perl_dbi_query_select();}
            if (strcmp("7.1 (lib_URI", a[0]) == 0){perl_uri_encode();}    
				if (strcmp("7.2 (lib_LWP", a[0]) == 0){perl_url_get();}     
				if (strcmp("8.1 (lib_net", a[0]) == 0){perl_ftp_connexion();}
				if (strcmp("8.2 (lib_net", a[0]) == 0){perl_ftp_list();}   
				if (strcmp("8.3 (lib_net", a[0]) == 0){perl_ftp_get();}
				if (strcmp("8.4 (lib_net", a[0]) == 0){perl_ftp_put();}
				if (strcmp("9.1 (lib_socket", a[0]) == 0){perl_socket_srv();}
				if (strcmp("9.2 (lib_socket", a[0]) == 0){perl_socket_client();}
				if (strcmp("4.9 (perl", a[0]) == 0){perl_substr();}
				if (strcmp("3.4 (perl", a[0]) == 0){perl_tube();}
				if (strcmp("3.5 (perl", a[0]) == 0){perl_stdin();}
				if (strcmp("5.5 (lib_dbi", a[0]) == 0){perl_dbi_lastid();}
				if (strcmp("5.6 (lib_dbi", a[0]) == 0){perl_dbi_rows();}
				if (strcmp("3.6 (perl", a[0]) == 0){perl_stat();}
				if (strcmp("8.5 (perl", a[0]) == 0){perl_ftp_cwd();}
				if (strcmp("8.6 (perl", a[0]) == 0){perl_ftp_delete();}

            //*************** APPEL AU FONCTION MYSQL
            if (strcmp("1.2 (mysql", a[0]) == 0){mysql_insert();}
            if (strcmp("1.1 (mysql", a[0]) == 0){mysql_select();}
            if (strcmp("1.3 (mysql", a[0]) == 0){mysql_update();} 
            if (strcmp("1.4 (mysql", a[0]) == 0){mysql_delete();}           
 
            //*************** APPEL AU FONCTION HTML
            if (strcmp("1.1 (html", a[0]) == 0){on_mni_html_default_template();}
            if (strcmp("1.3 (html", a[0]) == 0){on_mni_html_make_table();}
				if (strcmp("1.4 (html", a[0]) == 0){on_mni_html_enclose_link();}
				if (strcmp("1.5 (html", a[0]) == 0){html_css();}
				if (strcmp("1.6 (html", a[0]) == 0){html_reload();}
				if (strcmp("2.1 (html", a[0]) == 0){html_largeur();}
				if (strcmp("2.2 (html", a[0]) == 0){html_hauteur();}
				if (strcmp("2.4 (html", a[0]) == 0){html_rows();}
				if (strcmp("2.3 (html", a[0]) == 0){html_cols();}
				if (strcmp("3.1 (html", a[0]) == 0){html_form();}
				if (strcmp("3.2 (html", a[0]) == 0){html_input_text();}
				if (strcmp("3.3 (html", a[0]) == 0){html_input_textarea();}
				if (strcmp("3.4 (html", a[0]) == 0){html_input_select();}
				if (strcmp("1.2 (html", a[0]) == 0){on_mni_Markup_comment(NULL,NULL);}
				if (strcmp("1.7 (html", a[0]) == 0){on_mni_image_activate(NULL,NULL);}
				if (strcmp("1.8 (html", a[0]) == 0){html_div();}	
				if (strcmp("1.9 (html", a[0]) == 0){html_style();}
				if (strcmp("1.10 (html", a[0]) == 0){html_favicon();}

            //*************** APPEL AU FONCTION CSS
				if (strcmp("1.1 (css", a[0]) == 0){on_mni_Markup_comment(NULL,NULL);}
            if (strcmp("1.2 (css", a[0]) == 0){css_body();}
            if (strcmp("1.3 (css", a[0]) == 0){css_link();}
            if (strcmp("1.4 (css", a[0]) == 0){css_link_hover();}
            if (strcmp("1.5 (css", a[0]) == 0){css_div();}
				if (strcmp("2.2 (css", a[0]) == 0){css_bg_img();}
				if (strcmp("2.3 (css", a[0]) == 0){css_bg_img_norep();}	
				if (strcmp("2.1 (css", a[0]) == 0){css_bg_couleur();}
				if (strcmp("3.2 (css", a[0]) == 0){css_font_size();}
				if (strcmp("3.1 (css", a[0]) == 0){css_font_style();}
				if (strcmp("3.3 (css", a[0]) == 0){css_font_color();}
				if (strcmp("3.5 (css", a[0]) == 0){css_font_deco_und();}
				if (strcmp("3.4 (css", a[0]) == 0){css_font_deco_non();}
				if (strcmp("4.1 (css", a[0]) == 0){css_largeur();}
				if (strcmp("4.2 (css", a[0]) == 0){css_hauteur();}
				if (strcmp("5.4 (css", a[0]) == 0){css_gauche();}
				if (strcmp("5.5 (css", a[0]) == 0){css_droit();}
				if (strcmp("5.6 (css", a[0]) == 0){css_haut();}
				if (strcmp("5.7 (css", a[0]) == 0){css_bas();}
				if (strcmp("5.1 (css", a[0]) == 0){css_relative();}
				if (strcmp("5.2 (css", a[0]) == 0){css_absolute();}
				if (strcmp("5.3 (css", a[0]) == 0){css_fixed();}
				if (strcmp("6.1 (css", a[0]) == 0){css_none();}
				if (strcmp("6.2 (css", a[0]) == 0){css_block();}
				if (strcmp("6.3 (css", a[0]) == 0){css_inline();}
				if (strcmp("5.9 (css", a[0]) == 0){css_valign_top();}
				if (strcmp("5.8 (css", a[0]) == 0){css_valign_bottom();}

            //*************** APPEL AU FONCTION JAVASCRIPT JQUERY 
				if (strcmp("1.1 (javascript", a[0]) == 0){javascript_html();}
				if (strcmp("1.2 (javascript", a[0]) == 0){javascript_fonction();}
				if (strcmp("2.1 (javascript", a[0]) == 0){javascript_value_par_id();}
				if (strcmp("2.3 (javascript", a[0]) == 0){javascript_focus_id();}
				if (strcmp("2.2 (javascript", a[0]) == 0){javascript_value_select();}
				if (strcmp("2.4 (javascript", a[0]) == 0){javascript_id_load_file();}
				if (strcmp("2.5 (javascript", a[0]) == 0){javascript_html_change();}
				if (strcmp("2.6 (javascript", a[0]) == 0){javascript_id_change();}
				if (strcmp("2.7 (javascript", a[0]) == 0){javascript_id_change_add();}
				if (strcmp("3.1 (javascript", a[0]) == 0){javascript_onblur();}
				if (strcmp("3.2 (javascript", a[0]) == 0){javascript_onchange();}
				if (strcmp("3.3 (javascript", a[0]) == 0){javascript_onclick();}                  
				if (strcmp("3.4 (javascript", a[0]) == 0){javascript_onfocus();}
				if (strcmp("3.5 (javascript", a[0]) == 0){javascript_onload();}
				if (strcmp("3.6 (javascript", a[0]) == 0){javascript_onmouseover();}
				if (strcmp("3.7 (javascript", a[0]) == 0){javascript_onselect();}
				if (strcmp("3.8 (javascript", a[0]) == 0){javascript_onsubmit();}
				if (strcmp("1.3 (javascript", a[0]) == 0){javascript_split();}
				if (strcmp("4.1 (javascript", a[0]) == 0){javascript_file_contenu();}
				if (strcmp("1.4 (javascript", a[0]) == 0){javascript_urlencode();}

		char search_google[150];
		strcpy(search_google,"https://www.google.fr/#q=");
		strcat(search_google,a2[1]);
		strcat(search_google,a[1]);
		webkit_web_view_load_uri(webView_myadmin_aide, search_google);
				
	 //gtk_tree_selection_unselect_all(GTK_TREE_SELECTION(selection2));                                        
    g_free(value);
  }
	
}


//********************************* Fonction de recherche dans le treeview
gboolean util_match_word_in_sentence(gchar* pszWord, gchar* pszSentence)
{
	// First see if the search string is a prefix of the text...
	gint nWordLength = strlen(pszWord);
	if(strncmp(pszSentence, pszWord, nWordLength) == 0) return TRUE;

	// ...otherwise search inside the text, but only match to beginnings of words.
	// A little hack here: just search for " butter"	XXX: what about eg. "-butter"?
	gchar* pszWordWithSpace = g_strdup_printf(" %s", pszWord);
	gboolean bMatch = (strstr(pszSentence, pszWordWithSpace) != NULL);	// if it returns a pointer, we have a match
	g_free(pszWordWithSpace);

	return bMatch;
}

gboolean util_match_all_words_in_sentence(gchar* pszWords, gchar* pszSentence)
{
	// Split up search string into an array of word strings
	gchar** aWords = g_strsplit(pszWords, " ", 0);	// " " = delimeters, 0 = no max #

	// Make sure all words are in the sentence (order doesn't matter)
	gboolean bAllFound = TRUE;
	gint i;
	for(i = 0 ; aWords[i] != NULL ; i++) {
		if(!util_match_word_in_sentence(aWords[i], pszSentence)) {
			bAllFound = FALSE;
			break;
		}
	}
	g_strfreev(aWords);
	return bAllFound;
}

// This is a fairly slow function... perhaps too slow for huge lists?
gboolean util_treeview_match_all_words_callback(GtkTreeModel *pTreeModel, gint nColumn, const gchar *pszSearchText, GtkTreeIter* pIter)
{
	gboolean bMatch = FALSE;

	// Get the row text from the treeview
	gchar* pszRowText = NULL;
	gtk_tree_model_get(pTreeModel, pIter, nColumn, &pszRowText, -1);	// -1 because it's NULL terminated

	// Strip markup from tree view row
	gchar* pszRowTextClean = NULL;
	if(pango_parse_markup(pszRowText, -1, 0, NULL, &pszRowTextClean, NULL, NULL)) {
		// put both strings into lowercase
		gchar* pszRowTextCleanDown = g_utf8_casefold(pszRowTextClean, -1);	// -1 because it's NULL terminated
		gchar* pszSearchTextDown = g_utf8_casefold(pszSearchText, -1);

		bMatch = util_match_all_words_in_sentence(pszSearchTextDown, pszRowTextCleanDown);

		g_free(pszRowTextClean);
		g_free(pszRowTextCleanDown);
		g_free(pszSearchTextDown);
	}
	else {
		g_warning("pango_parse_markup failed on '%s'", pszRowText);
		// bMatch remains FALSE...
	}
	g_free(pszRowText);

	return (bMatch == FALSE);	// NOTE: we must return FALSE for matches... yeah, believe it.
}

//******************************* creation des models pour toutes les fenetres aide
GtkWidget * create_view_and_model (char clef[50])
{
  GtkTreeViewColumn *col;
  GtkCellRenderer *renderer;
  GtkWidget *view;
  GtkTreeModel *model=NULL;

  view = gtk_tree_view_new();
	gtk_tree_view_set_enable_search(GTK_TREE_VIEW(view),FALSE);
	gtk_tree_view_set_search_equal_func(GTK_TREE_VIEW(view),(GtkTreeViewSearchEqualFunc) util_treeview_match_all_words_callback, NULL, NULL);
	gtk_tree_view_expand_all (GTK_TREE_VIEW(view));

  col = gtk_tree_view_column_new();
  gtk_tree_view_column_set_title(col, (_("Aide à la programmation [Ctl+F] pour rechercher")));
  gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);

  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(col, renderer, TRUE);
  gtk_tree_view_column_add_attribute(col, renderer, 
      "text", COLUMN);	

 if (strcmp("php", clef) == 0){model = create_and_fill_model_php();}
 if (strcmp("bash", clef) == 0){model = create_and_fill_model_bash();}
 if (strcmp("perl", clef) == 0){model = create_and_fill_model_perl();}
 if (strcmp("mysql", clef) == 0){model = create_and_fill_model_mysql();}
 if (strcmp("html", clef) == 0){model = create_and_fill_model_html();}
 if (strcmp("css", clef) == 0){model = create_and_fill_model_css();}
 if (strcmp("javascript", clef) == 0){model = create_and_fill_model_javascript();}
 if (strcmp("htaccess", clef) == 0){model = create_and_fill_model_htaccess();}
 if (strcmp("term", clef) == 0){model = create_and_fill_model_term();}
 /*else{   
	gtk_notebook_set_current_page(notebook3,1);
	}*/
  gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);
  g_object_unref(model); 

  return view;
}

//******************************* fenetre aide php
GtkWidget* help_php_window (void)

{

	gtk_widget_destroy(window1);
  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window1), _((_("Aide Php"))));
    gtk_window_set_transient_for(GTK_WINDOW(window1),GTK_WINDOW(tea_main_window));
	gtk_window_resize (GTK_WINDOW (window1), 430, 600);
  gtk_widget_show (GTK_WIDGET(window1));

  GtkWidget *view;
  GtkWidget *vbox;
  GtkWidget *statusbar;

  vbox =gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(window1), vbox);
	gtk_widget_show (GTK_WIDGET(vbox));

    GtkWidget *scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	  gtk_widget_show (GTK_WIDGET(scrolledWindow));
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	  gtk_box_pack_start(GTK_BOX(vbox), scrolledWindow, TRUE, TRUE, 1);

  view = create_view_and_model("php");
  gtk_widget_show (GTK_WIDGET(view));
  selection2 = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
	gtk_widget_show (GTK_WIDGET(selection2));
    gtk_container_add(GTK_CONTAINER(scrolledWindow), GTK_WIDGET(view));

  statusbar = gtk_statusbar_new();
  gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, TRUE, 1);
	gtk_widget_show (GTK_WIDGET(statusbar));
	
	gtk_tree_view_set_enable_tree_lines (GTK_TREE_VIEW(view),TRUE);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(view),TRUE);

	gtk_tree_view_expand_all (GTK_TREE_VIEW(view));
	 g_signal_connect(G_OBJECT(view), "button-release-event",  
      G_CALLBACK(on_changed2), statusbar);
	
	gtk_widget_grab_focus (view);

  return window1;

}

//******************************* fenetre aide mysql
GtkWidget* help_mysql (void)

{

	gtk_widget_destroy(window1);
  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window1), _((_("Aide MySql"))));
    gtk_window_set_transient_for(GTK_WINDOW(window1),GTK_WINDOW(tea_main_window));
	gtk_window_resize (GTK_WINDOW (window1), 430, 600);
  gtk_widget_show (GTK_WIDGET(window1));

  GtkWidget *view;
  GtkWidget *vbox;
  GtkWidget *statusbar;

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(window1), vbox);
	gtk_widget_show (GTK_WIDGET(vbox));

    GtkWidget *scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	  gtk_widget_show (GTK_WIDGET(scrolledWindow));
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	  gtk_box_pack_start(GTK_BOX(vbox), scrolledWindow, TRUE, TRUE, 1);

  view = create_view_and_model("mysql");
  gtk_widget_show (GTK_WIDGET(view));
  selection2 = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
	gtk_widget_show (GTK_WIDGET(selection2));
	gtk_container_add(GTK_CONTAINER(scrolledWindow), GTK_WIDGET(view));

  statusbar = gtk_statusbar_new();
  gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, TRUE, 1);
	gtk_widget_show (GTK_WIDGET(statusbar));
	
	gtk_tree_view_set_enable_tree_lines (GTK_TREE_VIEW(view),TRUE);
	gtk_tree_view_expand_all (GTK_TREE_VIEW(view));
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(view),TRUE);

	 g_signal_connect(G_OBJECT(view), "button-release-event",  
      G_CALLBACK(on_changed2), statusbar);

	gtk_widget_grab_focus (view);

  return window1;

}

//******************************* fenetre aide html
GtkWidget* help_html (void)

{

	gtk_widget_destroy(window1);
  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window1), _((_("Aide HTML"))));
    gtk_window_set_transient_for(GTK_WINDOW(window1),GTK_WINDOW(tea_main_window));
	gtk_window_resize (GTK_WINDOW (window1), 430, 600);
  gtk_widget_show (GTK_WIDGET(window1));

  GtkWidget *view;
  GtkWidget *vbox;
  GtkWidget *statusbar;

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(window1), vbox);
	gtk_widget_show (GTK_WIDGET(vbox));

    GtkWidget *scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	  gtk_widget_show (GTK_WIDGET(scrolledWindow));
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	  gtk_box_pack_start(GTK_BOX(vbox), scrolledWindow, TRUE, TRUE, 1);

  view = create_view_and_model("html");
  gtk_widget_show (GTK_WIDGET(view));
  selection2 = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
	gtk_widget_show (GTK_WIDGET(selection2));
	gtk_container_add(GTK_CONTAINER(scrolledWindow), GTK_WIDGET(view));

  statusbar = gtk_statusbar_new();
  gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, TRUE, 1);
	gtk_widget_show (GTK_WIDGET(statusbar));
	
	gtk_tree_view_set_enable_tree_lines (GTK_TREE_VIEW(view),TRUE);
	gtk_tree_view_expand_all (GTK_TREE_VIEW(view));
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(view),TRUE);

	 g_signal_connect(G_OBJECT(view), "button-release-event",  
      G_CALLBACK(on_changed2), statusbar);

	gtk_widget_grab_focus (view);

  return window1;

}

//******************************* fenetre aide html
GtkWidget* help_css (void)

{
	gtk_widget_destroy(window1);
  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window1), _((_("Aide CSS"))));
    gtk_window_set_transient_for(GTK_WINDOW(window1),GTK_WINDOW(tea_main_window));
	gtk_window_resize (GTK_WINDOW (window1), 430, 600);
  gtk_widget_show (GTK_WIDGET(window1));

  GtkWidget *view;
  GtkWidget *vbox;
  GtkWidget *statusbar;

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(window1), vbox);
	gtk_widget_show (GTK_WIDGET(vbox));

    GtkWidget *scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	  gtk_widget_show (GTK_WIDGET(scrolledWindow));
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	  gtk_box_pack_start(GTK_BOX(vbox), scrolledWindow, TRUE, TRUE, 1);

  view = create_view_and_model("css");
  gtk_widget_show (GTK_WIDGET(view));
  selection2 = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
	gtk_widget_show (GTK_WIDGET(selection2));
	gtk_container_add(GTK_CONTAINER(scrolledWindow), GTK_WIDGET(view));

  statusbar = gtk_statusbar_new();
  gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, TRUE, 1);
	gtk_widget_show (GTK_WIDGET(statusbar));
	
	gtk_tree_view_set_enable_tree_lines (GTK_TREE_VIEW(view),TRUE);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(view),TRUE);

	gtk_tree_view_expand_all (GTK_TREE_VIEW(view));

	 g_signal_connect(G_OBJECT(view), "button-release-event",  
      G_CALLBACK(on_changed2), statusbar);

	gtk_widget_grab_focus (view);

  return window1;

}

//******************************* fenetre aide javascript
GtkWidget* help_javascript (void)

{

	gtk_widget_destroy(window1);
  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window1), _((_("Aide Javascript Jquery"))));
    gtk_window_set_transient_for(GTK_WINDOW(window1),GTK_WINDOW(tea_main_window));
	gtk_window_resize (GTK_WINDOW (window1), 430, 600);
  gtk_widget_show (GTK_WIDGET(window1));

  GtkWidget *view;
  GtkWidget *vbox;
  GtkWidget *statusbar;

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(window1), vbox);
	gtk_widget_show (GTK_WIDGET(vbox));

    GtkWidget *scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	  gtk_widget_show (GTK_WIDGET(scrolledWindow));
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	  gtk_box_pack_start(GTK_BOX(vbox), scrolledWindow, TRUE, TRUE, 1);

  view = create_view_and_model("javascript");
  gtk_widget_show (GTK_WIDGET(view));
  selection2 = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
	gtk_widget_show (GTK_WIDGET(selection2));
	gtk_container_add(GTK_CONTAINER(scrolledWindow), GTK_WIDGET(view));

  statusbar = gtk_statusbar_new();
  gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, TRUE, 1);
	gtk_widget_show (GTK_WIDGET(statusbar));
	
	gtk_tree_view_set_enable_tree_lines (GTK_TREE_VIEW(view),TRUE);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(view),TRUE);

	gtk_tree_view_expand_all (GTK_TREE_VIEW(view));

	 g_signal_connect(G_OBJECT(view), "button-release-event",  
      G_CALLBACK(on_changed2), statusbar);

	gtk_widget_grab_focus (view);

  return window1;

}

//******************************* fenetre aide bash
GtkWidget* Aide_BASH (void)

{

	gtk_widget_destroy(window1);
  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window1), _((_("Help for BASH"))));
    gtk_window_set_transient_for(GTK_WINDOW(window1),GTK_WINDOW(tea_main_window));
	gtk_window_resize (GTK_WINDOW (window1), 430, 600);
  gtk_widget_show (GTK_WIDGET(window1));

  GtkWidget *view;
  GtkWidget *vbox;
  GtkWidget *statusbar;

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(window1), vbox);
	gtk_widget_show (GTK_WIDGET(vbox));

    GtkWidget *scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	  gtk_widget_show (GTK_WIDGET(scrolledWindow));
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	  gtk_box_pack_start(GTK_BOX(vbox), scrolledWindow, TRUE, TRUE, 1);

  view = create_view_and_model("bash");
  gtk_widget_show (GTK_WIDGET(view));
  selection2 = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
	gtk_widget_show (GTK_WIDGET(selection2));
	gtk_container_add(GTK_CONTAINER(scrolledWindow), GTK_WIDGET(view));

  statusbar = gtk_statusbar_new();
  gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, TRUE, 1);
	gtk_widget_show (GTK_WIDGET(statusbar));

	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(view),TRUE);	
	gtk_tree_view_set_enable_tree_lines (GTK_TREE_VIEW(view),TRUE);
	gtk_tree_view_expand_all (GTK_TREE_VIEW(view));
	 g_signal_connect(G_OBJECT(view), "button-release-event",  
      G_CALLBACK(on_changed2), statusbar);

	gtk_widget_grab_focus (view);

  return window1;

}

//******************************* fenetre aide perl
GtkWidget* centre_perl (void)

{
	gtk_widget_destroy(window1);

  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window1), _((_("Help for Perl"))));
    gtk_window_set_transient_for(GTK_WINDOW(window1),GTK_WINDOW(tea_main_window));
	gtk_window_resize (GTK_WINDOW (window1), 430, 600);
  gtk_widget_show (GTK_WIDGET(window1));

  GtkWidget *view;
  GtkWidget *vbox;
  GtkWidget *statusbar;

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(window1), vbox);
	gtk_widget_show (GTK_WIDGET(vbox));

    GtkWidget *scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	  gtk_widget_show (GTK_WIDGET(scrolledWindow));
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	  gtk_box_pack_start(GTK_BOX(vbox), scrolledWindow, TRUE, TRUE, 1);

  view = create_view_and_model("perl");
  gtk_widget_show (GTK_WIDGET(view));
  selection2 = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
	gtk_widget_show (GTK_WIDGET(selection2));
	gtk_container_add(GTK_CONTAINER(scrolledWindow), GTK_WIDGET(view));

  statusbar = gtk_statusbar_new();
  gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, TRUE, 1);
	gtk_widget_show (GTK_WIDGET(statusbar));

	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(view),TRUE);
	gtk_tree_view_set_enable_tree_lines (GTK_TREE_VIEW(view),TRUE);	
	gtk_tree_view_expand_all (GTK_TREE_VIEW(view));
	

	 g_signal_connect(G_OBJECT(view), "button-release-event",  
      G_CALLBACK(on_changed2), statusbar);

	gtk_widget_grab_focus (view);

  return window1;

}

//******************************* fenetre aide htaccess
GtkWidget* centre_htaccess (void)

{
	gtk_widget_destroy(window1);

  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window1), _((_("Aide Htaccess"))));
    gtk_window_set_transient_for(GTK_WINDOW(window1),GTK_WINDOW(tea_main_window));
	gtk_window_resize (GTK_WINDOW (window1), 430, 600);
  gtk_widget_show (GTK_WIDGET(window1));

  GtkWidget *view;
  GtkWidget *vbox;
  GtkWidget *statusbar;

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(window1), vbox);
	gtk_widget_show (GTK_WIDGET(vbox));

    GtkWidget *scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	  gtk_widget_show (GTK_WIDGET(scrolledWindow));
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	  gtk_box_pack_start(GTK_BOX(vbox), scrolledWindow, TRUE, TRUE, 1);

  view = create_view_and_model("htaccess");
  gtk_widget_show (GTK_WIDGET(view));
  selection2 = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
	gtk_widget_show (GTK_WIDGET(selection2));
	gtk_container_add(GTK_CONTAINER(scrolledWindow), GTK_WIDGET(view));

  statusbar = gtk_statusbar_new();
  gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, TRUE, 1);
	gtk_widget_show (GTK_WIDGET(statusbar));

	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(view),TRUE);
	gtk_tree_view_set_enable_tree_lines (GTK_TREE_VIEW(view),TRUE);	
	gtk_tree_view_expand_all (GTK_TREE_VIEW(view));
	

	 g_signal_connect(G_OBJECT(view), "button-release-event",  
      G_CALLBACK(on_changed2), NULL);

	gtk_widget_grab_focus (view);

  return window1;

}

//******************************* template tree aide term
GtkTreeModel *create_and_fill_model_term (void)
{
  GtkTreeStore *treestore;
  GtkTreeIter toplevel, child;

  treestore = gtk_tree_store_new(NUM_COLS,
                  G_TYPE_STRING);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, "1 Base [Term]",
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.1 (term) : Utilisation du disque")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.2 (term) : Aspiration de site web WGET")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.3 (term) : RM sur un grand nombre de fichiers")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.4 (term) : FIND trouver un fichier par son nom")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.5 (term) : SCREEN Lancer un screen ou le récupérer")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.6 (term) : WC Nombre de lignes pour sortie ou fichier")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.7 (term) : WGET récupération de page web")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.8 (term) : Lister les fichiers et répertoire en couleur")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.9 (term) : NICE Lancer un script ou commande lourde avec une priorité basse")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.0 (term) : Compression avec tar.gz")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.1 (term) : Décompression avec tar.gz")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.2 (term) :  Différence entre deux fichiers")),-1);

  return GTK_TREE_MODEL(treestore);
}


//************** VTE HELP
void term_help(gpointer user_data)
{
	gtk_widget_destroy(menu_vte);
	gtk_widget_destroy(window1);

  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window1), _((_("Aide Htaccess"))));
    gtk_window_set_transient_for(GTK_WINDOW(window1),GTK_WINDOW(tea_main_window));
	gtk_window_resize (GTK_WINDOW (window1), 430, 600);
  gtk_widget_show (GTK_WIDGET(window1));

  GtkWidget *view;
  GtkWidget *vbox;
  GtkWidget *statusbar;

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(window1), vbox);
	gtk_widget_show (GTK_WIDGET(vbox));

    GtkWidget *scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	  gtk_widget_show (GTK_WIDGET(scrolledWindow));
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	  gtk_box_pack_start(GTK_BOX(vbox), scrolledWindow, TRUE, TRUE, 1);

  view = create_view_and_model("term");
  gtk_widget_show (GTK_WIDGET(view));
  selection2 = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
	gtk_widget_show (GTK_WIDGET(selection2));
	gtk_container_add(GTK_CONTAINER(scrolledWindow), GTK_WIDGET(view));

  statusbar = gtk_statusbar_new();
  gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, TRUE, 1);
	gtk_widget_show (GTK_WIDGET(statusbar));

	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(view),TRUE);
	gtk_tree_view_set_enable_tree_lines (GTK_TREE_VIEW(view),TRUE);	
	gtk_tree_view_expand_all (GTK_TREE_VIEW(view));
	
	 g_signal_connect(view, "button-release-event",  
      G_CALLBACK(on_changed2), user_data);

	gtk_widget_grab_focus (view);

  return ;
}

