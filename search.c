/* search.c */
#include "atsugami.h"
#include "types.h"
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdio.h>

PGresult *note_res;

/*
enum {
	COL_PATH,
	COL_PIXBUF,
	NUM_COLS
};
*/

GdkPixbuf *thumb;
GtkTreeIter *iter;
GtkWidget *icon_view;

static void pixbuf_loader(GtkListStore *list_store) {
	note_res = PQexec(conn, "SELECT thumb_dir, thumb_siz FROM public.settings;");

	int row_count;		// Number of rows returned by the query
	int row_now;		// Current row
	size_t size = strlen(PQgetvalue(note_res, 0, 0));
	char path[3 + size];
	char file_path[72 + size];
	GtkTreeIter tree_iter;

	/* Initial path; [thumb_dir]/[size]/ */
	sprintf(path, "%s/%s/", PQgetvalue(note_res, 0, 0), PQgetvalue(note_res, 0, 1));
	PQclear(note_res); 	// ASC and DESC are reversed, since they are inserted one by one into list_store
	gtk_list_store_clear(list_store);	// clear the list store

	note_res = PQexec(conn, "SELECT sha256 FROM public.files ORDER BY created_at DESC;");
	row_count = PQntuples(note_res);	// get the number of rows returned by the query

	if (row_count != 0) {
		for (row_now = 0; row_now < row_count; row_now++) {
			sprintf(file_path, "%s%s.png", path, PQgetvalue(note_res, row_now, 0));	// [thumb_dir]/[size]/[sha256]

			thumb = gdk_pixbuf_new_from_file(file_path, NULL);

			gtk_list_store_append(list_store, &tree_iter);
			gtk_list_store_set(list_store, &tree_iter, SHA256_COL, file_path, COL_PIXBUF, thumb, -1);
		}
	}

	PQclear(note_res);
}

extern void search(GtkWidget *parent) {
	GtkWidget *icon_view, *iv_scrolled_window;
	GtkListStore *list_store;

	list_store = gtk_list_store_new(2, G_TYPE_STRING, GDK_TYPE_PIXBUF, G_TYPE_INT);
	pixbuf_loader(list_store);

	icon_view = gtk_icon_view_new_with_model(GTK_TREE_MODEL(list_store));
	iv_scrolled_window = gtk_scrolled_window_new(NULL, NULL);

	gtk_container_add(GTK_CONTAINER(iv_scrolled_window), icon_view);
	gtk_icon_view_set_selection_mode(GTK_ICON_VIEW(icon_view), GTK_SELECTION_MULTIPLE);
	gtk_icon_view_set_columns(GTK_ICON_VIEW(icon_view), -1);
	g_object_unref(list_store);

	gtk_scrolled_window_set_propagate_natural_height(GTK_SCROLLED_WINDOW(iv_scrolled_window), TRUE);
	gtk_scrolled_window_set_kinetic_scrolling(GTK_SCROLLED_WINDOW(iv_scrolled_window), TRUE);
	gtk_widget_set_vexpand(iv_scrolled_window, TRUE);

	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(icon_view), COL_PIXBUF);
	gtk_widget_show_all(iv_scrolled_window);
	gtk_widget_set_vexpand(iv_scrolled_window, TRUE);
	gtk_container_add(GTK_CONTAINER(parent), iv_scrolled_window);
}
