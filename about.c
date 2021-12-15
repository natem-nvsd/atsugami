/* about.c */
#include "about.h"
#include <glib/gstdio.h>
#include <gtk/gtk.h>

//static void about_activate(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
//extern void about_activate(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
void about_activate(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
	GtkBuilder *builder = gtk_builder_new();
	GtkWidget *about_dlg;

	if (gtk_builder_add_from_file(builder,"atsugami.glade" , NULL) == 0) {
		printf("gtk_builder_add_from_file FAILED\n");
	}

	about_dlg = GTK_WIDGET(gtk_builder_get_object(builder, "aboutDialog"));
	gtk_dialog_run(GTK_DIALOG (about_dlg));
	gtk_widget_hide(about_dlg);
}
