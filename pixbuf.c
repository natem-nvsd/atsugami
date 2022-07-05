/* pixbuf.c
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
#include <errno.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdio.h>
#include <stdbool.h>

extern void pixbuf_loader(GtkListStore *list_store) {
	// A GtkFlowBox could be used instead of a GtkIconView.
	int row_count, row_now;
	GtkTreeIter tree_iter;
	PGresult *note_res;

	gtk_list_store_clear(list_store);
	if (safe_mode == true)
		note_res = PQexec(conn, "SELECT sha256 FROM public.files WHERE rating = 's' ORDER BY created_at DESC;");
	else
		note_res = PQexec(conn, "SELECT sha256 FROM public.files ORDER BY created_at DESC;");

	row_count = PQntuples(note_res);	// get the number of rows returned by the query

	if (row_count > 0)
		for (row_now = 0; row_now < row_count; row_now++) {
			GdkPixbufLoader *gdk_pixbuf_loader;
			char *path_base = get_thumb_path();
			char *file_path, *tree_path_str;
			size_t size = strlen(path_base), thumb_data_size = 0;
			FILE *thumb_file;
			long thumb_size;
			unsigned char *thumb_data;

			tree_path_str = (char *) malloc(sizeof(int) + 2);
			sprintf(tree_path_str, "0:%d", row_now);
			GtkTreePath *tree_path = gtk_tree_path_new_from_string(tree_path_str);

			gdk_pixbuf_loader = gdk_pixbuf_loader_new_with_mime_type("image/png", NULL);
			file_path = (char *) malloc(size + 4 + PQgetlength(note_res, row_now, 0));

			sprintf(file_path, "%s%s.png", path_base, PQgetvalue(note_res, row_now, 0));	// [thumb_dir]/[size]/[sha256].png
			printf("path_base: '%s'\n", path_base);

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

			/* Tree stuff */
			gdk_pixbuf_loader_write(gdk_pixbuf_loader, thumb_data, thumb_data_size, NULL);
			gtk_list_store_append(list_store, &tree_iter);
			gtk_list_store_set(list_store, &tree_iter, SHA256_COL, PQgetvalue(note_res, row_now, 0), COL_PIXBUF, gdk_pixbuf_loader_get_pixbuf(gdk_pixbuf_loader), -1);
			gtk_tree_path_append_index(tree_path, row_now);

			free(thumb_data);
			fclose(thumb_file);
			free(file_path);
			free(tree_path_str);
			error_point:
			gdk_pixbuf_loader_close(gdk_pixbuf_loader, NULL);
			gtk_tree_path_free(tree_path);
		}

	PQclear(note_res);
	return;
}
