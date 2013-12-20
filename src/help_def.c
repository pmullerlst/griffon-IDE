#ifdef HAVE_CONFIG_H
#  include "config.h"
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
                     COLUMN, "1 Functions base [PHP]",
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.1 (php) to start a PHP script")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.2 (php) echo: Displays a character string")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.3 (php) add a comment")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.4 (php) FUNCTION: create a function")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.5 (php) MAIL: send an email")),-1);  
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.6 (php) DATE format MySql YYYY-mm-jj hh:mm:ss")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.7 (php) RAND: Generate a random value")),-1);  
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.8 (php) POST: Display the POST variables")),-1); 
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.9 (php) COUNT : Account all the elements of an array")),-1);

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
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.4 (php) FOREACH: read a table row by row")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, "3 File",
                     -1);
                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.1 (php) FILE: place the contents of a file (or url) in a table")),-1);        
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.2 (php) UNLINK : delete a file")),-1);  
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.3 (php) FPUTS : write to a file")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.4 (php) DIR : List the files in a directory in a table")),-1);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("4 String")),
                     -1);
                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.1 (php) SPLIT: split a string with delimiter")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.2 (php) REPLACEMENT: find a string in a variable and replace")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.3 (php) IF string : if search for a string in another string")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.4 (php) stripslashes: Removes backslashes in a string")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.5 (php) STRLEN: Returns the size of the string")),-1);  
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.6 (php) urlencode: Encodes a URL string")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.7 (php) mb_substr_count: Counts the number of occurrences of a substring")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.8 (php) strip_tags: Removes HTML and PHP tags from a string")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.9 (php) EXPLODE : split a string with separator and returns an array")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.10 (php) ADDSLASHES : Adds backslashes in a string")),-1);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, "5 MySql",
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.1 (php) connection to a MySql database")),-1);                         
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.2 (php) LOOP SELECT: mysql select with recuperation loop data")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.3 (php) QUERY: simple query MySQL (UPDATE, DELETE, INSERT)")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.4 (php) QUERY: simple query MySQL (SELECT)")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.5 (php) LAST_INSERT_ID: recuperation after an INSERT id")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.6 (php) NUM_ROWS: number of results of a query")),-1);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, "6 FTP",
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.1 (php) connection to an FTP server")),-1); 
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.2 (php) ftp_nlist: list files in a directory")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.3 (php) ftp_put: upload a file")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.4 (php) FTP_DELETE: Delete a file")),-1);
 
  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, "7 Header",
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "7.1 (php) TEXT UTF8",-1); 
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "7.2 (php) IMAGE PNG",-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "7.3 (php) Download a CSV file",-1); 
                                                                              
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
                     COLUMN, (_("1 Base of page [HTML]")),
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.1 (html): Basic Template")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.2 (html): Comment")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.3 (html): Table [Option CMD: \"{NBR_TR} - {NBR_TD}\"]")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.4 (html): HREF link around the selection")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.5 (html): Integration of a css stylesheet")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.6 (html): Reload (refresh) of a page every 30 seconds")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.7 (html): Tag image around the selection")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.8 (html): Add a div around the selection")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.9 (html): add a style to a tag")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.10 (html): Add a favicon 16x16. Ico format")),-1);
       
  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("2 Size")),
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.1 (html) : [width]")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.2 (html) : [height]")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.3 (html): [cols] number of columns (textarea)")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.4 (html) : [rows] nombre de ligne (textarea)")),-1);
     
  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("3 Form")),
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.1 (html): Basic Form")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.2 (html) text input area")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.3 (html): textarea input field")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.4 (html): Zone option selection (SELECT)")),-1);


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
                     COLUMN, (_("1 Base Style Sheet [CSS]")),
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.1 (css) Comment")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.2 (css): Style for body tag")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.3 (css): Style for link")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.4 (css): Style for link hover")),-1);    
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.5 (css): Style for div tag with ID")),-1);  
    
  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, "2 Base background",
                     -1);  
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.1 (css) [background] background color")),-1);                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.2 (css) [background-image] Background Image")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.3 (css) [background-repeat] Option not repeated background image")),-1);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, "3 Base font",
                     -1);  
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.1 (css) : [font-family]")),-1); 
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.2 (css) : [font-size] ")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.3 (css) : [color] font color")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.4 (css) [decoration] No Text Decoration")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.5 (css) [decoration] Underline Text")),-1);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("4 Base size")),
                     -1);  
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.1 (css) : [width] width")),-1); 
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.2 (css) : [height] height")),-1); 

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("5 Base position")),
                     -1);  
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.1 (css) [position] on the relative positioning position of the previous")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.2 (css) [position] absolute absolute positioning relative to the corner upper left of the document")),-1);   
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.3 (css) [position] fixed fixed positioning relative to the corner top left of the browser window")),-1);                       
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.4 (css): [left] left Position from the left edge")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.5 (css) [right] right position at the right")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.6 (css) [top] top position at the top")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.7 (css): [low] bottom position to the bottom edge")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.8 (css) [valign] vertical alignment down bottom")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.9 (css) [valign] top vertical alignment")),-1);
  
  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("6 Base Display")),
                     -1);  
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.1 (css): [none] The display element is hidden")),-1);   
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.2 (css) [block] display display one below the other")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.3 (css) [inline] View online display div")),-1);  
                                                    
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
                     COLUMN, (_("1 Base [MYSQL]")),
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
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.1 (javascript) HTML tag for javascript / jquery")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.2 (javascript) : Create a function")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.3 (javascript) : Split a string into array by separating")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.4 (javascript) : Encoding a URL")),-1);
     
  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("2 Interaction with ID")),
                     -1);
 
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.1 (javascript) : Recovery of a value by id")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.2 (javascript) : Recovery of value by id in a HTML select")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.3 (javascript) : Focus on an id")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.4 (javascript) : Upload a file php / html in a div id")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.5 (javascript) : Replace the content of a div id with another text")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.6 (javascript): Replace the contents of an html id value in another text")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.7 (javascript): Add the contents of a text html id value")),-1);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("3 List of events")),
                     -1);
 
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.1 (javascript): [onBlur] output action focus")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.2 (javascript): [onChange] action for change")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.3 (javascript): [onClick] action for a click")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.4 (javascript): [onFocus] taking action focus")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.5 (javascript): [onLoad] action after loading")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.6 (javascript): [onMouseOver] action mouseover")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.7 (javascript): [onSelect] action for selection")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.8 (javascript): [onSubmit] Action Form Validation")),-1);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("4 Function")),
                     -1);
 
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.1 (JavaScript) function retrieve the content of a file")),-1);

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
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.1 (htaccess) : Default charset (UTF-8)")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.2 (htaccess): block all IP")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.3 (htaccess) : Allow IP")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.4 (htaccess) : Redirect errors")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.5 (htaccess) : Enable URL rewriting")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.6 (htaccess) : online simple example url rewriting")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.7 (htaccess) : Example line with regular expression option and url rewriting")),-1);

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
                     COLUMN, (_("1 Functions base [PERL]")),
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.1 (perl): Start a script")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.2 (perl) PRINT: Display a string")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);  
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.3 (perl) ARGUMENTS: use the program arguments")),-1);


	  gtk_tree_store_append(treestore, &child2, &toplevel);  
	  gtk_tree_store_set(treestore, &child2,COLUMN, (_("ARRAY")),-1);
		  gtk_tree_store_append(treestore, &child, &child2);    
		  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.4 (perl) array : declaring an array")),-1);  
		  gtk_tree_store_append(treestore, &child, &child2);
		  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.5 (perl) array : stack value at end of table")),-1);
		  gtk_tree_store_append(treestore, &child, &child2);
		  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.6 (perl) array : stacking a array value at the beginning of")),-1);  
		  gtk_tree_store_append(treestore, &child, &child2);
		  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.7 (perl) FOREACH: read a array row by row")),-1);  

  gtk_tree_store_append(treestore, &child, &toplevel);  
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.8 (perl) ENTRY: expects keyboard input")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);  
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.9 (perl) FUNCTION: create a function")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);  
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.10 (perl) EXIT: exit (killer) program")),-1);    
  gtk_tree_store_append(treestore, &child, &toplevel);  
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.11 (perl) LOCALTIME: Recovery from the date")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);  
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.12 (perl) RAND: random number generation")),-1);
  
  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("2 Instructions")),
                     -1);    

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, "2.1 (perl) IF",-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.2 (perl) UNLESS: negative test")),-1);  
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.3 (perl) IF ELSE")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.4 (perl) WHILE")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.5 (perl) WHILE infinite")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.6 (perl) FOREACH: read a array row by row")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("3 file read write")),
                     -1);                    
                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.1 (perl) Read a file line by line")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.2 (perl) to open a file for appending data")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.3 (perl) to open a file for appending data and overwrite the existing")),-1);    
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.4 (perl) open a pipe to a file")),-1);  
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.5 (perl) STDIN reading from standard input")),-1); 
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.6 (perl) STAT : recovery stats of a file  ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks)")),-1); 

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("4 Chaîne de carctères")),
                     -1);
                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.1 (perl) SPLIT: split a string with delimiter")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.2 (perl) IF UNLESS chain: education search for a string in a other string")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.3 (perl) REPLACEMENT: find a string in a variable and replace")),-1);    
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.4 (perl) CHOP: Removes the last character of the string")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.5 (perl) LENGTH: Returns the length of the string (number of characters)")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.6 (perl) CPU: Returns the string to uppercase")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.7 (perl) LC: Returns the string to lowercase")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.8 (perl) ucfirst: Returns the string with the first character uppercase")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("4.9 (perl) SUBSTR: returns a string contained between the start index and length")),-1);
          
  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, "5 Lib DBI MySql",
                     -1);
                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.1 (lib_dbi) CONNECTION: Connect to MySql database")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.2 (lib_dbi) QUERY: simple query MySQL (UPDATE, DELETE, INSERT)")),-1); 
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.3 (lib_dbi) QUERY: simple query MySQL (SELECT)")),-1);   
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.4 (lib_dbi) LOOP SELECT: mysql select with recuperation of loop data")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.5 (lib dbi) LAST ID: recuperation of the last insert id in")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.6 (lib_dbi) ROWS: rows recovery number of results for a query")),-1);


  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, "6 Lib MIME Lite",
                     -1);
                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.1 (lib_mime) Sending a simple email")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.2 (lib_mime) Sending mail with file")),-1);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("7 HTTP Lib URI and LWP")),
                     -1);
                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("7.1 (lib_URI) encoding a URL")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("7.2 (lib_LWP) get a URL")),-1);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, "8 FTP Lib NET::FTP",
                     -1);
                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("8.1 (lib_net) connection to an FTP server")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("8.2 (lib_net) list the contents of an FTP directory")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("8.3 (lib_net) GET to download a file from a FTP")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("8.4 (lib_net) PUT upload a file on an FTP")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("8.5 (lib_net) CWD Change directory")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("8.6 (lib_net) DELETE Delete a file")),-1);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, "9 SOCKET IO::Socket::INET",
                     -1);
                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("9.1 (lib_socket) socket in listening mode (server) with fork")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("9.2 (lib_socket) socket client mode")),-1);

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
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
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.1 (bash) Start a script")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.2 (bash) ECHO: Display a string")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.3 (bash) Add a comment")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.4 (bash) command in the background")),-1);
                     
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
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.4 (bash) CASE: multiple choice")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.5 (bash) READ: interactivity with the user")),-1);
 
  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("3 Logical Operator")),
                     -1);                     

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.1 (bash) AND")),-1);                                                            
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("3.2 (bash) OR")),-1);
 
 
  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("4 Basic Commands")),
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
                     COLUMN, (_("5 File Redirection")),
                     -1);
                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.1 (bash) Output Redirection in the NULL")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.2 (bash) The output overwrites a file")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.3 (bash) is added to the output file")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.4 (bash) Redirecting errors")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("5.5 (bash) Redirection errors and messages")),-1);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, (_("6 Building [TEST]")),
                     -1);
                     
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.1 (bash) TEST: if a file exists")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.2 (bash) TEST: if a directory exists")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.3 (bash) TEST: Reverse test")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.4 (bash) TEST: right reading")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.5 (bash) TEST: write permission")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.6 (bash) TEST:  execution?")),-1);                                                                                                                                                                                                                                                                 
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN,(_("6.7 (bash) TEST: two variables")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("6.8 (bash) TEST: two-digit")),-1);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
  return GTK_TREE_MODEL(treestore);
}

//******************************* match et envoi des fonction d'aide insert
void  on_changed(GtkWidget *widget, gpointer statusbar) 
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  char *value;

 if (! get_page_text()){ log_to_memo (_("You must open a file to use for help."), NULL, LM_ERROR);return;}

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
				if (strcmp("[IPTABLES]", value) == 0){centre_iptables();}
                                                        
	 gtk_tree_selection_unselect_all(GTK_TREE_SELECTION(widget));                                        
    g_free(value);
  }
	
}


//******************************* match et envoi des fonction d'aide insert
void  on_changed2(GtkWidget *tt, GdkEvent *eventt, gpointer *user_data) 
{
	if(eventt==NULL){printf("\n");}
	gtk_widget_get_name(tt);

  GtkTreeIter iter;
  GtkTreeModel *model;
  char *value;

  if (gtk_tree_selection_get_selected(
      GTK_TREE_SELECTION(selection2), &model, &iter)) {

		
    gtk_tree_model_get(model, &iter, COLUMN, &value,  -1);
      
				gchar **a = g_strsplit (value, ")", -1);   

		if(a[1]!=NULL)
		{

				gchar **a2 = g_strsplit (a[0], "(", -1);                      

            //*************** APPEL AU FONCTION IPTABLE
				if (strcmp("1.1 (iptables", a[0]) == 0){iptables_drop_all();return;}
				if (strcmp("1.2 (iptables", a[0]) == 0){iptables_accept_lo();return;}
				if (strcmp("1.3 (iptables", a[0]) == 0){iptables_accept_port();return;}
				if (strcmp("1.4 (iptables", a[0]) == 0){iptables_accept_port_ip();return;}
				if (strcmp("1.5 (iptables", a[0]) == 0){iptables_log();return;}
				if (strcmp("1.6 (iptables", a[0]) == 0){iptables_clean();return;}

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
				if (strcmp("2.3 (term", a[0]) == 0){term_git_status(user_data);return;}
				if (strcmp("2.4 (term", a[0]) == 0){term_git_commit(user_data);return;}
				if (strcmp("2.5 (term", a[0]) == 0){term_git_push(user_data);return;}
				if (strcmp("2.6 (term", a[0]) == 0){term_iptables_drop(user_data);return;}
				if (strcmp("2.7 (term", a[0]) == 0){term_iptables_clean(user_data);return;}

			 if (! get_page_text()){ log_to_memo (_("You must open a file to use for help."), NULL, LM_ERROR);return;}

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
            if (strcmp("4.4 (bash", a[0]) == 0){}
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
  gtk_tree_view_column_set_title(col, (_("Programming support [Ctl + F] to search")));
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
 if (strcmp("iptables", clef) == 0){model = create_and_fill_model_iptables();}
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
  gtk_window_set_title (GTK_WINDOW (window1), _((_("Help Php"))));
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
  gtk_window_set_title (GTK_WINDOW (window1), _((_("Help Mysql"))));
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
  gtk_window_set_title (GTK_WINDOW (window1), _((_("Help HTML"))));
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
  gtk_window_set_title (GTK_WINDOW (window1), _((_("Help CSS"))));
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
  gtk_window_set_title (GTK_WINDOW (window1), _((_("Help Javascript Jquery"))));
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

//******************************* fenetre aide iptables
GtkWidget* centre_iptables (void)
{
	gtk_widget_destroy(window1);

  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window1), _((_("Aide IPTABLES"))));
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

  view = create_view_and_model("iptables");
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
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.1 (term) : Disk Usage")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.2 (term) : Extractors website WGET")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.3 (term) : RM on a large number of files")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.4 (term) : FIND find a file by name")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.5 (term) : SCREEN Launch a screen or recover")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.6 (term) : WC number of lines to output or file")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.7 (term) : WGET download web page")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.8 (term) : List files and directories in color")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.9 (term) : NICE Launch a script or command heavy with low priority")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.0 (term) : Compression with tar.gz")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.1 (term) : Unzip with tar.gz")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.2 (term) : Difference between two files")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.3 (term) : Git status")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.4 (term) : Git commit")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.5 (term) : Git push")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.6 (term) : IPTABLES DROP IP")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("2.7 (term) : IPTABLES cleaning rules")),-1);
  return GTK_TREE_MODEL(treestore);
}

//******************************* template tree aide IPTABLES
GtkTreeModel *create_and_fill_model_iptables (void)
{
  GtkTreeStore *treestore;
  GtkTreeIter toplevel, child;

  treestore = gtk_tree_store_new(NUM_COLS,
                  G_TYPE_STRING);

  gtk_tree_store_append(treestore, &toplevel, NULL);
  gtk_tree_store_set(treestore, &toplevel,
                     COLUMN, "1 Base [Iptables]",
                     -1);

  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.1 (iptables) : DROP ALL")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.2 (iptables) : ACCEPT loop back (ping)")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.3 (iptables) : ACCEPT port (port=1000)")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.4 (iptables) : ACCEPT port/IP (port=1000 IP=192.192.192.1)")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.5 (iptables) : Sys LOG iptables")),-1);
  gtk_tree_store_append(treestore, &child, &toplevel);
  gtk_tree_store_set(treestore, &child,COLUMN, (_("1.6 (iptables) : IPTABLES cleaning rules")),-1);

  return GTK_TREE_MODEL(treestore);
}

//************** VTE HELP
void term_help(GtkWidget *tv,GdkEventButton *event,  gpointer user_data)
{
		if(event==NULL){printf("\n");}
		gtk_widget_get_name(tv);
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

