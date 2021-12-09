#include "gtkfunctions.h"
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdio.h>
//#include "wizard.h"

/* Postgres Variables */
PGconn *conn;

/* Quit function */
static void quit_activate(GSimpleAction *action,
			  GVariant      *parameter,
			  gpointer       user_data) {
	PQfinish(conn);
	GtkWidget *window = user_data;
	gtk_widget_destroy(window);
}

/* The "Import" function */
static void import_activate(GSimpleAction *action,
			    GVariant      *parameter,
			    gpointer       user_data) {
	GtkWidget *window = user_data;
	GtkBuilder *builder = gtk_builder_new();
	GtkWidget *import_dlg;

	if (gtk_builder_add_from_file(builder,"glade/new_atsugami.glade" , NULL) == 0) {
		printf("gtk_builder_add_from_file FAILED\n");
	}
	import_dlg = GTK_WIDGET(gtk_builder_get_object(builder, "importDialog"));
	gtk_dialog_run(GTK_DIALOG(import_dlg));
	gtk_widget_hide(import_dlg);
}

/* Import wizard */
static GtkWidget *assistant = NULL;
static GtkWidget *progress_bar = NULL;

static gboolean apply_changes_gradually (gpointer data) {
	gdouble fraction;

	/* Work, work, work... */
	fraction = gtk_progress_bar_get_fraction (GTK_PROGRESS_BAR (progress_bar));
	fraction += 0.05;

	if (fraction < 1.0) {
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress_bar), fraction);
		return G_SOURCE_CONTINUE;
	}
	else {
		/* Close automatically once changes are fully applied. */
		gtk_widget_destroy (assistant);
		assistant = NULL;
		return G_SOURCE_REMOVE;
	}
}

static void on_assistant_apply (GtkWidget *widget, gpointer data) {
	/* Start a timer to simulate changes taking a few seconds to apply. */
	g_timeout_add (100, apply_changes_gradually, NULL);
}

static void on_assistant_close_cancel (GtkWidget *widget, gpointer data) {
  GtkWidget **assistant = (GtkWidget **) data;

  gtk_widget_destroy (*assistant);
  *assistant = NULL;
}

static void on_assistant_prepare (GtkWidget *widget, GtkWidget *page, gpointer data) {
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
	GtkWidget *box, *label, *entry;

	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
	gtk_container_set_border_width(GTK_CONTAINER (box), 12);

	label = gtk_label_new("You must fill out this entry to continue:");
	gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);

	entry = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
	gtk_widget_set_valign(entry, GTK_ALIGN_CENTER);
	gtk_box_pack_start(GTK_BOX(box),entry, TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(entry), "changed",
	G_CALLBACK(on_wizard_entry_changed), assistant);

	gtk_widget_show_all(box);
	gtk_assistant_append_page(GTK_ASSISTANT(assistant), box);
	gtk_assistant_set_page_title(GTK_ASSISTANT(assistant), box, "Page 1");
	gtk_assistant_set_page_type(GTK_ASSISTANT(assistant), box, GTK_ASSISTANT_PAGE_INTRO);
}

static void wizard_create_page1 (GtkWidget *assistant) {
  GtkWidget *box, *checkbutton;

  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);
  gtk_container_set_border_width (GTK_CONTAINER (box), 12);

  checkbutton = gtk_check_button_new_with_label ("This is optional data, you may continue "
                                                 "even if you do not check this");
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


GtkWidget* do_assistant(GtkWidget *do_widget) {
  if (!assistant) {
	assistant = gtk_assistant_new ();

	gtk_window_set_default_size (GTK_WINDOW (assistant), -1, 300);

	gtk_window_set_screen (GTK_WINDOW (assistant),
			       gtk_widget_get_screen (do_widget));

	wizard_create_page0 (assistant);
	wizard_create_page1 (assistant);
	wizard_create_page2 (assistant);
	wizard_create_page3 (assistant);

	g_signal_connect (G_OBJECT (assistant), "cancel",
			  G_CALLBACK (on_assistant_close_cancel), &assistant);
	g_signal_connect (G_OBJECT (assistant), "close",
			  G_CALLBACK (on_assistant_close_cancel), &assistant);
	g_signal_connect (G_OBJECT (assistant), "apply",
			  G_CALLBACK (on_assistant_apply), NULL);
	g_signal_connect (G_OBJECT (assistant), "prepare",
			  G_CALLBACK (on_assistant_prepare), NULL);
	}

	if (!gtk_widget_get_visible (assistant))
		gtk_widget_show (assistant);
	else {
		gtk_widget_destroy (assistant);
		assistant = NULL;
	}

	return assistant;
}

static void wizard_activate() {
	GtkWidget *gtk_assistant_new(void);
	void gtk_assistant_add_action_widget(GtkAssistant *wizard, GtkWidget *child);
}
/*********************************************************\
********************* END WIZARD CODE *********************
\*********************************************************/

/* The "Bulk import" function */
static void importBulk_activate(GSimpleAction *action,
				GVariant      *parameter,
				gpointer       user_data) {
	GtkWidget *window = user_data;
	GtkBuilder *builder = gtk_builder_new();
	GtkWidget *import_dlg;

	if (gtk_builder_add_from_file(builder,"glade/new_atsugami.glade" , NULL) == 0) {
		printf("gtk_builder_add_from_file FAILED\n");
	}
	import_dlg = GTK_WIDGET(gtk_builder_get_object(builder, "importBulkDialog"));
	gtk_dialog_run(GTK_DIALOG(import_dlg));
	gtk_widget_hide(import_dlg);
}

/* The "Help" function */
static void help_activate(GSimpleAction *action,
			  GVariant      *parameter,
			  gpointer       user_data) {
	GtkWidget *window = user_data;
	GtkBuilder *builder = gtk_builder_new();
	GtkWidget *help_dlg;

	if (gtk_builder_add_from_file(builder,"glade/new_atsugami.glade" , NULL) == 0) {
		printf("gtk_builder_add_from_file FAILED\n");
	}
	help_dlg = GTK_WIDGET(gtk_builder_get_object(builder, "helpDialog"));
	gtk_dialog_run(GTK_DIALOG(help_dlg));
	gtk_widget_hide(help_dlg);
}

/* The "About" function */
static void about_activate(GSimpleAction *action,
			   GVariant      *parameter,
			   gpointer       user_data) {
	GtkWidget *window = user_data; 
	GtkBuilder *builder = gtk_builder_new();
	GtkWidget *about_dlg;

	if (gtk_builder_add_from_file(builder,"glade/new_atsugami.glade" , NULL) == 0) {
		printf("gtk_builder_add_from_file FAILED\n");
	}

	about_dlg = GTK_WIDGET(gtk_builder_get_object(builder, "aboutDialog"));
	gtk_dialog_run(GTK_DIALOG (about_dlg));
	gtk_widget_hide(about_dlg);
}

/* Open the import wizard */
/*static void wizard_activate(GSimpleAction *action,
			    GVariant      *parameter,
			    gpointer       user_data) {
} */

/* Index of app.* actions */
static GActionEntry win_entries[] = {
	{ "quit",   quit_activate,   NULL, NULL, NULL },
	{ "import", import_activate, NULL, NULL, NULL },
	{ "importBulk", importBulk_activate, NULL, NULL, NULL },
	{ "help",   help_activate,   NULL, NULL, NULL },
	{ "about",  about_activate,  NULL, NULL, NULL },
	{ "wizard", do_assistant, NULL, NULL, NULL },
//	{ "importBulkWizard", wizard_activate, NULL, NULL, NULL },
};

int main(int argc, char **argv) {
	GtkWidget *window;
	GtkBuilder *builder = NULL;

	GtkWidget *warnBanner;
	GtkWidget *errorBanner;
	GtkWidget *warnLabel;
	GtkWidget *errorLabel;
	GtkWidget *cancelButton0;
	GtkWidget *cancelButton1;

	GActionGroup *actions;
	GtkAccelGroup *accel_group;
	GtkWidget *item;

	gtk_init(&argc, &argv); /* Initialize GTK */

	/* Connect to PostgreSQL */
	char conninfo[] = "dbname=atsugami"; /* Sets the database for dbconnect() */
	conn = PQconnectdb(conninfo);

	/* Open the .glade/XML file for the UI */
	builder = gtk_builder_new();
	if (gtk_builder_add_from_file(builder,"glade/new_atsugami.glade" , NULL) == 0) {
		printf("gtk_builder_add_from_file FAILED\n");
		return(1);
	}

	/* Window definition */
	window = GTK_WIDGET(gtk_builder_get_object(builder,"primaryWindow"));
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_builder_connect_signals(builder,NULL);

	actions = (GActionGroup*)g_simple_action_group_new();
	g_action_map_add_action_entries(G_ACTION_MAP(actions),
					win_entries, G_N_ELEMENTS(win_entries),
					window);
	gtk_widget_insert_action_group(window, "app", actions);

	/* Warning and Error banners */
	warnBanner = (GtkWidget*)gtk_builder_get_object(builder, "warnBanner");
	errorBanner = (GtkWidget*)gtk_builder_get_object(builder, "errorBanner");

	/* Warning and Error labels */
	warnLabel = (GtkWidget*)gtk_builder_get_object(builder, "warnLabel");
	errorLabel = (GtkWidget*)gtk_builder_get_object(builder, "errorLabel");

	/* Create a dialog if the connection fails */
	if (PQstatus(conn) != CONNECTION_OK) {
//		gtk_label_seat_text(GTK_LABEL(errorLabel), "Connection to the database could not be established.");

		char errMsg[512];
		sprintf(errMsg, "\n%s", PQerrorMessage(conn));
		gtk_label_set_text(GTK_LABEL(errorLabel), errMsg);
	}
	else {
		gtk_label_set_text(GTK_LABEL(errorLabel), "No errors detected.");
	}

	/* buttons */
	item = (GtkWidget*)gtk_builder_get_object(builder, "import_button");
	item = (GtkWidget*)gtk_builder_get_object(builder, "importBulk_button");
	item = (GtkWidget*)gtk_builder_get_object(builder, "quit_button");
	item = (GtkWidget*)gtk_builder_get_object(builder, "about_button");

	item = (GtkWidget*)gtk_builder_get_object(builder, "importOpenWizard_button");
	cancelButton0 = (GtkWidget*)gtk_builder_get_object(builder, "cancelButton0");
	cancelButton1 = (GtkWidget*)gtk_builder_get_object(builder, "cancelButton1");

	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
