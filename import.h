/* import.h */
#ifndef __IMPORT_H__
#define __IMPORT_H__

#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic ignored "-Wimplicit-function-declaration"
#pragma clang diagnostic ignored "-Wincompatible-pointer-types"

#include <glib.h>
#include <gtk/gtk.h>

//#//define IMPORT_ACTIVATE
extern void import_activate(gpointer user_data);
extern gchar import_file_path[10240];

#endif
