/* import_wizard.c (c) 2021-2022 by Nate Morrison */
#include "atsugami.h"
#include "import.h"
#include <ctype.h>
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

GtkWidget *source_label, *artist_label, *copyright_label, *character_label, *general_label, *meta_label, *rating_label, *source_entry, *artist_label, *copyright_entry, *character_entry, *meta_entry, *rating_entry, *tv, *import_thumb, *cbox, *can_button, *imp_button, *header_box, *scrolled_window;
GtkTextBuffer *tb;
GtkTextIter istart, iend;
GdkPixbuf *import_thumb_pixbuf, *thumb_pixbuf;
GtkImage *diag_ico;
gint page_count = 0;
gboolean parent_bool = FALSE;
gboolean child_bool = FALSE;
gboolean has_children = FALSE;
GtkStyleContext *contxt0, *contxt1, *contxt2;

static void cancel_button_cb(void) {
	gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), scrolled_window);

	if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook)) == 0)
		quit_activate();
}

static void incomplete_diag(const char *label) {
	GtkWidget *diag;
	GtkDialogFlags diag_flags = GTK_RESPONSE_ACCEPT;

	/* Set the labels of required text fields to STYLE_CLASS_ERROR */
	contxt0 = gtk_widget_get_style_context(artist_label);
	contxt1 = gtk_widget_get_style_context(general_label);
	contxt2 = gtk_widget_get_style_context(rating_label);

	gtk_style_context_add_class(contxt0, "error");
	gtk_style_context_add_class(contxt1, "error");
	gtk_style_context_add_class(contxt2, "error");

	gtk_label_set_markup(GTK_LABEL(artist_label), "Artist(s)\t\t\t\t\t<b>* This field is required.</b>");
	gtk_label_set_markup(GTK_LABEL(general_label), "General\t\t\t\t\t<b>* This field is required.</b>");
	gtk_label_set_markup(GTK_LABEL(rating_label), "Rating\t\t\t\t\t<b>* This field is required.</b>");

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
	PGresult *wiz_res;
	/* Check if the text buffer was modified. This MUST be done before initializing text4, or there will be a segmentation fault. */
	if (gtk_text_buffer_get_modified(tb) == TRUE) {
		gtk_text_buffer_get_iter_at_offset(tb, &istart, 0);
		gtk_text_buffer_get_iter_at_offset(tb, &iend, -1);
	}
	else {
		incomplete_diag("One or more fields were not filled.\nPlease complete them and try again.");

		return 1;
	}

	size_t src_size = 0, size_res[5];
	char file_id[sizeof(unsigned long)], *size_names[5], *tags[5], *tag_buf[5], *src, *rating;
	int catid, wc = 0, iter, charid;	// Category id, tag count

	/* allocate memory for tag strings */
	src = (char *) gtk_entry_get_text(GTK_ENTRY(source_entry));			// Source
	tag_buf[0] = (char *) gtk_entry_get_text(GTK_ENTRY(artist_label));		// Artists
	tag_buf[1] = (char *) gtk_entry_get_text(GTK_ENTRY(copyright_entry));		// Copyrights
	tag_buf[2] = (char *) gtk_entry_get_text(GTK_ENTRY(character_entry));		// Characters
	tag_buf[3] = (char *) gtk_text_buffer_get_text(tb, &istart, &iend, FALSE);	// General
	tag_buf[4] = (char *) gtk_entry_get_text(GTK_ENTRY(meta_entry));		// Meta
	rating = (char *) gtk_entry_get_text(GTK_ENTRY(rating_entry));			// Rating
	src_size = strlen(src);								// Source size

	char query_string[161 + src_size];

	/* Check if text fields are empty. */
	if (strlen(tag_buf[2]) == 0 || strlen(rating) == 0) {
		incomplete_diag("One or more fields were not filled.\nPlease complete them and try again.");
		return 1;
	}

	/* Whitespace is needed at the end to ensure the tag processing algorithm works properely.
	 * If the last character of a string is " ", the last tag of a string will be looped.
	 *
	 * Remove all spaces from the end of tag strings
	 *
	 * iter represents tag string iteration
	 * catid represents tag buffer string (tex_buf[])
	 */
	for (iter = 0; iter < 5; iter++) {
		char *buffer = tag_buf[iter];
		size_t buf_size = strlen(buffer);	// temp
		int i;

		printf("%d, '%s'\n", iter, tag_buf[iter]);
		for (i = (buf_size - 1); buffer[buf_size] != ' ' && buf_size > 0; i--) {
			//printf("i = %d\n", i);
			//printf("'%c' ", buffer[i]);

			if (buffer[i] != ' ') {
				//printf("not space\n");
				//printf("'%c'\n", buffer[i]);
				size_res[iter] = (i + 1);

				tags[iter] = (char *) malloc(iter + 1);
				snprintf(tags[iter], (i + 3), "%s ", buffer);
				printf("'%s'\n\n", tags[iter]);

				break;
			}
		}
	}

	/* Clear tag_buf */
	for (iter = 0; iter < 5; iter++)
		strcpy(tag_buf[iter], "");

	printf("END\n");
	wiz_res = PQexec(conn, "BEGIN TRANSACTION");
	PQclear(wiz_res);

	sprintf(query_string, "INSERT INTO public.files (sha256, rating, source) VALUES ('%s', '%s', '%s') ON CONFLICT DO NOTHING;", file_sha256, rating, src);
	printf("%s\n", query_string);

	wiz_res = PQexec(conn, query_string);
	strcpy(query_string, "");

	/* Check command status */
	if (PQresultStatus(wiz_res) == PGRES_COMMAND_OK) {
		PQclear(wiz_res);
		sprintf(query_string, "SELECT id FROM public.files WHERE sha256 = '%s';", file_sha256);
		printf("%s\n\n", query_string);

		/* If the file _was_ inserted into the database, then file_id is guaranteed to be non-null */
		wiz_res = PQexec(conn, query_string);
		printf("'%s'\n", PQgetvalue(wiz_res, 0, 0));
		strcpy(file_id, PQgetvalue(wiz_res, 0, 0));
		PQclear(wiz_res);
	}
	else {
		PQclear(wiz_res);
		wiz_res = PQexec(conn, "ROLLBACK TRANSACTION;");

		gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), scrolled_window);
		fprintf(stderr, "%s", PQerrorMessage(conn));
		tag_process_fail_diag("Error", "Atsugami encountered an error:\n%s", PQerrorMessage(conn));	// This doesn't work; printf "%s" instead of PQerrorMessage's value
		PQclear(wiz_res);
		return 1;
	}

	/* Create and apply tags
	 * Move everything before the for loop closer to the start of the function 
	 *
	 * Create tags and apply them to the file being imported */
	for (catid = 0; catid < 5; catid++) {
		int bufid;
		size_t text_size = strlen(tags[catid]);
		char buffer[text_size], tag_str[text_size];

		sprintf(buffer, "%s", tags[catid]);
		printf("\ncategory id: %d\n", catid);
		printf("buffer: '%s'\n", buffer);
		for (charid = 0, bufid = 0; charid < strlen(buffer); charid++) {
			/* TODO: sanitize input
			 * Use PQexecParams() to insert data into the database without treating it as SQL syntax.
			 * See https://www.postgresql.org/docs/14/libpq-exec.html#LIBPQ-PQEXECPARAMS for more details. */
			if (isspace(buffer[charid]) == 0) {
				sprintf(&tag_str[bufid], "%c", buffer[charid]);
				++bufid;
			}
			else {
				char query[strlen(tag_str) + 72], tagid[sizeof(unsigned long)], tmp_tagid[sizeof(unsigned long)];

				printf("'%s'\n", tag_str);
				bufid = 0;
				++wc;

				/* Check if the tag exists, then create it if it doesn't */
				sprintf(query, "SELECT EXISTS (SELECT id FROM public.tags WHERE name = '%s');", tag_str);
				printf("%s\n", query);
				wiz_res = PQexec(conn, query);

				if (strcmp(PQgetvalue(wiz_res, 0, 0), "t") == 0) {
					// The tag already exists (wiz_res == "t")
					PQclear(wiz_res);
					printf("Tag '%s' already exists.\n", tag_str);

					sprintf(query, "SELECT id FROM public.tags WHERE name = '%s';", tag_str);
					printf("%s\n", query);
					wiz_res = PQexec(conn, query);
					sprintf(tmp_tagid, "%s", PQgetvalue(wiz_res, 0, 0));

					/* Create the file-tag bridge */
					sprintf(query, "INSERT INTO public.files_tags (file_id, tag_id) VALUES (%s, %s);", file_id, tmp_tagid);
					printf("%s\n\n", query);
					wiz_res = PQexec(conn, query);	// I can't believe I forgot to execute the query until _now_

					if (PQresultStatus(wiz_res) != PGRES_COMMAND_OK) {
						/* It _shouldn't_ be possible for this to be reached, but just in case. */
						fprintf(stderr, "%s\n", PQerrorMessage(conn));
						gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), scrolled_window);
						tag_process_fail_diag("Error", "Atsugami encountered an error:\n%s", PQerrorMessage(conn));
						PQclear(wiz_res);
						wiz_res = PQexec(conn, "ROLLBACK TRANSACTION;");
						
						PQclear(wiz_res);
						return 1;
					}

					PQclear(wiz_res);
				}
				else {
					// The tag does not exist (wiz_res == "f")
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
							tag_process_fail_diag("Error", "Atsugami encountered an error:\n%s", PQerrorMessage(conn));
							PQclear(wiz_res);
							wiz_res = PQexec(conn, "ROLLBACK TRANSACTION;");
							
							PQclear(wiz_res);
							gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), scrolled_window);
							return 1;
						}

						/* Create the file-tag bridge */
						sprintf(query, "INSERT INTO public.files_tags (file_id, tag_id) VALUES (%s, %s);", file_id, tagid);
						printf("%s\n\n", query);
						wiz_res = PQexec(conn, query);
						printf("%d\n", PQresultStatus(wiz_res));

						if (PQresultStatus(wiz_res) == PGRES_COMMAND_OK) {
							PQclear(wiz_res);
						}
						else {
							fprintf(stderr, "%s\n", PQerrorMessage(conn));
							tag_process_fail_diag("Error", "Atsugami encountered an error:\n%s", PQerrorMessage(conn));
							PQclear(wiz_res);
							wiz_res = PQexec(conn, "ROLLBACK TRANSACTION;");
							
							PQclear(wiz_res);
							gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), scrolled_window);
							return 1;
						}
					}
					else {
						fprintf(stderr, "%s\n", PQerrorMessage(conn));
						tag_process_fail_diag("Error", "Atsugami encountered an error:\n%s", PQerrorMessage(conn));
						PQclear(wiz_res);
						wiz_res = PQexec(conn, "ROLLBACK TRANSACTION;");
						
						PQclear(wiz_res);
						gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), scrolled_window);
						return 1;
					}
				}
			}
		}

		strcpy(buffer, "");
		strcpy(tag_str, "");
	}

	sprintf(query_string, "UPDATE public.files SET count = %d WHERE id = %s;", wc, file_id);
	printf("%s\n", query_string);
	wiz_res = PQexec(conn, query_string);
	if (PQresultStatus(wiz_res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "%s\n", PQerrorMessage(conn));
		PQclear(wiz_res);

		wiz_res = PQexec(conn, "ROLLBACK TRANSACTION;");
		printf("ROLLBACK\n");

		gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), scrolled_window);
		tag_process_fail_diag("Error", "%s", PQerrorMessage(conn));
		PQclear(wiz_res);
		return 1;
	}

	PQclear(wiz_res);
	wiz_res = PQexec(conn, "COMMIT TRANSACTION;");
	printf("COMMIT TRANSACTION;\n\n");
	PQclear(wiz_res);

	/* Create thumbnails of the image
	 * Windows ports _may_ not like the lack of file extensions. */
	char *cmd = NULL;
	int cmd_code;

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

	wiz_res = PQexec(conn, "SELECT thumb_dir FROM public.settings;");
	size_t path_size = strlen(PQgetvalue(wiz_res, 0, 0));
	//cmd = NULL;
	cmd = (char *) malloc(21 + path_size);

	/* Create thumbnails */
	for (catid = 0; catid <= 4; catid++) {
		thumb_pixbuf = gdk_pixbuf_new_from_file_at_scale(import_file_path, size_res[catid], size_res[catid], TRUE, NULL);
		sprintf(cmd, "%s/%s/%s.png", PQgetvalue(wiz_res, 0, 0), size_names[catid], file_sha256);	// Storage path, size, sha256
		printf("thumb path: %s\n", cmd);
		gdk_pixbuf_save(thumb_pixbuf, cmd, "png", NULL, NULL);
	}

	cmd = NULL;
	free(cmd);
	PQclear(wiz_res);

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

	printf("memory freed\n");
	file_count_update();
	printf("count updated\n");
	gtk_widget_destroy(scrolled_window);	// `realloc(): invalid next size` ???
	printf("Finished\n");
	return 0;
}

extern void import_wizard(void) {
	GtkWidget *button_box, *import_page, *tab_label_box, *tab_icon, *tab_label, *tab_close;

	import_page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	page_count = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);

	gtk_container_set_border_width(GTK_CONTAINER(import_page), 16);
	gtk_container_add(GTK_CONTAINER(scrolled_window), import_page);
	gtk_scrolled_window_set_kinetic_scrolling(GTK_SCROLLED_WINDOW(scrolled_window), TRUE);

	/* thumbnail */
	import_thumb_pixbuf = gdk_pixbuf_new_from_file_at_scale(import_file_path, 720, 720, TRUE, NULL);
	import_thumb = gtk_image_new_from_pixbuf(import_thumb_pixbuf);
	
	gtk_box_pack_start(GTK_BOX(import_page), import_thumb, FALSE, FALSE, 0);

	/* Source */
	source_label = gtk_label_new("Enter source URL here:");
	gtk_widget_set_halign(source_label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), source_label, FALSE, FALSE, 0);
	
	source_entry = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(source_entry), TRUE);
	gtk_widget_set_valign(source_entry, GTK_ALIGN_START);
	gtk_entry_set_placeholder_text(GTK_ENTRY(source_entry), "Enter the URL to import here");
	gtk_box_pack_start(GTK_BOX(import_page), source_entry, TRUE, TRUE, 0);

	/* Artist tag box */
	artist_label = gtk_label_new("Artist(s)");
	gtk_widget_set_halign(artist_label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), artist_label, FALSE, FALSE, 0);

	artist_label = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(artist_label), TRUE);
	gtk_widget_set_valign(artist_label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), artist_label, TRUE, TRUE, 0);

	/* Copyright tag box */
	copyright_label = gtk_label_new("Copyrights");
	gtk_widget_set_halign(copyright_label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), copyright_label, FALSE, FALSE, 0);

	copyright_entry = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(copyright_entry), TRUE);
	gtk_widget_set_valign(copyright_entry, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), copyright_entry, TRUE, TRUE, 0);

	/* Character tag box */
	character_label = gtk_label_new("Characters");
	gtk_widget_set_halign(character_label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), character_label, FALSE, FALSE, 0);

	character_entry = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(character_entry), TRUE);
	gtk_widget_set_valign(character_entry, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), character_entry, TRUE, TRUE, 0);

	/* General tag box */
	general_label = gtk_label_new("General");
	gtk_widget_set_halign(general_label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), general_label, FALSE, FALSE, 0);

	tv = gtk_text_view_new();
	tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));

	gtk_text_view_set_editable(GTK_TEXT_VIEW(tv), TRUE);
	gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(tv), FALSE);
	gtk_box_pack_start(GTK_BOX(import_page), tv, TRUE, TRUE, 0);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tv), GTK_WRAP_WORD_CHAR);
	gtk_text_buffer_get_bounds(tb, &istart, &iend);

	/* Meta tag box */
	meta_label = gtk_label_new("Meta");
	gtk_widget_set_halign(meta_label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), meta_label, FALSE, FALSE, 0);

	meta_entry = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(meta_entry), TRUE);
	gtk_widget_set_valign(meta_entry, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), meta_entry, TRUE, TRUE, 0);

	/* Rating */
	rating_label = gtk_label_new("Rating:");
	gtk_widget_set_halign(rating_label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), rating_label, FALSE, FALSE, 0);
	
	cbox = gtk_combo_box_text_new();
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(cbox), "s", "Safe");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(cbox), "q", "Questionable");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(cbox), "e", "Explicit");
	gtk_box_pack_start(GTK_BOX(import_page), cbox, FALSE, FALSE, 0);

	/* Invisible entry field */
	rating_entry = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(rating_entry), TRUE);
	g_object_bind_property(cbox, "active-id", rating_entry, "text", G_BINDING_DEFAULT);

	button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_pack_start(GTK_BOX(import_page), button_box, FALSE, FALSE, 0);
	
	can_button = gtk_button_new_with_label("Cancel");
	imp_button = gtk_button_new_with_label("Import");
	
	gtk_button_box_set_layout(GTK_BUTTON_BOX(button_box), GTK_BUTTONBOX_START);
	gtk_box_pack_start(GTK_BOX(button_box), can_button, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(button_box), imp_button, FALSE, FALSE, 0);
	
	g_signal_connect(can_button, "clicked", G_CALLBACK(cancel_button_cb), NULL);
	g_signal_connect(imp_button, "clicked", G_CALLBACK(import_button_cb), NULL);

	tab_label_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
	tab_icon = gtk_image_new_from_icon_name("insert-image", GTK_ICON_SIZE_MENU);
	tab_label = gtk_label_new("Import");
	tab_close = gtk_button_new_from_icon_name("window-close-symbolic", GTK_ICON_SIZE_MENU);

	/* Tab label */
	gtk_button_set_relief(GTK_BUTTON(tab_close), GTK_RELIEF_NONE);
	gtk_widget_add_accelerator(tab_close, "clicked", accel, GDK_KEY_w, GDK_CONTROL_MASK, GTK_ACCEL_LOCKED);
	g_signal_connect(GTK_BUTTON(tab_close), "clicked", G_CALLBACK(cancel_button_cb), NULL);

	gtk_box_pack_start(GTK_BOX(tab_label_box), tab_icon, FALSE, FALSE, 4);
	gtk_box_pack_start(GTK_BOX(tab_label_box), tab_label, FALSE, FALSE, 4);
	gtk_box_pack_start(GTK_BOX(tab_label_box), tab_close, FALSE, FALSE, 4);

	/* init stuff */
	gtk_widget_show_all(import_page);
	gtk_widget_show_all(tab_label_box);
	gtk_widget_show_all(scrolled_window);
	gtk_container_add(GTK_CONTAINER(notebook), scrolled_window);
	gtk_scrolled_window_set_propagate_natural_height(GTK_SCROLLED_WINDOW(scrolled_window), TRUE);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), page_count);
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), scrolled_window, TRUE);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook), scrolled_window, tab_label_box);
}
