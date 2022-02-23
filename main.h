/* main.h */
#define EOL -1
#define __VERSION "0.81.0"

typedef _Bool bool;

#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdio.h>

extern PGconn *conn;
extern char conninfo[];
extern void destroy_window(gpointer user_data);
extern GtkWidget *notebook;
extern GtkWidget *file_label, *vbox;
extern GtkWidget *window;

#endif
