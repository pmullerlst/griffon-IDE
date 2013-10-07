
#include <gtk/gtk.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdio.h>
#include <libintl.h>
#define _(String) gettext (String)
/* Stuff needed by stat to access file dates */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguage.h>

#include "interface.h"
#include "callbacks.h"
#include "griffon_config.h"

/* Interface refresh interface */

#define REFRESH_INTERVAL 15

typedef struct {
   GtkWidget *widget;
   gboolean is_combo;
} typed_text_entry;


static GArray *column_names, *column_types, *text_entries;
static GType *column_gtk_types;
static gint num_columns;
static gchar *minido_db, *current_row_file_name;
static GHashTable *column_values, *column_default_values, *file_times;
static GtkListStore *model;
static GtkWidget *top_window, *edit_button, *add_button, *remove_button;
static gulong add_signal_handler, edit_signal_handler, remove_signal_handler;

static block_change_signals (void)
{
  /* Used to disable change (add / edit / remove) signals,
   * because the corresponding callbacks use global variables
   * for their communication
   */
   g_signal_handler_block (GTK_BUTTON (add_button), add_signal_handler);
   g_signal_handler_block (GTK_BUTTON (edit_button), edit_signal_handler);
   g_signal_handler_block (GTK_BUTTON (remove_button), remove_signal_handler);
}

static unblock_change_signals (void)
{
   g_signal_handler_unblock (GTK_BUTTON (add_button), add_signal_handler);
   g_signal_handler_unblock (GTK_BUTTON (edit_button), edit_signal_handler);
   g_signal_handler_unblock (GTK_BUTTON (remove_button), remove_signal_handler);
}

static gint get_file_time (gchar *file_path)
{
   struct stat sts;
   int seconds;

   if (stat (file_path, &sts) != 0) 
   {
      g_printerr (_("Can't get file time for `%s'\n"), file_path);
   }

   seconds = sts.st_mtime;
   return (seconds);
}

static gint get_row_file_time (gchar *file_name)
{
   return get_file_time (g_build_filename (minido_db, file_name, NULL));
}

static void declare_columns (void)
{
   xmlDocPtr xml_data;
   xmlNodePtr root, node, subnode, subsubnode;
   GList *values;
   gchar *name, *value, *default_value, *type, *minido_profile;

	gchar *home_conf="";
	minido_profile="";

	minido_profile=confile.tea_todo;

   if (minido_profile == NULL)
   {
	   error_dialog (_("MINIDO_PROFILE variable pour la todo liste n'est pas trouvable (normalement /home/user/.griffon/griffon_todo)\n"));
   } 

   xml_data = xmlParseFile(minido_profile);

   column_names = g_array_new (FALSE, FALSE, sizeof (gchar*));
   column_types = g_array_new (FALSE, FALSE, sizeof (gchar*));
   column_default_values = g_hash_table_new (&g_str_hash, &g_str_equal); 
   column_values = g_hash_table_new (&g_str_hash, &g_str_equal); 



   /* Check that the root node is named 'columns' */

   root = xml_data->children;

   if (strcmp(root->name, "columns")!=0)
   {
      error_dialog (g_strdup_printf (_("Le tag xml %s est invalide dans le fichier : /home/user/.griffon/griffon_todo\n"), root->name));
      /* Should report line number too */
   }

   /* 1st column: keep track of file name */

   name = g_strdup ("MINIDO_ROW_ID");
   /*CAUTION: real type may be needed */
   type = g_strdup ("string");

   g_array_append_vals (column_names, &name, 1);
   g_array_append_vals (column_types, &type, 1);

   /* Read regular column info from column file */

   for (node = xml_data->children->children;node !=NULL;node=node->next)
   {
      if (strcmp(node->name, "column")==0)
      {
	 name = xmlGetProp (node, "name");
	 type = xmlGetProp (node, "type");
	 values = NULL;

   	 g_array_append_vals (column_names, &name, 1);
	 g_array_append_vals (column_types, &type, 1);

	 default_value = g_strdup ("");
	 
   	 for (subnode = node->children; subnode != NULL; subnode=subnode->next)
   	 {
   	     if (strcmp(subnode->name, "value")==0) 
   	     {
   		 subsubnode = subnode->children;
		 value = subsubnode->content;
                 values = g_list_append (values, g_strdup (value));
		 type = xmlGetProp (subnode, "type");

		 if (type!=NULL && strcmp(type, "default")==0)
		 {
		    default_value = g_strdup(value);
		 }
   	     }
   	 }

	 g_hash_table_insert (column_values, name, values);
	 g_hash_table_insert (column_default_values, name, default_value);
      }
   }

   /* Set interface column types */
 
   gint i;
   num_columns = column_names->len;
   column_gtk_types = g_new (GType, num_columns);

   for (i = 0; i < num_columns; i++)
   {
	   column_gtk_types [i] = G_TYPE_STRING;
   }
   
   xmlFreeDoc (xml_data);
} 

static void load_row (gchar *row_id, GtkTreeIter *reload_row_iter)

{
	/* Loads a new row, if reload_row_iter == NULL or reload it otherwise. */

        GHashTable *row;
	row = g_hash_table_new (&g_str_hash, &g_str_equal); 
	gchar *row_file_path, *key, *value;
        xmlDocPtr xml_data;
	xmlNodePtr root, node;
	gint i;
        GtkTreeIter iter;
        GtkTreeIter *iter_p;
	
	g_hash_table_insert (row, "MINIDO_ROW_ID", row_id);
	row_file_path = g_build_filename (minido_db, row_id, NULL);
	xml_data = xmlParseFile (row_file_path);

	/* Check that the root node is named 'row' */

        root = xml_data->children;

        if (strcmp(root->name, "row")!=0)
	{
	   error_dialog (g_strdup_printf (_("Tag xml invalide dans le fichier todo liste : %s\n"), root->name));
	   /* Should report line number too */
        }

	for (node = xml_data->children->children;node !=NULL;node=node->next)
	{
           if (strcmp(node->name, "column")==0)
           {
		/* CAUTION: should check that 'name' belongs to column_names */
	      g_hash_table_insert (row, xmlGetProp (node, "name"), xmlGetProp (node, "value"));
	   }
        }

	xmlFreeDoc (xml_data);

	/* Store the loaded row into the model */

	if (reload_row_iter == NULL)
	{
           gtk_list_store_append (model, &iter);
	   iter_p = &iter;
	}
	else
	{
	   iter_p = reload_row_iter;
	}

	for (i = 0; i < num_columns; i++)
	{
	    key = g_array_index (column_names, gchar*, i);
	    value = g_hash_table_lookup (row, key);
	    gtk_list_store_set (model, iter_p, i, value, -1);
	}

	/* Store the row file date, useful to check that the row is up to date */
	g_hash_table_insert (file_times, row_id, g_strdup_printf ("%d", get_row_file_time (row_id)));
} 

static void update_rows (void)
{
    /* Scan the database for rows to add, update or delete */ 

    GDir *dir;
    gchar *file_name, *file_path, *recorded_file_time_str;
    gint recorded_file_time, actual_file_time;

    dir = g_dir_open (minido_db, 0, NULL);

    if (dir == NULL)
    {
	   error_dialog (g_strdup_printf (_("Ne peux pas ouvrir le répértoire %s. Avez vous les bons droits sur /home/user/.griffon/todo/\n"), minido_db));
    } 

    /* Looks for new files */

    while ((file_name = g_strdup (g_dir_read_name (dir)))!=NULL)
    {
	if (g_pattern_match_simple ("*.xml", file_name))
	{
	   recorded_file_time_str = g_hash_table_lookup (file_times, file_name);

	   if (recorded_file_time_str == NULL)
	   {
	      /* New file, let's load it */
	      load_row (file_name, NULL);
	   }
	}
    }

    g_dir_close (dir);

    /* Update rows which file has been updated or suppress rows which file has been removed */

    GtkTreeIter iter;
    gboolean rows_left;

    rows_left = gtk_tree_model_get_iter_first (GTK_TREE_MODEL (model), &iter);

    while (rows_left)
    {
       gtk_tree_model_get (GTK_TREE_MODEL (model), &iter, 0, &file_name, -1);
       file_path = g_build_filename (minido_db, file_name, NULL);

       if (!g_file_test (file_path, G_FILE_TEST_EXISTS))
       {
          rows_left = gtk_list_store_remove (model, &iter);
	  g_hash_table_remove (file_times, file_name);
       }
       else 
       {
	  recorded_file_time_str = g_hash_table_lookup (file_times, file_name);
	  recorded_file_time = atoi (g_strdup(recorded_file_time_str));
	  actual_file_time = get_row_file_time (file_name);
	
   	  if (recorded_file_time < actual_file_time) 
	  {
	     load_row (file_name, &iter);
	  }

          rows_left = gtk_tree_model_iter_next (GTK_TREE_MODEL (model), &iter);
       }
    }
}

static void edit_row_quit (GtkWidget *widget, gpointer data)
{
    GtkWidget *window = (GtkWidget *) data;
    gtk_widget_destroy (window);
}

static void edit_row_destroy (GtkWidget *widget, gpointer data)
{
    GtkWidget *window = (GtkWidget *) data;
    /* CAUTION: crashes */
/*     g_array_free (text_entries, TRUE);*/ 
    unblock_change_signals ();
}

static gint edit_row_ok (GtkWidget *widget, gpointer data)

    /* Retrieves the widget values from the interface and saves them into a file */

{
    gint i, errno;
    typed_text_entry text_entry;
    const gchar *value;
    gchar *row_file_path;
    FILE *file_id;
    GtkWidget *window = (GtkWidget *) data;

    row_file_path = g_build_filename (minido_db, current_row_file_name, NULL);
    file_id = fopen (row_file_path, "w");

    if (file_id==NULL)
    {
       error_dialog (g_strdup_printf (((_("Ne peux pas écrire le fichier %s.\nVous n'avez propablement pas les bons droits."))), 
			       	     row_file_path));
       /* CAUTION: find how to exit without return */
       return (0);
    }

    fprintf (file_id, "<?xml version=\"1.0\"?>\n");
    fprintf (file_id, "<row>\n");

    for (i = 1; i < num_columns; i++)
    {
	    text_entry = g_array_index (text_entries, typed_text_entry, i-1); 
	    if (text_entry.is_combo)
	    {
	       value = gtk_entry_get_text (GTK_ENTRY (text_entry.widget));
	    }
	    else
	    {
	       value = gtk_entry_get_text (GTK_ENTRY (text_entry.widget));
	    }
   
	    fprintf (file_id, "<column name=\"%s\" value=\"%s\"></column>\n",
		    g_array_index (column_names, gchar*, i), value);
    }

    fprintf (file_id, "</row>\n");
    fclose (file_id);
    g_free (row_file_path);
    edit_row_quit (widget, (gpointer) window);
    update_rows ();
}

static void edit_row_box (gchar *title, GArray *default_values)
{
  GtkWidget *window, *table, *label, *text_input, *vbox, *hbox, *cancel_button, *ok_button;
  gint i;
  GList *values;
  gchar *column_name;
  gchar *default_value;
  typed_text_entry text_entry;
  
  text_entries = g_array_new (FALSE, FALSE, sizeof (typed_text_entry));

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_resize (GTK_WINDOW (window), 550, 100);
  gtk_window_set_title (GTK_WINDOW (window), title);
  vbox = gtk_vbox_new (FALSE, 5);
  gtk_container_add (GTK_CONTAINER (window), vbox);
  hbox = gtk_hbox_new (FALSE, 5);

  ok_button = gtk_button_new_from_stock (GTK_STOCK_OK);
  cancel_button = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
  gtk_box_pack_start (GTK_BOX (hbox), ok_button, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), cancel_button, TRUE, TRUE, 0);
      
  table = gtk_table_new (num_columns - 1, 2, FALSE);

  for (i = 1; i < num_columns; i++)
  {
	  column_name = g_array_index (column_names, gchar*, i);
	  label = gtk_label_new (column_name);
	  gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, i-1, i);

	  values = g_hash_table_lookup (column_values, column_name); 
	  default_value = g_array_index (default_values, gchar*, i-1);

	  if (values == NULL)
	  {
		 text_input = gtk_entry_new ();
		 text_entry.is_combo = FALSE;
		 gtk_entry_set_text (GTK_ENTRY (text_input), default_value);
	  }
	  else
	  {
	  	 text_input = gtk_entry_new ();
		 /*text_entry.is_combo = TRUE;
                 gtk_combo_set_popdown_strings (text_input, values);*/
                 gtk_entry_set_text (text_input, default_value);
	  }
	  gtk_table_attach_defaults (GTK_TABLE (table), text_input, 1, 2, i-1, i);
	  text_entry.widget = text_input;
	  g_array_append_vals (text_entries, &text_entry, 1);
  }

  g_signal_connect (G_OBJECT (ok_button), "clicked", G_CALLBACK (edit_row_ok), (gpointer) window);
  g_signal_connect (G_OBJECT (cancel_button), "clicked", G_CALLBACK (edit_row_quit), (gpointer) window);
  g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (edit_row_destroy), (gpointer) window);
  gtk_box_pack_start (GTK_BOX (vbox), table, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);
  gtk_widget_show_all (window);
}

static void add_row (GtkWidget *button, gpointer data)
{
  GArray *default_values;
  gchar *value, *row_file_name;
  gint i;

  block_change_signals ();

  /* Creating unique file name based on current time and user name */

  GTimeVal time;
  g_get_current_time (&time);    
  current_row_file_name = g_strdup_printf ("%s_%d.xml", g_get_user_name (), time.tv_sec);

  default_values = g_array_new (FALSE, FALSE, sizeof (gchar*));

  for (i = 1; i < num_columns; i++)
  {
	  value = g_hash_table_lookup (column_default_values, g_array_index (column_names, gchar *, i));
	  g_array_append_vals (default_values, &value, 1);
  }

  edit_row_box (_("Ajouter une nouvelle ligne"), default_values);
}

static void edit_row (GtkWidget *button, gpointer data)
{
  GtkTreeIter iter;
  GArray *default_values;
  gchar *value, *row_file_name;
  gint i;
  GtkTreeView *treeview = (GtkTreeView *) data;
  GtkTreeSelection *selection = gtk_tree_view_get_selection (treeview);

	//************** TEST si une séléction est bien présente
	if(! gtk_tree_selection_get_selected (selection, NULL, &iter)){return ;}

  if (selection)

  /* Needed when there are no rows */

  {
     block_change_signals ();

     if (gtk_tree_selection_get_selected (selection, NULL, &iter))
     {
        gtk_tree_model_get (GTK_TREE_MODEL (model), &iter, 0, &current_row_file_name, -1);
        default_values = g_array_new (FALSE, FALSE, sizeof (gchar*));

        for (i = 1; i < num_columns; i++)
        {
	     gtk_tree_model_get (GTK_TREE_MODEL (model), &iter, i, &value, -1);
	     g_array_append_vals (default_values, &value, 1);
        }
     }

     edit_row_box (_("Editer une ligne"), default_values);
  }
}

gint error_dialog (gchar *message)
{
   GtkWidget *dialog;
   
   dialog = gtk_message_dialog_new (GTK_WINDOW (tea_main_window),
		                    GTK_DIALOG_DESTROY_WITH_PARENT,
				    GTK_MESSAGE_ERROR,
				    GTK_BUTTONS_CLOSE,
				    message);
   gtk_dialog_run (GTK_DIALOG (dialog));
   gtk_widget_destroy (dialog);
}

gint confirm_dialog (gchar *message)
{
   GtkWidget *dialog, *label;
   gint result;
   
   dialog = gtk_dialog_new_with_buttons (_("Please confirm"),
                                         GTK_WINDOW (tea_main_window),
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_STOCK_YES,
                                         GTK_RESPONSE_YES,
					 GTK_STOCK_NO,
					 GTK_RESPONSE_NO,
                                         NULL);
   label = gtk_label_new (message);

	GtkWidget *content_area;
   content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
   gtk_container_add (GTK_CONTAINER (content_area), label);

   gtk_widget_show_all (dialog);
   result = gtk_dialog_run (GTK_DIALOG (dialog));
   gtk_widget_destroy (dialog);
   return result;
}


static void remove_row (GtkWidget *widget, gpointer data)
{
  GtkTreeIter iter;
  GtkTreeView *treeview = (GtkTreeView *) data;
  GtkTreeSelection *selection = gtk_tree_view_get_selection (treeview);
  gchar *file_name, *file_path;

  block_change_signals ();

  if (gtk_tree_selection_get_selected (selection, NULL, &iter))
    {
       if (confirm_dialog (_("Voulez vous vraiment supprimer cette ligne?")) == GTK_RESPONSE_YES)
       {
          gtk_tree_model_get (GTK_TREE_MODEL (model), &iter, 0, &file_name, -1);
	  file_path = g_build_filename (minido_db, file_name, NULL);
       
          if (remove (file_path) != 0)
          {
             error_dialog (g_strdup_printf (_("Ne peux pas supprimer %s. Avez vous les bons droits sur %s? \n"),
				            file_path, minido_db));
          }

          update_rows ();
       }
    }

  unblock_change_signals ();
}

static void add_columns (GtkTreeView *treeview)
{
  GtkCellRenderer *renderer;
  gint i;

  /* The first column is not rendered, it keeps track of row file name */

  for (i = 1; i < num_columns; i++)
  {
     renderer = gtk_cell_renderer_text_new ();
     g_object_set_data (G_OBJECT (renderer), "column", &i);
     gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview),
					         -1, g_array_index (column_names, gchar *, i), renderer,
					         "text", i,
					          NULL);
  } 
}

gint exit_event ( GtkWidget *widget, GdkEvent *event, gpointer data )
{
  //gtk_main_quit ();
}
	        
static gboolean update_rows_timeout (gpointer data)
{
   update_rows ();
   return TRUE;
}

main2 ()
{
    GtkWidget *vbox, *hbox, *sw, *treeview, *button;
    gint i;
	gchar *home_conf_dir=g_get_home_dir ();
	strcat(home_conf_dir,"/.griffon/todo/");
	minido_db = home_conf_dir;

	
    if (minido_db == NULL)
    {
	   error_dialog (_("MINIDO_DB variable non trouvable (normalement /home/user/.griffon/todo/).\n"));
    } 

    file_times = g_hash_table_new (&g_str_hash, &g_str_equal);
//******************************* bug ici
    declare_columns ();

    vbox = gtk_vbox_new (FALSE, 5);
    gtk_container_add (GTK_CONTAINER (notebook_down), vbox);
    sw = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw), GTK_SHADOW_ETCHED_IN);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);


    /* create model */
    model = gtk_list_store_newv (num_columns, column_gtk_types);

    /* create tree view */
    treeview = gtk_tree_view_new_with_model (GTK_TREE_MODEL (model));
    gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview), TRUE);
    gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview)),
				   GTK_SELECTION_SINGLE);

    add_columns (GTK_TREE_VIEW (treeview));
	 gtk_tree_view_set_grid_lines (treeview,GTK_TREE_VIEW_GRID_LINES_BOTH);
    gtk_container_add (GTK_CONTAINER (sw), treeview);

    /* some buttons */
    hbox = gtk_hbox_new (TRUE, 4);
    gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

    add_button = gtk_button_new_from_stock (GTK_STOCK_ADD);
    add_signal_handler = g_signal_connect (G_OBJECT (add_button), "clicked", G_CALLBACK (add_row), GTK_TREE_MODEL (model));
    gtk_box_pack_start (GTK_BOX (hbox), add_button, TRUE, TRUE, 0);


    edit_button = gtk_button_new_from_stock (GTK_STOCK_EDIT);
    edit_signal_handler = g_signal_connect (G_OBJECT (edit_button), "clicked", G_CALLBACK (edit_row), treeview);
    gtk_box_pack_start (GTK_BOX (hbox), edit_button, TRUE, TRUE, 0);

    remove_button = gtk_button_new_from_stock (GTK_STOCK_REMOVE);
    remove_signal_handler = g_signal_connect (G_OBJECT (remove_button), "clicked", G_CALLBACK (remove_row), treeview);
    gtk_box_pack_start (GTK_BOX (hbox), remove_button, TRUE, TRUE, 0);

    gtk_widget_show_all (vbox);
    update_rows ();

    /* Function that's called every REFRESH_INTERVAL seconds to update rows if needed */
    //g_timeout_add (REFRESH_INTERVAL*1000, update_rows_timeout, NULL);

}


