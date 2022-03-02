/* main.c (c) 2021-2022 by Nate Morrison */
#include "callbacks.h"
#include "main.h"
#include <ctype.h>
#include <errno.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdio.h>

PGconn *conn;
char conninfo[] = "dbname=atsugami";	/* Sets the database for dbconnect(); conn MUST be global */
GtkWidget *notebook, *vbox, *file_label, *window;

/* Quit function */
extern void quit_activate(void) {
	PQfinish(conn);
	printf("\nEXITING\n");
	exit(0);
}

int main(int argc, char *argv[]) {
	//FILE *db_file = fopen("~/.config/atsugami/dbname", "r");
	char *dbname;
	PGresult *mainres;
	GtkWidget *menu_bar, *toolbar;
	GActionGroup *actions;
	//GtkAccelGroup *accel_group;

	/* Parse the database name */

	gtk_init(&argc, &argv); /* Initialize GTK */

	/* Window definition */
	window = GTK_WIDGET(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	actions = (GActionGroup*)g_simple_action_group_new();
	conn = PQconnectdb(conninfo);	/* Connect to PostgreSQL */
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	/* Error Checking */
	if (PQstatus(conn) != CONNECTION_OK) {
		GtkWidget *diag;
		GtkDialogFlags diag_flags = GTK_RESPONSE_ACCEPT;

		diag = gtk_message_dialog_new(GTK_WINDOW(window), diag_flags, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Database error");

		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(diag), "Atsugami encountered a non-recoverable error with the database.\nAre you sure PostreSQL is installed or the database has been setup?");
		gtk_window_set_position(GTK_WINDOW(diag), GTK_WIN_POS_CENTER_ALWAYS);
		gtk_window_set_resizable(GTK_WINDOW(diag), FALSE);
		gtk_dialog_run(GTK_DIALOG(diag));
		gtk_widget_destroy(diag);
		PQfinish(conn);
		return 1;
	}

	mainres = PQexec(conn, "SELECT * FROM public.settings;");

	if (PQgetvalue(mainres, 0, 0) == NULL || PQgetvalue(mainres, 0, 1) == NULL || PQgetvalue(mainres, 0, 2) == NULL || PQgetvalue(mainres, 0, 3) == NULL) {
		GtkWidget *diag;
		GtkDialogFlags diag_flags = GTK_RESPONSE_ACCEPT;

		diag = gtk_message_dialog_new(GTK_WINDOW(window), diag_flags, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Configuration error");

		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(diag), "Atsugami could not retrive configuration settings from the database.\nBacktrace:\n\tStorage directory: %s\n\tThumbnail directory: %s\n\tLast directory: %s\n\tThumbnail size: %s", PQgetvalue(mainres, 0, 0), PQgetvalue(mainres, 0, 1), PQgetvalue(mainres, 0, 2), PQgetvalue(mainres, 0, 3));
		gtk_window_set_position(GTK_WINDOW(diag), GTK_WIN_POS_CENTER_ALWAYS);
		gtk_window_set_resizable(GTK_WINDOW(diag), FALSE);
		gtk_dialog_run(GTK_DIALOG(diag));
		gtk_widget_destroy(diag);
		PQclear(mainres);
		PQfinish(conn);

		return 1;
	}
	/* End error checking */

	g_signal_connect(window, "destroy", G_CALLBACK(quit_activate), NULL);
	//g_action_map_add_action_entries(G_ACTION_MAP(actions), app_entries, G_N_ELEMENTS(app_entries), window);
	gtk_widget_insert_action_group(window, "app", actions);
	gtk_window_set_default_size(GTK_WINDOW(window), -1, -1);

	/* Create vbox */
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	/* keyboard shortcuts */
	//accel_group = gtk_accel_group_new();

	/* Actions */
	/*
	GAction *import;
	GAction *new_artist;
	GAction *new_copyright;
	import = gtk_action_new("import", "Import", "Import an image", NULL);
	new_artist = gtk_action_new("artist", "New artist", "Add a new artist to Atsugami", NULL);
	new_copyright = gtk_action_new("copyright", "New copyright", "Add a new copyright to Atsugami", NULL);
	*/

	/* Menus (e.g. File) */
	GtkWidget *file_menu, *edit_menu, *tags_menu, *wiki_menu, *view_menu, *help_menu;

	/* Menu items (e.g. Quit) */
	// Column 0
	GtkWidget *file_menu_item, *import_menu_item, *bulk_import_menu_item, *export_menu_item, *delete_menu_item, *quit_menu_item;

	// Column 1
	GtkWidget *edit_menu_item, *edit_file_menu_item, *bulk_edit_menu_item, *settings_menu_item;

	// Column 2
	GtkWidget *tags_menu_item, *new_artist_menu_item, *new_copyright_menu_item, *new_character_menu_item, *new_tag_menu_item, *new_meta_tag_menu_item;

	// Column 3
	GtkWidget *wiki_menu_item;

	// Column 4
	GtkWidget *view_menu_item, *view_extern_menu_item, *view_intern_menu_item;

	// Column 5
	GtkWidget *help_menu_item, *help_menu_button, *console_menu_item, *about_menu_item;

	/* Toolbar */
	GtkToolItem *import_button, *bulk_import_button, *edit_button, *favourite_button, *view_button, *wiki_button, *home_button;

	/* Toolbar images */
	GtkWidget *import_image, *bulk_import_image, *edit_image, *favourite_image, *view_image, *wiki_image, *home_image;

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
	// File
	file_menu_item = gtk_menu_item_new_with_label("File");
	import_menu_item = gtk_menu_item_new_with_label("Import");
	bulk_import_menu_item = gtk_menu_item_new_with_label("Bulk import");
	export_menu_item = gtk_menu_item_new_with_label("Export");
	delete_menu_item = gtk_menu_item_new_with_label("Delete");
	quit_menu_item = gtk_menu_item_new_with_label("Quit");

	// Edit
	edit_menu_item = gtk_menu_item_new_with_label("Edit");
	edit_file_menu_item = gtk_menu_item_new_with_label("Edit");
	bulk_edit_menu_item = gtk_menu_item_new_with_label("Bulk edit ");
	settings_menu_item = gtk_menu_item_new_with_label("Preferences");

	// Tags
	tags_menu_item = gtk_menu_item_new_with_label("Tags");
	new_artist_menu_item = gtk_menu_item_new_with_label("New artist");
	new_copyright_menu_item = gtk_menu_item_new_with_label("New copyright");
	new_character_menu_item = gtk_menu_item_new_with_label("New character");
	new_tag_menu_item = gtk_menu_item_new_with_label("New tag");
	new_meta_tag_menu_item = gtk_menu_item_new_with_label("New meta tag");

	// Wikis
	wiki_menu_item = gtk_menu_item_new_with_label("Wiki");

	// View
	view_menu_item = gtk_menu_item_new_with_label("View");
	view_extern_menu_item = gtk_menu_item_new_with_label("Open in external viewer");
	view_intern_menu_item = gtk_menu_item_new_with_label("Open in internal viewer");

	// Help
	help_menu_item = gtk_menu_item_new_with_label("Help");
	help_menu_button = gtk_menu_item_new_with_label("Help");
	console_menu_item = gtk_menu_item_new_with_label("Console");
	about_menu_item = gtk_menu_item_new_with_label("About");

	/* File menu */
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_menu_item), file_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), import_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), bulk_import_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), export_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), delete_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), quit_menu_item);

	g_signal_connect(import_menu_item, "activate", G_CALLBACK(import_activate), NULL);
	g_signal_connect(quit_menu_item, "activate", G_CALLBACK(quit_activate), NULL);

	/* Edit menu */
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_menu_item), edit_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), edit_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), edit_file_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), bulk_edit_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), settings_menu_item);

	/* Tags menu */
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(tags_menu_item), tags_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), tags_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(tags_menu), new_artist_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(tags_menu), new_copyright_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(tags_menu), new_character_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(tags_menu), new_tag_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(tags_menu), new_meta_tag_menu_item);

	g_signal_connect(new_artist_menu_item, "activate", G_CALLBACK(new_artist_activate), NULL);
	g_signal_connect(new_copyright_menu_item, "activate", G_CALLBACK(new_copyright_activate), NULL);
	g_signal_connect(new_character_menu_item, "activate", G_CALLBACK(new_character_activate), NULL);
	g_signal_connect(new_tag_menu_item, "activate", G_CALLBACK(new_tag_activate), NULL);
	g_signal_connect(new_meta_tag_menu_item, "activate", G_CALLBACK(new_meta_tag_activate), NULL);

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
	gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), console_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), about_menu_item);
	gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);

	g_signal_connect(console_menu_item, "activate", G_CALLBACK(console), NULL);
	g_signal_connect(about_menu_item, "activate", G_CALLBACK(about_activate), NULL);

	/* Toolbar */
	toolbar = gtk_toolbar_new();

	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_BOTH_HORIZ);
	gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);

	/* Icons */
	import_image = gtk_image_new_from_icon_name("gtk-open", GTK_ICON_SIZE_LARGE_TOOLBAR);
	bulk_import_image = gtk_image_new_from_icon_name("gtk-dnd-multiple", GTK_ICON_SIZE_LARGE_TOOLBAR);
	edit_image = gtk_image_new_from_icon_name("gtk-edit", GTK_ICON_SIZE_LARGE_TOOLBAR);
	view_image = gtk_image_new_from_icon_name("image-x-generic", GTK_ICON_SIZE_LARGE_TOOLBAR);
	favourite_image = gtk_image_new_from_icon_name("emblem-favorite", GTK_ICON_SIZE_LARGE_TOOLBAR);
	wiki_image = gtk_image_new_from_icon_name("gtk-file", GTK_ICON_SIZE_LARGE_TOOLBAR);
	home_image = gtk_image_new_from_icon_name("go-home", GTK_ICON_SIZE_LARGE_TOOLBAR);

	/* Widgets */
	import_button = gtk_tool_button_new(import_image, NULL);
	bulk_import_button = gtk_tool_button_new(bulk_import_image, NULL);
	edit_button = gtk_tool_button_new(edit_image, NULL);
	favourite_button = gtk_tool_button_new(favourite_image, NULL);
	view_button = gtk_tool_button_new(view_image, NULL);
	wiki_button = gtk_tool_button_new(wiki_image, NULL);
	home_button = gtk_tool_button_new(home_image, NULL);		/* replace this with a toggle button for safe mode; */

	/* Tooltips */
	gtk_widget_set_tooltip_text(GTK_WIDGET(import_button), "Import an image to Atsugami");
	gtk_widget_set_tooltip_text(GTK_WIDGET(bulk_import_button), "Import multiple images to Atsugami");
	gtk_widget_set_tooltip_text(GTK_WIDGET(edit_button), "Edit the tags of the selected image");
	gtk_widget_set_tooltip_text(GTK_WIDGET(favourite_button), "Add the selected image to your favourites");
	gtk_widget_set_tooltip_text(GTK_WIDGET(view_button), "Open the selected image(s) in an external viewer");
	gtk_widget_set_tooltip_text(GTK_WIDGET(wiki_button), "Open the wiki");
	gtk_widget_set_tooltip_text(GTK_WIDGET(home_button), "Go home");

	/* Add widgets to the toolbar */
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(import_button), 0);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(bulk_import_button), 1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(edit_button), 2);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(favourite_button), 3);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(view_button), 4);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(wiki_button), 5);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(home_button), 6);
	//gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(he_will_not_divide_us), 7);

	/* Toolbar callbacks */
	g_signal_connect(import_button, "clicked", G_CALLBACK(import_activate), NULL);
//	g_signal_connect(bulk_import_button, "clicked", G_CALLBACK(import_activate), NULL);
//	g_signal_connect(edit_button, "clicked", G_CALLBACK(NULL), NULL);
//	g_signal_connect(favourite_button, "clicked", G_CALLBACK(NULL), NULL);
//	g_signal_connect(view_button, "clicked", G_CALLBACK(NULL), NULL);
//	g_signal_connect(wiki_button, "clicked", G_CALLBACK(NULL), NULL);
	g_signal_connect(home_button, "clicked", G_CALLBACK(tab), NULL);
	
	/* Warning info bar */
	GtkWidget *warn_widget, *warn_label, *warn_area, *warn_bar, *warn_image;

	warn_image = gtk_image_new_from_icon_name("gtk-dialog-warning", GTK_ICON_SIZE_DND);
	warn_widget = gtk_info_bar_new();
	warn_bar = warn_widget;

	gtk_widget_set_no_show_all(warn_widget, FALSE);

	warn_label = gtk_label_new("");
	warn_area = gtk_info_bar_get_content_area(GTK_INFO_BAR(warn_bar));

	gtk_container_add(GTK_CONTAINER(warn_area), warn_image);
	gtk_container_add(GTK_CONTAINER(warn_area), warn_label);
	gtk_info_bar_set_revealed(GTK_INFO_BAR(warn_bar), FALSE);
	
	/* Add warn_bar to vbox */
	gtk_info_bar_set_message_type(GTK_INFO_BAR(warn_bar), GTK_MESSAGE_WARNING);
	gtk_widget_show(warn_bar);
	gtk_box_pack_start(GTK_BOX(vbox), warn_bar, FALSE, FALSE, 0);

	/* Error info bar */
	GtkWidget *error_widget, *error_label, *error_area, *error_bar, *error_image;

	error_image = gtk_image_new_from_icon_name("gtk-dialog-error", GTK_ICON_SIZE_DND);

	/* Create the infobar */
	error_widget = gtk_info_bar_new();
	error_bar = error_widget;

	gtk_widget_set_no_show_all(error_widget, FALSE);

	error_label = gtk_label_new("");
	error_area = gtk_info_bar_get_content_area(GTK_INFO_BAR(error_bar));

	gtk_container_add(GTK_CONTAINER(error_area), GTK_WIDGET(error_image));
	gtk_container_add(GTK_CONTAINER(error_area), error_label);
	gtk_info_bar_set_revealed(GTK_INFO_BAR(error_bar), FALSE);

	/* Add error_bar to vbox */
	gtk_info_bar_set_message_type(GTK_INFO_BAR(error_bar), GTK_MESSAGE_ERROR);

	/* Show error info bar if the connection fails */
	if (PQstatus(conn) != CONNECTION_OK) {
		char *errMsg = NULL;

		gtk_info_bar_set_revealed(GTK_INFO_BAR(error_bar), TRUE);
		sprintf(errMsg, "\n%s", PQerrorMessage(conn));
		fprintf(stderr, "Atsugami: %s", PQerrorMessage(conn));
		gtk_label_set_text(GTK_LABEL(error_label), errMsg);
	}

	/* Tab bar */
	notebook = gtk_notebook_new();
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
	gtk_container_add(GTK_CONTAINER(vbox), GTK_WIDGET(notebook));
	tab();

	/* File count */
	file_label = gtk_label_new(NULL);
	file_count();

	/* Show window and vbox */
	gtk_window_set_title(GTK_WINDOW(window), "Atsugami");
	gtk_widget_show_all(vbox);
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
