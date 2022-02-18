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

GtkWidget *label0, *label1, *label2, *label3, *label4, *label5, *label6;
GtkWidget *entry0, *entry1, *entry2, *entry3, *entry5, *entry6;
//GtkTextView *tv;
GtkWidget *tv;
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

	gtk_label_set_markup(GTK_LABEL(label1), "Artist(s)\t\t\t\t\t<b>* This field is required.</b>");
	gtk_label_set_markup(GTK_LABEL(label4), "General\t\t\t\t\t<b>* This field is required.</b>");
	gtk_label_set_markup(GTK_LABEL(label6), "Rating\t\t\t\t\t<b>* This field is required.</b>");

	diag = gtk_message_dialog_new(GTK_WINDOW(window), diag_flags, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Form Error");

	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(diag), "%s", label);
	gtk_dialog_run(GTK_DIALOG(diag));
	gtk_widget_destroy(diag);

}

static void tag_process_fail_diag(const char *title, const char *body, ...) {
//static void tag_process_fail_diag(char *__restrict __format, char *__restrict __format, ...) {
	GtkWidget *diag;
	GtkDialogFlags diag_flags;

	diag_flags = GTK_RESPONSE_ACCEPT;
	diag = gtk_message_dialog_new(GTK_WINDOW(window), diag_flags, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", title);

	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(diag), "%s", body);
	gtk_dialog_run(GTK_DIALOG(diag));
	gtk_widget_destroy(diag);
}

static int import_button_cb(void) {
	register PGresult *wiz_res;
	/* Check if the text buffer was modified. This MUST be done before initializing text4, or there will be a segmentation fault. */
	if (gtk_text_buffer_get_modified(tb) == TRUE) {
		gtk_text_buffer_get_iter_at_offset(tb, &istart, 0);
		gtk_text_buffer_get_iter_at_offset(tb, &iend, -1);
	}
	else {
		incomplete_diag("One or more fields were not filled.\nPlease complete them and try again.");

		return 1;
	}

	int wc = 0;
	const size_t text0_size = (strlen(gtk_entry_get_text(GTK_ENTRY(entry0)) + 1));
	const size_t text1_size = (strlen(gtk_entry_get_text(GTK_ENTRY(entry1)) + 1));
	const size_t text2_size = (strlen(gtk_entry_get_text(GTK_ENTRY(entry2)) + 1));
	const size_t text3_size = (strlen(gtk_entry_get_text(GTK_ENTRY(entry3)) + 1));
	const size_t text4_size = (strlen(gtk_text_buffer_get_text(tb, &istart, &iend, FALSE) + 1));
	const size_t text5_size = (strlen(gtk_entry_get_text(GTK_ENTRY(entry5))));

	/* Check if text fields are empty. */
	if (text1_size == 0 || strlen(gtk_entry_get_text(GTK_ENTRY(entry6))) == 0) {
		incomplete_diag("One or more fields were not filled.\nPlease complete them and try again.");
		return 1;
	}

	char query_string[160 + text0_size];
	char file_id[sizeof(long)];
	char *size_names[5];
	int size_res[5];
	char *text[5];
	char text6[2];

			// allocate enough memory for tag strings
	text[0] = (char *) malloc(text1_size);		// FREED
	text[1] = (char *) malloc(text2_size);		// FREED
	text[2] = (char *) malloc(text3_size);		// FREED
	text[3] = (char *) malloc(text4_size);		// FREED
	text[4] = (char *) malloc(text5_size);		// FREED
	size_names[0] = "small";
	size_names[1] = "medium";
	size_names[2] = "large";
	size_names[3] = "huge";
	size_names[4] = "gigantic";
	size_res[0] = 150;
	size_res[1] = 180;
	size_res[2] = 225;
	size_res[3] = 270;
	size_res[4] = 360;

	// Whitespace is needed at the end to ensure the tag processing algorithm works properely.
	sprintf(text[0], "%s ", gtk_entry_get_text(GTK_ENTRY(entry1)));				// Artist
	sprintf(text[1], "%s ", gtk_entry_get_text(GTK_ENTRY(entry2)));				// Copyrights
	sprintf(text[2], "%s ", gtk_entry_get_text(GTK_ENTRY(entry3)));				// Characters
	sprintf(text[3], "%s ", gtk_text_buffer_get_text(tb, &istart, &iend, FALSE));	// General
	sprintf(text[4], "%s ", gtk_entry_get_text(GTK_ENTRY(entry5)));				// Meta
	sprintf(text6,	 "%s",  gtk_entry_get_text(GTK_ENTRY(entry6)));				// Rating

	wiz_res = PQexec(conn, "BEGIN TRANSACTION");
	PQclear(wiz_res);

	sprintf(query_string, "INSERT INTO public.files (sha256, rating, source) VALUES (\'%s\', \'%s\', \'%s\') ON CONFLICT DO NOTHING;", file_sha256, text6, gtk_entry_get_text(GTK_ENTRY(entry0)));
	printf("%s\n", query_string);

	wiz_res = PQexec(conn, query_string);
	strcpy(query_string, "");

	if (PQresultStatus(wiz_res) == PGRES_COMMAND_OK) {
		PQclear(wiz_res);

		sprintf(query_string, "SELECT id FROM public.files WHERE sha256 = '%s';", file_sha256);
		printf("%s\n\n", query_string);

		/* If the file _was_ inserted into the database, then file_id is guaranteed to be non-null */
		wiz_res = PQexec(conn, query_string);
		strcpy(query_string, "");
		strcpy(file_id, PQgetvalue(wiz_res, 0, 0));
		PQclear(wiz_res);
	}
	else {
		PQclear(wiz_res);

		wiz_res = PQexec(conn, "ROLLBACK TRANSACTION;");

		gtk_notebook_detach_tab(notebook, scrolled_window);
		fprintf(stderr, "%s", PQerrorMessage(conn));
		tag_process_fail_diag("Error", "Atsugami encountered an error:\n%s", PQerrorMessage(conn));	// This doesn't work; printf "%s" instead of PQerrorMessage's value
		PQclear(wiz_res);
		return 1;
	}

	/* Create and apply tags
	 * Move everything before the for loop closer to the start of the function */
	int catid;	// Category id
	int charid;	// Current charcter

	/* Create tags and apply them to the file being imported */
	for (catid = 0; catid < 5; catid++) {
		size_t text_size = strlen(text[catid]);
		char buffer[text_size];
		char tag_str[text_size];	// Tags are copied here for string for processing.
		int bufid;

		sprintf(buffer, "%s", text[catid]);
		printf("\ncategory id: %d\n", (catid + 1));
		printf("buffer: '%s'\n", buffer);

		for (charid = 0, bufid = 0; charid < strlen(buffer); charid++) {
			/* "--", "'", and """ must be prevented from insertion to prevent SQL injection and errors.
			 * Input should be sanitized _before_ it *EVER* gets into a query. */
			if (isspace(buffer[charid]) == 0) {
				sprintf(&tag_str[bufid], "%c", buffer[charid]);
				++bufid;
			}
			else {
				char query[strlen(tag_str) + 72];
				char tagid[sizeof(unsigned long)];

				bufid = 0;

				++wc;
				/* Check if the tag exists, then create it if it doesn't */
				sprintf(query, "SELECT EXISTS (SELECT id FROM public.tags WHERE name = '%s');", tag_str);

				wiz_res = PQexec(conn, query);

				switch (strcmp(PQgetvalue(wiz_res, 0, 0), "t")) {
					case 0:		// The tag already exists (wiz_res == "t")
						printf("This tag already exists.\n");
						/* Create the file-tag bridge */
						sprintf(query, "INSERT INTO public.files_tags (file_id, tag_id) VALUES (%s, %s);", file_id, PQgetvalue(wiz_res, 0, 0));
						printf("%s\n\n", query);

						wiz_res = PQexec(conn, query);	// I can't believe I forgot to execute the query until _now_

						if (PQresultStatus(wiz_res) != PGRES_COMMAND_OK) {
							fprintf(stderr, "%s\n", PQerrorMessage(conn));
							gtk_notebook_detach_tab(notebook, scrolled_window);
							tag_process_fail_diag("Error", "Atsugami encountered an error:\n%s", PQerrorMessage(conn));
							PQclear(wiz_res);

							wiz_res = PQexec(conn, "ROLLBACK TRANSACTION;");
							
							PQclear(wiz_res);
							return 1;
						}

						PQclear(wiz_res);
						break;
					case 1:		// The tag does not exist (wiz_res == "f")
						PQclear(wiz_res);
						sprintf(query, "INSERT INTO public.tags (name) VALUES ('%s') ON CONFLICT DO NOTHING;", tag_str);
						printf("%s\n", query);

						wiz_res = PQexec(conn, query);

						if (PQresultStatus(wiz_res) == PGRES_COMMAND_OK) {
							/* Create the tag-category bridge */
							PQclear(wiz_res);
							sprintf(query, "SELECT id FROM public.tags WHERE name = '%s';", tag_str);
							printf("%s\n", query);

							wiz_res = PQexec(conn, query);

							sprintf(tagid, "%s", PQgetvalue(wiz_res, 0, 0));
							PQclear(wiz_res);
							sprintf(query, "INSERT INTO public.tags_categories (tag_id, category_id) VALUES (%s, %d);", tagid, catid);
							printf("%s\n", query);

							wiz_res = PQexec(conn, query);

							if (PQresultStatus(wiz_res) != PGRES_COMMAND_OK) {
								fprintf(stderr, "%s\n", PQerrorMessage(conn));
								gtk_notebook_detach_tab(notebook, scrolled_window);
								tag_process_fail_diag("Error", "Atsugami encountered an error:\n%s", PQerrorMessage(conn));
								PQclear(wiz_res);

								wiz_res = PQexec(conn, "ROLLBACK TRANSACTION;");
								
								PQclear(wiz_res);
								return 1;
							}

							/* Create the file-tag bridge */
							sprintf(query, "INSERT INTO public.files_tags (file_id, tag_id) VALUES (%s, %s);", file_id, tagid);
							printf("%s\n\n", query);

							wiz_res = PQexec(conn, query);	// I can't believe I forgot to execute the query until _now_

							if (PQresultStatus(wiz_res) != PGRES_COMMAND_OK) {
								fprintf(stderr, "%s\n", PQerrorMessage(conn));
								gtk_notebook_detach_tab(notebook, scrolled_window);
								tag_process_fail_diag("Error", "Atsugami encountered an error:\n%s", PQerrorMessage(conn));
								PQclear(wiz_res);

								wiz_res = PQexec(conn, "ROLLBACK TRANSACTION;");
								
								PQclear(wiz_res);
								return 1;
							}
						}
						else {
							fprintf(stderr, "%s\n", PQerrorMessage(conn));
							gtk_notebook_detach_tab(notebook, scrolled_window);
							tag_process_fail_diag("Error", "Atsugami encountered an error:\n%s", PQerrorMessage(conn));
							PQclear(wiz_res);

							wiz_res = PQexec(conn, "ROLLBACK TRANSACTION;");
							
							PQclear(wiz_res);
							return 1;
						}

						break;
				}
			}

		}

		strcpy(buffer, "");
		strcpy(tag_str, "");
	}

	sprintf(query_string, "UPDATE public.files SET count = %d WHERE id = %s;", (wc - 1), file_id);
	
	wiz_res = PQexec(conn, query_string);

	if (PQresultStatus(wiz_res) == PGRES_COMMAND_OK)
		PQclear(wiz_res);
	else {
		fprintf(stderr, "%s\n", PQerrorMessage(conn));
		PQclear(wiz_res);

		wiz_res = PQexec(conn, "ROLLBACK TRANSACTION;");

		gtk_notebook_detach_tab(notebook, scrolled_window);
		tag_process_fail_diag("Error", "%s", PQerrorMessage(conn));
		PQclear(wiz_res);
		return 1;
	}

	wiz_res = PQexec(conn, "COMMIT TRANSACTION;");

	/* Create thumbnails of the image
	 * Windows ports _may_ not like the lack of file extensions.
	 */
	char *cmd = NULL;
	int cmd_code;

	wiz_res = PQexec(conn, "SELECT thumb_dir FROM public.settings;");
	size_t path_size = strlen(PQgetvalue(wiz_res, 0, 0));
	cmd = (char *) malloc(21 + path_size);

	for (catid = 0; catid <= 4; catid++) {
		thumb_pixbuf = gdk_pixbuf_new_from_file_at_scale(import_file_path, size_res[catid], size_res[catid], TRUE, NULL);

		sprintf(cmd, "%s/%s/%s.png", PQgetvalue(wiz_res, 0, 0), size_names[catid], file_sha256);	// Storage path, size, sha256
		printf("thumb path: %s\n", cmd);
		gdk_pixbuf_save(thumb_pixbuf, cmd, "png", NULL);
	}

	cmd = NULL;

	free(cmd);		// segfault?
	PQclear(wiz_res);
	//printf("\n\nDONE\n\n");

	/* Move the file to the storage directory */
	wiz_res = PQexec(conn, "SELECT store_dir FROM public.settings;");
	path_size = (strlen(PQgetvalue(wiz_res, 0, 0)) + 12);
	cmd = realloc(cmd, (path_size));

	sprintf(cmd, "chmod 600 %s", import_file_path);
	printf("%s\n", cmd);

	cmd_code = system(cmd);

	if (cmd_code != 0)
		fprintf(stderr, "chmod returned: %d\n", cmd_code);

	/* clear memory and reallocate */
	free(cmd);

	cmd = NULL;
	cmd = realloc(cmd, (path_size));

	sprintf(cmd, "mv %s /%s/%s", import_file_path, PQgetvalue(wiz_res, 0, 0), file_sha256);	// mv original_path import_path/files/file_sha256
	printf("%s\n", cmd);

	cmd_code = system(cmd);

	if (cmd_code != 0) {
		fprintf(stderr, "mv command returned: %d\n", cmd_code);
		return cmd_code;
	}

	free(cmd);
	PQclear(wiz_res);
	for (catid = 0; catid < 5; catid++) {
		free(text[catid]);
		printf("freed %d\n", catid);
	}

	printf("memory freed\n");
	file_count_update(file_label, vbox);	// Why is this an implicit declaration? file_count.h is included.
	printf("count updated\n");
	gtk_notebook_detach_tab(notebook, scrolled_window);
	printf("Finished\n");
	return 0;
}

extern void import_wizard(void) {
	GtkWidget *button_box, *import_page;

	import_page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	page_count = gtk_notebook_get_n_pages(notebook);
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);

	gtk_container_set_border_width(GTK_CONTAINER(import_page), 10);
	gtk_container_add(GTK_CONTAINER(scrolled_window), import_page);
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
	tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));

	gtk_text_view_set_editable(GTK_TEXT_VIEW(tv), TRUE);
	gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(tv), FALSE);
	gtk_box_pack_start(GTK_BOX(import_page), tv, TRUE, TRUE, 0);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tv), GTK_WRAP_WORD_CHAR);
	gtk_text_buffer_get_bounds(tb, &istart, &iend);

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
	gtk_notebook_set_tab_reorderable(notebook, scrolled_window, TRUE);
}
