/* new_character.c */
#include "main.h"
#include "callbacks.h"
#include "import.h"
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdio.h>

PGresult *characterres;
GtkWidget *character_dialog, *character_entry, *character_vbox;

static int on_character_apply(GtkWidget *widget, gpointer data) {
	size_t txt_size = strlen(gtk_entry_get_text(GTK_ENTRY(character_entry)));
	char text[txt_size];
	char character_query[80 + txt_size];

	characterres = PQexec(conn, "BEGIN TRANSACTION;");
	PQclear(characterres);

	sprintf(character_query, "INSERT INTO public.tags (name) VALUES ('%s') ON CONFLICT DO NOTHING;", text);

	characterres = PQexec(conn, character_query);

	if (PQresultStatus(characterres) != PGRES_COMMAND_OK) {
		PQclear(characterres);

		characterres = PQexec(conn, "ROLLBACK TRANSACTION;");
		PQclear(characterres);
		gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), character_vbox);
		return 1;
	}

	sprintf(character_query, "SELECT id FROM public.tags WHERE name LIKE '%s';", text);

	characterres = PQexec(conn, character_query);

	sprintf(character_query, "INSERT INTO public.tags_categories (tag_id, category_id) VALUES (%s, 2);", PQgetvalue(characterres, 0, 0));
	PQclear(characterres);

	characterres = PQexec(conn, character_query);

	if (PQresultStatus(characterres) != PGRES_COMMAND_OK) {
		PQclear(characterres);

		characterres = PQexec(conn, "ROLLBACK TRANSACTION;");
		PQclear(characterres);
		gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), character_vbox);
		return 1;
	}
	else {
		PQclear(characterres);

		characterres = PQexec(conn, "COMMIT TRANSACTION;");
		PQclear(characterres);
		gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), character_vbox);
		return 0;
	}
}

static void on_character_cancel(GtkWidget *widget, gpointer data) {
	characterres = PQexec(conn, "ROLLBACK TRANSACTION;");
	PQclear(characterres);
	gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), character_vbox);
}

extern void new_character_activate(void) {
	GtkWidget *label, *button0, *button1, *bbox;

	gint page_count = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
	character_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

	gtk_container_set_border_width(GTK_CONTAINER(character_vbox), 10);
	gtk_box_set_baseline_position(GTK_BOX(character_vbox), GTK_BASELINE_POSITION_TOP);
	
	/* Label */
	label = gtk_label_new("Enter the character's name here:");
	gtk_widget_set_valign(label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(character_vbox), label, FALSE, FALSE, 0);
	
	/* Entry */
	character_entry = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(character_entry), TRUE);
	gtk_widget_set_valign(character_entry, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(character_vbox), character_entry, TRUE, TRUE, 0);
	
	/* Button box */
	bbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_START);
	gtk_container_add(GTK_CONTAINER(character_vbox), bbox);
	
	/* Cancel button */
	button0 = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(bbox), button0, TRUE, TRUE, 0);
	gtk_button_set_label(GTK_BUTTON(button0), "Cancel");
	g_signal_connect(button0, "clicked", G_CALLBACK(on_character_cancel), NULL);
	
	/* Ok button */
	button1 = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(bbox), button1, TRUE, TRUE, 0);
	gtk_button_set_label(GTK_BUTTON(button1), "Add");
	g_signal_connect(button1, "clicked", G_CALLBACK(on_character_apply), NULL);
	
	gtk_widget_show_all(character_vbox);
	gtk_container_add(GTK_CONTAINER(notebook), character_vbox);
	gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(notebook), character_vbox, "New character");
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), page_count);
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), character_vbox, TRUE);
}
