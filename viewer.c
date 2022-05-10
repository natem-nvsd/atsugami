/* viewer.c: Built-in image viewer */
#include "atsugami.h"
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>

extern void viewer(GtkWidget *icon_view, GtkTreePath *tree_path, gpointer user_data) {
	char *sha256, *image_path, *image_path_base;
	GtkWidget *scrolled_window, *viewport;
	GtkListStore *list_store;
	GtkTreeIter tree_iter;

	sha256 = (char *) malloc(65);
	image_path_base = get_file_path();
	image_path = (char *) malloc(strlen(image_path_base) + 65);
	list_store = GTK_LIST_STORE(user_data);

	/* Get the file path */
	gtk_tree_model_get_iter(GTK_TREE_MODEL(list_store), &tree_iter, tree_path);
	gtk_tree_model_get(GTK_TREE_MODEL(list_store), &tree_iter, 0, &sha256, -1);
	sprintf(image_path, "%s%s", image_path_base, sha256);
	printf("%s\n\n", image_path);

	/* Display the viewer */
	icon_view = gtk_image_new_from_file(image_path);
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);

	gtk_container_add(GTK_CONTAINER(scrolled_window), icon_view);
	gtk_widget_show_all(icon_view);
}
