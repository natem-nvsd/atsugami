/* notebook.c: Icon view */
#include "atsugami.h"
#include "types.h"
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdio.h>

PGresult *note_res;
GtkTreeIter ico_tree_iter;
GtkListStore *ico_list_store;
GtkTreePath *ico_tree_path;

static int query_tooltip(GtkWidget *widget, gint x, gint y, gboolean keyboard_mode, GtkTooltip *tooltip, gpointer user_data) {
	/************************************************\
	** TOOLTIP OVERVIEW				**
	** ================				**
	**						**
	** 0. user hovers over icon			**
	** 1. callback emitted				**
	** i. Get tree model from icon view		**
	** ii. Get tree path from icon view		**
	** iii. Get item at position			**
	** iv. Get widget from bin window coords	**
	** v. Get cell_rect from icon view 		**
	** vi. Set tooltip cell				**
	** vii. Run tooltip function			**
	**						**
	\************************************************/
	GtkTreeModel *tree_model;
	GtkTreePath *tree_path;
	GtkCellRenderer *cell;
	GdkRectangle *rect;
	gboolean tooltip_status;
	char *sha256;

	printf("\n\n");
	/* Prevent execution with a null pointer */
	if (user_data == NULL) {
		printf("user_data is NULL\n");
		return 1;
	}
	
	/* Prepare the tooltip */
	keyboard_mode = FALSE;
	tree_model = GTK_TREE_MODEL(gtk_icon_view_get_model(user_data));
	tree_path = gtk_icon_view_get_path_at_pos(GTK_ICON_VIEW(widget), x, y);

	/* tree_path is NULL when the user's cursor is not over an icon, and vice versa */
	if (tree_path == NULL) {
		fprintf(stderr, "path is null.\n");
		return 1;
	}
	else {
		printf("path is NOT null.\n");
	}

	gtk_icon_view_get_item_at_pos(GTK_ICON_VIEW(user_data), x, y, &tree_path, &cell);
	//gtk_icon_view_get_cell_rect(GTK_ICON_VIEW(user_data), tree_path, cell, rect);	// segfault here
	// widget=0x0000000000862290, x=367, y=191, keyboard_mode=0, tooltip=<unavailable>, user_data=0x0000000000862290
	gtk_icon_view_set_tooltip_column(GTK_ICON_VIEW(user_data), 0);

	/* Get the SHA256 checksum */
	sha256 = (char *) malloc(65);
	gtk_tree_model_get(tree_model, &ico_tree_iter, 0, &sha256, -1);
	printf("'%s'\n", sha256);

	/* SHow the tooltip */
	tooltip = att(sha256, user_data, cell);	// issue?

	// gtk_tooltip_set_custom(tooltip, NULL);
	gtk_icon_view_set_tooltip_cell(GTK_ICON_VIEW(user_data), tooltip, tree_path, cell);
	if (sha256 != NULL)
		free(sha256);
	else
		printf("no need to free\n");
	gtk_widget_trigger_tooltip_query(user_data);	// issue?

	/* The tooltip widget must be destroyed once it is no longer needed. */
	//g_object_unref(tooltip);
	return 0;
}

static void pixbuf_loader(GtkListStore *list_store) {
	note_res = PQexec(conn, "SELECT thumb_dir, thumb_siz FROM public.settings;");

	int row_count;		// Number of rows returned by the query
	int row_now;		// Current row
	size_t size = strlen(PQgetvalue(note_res, 0, 0));
	char path[3 + size], file_path[72 + size], *tree_path;
	GdkPixbuf *thumb;

	/* Initial path; [thumb_dir]/[size]/ */
	sprintf(path, "%s/%s/", PQgetvalue(note_res, 0, 0), PQgetvalue(note_res, 0, 1));
	PQclear(note_res); 	// ASC and DESC are reversed, since they are inserted one by one into list_store
	gtk_list_store_clear(list_store);

	note_res = PQexec(conn, "SELECT sha256 FROM public.files ORDER BY created_at DESC;");
	row_count = PQntuples(note_res);	// get the number of rows returned by the query
	ico_tree_path = gtk_tree_path_new_from_string("0");

	if (row_count > 0)
		for (row_now = 0; row_now < row_count; row_now++) {
			tree_path = (char *) malloc((sizeof(int) * 3) + 3);
			sprintf(file_path, "%s%s.png", path, PQgetvalue(note_res, row_now, 0));	// [thumb_dir]/[size]/[sha256]

			thumb = gdk_pixbuf_new_from_file(file_path, NULL);
			gtk_list_store_append(list_store, &ico_tree_iter);
			gtk_list_store_set(list_store, &ico_tree_iter, SHA256_COL, PQgetvalue(note_res, row_now, 0), COL_PIXBUF, thumb, -1);
			sprintf(tree_path, "0:%d", row_now);

			gtk_tree_path_append_index(ico_tree_path, row_now);
			free(tree_path);
		}

	PQclear(note_res);
}

extern GtkWidget *home_page(void) {
	/* TODO:
	 * Get the advanced tooltip working
	 * Short and thin icons have too much padding
	 */

	GtkWidget *icon_view;
	GtkTooltip *tt;
	char *sha256;

	/* Create and fill the list store */
	sha256 = (char *) malloc(65);
	ico_list_store = gtk_list_store_new(2, G_TYPE_STRING, GDK_TYPE_PIXBUF);
	pixbuf_loader(ico_list_store);

	icon_view = gtk_icon_view_new_with_model(GTK_TREE_MODEL(ico_list_store));
	ico_tree_path = gtk_tree_model_get_path(GTK_TREE_MODEL(ico_list_store), &ico_tree_iter);

	CallBackData_set_tree_path(viewer_data, ico_tree_path);
	CallBackData_set_tree_model(viewer_data, GTK_TREE_MODEL(ico_list_store));
	CallBackData_set_list_store(viewer_data, ico_list_store);

	/* Setup containers */
	gtk_icon_view_set_selection_mode(GTK_ICON_VIEW(icon_view), GTK_SELECTION_MULTIPLE);
	gtk_icon_view_set_columns(GTK_ICON_VIEW(icon_view), -1);

	/* Advanced tooltip */
	gtk_widget_set_has_tooltip(icon_view, TRUE);
	//g_signal_connect(GTK_ICON_VIEW(icon_view), "select-cursor-item", G_CALLBACK(iv_select_cb), icon_view);
	g_signal_connect(icon_view, "query-tooltip", G_CALLBACK(query_tooltip), icon_view);

	/* SHow the widgets */
	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(icon_view), COL_PIXBUF);
	return icon_view;
}
