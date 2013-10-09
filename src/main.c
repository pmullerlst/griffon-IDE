
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include "griffon_text_document.h"
#include "callbacks.h"
#include "griffon_config.h"
#include "interface.h"
#include "griffon_hl.h"
#include "griffon_funx.h"

void sig_handler (int i)
{
  switch (i)
         {
          case SIGABRT:
          case SIGTERM:
          case SIGINT:
                      ui_done();
                      gtk_main_quit ();
                      exit (i);
         }
}



int main (int argc, char *argv[])
{

  GtkWidget *app_main_window;

#ifdef ENABLE_NLS
  bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);
#endif

//  gtk_set_locale ();
  gtk_init (&argc, &argv);
//    splash_screen ();

  signal (SIGABRT, sig_handler);
  signal (SIGTERM, sig_handler);
  signal (SIGINT, sig_handler);

  tea_start ();
  
  app_main_window = create_tea_main_window ();
  gtk_widget_show (app_main_window);

  g_argv = argv;
  g_argc = argc;

 // gtk_init_add (tea_init, NULL);
  
  update_enc_menu ();
  bmx_reload ();
  create_tags ();
  prepare_hl_all ();

  reload_snippets ();   
  //reload_dicts ();
  reload_usermenu ();
  reload_sessions ();
  reload_templates ();

  //reload_hotkeys ();
  reload_autoreplace ();
  update_recent_list_menu (TRUE);

	if(argv[1]!=NULL)
	{
		if (!g_file_test (argv[1], G_FILE_TEST_EXISTS))
		{

			char cwd[956];
			char *systemRet2 =getcwd(cwd, sizeof(cwd));
			if(systemRet2 == NULL){printf("\n");}
			strcat(cwd,"/");
			strcat(cwd,argv[1]);

			create_empty_file (cwd, "");
			doc_open_file (cwd);

			char *extension;
			if(strrchr(argv[1],'.'))
			{
				extension = strrchr(argv[1],'.');
				if (strcmp(".pl", extension) == 0){start_perl_script();}
				if (strcmp(".sh", extension) == 0){print_bash();}
				if (strcmp(".php", extension) == 0){start_php_script();}
				if (strcmp(".html", extension) == 0){on_mni_html_default_template();}
				if (strcmp(".htm", extension) == 0){on_mni_html_default_template();}
				}

					char commande[150];
					strcpy(commande,"chmod u+x ");
					strcat(commande,cwd);
					int systemRet =system(commande);
					if(systemRet == -1){printf("WARN : chmod impossible\n");}
		}
		else
		{
		doc_open_file (argv[1]);
		}
	}  

  gtk_main ();
  tea_done ();

  return 0;
}
