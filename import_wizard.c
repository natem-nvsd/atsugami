/***************************************************************\
* wizard.c rewritten and redesigned to work in a notebook tab.	*
*	(c) 2022, Nate Morrison					*
\***************************************************************/

#include <gtk/gtk.h>
#include "import.h"
#include <libpq-fe.h>
#include "main.h"
#include "notebook.h"
#include <stdio.h>
#include "import_wizard.h"

PGresult *wiz_res;
GtkWidget *label0, *label1, *label2, *label3, *label4, *label5, *label6;
GtkWidget *entry0, *entry1, *entry2, *entry3, *entry4, *entry5, *entry6, *rating_entry;
GtkWidget *import_thumb, *cbox, *button_box, *can_button, *imp_button;
GdkPixbuf *import_thumb_pixbuf;

static GtkWidget *assistant = NULL;
const gchar *text0;
const gchar *text1;
const gchar *text2;
const gchar *text3;
const gchar *text4;
const gchar *text5;
const gchar *text6;
const gchar *text7;

float workarea_width;
float workarea_height;
float width;
float height;
gint page_count = 0;
char char_width[6]; /* only corrupt, damaged, or malicisious images can over flow these two buffers; fix them */
char char_height[6];
char query_string[20480];       /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
char child_uuids[2048];         /* THESE MUST BE FIXED TO PREVENT BUFFER OVERFLOWS */
gboolean parent_bool = FALSE;
gboolean child_bool = FALSE;
gboolean has_children = FALSE;

static void on_assistant_apply() {
}

extern void import_wizard(GtkWidget *import_page, gpointer user_data) {
	import_page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	page_count = gtk_notebook_get_n_pages(notebook);

	gtk_container_set_border_width(GTK_CONTAINER(import_page), 10);

	/* First label and text field */
	label0 = gtk_label_new("Enter the artist\'s name here:");
	gtk_widget_set_halign(label0, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), label0, FALSE, FALSE, 0);

	entry0 = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry0), TRUE);
	gtk_widget_set_valign(entry0, GTK_ALIGN_START);
	gtk_entry_set_placeholder_text(entry0, "Values are separated by commas; spaces are optional.");
	gtk_box_pack_start(GTK_BOX(import_page), entry0, TRUE, TRUE, 0);
	//g_signal_connect(G_OBJECT(entry0), "changed", G_CALLBACK(on_wizard_entry_changed), assistant);

	//gtk_widget_set_halign(revealer_label, GTK_ALIGN_START);
	//gtk_box_pack_start(GTK_BOX(import_page), revealer_label, FALSE, FALSE, 0);

	/* Second label text field */
	label1 = gtk_label_new("Enter copyrights here:");
	gtk_widget_set_halign(label1, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), label1, FALSE, FALSE, 0);

	entry1 = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry1), TRUE);
	gtk_widget_set_valign(entry1, GTK_ALIGN_START);
	gtk_entry_set_placeholder_text(entry1, "Values are separated by commas; spaces are optional.");
	gtk_box_pack_start(GTK_BOX(import_page), entry1, TRUE, TRUE, 0);
	//g_signal_connect(G_OBJECT(entry1), "changed", G_CALLBACK(on_wizard_entry_changed), assistant);

	//revealer1 = gtk_revealer_new();
	//gtk_widget_set_halign(revealer_label, GTK_ALIGN_START);
	//gtk_box_pack_start(GTK_BOX(import_page), revealer_label, FALSE, FALSE, 0);

	/* Third label and text field */
	label2 = gtk_label_new("Enter character names here:");
	gtk_widget_set_halign(label2, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), label2, FALSE, FALSE, 0);

	entry2 = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry2), TRUE);
	gtk_widget_set_valign(entry2, GTK_ALIGN_START);
	gtk_entry_set_placeholder_text(entry2, "Values are separated by commas; spaces are optional.");
	gtk_box_pack_start(GTK_BOX(import_page), entry2, TRUE, TRUE, 0);
	//g_signal_connect(G_OBJECT(entry2), "changed", G_CALLBACK(on_wizard_entry_changed), assistant);

	//revealer2 = gtk_revealer_new();
	//gtk_widget_set_halign(revealer_label, GTK_ALIGN_START);
	//gtk_box_pack_start(GTK_BOX(import_page), revealer_label, FALSE, FALSE, 0);

	/* Fourth label and text field */
	label3 = gtk_label_new("Enter tags here:");
	gtk_widget_set_halign(label3, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), label3, FALSE, FALSE, 0);
	
	entry3 = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry3), TRUE);
	gtk_widget_set_valign(entry3, GTK_ALIGN_START);
	gtk_entry_set_placeholder_text(entry3, "Values are separated by commas; spaces are optional.");
	gtk_box_pack_start(GTK_BOX(import_page), entry3, TRUE, TRUE, 0);
	//g_signal_connect(G_OBJECT(entry3), "changed", G_CALLBACK(on_wizard_entry_changed), assistant);
	
	//revealer3 = gtk_revealer_new();
	//gtk_widget_set_halign(revealer_label, GTK_ALIGN_START);
	//gtk_box_pack_start(GTK_BOX(import_page), revealer_label, FALSE, FALSE, 0);

	/* Fifth label and text field */
	label4 = gtk_label_new("Enter source URL here:");
	gtk_widget_set_halign(label4, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), label4, FALSE, FALSE, 0);
	
	entry4 = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry4), TRUE);
	gtk_widget_set_valign(entry4, GTK_ALIGN_START);
	gtk_entry_set_placeholder_text(entry4, "Values are separated by commas; spaces are optional.");
	gtk_box_pack_start(GTK_BOX(import_page), entry4, TRUE, TRUE, 0);
	//g_signal_connect(G_OBJECT(entry4), "changed", G_CALLBACK(on_wizard_entry_changed), assistant);
	
	//revealer4 = gtk_revealer_new();
	//gtk_widget_set_halign(revealer_label, GTK_ALIGN_START);
	//gtk_box_pack_start(GTK_BOX(import_page), revealer_label, FALSE, FALSE, 0);

	/*Sixth label and text field */
	label5 = gtk_label_new("Rating:");
	gtk_widget_set_halign(label5, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(import_page), label5, FALSE, FALSE, 0);
	
	cbox = gtk_combo_box_text_new();
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(cbox), "safe", "Safe");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(cbox), "questionable", "Questionable");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(cbox), "explicit", "Explicit");
	gtk_box_pack_start(GTK_BOX(import_page), cbox, FALSE, FALSE, 0);
	
	//revealer5 = gtk_revealer_new();
	//gtk_widget_set_halign(revealer_label, GTK_ALIGN_START);
	//gtk_box_pack_start(GTK_BOX(import_page), revealer_label, FALSE, FALSE, 0);

	/* Invisible entry field */
	rating_entry = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(rating_entry), TRUE);
	g_object_bind_property(cbox, "active-id", rating_entry, "text", G_BINDING_DEFAULT);
	//g_signal_connect(G_OBJECT(rating_entry), "changed", G_CALLBACK(on_wizard_entry_changed), assistant);

	/* "Cancel" and "Import" buttons */
	button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_pack_start(GTK_BOX(import_page), button_box, FALSE, FALSE, 0);

	can_button = gtk_button_new_with_label("Cancel");
	imp_button = gtk_button_new_with_label("Import");

	gtk_button_box_set_layout(button_box, GTK_BUTTONBOX_START);
	gtk_box_pack_start(GTK_BOX(button_box), can_button, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(button_box), imp_button, FALSE, FALSE, 0);

	/* init stuff */
	gtk_widget_show_all(import_page);
	gtk_container_add(GTK_CONTAINER(notebook), import_page);
	gtk_notebook_set_tab_label_text(notebook, import_page, "Import");
	gtk_notebook_set_current_page(notebook, page_count);
}
