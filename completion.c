/* completion.c: Global tag completion model */
#include "atsugami.h"
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdio.h>

enum {
	NAME_COLUMN,	// Tag name	(char *)
	ID_COLUMN,	// Tag id	(int)
	CAT_COLUMN,	// Tag category	(int)
	N_COLUMNS
};

GtkTreeModel *completion_model;
GtkListStore *completion_list;

extern int completion_bootstrap(void) {
	PGresult *completion_res;
	GtkTreeIter completion_iter;
	int iter;

	completion_model = (GtkTreeModel *) gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT);
	completion_res = PQexec(conn, "SELECT categories.category_id, tags.id, tags.name FROM public.tags AS tags INNER JOIN public.tags_categories AS categories ON categories.tag_id = tags.id ORDER BY tags.id ASC;");

	/* Fill the list store */
	for (iter = 0; iter < PQntuples(completion_res); iter++) {
		gtk_list_store_append(GTK_LIST_STORE(completion_model), &completion_iter);
		gtk_list_store_set(GTK_LIST_STORE(completion_model), &completion_iter, NAME_COLUMN, PQgetvalue(completion_res, iter, 2), ID_COLUMN, PQgetvalue(completion_res, iter, 1), CAT_COLUMN, PQgetvalue(completion_res, iter, 0), -1);
	}

	/* Error check here */

	return 0;
}
