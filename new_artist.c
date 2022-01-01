#include "error_dialogs.h"
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include "import.h"
#include <libpq-fe.h>
#include "main.h"
#include "new.h"
#include <stdio.h>

PGresult *artres;
//char conninfo[] = "dbname=atsugami"; /* Sets the database for dbconnect() */
//char main_psql_error[2048];
GtkWidget *dialog_window, *entry;

	//g_signal_connect(G_OBJECT(dialog_window), "close", G_CALLBACK(on_artist_close), NULL);
	//g_signal_connect(G_OBJECT(dialog_window), "prepare", G_CALLBACK(on_), NULL);

static void on_artist_apply(GtkWidget *widget, gpointer data) {
	//char psql_error[2048];
	const gchar *text;
	int query_size = (sizeof(text) + 70);

	char query_base[] = "INSERT INTO public.artists (name) VALUES ('";
	char artist_query[query_size];

	strcpy(artist_query, query_base);

	text = gtk_entry_get_text(GTK_ENTRY(entry));
	/*
	if (gtk_entry_get_text_length(GTK_ENTRY(entry) == 0)) {
		fprintf(stderr, "This cannot be null\n");		// add an info bar here
	} */
	//if (gtk_entry_get_text_length(GTK_ENTRY(entry) > 0)) {
		strcat(artist_query, text);
	//}

	strcat(artist_query, "') ON CONFLICT DO NOTHING;");
	PQexec(conn, artist_query);
	strcpy(artist_query, "");	// clear the query string

	// the query is done, now the window must be destroyed
	gtk_widget_destroy(dialog_window);
}

static void on_artist_cancel(GtkWidget *widget, gpointer data) {
	gtk_widget_destroy(dialog_window);
}

/* Quick and dirty solution */
extern void new_artist_activate(void) {
	GtkWidget *label, *button0, *button1, *vbox, *bbox;

	//dialog_window = gtk_window_new(GTK_WINDOW_POPUP);
	dialog_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
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

	/* bbox */		// replace with gtk button box
	bbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_container_add(GTK_CONTAINER(vbox), bbox);

	/* buttons */
	button0 = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(bbox), button0, TRUE, TRUE, 0);
	gtk_button_set_label(button0, "Cancel");
	g_signal_connect(button0, "clicked", G_CALLBACK(on_artist_cancel), NULL);

	button1 = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(bbox), button1, TRUE, TRUE, 0);
	gtk_button_set_label(button1, "Add");
	g_signal_connect(button1, "clicked", G_CALLBACK(on_artist_apply), NULL);

	//gtk_box_set_spacing(bbox, GTK_BUTTONBOX_CENTER);
	/* SHow the window */
	gtk_window_set_resizable(dialog_window, FALSE);
	gtk_window_set_position(dialog_window, GTK_WIN_POS_CENTER_ALWAYS);
	gtk_window_set_default_size(GTK_WINDOW(dialog_window), 300, 100);
	gtk_window_set_title(dialog_window, "Add artist");
	gtk_widget_show_all(vbox);
	gtk_widget_show_all(dialog_window);
}
