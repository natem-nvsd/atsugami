/* file_count.h */
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic ignored "-Wsometimes-uninitialized"
#pragma clang diagnostic ignored "-Wincompatible-pointer-types"
#pragma clang diagnostic ignored "-Wimplicit-function-declaration"

typedef _Bool bool;

#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
#include <gtk/gtk.h>

extern file_count(GtkWidget *label, GtkWidget *box);
extern file_count_update(GtkWidget *label, GtkWidget *box);

#endif
