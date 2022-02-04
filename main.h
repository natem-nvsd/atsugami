/* main.h */
#define EOL -1
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic ignored "-Wsometimes-uninitialized"
#pragma clang diagnostic ignored "-Wincompatible-pointer-types"
#pragma clang diagnostic ignored "-Wimplicit-function-declaration"

typedef _Bool bool;

#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdio.h>

extern PGconn *conn;
extern char conninfo[];
extern void destroy_window(gpointer user_data);
extern GtkNotebook *notebook;
extern GtkWidget *file_label, *vbox;
extern GtkWidget *window;

#endif
