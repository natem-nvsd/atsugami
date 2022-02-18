#include "main.h"
#include "file_count.h"
#include <gtk/gtk.h>
#include <libpq-fe.h>

PGresult *count_res;

extern void file_count(GtkWidget *label, GtkWidget *box) {
	char label_str[25];	// use malloc later
	
	count_res = PQexec(conn, "SELECT count(*) FROM public.files;");

	sprintf(label_str, "%s files", PQgetvalue(count_res, 0, 0));

	 if (strcmp(PQgetvalue(count_res, 0, 0), "1") == 0)
		label = gtk_label_new("1 file");
	else
		label = gtk_label_new(label_str);
	gtk_widget_set_halign(label, GTK_ALIGN_START);
	gtk_widget_set_valign(label, GTK_ALIGN_END);
	gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);
	PQclear(count_res);
}

extern void file_count_update(GtkWidget *label, GtkWidget *box) {
	char label_str[25];	// use malloc later
	
	count_res = PQexec(conn, "SELECT count(*) FROM public.files;");

	sprintf(label_str, "%s files", PQgetvalue(count_res, 0, 0));
	if (strcmp(PQgetvalue(count_res, 0, 0), "1") == 0)
		gtk_label_set_text(label, "1 file");
	else
		gtk_label_set_text(label, label_str);
}
