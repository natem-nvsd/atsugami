/* file_count.c */
#include "atsugami.h"
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdbool.h>

extern void file_count(void) {
	char *label_str = NULL;	// use malloc later
	PGresult *count_res;
	size_t count;

	if (safe_mode == TRUE) {
		size_t safe_count, total;

		count_res = PQexec(conn, "SELECT count(*) FROM public.files;");
		total = atol(PQgetvalue(count_res, 0, 0));
		PQclear(count_res);

		label_str = (char *) malloc(35 + (sizeof(unsigned long) * 3));
		count_res = PQexec(conn, "SELECT count(*) FROM public.files WHERE rating = 's';");
		safe_count = atol(PQgetvalue(count_res, 0, 0));
		sprintf(label_str, "%lu of %lu images (%lu hidden by safe mode)", safe_count, total, (total - safe_count));

		switch (safe_count) {
			case 0: {
				msg_id = gtk_statusbar_push(GTK_STATUSBAR(status_bar), 0, label_str);
				break;
			}

			case 1: {
				msg_id = gtk_statusbar_push(GTK_STATUSBAR(status_bar), 0, "1 image");
				break;
			}
			default: {
				msg_id = gtk_statusbar_push(GTK_STATUSBAR(status_bar), 0, label_str);
				break;
			 }
		}

		PQclear(count_res);
		free(label_str);
	} else {
		label_str = (char *) malloc(16 + sizeof(int));
		count_res = PQexec(conn, "SELECT count(*) FROM public.files;");
		sprintf(label_str, "%s files", PQgetvalue(count_res, 0, 0));
		count = atol(PQgetvalue(count_res, 0, 0));

		switch (count) {
			case 0: {
				msg_id = gtk_statusbar_push(GTK_STATUSBAR(status_bar), 0, "No images found");
				break;
			}

			case 1: {
				msg_id = gtk_statusbar_push(GTK_STATUSBAR(status_bar), 0, "1 image");
				break;
			}
			default: {
				msg_id = gtk_statusbar_push(GTK_STATUSBAR(status_bar), 0, label_str);
				break;
			 }
		}

		PQclear(count_res);
	}

}

extern void file_count_update(void) {
	char label_str[25];	// use malloc later
	unsigned long count;	// unsigned long is used to gurantee compatibility with Postgres' BIGINT.
	PGresult *count_res;
	
	count_res = PQexec(conn, "SELECT COUNT(*) FROM public.files;");
	count = strtoll(PQgetvalue(count_res, 0, 0), (char **) NULL, 10);

	switch (count) {
		case 0: {
			gtk_statusbar_remove(GTK_STATUSBAR(status_bar), 0, msg_id);
			msg_id = gtk_statusbar_push(GTK_STATUSBAR(status_bar), 0, "No files found.");
			break;
		}

		case 1: {
			gtk_statusbar_remove(GTK_STATUSBAR(status_bar), 0, msg_id);
			msg_id = gtk_statusbar_push(GTK_STATUSBAR(status_bar), 0, "1 file");
			break;
		}

		default: {
			sprintf(label_str, "%s files", PQgetvalue(count_res, 0, 0));
			gtk_statusbar_remove(GTK_STATUSBAR(status_bar), 0, msg_id);
			msg_id = gtk_statusbar_push(GTK_STATUSBAR(status_bar), 0, label_str);
			break;
		}
	}

	PQclear(count_res);
}
