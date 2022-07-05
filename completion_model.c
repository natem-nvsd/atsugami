/* completion.c
 * Copyright (c) 2021-2022 by Nate Morrison.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
