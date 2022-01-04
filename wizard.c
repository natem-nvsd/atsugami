/* wizard.c */
#include <gtk/gtk.h>
#include "import.h"
#include <libpq-fe.h>
#include "main.h"
#include <stdio.h>
#include "wizard.h"

/* Variables and pointers */
PGresult *res;
GtkWidget *entry0, *entry1, *entry2, *entry3, *entry4, *entry5, *entry6, *rating_entry;
GtkWidget *image;
GdkPixbuf *image_pixbuf;
static GtkWidget *assistant = NULL;
//static GtkWidget *progress_bar = NULL;
const gchar *text0;
const gchar *text1;
const gchar *text2;
const gchar *text3;
const gchar *text4;
const gchar *text5;
const gchar *text6;
const gchar *textx;

float workarea_width;
float workarea_height;
float width;
float height;
char char_width[6]; /* only corrupt, damaged, or malicisious images can over flow these two buffers; fix them */
char char_height[6];
char query_string[20480];	/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
char child_uuids[2048];		/* THESE MUST BE FIXED TO PREVENT BUFFER OVERFLOWS */
gboolean parent_bool = FALSE;
gboolean child_bool = FALSE;
gboolean has_children = FALSE;

static void on_assistant_apply(GtkWidget *widget, gpointer data) {
	//char psql_error[2048];

	//PQexec(conn, "BEGIN TRANSACTION;");

	/* Create the query to submit to PostgreSQL */
	strcpy(query_string, "INSERT INTO public.files (path, artist, copyrights, characters, tags, source, rating, width, height, is_parent, is_child, has_children, parent_uuid, child_uuids, imported_at) VALUES ('");

	/* add the file path to the query */
	strcat(query_string, import_file_path);
	strcat(query_string, "', '{");

	/* Entry fields from page0 */
	text0 = gtk_entry_get_text(GTK_ENTRY(entry0));	/* Artist */
	if (gtk_entry_get_text_length(entry0) == 0) {
		strcat(query_string, "NULL}', '{");
	}
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
	text1 = gtk_entry_get_text(GTK_ENTRY(entry1));	/* Copyrights */
	if (gtk_entry_get_text_length(entry1) == 0) {
		strcat(query_string, "NULL}', '{");
	}
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
	strcat(copyright_query, "') ON CONFLICT DO NOTHING;"); 	/* i should use a macro for this */

	/* Entry fields from page2 */
	text2 = gtk_entry_get_text(GTK_ENTRY(entry2));	/* Characters */
	if (gtk_entry_get_text_length(entry2) == 0) {
		strcat(query_string, "NULL}', '{");
	}
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
	strcat(characters_query, "') ON CONFLICT DO NOTHING;"); 	/* i should use a macro for this */
		/* This won't work for multiple characters */

	/* Entry fields from page3 */
	text3 = gtk_entry_get_text(GTK_ENTRY(entry3));	/* Tags */
	if (gtk_entry_get_text_length(entry3) == 0) {
		strcat(query_string, "NULL}', '");
	}
	if (gtk_entry_get_text_length(entry3) > 0) {
		strcat(query_string, text3);
		strcat(query_string, "}', '");
	}

	/* Entry fields from page4 */
	text4 = gtk_entry_get_text(GTK_ENTRY(entry4));	/* Source */
	if (gtk_entry_get_text_length(entry4) == 0) {
		strcat(query_string, "NULL', '");
	}
	if (gtk_entry_get_text_length(entry4) > 0) {
		strcat(query_string, text4);
		strcat(query_string, "', '");
	}

	textx = gtk_entry_get_text(GTK_ENTRY(rating_entry));	/* add the rating to the query */
	printf("textx: %s\n", textx);
	strcat(query_string, textx);
	strcat(query_string, "', ");

	/* add the image resolution to the query */
	strcat(query_string, char_width); /* Width */
	strcat(query_string, ", ");

	strcat(query_string, char_height); /* Height */
	strcat(query_string, ", ");

	/* add the values of is_parent */
	if (parent_bool == TRUE) {
		strcat(query_string, "TRUE, ");
	}
	if (parent_bool == FALSE) {
		strcat(query_string, "FALSE, ");
	}

	/* add the values of  is_child */
	if (child_bool == TRUE) {
		strcat(query_string, "TRUE, ");
	}
	if (child_bool == FALSE) {
		strcat(query_string, "FALSE, ");
	}
	
	/* Has children */
	if (has_children == TRUE) {
		strcat(query_string, "TRUE, ");
	}
	if (has_children == FALSE) {
		strcat(query_string, "FALSE, ");
	}

	/* Parent UUID */
	text5 = gtk_entry_get_text(GTK_ENTRY(entry5));
	if (gtk_entry_get_text_length(entry5) == 0) {
		strcat(query_string, "NULL, '{");
	}
	if (gtk_entry_get_text_length(entry5) > 0) {
		strcat(query_string, text5);
		strcat(query_string, ", '{");
	}

	/* Child UUIDs */
	text6 = gtk_entry_get_text(GTK_ENTRY(entry6));
	if (gtk_entry_get_text_length(entry6) == 0) {
		strcat(query_string, "NULL}', now());");
	}
	if (gtk_entry_get_text_length(entry6) > 0) {
		strcat(query_string, text6);
		strcat(query_string, "}', now()) ON CONFLICT DO NOTHING;");
	}

	PQexec(conn, query_string);

	/* Show an error dialog if the query failed */
	/*
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		strcpy(psql_error, PQerrorMessage(conn));
		printf("%s\n", psql_error);
		postgres_error_activate();
		PQclear(res);
	}
	*/
	// AAAAAAaaaaaaaaaaaaaaaaaaaaaaandd this causes a segfault

	//PQexec(conn, "COMMIT TRANSACTION");
	strcpy(import_file_path, ""); /* Clear import_file_path. */
}

static void on_assistant_close_cancel(GtkWidget *widget, gpointer data) {
	GtkWidget **assistant = (GtkWidget **) data;

	gtk_widget_destroy(*assistant);
	*assistant = NULL;
	strcpy(import_file_path, "");
}

static void on_assistant_prepare(GtkWidget *widget, GtkWidget *page, gpointer data) {
	gint current_page, n_pages;
	gchar *title;

	current_page = gtk_assistant_get_current_page(GTK_ASSISTANT(widget));
	n_pages = gtk_assistant_get_n_pages(GTK_ASSISTANT(widget));

	title = g_strdup_printf("assistant (%d of %d)", current_page + 1, n_pages);
	gtk_window_set_title(GTK_WINDOW(widget), title);
	g_free(title);

	if (current_page == 5)
		gtk_assistant_commit(GTK_ASSISTANT(widget));
}

static void on_wizard_entry_changed(GtkWidget *widget, gpointer data) {
	GtkAssistant *assistant = GTK_ASSISTANT(data);
	GtkWidget *current_page;
	gint page_number;
	const gchar *text;

	page_number = gtk_assistant_get_current_page(assistant);
	current_page = gtk_assistant_get_nth_page(assistant, page_number);
	text = gtk_entry_get_text(GTK_ENTRY(widget));

	if (text && *text)
		gtk_assistant_set_page_complete(assistant, current_page, TRUE);
	else
		gtk_assistant_set_page_complete(assistant, current_page, FALSE);
}

static void wizard_create_page0(GtkWidget *assistant) {
	GtkWidget *box, *cbox, *image_preview;
	GtkWidget *label0, *label1, *label2, *label3, *label4, *label5, *revealer_label;
	GtkWidget *revealer0, *revealer1, *revealer2, *revealer3, *revealer4, *revealer5;
	GtkStyleContext *css_context;
	GtkCssProvider *css;

	css = gtk_css_provider_new();
	css_context = gtk_style_context_new();
	revealer_label = gtk_label_new("Text fields cannot contain the following characters: % -- ; ' \" [ ] { }");

	gtk_css_provider_load_from_path(css, CSSFILE, NULL);
	gtk_style_context_add_class(css_context, "revealer_label_text");

	GdkPixbuf *image_preview_pixbuf = NULL;
	int int_width, int_height, win_height, win_width;

	image = gtk_image_new_from_file(import_file_path);
	image_pixbuf = gdk_pixbuf_new_from_file(import_file_path, NULL);

	/* width & height of the the image, respectively */
	width  = gdk_pixbuf_get_width(image_pixbuf);
	height = gdk_pixbuf_get_height(image_pixbuf);

	/* Make int copies of width and height */
	int_width  = (int)width;
	int_height = (int)height;

	/* Make char copies of int_width and int_height */
	sprintf(char_width, "%d", int_width);
	sprintf(char_height, "%d", int_height);

	gtk_window_get_size(GTK_WIDGET(assistant), &win_height, &win_width);

	printf("Window width: %d\n",  win_width);
	printf("Window height: %d\n", win_height);
	printf("Image width: %d\n",  int_width);
	printf("Image height: %d\n", int_height);

	/* Resize the input image
	*  Lower resolution images will show up bigger (but less than their original resolution), and large images will be more
	* appropriately sized */
	if (int_width < win_width || int_height < win_height) {
		image_preview_pixbuf = gdk_pixbuf_scale_simple(image_pixbuf, (width * 0.75), (height * 0.75), GDK_INTERP_BILINEAR);
		image_preview = gtk_image_new_from_pixbuf(image_preview_pixbuf);
	}
	else if (int_width > win_width || int_height > win_height) {
		image_preview_pixbuf = gdk_pixbuf_scale_simple(image_pixbuf, (width * 0.25), (height * 0.25), GDK_INTERP_BILINEAR);
		image_preview = gtk_image_new_from_pixbuf(image_preview_pixbuf);
	}
	else if (int_width > win_width || int_height > win_height) {
		image_preview_pixbuf = gdk_pixbuf_scale_simple(image_pixbuf, (width * 0.25), (height * 0.25), GDK_INTERP_BILINEAR);
		image_preview = gtk_image_new_from_pixbuf(image_preview_pixbuf);
	}

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_container_set_border_width(GTK_CONTAINER(box), 10);

	gtk_box_pack_start(GTK_BOX(box), image_preview, FALSE, FALSE, 0);

	/* First label and text field */
	label0 = gtk_label_new("Enter the artist\'s name here:");
	gtk_widget_set_halign(label0, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(box), label0, FALSE, FALSE, 0);

	entry0 = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry0), TRUE);
	gtk_widget_set_valign(entry0, GTK_ALIGN_START);
	gtk_entry_set_placeholder_text(entry0, "Values are separated by commas; spaces are optional.");
	gtk_box_pack_start(GTK_BOX(box), entry0, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(entry0), "changed", G_CALLBACK(on_wizard_entry_changed), assistant);

	revealer0 = gtk_revealer_new();
	css_context = gtk_widget_get_style_context(revealer_label);
	css_context = gtk_widget_get_style_context(entry0);

	gtk_widget_set_halign(revealer_label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(box), revealer_label, FALSE, FALSE, 0);

	/* Second label text field */
	label1 = gtk_label_new("Enter copyrights here:");
	gtk_widget_set_halign(label1, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(box), label1, FALSE, FALSE, 0);

	entry1 = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry1), TRUE);
	gtk_widget_set_valign(entry1, GTK_ALIGN_START);
	gtk_entry_set_placeholder_text(entry1, "Values are separated by commas; spaces are optional.");
	gtk_box_pack_start(GTK_BOX(box), entry1, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(entry1), "changed", G_CALLBACK(on_wizard_entry_changed), assistant);

	revealer1 = gtk_revealer_new();
	css_context = gtk_widget_get_style_context(entry1);

	gtk_widget_set_halign(revealer_label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(box), revealer_label, FALSE, FALSE, 0);

	/* Third label and text field */
	label2 = gtk_label_new("Enter character names here:");
	gtk_widget_set_halign(label2, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(box), label2, FALSE, FALSE, 0);

	entry2 = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry2), TRUE);
	gtk_widget_set_valign(entry2, GTK_ALIGN_START);
	gtk_entry_set_placeholder_text(entry2, "Values are separated by commas; spaces are optional.");
	gtk_box_pack_start(GTK_BOX(box), entry2, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(entry2), "changed", G_CALLBACK(on_wizard_entry_changed), assistant);
	css_context = gtk_widget_get_style_context(entry2);

	revealer2 = gtk_revealer_new();

	gtk_widget_set_halign(revealer_label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(box), revealer_label, FALSE, FALSE, 0);

	/* Fourth label and text field */
	label3 = gtk_label_new("Enter tags here:");
	gtk_widget_set_halign(label3, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(box), label3, FALSE, FALSE, 0);

	entry3 = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry3), TRUE);
	gtk_widget_set_valign(entry3, GTK_ALIGN_START);
	gtk_entry_set_placeholder_text(entry3, "Values are separated by commas; spaces are optional.");
	gtk_box_pack_start(GTK_BOX(box), entry3, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(entry3), "changed", G_CALLBACK(on_wizard_entry_changed), assistant);
	css_context = gtk_widget_get_style_context(entry3);

	revealer3 = gtk_revealer_new();

	gtk_widget_set_halign(revealer_label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(box), revealer_label, FALSE, FALSE, 0);

	/* Fifth label and text field */
	label4 = gtk_label_new("Enter source URL here:");
	gtk_widget_set_halign(label4, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(box), label4, FALSE, FALSE, 0);

	entry4 = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry4), TRUE);
	gtk_widget_set_valign(entry4, GTK_ALIGN_START);
	gtk_entry_set_placeholder_text(entry4, "Values are separated by commas; spaces are optional.");
	gtk_box_pack_start(GTK_BOX(box), entry4, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(entry4), "changed", G_CALLBACK(on_wizard_entry_changed), assistant);
	css_context = gtk_widget_get_style_context(entry4);

	revealer4 = gtk_revealer_new();

	gtk_widget_set_halign(revealer_label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(box), revealer_label, FALSE, FALSE, 0);

	/*Sixth label and text field; Add a GTK combo box here */
	label5 = gtk_label_new("Rating:");
	gtk_widget_set_halign(label5, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(box), label5, FALSE, FALSE, 0);
	css_context = gtk_widget_get_style_context(entry5);

	cbox = gtk_combo_box_text_new();
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(cbox), "safe", "Safe");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(cbox), "questionable", "Questionable");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(cbox), "explicit", "Explicit");
	gtk_box_pack_start(GTK_BOX(box), cbox, FALSE, FALSE, 0);

	revealer5 = gtk_revealer_new();

	gtk_widget_set_halign(revealer_label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(box), revealer_label, FALSE, FALSE, 0);

	/* Invisible entry field */
	rating_entry = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(rating_entry), TRUE);
	g_object_bind_property(cbox, "active-id", rating_entry, "text", G_BINDING_DEFAULT);
	g_signal_connect(G_OBJECT(rating_entry), "changed", G_CALLBACK(on_wizard_entry_changed), assistant);

	/* Display the window */
	gtk_widget_show_all(box); /* Don't mess with this code */
	gtk_assistant_append_page(GTK_ASSISTANT(assistant), box);
	gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), box, "Tagging");
	gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), box, GTK_ASSISTANT_PAGE_INTRO);
}

/* Check button callback
*
*  THis would be soooooooooooo much easier in GTK4
*  gtk_check_button_get_active();
* https://docs.gtk.org/gtk4/method.CheckButton.get_active.html */
GtkWidget *pg1_label0, *pg1_label1, *check_button0, *check_button1, *check_button2;


static void is_parent_cb(GtkWidget *check_button, gpointer data) {
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button0))) {
		parent_bool = TRUE;
		gtk_widget_set_sensitive(pg1_label1, TRUE);
		gtk_widget_set_sensitive(entry6, TRUE);
	}
	else {
		g_print("%s off\n", gtk_button_get_label(GTK_BUTTON(check_button0)));
		gtk_widget_set_sensitive(pg1_label1, FALSE);
		gtk_widget_set_sensitive(entry6, FALSE);
	}
}

static void is_child_cb(GtkWidget *check_button, gpointer data) {
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button1))) {
		child_bool = TRUE;
		gtk_widget_set_sensitive(pg1_label0, TRUE);
		gtk_widget_set_sensitive(entry5, TRUE);
	}
	else {
		g_print("%s off\n", gtk_button_get_label(GTK_BUTTON(check_button1)));
		gtk_widget_set_sensitive(pg1_label0, FALSE);
		gtk_widget_set_sensitive(entry5, FALSE);
	}
}

static void has_children_cb(GtkWidget *check_button, gpointer data) {
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button2))) {
		has_children = TRUE;
	}
	else {
		g_print("%s off\n", gtk_button_get_label(GTK_BUTTON(check_button2)));
	}
}

static void wizard_create_page1(GtkWidget *assistant) {
	GtkWidget *box;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
	gtk_container_set_border_width(GTK_CONTAINER(box), 12);

	/* First check box */
	check_button0 = gtk_check_button_new_with_label("This file is a parent");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button0), FALSE);
	g_signal_connect(check_button0, "toggled", G_CALLBACK(is_parent_cb), NULL);
	gtk_widget_set_valign(check_button0, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(box), check_button0, FALSE, FALSE, 0);

	/* Child UUID entry */
	pg1_label1 = gtk_label_new("Enter the UUID(s) of the child(ren) here:");
	gtk_widget_set_halign(pg1_label1, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(box), pg1_label1, FALSE, FALSE, 0);

	entry6 = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry6), TRUE);
	gtk_widget_set_valign(entry6, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(box), entry6, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(entry6), "changed",
	G_CALLBACK(on_wizard_entry_changed), assistant);

	gtk_widget_set_sensitive(pg1_label1, FALSE);
	gtk_widget_set_sensitive(entry6, FALSE);

	/* Second check box */
	check_button1 = gtk_check_button_new_with_label("This file is a child");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button1), FALSE);
	g_signal_connect(check_button1, "toggled", G_CALLBACK(is_child_cb), NULL);
	gtk_widget_set_valign(check_button1, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(box), check_button1, FALSE, FALSE, 0);
	/* label */
	pg1_label0 = gtk_label_new("Enter Parent UUID here:");
	gtk_widget_set_halign(pg1_label0, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(box), pg1_label0, FALSE, FALSE, 0);
	/* text box */
	entry5 = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry5), TRUE);
	gtk_widget_set_valign(entry5, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(box), entry5, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(entry5), "changed",
	G_CALLBACK(on_wizard_entry_changed), assistant);

	gtk_widget_set_sensitive(pg1_label0, FALSE);
	gtk_widget_set_sensitive(entry5, FALSE);

	/* Third check box */
	check_button2 = gtk_check_button_new_with_label("This file has children");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button2), FALSE);
	g_signal_connect(check_button2, "toggled", G_CALLBACK(has_children_cb), NULL);
	gtk_widget_set_valign(check_button2, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(box), check_button2, FALSE, FALSE, 0);

	/* SHow the box and its widgets */
	gtk_widget_show_all(box);
	gtk_assistant_append_page(GTK_ASSISTANT(assistant), box);
	gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant), box, TRUE);
	gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), box, "Hierarchy");
}

static void wizard_create_page2(GtkWidget *assistant) {
	GtkWidget *box, *checkbutton;
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
	gtk_container_set_border_width(GTK_CONTAINER(box), 12);

	checkbutton = gtk_check_button_new_with_label("This is file has commentary");
	gtk_widget_set_sensitive(checkbutton, FALSE);
	gtk_box_pack_start(GTK_BOX(box), checkbutton, FALSE, FALSE, 0);

	gtk_widget_show_all(box);
	gtk_assistant_append_page(GTK_ASSISTANT(assistant), box);
	gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant), box, TRUE);
	gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), box, "Commentary");
}

static void wizard_create_page3(GtkWidget *assistant) {
	GtkWidget *label;

	label = gtk_label_new("This is a confirmation page, press 'Apply' to apply changes");

	gtk_widget_show (label);
	gtk_assistant_append_page(GTK_ASSISTANT(assistant), label);
	gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), label, GTK_ASSISTANT_PAGE_CONFIRM);
	gtk_assistant_set_page_complete(GTK_ASSISTANT(assistant), label, TRUE);
	gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), label, "Confirmation");
}

extern GtkWidget *do_assistant() {
	if (!assistant) {
		/* Get the resolution of the user's screen */
		GdkRectangle workarea = {0}; gdk_monitor_get_workarea(gdk_display_get_primary_monitor(gdk_display_get_default()), &workarea);
		workarea_width  = workarea.width  * 0.5;
		workarea_height = workarea.height * 0.75;

		/* Create the assistant window */
		assistant = gtk_assistant_new();
		gtk_window_set_resizable(assistant, FALSE);
		gtk_window_set_position(assistant, GTK_WIN_POS_CENTER_ALWAYS);
		gtk_window_set_default_size(GTK_WINDOW(assistant), workarea_width, workarea_height);

		/* Pages in the wizard */
		wizard_create_page0(assistant);
		wizard_create_page1(assistant);
		wizard_create_page2(assistant);
		wizard_create_page3(assistant);

		g_signal_connect(G_OBJECT(assistant), "cancel", G_CALLBACK(on_assistant_close_cancel), &assistant);
		g_signal_connect(G_OBJECT(assistant), "close", G_CALLBACK(on_assistant_close_cancel), &assistant);
		g_signal_connect(G_OBJECT(assistant), "apply", G_CALLBACK(on_assistant_apply), NULL);
		g_signal_connect(G_OBJECT(assistant), "prepare", G_CALLBACK(on_assistant_prepare), NULL);
	}
	if (!gtk_widget_get_visible(assistant))
		gtk_widget_show(assistant);
	else {
		gtk_widget_destroy(assistant);
		assistant = NULL;
	}

	return assistant;
}
