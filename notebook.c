/* notebook.c */
#include <gtk/gtk.h>
#include "main.h"
#include "notebook.h"
#include <libpq-fe.h>
#include <stdio.h>

PGresult *note_res;

enum {
	COL_PATH,
	COL_PIXBUF,
	NUM_COLS
};

GdkPixbuf *thumb;
GtkTreeIter *iter;
GtkWidget *icon_view;

static void pixbuf_loader_and_store_filler(GtkListStore *list_store) {
	note_res = PQexec(conn, "SELECT store_dir FROM public.settings;");
	printf("store_dir\n");

	int x;		// Number of rows returned by the query
	int i;		// Current row
	size_t size = strlen(PQgetvalue(note_res, 0, 0));
	char *sha256, *tmp_path;
	const char path[65 + size];
	const char file_path[65 + size];
	GtkTreeIter tree_iter;

	tmp_path = PQgetvalue(note_res, 0, 0);

	strcpy(path, tmp_path);
	strcat(path, "/");
	PQclear(note_res);
	strcpy(tmp_path, "");
		// ASC and DESC are reversed, since they are inserted one by one into list_store

	gtk_list_store_clear(list_store);	// clear the list store
	note_res = PQexec(conn, "SELECT sha256 FROM public.files ORDER BY created_at DESC;");
	x = PQntuples(note_res);	// get the number of rows returned by the query

	for (i = 0; i < x; i++) {		// multithreading would make this much faster
		sha256 = PQgetvalue(note_res, i, 0);
		strcpy(file_path, path);
		strcat(file_path, sha256);

		thumb = gdk_pixbuf_new_from_file_at_scale(file_path, 180, 180, TRUE, NULL);

		gtk_list_store_append(list_store, &tree_iter);
		gtk_list_store_set(list_store, &tree_iter, COL_PATH, file_path, COL_PIXBUF, thumb, -1);

		sha256 = "";
	}

	PQclear(note_res);
}

static GtkListStore *create_list_store(void) {
	GtkListStore *list_store;

	list_store = gtk_list_store_new(3, G_TYPE_STRING, GDK_TYPE_PIXBUF, G_TYPE_INT);
	
	return list_store;
}

extern void home_page(void) {
	GtkWidget *icon_view, *iv_scrolled_window;
	GtkListStore *list_store;

	list_store = create_list_store();
	pixbuf_loader_and_store_filler(list_store);

	icon_view = gtk_icon_view_new_with_model(GTK_TREE_MODEL(list_store));
	iv_scrolled_window = gtk_scrolled_window_new(NULL, NULL);

	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(iv_scrolled_window), icon_view);
	gtk_icon_view_set_selection_mode(GTK_ICON_VIEW(icon_view), GTK_SELECTION_MULTIPLE);
	gtk_icon_view_set_columns(icon_view, -1);
	g_object_unref(list_store);

	gtk_scrolled_window_set_propagate_natural_height(GTK_SCROLLED_WINDOW(iv_scrolled_window), TRUE);
	gtk_scrolled_window_set_kinetic_scrolling(GTK_SCROLLED_WINDOW(iv_scrolled_window), TRUE);
	gtk_widget_set_vexpand(iv_scrolled_window, TRUE);

	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(icon_view), COL_PIXBUF);
	gtk_widget_show_all(iv_scrolled_window);
	gtk_widget_set_vexpand(iv_scrolled_window, TRUE);
	gtk_container_add(GTK_CONTAINER(notebook), iv_scrolled_window);
	gtk_notebook_set_tab_label_text(notebook, iv_scrolled_window, "Home");
}
