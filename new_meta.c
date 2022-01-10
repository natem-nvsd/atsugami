#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include "import.h"
#include <libpq-fe.h>
#include "main.h"
#include "new.h"
#include <stdio.h>

PGresult *metares;
GtkWidget *meta_dialog, *meta_entry, *meta_vbox;

static void on_meta_apply(GtkWidget *widget, gpointer data) {
	//char psql_error[2048];
	const gchar *text;
	char meta_query[80 + sizeof(text)];

	text = gtk_entry_get_text(GTK_ENTRY(meta_entry));

	strcpy(meta_query, "INSERT INTO public.tags (category, name) VALUES (5, '");
	strcat(meta_query, text);
	strcat(meta_query, "') ON CONFLICT DO NOTHING;");
	PQexec(conn, meta_query);
	strcpy(meta_query, "");	// clear the query string

	// the query is done, now the window must be destroyed
	gtk_widget_destroy(meta_dialog);
	gtk_notebook_detach_tab(notebook, meta_vbox);
}

static void on_meta_cancel(GtkWidget *widget, gpointer data) {
	gtk_widget_destroy(meta_dialog);
	gtk_notebook_detach_tab(notebook, meta_vbox);
}

extern void new_meta_tag_activate(void) {
	GtkWidget *label, *button0, *button1, *bbox;
	gint page_count = gtk_notebook_get_n_pages(notebook);
	
	meta_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_container_set_border_width(GTK_CONTAINER(meta_vbox), 10);
	gtk_box_set_baseline_position(meta_vbox, GTK_BASELINE_POSITION_TOP);
	
	/* Label */
	label = gtk_label_new("Enter the artist's name here:");
	gtk_widget_set_valign(label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(meta_vbox), label, FALSE, FALSE, 0);
	
	/* Entry */
	meta_entry = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(meta_entry), TRUE);
	gtk_widget_set_valign(meta_entry, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(meta_vbox), meta_entry, TRUE, TRUE, 0);
	
	/* Button box */
	bbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_button_box_set_layout(bbox, GTK_BUTTONBOX_START);
	gtk_container_add(GTK_CONTAINER(meta_vbox), bbox);
	
	/* Cancel button */
	button0 = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(bbox), button0, TRUE, TRUE, 0);
	gtk_button_set_label(button0, "Cancel");
	g_signal_connect(button0, "clicked", G_CALLBACK(on_meta_cancel), NULL);
	
	/* Ok button */
	button1 = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(bbox), button1, TRUE, TRUE, 0);
	gtk_button_set_label(button1, "Add");
	g_signal_connect(button1, "clicked", G_CALLBACK(on_meta_apply), NULL);
	
	gtk_widget_show_all(meta_vbox);
	gtk_container_add(GTK_CONTAINER(notebook), meta_vbox);
	gtk_notebook_set_tab_label_text(notebook, meta_vbox, "New meta");
	gtk_notebook_set_current_page(notebook, page_count);
}
