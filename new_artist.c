//#include "error_dialogs.h"
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include "import.h"
#include <libpq-fe.h>
#include "main.h"
#include "new.h"
#include <stdio.h>

PGresult *artistres;
GtkWidget *artist_dialog, *artist_entry, *artist_vbox;

static int on_artist_apply(GtkWidget *widget, gpointer data) {
	const gchar *tmp_text = gtk_entry_get_text(GTK_ENTRY(artist_entry));
	const char text[sizeof(tmp_text)];
	char artist_query[80 + sizeof(text)];
	const gchar *id = NULL;

	strcpy(text, tmp_text);
	artistres = PQexec(conn, "BEGIN TRANSACTION;");
	PQclear(artistres);

	strcpy(artist_query, "INSERT INTO public.tags (name) VALUES ('");
	strcat(artist_query, text);
	strcat(artist_query, "') ON CONFLICT DO NOTHING;");

	artistres = PQexec(conn, artist_query);
	strcpy(artist_query, "");	// clear the query string

	//if (PQresultStatus(artistres) != PGRES_TUPLES_OK) {
	if (PQresultStatus(artistres) != PGRES_COMMAND_OK) {
		PQclear(artistres);

		artistres = PQexec(conn, "ROLLBACK TRANSACTION;");
		PQclear(artistres);
		gtk_notebook_detach_tab(notebook, artist_vbox);
		return 1;
	}

	strcpy(artist_query, "SELECT id FROM public.tags WHERE name LIKE '");
	strcat(artist_query, text);
	strcat(artist_query, "';");

	artistres = PQexec(conn, artist_query);
	id = PQgetvalue(artistres, 0, 0);

	PQclear(artistres);
	strcpy(artist_query, "");	// clear the query string

	strcpy(artist_query, "INSERT INTO public.tags_categories (tag_id, category_id) VALUES (");
	strcat(artist_query, id);
	strcat(artist_query, ", 1);");

	artistres = PQexec(conn, artist_query);

	if (PQresultStatus(artistres) != PGRES_COMMAND_OK) {
		PQclear(artistres);

		artistres = PQexec(conn, "ROLLBACK TRANSACTION;");
		PQclear(artistres);
		gtk_notebook_detach_tab(notebook, artist_vbox);
		return 1;
	}

	if (PQresultStatus(artistres) == PGRES_COMMAND_OK) {
		PQclear(artistres);

		artistres = PQexec(conn, "COMMIT TRANSACTION;");
		PQclear(artistres);
		gtk_notebook_detach_tab(notebook, artist_vbox);
	}
	return 0;
}

static void on_artist_cancel(GtkWidget *widget, gpointer data) {
	artistres = PQexec(conn, "ROLLBACK TRANSACTION;");
	PQclear(artistres);
	gtk_notebook_detach_tab(notebook, artist_vbox);
}

extern void new_artist_activate(void) {
	GtkWidget *label, *button0, *button1, *bbox;
	gint page_count = gtk_notebook_get_n_pages(notebook);
	
	artist_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_container_set_border_width(GTK_CONTAINER(artist_vbox), 10);
	gtk_box_set_baseline_position(GTK_BOX(artist_vbox), GTK_BASELINE_POSITION_TOP);
	
	/* Label */
	label = gtk_label_new("Enter the artist's name here:");
	gtk_widget_set_valign(label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(artist_vbox), label, FALSE, FALSE, 0);
	
	/* Entry */
	artist_entry = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(artist_entry), TRUE);
	gtk_widget_set_valign(artist_entry, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(artist_vbox), artist_entry, TRUE, TRUE, 0);
	
	/* Button box */
	bbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_START);
	gtk_container_add(GTK_CONTAINER(artist_vbox), bbox);
	
	/* Cancel button */
	button0 = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(bbox), button0, TRUE, TRUE, 0);
	gtk_button_set_label(GTK_BUTTON(button0), "Cancel");
	g_signal_connect(button0, "clicked", G_CALLBACK(on_artist_cancel), NULL);
	
	/* Ok button */
	button1 = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(bbox), button1, TRUE, TRUE, 0);
	gtk_button_set_label(GTK_BUTTON(button1), "Add");
	g_signal_connect(button1, "clicked", G_CALLBACK(on_artist_apply), NULL);
	
	gtk_widget_show_all(artist_vbox);
	gtk_container_add(GTK_CONTAINER(notebook), artist_vbox);
	gtk_notebook_set_tab_label_text(notebook, artist_vbox, "New artist");
	gtk_notebook_set_current_page(notebook, page_count);
	gtk_notebook_set_tab_reorderable(notebook, artist_vbox, TRUE);
}
