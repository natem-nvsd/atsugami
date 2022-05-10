/***************************************************************\
*	wiki.c (c) 2022, by Nate Morrison			*
*								*
*	Wiki page functionality					*
\***************************************************************/

#include "atsugami.h"
#include <gtk/gtk.h>
#include <libpq-fe.h>

GtkWidget *wiki_pane;

static void close_tab(void) {
	gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), wiki_pane);
	if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook)) == 0)
		quit_activate();
}

extern void wiki(void) {
	PGresult *wiki_res;
	GtkWidget *wiki_search, *left_scrolled, *right_scrolled, *left_tv, *right_tv, *left_box, *right_box, *tab_label_box, *tab_icon, *tab_label, *tab_close;
	GtkTextBuffer *left_tb, *right_tb;
	GtkTextIter start_iter, end_iter;
	GtkTextTag *title;
	int page_count;

	wiki_pane = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
	wiki_search = gtk_search_entry_new();
	left_scrolled = gtk_scrolled_window_new(NULL, NULL);
	right_scrolled = gtk_scrolled_window_new(NULL, NULL);
	left_tv = gtk_text_view_new();
	right_tv = gtk_text_view_new();
	left_tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(left_tv));
	right_tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(right_tv));
	left_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	page_count = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));

	/* Left pane */
	gtk_paned_add1(GTK_PANED(wiki_pane), left_box);
	gtk_paned_pack1(GTK_PANED(wiki_pane), left_box, TRUE, FALSE);
	gtk_box_pack_start(GTK_BOX(left_box), wiki_search, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(left_box), left_scrolled, TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(left_scrolled), left_tv);
	gtk_widget_set_vexpand(left_box, TRUE);
	gtk_widget_set_hexpand(left_box, TRUE);

	/* Text view */
	gtk_text_view_set_editable(GTK_TEXT_VIEW(left_tv), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(left_tv), GTK_WRAP_WORD_CHAR);
	gtk_text_view_set_monospace(GTK_TEXT_VIEW(left_tv), FALSE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(left_tv), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(left_tv), GTK_WRAP_CHAR);
	gtk_text_view_set_left_margin(GTK_TEXT_VIEW(left_tv), 12);
	gtk_text_view_set_right_margin(GTK_TEXT_VIEW(left_tv), 12);
	gtk_text_view_set_top_margin(GTK_TEXT_VIEW(left_tv), 12);
	gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(left_tv), 12);

	/* Search */
	gtk_entry_set_placeholder_text(GTK_ENTRY(wiki_search), "Search...");
	gtk_widget_set_margin_start(wiki_search, 8);
	gtk_widget_set_margin_end(wiki_search, 8);
	gtk_widget_set_margin_top(wiki_search, 8);
	gtk_widget_set_margin_bottom(wiki_search, 8);
	gtk_widget_set_tooltip_text(wiki_search, "Shortcut: ctrl+f");

	/* Right pane */
	gtk_paned_add2(GTK_PANED(wiki_pane), right_scrolled);
	gtk_container_add(GTK_CONTAINER(right_scrolled), right_box);
	gtk_box_pack_start(GTK_BOX(right_box), right_tv, TRUE, TRUE, 0);

	/* Text view */
	gtk_text_view_set_editable(GTK_TEXT_VIEW(right_tv), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(right_tv), GTK_WRAP_WORD_CHAR);
	gtk_text_view_set_monospace(GTK_TEXT_VIEW(right_tv), FALSE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(right_tv), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(right_tv), GTK_WRAP_CHAR);
	gtk_text_view_set_left_margin(GTK_TEXT_VIEW(right_tv), 12);
	gtk_text_view_set_right_margin(GTK_TEXT_VIEW(right_tv), 12);
	gtk_text_view_set_top_margin(GTK_TEXT_VIEW(right_tv), 12);
	gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(right_tv), 12);

	gtk_text_buffer_set_text(right_tb, "Wiki", -1);
	title = gtk_text_buffer_create_tag(right_tb, "title", "weight", "bold", "scale", 2.000, NULL);
	gtk_text_buffer_get_iter_at_offset(right_tb, &start_iter, 0);
	gtk_text_buffer_get_iter_at_offset(right_tb, &end_iter, 9);
	gtk_text_buffer_apply_tag(right_tb, title, &start_iter, &end_iter);

	/* Tab label */
	tab_label_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
	tab_icon = gtk_image_new_from_icon_name("text-x-generic", GTK_ICON_SIZE_MENU);
	tab_label = gtk_label_new("Wiki home");
	tab_close = gtk_button_new_from_icon_name("window-close-symbolic", GTK_ICON_SIZE_MENU);

	gtk_button_set_relief(GTK_BUTTON(tab_close), GTK_RELIEF_NONE);
	gtk_widget_add_accelerator(tab_close, "clicked", accel, GDK_KEY_w, GDK_CONTROL_MASK, GTK_ACCEL_LOCKED);
	g_signal_connect(GTK_BUTTON(tab_close), "clicked", G_CALLBACK(close_tab), NULL);

	gtk_box_pack_start(GTK_BOX(tab_label_box), tab_icon, FALSE, FALSE, 4);
	gtk_box_pack_start(GTK_BOX(tab_label_box), tab_label, FALSE, FALSE, 4);
	gtk_box_pack_start(GTK_BOX(tab_label_box), tab_close, FALSE, FALSE, 4);

	gtk_widget_show_all(tab_label_box);
	gtk_widget_show_all(wiki_pane);
	gtk_container_add(GTK_CONTAINER(notebook), wiki_pane);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook), wiki_pane, tab_label_box);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), page_count);
}
