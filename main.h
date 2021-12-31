/* main.h */
//#define GLADEFILE "/usr/local/share/atsugami/atsugami.glade"
#define GLADEFILE "atsugami.glade"
//#define GLADEFILEHASH "a3dbbc3447a7d23c6eac31489b624194cc5d5fb5037373c14c940cbf9bb7d600"
#define VERSION 0.53.2
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic ignored "-Wincompatible-pointer-types"
#pragma clang diagnostic ignored "-Wimplicit-function-declaration"

typedef _Bool bool;

#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
#include <libpq-fe.h>
#include <stdio.h>

extern PGconn *conn;
//extern PGconn *res;
extern char conninfo[];

#endif
