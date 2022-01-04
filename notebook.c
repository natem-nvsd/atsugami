/* notebook.c */
#include <gtk/gtk.h>
#include "main.h"
#include "notebook.h"
#include <libpq-fe.h>
#include <stdio.h>

PGresult *note_res;

enum {
	COL_PATH,
	COL_DISPLAY_NAME,
	COL_PIXBUF,
	NUM_COLS
};

GdkPixbuf *thumb_preview;
GtkTreeIter *iter;
GtkTreeModel *tree_model;
GtkTreePath *tree_path;
GtkWidget *icon_view;
gchar *parent;
const gchar *file_path = NULL;

static void pixbuf_loader(void) {
	int x;
	int i;

	for (i = 0; i < x; i++) {
		note_res = PQexec(conn, "SELECT path FROM public.files ORDER BY imported_at ASC;");
		x = PQntuples(note_res);
		file_path = PQgetvalue(note_res, i, 0);
		thumb_preview = gdk_pixbuf_new_from_file(file_path, NULL);

		printf("Current path: %s\n", file_path);
		/*
		if (x == 0 || x == NULL) {
			printf("Nobody here but us chickens!\n");
		}
		*/
		PQclear(note_res);
		file_path = "";
	}

}

static void store_filler(GtkListStore *list_store) {
	gtk_list_store_clear(list_store);

	//if (safemode_enable == 1)
		//note_res = PQexecParams(conn, "SELECT path FROM public.files WHERE rating = 'safe' ORDER BY imported_at ASC;", 0 NULL, NULL, NULL, 0, 0);
	//if (safemode_enable == 0)
	//	note_res = PQexecParams(conn, "SELECT path FROM public.files ORDER BY imported_at ASC;", 0, NULL, NULL, NULL, 0, 0);
}

static GtkListStore *create_list_store(void) {
	GtkListStore *list_store;
	list_store = gtk_list_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, GDK_TYPE_PIXBUF);
	
	return list_store;
}
/*
static void item_activated_cb(GtkIconView *icon_view, GtkTreePath *tree_path, gpointer user_data) {
	GtkListStore *list_store;
	//gchar *file_path;
	GtkTreeIter *tree_iter;

	list_store = GTK_LIST_STORE(user_data);

	gtk_tree_model_get_iter(GTK_TREE_MODEL(list_store), &tree_iter, file_path);
	parent = file_path;
}
*/

extern void home_page(void) {
	GtkWidget *icon_view;
	GtkListStore *list_store;
	
	pixbuf_loader();
	list_store = create_list_store();
	store_filler(list_store);

	icon_view = gtk_icon_view_new_with_model(GTK_TREE_MODEL(tree_model));

	gtk_icon_view_set_selection_mode(GTK_ICON_VIEW(icon_view), GTK_SELECTION_MULTIPLE);
	gtk_icon_view_set_columns(icon_view, -1);
	gtk_icon_view_set_row_spacing(icon_view, 5);

	g_object_unref(list_store);

	//g_signal_connect(icon_view, "item-activated", G_CALLBACK(item_activated_cb), list_store);
	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(icon_view), COL_PIXBUF);

	gtk_container_add(GTK_CONTAINER(tab_bar), icon_view);		//
	gtk_notebook_set_tab_label_text(tab_bar, icon_view, "Home");
}
