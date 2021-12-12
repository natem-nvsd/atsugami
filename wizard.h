#include <gtk/gtk.h>
#include <stdio.h>
#include "variables.h"

/* GTK+ variables */
static GdkPixbufLoader *pixbuf_loader = NULL;
static guint load_timeout = 0;
static GInputStream *image_stream = NULL;
static GtkWidget *assistant = NULL;
static GtkWidget *progress_bar = NULL;

/* Import wizard */
/* Wizard code from GTK3 demo application; modifications have been made. */
static gboolean apply_changes_gradually(gpointer data) {
	gdouble fraction;

	/* Work, work, work... */
	fraction = gtk_progress_bar_get_fraction (GTK_PROGRESS_BAR (progress_bar));
	fraction += 0.05;

	if (fraction < 1.0) {
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), fraction);
		return G_SOURCE_CONTINUE;
	}
	else {
		/* Close automatically once changes are fully applied. */
		gtk_widget_destroy(assistant);
		assistant = NULL;
		return G_SOURCE_REMOVE;
	}
}

static void on_assistant_apply (GtkWidget *widget, gpointer data) {
	/* Start a timer to simulate changes taking a few seconds to apply. */
	g_timeout_add(50, apply_changes_gradually, NULL);

	strcpy(import_file_path, ""); /* Clear import_file_path. The Postgres query will have been issued, and the transaction committed. */
}

static void on_assistant_close_cancel(GtkWidget *widget, gpointer data) {
	GtkWidget **assistant = (GtkWidget **) data;

	gtk_widget_destroy (*assistant);
	*assistant = NULL;
	strcpy(import_file_path, "");
}

static void on_assistant_prepare(GtkWidget *widget, GtkWidget *page, gpointer data) {
	gint current_page, n_pages;
	gchar *title;

	current_page = gtk_assistant_get_current_page(GTK_ASSISTANT(widget));
	n_pages = gtk_assistant_get_n_pages(GTK_ASSISTANT(widget));

	title = g_strdup_printf("Import wizard", current_page + 1, n_pages);
	gtk_window_set_title(GTK_WINDOW(widget), title);
	g_free(title);

	if (current_page == 3)
		gtk_assistant_commit(GTK_ASSISTANT(widget));
}

static void on_wizard_entry_changed(GtkWidget *widget, gpointer data) {
	GtkAssistant *assistant = GTK_ASSISTANT(data);
	GtkWidget *current_page;
	gint page_number;
	const gchar *text;

	page_number = gtk_assistant_get_current_page(assistant);
	current_page = gtk_assistant_get_nth_page(assistant, page_number);
	text = gtk_entry_get_text(GTK_ENTRY (widget));

	if (text && *text)
		gtk_assistant_set_page_complete(assistant, current_page, TRUE);
	else
		gtk_assistant_set_page_complete(assistant, current_page, FALSE);
}

static void wizard_create_page0(GtkWidget *assistant) {
	GtkWidget *box, *image, *label0, *label1, *label2, *label3, *entry0, *entry1, *entry2, *entry3;

	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_container_set_border_width(GTK_CONTAINER (box), 5);

	image = gtk_image_new_from_file(import_file_path);
	printf("%s\n", import_file_path);
	gtk_box_pack_start(GTK_BOX(box), image, FALSE, FALSE, 0);

	/* First label and text field */
	label0 = gtk_label_new("Enter the artist\'s name here:");
	gtk_box_pack_start(GTK_BOX(box), label0, FALSE, FALSE, 0);

	entry0 = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry0), TRUE);
	gtk_widget_set_valign(entry0, GTK_ALIGN_CENTER);
	gtk_box_pack_start(GTK_BOX(box), entry0, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(entry0), "changed",
	G_CALLBACK(on_wizard_entry_changed), assistant);

	/* Second label text field */
	label1 = gtk_label_new("Enter copyrights here:");
	gtk_box_pack_start(GTK_BOX(box), label1, FALSE, FALSE, 0);

	entry1 = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry1), TRUE);
	gtk_widget_set_valign(entry1, GTK_ALIGN_CENTER);
	gtk_box_pack_start(GTK_BOX(box), entry1, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(entry1), "changed",
	G_CALLBACK(on_wizard_entry_changed), assistant);

	/* Third label and text field */
	label2 = gtk_label_new("Enter character names here:");
	gtk_box_pack_start(GTK_BOX(box), label2, FALSE, FALSE, 0);

	entry2 = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry2), TRUE);
	gtk_widget_set_valign(entry2, GTK_ALIGN_CENTER);
	gtk_box_pack_start(GTK_BOX(box), entry2, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(entry2), "changed",
	G_CALLBACK(on_wizard_entry_changed), assistant);


	/* Fourth label and text field */
	label3 = gtk_label_new("Enter tags here:");
	gtk_box_pack_start(GTK_BOX(box), label3, FALSE, FALSE, 0);

	entry3 = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry3), TRUE);
	gtk_widget_set_valign(entry3, GTK_ALIGN_CENTER);
	gtk_box_pack_start(GTK_BOX(box), entry3, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(entry3), "changed",
	G_CALLBACK(on_wizard_entry_changed), assistant);

	/* Display the window */
	gtk_widget_show_all(box);
	gtk_assistant_append_page(GTK_ASSISTANT(assistant), box);
	gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), box, "Page 1");
	gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), box, GTK_ASSISTANT_PAGE_INTRO);
}

static void wizard_create_page1 (GtkWidget *assistant) {
	GtkWidget *box, *checkbutton;
	box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);
	gtk_container_set_border_width (GTK_CONTAINER (box), 12);

	checkbutton = gtk_check_button_new_with_label ("This is file has children");
	gtk_box_pack_start (GTK_BOX (box), checkbutton, FALSE, FALSE, 0);

	gtk_widget_show_all (box);
	gtk_assistant_append_page (GTK_ASSISTANT (assistant), box);
	gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), box, TRUE);
	gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), box, "Page 2");
}

static void wizard_create_page2 (GtkWidget *assistant) {
  GtkWidget *label;

  label = gtk_label_new ("This is a confirmation page, press 'Apply' to apply changes");

  gtk_widget_show (label);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), label);
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), label, GTK_ASSISTANT_PAGE_CONFIRM);
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), label, TRUE);
  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), label, "Confirmation");
}

static void wizard_create_page3 (GtkWidget *assistant) {
  progress_bar = gtk_progress_bar_new ();
  gtk_widget_set_halign (progress_bar, GTK_ALIGN_CENTER);
  gtk_widget_set_valign (progress_bar, GTK_ALIGN_CENTER);

  gtk_widget_show (progress_bar);
  gtk_assistant_append_page (GTK_ASSISTANT (assistant), progress_bar);
  gtk_assistant_set_page_type (GTK_ASSISTANT (assistant), progress_bar, GTK_ASSISTANT_PAGE_PROGRESS);
  gtk_assistant_set_page_title (GTK_ASSISTANT (assistant), progress_bar, "Applying changes");

  /* This prevents the assistant window from being
   * closed while we're "busy" applying changes.
   */
  gtk_assistant_set_page_complete (GTK_ASSISTANT (assistant), progress_bar, FALSE);
}

//extern GtkWidget *do_assistant(GtkWidget *do_widget) {
extern GtkWidget *do_assistant() {
	if (!assistant) {
		assistant = gtk_assistant_new();

		gtk_window_set_default_size(GTK_WINDOW(assistant), 400, 500);

/*		gtk_window_set_screen(GTK_WINDOW(assistant), gtk_widget_get_screen (do_widget)); */

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
		gtk_widget_destroy (assistant);
		assistant = NULL;
	}

	return assistant;
}
