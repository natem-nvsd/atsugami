#include <gtk-3.0/gtk/gtk.h>
#include "import.h"
#include <libpq-fe.h>
#include "main.h"
#include "notebook.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "import_wizard.h"

PGresult *wiz_res;
GtkWidget *label0, *label1, *label2, *label3, *label4, *label5, *label6;
GtkWidget *entry0, *entry1, *entry2, *entry3, *entry4, *entry5, *entry6;
GtkTextView *tv;
GtkWidget *import_thumb, *cbox, *can_button, *imp_button;
GdkPixbuf *import_thumb_pixbuf;

gint page_count = 0;
gboolean parent_bool = FALSE;
gboolean child_bool = FALSE;
gboolean has_children = FALSE;
GtkWidget *header_box, *scrolled_window;

static void cancel_button_cb(void) {
	wiz_res = PQexec(conn, "ROLLBACK TRANSACTION");

	PQclear(wiz_res);
	gtk_notebook_detach_tab(notebook, scrolled_window);
}

static int import_button_cb(void) {
	int a, b, c;
	int wc;
	const gchar *text0 = gtk_entry_get_text(GTK_ENTRY(entry0));	/* Source */
	const gchar *text1 = gtk_entry_get_text(GTK_ENTRY(entry1));	/* Artists */
	const gchar *text2 = gtk_entry_get_text(GTK_ENTRY(entry2));	/* Copyrights */
	const gchar *text3 = gtk_entry_get_text(GTK_ENTRY(entry3));	/* Characters */
	const gchar *text4 = gtk_entry_get_text(GTK_ENTRY(entry4));	/* General */
	const gchar *text5 = gtk_entry_get_text(GTK_ENTRY(entry5));	/* Meta */
	const gchar *text6 = gtk_entry_get_text(GTK_ENTRY(entry6));	/* Invisible */
	int src_size = sizeof(text0);
	int art_size = sizeof(text1);
	int cop_size = sizeof(text2);
	int cha_size = sizeof(text3);
	int gen_size = sizeof(text4);
	int met_size = sizeof(text5);
	char query_string[159 + src_size];
	char art_arr[67 + art_size];
	char cop_arr[67 + cop_size];
	char cha_arr[67 + cha_size];
	char gen_arr[67 + gen_size];
	char met_arr[67 + met_size];
	char art_tag[art_size];
	char cop_tag[cop_size];
	char cha_tag[cha_size];
	char gen_tag[gen_size];
	char met_tag[met_size];

	wiz_res = PQexec(conn, "BEGIN TRANSACTION");
	PQclear(wiz_res);

	/* Create the base of the query */
	strcpy(query_string, "INSERT INTO public.files (sha256, rating, source) VALUES ('");
	strcat(query_string, file_sha256);
	strcat(query_string, "', '");
	strcat(query_string, text6);
	strcat(query_string, "', '");
	strcat(query_string, text0);
	strcat(query_string, "') ON CONFLICT DO NOTHING;");

	wiz_res = PQexec(conn, query_string);

	if (PQresultStatus(wiz_res) == PGRES_COMMAND_OK) {
		PQclear(wiz_res);

		wiz_res = PQexec(conn, "COMMIT TRANSACTION;");
		PQclear(wiz_res);
		printf("Command successful.\n");
	}
	else {
		PQclear(wiz_res);

		wiz_res = PQexec(conn, "ROLLBACK TRANSACTION;");
		PQclear(wiz_res);
		gtk_notebook_detach_tab(notebook, scrolled_window);
		fprintf(stderr, "Command failed.\n");
		// Spawn an error dialog here
		return 1;
	}

	/* Create artist tags */
	b = 0;

	strcpy(art_arr, text1);
	printf("%s\n", art_arr);

	for (a = 0; a < art_size; a++) {
		if (isspace(art_arr[a]) == 0) {
			art_tag[b] = text1[a];
			printf("%s %d %d\n", art_tag, a, b);
			++b;
		}
		else {
			printf("%s %d %d\n", art_tag, a, b);

			for (c = 0; c < a; c++) {
				art_tag[c] = '\0';
			}

			b = 0;
			strcpy(query_string, "INSERT INTO public.tags (name) VALUES ('");
		}
	}

	gtk_notebook_detach_tab(notebook, scrolled_window);
	return 0;
}

extern void import_wizard(GtkWidget *import_page, gpointer user_data) {
	GtkWidget *button_box;

	import_page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	page_count = gtk_notebook_get_n_pages(notebook);
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);

	gtk_container_set_border_width(GTK_CONTAINER(import_page), 10);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), import_page);
	gtk_scrolled_window_set_kinetic_scrolling(GTK_SCROLLED_WINDOW(scrolled_window), TRUE);

	/* thumbnail */
	import_thumb_pixbuf = gdk_pixbuf_new_from_file_at_scale(import_file_path, 720, 720, TRUE, NULL);
	import_thumb = gtk_image_new_from_pixbuf(import_thumb_pixbuf);
	
	gtk_box_pack_start(GTK_BOX(import_page), import_thumb, FALSE, FALSE, 0);

	/* Source */
	label0 = gtk_label_new("Enter source URL here:");
	gtk_widget_set_halign(label0, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), label0, FALSE, FALSE, 0);
	
	entry0 = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry0), TRUE);
	gtk_widget_set_valign(entry0, GTK_ALIGN_START);
	gtk_entry_set_placeholder_text(GTK_ENTRY(entry0), "Enter the URL to import here");
	gtk_box_pack_start(GTK_BOX(import_page), entry0, TRUE, TRUE, 0);

	/* Artist tag box */
	label1 = gtk_label_new("Artist(s)");
	gtk_widget_set_halign(label1, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), label1, FALSE, FALSE, 0);

	entry1 = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry1), TRUE);
	gtk_widget_set_valign(entry1, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), entry1, TRUE, TRUE, 0);

	/* Copyright tag box */
	label2 = gtk_label_new("Copyrights");
	gtk_widget_set_halign(label2, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), label2, FALSE, FALSE, 0);

	entry2 = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry2), TRUE);
	gtk_widget_set_valign(entry2, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), entry2, TRUE, TRUE, 0);

	/* Character tag box */
	label3 = gtk_label_new("Characters");
	gtk_widget_set_halign(label3, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), label3, FALSE, FALSE, 0);

	entry3 = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry3), TRUE);
	gtk_widget_set_valign(entry3, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), entry3, TRUE, TRUE, 0);

	/* General tag box */
	label4 = gtk_label_new("General");
	gtk_widget_set_halign(label4, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), label4, FALSE, FALSE, 0);

	tv = gtk_text_view_new();
	gtk_text_view_set_editable(tv, TRUE);
	gtk_widget_set_valign(tv, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), tv, TRUE, TRUE, 0);
	
	/* Meta tag box */
	label5 = gtk_label_new("Meta");
	gtk_widget_set_halign(label5, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), label5, FALSE, FALSE, 0);

	entry5 = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry5), TRUE);
	gtk_widget_set_valign(entry5, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), entry5, TRUE, TRUE, 0);

	/* Rating */
	label6 = gtk_label_new("Rating:");
	gtk_widget_set_halign(label6, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), label6, FALSE, FALSE, 0);
	
	cbox = gtk_combo_box_text_new();
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(cbox), "s", "Safe");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(cbox), "q", "Questionable");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(cbox), "e", "Explicit");
	gtk_box_pack_start(GTK_BOX(import_page), cbox, FALSE, FALSE, 0);

	/* Invisible entry field */
	entry6 = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry6), TRUE);
	g_object_bind_property(cbox, "active-id", entry6, "text", G_BINDING_DEFAULT);

	button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_pack_start(GTK_BOX(import_page), button_box, FALSE, FALSE, 0);
	
	can_button = gtk_button_new_with_label("Cancel");
	imp_button = gtk_button_new_with_label("Import");
	
	gtk_button_box_set_layout(GTK_BUTTON_BOX(button_box), GTK_BUTTONBOX_START);
	gtk_box_pack_start(GTK_BOX(button_box), can_button, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(button_box), imp_button, FALSE, FALSE, 0);
	
	g_signal_connect(can_button, "clicked", G_CALLBACK(cancel_button_cb), NULL);
	g_signal_connect(imp_button, "clicked", G_CALLBACK(import_button_cb), NULL);
	
	/* init stuff */
	gtk_widget_show_all(import_page);
	gtk_widget_show_all(scrolled_window);
	gtk_container_add(GTK_CONTAINER(notebook), scrolled_window);
	gtk_scrolled_window_set_propagate_natural_height(GTK_SCROLLED_WINDOW(scrolled_window), TRUE);
	gtk_notebook_set_tab_label_text(notebook, scrolled_window, "Import");
	gtk_notebook_set_current_page(notebook, page_count);
}
