/* import.h */
#ifndef __IMPORT_H__
#define __IMPORT_H__

#include <glib.h>
#include <gtk/gtk.h>

#//define IMPORT_ACTIVATE

extern void import_activate(GSimpleAction *action, GVariant *parameter, gpointer user_data);
//static void import_activate(GSimpleAction *action, GVariant *parameter, gpointer user_data);

//#define IMPORT_FILE

//extern gchar import_file_path[10240];
extern gchar import_file_path[10240];

#endif
