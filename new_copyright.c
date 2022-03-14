/* new_copyright.c */
#include "atsugami.h"
#include "import.h"
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdio.h>

PGresult *copyrightres;
GtkWidget *copyright_dialog, *copyright_entry, *copyright_vbox;

static int on_copyright_apply(GtkWidget *widget, gpointer data) {
	size_t txt_size = strlen(gtk_entry_get_text(GTK_ENTRY(copyright_entry)));
	char text[txt_size];
	char copyright_query[80 + txt_size];

	copyrightres = PQexec(conn, "BEGIN TRANSACTION;");

	PQclear(copyrightres);
	sprintf(copyright_query, "INSERT INTO public.tags (name) VALUES ('%s') ON CONFLICT DO NOTHING;", text);

	copyrightres = PQexec(conn, copyright_query);

	if (PQresultStatus(copyrightres) != PGRES_COMMAND_OK) {
		PQclear(copyrightres);

		copyrightres = PQexec(conn, "ROLLBACK TRANSACTION;");

		PQclear(copyrightres);
		gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), copyright_vbox);
		return 1;
	}

	sprintf(copyright_query, "SELECT id FROM public.tags WHERE name LIKE '%s';", text);

	copyrightres = PQexec(conn, copyright_query);

	sprintf(copyright_query, "INSERT INTO public.tags_categories (tag_id, category_id) VALUES (%s, 1);", PQgetvalue(copyrightres, 0, 0));
	PQclear(copyrightres);

	copyrightres = PQexec(conn, copyright_query);

	if (PQresultStatus(copyrightres) != PGRES_COMMAND_OK) {
		PQclear(copyrightres);

		copyrightres = PQexec(conn, "ROLLBACK TRANSACTION;");
		PQclear(copyrightres);
		gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), copyright_vbox);
		return 1;
	}

	if (PQresultStatus(copyrightres) == PGRES_COMMAND_OK) {
		PQclear(copyrightres);

		copyrightres = PQexec(conn, "COMMIT TRANSACTION;");
		PQclear(copyrightres);
		gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), copyright_vbox);
	}
	return 0;
}

static void on_copyright_cancel(GtkWidget *widget, gpointer data) {
	copyrightres = PQexec(conn, "ROLLBACK TRANSACTION;");
	PQclear(copyrightres);
	gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), copyright_vbox);
}

extern void new_copyright_activate(void) {
	GtkWidget *label, *button0, *button1, *bbox;
	gint page_count = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
	
	copyright_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_container_set_border_width(GTK_CONTAINER(copyright_vbox), 10);
	gtk_box_set_baseline_position(GTK_BOX(copyright_vbox), GTK_BASELINE_POSITION_TOP);
	
	/* Label */
	label = gtk_label_new("Enter the copyright's name here:");
	gtk_widget_set_valign(label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(copyright_vbox), label, FALSE, FALSE, 0);
	
	/* Entry */
	copyright_entry = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(copyright_entry), TRUE);
	gtk_widget_set_valign(copyright_entry, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(copyright_vbox), copyright_entry, TRUE, TRUE, 0);
	
	/* Button box */
	bbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_START);
	gtk_container_add(GTK_CONTAINER(copyright_vbox), bbox);
	
	/* Cancel button */
	button0 = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(bbox), button0, TRUE, TRUE, 0);
	gtk_button_set_label(GTK_BUTTON(button0), "Cancel");
	g_signal_connect(button0, "clicked", G_CALLBACK(on_copyright_cancel), NULL);
	
	/* Ok button */
	button1 = gtk_button_new();
	gtk_box_pack_start(GTK_BOX(bbox), button1, TRUE, TRUE, 0);
	gtk_button_set_label(GTK_BUTTON(button1), "Add");
	g_signal_connect(button1, "clicked", G_CALLBACK(on_copyright_apply), NULL);
	
	gtk_widget_show_all(copyright_vbox);
	gtk_container_add(GTK_CONTAINER(notebook), copyright_vbox);
	gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(notebook), copyright_vbox, "New copyright");
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), page_count);
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), copyright_vbox, TRUE);
}
