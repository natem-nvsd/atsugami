/***********************************************\
* main.c: location of main() in Atsugami.	*
*						*
* TODO:						*
* - Re-write UI in C				*
* - Create new error banner when uuid-ossp is	*
* available/ENOENT				*
* - Add EXIF support				*
* - Fix incompatible pointers			*
*						*
* - URGENT: prevent SQL injection		*
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
	GtkWidget *window = user_data;

	PQfinish(conn);
	printf("Disconnected from Postgres.\n");
	gtk_widget_destroy(GTK_WIDGET(window));


}

/* Index of app.* actions */
static GActionEntry app_entries[] = {
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

	/* THis is in order of appearance */
	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *menu_bar;
//	GtkWidget *tool_bar;

	//GtkIconFactory *gif;
	GActionGroup *actions;
	//GtkAccelGroup *accel_group;

	gtk_init(&argc, &argv); /* Initialize GTK */

	/* Window definition */
	window = GTK_WIDGET(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	actions = (GActionGroup*)g_simple_action_group_new();
	g_action_map_add_action_entries(G_ACTION_MAP(actions), app_entries, G_N_ELEMENTS(app_entries), window);
	gtk_widget_insert_action_group(window, "app", actions);

	/* Create the virtical box */
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	/* Actions */
	GAction *import;
	GAction *quit;
	import = gtk_action_new("import", "Import", "Import an image", NULL);
	quit = gtk_action_new("quit", "Quit", "Quit the program", NULL);

	/* Menus (e.g. File) */
	GtkWidget *file_menu;
	GtkWidget *edit_menu;
	GtkWidget *tags_menu;
	GtkWidget *wiki_menu;
	GtkWidget *view_menu;
	GtkWidget *help_menu;

	/* Menu items (e.g. Quit) */
	GtkWidget *file_menu_item;
	GtkWidget *import_menu_item;
	GtkWidget *bulk_import_menu_item;
	GtkWidget *export_menu_item;
	GtkWidget *delete_menu_item;
	GtkWidget *quit_menu_item;

	GtkWidget *edit_menu_item;
	GtkWidget *edit_file_menu_item;
	GtkWidget *bulk_edit_menu_item;
	GtkWidget *settings_menu_item;

	GtkWidget *tags_menu_item;

	GtkWidget *wiki_menu_item;

	GtkWidget *view_menu_item;
	GtkWidget *view_extern_menu_item;
	GtkWidget *view_intern_menu_item;

	GtkWidget *help_menu_item;
	GtkWidget *help_menu_button;
	GtkWidget *about_menu_button;

	/* Create the menu bar */
	menu_bar = gtk_menu_bar_new();

	/* Create menus */
	file_menu = gtk_menu_new();
	edit_menu = gtk_menu_new();
	tags_menu = gtk_menu_new();
	wiki_menu = gtk_menu_new();
	view_menu = gtk_menu_new();
	help_menu = gtk_menu_new();

	/* Create menu items */
	file_menu_item = gtk_menu_item_new_with_label("File");
	import_menu_item = gtk_menu_item_new_with_label("Import");
	bulk_import_menu_item = gtk_menu_item_new_with_label("Bulk import");
	export_menu_item = gtk_menu_item_new_with_label("Export");
	delete_menu_item = gtk_menu_item_new_with_label("Delete");
	quit_menu_item = gtk_menu_item_new_with_label("Quit");

	edit_menu_item = gtk_menu_item_new_with_label("Edit");
	edit_file_menu_item = gtk_menu_item_new_with_label("Edit");
	bulk_edit_menu_item = gtk_menu_item_new_with_label("Bulk edit ");
	settings_menu_item = gtk_menu_item_new_with_label("Preferences");

	tags_menu_item = gtk_menu_item_new_with_label("Tags");

	wiki_menu_item = gtk_menu_item_new_with_label("Wiki");

	view_menu_item = gtk_menu_item_new_with_label("View");
	view_extern_menu_item = gtk_menu_item_new_with_label("Open in external viewer");
	view_intern_menu_item = gtk_menu_item_new_with_label("Open in internal viewer");

	help_menu_item = gtk_menu_item_new_with_label("Help");
	help_menu_button = gtk_menu_item_new_with_label("Help");
	about_menu_button = gtk_menu_item_new_with_label("About");

	/* File menu */
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_menu_item), file_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), import_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), bulk_import_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), export_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), delete_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), quit_menu_item);

	/* Edit menu */
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_menu_item), edit_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), edit_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), edit_file_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), bulk_edit_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), settings_menu_item);

	/* Tags menu */
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(tags_menu_item), tags_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), tags_menu_item);

	/* Wiki menu */
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(wiki_menu_item), wiki_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), wiki_menu_item);

	/* View menu */
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_menu_item), view_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), view_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), view_extern_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), view_intern_menu_item);

	/* Help menu */
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(help_menu_item), help_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), help_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), help_menu_button);
	gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), about_menu_button);
	gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);

	/* Warning info bar */
	GtkWidget *warn_widget, *warn_label, *warn_area, *warn_grid;
	GtkInfoBar *warn_bar;
	GtkImage *warn_image;

	warn_image = gtk_image_new_from_stock("gtk-dialog-warning", GTK_ICON_SIZE_DND);

	warn_widget = gtk_info_bar_new();
	warn_bar = GTK_INFO_BAR(warn_widget);
	warn_grid = gtk_grid_new();

	gtk_widget_set_no_show_all(warn_widget, FALSE);
	warn_label = gtk_label_new("");
	warn_area = gtk_info_bar_get_content_area(warn_bar);
	gtk_container_add(GTK_CONTAINER(warn_area), warn_image);
	gtk_container_add(GTK_CONTAINER(warn_area), warn_label);
	gtk_info_bar_set_revealed(warn_bar, FALSE);
	
	/* Add warn_bar to vbox */
	gtk_info_bar_set_message_type(warn_bar, GTK_MESSAGE_WARNING);
	gtk_widget_show(warn_bar);
	gtk_box_pack_start(GTK_BOX(vbox), warn_bar, FALSE, FALSE, 0);

	/* Error info bar */
	GtkWidget *error_widget, *error_label, *error_area, *error_grid;
	GtkInfoBar *error_bar;
	GtkImage *error_image;

	error_image = gtk_image_new_from_stock("gtk-dialog-error", GTK_ICON_SIZE_DND);

	/* Create the infobar */
	error_widget = gtk_info_bar_new();
	error_bar = GTK_INFO_BAR(error_widget);
	error_grid = gtk_grid_new();

	gtk_widget_set_no_show_all(error_widget, FALSE);
	error_label = gtk_label_new("");
	error_area = gtk_info_bar_get_content_area(error_bar);
	gtk_container_add(GTK_CONTAINER(error_area), error_image);
	gtk_container_add(GTK_CONTAINER(error_area), error_label);
	gtk_info_bar_set_revealed(error_bar, FALSE);

	/* Add error_bar to vbox */
	gtk_info_bar_set_message_type(error_bar, GTK_MESSAGE_ERROR);
	gtk_box_pack_start(GTK_BOX(vbox), error_bar, FALSE, FALSE, 0);

	/* Show error info bar if the connection fails */
	if (PQstatus(conn) != CONNECTION_OK) {
		//gtk_widget_show(error_bar);
		//gtk_widget_set_no_show_all(error_widget, FALSE);
		gtk_info_bar_set_revealed(error_bar, TRUE);

		//int n = sizeof(PQerrorMessage(conn));
		char errMsg[1024];
		sprintf(errMsg, "\n%s", PQerrorMessage(conn));
		fprintf(stderr, "Atsugami: \n%s", PQerrorMessage(conn));
		gtk_label_set_text(GTK_LABEL(error_label), errMsg);
	}

	/* Show window and vbox */
	gtk_widget_show_all(vbox);
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
