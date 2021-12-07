#include "functions.h"
#include <gtk/gtk.h>
#include <libpq-fe.h>

static void activate(GtkApplication* app, gpointer user_data) {
	GtkWidget *window;
	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "Atsugami");
	gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
	gtk_widget_show_all(window);
}
typedef struct {
	GtkApplicationWindow parent_instance;
	
	GtkWidget *message;
	GtkWidget *infobar;
	GtkWidget *status;
	GtkWidget *menutool;
	GMenuModel *toolmenu;
	GtkTextBuffer *buffer;
//	GtkWidget *dialog;

	int width;
	int height;
	gboolean maximized;
	gboolean fullscreen;
} ApplicationWindow;

int main(int argc, char *argv[]) {
	/* Connect to PostgreSQL */
	char conninfo[] = "dbname=atsugami"; /* Sets the database for dbconnect() */
	PGconn *conn = PQconnectdb(conninfo);

	GtkApplication *app;
	int status;
	app = gtk_application_new("ca.nvsd.atsugami", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	return status;
}
