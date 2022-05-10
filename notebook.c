/***************************************************************\
*	notebook.c (c) 2021-2022, by Nate Morrison		*
*								*
*	notebook.c contains functions for the primary GTK Icon	*
*	View found in Atsugami, and its callbacks.		*
\***************************************************************/

#include "atsugami.h"
#include "colours.h"
#include "types.h"
#include <errno.h>
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdio.h>
#include <stdbool.h>

static void query_tooltip(GtkWidget *widget, gint x, gint y, gboolean keyboard_mode, GtkTooltip *tooltip, gpointer user_data) {
	/* TODO: Make popover relative to the icon the cursor is over. */
	GtkWidget *popover, *scrolled_window, *viewport;
	GtkListStore *list_store;
	GtkTreePath *tree_path;
	GtkTreeIter tree_iter;

	GtkTextTag *artist_tt, *copyright_tt, *character_tt, *general_tt, *meta_tt, *tag_array[5];
	GtkTextIter start_iter, end_iter;
	GtkWidget *tag_tv;
	GtkTextBuffer *tag_tb;

	PGresult *tooltip_res;
	char *sha256, *query;

	/* Prevent execution with a null pointer */
	if (user_data == NULL) {
		dbg_err("notebook.c: user_data is NULL");
		return;
	}
	
	/* Prepare the tooltip */
	tree_path = gtk_tree_path_new();
	list_store = GTK_LIST_STORE(user_data);
	tree_path = gtk_icon_view_get_path_at_pos(GTK_ICON_VIEW(widget), x, y);
	gtk_tree_model_get_iter(GTK_TREE_MODEL(list_store), &tree_iter, tree_path);	// FIXME: Something is happening here.

	/* tree_path is NULL when the user's cursor is not over an icon, and vice versa */
	if (tree_path == NULL)
		return;

	/* Get the SHA256 checksum */
	sha256 = (char *) malloc(65);
	gtk_tree_model_get(GTK_TREE_MODEL(list_store), &tree_iter, 0, &sha256, -1);

	if (sha256 == NULL) {
		dbg_err("notebook.c: sha256 is NULL\n");
		return;
	}

	/** Prepare data for the tooltip **/
	/* Text view */
	tag_tv = gtk_text_view_new();
	tag_tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tag_tv));

	gtk_text_view_set_editable(GTK_TEXT_VIEW(tag_tv), FALSE);
	gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(tag_tv), FALSE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(tag_tv), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tag_tv), GTK_WRAP_WORD);
	gtk_text_view_set_top_margin(GTK_TEXT_VIEW(tag_tv), 4);
	gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(tag_tv), 4);
	gtk_text_view_set_left_margin(GTK_TEXT_VIEW(tag_tv), 4);
	gtk_text_view_set_right_margin(GTK_TEXT_VIEW(tag_tv), 4);

	/* Text tags */
	artist_tt = gtk_text_buffer_create_tag(tag_tb, "artist", "foreground", ARTIST, NULL);
	copyright_tt = gtk_text_buffer_create_tag(tag_tb, "copyright", "foreground", COPYRIGHT, NULL);
	character_tt = gtk_text_buffer_create_tag(tag_tb, "character", "foreground", CHARACTER, NULL);
	general_tt = gtk_text_buffer_create_tag(tag_tb, "general", "foreground", GENERAL, NULL);
	meta_tt = gtk_text_buffer_create_tag(tag_tb, "meta", "foreground", META, NULL);

	tag_array[0] = artist_tt;
	tag_array[1] = copyright_tt;
	tag_array[2] = character_tt;
	tag_array[3] = general_tt;
	tag_array[4] = meta_tt;

	/* Queries */
	query = (char *) malloc(112);
	sprintf(query, "SELECT id FROM public.files WHERE sha256 = '%s';", sha256);
	tooltip_res = PQexec(conn, query);

	query = (char *) malloc(389 + sizeof(int));
	sprintf(query, "SELECT tags_cats.category_id, tags.name FROM public.tags AS tags INNER JOIN (SELECT * FROM public.files_tags WHERE file_id = %s) files_tags ON files_tags.tag_id = tags.id INNER JOIN public.files AS files ON files_tags.file_id = files.id INNER JOIN public.tags_categories AS tags_cats ON tags_cats.category_id = tags_cats.category_id WHERE tags_cats.tag_id = tags.id ORDER BY tags_cats.category_id ASC;", PQgetvalue(tooltip_res, 0, 0));

	PQclear(tooltip_res);

	/* Apply text tags */
	size_t res_size;
	char *buffer;
	int row_count, iter;

	tooltip_res = PQexec(conn, query);
	row_count = PQntuples(tooltip_res);
	res_size = 0;

	int cursor[row_count][3];

	for (iter = 0; iter < row_count; iter++)
		res_size = res_size + (1 + strlen(PQgetvalue(tooltip_res, iter, 1)));

	if (res_size > 0) {
		buffer = (char *) malloc(res_size);
		strcpy(buffer, "");
	} else
		exit(ENOMEM);

	for (iter = 0; iter < row_count; iter++) {
		char *catid_char = PQgetvalue(tooltip_res, iter, 0);	// category id
		char *tag = PQgetvalue(tooltip_res, iter, 1);		// name
		char buffer1[strlen(tag) + 1];				// tag and space
		int catid, start, end;

		catid	= 0;
		start	= 0;
		end	= 0;

		if (strlen(buffer) > 0)
			start = strlen(buffer);
		else
			start = 0;

		end = (strlen(buffer) + strlen(tag));
		sscanf(catid_char, "%d", &catid);
		cursor[iter][0] = start;
		cursor[iter][1] = end;
		cursor[iter][2] = catid;

		/* set the text iters */
		sprintf(buffer1, "%s ", tag);	// tag name
		strcat(buffer, buffer1);
	}

	gtk_text_buffer_set_text(tag_tb, buffer, -1);
	for (iter = 0; iter < row_count; iter++) {
		gtk_text_buffer_get_iter_at_offset(tag_tb, &start_iter, cursor[iter][0]);
		gtk_text_buffer_get_iter_at_offset(tag_tb, &end_iter, cursor[iter][1]);
		gtk_text_buffer_apply_tag(tag_tb, tag_array[cursor[iter][2]], &start_iter, &end_iter);
	}

	PQclear(tooltip_res);

	/* Show the tooltip */
	popover = gtk_popover_new(widget);
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	viewport = gtk_viewport_new(NULL, NULL);

	gtk_container_add(GTK_CONTAINER(popover), viewport);
	gtk_container_add(GTK_CONTAINER(viewport), tag_tv);
	gtk_widget_show_all(popover);

	free(sha256);
}

extern void pixbuf_loader(GtkListStore *list_store) {
	int row_count, row_now;
	GtkTreeIter tree_iter;
	GtkTreePath *tree_path;
	PGresult *note_res;

	gtk_list_store_clear(list_store);
	if (safe_mode == true)
		note_res = PQexec(conn, "SELECT sha256 FROM public.files WHERE rating = 's' ORDER BY created_at DESC;");
	else
		note_res = PQexec(conn, "SELECT sha256 FROM public.files ORDER BY created_at DESC;");

	row_count = PQntuples(note_res);	// get the number of rows returned by the query
	tree_path = gtk_tree_path_new_from_string("0");

	if (row_count > 0)
		for (row_now = 0; row_now < row_count; row_now++) {
			GdkPixbufLoader *gdk_pixbuf_loader;
			char *path_base = get_thumb_path();
			char *file_path, *tree_path_str;
			size_t size = strlen(path_base), thumb_data_size = 0;
			FILE *thumb_file;
			long thumb_size;
			unsigned char *thumb_data;

			gdk_pixbuf_loader = gdk_pixbuf_loader_new_with_mime_type("image/png", NULL);
			tree_path_str = (char *) malloc(sizeof(int) + 2);
			file_path = (char *) malloc(size + 4 + PQgetlength(note_res, row_now, 0));
			sprintf(file_path, "%s%s.png", path_base, PQgetvalue(note_res, row_now, 0));	// [thumb_dir]/[size]/[sha256].png

			/* File operations */
			thumb_file = fopen(file_path, "r");
			if (thumb_file == NULL) {
				GtkWidget *err_img;

				printf("NULL\n");
				err_img = gtk_image_new_from_icon_name("image-missing", GTK_ICON_SIZE_DIALOG);

				goto error_point;
			}

			fseek(thumb_file, 0L, SEEK_END);
			thumb_size = ftell(thumb_file);
			rewind(thumb_file);

			thumb_data = (unsigned char *) malloc(thumb_size);
			thumb_data_size = fread(thumb_data, sizeof(char), thumb_size, thumb_file);

			gdk_pixbuf_loader_write(gdk_pixbuf_loader, thumb_data, thumb_data_size, NULL);
			gtk_list_store_append(list_store, &tree_iter);
			gtk_list_store_set(list_store, &tree_iter, SHA256_COL, PQgetvalue(note_res, row_now, 0), COL_PIXBUF, gdk_pixbuf_loader_get_pixbuf(gdk_pixbuf_loader), -1);
			sprintf(tree_path_str, "0:%d", row_now);
			gtk_tree_path_append_index(tree_path, row_now);

			free(thumb_data);
			error_point:
			fclose(thumb_file);
			free(file_path);
			free(tree_path_str);
			gdk_pixbuf_loader_close(gdk_pixbuf_loader, NULL);
		}

	PQclear(note_res);
	return;
}

extern GtkWidget *home_page(void) {
	GtkListStore *icon_list_store;
	GtkWidget *icon_view;
	GtkTreeIter tree_iter;
	GtkTreePath *tree_path;
	int icon_size;

	/* Create and fill the list store */
	icon_size = get_icon_size();
	icon_list_store = gtk_list_store_new(2, G_TYPE_STRING, GDK_TYPE_PIXBUF);
	pixbuf_loader(icon_list_store);

	icon_view = gtk_icon_view_new_with_model(GTK_TREE_MODEL(icon_list_store));
	tree_path = gtk_tree_model_get_path(GTK_TREE_MODEL(icon_list_store), &tree_iter);

	CallBackData_set_tree_path(viewer_data, tree_path);
	CallBackData_set_tree_model(viewer_data, GTK_TREE_MODEL(icon_list_store));
	CallBackData_set_list_store(viewer_data, icon_list_store);

	/* Setup containers */
	gtk_icon_view_set_selection_mode(GTK_ICON_VIEW(icon_view), GTK_SELECTION_MULTIPLE);
	gtk_icon_view_set_columns(GTK_ICON_VIEW(icon_view), -1);

	/* Advanced tooltip */
	gtk_widget_set_has_tooltip(icon_view, TRUE);
	//g_signal_connect(icon_view, "query-tooltip", G_CALLBACK(query_tooltip), icon_view);
	g_signal_connect(icon_view, "query-tooltip", G_CALLBACK(query_tooltip), icon_list_store);
	g_signal_connect(icon_view, "item-activated", G_CALLBACK(viewer), icon_list_store);

	/* Misc. */
	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(icon_view), COL_PIXBUF);
	gtk_icon_view_set_item_width(GTK_ICON_VIEW(icon_view), get_icon_size());
	gtk_icon_view_set_item_padding(GTK_ICON_VIEW(icon_view), 0);

	return icon_view;
}
