/* main.h */
//#define GLADEFILE "/usr/local/share/atsugami/atsugami.glade"
#define GLADEFILE "atsugami.glade"
typedef _Bool bool;

#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
#include <libpq-fe.h>
#include <stdio.h>

extern PGconn *conn;
extern char conninfo[];

#endif
