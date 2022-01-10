//#include "error_dialogs.h"
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include "import.h"
#include <libpq-fe.h>
#include "main.h"
#include "new.h"
#include <stdio.h>

PGresult *tagres;
GtkWidget *tag_dialog, *tag_entry, *tag_vbox;

static void on_tag_apply(GtkWidget *widget, gpointer data) {
	//char psql_error[2048];
	const gchar *text;
	char tag_query[80 + sizeof(text)];

	text = gtk_entry_get_text(GTK_ENTRY(tag_entry));

	strcpy(tag_query, "INSERT INTO public.tags (category, name) VALUES (0, '");
	strcat(tag_query, text);
	strcat(tag_query, "') ON CONFLICT DO NOTHING;");
	PQexec(conn, tag_query);
	strcpy(tag_query, "");	// clear the query string

	// the query is done, now the window must be destroyed
	gtk_widget_destroy(tag_dialog);
	gtk_notebook_detach_tab(notebook, tag_vbox);
}

static void on_tag_cancel(GtkWidget *widget, gpointer data) {
	gtk_widget_destroy(tag_dialog);
	gtk_notebook_detach_tab(notebook, tag_vbox);
}

extern void new_tag_activate(void) {
	GtkWidget *label, *button0, *button1, *bbox;
	gint page_count = gtk_notebook_get_n_pages(notebook);
	
	tag_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_container_set_border_width(GTK_CONTAINER(tag_vbox), 10);
	gtk_box_set_baseline_position(tag_vbox, GTK_BASELINE_POSITION_TOP);
	
	/* Label */
	label = gtk_label_new("Enter the artist's name here:");
	gtk_widget_set_valign(label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(tag_vbox), label, FALSE, FALSE, 0);
	
	/* Entry */
	tag_entry = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(tag_entry), TRUE);
	gtk_widget_set_valign(tag_entry, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(tag_vbox), tag_entry, TRUE, TRUE, 0);
	
	/* Button box */
	bbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_button_box_set_layout(bbox, GTK_BUTTONBOX_START);
	gtk_container_add(GTK_CONTAINER(tag_vbox), bbox);
	
	/* Cancel button */
	button0 = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(bbox), button0, TRUE, TRUE, 0);
	gtk_button_set_label(button0, "Cancel");
	g_signal_connect(button0, "clicked", G_CALLBACK(on_tag_cancel), NULL);
	
	/* Ok button */
	button1 = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(bbox), button1, TRUE, TRUE, 0);
	gtk_button_set_label(button1, "Add");
	g_signal_connect(button1, "clicked", G_CALLBACK(on_tag_apply), NULL);
	
	gtk_widget_show_all(tag_vbox);
	gtk_container_add(GTK_CONTAINER(notebook), tag_vbox);
	gtk_notebook_set_tab_label_text(notebook, tag_vbox, "New tag");
	gtk_notebook_set_current_page(notebook, page_count);
}
