/***********************************************\
* main.c: location of main() in Atsugami.	*
*						*
* TODO:						*
* - Re-write UI in C | rewrite About dialog	*
* - Create new error banner when uuid-ossp is	*
* ENOENT					*	
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
	/* This causes a seg fault when used as a callback */
	GtkWidget *window = user_data;

	PQfinish(conn);
	gtk_widget_destroy(window);
}

/* Index of app.* actions */
static GActionEntry app_entries[] = {
	{ "quit",   quit_activate,   NULL, NULL, NULL },
	{ "import", import_activate, NULL, NULL, NULL },
//	{ "import_bulk", import_bulk_activate, NULL, NULL, NULL },
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
	GtkWidget *toolbar;

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
	GtkWidget *about_menu_item;

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
	about_menu_item = gtk_menu_item_new_with_label("About");

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
	gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), about_menu_item);
	gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);

	/* Menu bar callbacks */
	g_signal_connect(import_menu_item, "activate", G_CALLBACK(import_activate), NULL);
	g_signal_connect(quit_menu_item,   "activate", G_CALLBACK(quit_activate), NULL);

	g_signal_connect(about_menu_item, "activate", G_CALLBACK(about_activate), NULL);
	//g_signal_connect(G_OBJECT(import_menu_item), "activate", G_CALLBACK(import_activate), NULL);

	/* Toolbar */
	GtkWidget *import_button;
	GtkWidget *bulk_import_button;
	GtkWidget *edit_button;
	GtkWidget *view_button;
	GtkWidget *wiki_button;
	GtkWidget *quit_button; 
	GtkWidget *he_will_not_divide_us;
	GtkWidget *search_by_tag;
	GtkWidget *search_wiki;
	GtkWidget *search_tag_wrapper;
	GtkWidget *search_wiki_wrapper;

	GtkImage *import_image;
	GtkImage *bulk_import_image;
	GtkImage *edit_image;
	GtkImage *view_image;
	GtkImage *wiki_image;
	GtkImage *quit_image;

	toolbar = gtk_toolbar_new();
	gtk_toolbar_set_style(toolbar, GTK_TOOLBAR_BOTH_HORIZ);
	gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);

	/* Icons */
	import_image = gtk_image_new_from_stock("gtk-open", GTK_ICON_SIZE_LARGE_TOOLBAR);
	bulk_import_image = gtk_image_new_from_stock("gtk-dnd-multiple", GTK_ICON_SIZE_LARGE_TOOLBAR);
	edit_image = gtk_image_new_from_icon_name("gtk-edit", GTK_ICON_SIZE_LARGE_TOOLBAR);
	view_image = gtk_image_new_from_icon_name("image-x-generic", GTK_ICON_SIZE_LARGE_TOOLBAR);
	wiki_image = gtk_image_new_from_stock("gtk-file", GTK_ICON_SIZE_LARGE_TOOLBAR);
	quit_image = gtk_image_new_from_stock("gtk-quit", GTK_ICON_SIZE_LARGE_TOOLBAR);

	/* Widgets */
	import_button = gtk_tool_button_new(import_image, NULL);
	bulk_import_button = gtk_tool_button_new(bulk_import_image, NULL);
	edit_button = gtk_tool_button_new(edit_image, NULL);
	view_button = gtk_tool_button_new(view_image, NULL);
	wiki_button = gtk_tool_button_new(wiki_image, NULL);
	quit_button = gtk_tool_button_new(quit_image, NULL);
	he_will_not_divide_us = gtk_separator_tool_item_new();

	/* Search bars */
	search_by_tag = gtk_search_entry_new();
	search_wiki = gtk_search_entry_new();
	search_tag_wrapper = gtk_tool_item_new();
	search_wiki_wrapper = gtk_tool_item_new();

	gtk_container_add(GTK_CONTAINER(search_tag_wrapper), GTK_WIDGET(search_by_tag));
	gtk_container_add(GTK_CONTAINER(search_wiki_wrapper), GTK_WIDGET(search_wiki));

	/* Add widgets to the toolbar */
	gtk_toolbar_insert(toolbar, import_button, 0);
	gtk_toolbar_insert(toolbar, bulk_import_button, 1);
	gtk_toolbar_insert(toolbar, edit_button, 2);
	gtk_toolbar_insert(toolbar, view_button, 3);
	gtk_toolbar_insert(toolbar, wiki_button, 4);
	gtk_toolbar_insert(toolbar, quit_button, 5);
	gtk_toolbar_insert(toolbar, he_will_not_divide_us, 6);
	gtk_toolbar_insert(toolbar, search_tag_wrapper, 7);
	gtk_toolbar_insert(toolbar, search_wiki_wrapper, 8);

	/* Toolbar callbacks */
	g_signal_connect(import_button, "clicked", G_CALLBACK(import_activate), NULL);
	g_signal_connect(bulk_import_button, "clicked", G_CALLBACK(import_activate), NULL);
	g_signal_connect(edit_button, "clicked", G_CALLBACK(NULL), NULL);
	g_signal_connect(view_button, "clicked", G_CALLBACK(NULL), NULL);
	g_signal_connect(wiki_button, "clicked", G_CALLBACK(NULL), NULL);
	g_signal_connect(quit_button, "clicked", G_CALLBACK(gtk_main_quit), NULL); /* segfault when quit_activate called */

	/* Warning info bar */
	GtkWidget *warn_widget, *warn_label, *warn_area, *warn_grid;
	GtkInfoBar *warn_bar;
	GtkImage *warn_image;

	warn_image = gtk_image_new_from_stock("gtk-dialog-warning", GTK_ICON_SIZE_DIALOG);

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
		fprintf(stderr, "Atsugami: %s", PQerrorMessage(conn));
		gtk_label_set_text(GTK_LABEL(error_label), errMsg);
	}

	/* Show window and vbox */
	gtk_widget_show_all(vbox);
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
