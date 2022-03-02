/* tab.c: Generic tab */
#include "main.h"
#include "callbacks.h"
#include "colours.h"
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GtkWidget *search_bar;
PGresult *tab_res;

static void search_cb(void) {
}

extern void tab(void) {
	GtkWidget *pane, *side_search, *side_vbox, *side_scrolled, *tag_tv, *main_scrolled, *search_bar;
	GtkTextBuffer *tag_tb;
	GtkTextIter start_iter, end_iter;
	GtkTextTag *artist_tt, *copyright_tt, *character_tt, *general_tt, *meta_tt, *tag_array[5];
	int iter, row_count;
	size_t res_size;
	char *buffer;

	pane = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
	side_search = gtk_search_entry_new();
	side_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	side_scrolled = gtk_scrolled_window_new(NULL, NULL);
	tag_tv = gtk_text_view_new();
	tag_tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tag_tv));
	main_scrolled = gtk_scrolled_window_new(NULL, NULL);
	search_bar = gtk_search_bar_new();
	artist_tt 	= gtk_text_buffer_create_tag(tag_tb, "artist", 		"foreground", ARTIST, NULL);
	copyright_tt	= gtk_text_buffer_create_tag(tag_tb, "copyright",	"foreground", COPYRIGHT, NULL);
	character_tt	= gtk_text_buffer_create_tag(tag_tb, "character",	"foreground", CHARACTER, NULL);
	general_tt	= gtk_text_buffer_create_tag(tag_tb, "general",		"foreground", GENERAL, NULL);
	meta_tt		= gtk_text_buffer_create_tag(tag_tb, "meta",		"foreground", META, NULL);
	tag_array[0] = artist_tt;
	tag_array[1] = copyright_tt;
	tag_array[2] = character_tt;
	tag_array[3] = general_tt;
	tag_array[4] = meta_tt;

	/* Left Pane */
	gtk_paned_add1(GTK_PANED(pane), side_vbox);
	gtk_box_pack_start(GTK_BOX(side_vbox), search_bar, FALSE, FALSE, 0);
	gtk_container_add(GTK_CONTAINER(search_bar), side_search);
	gtk_box_pack_start(GTK_BOX(side_vbox), side_scrolled, TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(side_scrolled), tag_tv);

	/* Entry */
	gtk_search_bar_connect_entry(GTK_SEARCH_BAR(search_bar), GTK_ENTRY(side_search));
	gtk_search_bar_set_search_mode(GTK_SEARCH_BAR(search_bar), TRUE);
	gtk_entry_set_placeholder_text(GTK_ENTRY(side_search), "Search...");

	/* Text View */
	gtk_text_view_set_editable(GTK_TEXT_VIEW(tag_tv), FALSE);
	gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(tag_tv), FALSE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(tag_tv), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tag_tv), GTK_WRAP_WORD_CHAR);

	tab_res = PQexec(conn, "SELECT tags_categories.category_id, tags_categories.tag_id, (SELECT COUNT(tag_id) AS counter FROM public.files_tags WHERE tag_id = tags_categories.tag_id), tags.name FROM public.tags_categories tags_categories INNER JOIN public.tags tags ON tags_categories.tag_id = tags.id ORDER BY counter DESC;");
	row_count = PQntuples(tab_res);

	for (iter = 0; iter < row_count; iter++)
		res_size = res_size + strlen(PQgetvalue(tab_res, 3, 0));
	buffer = (char *) malloc(res_size);

	int cursor[row_count][2][2];	// start, end, catid

	for (iter = 0; iter < row_count; iter++) {
		char *tag = PQgetvalue(tab_res, iter, 3);
		char *catid_char = PQgetvalue(tab_res, iter, 0);
		char *tag_count = PQgetvalue(tab_res, iter, 2);
		char buffer1[(strlen(tag) + strlen(tag_count) + 3)];	// tag, tag_count, size of space and new line character
		int catid = 0, start = 0, end = 0;

		if (strlen(buffer) > 0)
			start = strlen(buffer);
		else
			start = 0;
		end   = (strlen(buffer) + strlen(tag) + strlen(tag_count) + 1);

		sscanf(catid_char, "%d", &catid);

		cursor[iter][0][0] = start;
		cursor[iter][1][0] = end;
		cursor[iter][0][1] = catid;

		/* set the text iters */
		sprintf(buffer1, "%s %s\n", tag, tag_count);	// tag name, tag count
		strcat(buffer, buffer1);
		gtk_text_buffer_set_text(tag_tb, buffer, -1);
	}
	for (iter = 0; iter < row_count; iter++) {
		gtk_text_buffer_get_iter_at_offset(tag_tb, &start_iter, cursor[iter][0][0]);
		gtk_text_buffer_get_iter_at_offset(tag_tb, &end_iter, cursor[iter][1][0]);
		gtk_text_buffer_apply_tag(tag_tb, tag_array[cursor[iter][0][1]], &start_iter, &end_iter);
	}

	PQclear(tab_res);
	buffer = NULL;
	free(buffer);

	/* Right Pane */
	gtk_paned_add2(GTK_PANED(pane), GTK_WIDGET(main_scrolled));
	home_page(GTK_WIDGET(main_scrolled));

	gtk_widget_show_all(pane);
	gtk_container_add(GTK_CONTAINER(notebook), pane);
}
