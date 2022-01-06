//#include "error_dialogs.h"
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include "import.h"
#include <libpq-fe.h>
#include "main.h"
#include "new.h"
#include <stdio.h>

PGresult *characterres;
GtkWidget *entry_2_electric_boogaloo, *char_vbox;

static void on_character_apply(GtkWidget *widget, gpointer data) {
	//char psql_error[2048];
	const gchar *text;
	int query_size = (sizeof(text) + 70);

	char query_base[] = "INSERT INTO public.characters (name) VALUES ('";
	char character_query[query_size];

	strcpy(character_query, query_base);

	text = gtk_entry_get_text(GTK_ENTRY(entry_2_electric_boogaloo));
	/*
	if (gtk_entry_get_text_length(GTK_ENTRY(entry) == 0)) {
		fprintf(stderr, "This cannot be null\n");		// add an info bar here
	} */
	//if (gtk_entry_get_text_length(GTK_ENTRY(entry) > 0)) {
		strcat(character_query, text);
	//}

	strcat(character_query, "') ON CONFLICT DO NOTHING;");
	PQexec(conn, character_query);
	strcpy(character_query, "");	// clear the query string

	gtk_notebook_detach_tab(notebook, char_vbox);
}

static void on_character_cancel(GtkWidget *widget, gpointer data) {
	gtk_notebook_detach_tab(notebook, char_vbox);
}

extern void new_character_activate(void) {
	GtkWidget *label, *button0, *button1, *bbox;
	gint page_count = gtk_notebook_get_n_pages(notebook);

	char_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_container_set_border_width(GTK_CONTAINER(char_vbox), 10);
	
	/* label */
	label = gtk_label_new("Enter the character's name here:");
	gtk_widget_set_valign(label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(char_vbox), label, FALSE, FALSE, 0);
	
	/* entry */
	entry_2_electric_boogaloo = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry_2_electric_boogaloo), TRUE);
	gtk_widget_set_valign(entry_2_electric_boogaloo, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(char_vbox), entry_2_electric_boogaloo, TRUE, TRUE, 0);
	
	/* bbox */              // replace with gtk button box
	bbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_button_box_set_layout(bbox, GTK_BUTTONBOX_START);
	gtk_container_add(GTK_CONTAINER(char_vbox), bbox);
	
	/* buttons */
	button0 = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(bbox), button0, TRUE, TRUE, 0);
	gtk_button_set_label(button0, "Cancel");
	g_signal_connect(button0, "clicked", G_CALLBACK(on_character_cancel), NULL);
	
	button1 = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(bbox), button1, TRUE, TRUE, 0);
	gtk_button_set_label(button1, "Add");
	g_signal_connect(button1, "clicked", G_CALLBACK(on_character_apply), NULL);
	
	gtk_widget_show_all(char_vbox);
	gtk_container_add(GTK_CONTAINER(notebook), char_vbox);
	gtk_notebook_set_tab_label_text(notebook, char_vbox, "New character");
	gtk_notebook_set_current_page(notebook, page_count);
}
