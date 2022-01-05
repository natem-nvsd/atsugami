/* main.h */
//#define GLADEFILE "/usr/local/share/atsugami/atsugami.glade"
#define GLADEFILE "atsugami.glade"
#define VERSION 0.66
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic ignored "-Wincompatible-pointer-types"
#pragma clang diagnostic ignored "-Wimplicit-function-declaration"
#pragma clang diagnostic ignored "-Wsometimes-uninitialized"

typedef _Bool bool;

#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
#include <libpq-fe.h>
#include <stdio.h>

extern PGconn *conn;
//extern PGconn *res;
extern char conninfo[];
extern void destroy_window(gpointer user_data);
//static const char *restricted[] {"%", "--", ";", "\n", "\"", "'", "[", "]", "{", "}"};
extern GtkWidget *notebook;

#endif
