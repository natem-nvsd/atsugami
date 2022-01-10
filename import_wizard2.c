#include <gtk/gtk.h>
#include "import.h"
#include <libpq-fe.h>
#include "main.h"
#include "notebook.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "import_wizard.h"

PGresult *wiz_res;
GtkWidget *label0, *label1, *label2;
GtkEntry *entry0, *entry1, *entry2;
GtkTextView *tv;
GtkWidget *import_thumb, *cbox, *can_button, *imp_button;
GtkButtonBox *button_box;
GdkPixbuf *import_thumb_pixbuf;

const gchar *text0;
const gchar *text1;
const gchar *text2;
const gchar *text3;
const gchar *text4;
const gchar *text5;
const gchar *text6;
const gchar *text7;
const gchar *uuid = NULL;

gint page_count = 0;
char query_string[20480];       /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
char child_uuids[2048];         /* THESE MUST BE FIXED TO PREVENT BUFFER OVERFLOWS */
gboolean parent_bool = FALSE;
gboolean child_bool = FALSE;
gboolean has_children = FALSE;
GtkWidget *header_box, *scrolled_window;

static void cancel_button_cb(void) {
	gtk_notebook_detach_tab(notebook, scrolled_window);
}

static void import_button_cb(void) {
	gtk_notebook_detach_tab(notebook, scrolled_window);
}

extern void import_wizard(GtkWidget *import_page, gpointer user_data) {
	import_page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	page_count = gtk_notebook_get_n_pages(notebook);
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);

	gtk_container_set_border_width(GTK_CONTAINER(import_page), 10);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), import_page);
	gtk_scrolled_window_set_kinetic_scrolling(GTK_SCROLLED_WINDOW(scrolled_window), TRUE);

	/* thumbnail */
	import_thumb_pixbuf = gdk_pixbuf_new_from_file_at_scale(import_file_path, 720, 720, TRUE, NULL);
	import_thumb = gtk_image_new_from_pixbuf(import_thumb_pixbuf);
	
	gtk_box_pack_start(GTK_BOX(import_page), import_thumb, FALSE, FALSE, 0);

	/* Source */
	label1 = gtk_label_new("Enter source URL here:");
	gtk_widget_set_halign(label1, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), label1, FALSE, FALSE, 0);
	
	entry0 = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry0), TRUE);
	gtk_widget_set_valign(entry0, GTK_ALIGN_START);
	gtk_entry_set_placeholder_text(entry0, "Enter the URL to upload here");
	gtk_box_pack_start(GTK_BOX(import_page), entry0, TRUE, TRUE, 0);

	/* Tag box */
	label0 = gtk_label_new("Tags");
	gtk_widget_set_halign(label0, GTK_ALIGN_START);
	gtk_text_view_set_editable(tv, TRUE);
	gtk_box_pack_start(GTK_BOX(import_page), label0, FALSE, FALSE, 0);

	tv = gtk_text_view_new();
	gtk_widget_set_valign(GTK_TEXT_VIEW(tv), GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), tv, TRUE, TRUE, 0);
	
	/* Rating */
	label1 = gtk_label_new("Rating:");
	gtk_widget_set_halign(label1, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), label1, FALSE, FALSE, 0);
	
	cbox = gtk_combo_box_text_new();
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(cbox), "s", "Safe");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(cbox), "q", "Questionable");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(cbox), "e", "Explicit");
	gtk_box_pack_start(GTK_BOX(import_page), cbox, FALSE, FALSE, 0);

	/* Invisible entry field */
	entry2 = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry2), TRUE);
	g_object_bind_property(cbox, "active-id", entry2, "text", G_BINDING_DEFAULT);

	button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_pack_start(GTK_BOX(import_page), GTK_BUTTON_BOX(button_box), FALSE, FALSE, 0);
	
	can_button = gtk_button_new_with_label("Cancel");
	imp_button = gtk_button_new_with_label("Import");
	
	gtk_button_box_set_layout(button_box, GTK_BUTTONBOX_START);
	gtk_box_pack_start(GTK_BUTTON_BOX(button_box), can_button, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BUTTON_BOX(button_box), imp_button, FALSE, FALSE, 0);
	
	g_signal_connect(can_button, "clicked", G_CALLBACK(cancel_button_cb), NULL);
	g_signal_connect(imp_button, "clicked", G_CALLBACK(import_button_cb), NULL);
	
	/* init stuff */
	gtk_widget_show_all(import_page);
	gtk_widget_show_all(scrolled_window);
	gtk_container_add(GTK_CONTAINER(notebook), scrolled_window);
	gtk_scrolled_window_set_propagate_natural_height(GTK_SCROLLED_WINDOW(scrolled_window), TRUE);
	gtk_notebook_set_tab_label_text(notebook, scrolled_window, "Import");
	gtk_notebook_set_current_page(notebook, page_count);
}
