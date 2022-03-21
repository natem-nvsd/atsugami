/***************************************************************\
* tooltip.c: (c) 2022, by Nate Morrison				*
*								*
* Code for the Advanced Tooltip.				*
* The Advanced Tooltip (AT) shows a coloured list of the tags 	*
* belonging to the file the user's cursor is hovering over.	*
*								*
* The AT in Atsugami is based on the Advanced Tooltip found in	*
* Danbooru.							*
*								*
* TODO: replace cursor with an [iter][3] array			*
*								*
\***************************************************************/

#include "atsugami.h"
#include "colours.h"
#include "types.h"
#include <errno.h>
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdio.h>

extern GtkTooltip *att(char *sha256, GtkWidget *rel2, GtkCellRenderer *cell) {
	/* FIXME:
	 * GLib-GObject-WARNING **: invalid cast from 'GtkPopover' to 'GtkTooltip'
	 * Gtk-CRITICAL **: gtk_icon_view_set_tooltip_cell: assertion 'GTK_IS_TOOLTIP (tooltip)' failed
	 *
	 * Crashes sometimes:
	 * free(): invalid next size (normal)
	 * Abort (core dumped)
	 */
	PGresult *att_res;
	GtkWidget *att_tv;
	GtkTextBuffer *att_tb;
	GtkTextIter start_iter, end_iter;
	GtkTextTag *artist_tt, *copyright_tt, *character_tt, *general_tt, *meta_tt, *tag_array[5];
	char query[458 + sizeof(long)], *buffer = NULL;
	int iter = 0, row_count = 0;
	size_t res_size;
	AdvancedTooltip advanced_tooltip;

	printf("'%s'\n", sha256);	// debugging
	if (buffer != NULL) {
		dbg_info("tooltip.c: Freeing (First)");
		free(buffer);		/* Crashes here sometimes, but only after many query_tooltipo cycles */
		dbg_info("tooltip.c: Freed");
	}
	else {
		dbg_err("tooltip.c: buffer is NULL (0/1)");
		exit(EADDRNOTAVAIL);
	}

	/* Setup the tooltip widget */
	advanced_tooltip = AdvancedTooltip_new(rel2);
	att_tv = gtk_text_view_new();
	att_tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(att_tv));

	/* Tag highlighting */
	artist_tt = gtk_text_buffer_create_tag(att_tb, "artist", "foreground", ARTIST, NULL);
	copyright_tt = gtk_text_buffer_create_tag(att_tb, "copyright", "foreground", COPYRIGHT, NULL);
	character_tt = gtk_text_buffer_create_tag(att_tb, "character", "foreground", CHARACTER, NULL);
	general_tt = gtk_text_buffer_create_tag(att_tb, "general", "foreground", GENERAL, NULL);
	meta_tt = gtk_text_buffer_create_tag(att_tb, "meta", "foreground", META, NULL);
	tag_array[0] = artist_tt;
	tag_array[1] = copyright_tt;
	tag_array[2] = character_tt;
	tag_array[3] = general_tt;
	tag_array[4] = meta_tt;

	/* Display the tooltip */
	gtk_container_add(GTK_CONTAINER(advanced_tooltip.tooltip), att_tv);
	gtk_widget_set_hexpand(att_tv, TRUE);
	gtk_widget_set_vexpand(att_tv, TRUE);

	/* Text view */
	gtk_text_view_set_editable(GTK_TEXT_VIEW(att_tv), FALSE);
	gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(att_tv), FALSE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(att_tv), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(att_tv), GTK_WRAP_WORD);
	gtk_text_view_set_top_margin(GTK_TEXT_VIEW(att_tv), 4);
	gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(att_tv), 4);
	gtk_text_view_set_left_margin(GTK_TEXT_VIEW(att_tv), 4);
	gtk_text_view_set_right_margin(GTK_TEXT_VIEW(att_tv), 4);

	/* Get the file's id from its sha256 sum */
	sprintf(query, "SELECT id FROM public.files WHERE sha256 = '%s';", sha256);
	att_res = PQexec(conn, query);

	/* Get the tags associated with the file */
	sprintf(query, "SELECT tags.name, tags.id AS tag_id, files.id AS file_id, tags_cats.category_id FROM public.tags AS tags INNER JOIN (SELECT * FROM public.files_tags WHERE file_id = %s) files_tags ON files_tags.tag_id = tags.id INNER JOIN public.files AS files ON files_tags.file_id = files.id INNER JOIN public.tags_categories AS tags_cats ON tags_cats.category_id = tags_cats.category_id WHERE tags_cats.tag_id = tags.id ORDER BY tags.name ASC;", PQgetvalue(att_res, 0, 0)); 
	/*              name              | tag_id | file_id | category_id 
	 * -------------------------------+--------+---------+-------------
	 */
	PQclear(att_res);

	res_size = 0;		// if this isn't set every time, it will crash
	att_res = PQexec(conn, query);
	row_count = PQntuples(att_res);
	int cursor[row_count][3];    // start; end, catid

	/* Fill the text buffer */
	for (iter = 0; iter < row_count; iter++)
		res_size = res_size + strlen(PQgetvalue(att_res, iter, 0));

	printf("res_size1 = %lu\n", res_size);
	/* Allocate memory to buffer if the query result is > 0.
	 * This is rundant, but it provides addition protection. */
	if (res_size > 0) {
		buffer = (char *) malloc(res_size);
		strcpy(buffer, "");	// remove all the crap in buffer
	}
	else
		exit(ENOMEM);

	if (buffer[strlen(buffer)] != '\0')
		dbg_warn("tooltip.c: Buffer is not null-terminated.");
	else
		dbg_info("tooltip.c: Buffer is null-terminated.");

	for (iter = 0; iter < row_count; iter++) {
		char *tag = PQgetvalue(att_res, iter, 0);		// name
		char *catid_char = PQgetvalue(att_res, iter, 3);	// category id
		char buffer1[strlen(tag) + 1];	// tag and space
		int catid = 0, start = 0, end = 0;

		if (strlen(buffer) > 0)
			start = strlen(buffer);
		else
			start = 0;
		end   = (strlen(buffer) + strlen(tag));

		sscanf(catid_char, "%d", &catid);
		cursor[iter][0] = start;
		cursor[iter][1] = end;
		cursor[iter][2] = catid;
		
		/* set the text iters */
		sprintf(buffer1, "%s ", tag);	// tag name
		strcat(buffer, buffer1);
	}

	gtk_text_buffer_set_text(att_tb, buffer, -1);
	for (iter = 0; iter < row_count; iter++) {
		gtk_text_buffer_get_iter_at_offset(att_tb, &start_iter, cursor[iter][0]);
		gtk_text_buffer_get_iter_at_offset(att_tb, &end_iter, cursor[iter][1]);
		gtk_text_buffer_apply_tag(att_tb, tag_array[cursor[iter][2]], &start_iter, &end_iter);
	}

	/* Free memory */
	PQclear(att_res);
	if (buffer != NULL) {
		dbg_info("tooltip.c: Freeing (Final)");
		free(buffer);		/* Crashes here sometimes, but only after many query_tooltipo cycles */
		dbg_info("tooltip.c: Freed");
	}
	else {
		dbg_err("tooltip.c: buffer is NULL (1/1)");
		exit(EADDRNOTAVAIL);
	}

	gtk_widget_show_all(advanced_tooltip.tooltip);
	return GTK_TOOLTIP(advanced_tooltip.tooltip);	// segfault
}
