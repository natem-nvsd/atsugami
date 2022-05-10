/***************************************************************\
*	notebook.c (c) 2021-2022, by Nate Morrison		*
*								*
*	notebook.c contains functions for the primary GTK Icon	*
*	View found in Atsugami, and its callbacks.		*
\***************************************************************/

#include "atsugami.h"
#include "types.h"
#include <errno.h>
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdio.h>
#include <stdbool.h>

PGresult *note_res;
GtkTreeIter ico_tree_iter;
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
	** *. Lots of error checking			**
	**						**
	\************************************************/

	GtkTreeModel *tree_model;
	GtkTreePath *tree_path;
	GtkCellRenderer *cell;
	//GdkRectangle *rect;
	//gboolean tooltip_status;
	char *sha256;

	/* Prevent execution with a null pointer */
	if (user_data == NULL) {
	//	dbg_info("notebook.c: user_data is NULL");
		return 1;
	}
	
	/* Prepare the tooltip */
	keyboard_mode = FALSE;
	tree_model = GTK_TREE_MODEL(gtk_icon_view_get_model(user_data));
	tree_path = gtk_icon_view_get_path_at_pos(GTK_ICON_VIEW(widget), x, y);

	/* tree_path is NULL when the user's cursor is not over an icon, and vice versa */
	if (tree_path == NULL) {
	//	dbg_info("notebook.c: path is null.");
		return 1;
	}

	gtk_icon_view_get_item_at_pos(GTK_ICON_VIEW(user_data), x, y, &tree_path, &cell);
	//gtk_icon_view_get_cell_rect(GTK_ICON_VIEW(user_data), tree_path, cell, rect);	// segfault here
	gtk_icon_view_set_tooltip_column(GTK_ICON_VIEW(user_data), 0);

	/* Get the SHA256 checksum */
	sha256 = (char *) malloc(65);
	gtk_tree_model_get(tree_model, &ico_tree_iter, 0, &sha256, -1);
	printf("'%s'\n", sha256);

	/* SHow the tooltip */
	tooltip = att(sha256, user_data, cell);	// issue?
	gtk_icon_view_set_tooltip_cell(GTK_ICON_VIEW(user_data), tooltip, tree_path, cell);

	if (sha256 != NULL) {
		dbg_info("notebook.c: free() (Only)");
		free(sha256);
		dbg_info("notebook.c: freed");
	}
	else {
		dbg_err("notebook.c: sha256 is NULL");
		exit(EADDRNOTAVAIL);
	}

	return 0;
}

extern void pixbuf_loader(GtkListStore *list_store) {
	int row_count, row_now;

	gtk_list_store_clear(list_store);
	if (safe_mode == true)
		note_res = PQexec(conn, "SELECT sha256 FROM public.files WHERE rating = 's' ORDER BY created_at DESC;");
	else
		note_res = PQexec(conn, "SELECT sha256 FROM public.files ORDER BY created_at DESC;");

	row_count = PQntuples(note_res);	// get the number of rows returned by the query
	ico_tree_path = gtk_tree_path_new_from_string("0");
//	printf("row count: %d\n", row_count);

	if (row_count > 0)
		for (row_now = 0; row_now < row_count; row_now++) {
			GdkPixbufLoader *gdk_pixbuf_loader;
			char *path_base = get_thumb_path();
			char *file_path, *tree_path;
			size_t size = strlen(path_base), thumb_data_size = 0;
			FILE *thumb_file;
			long thumb_size;
			unsigned char *thumb_data;

			gdk_pixbuf_loader = gdk_pixbuf_loader_new_with_mime_type("image/png", NULL);
			tree_path = (char *) malloc(sizeof(int) + 2);
			file_path = (char *) malloc(size + 4 + PQgetlength(note_res, row_now, 0));
			sprintf(file_path, "%s%s.png", path_base, PQgetvalue(note_res, row_now, 0));	// [thumb_dir]/[size]/[sha256].png
		//	printf("'%s' ", file_path);

			/* File operations */
			thumb_file = fopen(file_path, "r");
			if (thumb_file == NULL) {
				GtkWidget *err_img;

				printf("NULL\n");
				err_img = gtk_image_new_from_icon_name("image-missing", GTK_ICON_SIZE_DIALOG);

				goto error_point;
			}

			fseek(thumb_file, 0L, SEEK_END);
			thumb_size = ftell(thumb_file);
			rewind(thumb_file);

			thumb_data = (unsigned char *) malloc(thumb_size);
			thumb_data_size = fread(thumb_data, sizeof(char), thumb_size, thumb_file);

			gdk_pixbuf_loader_write(gdk_pixbuf_loader, thumb_data, thumb_data_size, NULL);
			gtk_list_store_append(list_store, &ico_tree_iter);
			gtk_list_store_set(list_store, &ico_tree_iter, SHA256_COL, PQgetvalue(note_res, row_now, 0), COL_PIXBUF, gdk_pixbuf_loader_get_pixbuf(gdk_pixbuf_loader), -1);
			sprintf(tree_path, "0:%d", row_now);
			gtk_tree_path_append_index(ico_tree_path, row_now);

			fclose(thumb_file);
			free(thumb_data);
			free(file_path);
			free(tree_path);
			error_point:
			gdk_pixbuf_loader_close(gdk_pixbuf_loader, NULL);
		}

	PQclear(note_res);
	return;
}

static void viewer_test(GtkWidget *icon_view, GtkTreePath *tree_path, gpointer user_data) {
	GtkListStore *list_store;
	GtkTreeIter tree_iter;
	char *sha256;

	sha256 = (char *) malloc(65);
	list_store = GTK_LIST_STORE(user_data);

	gtk_tree_model_get_iter(GTK_TREE_MODEL(list_store), &tree_iter, tree_path);
	gtk_tree_model_get(GTK_TREE_MODEL(list_store), &tree_iter, 0, &sha256, -1);
	printf("\n\nPath: '%s'\n\n", sha256);
}

extern GtkWidget *home_page(void) {
	GtkListStore *icon_list_store;
	GtkWidget *icon_view;
	int icon_size;

	/* Create and fill the list store */
	icon_size = get_icon_size();
	icon_list_store = gtk_list_store_new(2, G_TYPE_STRING, GDK_TYPE_PIXBUF);
	pixbuf_loader(icon_list_store);

	icon_view = gtk_icon_view_new_with_model(GTK_TREE_MODEL(icon_list_store));
	ico_tree_path = gtk_tree_model_get_path(GTK_TREE_MODEL(icon_list_store), &ico_tree_iter);

	CallBackData_set_tree_path(viewer_data, ico_tree_path);
	CallBackData_set_tree_model(viewer_data, GTK_TREE_MODEL(icon_list_store));
	CallBackData_set_list_store(viewer_data, icon_list_store);

	/* Setup containers */
	gtk_icon_view_set_selection_mode(GTK_ICON_VIEW(icon_view), GTK_SELECTION_MULTIPLE);
	gtk_icon_view_set_columns(GTK_ICON_VIEW(icon_view), -1);

	/* Advanced tooltip */
	gtk_widget_set_has_tooltip(icon_view, TRUE);
	g_signal_connect(icon_view, "query-tooltip", G_CALLBACK(query_tooltip), icon_view);
	g_signal_connect(icon_view, "item-activated", G_CALLBACK(viewer_test), icon_list_store);

	/* Misc. */
	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(icon_view), COL_PIXBUF);
	gtk_icon_view_set_item_width(GTK_ICON_VIEW(icon_view), get_icon_size());

	return icon_view;
}
