/* tab.c: Generic tab */
#include "main.h"
#include "callbacks.h"
#include <gtk/gtk.h>
#include <libpq-fe.h>

GtkWidget *search_bar;
PGresult *tab_res;

static void search_cb(void) {
}

extern void tab(void) {
	GtkWidget *pane, *side_search, *side_vbox, *side_scrolled, *tag_tv, *main_scrolled;
	GtkTextBuffer *tag_tb;

	pane = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
	side_search = gtk_search_entry_new();
	side_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	side_scrolled = gtk_scrolled_window_new(NULL, NULL);
	tag_tv = gtk_text_view_new();
	tag_tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tag_tv));
	main_scrolled = gtk_scrolled_window_new(NULL, NULL);

	/* Left Pane */
	gtk_paned_add1(GTK_PANED(pane), side_vbox);
	gtk_box_pack_start(GTK_BOX(side_vbox), side_search, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(side_vbox), GTK_WIDGET(side_scrolled), TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(side_scrolled), tag_tv);

	/* Entry */
	gtk_entry_set_placeholder_text(GTK_ENTRY(side_search), "Search...");

	/* Text View */
	gtk_text_view_set_editable(GTK_TEXT_VIEW(tag_tv), FALSE);
	gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(tag_tv), FALSE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(tag_tv), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tag_tv), GTK_WRAP_WORD);
	gtk_text_buffer_set_text(tag_tb, "tetx", -1);

	/* Right Pane */
	gtk_paned_add2(GTK_PANED(pane), GTK_WIDGET(main_scrolled));
	home_page(GTK_WIDGET(main_scrolled));

	gtk_widget_show_all(pane);
	gtk_container_add(GTK_CONTAINER(notebook), pane);
}
