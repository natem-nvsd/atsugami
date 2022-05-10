/* import_wizard.c (c) 2021-2022 by Nate Morrison */
#include "atsugami.h"
#include "import.h"
#include <ctype.h>
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

GtkWidget *source_label, *artist_label, *copyright_label, *character_label, *general_label, *meta_label, *rating_label, *source_entry, *artist_entry, *copyright_entry, *character_entry, *meta_entry, *rating_entry, *tv, *import_thumb, *can_button, *imp_button, *header_box, *og_commentary_tv, *tr_commentary_tv, *og_commentary_cbox, *tr_commentary_cbox, *import_page, *og_commentary_revealer, *tr_commentary_revealer, *commentary_revealer_vbox, *safe_radio_btn, *questionable_radio_btn, *explicit_radio_btn;
GtkTextBuffer *tb, *og_commentary_tb, *tr_commentary_tb;
GtkTextIter istart, iend, og_start, og_end, tr_start, tr_end;
GdkPixbuf *import_thumb_pixbuf, *thumb_pixbuf;
GtkImage *diag_ico;
gint page_count = 0;
gboolean parent_bool = FALSE;
gboolean child_bool = FALSE;
gboolean has_children = FALSE;
GtkStyleContext *contxt0, *contxt1, *contxt2;

enum {
	ARTIST_NAME_COLUMN,	// Tag name	(char *)
	ARTISTN_COLUMNS
};

enum {
	COPYRIGHT_NAME_COLUMN,	// Tag name	(char *)
	COPYRIGHTN_COLUMNS
};

enum {
	CHARACTER_NAME_COLUMN,	// Tag name	(char *)
	CHARACTERN_COLUMNS
};

enum {
	GENERAL_NAME_COLUMN,	// Tag name	(char *)
	GENERALN_COLUMNS
};

enum {
	META_NAME_COLUMN,	// Tag name	(char *)
	METAN_COLUMNS
};

static void og_commentary_cb(GtkWidget *vbox) {
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(og_commentary_cbox)) == TRUE) {
		printf("commentary 0 enabled\n");
		gtk_revealer_set_reveal_child(GTK_REVEALER(og_commentary_revealer), TRUE);
	} else {
		printf("commentary 0 disabled\n");
		gtk_revealer_set_reveal_child(GTK_REVEALER(og_commentary_revealer), FALSE);
	}
}

static void tr_commentary_cb(GtkWidget *vbox) {
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(tr_commentary_cbox)) == TRUE) {
		printf("commentary 1 enabled\n");
		gtk_revealer_set_reveal_child(GTK_REVEALER(tr_commentary_revealer), TRUE);
	} else {
		printf("commentary 1 disabled\n");
		gtk_revealer_set_reveal_child(GTK_REVEALER(tr_commentary_revealer), FALSE);
	}
}

static void cancel_button_cb(GtkWidget *scrolled_window) {
	gtk_widget_destroy(scrolled_window);
	gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), scrolled_window);
	gtk_widget_destroy(import_page);

	if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook)) == 0)
		quit_activate();
}

static void incomplete_diag(const char *label) {
	GtkWidget *diag;
	GtkDialogFlags diag_flags = GTK_RESPONSE_ACCEPT;

	/* Set the labels of required text fields to STYLE_CLASS_ERROR */
	contxt0 = gtk_widget_get_style_context(artist_label);
	contxt2 = gtk_widget_get_style_context(rating_label);

	gtk_style_context_add_class(contxt0, "error");
	gtk_style_context_add_class(contxt1, "error");
	gtk_style_context_add_class(contxt2, "error");

	gtk_label_set_markup(GTK_LABEL(artist_label), "Artist(s)\t\t\t\t\t<b>* This field is required.</b>");
	gtk_label_set_markup(GTK_LABEL(rating_label), "Rating\t\t\t\t\t<b>* This field is required.</b>");

	diag = gtk_message_dialog_new(GTK_WINDOW(window), diag_flags, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Form Error");
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(diag), "%s", label);
	gtk_dialog_run(GTK_DIALOG(diag));
	gtk_widget_destroy(diag);
}

static void tag_process_fail_diag(const char *title, const char *body, ...) {
	GtkWidget *diag;
	GtkDialogFlags diag_flags;

	diag_flags = GTK_RESPONSE_ACCEPT;
	diag = gtk_message_dialog_new(GTK_WINDOW(window), diag_flags, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", title);

	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(diag), "%s", body);
	gtk_dialog_run(GTK_DIALOG(diag));
	gtk_widget_destroy(diag);
}

static void import_button_cb(GtkWidget *scrolled_window) {
	PGresult *wiz_res;
	size_t size_res[6], sizes[7];
	char *query, *tag_buf[7];
	int catid, iter, tag_c = 0;	// Category id, tag count
	long file_id;
	bool src_req, tag_req, art_req;

	gtk_text_buffer_get_iter_at_offset(tb, &istart, 0);
	gtk_text_buffer_get_iter_at_offset(tb, &iend, -1);
	tag_buf[3] = (char *) gtk_text_buffer_get_text(tb, &istart, &iend, FALSE);	// General

	/* Allocate memory for tag strings */
	tag_buf[0] = (char *) gtk_entry_get_text(GTK_ENTRY(artist_entry));		// Artists
	tag_buf[1] = (char *) gtk_entry_get_text(GTK_ENTRY(copyright_entry));		// Copyrights
	tag_buf[2] = (char *) gtk_entry_get_text(GTK_ENTRY(character_entry));		// Characters
	tag_buf[4] = (char *) gtk_entry_get_text(GTK_ENTRY(meta_entry));		// Meta
	tag_buf[5] = (char *) gtk_entry_get_text(GTK_ENTRY(source_entry));		// Source


	/* Rating */
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(safe_radio_btn)) == TRUE)
		tag_buf[6] = "s";
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(questionable_radio_btn)) == TRUE)
		tag_buf[6] = "q";
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(explicit_radio_btn)) == TRUE)
		tag_buf[6] = "e";

	for (iter = 0; iter < 7; iter++) {
		sizes[iter] = strlen(tag_buf[iter]);
		printf("%d '%s'\n", iter, tag_buf[iter]);
	}
	
	/* Check if text fields are empty. */
	if (sizes[6] == 0) {
		incomplete_diag("One or more fields were not filled.\nPlease complete them and try again.");
		return;
	}

	/* Begin the transaction */
	wiz_res = PQexec(conn, "BEGIN TRANSACTION");
	if (PQresultStatus(wiz_res) != PGRES_COMMAND_OK) {
		PQclear(wiz_res);

		// FIXME: put a dialog here.
		return;
	} else
		PQclear(wiz_res);

	/* TODO: tagme, artist_request, and source_request needed. */
	if (sizes[0] == 0)
		art_req = true;
	
	if (sizes[3] == 0)
		tag_req = true;
	
	if (sizes[5] == 0)
		src_req = true;

	/** Sanitize source **/
	/* Remove spaces at the end of str */
	iter = (strlen(tag_buf[5]) - 1);
	while (iter > 0) {
		if (tag_buf[5][iter] == ' ')
			for (int iter1 = iter; iter1 > 0; iter1--)
				tag_buf[5][iter1] = tag_buf[5][iter1 - 1];
		else
			break;
	}

	/* Remove spaces at the start of str */
	source_loop: iter = 0;
	while (iter < strlen(tag_buf[5])) {
		if (tag_buf[5][iter] == ' ')
			for (int iter1 = iter; iter1 < strlen(tag_buf[5]); iter1++)
				tag_buf[5][iter1] = tag_buf[5][iter1 + 1];
		else
			break;
	}

	/* Move to the end of the current word */
	while (tag_buf[5][iter] != ' ')
		++iter;
	
	/* Remove whitespace */
	while (iter < strlen(tag_buf[5])){
		if (tag_buf[5][iter] == ' ')
			for (int iter1 = iter; iter1 < strlen(tag_buf[5]); iter1++)
				tag_buf[5][iter1] = tag_buf[5][iter1 + 1];
		else {
			tag_buf[5][iter - 1] = '_';
			break;
		}
	}
	
	/* Check if ... */
	if (tag_buf[5][iter] != ' ')
		goto source_loop;
	else {
		query = (char *) malloc(160 + sizes[5]);
		//sprintf(query, "INSERT INTO public.files (sha256, rating, source) VALUES ('%s', '%s', '%s') ON CONFLICT DO NOTHING;", file_sha256, tag_buf[6], tag_buf[5]);
		sprintf(query, "INSERT INTO public.files (sha256, rating, source) VALUES ('%s', '%s', '%s');", file_sha256, tag_buf[6], tag_buf[5]);
		printf("%s\n", query);
	}

	/* Submit query */
	wiz_res = PQexec(conn, query);
	//wiz_res = PQexecParams(conn, query, 0, NULL, NULL, NULL, NULL, 0);
	free(query);

	/* Check command status */
	if (PQresultStatus(wiz_res) == PGRES_COMMAND_OK) {
		PQclear(wiz_res);

		query = (char *) malloc(112);
		sprintf(query, "SELECT id FROM public.files WHERE sha256 = '%s';", file_sha256);
		wiz_res = PQexec(conn, query); // If the file _was_ inserted into the database, then file_id is guaranteed to be non-null
		printf("%s\n\n", query);

		file_id = atol(PQgetvalue(wiz_res, 0, 0));
		printf("'%lu'\n", file_id);
		PQclear(wiz_res);
		free(query);
	} else {
		PQclear(wiz_res);
		wiz_res = PQexec(conn, "ROLLBACK TRANSACTION;");

		gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), scrolled_window);
		fprintf(stderr, "%s", PQerrorMessage(conn));
		tag_process_fail_diag("Error", PQerrorMessage(conn));	// This doesn't work; printf "%s" instead of PQerrorMessage's value
		PQclear(wiz_res);
		return;
	}

	/** Sanitize tag input, generate queries, and submit queries **/
	for (catid = 0; catid < 5; catid++, tag_c++) {
	/* TODO: Use PQexecParams() to insert data into the database without treating it as SQL syntax.
	 * See https://www.postgresql.org/docs/14/libpq-exec.html#LIBPQ-EXEC-ESCAPE-STRING for more details. */
		char *buff;
		size_t buf_s;

		/* Replace newlines with spaces */
		iter = (strlen((tag_buf[catid])) - 1);
		while (iter > 0) {
			if ((tag_buf[catid])[iter] == '\n')
				for (int i2 = iter; i2 > 0; i2--)
					(tag_buf[catid])[i2] = ' ';
			else
				break;
		}

		/* Replace tabs with spaces */
		iter = (strlen((tag_buf[catid])) - 1);
		while (iter > 0) {
			if ((tag_buf[catid])[iter] == '\t')
				for (int i2 = iter; i2 > 0; i2--)
					(tag_buf[catid])[i2] = ' ';
			else
				break;
		}

		/* Remove spaces at the end of str */
		iter = (strlen((tag_buf[catid])) - 1);
		while (iter > 0) {
			if ((tag_buf[catid])[iter] == ' ')
				for (int i2 = iter; i2 > 0; i2--)
					(tag_buf[catid])[i2] = (tag_buf[catid])[i2 - 1];
			else
				break;
		}

		/* Remove spaces at the start of str */
		iter_loop: iter = 0;
		while (iter < strlen(tag_buf[catid])) {
			if ((tag_buf[catid])[iter] == ' ')
				for (int i2 = iter; i2 < strlen((tag_buf[catid])); i2++)
					(tag_buf[catid])[i2] = (tag_buf[catid])[i2 + 1];
			else
				break;
		}

		/* Seek to the next space */
		while ((tag_buf[catid])[iter] != ' ')
			iter++;

		/** Execute queries **/
		buff = (char *) malloc(iter + 1);
		snprintf(buff, (iter + 1), "%s", tag_buf[catid]);
		//printf("0 '%s'\n", buff);
		buf_s = strlen(buff);

		/* Check if the tag exists, then create it if it doesn't */
		query = (char *) malloc(60 + buf_s);
		sprintf(query, "SELECT EXISTS (SELECT id FROM public.tags WHERE name = '%s');", buff);
		printf("%s\n", query);
		wiz_res = PQexec(conn, query);
		free(query);

		if (strcmp(PQgetvalue(wiz_res, 0, 0), "t") == 0) {
			// The tag already exists (wiz_res == "t")
			PQclear(wiz_res);

			/* Create the file-tag bridge */
			query = (char *) malloc(105 + sizeof(long) + buf_s);
			sprintf(query, "INSERT INTO public.files_tags (file_id, tag_id) VALUES (%li, (SELECT id FROM public.tags WHERE name = '%s'));", file_id, buff);
			printf("%s\n", query);
			//sani_query = PQescapeLiteral(conn, query, strlen(query));
	//		wiz_res = PQexec(conn, sani_query);
			wiz_res = PQexec(conn, query);
	//		printf("%s\n", sani_query);
			free(query);
	//		free(sani_query);
			if (PQresultStatus(wiz_res) != PGRES_COMMAND_OK) {
				/* It _shouldn't_ be possible for this to be reached, but just in case. */
				fprintf(stderr, "%s\n", PQerrorMessage(conn));
				gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), scrolled_window);
				tag_process_fail_diag("Error", PQerrorMessage(conn));
				PQclear(wiz_res);
				wiz_res = PQexec(conn, "ROLLBACK TRANSACTION;");
				
				PQclear(wiz_res);
				return;
			} else
				PQclear(wiz_res);
		} else {
			// The tag does not exist (wiz_res == "f")
			PQclear(wiz_res);
			query = (char *) malloc(67 + buf_s);
			sprintf(query, "INSERT INTO public.tags (name) VALUES ('%s');", buff);	// The tag does not exist yet, so no need for `ON CONFLICT DO NOTHING;`
			printf("%s\n", query);
			wiz_res = PQexec(conn, query);
			free(query);
			if (PQresultStatus(wiz_res) != PGRES_COMMAND_OK) {
				PQclear(wiz_res);
				/* It _shouldn't_ be possible for this to be reached, but just in case. */
				fprintf(stderr, "%s\n", PQerrorMessage(conn));
				gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), scrolled_window);
				tag_process_fail_diag("Error", PQerrorMessage(conn));
				PQclear(wiz_res);
				wiz_res = PQexec(conn, "ROLLBACK TRANSACTION;");
				
				PQclear(wiz_res);
				return;
			}

			/* Create the category-tag bridge */
			PQclear(wiz_res);
			query = (char *) malloc(114 + buf_s + sizeof(int));
			sprintf(query, "INSERT INTO public.tags_categories (tag_id, category_id) VALUES ((SELECT id FROM public.tags WHERE name = '%s'), %d);", buff, catid);
			printf("%s\n", query);
			wiz_res = PQexec(conn, query);
			free(query);
			if (PQresultStatus(wiz_res) != PGRES_COMMAND_OK) {
				/* It _shouldn't_ be possible for this to be reached, but just in case. */
				fprintf(stderr, "%s\n", PQerrorMessage(conn));
				gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), scrolled_window);
				tag_process_fail_diag("Error", PQerrorMessage(conn));
				PQclear(wiz_res);
				wiz_res = PQexec(conn, "ROLLBACK TRANSACTION;");
				
				PQclear(wiz_res);
				return;
			}

			/* Create the file-tag bridget */
			PQclear(wiz_res);
			query = (char *) malloc(105 + buf_s + sizeof(long));
			sprintf(query, "INSERT INTO public.files_tags (file_id, tag_id) VALUES (%li, (SELECT id FROM public.tags WHERE name = '%s'));", file_id, buff);
			printf("%s\n", query);
			wiz_res = PQexec(conn, query);
			free(query);
			if (PQresultStatus(wiz_res) != PGRES_COMMAND_OK) {
				fprintf(stderr, "%s\n", PQerrorMessage(conn));
				gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), scrolled_window);
				tag_process_fail_diag("Error", PQerrorMessage(conn));
				PQclear(wiz_res);
				wiz_res = PQexec(conn, "ROLLBACK TRANSACTION;");
				
				PQclear(wiz_res);
				return;
			}

			PQclear(wiz_res);
		}

		/** Shift the string **/
		//printf("1\t '%s'\n", (tag_buf[catid]));
		for (int i2 = 0; i2 < (iter + 1); i2++)
			for (int i3 = 0; i3 < strlen((tag_buf[catid])); i3++)
				(tag_buf[catid])[i3] = (tag_buf[catid])[i3 + 1];

		//printf("2\t '%s'\n\n\n\n", (tag_buf[catid]));
		if (strlen((tag_buf[catid])) != 0)
			goto iter_loop;
		//else
		//	printf("3\t '%s'\n", (tag_buf[catid]));
	}

	if (PQresultStatus(wiz_res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "%s\n", PQerrorMessage(conn));
		PQclear(wiz_res);

		wiz_res = PQexec(conn, "ROLLBACK TRANSACTION;");
		printf("ROLLBACK\n");

		gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), scrolled_window);
		tag_process_fail_diag("Error", PQerrorMessage(conn));
		PQclear(wiz_res);
		return;
	}

	PQclear(wiz_res);
	wiz_res = PQexec(conn, "COMMIT TRANSACTION;");
	printf("COMMIT TRANSACTION;\n\n");
	PQclear(wiz_res);

	/* Create thumbnails of the image
	 * Windows ports _may_ not like the lack of file extensions. */
	char *cmd = NULL;
	size_t path_size  = 0;
	int cmd_code;

	size_res[0] = 150;
	size_res[1] = 180;
	size_res[2] = 225;
	size_res[3] = 270;
	size_res[4] = 360;

	/* Create thumbnails */
	for (catid = 0; catid <= 4; catid++) {
		char *path = get_thumb_path_by_id(catid);

		cmd = (char *) malloc(strlen(path) + 5 + strlen(file_sha256));
		thumb_pixbuf = gdk_pixbuf_new_from_file_at_scale(import_file_path, size_res[catid], size_res[catid], TRUE, NULL);

		sprintf(cmd, "%s/%s.png", path, file_sha256);	// Storage path, size, sha256
		printf("thumb path: %s\n", cmd);
		gdk_pixbuf_save(thumb_pixbuf, cmd, "png", NULL, NULL);

		free(cmd);
	}

	/** Move the file to the storage directory **/
	/* TODO: Use system calls to keep the entire process internal */
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
		// TODO: create a dialog with instructions for the user to manually move the file to the correct directory.
		fprintf(stderr, "mv command returned: %d\n", cmd_code);
		free(cmd);
		free(wiz_res);
		file_count_update();
		gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), scrolled_window);
		gtk_widget_destroy(scrolled_window);
		gtk_widget_destroy(import_page);

		return;
	}

	free(cmd);
	PQclear(wiz_res);

	file_count_update();
	gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), scrolled_window);
	gtk_widget_destroy(scrolled_window);
	gtk_widget_destroy(import_page);
	//gtk_widget_destroy(tab_label_box);
	printf("Finished\n");
}

extern void import_wizard(void) {
	GtkWidget *button_box, *commentary_label, *scrolled_window, *tab_label_box, *tab_icon, *tab_label, *radio_vbox, *tab_close, *commentary_radio_vbox;
	GtkTreeModel *artist_completion_model, *copyright_completion_model, *character_completion_model, *general_completion_model, *meta_completion_model;
	GtkEntryCompletion *artist_completion, *copyright_completion, *character_completion, *general_completion, *meta_completion;

	/* Entry Completion */
	source_entry = gtk_entry_new();
	artist_entry = gtk_entry_new();
	copyright_entry = gtk_entry_new();
	character_entry = gtk_entry_new();
	meta_entry = gtk_entry_new();

	artist_completion = gtk_entry_completion_new();
	copyright_completion = gtk_entry_completion_new();
	character_completion = gtk_entry_completion_new();
	general_completion = gtk_entry_completion_new();
	meta_completion = gtk_entry_completion_new();

	artist_completion_model = completion_model_generator(0);
	copyright_completion_model = completion_model_generator(1);
	character_completion_model = completion_model_generator(2);
	general_completion_model = completion_model_generator(3);
	meta_completion_model = completion_model_generator(4);

	gtk_entry_completion_set_text_column(artist_completion, 0);
	gtk_entry_completion_set_text_column(copyright_completion, 0);
	gtk_entry_completion_set_text_column(character_completion, 0);
	gtk_entry_completion_set_text_column(general_completion, 0);
	gtk_entry_completion_set_text_column(meta_completion, 0);

	entry_completion_set_model(artist_completion, artist_completion_model);
	entry_completion_set_model(copyright_completion, copyright_completion_model);
	entry_completion_set_model(character_completion, character_completion_model);
	//entry_completion_set_model(general_completion, general_completion_model);
	entry_completion_set_model(meta_completion, meta_completion_model);

	gtk_entry_set_completion(GTK_ENTRY(artist_entry), artist_completion);
	gtk_entry_set_completion(GTK_ENTRY(copyright_entry), copyright_completion);
	gtk_entry_set_completion(GTK_ENTRY(character_entry), character_completion);
	gtk_entry_set_completion(GTK_ENTRY(meta_entry), meta_completion);

	/* Other stuff */
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
	
	gtk_entry_set_activates_default(GTK_ENTRY(source_entry), TRUE);
	gtk_widget_set_valign(source_entry, GTK_ALIGN_START);
	gtk_entry_set_placeholder_text(GTK_ENTRY(source_entry), "Enter the URL to import here");
	gtk_entry_set_icon_from_icon_name(GTK_ENTRY(source_entry), GTK_ENTRY_ICON_SECONDARY, "dialog-question-symbolic");
	gtk_box_pack_start(GTK_BOX(import_page), source_entry, TRUE, TRUE, 0);

	/* Artist tag box */
	artist_label = gtk_label_new("Artist(s)");
	gtk_widget_set_halign(artist_label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), artist_label, FALSE, FALSE, 0);

	gtk_entry_set_activates_default(GTK_ENTRY(artist_entry), TRUE);
	gtk_widget_set_valign(artist_entry, GTK_ALIGN_START);
	gtk_entry_set_icon_from_icon_name(GTK_ENTRY(artist_entry), GTK_ENTRY_ICON_SECONDARY, "dialog-question-symbolic");
	gtk_box_pack_start(GTK_BOX(import_page), artist_entry, TRUE, TRUE, 0);


	/* Copyright tag box */
	copyright_label = gtk_label_new("Copyrights");
	gtk_widget_set_halign(copyright_label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), copyright_label, FALSE, FALSE, 0);

	gtk_entry_set_activates_default(GTK_ENTRY(copyright_entry), TRUE);
	gtk_widget_set_valign(copyright_entry, GTK_ALIGN_START);
	gtk_entry_set_icon_from_icon_name(GTK_ENTRY(copyright_entry), GTK_ENTRY_ICON_SECONDARY, "dialog-question-symbolic");
	gtk_box_pack_start(GTK_BOX(import_page), copyright_entry, TRUE, TRUE, 0);


	/* Character tag box */
	character_label = gtk_label_new("Characters");
	gtk_widget_set_halign(character_label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), character_label, FALSE, FALSE, 0);

	gtk_entry_set_activates_default(GTK_ENTRY(character_entry), TRUE);
	gtk_widget_set_valign(character_entry, GTK_ALIGN_START);
	gtk_entry_set_icon_from_icon_name(GTK_ENTRY(character_entry), GTK_ENTRY_ICON_SECONDARY, "dialog-question-symbolic");
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

	gtk_entry_set_activates_default(GTK_ENTRY(meta_entry), TRUE);
	gtk_widget_set_valign(meta_entry, GTK_ALIGN_START);
	gtk_entry_set_icon_from_icon_name(GTK_ENTRY(meta_entry), GTK_ENTRY_ICON_SECONDARY, "dialog-question-symbolic");
	gtk_box_pack_start(GTK_BOX(import_page), meta_entry, TRUE, TRUE, 0);


	/* Rating */
	rating_label = gtk_label_new("Rating");
	gtk_widget_set_halign(rating_label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), rating_label, FALSE, FALSE, 0);

	radio_vbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	safe_radio_btn = gtk_radio_button_new_with_mnemonic(NULL, "_Safe");
	questionable_radio_btn = gtk_radio_button_new_with_mnemonic(NULL, "_Questionable");
	explicit_radio_btn = gtk_radio_button_new_with_mnemonic(NULL, "_Explicit");
	gtk_radio_button_join_group(GTK_RADIO_BUTTON(questionable_radio_btn), GTK_RADIO_BUTTON(safe_radio_btn));
	gtk_radio_button_join_group(GTK_RADIO_BUTTON(explicit_radio_btn), GTK_RADIO_BUTTON(safe_radio_btn));
	gtk_box_pack_start(GTK_BOX(radio_vbox), safe_radio_btn, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(radio_vbox), questionable_radio_btn, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(radio_vbox), explicit_radio_btn, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(import_page), radio_vbox, FALSE, FALSE, 0);

	/** Commentary **/
	GtkWidget *og_label, *tr_label, *og_vbox, *tr_vbox;
	commentary_label = gtk_label_new("Commentary");
	gtk_widget_set_halign(commentary_label, GTK_ALIGN_START);

	commentary_radio_vbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	commentary_revealer_vbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	og_commentary_cbox = gtk_check_button_new_with_mnemonic("_Original commentary");
	tr_commentary_cbox = gtk_check_button_new_with_mnemonic("_Translated commentary");

	/* Commentary radio buttons */
	gtk_box_pack_start(GTK_BOX(import_page), commentary_label, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(commentary_radio_vbox), og_commentary_cbox, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(commentary_radio_vbox), tr_commentary_cbox, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(import_page), commentary_radio_vbox, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(import_page), commentary_revealer_vbox, FALSE, FALSE, 0);

	/* Original commentary */
	og_label = gtk_label_new("Original");
	og_vbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	og_commentary_revealer = gtk_revealer_new();
	og_commentary_tv = gtk_text_view_new();
	og_commentary_tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(og_commentary_tv));

	gtk_text_view_set_editable(GTK_TEXT_VIEW(og_commentary_tv), TRUE);
	gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(og_commentary_tv), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(og_commentary_tv), GTK_WRAP_WORD_CHAR);
	gtk_text_buffer_get_bounds(og_commentary_tb, &og_start, &og_end);

	gtk_box_pack_start(GTK_BOX(og_vbox), og_label, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(og_vbox), og_commentary_tv, TRUE, FALSE, 0);
	gtk_container_add(GTK_CONTAINER(og_commentary_revealer), og_vbox);
	gtk_box_pack_start(GTK_BOX(commentary_revealer_vbox), og_commentary_revealer, TRUE, FALSE, 10);

	gtk_revealer_set_reveal_child(GTK_REVEALER(og_commentary_revealer), FALSE);
	g_signal_connect(og_commentary_cbox, "toggled", G_CALLBACK(og_commentary_cb), import_page);

	/* Translated commentary */
	tr_vbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	tr_label = gtk_label_new("Translated");
	tr_commentary_revealer = gtk_revealer_new();
	tr_commentary_tv = gtk_text_view_new();
	tr_commentary_tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(og_commentary_tv));

	gtk_text_view_set_editable(GTK_TEXT_VIEW(tr_commentary_tv), TRUE);
	gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(tr_commentary_tv), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tr_commentary_tv), GTK_WRAP_WORD_CHAR);
	gtk_text_buffer_get_bounds(tr_commentary_tb, &tr_start, &tr_end);

	gtk_box_pack_start(GTK_BOX(tr_vbox), tr_label, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(tr_vbox), tr_commentary_tv, TRUE, FALSE, 0);
	gtk_container_add(GTK_CONTAINER(tr_commentary_revealer), tr_vbox);
	gtk_box_pack_start(GTK_BOX(commentary_revealer_vbox), tr_commentary_revealer, TRUE, FALSE, 10);

	gtk_revealer_set_reveal_child(GTK_REVEALER(tr_commentary_revealer), FALSE);
	g_signal_connect(tr_commentary_cbox, "toggled", G_CALLBACK(tr_commentary_cb), import_page);

	/** Button box **/
	button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_pack_start(GTK_BOX(import_page), button_box, FALSE, FALSE, 0);
	
	can_button = gtk_button_new_with_label("Cancel");
	imp_button = gtk_button_new_with_label("Import");
	
	gtk_button_box_set_layout(GTK_BUTTON_BOX(button_box), GTK_BUTTONBOX_START);
	gtk_box_pack_start(GTK_BOX(button_box), can_button, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(button_box), imp_button, FALSE, FALSE, 0);
	
	g_signal_connect(can_button, "clicked", G_CALLBACK(cancel_button_cb), scrolled_window);
	g_signal_connect(imp_button, "clicked", G_CALLBACK(import_button_cb), scrolled_window);

	/** Tab label **/
	tab_label_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
	tab_icon = gtk_image_new_from_icon_name("insert-image", GTK_ICON_SIZE_MENU);
	tab_label = gtk_label_new("Import");
	tab_close = gtk_button_new_from_icon_name("window-close-symbolic", GTK_ICON_SIZE_MENU);

	gtk_button_set_relief(GTK_BUTTON(tab_close), GTK_RELIEF_NONE);
	gtk_widget_add_accelerator(tab_close, "clicked", accel, GDK_KEY_w, GDK_CONTROL_MASK, GTK_ACCEL_LOCKED);
	g_signal_connect(GTK_BUTTON(tab_close), "clicked", G_CALLBACK(cancel_button_cb), scrolled_window);

	gtk_box_pack_start(GTK_BOX(tab_label_box), tab_icon, FALSE, FALSE, 4);
	gtk_box_pack_start(GTK_BOX(tab_label_box), tab_label, FALSE, FALSE, 4);
	gtk_box_pack_start(GTK_BOX(tab_label_box), tab_close, FALSE, FALSE, 4);

	/** init stuff **/
	gtk_widget_show_all(import_page);
	gtk_widget_show_all(tab_label_box);
	gtk_widget_show_all(scrolled_window);
	gtk_container_add(GTK_CONTAINER(notebook), scrolled_window);
	gtk_scrolled_window_set_propagate_natural_height(GTK_SCROLLED_WINDOW(scrolled_window), TRUE);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), page_count);
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), scrolled_window, TRUE);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook), scrolled_window, tab_label_box);
}
