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
gchar *parent;
const gchar *file_path = NULL;

static void pixbuf_loader_and_store_filler(GtkListStore *list_store) {
	int x;		// Number of rows returned by the query
	int i;		// Current row
	GtkTreeIter tree_iter;

	note_res = PQexec(conn, "SELECT path FROM public.files ORDER BY imported_at ASC;");	// execute the query once to save time and resources

	gtk_list_store_clear(list_store);	// clear the list store
	for (i = 0; i < x; i++) {		// multithreading would make this much faster
		//gtk_list_store_clear(list_store);
		//note_res = PQexec(conn, "SELECT path FROM public.files ORDER BY imported_at ASC;");
		x = PQntuples(note_res);	// get the number of rows returned by the query
		file_path = PQgetvalue(note_res, i, 0);	// get the path from the query
		thumb = gdk_pixbuf_new_from_file_at_scale(file_path, 180, 180, TRUE, NULL);

		//g_assert(thumb);
		gtk_list_store_append(list_store, &tree_iter);
		gtk_list_store_set(list_store, &tree_iter, COL_PATH, file_path, COL_PIXBUF, thumb, -1);
		printf("Current path: %s\n", file_path);

		file_path = "";
//		g_free(thumb);
	}
	PQclear(note_res);

}

static GtkListStore *create_list_store(void) {
	GtkListStore *list_store;

	list_store = gtk_list_store_new(3, G_TYPE_STRING, GDK_TYPE_PIXBUF, G_TYPE_INT);
	
	return list_store;
}

extern void home_page(void) {
	GtkWidget *icon_view;
	GtkListStore *list_store;

	//parent = g_strdup("/");
	list_store = create_list_store();
	pixbuf_loader_and_store_filler(list_store);

	icon_view = gtk_icon_view_new_with_model(GTK_TREE_MODEL(list_store));

	gtk_icon_view_set_selection_mode(GTK_ICON_VIEW(icon_view), GTK_SELECTION_MULTIPLE);
	gtk_icon_view_set_columns(icon_view, -1);
	//gtk_icon_view_set_row_spacing(icon_view, -1);

	g_object_unref(list_store);

	//g_signal_connect(icon_view, "item-activated", G_CALLBACK(item_activated_cb), list_store);
	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(icon_view), COL_PIXBUF);

	gtk_container_add(GTK_CONTAINER(tab_bar), icon_view);
	gtk_notebook_set_tab_label_text(tab_bar, icon_view, "Home");

	gtk_widget_show_all(icon_view);
}
