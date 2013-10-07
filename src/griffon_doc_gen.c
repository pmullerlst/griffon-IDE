
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 **************************************-************************************/
//Variable ................cur_text_doc->file_name pour le nom du fichier 

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

#include "griffon_text_document.h"
#include "callbacks.h"
#include "interface.h"
#include "griffon_config.h"

#include "griffon_gtk_utils.h"

//******************************* Documentation génération
void
gen_doc_html (void)
{	
	if (! get_page_text()) return;

	   char *extension;
	   if(extension = strrchr(cur_text_doc->file_name,'.')){}
		char *titre=cur_text_doc->file_name;

  FILE *fich;
  char carac;
  int position;
  char motrch[100],motrch2[100],motrch3[100],motrch4[100],motrch5[100],motrch6[100],motrch7[100], mot[1000],mot2[1000],ligne[10],doc_html[90000],doc_html_menu[90000];
  char motrch8[100],motrch9[100],motrch10[100],motrch11[100],motrch12[100],motrch13[100];
  int nbapparition=0,nbcarac=0,nbmot=0,counter=0,nbspace=0;
  int nbligne=1;

  if (! get_page_text()) return;
	//gtk_notebook_set_current_page(notebook2,0);
	
     icon_affiche_bug();
	clear_list_include ();

  nbapparition=0,nbcarac=0,nbmot=0,nbligne=1;

//************************* INIT DES MOTS RECHERCHE
  mot[0]='\0';
  mot2[0]='\0';
  motrch[0]='\0';
  motrch2[0]='\0';
  motrch3[0]='\0';
  motrch4[0]='\0';
  motrch5[0]='\0';
  motrch6[0]='\0';
  motrch7[0]='\0';
  motrch8[0]='\0';
  motrch9[0]='\0';
  motrch10[0]='\0';
  motrch11[0]='\0';
  motrch12[0]='\0';
  motrch13[0]='\0';

	doc_html[0]='\0';
	doc_html_menu[0]='\0';

//************************ MOTS RECHERCHER
	strcpy(motrch,"include");
	strcpy(motrch2,"require");
	strcpy(motrch3,"function");
	strcpy(motrch4,"REQUEST");
	strcpy(motrch5,"POST");
	strcpy(motrch6,"GET");
	strcpy(motrch7,"sub");
	strcpy(motrch8,"use");
	strcpy(motrch9,"if");
	strcpy(motrch10,"while");
	strcpy(motrch11,"unless");
	strcpy(motrch12,"foreach");
	strcpy(motrch13,"exit");

//********************************** BOUCLE DE LIGNE CARAC PAR CARAC
  fich=fopen(cur_text_doc->file_name,"r");
  while ((carac =fgetc(fich)) != EOF)
    {
		if(carac=='#'){counter=6;}
		if(carac==' '){nbspace++;}
		if(carac=='<'){carac='/';}
		if(carac=='>'){carac='/';}

	//******************************* pour include ou require     
  if(counter==1)
  {
	  if (carac !='\n' || carac !='\r'){strncat(mot,&carac,1);}
  
	    if (carac =='\n' || carac =='\r')
	    {

			gchar **a = g_strsplit (mot, "\"", -1);	 
			if(a[1]!='\0')
			{
		    sprintf(ligne,"%d",nbligne); 

		 	//**** ADD IN MENU HTML
			strcat(doc_html_menu,"<li style=\"color:#a427f2;\"><a href=\"#");
			strcat(doc_html_menu,"Include file : ");
			strcat(doc_html_menu,a[1]);
			strcat(doc_html_menu,"\">");
			strcat(doc_html_menu,"Include file : ");
			strcat(doc_html_menu,a[1]);
			strcat(doc_html_menu,"</a></li>\n");

			//**** ADD IN DOC
			strcat(doc_html,"<hr><a href=\"#top\">Top page</a>\n        <a name=\"");
			strcat(doc_html,"Include file : ");
			strcat(doc_html,a[1]);
			strcat(doc_html,"\"></a>\n        <h1 style=\"color:#a427f2;\">");
			strcat(doc_html,"Include file : ");
			strcat(doc_html,a[1]);
			strcat(doc_html,"</h1>\n                <div class=\"table-of-contents\" style=\"width:90%;\">\n    <p class=\"table-of-contents-heading\">Summary</p>\n\nYour text here<br><br> \n\n			</div>\n\n");
	     }

    mot[0]='\0'; nbmot=0;
    ligne[0]='\0';
    counter=0;  
	    }
    
  }  

	//******************************* pour fonction
  if(counter==2)
  {
	  if (carac !='\n' || carac !='\r'){strncat(mot,&carac,1);}
  
	    if (carac =='\n' || carac =='\r')
	    {
			gchar **b = g_strsplit (mot, "\n", -1);
			gchar **a = g_strsplit (b[0], "function", -1);	 
			if(a[1]!='\0')
			{
		    sprintf(ligne,"%d",nbligne); 

		 	//**** ADD IN MENU HTML
			strcat(doc_html_menu,"<li><a href=\"#");
			strcat(doc_html_menu,"Function : ");
			strcat(doc_html_menu,a[1]);
			strcat(doc_html_menu,"\">");
			strcat(doc_html_menu,"Function : ");
			strcat(doc_html_menu,a[1]);
			strcat(doc_html_menu,"</a></li>\n");

			//**** ADD IN DOC
			strcat(doc_html,"<hr><a href=\"#top\">Top page</a>\n        <a name=\"");
			strcat(doc_html,"Function : ");
			strcat(doc_html,a[1]);
			strcat(doc_html,"\"></a>\n        <h1>");
			strcat(doc_html,"Function : ");
			strcat(doc_html,a[1]);
			strcat(doc_html,"</h1>\n                <div class=\"table-of-contents\" style=\"width:90%;\">\n    <p class=\"table-of-contents-heading\">Summary</p>\n\nYour text here<br><br> \n\n			</div>\n\n");


	     }

    mot[0]='\0'; nbmot=0;
    ligne[0]='\0';
    counter=0;  
	    }
	} 

	//******************************* pour variable php
  if(counter==3 && strcmp(".php", extension) == 0)
  {
	  if (carac !='\n' || carac !='\r'){strncat(mot,&carac,1);}
  
	    if (carac =='\n' || carac =='\r')
	    {
			gchar **b = g_strsplit (mot, "\n", -1);
			gchar **a = g_strsplit (b[0], "]", -1);
			if(a[0]!='\0')
			{
		    sprintf(ligne,"%d",nbligne); 

		 	//**** ADD IN MENU HTML
			strcat(doc_html_menu,"<li style=\"color:#2e8b71;\"><a href=\"#");
			strcat(doc_html_menu,"Variable : ");
			strcat(doc_html_menu,a[0]);
			strcat(doc_html_menu,"]\">");
			strcat(doc_html_menu,"Variable : ");
			strcat(doc_html_menu,a[0]);
			strcat(doc_html_menu,"]</a></li>\n");

			//**** ADD IN DOC
			strcat(doc_html,"<hr><a href=\"#top\">Top page</a>\n        <a name=\"");
			strcat(doc_html,"Variable : ");
			strcat(doc_html,a[0]);
			strcat(doc_html,"]\"></a>\n        <h1 style=\"color:#2e8b71;\">");
			strcat(doc_html,"Variable : ");
			strcat(doc_html,a[0]);
			strcat(doc_html,"]</h1>\n                <div class=\"table-of-contents\" style=\"width:90%;\">\n    <p class=\"table-of-contents-heading\">Summary</p>\n\nYour text here<br><br> \n\n			</div>\n\n");
		
	     }

    mot[0]='\0'; nbmot=0;
    mot2[0]='\0';
    ligne[0]='\0';
    counter=0;  
	    }
   
  }  

	//******************************* pour fonction perl sub
  if(counter==4 && strcmp(".pl", extension) == 0 )
  {
	  if (carac !='\n' || carac !='\r'){strncat(mot,&carac,1);}
  
	    if (carac =='\n' || carac =='\r')
	    {
			gchar **b = g_strsplit (mot, "\n", -1);
			gchar **a = g_strsplit (b[0], "sub", -1);	 
			if(a[1]!='\0')
			{
		    sprintf(ligne,"%d",nbligne); 

		 	//**** ADD IN MENU HTML
			strcat(doc_html_menu,"<li><a href=\"#");
			strcat(doc_html_menu,"Function : ");
			strcat(doc_html_menu,a[1]);
			strcat(doc_html_menu,"\">");
			strcat(doc_html_menu,"Function : ");
			strcat(doc_html_menu,a[1]);
			strcat(doc_html_menu,"</a></li>\n");

			//**** ADD IN DOC
			strcat(doc_html,"<hr><a href=\"#top\">Top page</a>\n        <a name=\"");
			strcat(doc_html,"Function : ");
			strcat(doc_html,a[1]);
			strcat(doc_html,"\"></a>\n        <h1>");
			strcat(doc_html,"Function : ");
			strcat(doc_html,a[1]);
			strcat(doc_html,"</h1>\n                <div class=\"table-of-contents\" style=\"width:90%;\">\n    <p class=\"table-of-contents-heading\">Summary</p>\n\nYour text here<br><br> \n\n			</div>\n\n");

	     }

    mot[0]='\0'; nbmot=0;
    ligne[0]='\0';
    counter=0;  
	    }
	} 

	//******************************* pour fonction perl use  
  if(counter==5 && strcmp(".pl", extension) == 0 )
  {
	  if (carac !='\n' || carac !='\r'){strncat(mot,&carac,1);}
  
	    if (carac =='\n' || carac =='\r')
	    {
			gchar **b = g_strsplit (mot, "\n", -1);
			gchar **a = g_strsplit (b[0], "use", -1);	 
			if(a[1]!='\0')
			{
		    sprintf(ligne,"%d",nbligne); 

		 	//**** ADD IN MENU HTML
			strcat(doc_html_menu,"<li style=\"color:#a427f2;\"><a href=\"#");
			strcat(doc_html_menu,"Module : ");
			strcat(doc_html_menu,a[1]);
			strcat(doc_html_menu,"\">");
			strcat(doc_html_menu,"Module : ");
			strcat(doc_html_menu,a[1]);
			strcat(doc_html_menu,"</a></li>\n");

			//**** ADD IN DOC
			strcat(doc_html,"<hr><a href=\"#top\">Top page</a>\n        <a name=\"");
			strcat(doc_html,"Module : ");
			strcat(doc_html,a[1]);
			strcat(doc_html,"\"></a>\n        <h1 style=\"color:#a427f2;\">");
			strcat(doc_html,"Module : ");
			strcat(doc_html,a[1]);
			strcat(doc_html,"</h1>\n                <div class=\"table-of-contents\" style=\"width:90%;\">\n    <p class=\"table-of-contents-heading\">Summary</p>\n\nYour text here<br><br> \n\n			</div>\n\n");

	     }

    mot[0]='\0'; nbmot=0;
    ligne[0]='\0';
    counter=0;  	 
	    }
	} 


	//******************************* pour fonction perl et bash pour les commentaires 
   if(counter==6)
   {
		if(strcmp(".pl", extension) == 0 || strcmp(".sh", extension) == 0)
		{
	  if (carac !='\n' || carac !='\r'){strncat(mot,&carac,1);}
  
	    if (carac =='\n' || carac =='\r')
	    {
			
			if(nbspace>2 && strlen(mot)>3)
			{
		    sprintf(ligne,"%d",nbligne);
 
			 gchar **b = g_strsplit (mot, "\n", -1);
			if(b[0]!='\0')
			{
		 	//**** ADD IN MENU HTML
			strcat(doc_html_menu,"<li style=\"color:#0d0dff;\"><a href=\"#");
			strcat(doc_html_menu,"Comment : ");
			strcat(doc_html_menu,b[0]);
			strcat(doc_html_menu,"\">");
			strcat(doc_html_menu,"Comment : ");
			strcat(doc_html_menu,b[0]);
			strcat(doc_html_menu,"</a></li>\n");

			//**** ADD IN DOC
			strcat(doc_html,"<hr><a href=\"#top\">Top page</a>\n        <a name=\"");
			strcat(doc_html,"Comment : ");
			strcat(doc_html,b[0]);
			strcat(doc_html,"\"></a>\n        <h1 style=\"color:#0d0dff;\">");
			strcat(doc_html,"Comment : ");
			strcat(doc_html,b[0]);
			strcat(doc_html,"</h1>\n                <div class=\"table-of-contents\" style=\"width:90%;\">\n    <p class=\"table-of-contents-heading\">Summary</p>\n\nYour text here<br><br> \n\n			</div>\n\n");
			}
	     }

	    counter=0;  
		 nbspace=0;
	    }
	  }
	}

	//******************************* pour instruction
  if(counter==7)
  {
	  if (carac !='\n' || carac !='\r'){strncat(mot,&carac,1);}
  
	    if (carac =='\n' || carac =='\r')
	    {
	 
		    sprintf(ligne,"%d",nbligne); 
			 gchar **b = g_strsplit (mot, "\n", -1);
			if(b[0]!='\0')
			{
		 	//**** ADD IN MENU HTML
			strcat(doc_html_menu,"<li style=\"color:#a52a44;\"><a href=\"#");
			strcat(doc_html_menu,"Instruction : ");
			strcat(doc_html_menu,b[0]);
			strcat(doc_html_menu,"\">");
			strcat(doc_html_menu,"Instruction : ");
			strcat(doc_html_menu,b[0]);
			strcat(doc_html_menu,"</a></li>\n");

			//**** ADD IN DOC
			strcat(doc_html,"<hr><a href=\"#top\">Top page</a>\n        <a name=\"");
			strcat(doc_html,"Instruction : ");
			strcat(doc_html,b[0]);
			strcat(doc_html,"\"></a>\n        <h1 style=\"color:#a52a44;\">");
			strcat(doc_html,"Instruction : ");
			strcat(doc_html,b[0]);
			strcat(doc_html,"</h1>\n                <div class=\"table-of-contents\" style=\"width:90%;\">\n    <p class=\"table-of-contents-heading\">Summary</p>\n\nYour text here<br><br> \n\n			</div>\n\n");
			}
    mot[0]='\0'; nbmot=0;
    ligne[0]='\0';
    counter=0;  
	    }
	} 

//********************************************** RECHERCHE CARACTERE MOT BOUCLE *********************************************    
if (counter==0)
{    

		if(carac=='_')
		{
			mot2[0]='\0';
		}

    if (carac =='\n' || carac =='\r')
    {   
    mot[0]='\0';
	 mot2[0]='\0';
    nbligne++;
	 nbmot=0;
    }

		if(strlen(mot)>900){mot[0]='\0';}
		if(strlen(mot2)>900){mot2[0]='\0';}

      nbcarac++;
      if (mot[0] != '\0')
		{
		  if (carac !='\n' || carac !='\r'){strncat(mot,&carac,1);}
		}
		
      if (mot[0] != '\0' && isalnum(carac) != 0)
		{
		  if (carac !='\n' || carac !='\r'){strncat(mot2,&carac,1);	}		
		}
		
      if (mot[0] != '\0' && isalnum(carac) == 0)
		{
		 nbmot++;
		//****************************************** INCLUDE REQUIRE
		  if (strncmp(motrch,mot,strlen(motrch))==0 || strncmp(motrch2,mot,strlen(motrch2))==0){counter=1;}	

		//**************************************** FUNCTION
		  if (strncmp(motrch3,mot,strlen(motrch3))==0){counter=2;}	


		//***************************************** VARIABLE PHP
		  if (strncmp(motrch4,mot2,strlen(motrch4))==0 || strncmp(motrch5,mot2,strlen(motrch5))==0 || strncmp(motrch6,mot2,strlen(motrch6))==0){counter=3;}	

		//*********************************** PERL SUB
		 if (strncmp(motrch7,mot,strlen(motrch7))==0){counter=4;}			

		//*********************************** PERL USE MODULE
		 if (strncmp(motrch8,mot,strlen(motrch8))==0) {counter=5;}	
		
		//*********************************** INSTRUCTION
		 if (strncmp(motrch9,mot,strlen(motrch9))==0) {counter=7;}
		//*********************************** INSTRUCTION
		 if (strncmp(motrch10,mot,strlen(motrch10))==0) {counter=7;}
		//*********************************** INSTRUCTION
		 if (strncmp(motrch11,mot,strlen(motrch11))==0) {counter=7;}
		//*********************************** INSTRUCTION
		 if (strncmp(motrch12,mot,strlen(motrch12))==0) {counter=7;}
		//*********************************** INSTRUCTION
		 if (strncmp(motrch13,mot,strlen(motrch13))==0) {counter=7;}

	}

      if (mot[0] == '\0' && isalnum(carac) != 0)
		{
			if (carac !='\n' || carac !='\r'){
		  strncat(mot,&carac,1);    
			strncat(mot2,&carac,1);}
			 		                     
		}

			

  }
    
}

  fclose(fich);

//*********************************** Construction du fichier HTML

	file_new();
	doc_insert_at_cursor (cur_text_doc,"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/1999/REC-html401-19991224/loose.dtd\">\n<html>\n<head>\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n<title></title>\n<style type=\"text/css\">\ndiv#content a {\n	color: #3465a4;\n	text-decoration: none;\n}\n\ndiv#content a:hover {\n	text-decoration: underline;\n}\n\ndiv#content {\n	clear: both;\n	padding: 1em;\n	margin: 1em;\n}\n\ndiv.table-of-contents {\n    border: 1px dotted #888;\n    background: #f0f0f0;\n    margin: 1em 1em 1em 1em;\n\n    font-size: 85%;\n    width: 750px;\n}\n\ndiv.table-of-contents p.table-of-contents-heading {\n    font-weight: bold;\n    margin: 0.5em 1em;\n}\n\ndiv.table-of-contents ol {\n    margin: 1em 0.5em 1em 1em;\n    padding: 0;\n}\n\ndiv.table-of-contents ol li {\n    margin: 0;\n    padding: 0;\n    margin-left: 1em;\n}\n\ndiv.table-of-contents ol ol {\n    margin: 0.5em 0.5em 0.5em 1em;\n    padding: 0;\n}\n\nhr {\n  border: 1;\n  width: 80%;\n  color: gray;\n	background-color: gray;\n}\n\nbody\n{\nfont-family: Abel, sans-serif;\nfont-size: 10pt;\n\n}\n\nh1\n{\nfont-family: Abel, sans-serif;\nfont-size: 15pt;\n}\n</style></head><body>\n\n");
	doc_insert_at_cursor (cur_text_doc,"\n\n<a name=\"top\"></a>\n\n      <div id=\"content\"><img src=\"http://griffon.lasotel.fr/DOC/images/griffon_start.png\">\n        <div class=\"table-of-contents\">\n    <p class=\"table-of-contents-heading\">Documentation</p>\n      <ol>\n        <li><a href=\"#Introduction\">Introduction</a>\n        <ol>\n\n");
	doc_insert_at_cursor (cur_text_doc,doc_html_menu);
	doc_insert_at_cursor (cur_text_doc,"\n\n        </ol>\n        </li>\n\n  </div>\n\n		  <br>\n        <a name=\"Introduction\"></a>\n        <h1>Introduction</h1>\n        <p>\n			Autor :      <br>\n			Email :       <br>File :  \n");
	doc_insert_at_cursor (cur_text_doc,titre);
	doc_insert_at_cursor (cur_text_doc,"			\n<br><br>\n			Summary :<br><br>\n\n			</p>\n\n");
	doc_insert_at_cursor (cur_text_doc,doc_html);
	doc_insert_at_cursor (cur_text_doc,"\n\n<br><br>&#169;&nbsp;&nbsp;Document generated and edited with <a href=\"griffon.lasotel.fr\">Griffon IDE</a></div></body></html>");

	file_save();
   
	//gtk_notebook_set_current_page(notebook_down,4);
	web_current_file();
}
