#include "atsugami.h"
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <libpq-fe.h>

extern GtkTreeModel *completion_model_generator(const int category) {
	/* -1:	Tags from all categories; returns multiple columns.
	 *  0:	All artist tags.
	 *  1:	All copyright tags.
	 *  2:	All character tags.
	 *  3:	All general tags.
	 *  4:	All meta tags. */
	GtkListStore *list_store;
	GtkTreeIter entry_completion_iter;
	PGresult *completion_res;
	int iter;

	list_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);

	switch (category) {
		// TODO: 0-4 can be done with default: { ... }
		case -1: {
			completion_res = PQexec(conn, "SELECT categories.category_id, tags.name FROM public.tags AS tags INNER JOIN public.tags_categories AS categories ON categories.tag_id = tags.id ORDER BY tags.name ASC;");
			for (iter = 0; iter < PQntuples(completion_res); iter++) {
				gtk_list_store_append(list_store, &entry_completion_iter);
				gtk_list_store_set(list_store, &entry_completion_iter, COMPLETION_MODEL_NAME_COLUMN, PQgetvalue(completion_res, iter, 1), COMPLETION_MODEL_CATEGORY_COLUMN, PQgetvalue(completion_res, iter, 0), -1);
			}

			PQclear(completion_res);
			break;
		}

		default: {
			char *query;

			query = (char *) malloc(229);
			sprintf(query, "SELECT categories.id, tags.name FROM (SELECT id, name FROM public.tags) AS tags INNER JOIN public.tags_categories AS tags_categories ON tags_categories.tag_id = tags.id AND tags_categories.category_id = %d ORDER BY tags.name ASC;", category);

			completion_res = PQexec(conn, query);
			for (iter = 0; iter < PQntuples(completion_res); iter++) {
				gtk_list_store_append(list_store, &entry_completion_iter);
				gtk_list_store_set(list_store, &entry_completion_iter, COMPLETION_MODEL_NAME_COLUMN, PQgetvalue(completion_res, iter, 1), COMPLETION_MODEL_CATEGORY_COLUMN, PQgetvalue(completion_res, iter, 0), -1);
			}

			PQclear(completion_res);
			free(query);
			break;
		}
	}

	return GTK_TREE_MODEL(list_store);
}

extern void entry_completion_set_model(GtkEntryCompletion *entry_completion, GtkTreeModel *entry_completion_model) {
	gtk_entry_completion_set_model(entry_completion, entry_completion_model);
	gtk_entry_completion_set_popup_completion(entry_completion, TRUE);
	gtk_entry_completion_set_popup_single_match(entry_completion, TRUE);
	gtk_entry_completion_set_text_column(entry_completion, 0);
	gtk_entry_completion_set_inline_completion(entry_completion, TRUE);
	gtk_entry_completion_set_inline_selection(entry_completion, TRUE);
	//gtk_entry_completion_set_popup_single_match(entry_completion, TRUE);
	gtk_entry_completion_set_popup_single_match(entry_completion, FALSE);
}
