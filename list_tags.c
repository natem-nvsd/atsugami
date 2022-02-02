#include <gtk/gtk.h>
#include <libpq-fe.h>
#include "main.h"
#include "list_tags.h"

PGresult *list_res;

extern void list_tags(void) {
	GtkWidget *tags_vbox, *label, *tv;
	//GtkTextView *tv;
	int page_count;

	label = gtk_label_new("Tags");
	tags_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	tv = gtk_text_view_new();
	page_count = gtk_notebook_get_n_pages(notebook);

	gtk_widget_set_valign(label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(tags_vbox), label, TRUE, TRUE, 0);

	//gtk_widget_set_valign();
	gtk_widget_show_all(tags_vbox);
	gtk_container_add(GTK_CONTAINER(notebook), tags_vbox);
	gtk_notebook_set_tab_label_text(notebook, tags_vbox, "Tags");
	gtk_notebook_set_current_page(notebook, page_count);
}
