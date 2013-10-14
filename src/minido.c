
#include <gtk/gtk.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdio.h>
#include <libintl.h>
#define _(String) gettext (String)
/* Stuff needed by stat to access file dates */
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguage.h>

#include "interface.h"
#include "callbacks.h"
#include "griffon_config.h"


