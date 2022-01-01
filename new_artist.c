#include "error_dialogs.h"
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include "import.h"
#include <libpq-fe.h>
#include "main.h"
#include "new_artist.h"
#include <stdio.h>

PGresult *artres;
//char conninfo[] = "dbname=atsugami"; /* Sets the database for dbconnect() */
//char main_psql_error[2048];

/* Quick and dirty solution */
extern void new_artist_activate(void) {		/* This shouldn't be in main.c. Move to another file soon */
	GtkWidget *dialog_window, *label, *entry, *button0, *button1, *vbox, *hbox;

	//dialog_window = gtk_window_new(GTK_WINDOW_POPUP);
	dialog_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 0);
	gtk_container_add(GTK_CONTAINER(dialog_window), vbox);

	/* label */
	label = gtk_label_new("Enter the artist's name here:");
	gtk_widget_set_valign(label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

	/* entry */
	entry = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
	gtk_widget_set_valign(entry, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(vbox), entry, TRUE, TRUE, 0);

	/* hbox */
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), 0);
	gtk_container_add(GTK_CONTAINER(vbox), hbox);

	/* buttons */
	button0 = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(hbox), button0, TRUE, TRUE, 0);
	gtk_button_set_label(button0, "Cancel");

	button1 = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(hbox), button1, TRUE, TRUE, 0);
	gtk_button_set_label(button1, "Add");

	/* SHow the window */
	gtk_window_set_resizable(dialog_window, FALSE);
	gtk_window_set_position(dialog_window, GTK_WIN_POS_CENTER_ALWAYS);
	gtk_window_set_default_size(GTK_WINDOW(dialog_window), 600, 400);
	gtk_window_set_title(dialog_window, "Add artist");
	gtk_widget_show_all(vbox);
	gtk_widget_show_all(dialog_window);
}
