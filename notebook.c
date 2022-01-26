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

static void pixbuf_loader_and_store_filler(GtkWidget *list_store) {
	int x = 0;	// Number of rows returned by the query
	int i;		// Current row
	char *sha256, *tmp_path;
	char *path;	// DO NOT INITIALIZE
	char *file_path;
	GtkTreeIter tree_iter;

	note_res = PQexec(conn, "SELECT store_dir FROM public.settings;");
	tmp_path = PQgetvalue(note_res, 0, 0);

	strcpy(path, tmp_path);
	strcat(path, "/");
	PQclear(note_res);
	strcpy(tmp_path, "");
	printf("tmp_path: %s\n", tmp_path);

	note_res = PQexec(conn, "SELECT sha256 FROM public.files ORDER BY created_at DESC;");
	x = PQntuples(note_res);

	for (i = 0; i < x; i++) {		// multithreading would make this much faster
		sha256 = PQgetvalue(note_res, i, 0);	// get the sha256
		tmp_path = path;
		file_path = path;

		printf("tmp_path: %s\n", tmp_path);
		strcpy(file_path, tmp_path);
		printf("path0: %s\n", path);
		printf("path1: %s\n", file_path);

		strcat(file_path, sha256);
		printf("path2: %s\n", file_path);
		printf("sha256: %s\n", sha256);

		//g_assert(thumb);
		gtk_list_store_append(GTK_LIST_STORE(list_store), &tree_iter);
		gtk_list_store_set(GTK_LIST_STORE(list_store), &tree_iter, COL_PATH, file_path, COL_PIXBUF, thumb, -1);

		thumb = gdk_pixbuf_new_from_file_at_scale(file_path, 180, 180, TRUE, NULL);
		file_path = "";
		sha256 = "";
	}

	PQclear(note_res);
}

extern void notebook_reload(GtkWidget *list_store) {
}

extern void notebook_reload_relay(void) {
	GtkWidget *list_store = NULL;
	notebook_reload(list_store);
}

static void item_selected_cb(GtkWidget *pop_over, GtkWidget *list_store) {
	gtk_popover_set_pointing_to(GTK_POPOVER(pop_over), list_store);
	gtk_popover_popup(GTK_POPOVER(pop_over));
}

/*
static GtkListStore *create_list_store(void) {
	GtkListStore *list_store;

	list_store = gtk_list_store_new(3, G_TYPE_STRING, GDK_TYPE_PIXBUF, G_TYPE_INT);
	
	return list_store;
}
*/

extern void home_page(void) {
	GtkWidget *icon_view, *pop_over, *iv_scrolled_window, *list_store = NULL;
	//GtkScrolledWindow *iv_scrolled_window;
	
	//GtkListStore *list_store;

	//list_store = create_list_store();
	pixbuf_loader_and_store_filler(list_store);

	icon_view = gtk_icon_view_new_with_model(GTK_TREE_MODEL(list_store));
	iv_scrolled_window = gtk_scrolled_window_new(NULL, NULL);

	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(iv_scrolled_window), icon_view);

	gtk_icon_view_set_selection_mode(GTK_ICON_VIEW(icon_view), GTK_SELECTION_MULTIPLE);
	gtk_icon_view_set_columns(GTK_ICON_VIEW(icon_view), -1);

	g_object_unref(list_store);

	/* Popover */
	pop_over = gtk_popover_new(list_store);

	//g_signal_connect(icon_view, "item-activated", G_CALLBACK(item_activated_cb), list_store);
	g_signal_connect(icon_view, "select-cursor-item", G_CALLBACK(item_selected_cb), list_store);
	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(icon_view), COL_PIXBUF);

	/* set the size of icon_view */
	gtk_scrolled_window_set_propagate_natural_height(GTK_SCROLLED_WINDOW(iv_scrolled_window), TRUE);
	gtk_scrolled_window_set_kinetic_scrolling(GTK_SCROLLED_WINDOW(iv_scrolled_window), TRUE);

	gtk_widget_show_all(iv_scrolled_window);
	gtk_widget_set_vexpand(iv_scrolled_window, TRUE);
	gtk_container_add(GTK_CONTAINER(notebook), iv_scrolled_window);
	gtk_notebook_set_tab_label_text(notebook, iv_scrolled_window, "Home");
}
