#include <gtk/gtk.h>
#include <libpq-fe.h>
#include "main.h"
#include "list_tags.h"

extern void list_tags(void) {
	//GtkLabel *label;
	//GtkBox *vbox;
	GtkWidget *vbox, *label, *tv;
	//GtkTextView *tv;
	int page_count;

	label = gtk_label_new("Tags");
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	tv = gtk_text_view_new();
	page_count = gtk_notebook_get_n_pages(notebook);

	gtk_widget_set_valign(label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);

	//gtk_widget_set_valign();
	gtk_widget_show_all(vbox);
	gtk_container_add(GTK_CONTAINER(notebook), vbox);
	gtk_notebook_set_tab_label_text(notebook, vbox, "Tags");
	gtk_notebook_set_current_page(notebook, page_count);
}
