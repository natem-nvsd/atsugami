/* search.c */
#include "atsugami.h"
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdio.h>

PGresult *search_res;
GdkPixbuf *thumb;
GtkTreeIter *iter;

extern GtkListStore *search_cb(GtkWidget *parent, char *keyword_list) {
	GtkListStore *list_store;
	int iter;
	long wc;

	wc = word_count(keyword_list);

	char *query_base[3], *keywords[wc], *query, *raw_str;

	iter = 0;
	raw_str = search_input_sanitiser(keyword_list);

	query_base[0] = "SELECT * FROM (WITH files(sha256, id, created_at) AS (SELECT * FROM (SELECT DISTINCT ON (files.id) files.sha256, files.id, files.created_at, tags.tag_id FROM public.files AS \"files\" INNER JOIN public.files_tags AS \"tags\" ON files.id = tags.file_id WHERE tags.tag_id = (SELECT id FROM public.tags WHERE name = '%s') ORDER BY id ASC) AS \"keyword0\"";
	query_base[1] = "UNION (SELECT * FROM (SELECT DISTINCT ON (files.id) files.sha256, files.id, files.created_at, tags.tag_id FROM public.files AS \"files\" INNER JOIN public.files_tags AS \"tags\" ON files.id = tags.file_id WHERE tags.tag_id = (SELECT id FROM public.tags WHERE name = '%s') ORDER BY id ASC) AS \"keyword1\")";
	query_base[2] = ") SELECT DISTINCT ON (files.id) * FROM files ORDER BY (files.id) ASC) AS results ORDER BY created_at ASC;";

	list_store = gtk_list_store_new(2, G_TYPE_STRING, GDK_TYPE_PIXBUF, G_TYPE_INT);
	pixbuf_loader(list_store);

	return list_store;
}
