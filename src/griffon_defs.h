
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif

#define cTEA_TEMP "tea_temp_file"
#define ERRO g_print("%s\n", strerror (errno))
#define DBM(x) g_print("%s\n", x)
#define STRMAXLEN G_MAXINT
#define dbm(x) g_print("%s\n", x)
#define CURR_LOCALE "current locale"

#define CHARSET_JAPANESE "Japanese autodetect"
#define CHARSET_CHINESE "Chinese autodetect"
#define CHARSET_AUTODETECT "autodetect"

#define LM_ERROR 13
#define LM_NORMAL 14
#define LM_ADVICE 15
#define LM_GREET 16

#define opt_ht_to_glist_full 1
#define opt_ht_to_glist_keys 2

#define HL_NONE "none"
#define HL_CPP "C++"
#define HL_C "C"
#define HL_PASCAL "Pascal"
#define HL_PYTHON "Python"
#define HL_RUBY "Ruby"
#define HL_PHP "PHP"
#define HL_PERL "PERL"
#define HL_CSS "CSS"
#define HL_JAVA "JAVA"
#define HL_JAVASCRIPT "JAVASCRIPT"
#define HL_MARKUP "HTML"
#define HL_XML "XML"
#define HL_BASH "BASH Script"
#define HL_PO "gettext PO file"

#define TEA_PIX_MAINICON PACKAGE_DATA_DIR"/tea/pixmaps/tea_icon_v2.png"
#define TEA_PIX_LGM PACKAGE_DATA_DIR"/tea/pixmaps/lgm.png"
#define TEA_PIX_DIR PACKAGE_DATA_DIR"/tea/pixmaps/"
#define TEA_DOC_DIR PACKAGE_DATA_DIR"/tea/doc/"
#define TEA_DATA_DIR PACKAGE_DATA_DIR"/tea/"

void buffer_insert_text ( GtkTextIter *arg1,gpointer user_data);

