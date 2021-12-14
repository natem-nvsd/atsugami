/***********************************************\
* TODO:						*
* - Move all non-main functions into their own	*
* *.c files					*
* - 
\***********************************************/

#include <errno.h>
#include "error_dialogs.h"
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdio.h>
#include "variables.h"
#include "wizard.h"

/* Postgres Variables */
PGconn *conn;
char import_query[] = "INSERT INTO public.files (path, has_children, child, parent_uuid, child_uuids, copyrights, characters, tags, width, height, source, format) VALUES ('";

/* Quit function */
static void quit_activate(GSimpleAction *action,
			  GVariant      *parameter,
			  gpointer       user_data) {
	PQfinish(conn);
	GtkWidget *window = user_data;
	gtk_widget_destroy(window);
}

/* The "Import" function */
static void open_response_cb(GtkNativeDialog *dialog, gint response_id, gpointer user_data) {
	GtkFileChooserNative *native = user_data;
//	GtkWidget *message_dialog;
	char *file_path;
//	char *contents;
//	GError *error = NULL;

	if (response_id == GTK_RESPONSE_ACCEPT) {
		/* Get the path of the file selected when the user presses "Import" */
		file_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(native));

		/* Copy the path into `import_file_path`, since `file_path` is cleared when the dialog is destroyed. */
		strcpy(import_file_path, file_path);

		/* Run the wizard; the file chooser window is destroyed before the wizard opens. */
		do_assistant();
	}

	gtk_native_dialog_destroy(GTK_NATIVE_DIALOG(native));
	g_object_unref(native);
}

/* The import window itself */
static void import_activate(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
	GApplication *app = user_data;
	GtkFileChooserNative *native;
	GtkFileFilter *filter0;
	GtkFileFilter *filter1;

	native = gtk_file_chooser_native_new("Import File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, "_Import", "_Cancel");

	filter0 = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter0, "*.gif");
	gtk_file_filter_add_pattern(filter0, "*.jpg");
	gtk_file_filter_add_pattern(filter0, "*.jpeg");
	gtk_file_filter_add_pattern(filter0, "*.png");
	gtk_file_filter_add_pattern(filter0, "*.svg");
	gtk_file_filter_set_name(filter0, "Image files");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(native), filter0);

	filter1 = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter1, "*");
	gtk_file_filter_set_name(filter1, "All files");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(native), filter1);


	g_object_set_data_full(G_OBJECT(native), "app", g_object_ref(app), g_object_unref);
	g_signal_connect(native, "response", G_CALLBACK(open_response_cb), native);

	gtk_native_dialog_show(GTK_NATIVE_DIALOG(native));
}

/* The "About" function */
static void about_activate(GSimpleAction *action,
			   GVariant      *parameter,
			   gpointer       user_data) {
//	GtkWidget *window = user_data; 
	GtkBuilder *builder = gtk_builder_new();
	GtkWidget *about_dlg;

	if (gtk_builder_add_from_file(builder,"atsugami.glade" , NULL) == 0) {
//	if (gtk_builder_add_from_file(builder,"/usr/local/share/atsugami/atsugami.glade" , NULL) == 0) {
		printf("gtk_builder_add_from_file FAILED\n");
	}

	about_dlg = GTK_WIDGET(gtk_builder_get_object(builder, "aboutDialog"));
	gtk_dialog_run(GTK_DIALOG (about_dlg));
	gtk_widget_hide(about_dlg);
}

/* Index of app.* actions */
static GActionEntry win_entries[] = {
	{ "quit",   quit_activate,   NULL, NULL, NULL },
	{ "import", import_activate, NULL, NULL, NULL },
//	{ "importBulk", import_bulk_activate, NULL, NULL, NULL },
	{ "importBulk", do_expander, NULL, NULL, NULL },
//	{ "help",   help_activate,   NULL, NULL, NULL },
	{ "about",  about_activate,  NULL, NULL, NULL },
};

int main(int argc, char *argv[]) {
	GtkWidget *window;
	GtkBuilder *builder = NULL;

	GtkWidget *warnBanner;
	GtkWidget *errorBanner;
	GtkWidget *warnLabel;
	GtkWidget *errorLabel;

	GActionGroup *actions;
//	GtkAccelGroup *accel_group;
	GtkWidget *item;

	gtk_init(&argc, &argv); /* Initialize GTK */

	/* Connect to PostgreSQL */
	char conninfo[] = "dbname=atsugami"; /* Sets the database for dbconnect() */
	conn = PQconnectdb(conninfo);

	/* Open the .glade/XML file for the UI */
	builder = gtk_builder_new();
	if (gtk_builder_add_from_file(builder,"atsugami.glade" , NULL) == 0) {
//	if (gtk_builder_add_from_file(builder, "/usr/local/share/atsugami/atsugami.glade", NULL) == 0) {
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

	if (PQstatus(conn) == CONNECTION_OK) {
		gtk_label_set_text(GTK_LABEL(errorLabel), "Connection to the database was successful.");
	}

	/* buttons */
	item = (GtkWidget*)gtk_builder_get_object(builder, "import_button");
	item = (GtkWidget*)gtk_builder_get_object(builder, "importBulk_button");
	item = (GtkWidget*)gtk_builder_get_object(builder, "quit_button");
	item = (GtkWidget*)gtk_builder_get_object(builder, "about_button");
	item = (GtkWidget*)gtk_builder_get_object(builder, "import_cancel_button");
	item = (GtkWidget*)gtk_builder_get_object(builder, "import_ok_button");

	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
