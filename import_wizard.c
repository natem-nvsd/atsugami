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
GtkWidget *header_box, *scrolled_window;

static void cancel_button_cb(void) {
	gtk_notebook_detach_tab(notebook, scrolled_window);
}

static void import_button_cb(GtkWidget *widget, gpointer user_data) {
	/* Create the query to submit to PostgreSQL */
	strcpy(query_string, "INSERT INTO public.files (path, artist, copyrights, characters, tags, source, rating, width, height, is_parent, is_child, has_children, parent_uuid, child_uuids, imported_at) VALUES ('");

	/* add the file path to the query */
	strcat(query_string, import_file_path);
	strcat(query_string, "', '{");

	/* Entry fields from page0 */
	text0 = gtk_entry_get_text(GTK_ENTRY(entry0));  /* Artist */
	if (gtk_entry_get_text_length(entry0) == 0)
		strcat(query_string, "NULL}', '{");

	if (gtk_entry_get_text_length(entry0) > 0) {
		strcat(query_string, text0);
		strcat(query_string, "}', '{");
	}

	char artist_query_base[] = "INSERT INTO public.artists (name) VALUES ('";
	gint artist_text_size;
	gint artist_query_base_size;
	int artist_query_size;
	
	artist_text_size = sizeof(text0);
	artist_query_base_size = 44;
	artist_query_size = (artist_query_base_size + text0);
	
	char artist_query[artist_query_size];
	strcpy(artist_query, artist_query_base);
	strcat(artist_query, text0);
	strcat(artist_query, "') ON CONFLICT DO NOTHING;");
	
	printf("artist_query_size = %d\n", artist_query_size);
	printf("%s\n", artist_query);
	PQexec(conn, artist_query);
	strcpy(artist_query, "");	/* Clean the string */

	/* Entry fields from page1 */                                                                                                                      
	text1 = gtk_entry_get_text(GTK_ENTRY(entry1));  /* Copyrights */
	if (gtk_entry_get_text_length(entry1) == 0)
		strcat(query_string, "NULL}', '{");
	
	if (gtk_entry_get_text_length(entry1) > 0) {
		strcat(query_string, text1);
		strcat(query_string, "}', '{");
	}

	char copyright_query_base[] = "INSERT INTO public.copyrights (name) VALUES ('";
	gint copyright_text_size;
	gint copyright_query_base_size;
	int copyright_query_size;
	
	copyright_text_size = sizeof(text1);
	copyright_query_base_size = 47;
	copyright_query_size = (copyright_query_base_size + text1);
	
	char copyright_query[copyright_query_size];
	strcpy(copyright_query, copyright_query_base);
	strcat(copyright_query, text1);
	strcat(copyright_query, "') ON CONFLICT DO NOTHING;");	/* i should use a macro for this */

	/* Entry fields from page2 */
	text2 = gtk_entry_get_text(GTK_ENTRY(entry2));	/* Characters */
	if (gtk_entry_get_text_length(entry2) == 0)
		strcat(query_string, "NULL}', '{");
	
	if (gtk_entry_get_text_length(entry2) > 0) {
		strcat(query_string, text2);
		strcat(query_string, "}', '{");
	}

	char characters_query_base[] = "INSERT INTO public.characters (name) VALUES ('";
	gint characters_text_size;
	gint characters_query_base_size;
	int characters_query_size;
	
	characters_text_size = sizeof(text1);
	characters_query_base_size = 47;
	characters_query_size = (characters_query_base_size + text1);
	
	char characters_query[characters_query_size];
	strcpy(characters_query, characters_query_base);
	strcat(characters_query, text2);
	strcat(characters_query, "') ON CONFLICT DO NOTHING;");
		/* This won't work for multiple characters */

	/* Tags */
	text3 = gtk_entry_get_text(GTK_ENTRY(entry3));
	if (gtk_entry_get_text_length(entry3) == 0)
		strcat(query_string, "NULL}', '");
	
	if (gtk_entry_get_text_length(entry3) > 0) {
		strcat(query_string, text3);
		strcat(query_string, "}', '");
	}

	/* Source */
	text4 = gtk_entry_get_text(GTK_ENTRY(entry4));
	if (gtk_entry_get_text_length(entry4) == 0)
		strcat(query_string, "NULL}', '");
	
	if (gtk_entry_get_text_length(entry4) > 0) {
		strcat(query_string, text4);
		strcat(query_string, "', '");
	}

	/* Rating */
	text7 = gtk_entry_get_text(GTK_ENTRY(rating_entry));
	printf("text7: %s\n", text7);
	strcat(query_string, text7);
	strcat(query_string, "', ");
	
	/* add the image resolution to the query */
	strcat(query_string, char_width); /* Width */
	strcat(query_string, ", ");
	
	strcat(query_string, char_height); /* Height */
	strcat(query_string, ", ");
	
	/* add the values of is_parent */
	if (parent_bool == TRUE)
		strcat(query_string, "TRUE, ");
	else
		strcat(query_string, "FALSE, ");
	
	/* add the values of  is_child */
	if (child_bool == TRUE)
		strcat(query_string, "TRUE, ");
	else
		strcat(query_string, "FALSE, ");
	
	/* Has children */
	if (has_children == TRUE)
		strcat(query_string, "TRUE, ");
	else
		strcat(query_string, "FALSE, ");
	
	/* Parent UUID */
	text5 = gtk_entry_get_text(GTK_ENTRY(entry5));
	if (gtk_entry_get_text_length(entry5) == 0)
	
	if (gtk_entry_get_text_length(entry5) > 0) {
		strcat(query_string, text5);
		strcat(query_string, ", '{");
	}

	/* Child UUIDs */
	text6 = gtk_entry_get_text(GTK_ENTRY(entry6));
	if (gtk_entry_get_text_length(entry6) == 0)
		strcat(query_string, "NULL}', now());");
	
	if (gtk_entry_get_text_length(entry6) > 0) {
		strcat(query_string, text6);
		strcat(query_string, "}', now()) ON CONFLICT DO NOTHING;");
	}

	PQexec(conn, query_string);
	PQclear(wiz_res);

	char get_uuid_from_path[2048];
	char new_file_path[2048];
	char uuid[38];
	char new_file_query[2048];

	/* Get the uuid of the file just imported */
	strcpy(uuid, PQgetvalue(wiz_res, 0, 0));
	strcpy(get_uuid_from_path, "SELECT uuid FROM public.files WHERE path = '");
	strcat(get_uuid_from_path, import_file_path);
	strcat(get_uuid_from_path, "';");
	PQexec(conn, get_uuid_from_path);

	/* Move the file to a the storage directory */
	strcpy(new_file_path, STORDIR);
	strcat(new_file_path, "/files/");
	strcat(new_file_path, uuid);
	//system("mv %s %s", import_file_path, new_file_path);

	/* Update the file's entry in Postgres */

	strcpy(import_file_path, ""); /* Clear import_file_path. */
}

extern void import_wizard(GtkWidget *import_page, gpointer user_data) {
	import_page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	page_count = gtk_notebook_get_n_pages(notebook);
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);

	gtk_container_set_border_width(GTK_CONTAINER(import_page), 10);
	//gtk_box_pack_start(GTK_BOX(scrolled_window), import_page, FALSE, FALSE, 0);
	gtk_scrolled_window_add_with_viewport(scrolled_window, import_page);
	gtk_scrolled_window_set_kinetic_scrolling(scrolled_window, TRUE);

	/* thumbnail */
	import_thumb_pixbuf = gdk_pixbuf_new_from_file_at_scale(import_file_path, 720, 720, TRUE, NULL);
	import_thumb = gtk_image_new_from_pixbuf(import_thumb_pixbuf);

	gtk_box_pack_start(GTK_BOX(import_page), import_thumb, FALSE, FALSE, 0);

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

	g_signal_connect(can_button, "clicked", G_CALLBACK(cancel_button_cb), NULL);
	g_signal_connect(can_button, "clicked", G_CALLBACK(import_button_cb), NULL);

	/* init stuff */
	gtk_widget_show_all(import_page);
	gtk_widget_show_all(scrolled_window);
	//gtk_container_add(GTK_CONTAINER(notebook), import_page);
	gtk_container_add(GTK_CONTAINER(notebook), scrolled_window);
	//gtk_notebook_set_tab_label_text(notebook, import_page, "Import");
	gtk_notebook_set_tab_label_text(notebook, scrolled_window, "Import");
	gtk_notebook_set_current_page(notebook, page_count);
}
