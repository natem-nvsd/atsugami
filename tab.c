/* tab.c: Generic tab */
#include "atsugami.h"
#include "colours.h"
#include "types.h"
#include <errno.h>
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GtkWidget *search_bar, *right_scrolled;
PGresult *tab_res;
CallBackData *viewer_data;

static void search_cb(GtkWidget **entry) {
	char *query;

	query = (char *) malloc(strlen(gtk_entry_get_text(GTK_ENTRY(entry))));
	sprintf(query, "%s", gtk_entry_get_text(GTK_ENTRY(entry)));
	printf("query: '%s'\n", query);

	//gtk_widget_destroy(tab_child);
	//tab_child = search();
}

static void select_cursor_item(GtkIconView *icon_view, GtkTreePath *tree_path, gpointer user_data) {
	GtkListStore *list_store;
	char *sha256;
	GtkTreeIter iter;
	
	dbg_info("Selected");
	list_store = GTK_LIST_STORE(user_data);
	gtk_tree_model_get_iter(GTK_TREE_MODEL(list_store), &iter, tree_path);
	gtk_tree_model_get(GTK_TREE_MODEL(list_store), &iter, SHA256_COL, &sha256, -1);
}

//static void item_activated(CallBackData **user_data) {
static void item_activated(gpointer user_data) {
	GtkTreeModel *tree_model;
 	char *sha256;
	int x;

	x = CallBackData_get_test_int(user_data);
	//x = (*user_data)->test_int;

	printf("%d\n", x);
	if (x != 999) {
		/* If userdata.test_int != 999, everything else won't work. */
	//	fprintf(stderr, "ERROR: user_data corruption detected.\n");
		dbg_err("user_data corruption detected.");
		CallBackData_destroy(user_data);
		return;
	}

	/* Get the SHA256 checksum of the icon the user clicked on */
	//tree_model = GTK_TREE_MODEL(gtk_icon_view_get_model(CallBackData_get_icon_view(*user_data)));
	//gtk_tree_model_get_iter(tree_model, CallBackData_get_tree_iter(*user_data), CallBackData_get_tree_path(*user_data));
 	//gtk_tree_model_get(tree_model, CallBackData_get_tree_iter(*user_data), SHA256_COL, &sha256, -1);

	tree_model = GTK_TREE_MODEL(gtk_icon_view_get_model(CallBackData_get_icon_view(user_data)));
	gtk_tree_model_get_iter(tree_model, CallBackData_get_tree_iter(user_data), CallBackData_get_tree_path(user_data));
 	gtk_tree_model_get(tree_model, CallBackData_get_tree_iter(user_data), SHA256_COL, &sha256, -1);

	/* Remove the old widget and display the new widget */
	//gtk_widget_destroy(CallBackData_get_child(*user_data));
	//CallBackData_set_child(*user_data, viewer(CallBackData_get_text_view(*user_data), CallBackData_get_text_buffer(*user_data), &sha256));
	//gtk_container_add(GTK_CONTAINER(CallBackData_get_parent(*user_data)), CallBackData_get_child(*user_data));
	//CallBackData_destroy(*user_data);

	gtk_widget_destroy(CallBackData_get_child(user_data));
	CallBackData_set_child(user_data, viewer(CallBackData_get_text_view(user_data), CallBackData_get_text_buffer(user_data), &sha256));
	gtk_container_add(GTK_CONTAINER(CallBackData_get_parent(user_data)), CallBackData_get_child(user_data));
	CallBackData_destroy(user_data);
}

extern void tab(void) {
	GtkWidget *pane, *side_search, *side_vbox, *left_scrolled, *tag_tv, *tab_child;
	GtkTextBuffer *tag_tb;
	GtkTextIter start_iter, end_iter;
	GtkTextTag *artist_tt, *copyright_tt, *character_tt, *general_tt, *meta_tt, *tag_array[5];
	GtkEntryCompletion *search_completion;
	int iter, row_count, page_count;
	size_t res_size;
	char *buffer;

	viewer_data = CallBackData_new();
	tab_child = GTK_WIDGET(home_page());
	pane = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
	side_search = gtk_search_entry_new();
	side_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	left_scrolled = gtk_scrolled_window_new(NULL, NULL);
	tag_tv = gtk_text_view_new();
	tag_tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tag_tv));
	right_scrolled = gtk_scrolled_window_new(NULL, NULL);
	search_bar = gtk_search_bar_new();
	search_completion = gtk_entry_completion_new();
	page_count = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));

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

	/* Set values of viewer_data */
	CallBackData_set_icon_view(viewer_data, GTK_ICON_VIEW(tab_child));		// icon_view *must* be set before list_store.
	CallBackData_set_parent(viewer_data, right_scrolled);
	CallBackData_set_child(viewer_data, tab_child);
	CallBackData_set_text_view(viewer_data, tag_tv);
	CallBackData_set_text_buffer(viewer_data, tag_tb);
	CallBackData_set_test_int(viewer_data, 999);

	/* tmp */
	printf("%d\n", CallBackData_get_test_int(viewer_data));

	/* Child callbacks */
	g_signal_connect(GTK_ICON_VIEW(tab_child), "item-activated", G_CALLBACK(item_activated), viewer_data);
	g_signal_connect(GTK_ICON_VIEW(tab_child), "select-cursor-item", G_CALLBACK(select_cursor_item), tab_child);

	/* Left Pane */
	gtk_paned_add1(GTK_PANED(pane), side_vbox);
	gtk_paned_pack1(GTK_PANED(pane), side_vbox, TRUE, FALSE);
	gtk_box_pack_start(GTK_BOX(side_vbox), side_search, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(side_vbox), left_scrolled, TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(left_scrolled), tag_tv);
	gtk_widget_set_vexpand(side_vbox, TRUE);
	gtk_widget_set_hexpand(side_vbox, TRUE);

	/* Left pane scrolled window */
	gtk_widget_set_vexpand_set(left_scrolled, TRUE);
	gtk_widget_set_hexpand_set(left_scrolled, TRUE);
	gtk_scrolled_window_set_propagate_natural_height(GTK_SCROLLED_WINDOW(left_scrolled), TRUE);
	gtk_scrolled_window_set_kinetic_scrolling(GTK_SCROLLED_WINDOW(left_scrolled), TRUE);

	/* Right pane scrolled window */
	gtk_widget_set_vexpand(right_scrolled, TRUE);
	gtk_widget_set_hexpand(right_scrolled, TRUE);
	gtk_scrolled_window_set_propagate_natural_height(GTK_SCROLLED_WINDOW(right_scrolled), TRUE);
	gtk_scrolled_window_set_kinetic_scrolling(GTK_SCROLLED_WINDOW(right_scrolled), TRUE);

	/* Entry */
	gtk_widget_add_accelerator(side_search, "grab-focus", accel, GDK_KEY_f, GDK_CONTROL_MASK, GTK_ACCEL_LOCKED);
	//gtk_widget_add_accelerator(side_search, "focus-out", accel, GDK_KEY_Escape, GDK_CONTROL_MASK, GTK_ACCEL_LOCKED);	// FIXME: The widget should lose focus when Esc pressed.
	gtk_entry_set_placeholder_text(GTK_ENTRY(side_search), "Search...");
	gtk_widget_set_margin_start(side_search, 8);
	gtk_widget_set_margin_end(side_search, 8);
	gtk_widget_set_margin_top(side_search, 8);
	gtk_widget_set_margin_bottom(side_search, 8);
	gtk_widget_set_tooltip_text(side_search, "Shortcut: ctrl+f");
	g_signal_connect(side_search, "activate", G_CALLBACK(search_cb), side_search);

	/* Search completion */
	gtk_entry_set_completion(GTK_ENTRY(side_search), search_completion);
	gtk_entry_completion_set_model(search_completion, completion_model);
	gtk_entry_completion_set_popup_completion(search_completion, TRUE);
	gtk_entry_completion_set_popup_single_match(search_completion, TRUE);
	gtk_entry_completion_set_text_column(search_completion, 0);

	/* Text View */
	gtk_text_view_set_editable(GTK_TEXT_VIEW(tag_tv), FALSE);
	gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(tag_tv), FALSE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(tag_tv), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tag_tv), GTK_WRAP_WORD_CHAR);
	gtk_text_view_set_top_margin(GTK_TEXT_VIEW(tag_tv), 4);
	gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(tag_tv), 4);
	gtk_text_view_set_left_margin(GTK_TEXT_VIEW(tag_tv), 4);
	gtk_text_view_set_right_margin(GTK_TEXT_VIEW(tag_tv), 4);

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
		end   = (strlen(buffer) + strlen(tag));

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
	free(buffer);

	/* Right Pane */
	gtk_paned_add2(GTK_PANED(pane), GTK_WIDGET(right_scrolled));
	gtk_container_add(GTK_CONTAINER(right_scrolled), tab_child);

	gtk_widget_show_all(pane);
	gtk_container_add(GTK_CONTAINER(notebook), pane);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), page_count);
}