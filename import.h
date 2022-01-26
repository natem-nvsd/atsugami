/* import.h */
#ifndef __IMPORT_H__
#define __IMPORT_H__

#include <glib.h>
#include <gtk/gtk.h>

//#//define IMPORT_ACTIVATE
extern void import_activate(gpointer user_data);
extern gchar import_file_path[10240];
extern gchar file_sha256[65];

#endif
