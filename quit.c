/* quit.c */
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include "main.h"
#include "quit.h"

static void quit_activate(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
//extern void quit_activate(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
//void quit_activate(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
	PQfinish(conn);
	GtkWidget *window = user_data;
	gtk_widget_destroy(window);
}
