#include <errno.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdio.h>
#include "variables.h"
#include "wizard.h"

/* GTK+ variables */
/*static GdkPixbufLoader *pixbuf_loader = NULL;
static guint load_timeout = 0;
static GInputStream *image_stream = NULL;
static GtkWidget *assistant = NULL;
static GtkWidget *progress_bar = NULL; */

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
//	GApplication *app = g_object_get_data(G_OBJECT(native), "app");
//	GtkWidget *message_dialog;
	char *file_path;
//	char *contents;
//	GError *error = NULL;

	GtkBuilder *builder;
	GtkFileFilter* filter;

	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder,"glade/new_atsugami.glade", NULL);
	filter = (GtkFileFilter*)gtk_builder_get_object(builder, "import_file_filter");

	if (response_id == GTK_RESPONSE_ACCEPT) {
		/* Get the path of the file selected when the user presses "Import" */
		file_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(native));

		/* Copy the path into `import_file_path`, since `file_path` is cleared when the dialog is destroyed. */
		strcpy(import_file_path, file_path);
		printf("%s\n", import_file_path);
		do_assistant();
	}

	gtk_native_dialog_destroy(GTK_NATIVE_DIALOG(native));
	g_object_unref(native);
}

/* The import window itself */
static void import_activate(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
	GApplication *app = user_data;
	GtkFileChooserNative *native;
//	action = "app.wizard";
	GtkBuilder *builder;
	GtkFileFilter* filter;

	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder,"glade/new_atsugami.glade", NULL);
	filter = (GtkFileFilter*)gtk_builder_get_object(builder, "import_file_filter");

	native = gtk_file_chooser_native_new("Import File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, "_Import", "_Cancel");

	g_object_set_data_full(G_OBJECT(native), "app", g_object_ref(app), g_object_unref);
	g_signal_connect(native, "response", G_CALLBACK(open_response_cb), native);

	gtk_native_dialog_show(GTK_NATIVE_DIALOG(native));
}

/* The "Bulk import" function */
static void importBulk_activate(GSimpleAction *action,
				GVariant      *parameter,
				gpointer       user_data) {
//	GtkWidget *window = user_data;
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
static void help_activate(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
//	GtkWidget *window = user_data;
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
//	GtkWidget *window = user_data; 
	GtkBuilder *builder = gtk_builder_new();
	GtkWidget *about_dlg;

	if (gtk_builder_add_from_file(builder,"glade/new_atsugami.glade" , NULL) == 0) {
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
	{ "importBulk", importBulk_activate, NULL, NULL, NULL },
	{ "help",   help_activate,   NULL, NULL, NULL },
	{ "about",  about_activate,  NULL, NULL, NULL },
	{ "wizard", do_assistant,    NULL, NULL, NULL },
	//	{ "importBulkWizard", wizard_activate, NULL, NULL, NULL },
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
	item = (GtkWidget*)gtk_builder_get_object(builder, "import_cancel_button");
	item = (GtkWidget*)gtk_builder_get_object(builder, "import_ok_button");

	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
