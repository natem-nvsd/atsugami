//#include "error_dialogs.h"
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include "import.h"
#include <libpq-fe.h>
#include "main.h"
#include "new.h"
#include <stdio.h>

PGresult *copyres;
GtkWidget *damn_entry, *copy_vbox;

static void on_copyright_apply(GtkWidget *widget, gpointer data) {
	//char psql_error[2048];
	const gchar *text;
	char copyright_query[80 + sizeof(text)];

	text = gtk_entry_get_text(GTK_ENTRY(damn_entry));

	strcpy(copyright_query, "INSERT INTO public.tags (category, name) VALUES (3, '");
	strcat(copyright_query, text);
	strcat(copyright_query, "') ON CONFLICT DO NOTHING;");
	PQexec(conn, copyright_query);
	strcpy(copyright_query, "");	// clear the query string

	gtk_notebook_detach_tab(notebook, copy_vbox);
}

static void on_copyright_cancel(GtkWidget *widget, gpointer data) {
	gtk_notebook_detach_tab(notebook, copy_vbox);
}

extern void new_copyright_activate(void) {
	GtkWidget *label, *button0, *button1, *bbox;
	gint page_count = gtk_notebook_get_n_pages(notebook);

	copy_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_container_set_border_width(GTK_CONTAINER(copy_vbox), 10);
	
	/* label */
	label = gtk_label_new("Enter the copyright's name here:");
	gtk_widget_set_valign(label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(copy_vbox), label, FALSE, FALSE, 0);
	
	/* entry */
	damn_entry = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(damn_entry), TRUE);
	gtk_widget_set_valign(damn_entry, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(copy_vbox), damn_entry, TRUE, TRUE, 0);
	
	/* bbox */              // replace with gtk button box
	bbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_button_box_set_layout(bbox, GTK_BUTTONBOX_START);
	gtk_container_add(GTK_CONTAINER(copy_vbox), bbox);
	
	/* buttons */
	button0 = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(bbox), button0, TRUE, TRUE, 0);
	gtk_button_set_label(button0, "Cancel");
	g_signal_connect(button0, "clicked", G_CALLBACK(on_copyright_cancel), NULL);
	
	button1 = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(bbox), button1, TRUE, TRUE, 0);
	gtk_button_set_label(button1, "Add");
	g_signal_connect(button1, "clicked", G_CALLBACK(on_copyright_apply), NULL);

	gtk_widget_show_all(copy_vbox);
	gtk_container_add(GTK_CONTAINER(notebook), copy_vbox);
	gtk_notebook_set_tab_label_text(notebook, copy_vbox, "New copyright");
	gtk_notebook_set_current_page(notebook, page_count);
}
