#include <gtk-3.0/gtk/gtk.h>
#include <gtk/gtk.h>
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
GtkEntry *entry0, *entry1, *entry2, *entry3, *entry4, *entry5, *entry6;
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
	int wc = 0;
	const char *text0 = gtk_entry_get_text(entry0);	/* Source */
	const char *text1 = gtk_entry_get_text(entry1);	/* Artists */
	const char *text2 = gtk_entry_get_text(entry2);	/* Copyrights */
	const char *text3 = gtk_entry_get_text(entry3);	/* Characters */
	const char *text4 = gtk_entry_get_text(entry4);	/* General */
	const char *text5 = gtk_entry_get_text(entry5);	/* Meta */
	const char *text6 = gtk_entry_get_text(entry6);	/* Invisible */
	//const char query_string[159 + strlen(text0)];
	/*
	const char *text0 = NULL;
	const char *text1 = NULL;
	const char *text2 = NULL;
	const char *text3 = NULL;
	const char *text4 = NULL;
	const char *text5 = NULL;
	const char *text6 = NULL;
	*/
	const char *value0;
	const char *value1;
	const char *value2;
	const char *value3;
	const char *value4;
	const char *value5;
	const char *value6;

	strcpy(&value0, &text0);
	strcpy(&value1, &text1);
	strcpy(&value2, &text2);
	strcpy(&value3, &text3);
	strcpy(&value4, &text4);
	strcpy(&value5, &text5);
	strcpy(&value6, &text6);

	const int src_size = strlen(&value0);
	const int art_size = strlen(&value1);
	const int cop_size = strlen(&value2);
	const int cha_size = strlen(&value3);
	const int gen_size = strlen(&value4);
	const int met_size = strlen(&value5);
	const char query_string[159 + src_size];
	char art_arr[art_size + 2];
	char cop_arr[cop_size + 2];
	char cha_arr[cha_size + 2];
	char gen_arr[gen_size + 2];
	char met_arr[met_size + 2];
	char art_tag[art_size];
	char cop_tag[cop_size];
	char cha_tag[cha_size];
	char gen_tag[gen_size];
	char met_tag[met_size];

	printf("art_size is %lu\n", strlen(text1));

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

	/*
	wiz_res = PQexec(conn, query_string);

	if (PQresultStatus(wiz_res) == PGRES_COMMAND_OK) {
		PQclear(wiz_res);

		wiz_res = PQexec(conn, "COMMIT TRANSACTION;");
		PQclear(wiz_res);
		printf("Query successful.\n");
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
	*/

	/* Create artist tags */
	strcpy(art_arr, text1);
	printf("\'%s\'\n", art_arr);

	art_arr[strlen(art_arr) - 1] = ' ';
	art_arr[strlen(art_arr)] = '\0';
	printf("\'%s\'\n", art_arr);

	b = 0;

	for (a = 0; a < strlen(art_arr); a++) {
		if (isspace(art_arr[a]) == 0) {
			art_tag[b] = art_arr[a];
			++b;
		}
		else {
			printf("%s %d %d\n", art_tag, a, b);
			for (c = 1; c < b; c++) {
				art_tag[c] = '\0';
			}

			++wc;
			b = 0;
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
	gtk_entry_set_activates_default(entry0, TRUE);
	gtk_widget_set_valign(GTK_WIDGET(entry0), GTK_ALIGN_START);
	gtk_entry_set_placeholder_text(entry0, "Enter the URL to import here");
	gtk_box_pack_start(GTK_BOX(import_page), GTK_WIDGET(entry0), TRUE, TRUE, 0);

	/* Artist tag box */
	label1 = gtk_label_new("Artist(s)");
	gtk_widget_set_halign(label1, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), label1, FALSE, FALSE, 0);

	entry1 = gtk_entry_new();
	gtk_entry_set_activates_default(entry1, TRUE);
	gtk_widget_set_valign(GTK_WIDGET(entry1), GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), GTK_WIDGET(entry1), TRUE, TRUE, 0);

	/* Copyright tag box */
	label2 = gtk_label_new("Copyrights");
	gtk_widget_set_halign(label2, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), label2, FALSE, FALSE, 0);

	entry2 = gtk_entry_new();
	gtk_entry_set_activates_default(entry2, TRUE);
	gtk_widget_set_valign(GTK_WIDGET(entry2), GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), GTK_WIDGET(entry2), TRUE, TRUE, 0);

	/* Character tag box */
	label3 = gtk_label_new("Characters");
	gtk_widget_set_halign(label3, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), label3, FALSE, FALSE, 0);

	entry3 = gtk_entry_new();
	gtk_entry_set_activates_default(entry3, TRUE);
	gtk_widget_set_valign(GTK_WIDGET(entry3), GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), GTK_WIDGET(entry3), TRUE, TRUE, 0);

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
	gtk_entry_set_activates_default(entry5, TRUE);
	gtk_widget_set_valign(GTK_WIDGET(entry5), GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), GTK_WIDGET(entry5), TRUE, TRUE, 0);

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
	gtk_entry_set_activates_default(entry6, TRUE);
	g_object_bind_property(cbox, "active-id", GTK_WIDGET(entry6), "text", G_BINDING_DEFAULT);

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
