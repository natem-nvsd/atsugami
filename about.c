/* about.c */
#include "about.h"
#include <gtk/gtk.h>
#include "main.h"

void about_activate(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
	GtkBuilder *builder = gtk_builder_new();
	GtkWidget *about_dlg;

	if (gtk_builder_add_from_file(builder, GLADEFILE, NULL) == 0) {
		printf("gtk_builder_add_from_file FAILED\n");
	}

	about_dlg = GTK_WIDGET(gtk_builder_get_object(builder, "aboutDialog"));
	gtk_dialog_run(GTK_DIALOG (about_dlg));
	gtk_widget_hide(about_dlg);
}
/*
extern void about_activate(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
	GtkWidget *about_dialog;
	//gchar *authors = "Nate Morrison";

	about_dialog = gtk_about_dialog_new();

	// define the values of the window
	//gtk_about_dialog_set_authors(about_dialog, authors);

	gtk_about_dialog_set_logo_icon_name(about_dialog, "image-x-generic");

	// SHow the dialog
	gtk_window_set_position(about_dialog, GTK_WIN_POS_CENTER_ALWAYS);
	gtk_show_about_dialog(about_dialog, "title", ("About ExampleCode"), NULL);
}
*/
