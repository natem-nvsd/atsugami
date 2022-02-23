/* file_count.c */
#include "main.h"
#include "callbacks.h"
#include <gtk/gtk.h>
#include <libpq-fe.h>

PGresult *count_res;

extern void file_count(GtkWidget *label, GtkWidget *box) {
	char label_str[25];	// use malloc later
	int count;
	
	count_res = PQexec(conn, "SELECT count(*) FROM public.files;");

	sprintf(label_str, "%s files", PQgetvalue(count_res, 0, 0));

	count = (int) strtol(PQgetvalue(count_res, 0, 0), (char **) NULL, 10);

	switch (count) {
		case 0:
			label = gtk_label_new("No files found.");
			break;
		case 1:
			label = gtk_label_new("1 file");
			break;
		default:
			label = gtk_label_new(label_str);
			break;
	}

	gtk_widget_set_halign(label, GTK_ALIGN_START);
	gtk_widget_set_valign(label, GTK_ALIGN_END);
	gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);
	PQclear(count_res);
}

extern void file_count_update(GtkWidget *label, GtkWidget *box) {
	char label_str[25];	// use malloc later
	int count;
	
	count_res = PQexec(conn, "SELECT count(*) FROM public.files;");

	sprintf(label_str, "%s files", PQgetvalue(count_res, 0, 0));
	//sprintf(count, "%d", PQgetvalue(count_res, 0, 0));

	count = (int) strtol(PQgetvalue(count_res, 0, 0), (char **) NULL, 10);

	switch (count) {
		case 0:
			label = gtk_label_new("No files found.");
			break;
		case 1:
			label = gtk_label_new("1 file");
			break;
		default:
			label = gtk_label_new(label_str);
			break;
	}
}
