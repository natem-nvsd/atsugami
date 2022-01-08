//#include "error_dialogs.h"
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
GtkWidget *entry, *art_vbox;

static void on_artist_apply(GtkWidget *widget, gpointer data) {
	//char psql_error[2048];
	const gchar *text;
	char query_base[] = "INSERT INTO public.artists (name, created_at) VALUES ('";
	char artist_query[90 + sizeof(text)];

	strcpy(artist_query, query_base);

	text = gtk_entry_get_text(GTK_ENTRY(entry));
	/*
	if (gtk_entry_get_text_length(GTK_ENTRY(entry) == 0)) {
		fprintf(stderr, "This cannot be null\n");		// add an info bar here
	} */
	//if (gtk_entry_get_text_length(GTK_ENTRY(entry) > 0)) {
		strcat(artist_query, text);
	//}

	strcat(artist_query, "', now()) ON CONFLICT DO NOTHING;");
	PQexec(conn, artist_query);
	strcpy(artist_query, "");	// clear the query string

	// the query is done, now the window must be destroyed
	gtk_notebook_detach_tab(notebook, art_vbox);
}

static void on_artist_cancel(GtkWidget *widget, gpointer data) {
	gtk_notebook_detach_tab(notebook, art_vbox);
}

extern void new_artist_activate(void) {
	GtkWidget *label, *button0, *button1, *bbox;
	gint page_count = gtk_notebook_get_n_pages(notebook);

	art_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_container_set_border_width(GTK_CONTAINER(art_vbox), 10);
	gtk_box_set_baseline_position(art_vbox, GTK_BASELINE_POSITION_TOP);

	/* Label */
	label = gtk_label_new("Enter the artist's name here:");
	gtk_widget_set_valign(label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(art_vbox), label, FALSE, FALSE, 0);

	/* Entry */
	entry = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
	gtk_widget_set_valign(entry, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(art_vbox), entry, TRUE, TRUE, 0);

	/* Button box */
	bbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_button_box_set_layout(bbox, GTK_BUTTONBOX_START);
	gtk_container_add(GTK_CONTAINER(art_vbox), bbox);

	/* Cancel button */
	button0 = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(bbox), button0, TRUE, TRUE, 0);
	gtk_button_set_label(button0, "Cancel");
	g_signal_connect(button0, "clicked", G_CALLBACK(on_artist_cancel), NULL);

 	/* Ok button */
	button1 = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(bbox), button1, TRUE, TRUE, 0);
	gtk_button_set_label(button1, "Add");
	g_signal_connect(button1, "clicked", G_CALLBACK(on_artist_apply), NULL);

	gtk_widget_show_all(art_vbox);
	gtk_container_add(GTK_CONTAINER(notebook), art_vbox);
	gtk_notebook_set_tab_label_text(notebook, art_vbox, "New artist");
	gtk_notebook_set_current_page(notebook, page_count);
}
