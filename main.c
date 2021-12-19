/***********************************************\
* main.c: location of main() in Atsugami.	*
*						*
* TODO:						*
* - Re-write UI in C				*
* - Create new error banner when uuid-ossp is	*
* available/ENOENT				*
*						*
\***********************************************/

#include "about.h"
#include <errno.h>
#include "error_dialogs.h"
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include "import.h"
#include <libpq-fe.h>
#include "main.h"
#include <stdio.h>

PGconn *conn;
char conninfo[] = "dbname=atsugami"; /* Sets the database for dbconnect() */

/* Quit function */
static void quit_activate(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
	PQfinish(conn);
	GtkWidget *window = user_data;
	gtk_widget_destroy(window);
}

/* Index of app.* actions */
static GActionEntry win_entries[] = {
	{ "quit",   quit_activate,   NULL, NULL, NULL },
	{ "import", import_activate, NULL, NULL, NULL },
//	{ "import_bulk", import_bulk_activate, NULL, NULL, NULL },
//	{ "import_bulk", error_activate, NULL, NULL, NULL },
//	{ "help",   help_activate,   NULL, NULL, NULL },
	{ "about",  about_activate,  NULL, NULL, NULL }
};

int main(int argc, char *argv[]) {
	/* Connect to PostgreSQL */
	conn = PQconnectdb(conninfo);

	GtkWidget *window;
	GtkBuilder *builder = NULL;
	GtkWidget *warn_banner;
	GtkWidget *error_banner;
	GtkWidget *warnLabel;
	GtkWidget *errorLabel;

	GActionGroup *actions;
//	GtkAccelGroup *accel_group;
	GtkWidget *item;

	gtk_init(&argc, &argv); /* Initialize GTK */

	/* Open the .glade/XML file for the UI */
	builder = gtk_builder_new();
	if (gtk_builder_add_from_file(builder, GLADEFILE, NULL) == 0) {
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
	warn_banner = (GtkWidget*)gtk_builder_get_object(builder, "warning_banner");
	error_banner = (GtkWidget*)gtk_builder_get_object(builder, "error_banner");

	/* Warning and Error labels */
	warnLabel = (GtkWidget*)gtk_builder_get_object(builder, "warnLabel");
	errorLabel = (GtkWidget*)gtk_builder_get_object(builder, "errorLabel");

	/* Show an error info bar if the connection fails */
	if (PQstatus(conn) != CONNECTION_OK) {
		gtk_info_bar_set_revealed(error_banner, TRUE);

		char errMsg[1024];
		sprintf(errMsg, "\n%s", PQerrorMessage(conn));
		gtk_label_set_text(GTK_LABEL(errorLabel), errMsg);
	}

	if (PQstatus(conn) == CONNECTION_OK) {
		gtk_info_bar_set_revealed(error_banner, FALSE);
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
