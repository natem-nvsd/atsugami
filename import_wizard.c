#include <ctype.h>
#include <gtk/gtk.h>
#include "import.h"
#include <libpq-fe.h>
#include "main.h"
#include "notebook.h"
#include "file_count.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "import_wizard.h"
#include <wand/MagickWand.h>

PGresult *wiz_res;
GtkWidget *label0, *label1, *label2, *label3, *label4, *label5, *label6;
//GtkEntry *entry0, *entry1, *entry2, *entry3, *entry5, *entry6;
GtkEntry *entry0, *entry1, *entry2, *entry3, *entry5, *entry6;
GtkTextView *tv;
GtkTextBuffer *tb;
GtkTextIter istart, iend;
GtkWidget *import_thumb, *cbox, *can_button, *imp_button;
GdkPixbuf *import_thumb_pixbuf, *thumb_pixbuf;
GtkImage *diag_ico;
gint page_count = 0;
gboolean parent_bool = FALSE;
gboolean child_bool = FALSE;
gboolean has_children = FALSE;
GtkWidget *header_box, *scrolled_window;
GtkStyleContext *contxt0, *contxt1, *contxt2;

static void cancel_button_cb(void) {
	gtk_notebook_detach_tab(notebook, scrolled_window);
}

static void incomplete_diag(const char *label) {
	GtkWidget *diag;
	GtkDialogFlags diag_flags = GTK_RESPONSE_ACCEPT;

	/* Set the labels of required text fields to STYLE_CLASS_ERROR */
	contxt0 = gtk_widget_get_style_context(label1);
	contxt1 = gtk_widget_get_style_context(label4);
	contxt2 = gtk_widget_get_style_context(label6);

	gtk_style_context_add_class(contxt0, "error");
	gtk_style_context_add_class(contxt1, "error");
	gtk_style_context_add_class(contxt2, "error");

	gtk_label_set_markup(label1, "Artist(s)						<b>* This field is required.</b>");
	gtk_label_set_markup(label4, "General						<b>* This field is required.</b>");
	gtk_label_set_markup(label6, "Rating						<b>* This field is required.</b>");

	diag = gtk_message_dialog_new(window, diag_flags, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Form Error");

	gtk_message_dialog_format_secondary_text(diag, "%s", label);
	gtk_dialog_run(GTK_DIALOG(diag));
	gtk_widget_destroy(diag);

}

static void tag_process_fail_diag(const char *title, const char *body) {
	GtkWidget *diag;
	GtkDialogFlags diag_flags;

	diag_flags = GTK_RESPONSE_ACCEPT;
	diag = gtk_message_dialog_new(window, diag_flags, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", title);

	gtk_message_dialog_format_secondary_text(diag, "%s", body);
	gtk_dialog_run(GTK_DIALOG(diag));
	gtk_widget_destroy(diag);
}

static int import_button_cb(void) {
	/* Check if the text buffer was modified. This MUST be done before initializing text4. */
	if (gtk_text_buffer_get_modified(tb) == TRUE) {
		gtk_text_buffer_get_iter_at_offset(tb, &istart, 0);
		gtk_text_buffer_get_iter_at_offset(tb, &iend, -1);
	}
	else {
		incomplete_diag("One or more fields were not filled.\nPlease complete them and try again.");

		return 1;
	}

	char *tag_arr[6];
	int wc = 0;
	const size_t text0_size = (strlen(gtk_entry_get_text(entry0) + 1));
	const size_t text1_size = (strlen(gtk_entry_get_text(entry1) + 1));
	const size_t text2_size = (strlen(gtk_entry_get_text(entry2) + 1));
	const size_t text3_size = (strlen(gtk_entry_get_text(entry3) + 1));
	const size_t text4_size = (strlen(gtk_text_buffer_get_text(tb, &istart, &iend, FALSE) + 1));
	const size_t text5_size = (strlen(gtk_entry_get_text(entry5)));

	/* Check if text fields are empty. */
	if (text1_size == 0 || strlen(gtk_entry_get_text(entry6)) == 0) {
		incomplete_diag("One or more fields were not filled.\nPlease complete them and try again.");
		return 1;
	}

	char text0[text0_size];
	char text1[text1_size];
	char text2[text2_size];
	char text3[text3_size];
	char text4[text4_size];
	char text5[text5_size];
	char text6[2];

	printf("text2: '%s'\n", gtk_entry_get_text(entry2));
	printf("text3: '%s'\n\n", gtk_entry_get_text(entry3));

	sprintf(text0, "%s", gtk_entry_get_text(entry0));				// Source
	printf("text0: \'%s\'\n", text0);

	sprintf(text1, "%s ", gtk_entry_get_text(entry1));				// Artist
	printf("text1: \'%s\'\n", text1);

	sprintf(text2, "%s ", gtk_entry_get_text(entry2));				// Copyrights 
	printf("text2: \'%s\'\n", text2);

	sprintf(text3, "%s ", gtk_entry_get_text(entry3));				// Characters
	printf("text3: \'%s\'\n", text3);	// NULL for some reason...

	sprintf(text4, "%s ", gtk_text_buffer_get_text(tb, &istart, &iend, FALSE));	// General
	printf("text4: \'%s\'\n", text4);	// NULL for some reason...

	sprintf(text5, "%s ", gtk_entry_get_text(entry5));				// Meta
	printf("text5: \'%s\'\n", text5);

	sprintf(text6, "%s", gtk_entry_get_text(entry6));				// Rating
	printf("text6: \'%s\'\n", text6);


	char query_string[160 + text0_size];
	char file_id[sizeof(long)];
	char *cmd = NULL;
	char *path = NULL;
	char *size_names[5];
	int size_res[5];

	wiz_res = PQexec(conn, "BEGIN TRANSACTION");
	PQclear(wiz_res);

	/* Create the base of the query */
	strcpy(query_string, "INSERT INTO public.files (sha256, rating, source) VALUES ('");
	strcat(query_string, file_sha256);
	strcat(query_string, "', '");
	strcat(query_string, text6);
	strcat(query_string, "', '");
	sprintf(query_string, "%s') ON CONFLICT DO NOTHING;", text0);
	//strcat(query_string, "') ON CONFLICT DO NOTHING;");
	printf("\'%s\'\n", query_string);

	wiz_res = PQexec(conn, query_string);
	strcpy(query_string, "");

	if (PQresultStatus(wiz_res) == PGRES_COMMAND_OK) {
		PQclear(wiz_res);

		strcpy(query_string, "SELECT id FROM public.files WHERE sha256 = '");
		strcat(query_string, file_sha256);
		strcat(query_string, "';");

		/* If the file _was_ inserted into the database, then file_id is guaranteed to be non-null */
		wiz_res = PQexec(conn, query_string);
		strcpy(query_string, "");
		strcpy(file_id, PQgetvalue(wiz_res, 0, 0));	/* This appears to cause a segmentation fault. */
		PQclear(wiz_res);
	}
	else {
		char *err = NULL;
		PQclear(wiz_res);

		wiz_res = PQexec(conn, "ROLLBACK TRANSACTION;");

		gtk_notebook_detach_tab(notebook, scrolled_window);
		fprintf(stderr, "%s", PQerrorMessage(conn));
		sprintf(err, "%s", PQerrorMessage(conn));
		tag_process_fail_diag("Error", err);
		PQclear(wiz_res);
		return 1;
	}

	/* Create and apply tags */
	char *tag_str = NULL;	// Tags are copied here for string for processing.
	int catid;		// Category id
	int charid;
	char *text[5];

	strcpy(text[0], text1);
	strcpy(text[1], text2);
	strcpy(text[2], text3);
	strcpy(text[3], text4);
	strcpy(text[4], text5);

	for (catid = 1; catid < 6; catid++) {
		char buffer[strlen((text[catid]) + 1)];
		strcpy(buffer, text[catid]);

		for (charid = 0; charid <= strlen(tag_arr[catid]); charid++) {
			if (isspace(buffer[charid]) == 0)
				sprintf(tag_str, "%c", buffer[charid]);
			else {
				char query[72 + strlen(tag_str)];
				char tagid[sizeof(unsigned long)];

				/* Create the tag */
				sprintf(tag_str, "%c", ' ');
				printf("\'%s\'\n", tag_str);
				++wc;
				strcpy(query, "INSERT INTO public.tags (name) VALUES ('");
				strcat(query, tag_str);
				strcat(query, "') ON CONFLICT DO NOTHING;");

				wiz_res = PQexec(conn, query);

				if (PQresultStatus(wiz_res) == PGRES_COMMAND_OK) {
					/* Create the category-tag bridge */
					PQclear(wiz_res);
					strcpy(query, "SELECT id FROM public.tags WHERE name = '");
					strcat(query, tag_str);
					strcat(query, "');");

					wiz_res = PQexec(conn, query);

					strcpy(tagid, PQgetvalue(wiz_res, 0, 0));
					PQclear(wiz_res);
					strcpy(query, "INSERT INTO public.tags_categories (tag_id, category_id) VALUES (");
					strcat(query, tagid);
					sprintf(query, "%d", catid);
					strcat(query, ");");

					wiz_res = PQexec(conn, query);

					if (PQresultStatus(wiz_res) != PGRES_COMMAND_OK) {
						fprintf(stderr, "%s\n", PQerrorMessage(conn));
						gtk_notebook_detach_tab(notebook, scrolled_window);
						tag_process_fail_diag("Error", PQerrorMessage(conn));
						PQclear(wiz_res);

						wiz_res = PQexec(conn, "ROLLBACK TRANSACTION;");
						
						PQclear(wiz_res);
						return 1;
					}

					/* Create the tag-file bridge */
					strcpy(query, "INSERT INTO public.files_tags (file_id, category_id) VALUES (");
					strcat(query, file_id);
					strcat(query, ", ");
					sprintf(query, "%d", catid);
					strcat(query, ");");

					if (PQresultStatus(wiz_res) != PGRES_COMMAND_OK) {
						fprintf(stderr, "%s\n", PQerrorMessage(conn));
						gtk_notebook_detach_tab(notebook, scrolled_window);
						tag_process_fail_diag("Error", PQerrorMessage(conn));
						PQclear(wiz_res);

						wiz_res = PQexec(conn, "ROLLBACK TRANSACTION;");
						
						PQclear(wiz_res);
						return 1;
					}
				}

				else {
					fprintf(stderr, "%s\n", PQerrorMessage(conn));
					gtk_notebook_detach_tab(notebook, scrolled_window);
					tag_process_fail_diag("Error", PQerrorMessage(conn));
					PQclear(wiz_res);

					wiz_res = PQexec(conn, "ROLLBACK TRANSACTION;");
					
					PQclear(wiz_res);
					return 1;
				}
			}

			strcpy(tag_str, "");
		}

		strcpy(buffer, "");
	}

	/* TEMPORARY */
	wiz_res = PQexec(conn, "ROLLBACK TRANSACTION;");
	PQclear(wiz_res);
	return 1;
	/* END TEMPORARY */

	strcpy(query_string, "INSERT INTO public.tag_count (file_id, tag_count) VALUES (");
	strcat(query_string, file_id);
	strcat(query_string, ", ");
	sprintf(query_string, "%d", wc);
	strcat(query_string, ");");
	
	wiz_res = PQexec(conn, query_string);

	if (PQresultStatus(wiz_res) == PGRES_COMMAND_OK)
		PQclear(wiz_res);
	else {
		fprintf(stderr, "%s\n", PQerrorMessage(conn));
		PQclear(wiz_res);

		wiz_res = PQexec(conn, "ROLLBACK TRANSACTION;");

		gtk_notebook_detach_tab(notebook, scrolled_window);
		tag_process_fail_diag("Error", PQerrorMessage(conn));
		PQclear(wiz_res);
		return 1;
	}

	wiz_res = PQexec(conn, "COMMIT TRANSACTION;");

	/* Create thumbnails of the image
	 * Windows ports may not like the lack of file extensions.
	 */
	GError *thumb_write_err;

	size_names[0] = "gigantic";
	size_names[1] = "huge";
	size_names[2] = "large";
	size_names[3] = "medium";
	size_names[4] = "small";
	size_res[0] = 360;
	size_res[1] = 270;
	size_res[2] = 225;
	size_res[3] = 180;
	size_res[4] = 150;
	thumb_write_err = g_error_new(1, 1, "An error occured while creating the thumbnail.");
	wiz_res = PQexec(conn, "SELECT thumb_dir FROM public.settings;");

	for (catid = 0; catid <= 4; catid++) {
		//thumb_pixbuf = gdk_pixbuf_new_from_file_at_scale(import_file_path, size_res[a], size_res[a], TRUE, &thumb_write_err);
		thumb_pixbuf = gdk_pixbuf_new_from_file_at_scale(import_file_path, size_res[catid], size_res[catid], TRUE, NULL);

		printf("strcpy\n");
		strcpy(cmd, PQgetvalue(wiz_res, 0, 0));
		strcat(cmd, "/");
		sprintf(cmd, "%s", size_names[catid]);
		strcat(cmd, "/");
		strcat(cmd, file_sha256);
		//gdk_pixbuf_save(thumb_pixbuf, cmd, "png", &thumb_write_err, NULL);
	//	gdk_pixbuf_save(thumb_pixbuf, cmd, "png", &thumb_write_err);
		gdk_pixbuf_save(thumb_pixbuf, cmd, "jpeg", &thumb_write_err, "quality", "100", NULL);

	//	GNOME example: gdk_pixbuf_save(pixbuf, handle, "jpeg", &error, "quality", "100", NULL);

		printf("%s\n", cmd);
		strcpy(path, "");
	}

	PQclear(wiz_res);

	/* Move the file to the storage directory */
	wiz_res = PQexec(conn, "SELECT store_dir FROM public.settings;");
	printf("path: %s\n", PQgetvalue(wiz_res, 0, 0));

	strcpy(cmd, "mv ");
	strcat(cmd, import_file_path);
	strcat(cmd, " /");
	strcat(cmd, PQgetvalue(wiz_res, 0, 0));
	strcat(cmd, "/");
	strcat(cmd, file_sha256);
	system(cmd);

	strcpy(path, "");
	PQclear(wiz_res);

	// Why is this an implicit declaration? file_count.h is included.
	file_count_update(file_label, vbox);

	PQclear(wiz_res);
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
	tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));

	gtk_text_view_set_editable(tv, TRUE);
	gtk_text_view_set_accepts_tab(tv, FALSE);
	gtk_box_pack_start(GTK_BOX(import_page), tv, TRUE, TRUE, 0);
	gtk_text_view_set_wrap_mode(tv, GTK_WRAP_WORD_CHAR);
	gtk_text_buffer_get_bounds(tb, &istart, &iend);

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
	gtk_notebook_set_tab_reorderable(notebook, scrolled_window, TRUE);
}
