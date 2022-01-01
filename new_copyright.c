#include "error_dialogs.h"
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include "import.h"
#include <libpq-fe.h>
#include "main.h"
#include "new_copyright.h"
#include <stdio.h>

PGresult *copyres;
GtkWidget *diddley_dee_dialog, *damn_entry;

static void on_copyright_apply(GtkWidget *widget, gpointer data) {
	//char psql_error[2048];
	const gchar *text;
	int query_size = (sizeof(text) + 70);

	char query_base[] = "INSERT INTO public.copyrights (name) VALUES ('";
	char copyright_query[query_size];

	strcpy(copyright_query, query_base);

	text = gtk_entry_get_text(GTK_ENTRY(damn_entry));
	/*
	if (gtk_entry_get_text_length(GTK_ENTRY(damn_entry) == 0)) {
		fprintf(stderr, "This cannot be null\n");		// add an info bar here
	} */
	//if (gtk_entry_get_text_length(GTK_ENTRY(damn_entry) > 0)) {
		strcat(copyright_query, text);
	//}

	strcat(copyright_query, "') ON CONFLICT DO NOTHING;");
	PQexec(conn, copyright_query);
	strcpy(copyright_query, "");	// clear the query string

	// the query is done, now the window must be destroyed
	gtk_widget_destroy(diddley_dee_dialog);
}

static void on_copyright_cancel(GtkWidget *widget, gpointer data) {
	gtk_widget_destroy(diddley_dee_dialog);
}

/* Quick and dirty solution */
extern void new_copyright_activate(void) {
	GtkWidget *label, *button0, *button1, *vbox, *hbox;

	//diddley_dee_dialog = gtk_window_new(GTK_WINDOW_POPUP);
	diddley_dee_dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 0);
	gtk_container_add(GTK_CONTAINER(diddley_dee_dialog), vbox);

	/* label */
	label = gtk_label_new("Enter the copyright's name here:");
	gtk_widget_set_valign(label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

	/* entry */
	damn_entry = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(damn_entry), TRUE);
	gtk_widget_set_valign(damn_entry, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(vbox), damn_entry, TRUE, TRUE, 0);

	/* hbox */		// replace with gtk button box
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), 0);
	gtk_container_add(GTK_CONTAINER(vbox), hbox);

	/* buttons */
	button0 = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(hbox), button0, TRUE, TRUE, 0);
	gtk_button_set_label(button0, "Cancel");
	g_signal_connect(button0, "clicked", G_CALLBACK(on_copyright_cancel), NULL);

	button1 = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(hbox), button1, TRUE, TRUE, 0);
	gtk_button_set_label(button1, "Add");
	g_signal_connect(button1, "clicked", G_CALLBACK(on_copyright_apply), NULL);

	/* SHow the window */
	gtk_window_set_resizable(diddley_dee_dialog, FALSE);
	gtk_window_set_position(diddley_dee_dialog, GTK_WIN_POS_CENTER_ALWAYS);
	gtk_window_set_default_size(GTK_WINDOW(diddley_dee_dialog), 600, 400);
	gtk_window_set_title(diddley_dee_dialog, "Add copyright");
	gtk_widget_show_all(vbox);
	gtk_widget_show_all(diddley_dee_dialog);
}
