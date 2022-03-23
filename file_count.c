/* file_count.c */
#include "atsugami.h"
#include <gtk/gtk.h>
#include <libpq-fe.h>

PGresult *count_res;

extern void file_count(void) {
	char label_str[25];	// use malloc later
	int count;
	
	count_res = PQexec(conn, "SELECT count(*) FROM public.files;");
	sprintf(label_str, "%s files", PQgetvalue(count_res, 0, 0));
	count = (int) strtol(PQgetvalue(count_res, 0, 0), (char **) NULL, 10);

	switch (count) {
		case 0: {
			file_label = gtk_label_new("No files found.");
			break;
		}

		case 1: {
			file_label = gtk_label_new("1 file");
			break;
		}
		default: {
			file_label = gtk_label_new(label_str);
			break;
		 }
	}

	gtk_widget_set_halign(file_label, GTK_ALIGN_START);
	gtk_widget_set_valign(file_label, GTK_ALIGN_END);
	gtk_box_pack_start(GTK_BOX(vbox), file_label, FALSE, FALSE, 2);
	gtk_widget_set_margin_start(file_label, 4);
	PQclear(count_res);
}

extern void file_count_update(void) {
	char label_str[25];	// use malloc later
	int count;
	
	count_res = PQexec(conn, "SELECT count(*) FROM public.files;");
	sprintf(label_str, "%s files", PQgetvalue(count_res, 0, 0));
	count = (int) strtol(PQgetvalue(count_res, 0, 0), (char **) NULL, 10);

	PQclear(count_res);
	switch (count) {
		case 0: {
			gtk_label_set_text(GTK_LABEL(file_label), "No files found.");
			break;
		}

		case 1: {
			gtk_label_set_text(GTK_LABEL(file_label), "1 file");
			break;
		}

		default: {
			gtk_label_set_text(GTK_LABEL(file_label), label_str);
			break;
		}
	}
}
