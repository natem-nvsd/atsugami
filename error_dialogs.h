/* error_dialogs.h */
#include <gtk/gtk.h>
#include "main.h"

#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic ignored "-Wincompatible-pointer-types"

#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

//extern GtkWidget *error_activate(GtkWidget *do_widget);
extern GtkWidget *postgres_error_activate(GtkWidget *do_widget);

#endif
