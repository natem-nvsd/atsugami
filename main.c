/* main.c (c) 2021-2022 by Nate Morrison */
#include "atsugami.h"
#include <ctype.h>
#include <errno.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdio.h>
#include <stdbool.h>

PGconn *conn;
int msg_id;
bool safe_mode;
char *conninfo;
GtkWidget *notebook, *vbox, *window, *status_bar;
GtkWidget *about_menu_item, *bulk_edit_menu_item, *bulk_import_image, *bulk_import_menu_item, *console_menu_item, *delete_menu_item, *edit_file_menu_item, *edit_image, *edit_menu, *edit_menu_item, *error_area, *error_bar, *error_image, *error_label, *error_widget, *export_menu_item, *favourite_image, *file_menu, *file_menu_item, *gigantic_icons_menu_item, *help_menu, *help_menu_button, *help_menu_item, *home_image, *huge_icons_menu_item, *image_menu, *image_menu_item, *img_info_menu_item, *import_image, *import_menu_item, *large_icons_menu_item, *medium_icons_menu_item, *menu_bar, *new_tag_menu_item, *new_wiki_page, *next_tab, *open_img_extern_menu_item, *open_img_menu_item, *open_img_new_tab_menu_item, *open_wiki_menu_item, *prev_tab, *quit_menu_item, *safe_mode_menu_item, *settings_menu_item, *small_icons_menu_item, *tags_menu, *tags_menu_item, *thumb_size_menu, *thumb_size_menu_item, *toolbar, *view_menu, *view_menu_item, *warn_area, *warn_bar, *warn_image, *warn_label, *warn_widget, *wiki_image, *wiki_menu, *wiki_menu_item;
GtkAccelGroup *accel;

int main(int argc, char *argv[]) {
	//FILE *logfile;
	GtkToolItem *import_button, *bulk_import_button, *edit_button, *favourite_button, *wiki_button, *home_button;
	GActionGroup *actions;
	PGresult *main_res;
	char *db_name;

	/** Postgres setup **/
	/* User name */
	char *user_name = getenv("USER");

	/* dbname */
	db_name = (char *) malloc(strlen(user_name) + 21);
	sprintf(db_name, "dbname=%s_atsugami", user_name);

	/* db_name */
	if (user_name == NULL) {
		fprintf(stderr, "%s: Your $USER environment variable has not been set.\n", argv[0]);
		exit(1);
	}

	conninfo = db_name;
	conn = PQconnectdb(conninfo);	/* Connect to PostgreSQL */

	/* Logging */

	/* Safe Mode */
	main_res = PQexec(conn, "SELECT safe_mode FROM public.settings;");
	if (strcmp(PQgetvalue(main_res, 0, 0), "f") == 0)
		safe_mode = false;
	else
		safe_mode = true;

	PQclear(main_res);

	/** Window definition **/
	gtk_init(&argc, &argv); /* Initialize GTK */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	actions = (GActionGroup*)g_simple_action_group_new();
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	accel = gtk_accel_group_new();
	notebook = gtk_notebook_new();

	/* Window setup */
	g_signal_connect(window, "destroy", G_CALLBACK(quit_activate), NULL);
	//g_action_map_add_action_entries(G_ACTION_MAP(actions), app_entries, G_N_ELEMENTS(app_entries), window);
	gtk_widget_insert_action_group(window, "app", actions);
	gtk_window_add_accel_group(GTK_WINDOW(window), accel);
	gtk_widget_set_hexpand(window, TRUE);
	gtk_widget_set_vexpand(window, TRUE);

	/* Create vbox */
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	/* Menu bar buttons */
	menu_bar = gtk_menu_bar_new();
	file_menu = gtk_menu_new();
	edit_menu = gtk_menu_new();
	image_menu = gtk_menu_new();
	tags_menu = gtk_menu_new();
	wiki_menu = gtk_menu_new();
	view_menu = gtk_menu_new();
	help_menu = gtk_menu_new();

	/* File menu */
	file_menu_item = gtk_menu_item_new_with_mnemonic("_File");
	import_menu_item = gtk_menu_item_new_with_mnemonic("_Import");
	bulk_import_menu_item = gtk_menu_item_new_with_mnemonic("_Bulk import");
	export_menu_item = gtk_menu_item_new_with_mnemonic("_Export");
	delete_menu_item = gtk_menu_item_new_with_mnemonic("_Delete");
	quit_menu_item = gtk_menu_item_new_with_mnemonic("_Quit");

	/* Edit menu */
	edit_menu_item = gtk_menu_item_new_with_mnemonic("_Edit");
	edit_file_menu_item = gtk_menu_item_new_with_mnemonic("_Edit");
	bulk_edit_menu_item = gtk_menu_item_new_with_mnemonic("_Bulk edit ");
	settings_menu_item = gtk_menu_item_new_with_mnemonic("_Preferences");

	/* Image menu */
	image_menu_item = gtk_menu_item_new_with_mnemonic("_Image");
	open_img_menu_item = gtk_menu_item_new_with_mnemonic("_Open image");
	open_img_new_tab_menu_item = gtk_menu_item_new_with_mnemonic("Open _image in new tab");
	open_img_extern_menu_item = gtk_menu_item_new_with_mnemonic("Open image in _external viewer");
	img_info_menu_item = gtk_menu_item_new_with_mnemonic("Image _properties");

	/* Tags menu */
	tags_menu_item = gtk_menu_item_new_with_mnemonic("_Tags");
	new_tag_menu_item = gtk_menu_item_new_with_mnemonic("New _tag");

	/* Wiki menu */
	wiki_menu_item = gtk_menu_item_new_with_mnemonic("_Wiki");
	open_wiki_menu_item = gtk_menu_item_new_with_mnemonic("_Open wiki home");
	new_wiki_page = gtk_menu_item_new_with_mnemonic("_New wiki page");

	/* View menu */
	view_menu_item = gtk_menu_item_new_with_mnemonic("_View");
	safe_mode_menu_item = gtk_check_menu_item_new_with_mnemonic("_Safe Mode");
	thumb_size_menu = gtk_menu_new();
	thumb_size_menu_item = gtk_check_menu_item_new_with_mnemonic("_Icon size");
	small_icons_menu_item = gtk_check_menu_item_new_with_mnemonic("_Small");
	medium_icons_menu_item = gtk_check_menu_item_new_with_mnemonic("_Medium");
	large_icons_menu_item = gtk_check_menu_item_new_with_mnemonic("_Large");
	huge_icons_menu_item = gtk_check_menu_item_new_with_mnemonic("_Huge");
	gigantic_icons_menu_item = gtk_check_menu_item_new_with_mnemonic("_Gigantic");

	/* Help menu */
	help_menu_item = gtk_menu_item_new_with_mnemonic("_Help");
	help_menu_button = gtk_menu_item_new_with_mnemonic("_Help");
	console_menu_item = gtk_menu_item_new_with_mnemonic("_Console");
	about_menu_item = gtk_menu_item_new_with_mnemonic("_About");

	/* Toolbar */
	toolbar = gtk_toolbar_new();
	import_image = gtk_image_new_from_icon_name("document-open", GTK_ICON_SIZE_LARGE_TOOLBAR);
	bulk_import_image = gtk_image_new_from_icon_name("emblem-photos", GTK_ICON_SIZE_LARGE_TOOLBAR);
	edit_image = gtk_image_new_from_icon_name("gtk-edit", GTK_ICON_SIZE_LARGE_TOOLBAR);
	favourite_image = gtk_image_new_from_icon_name("emblem-favorite", GTK_ICON_SIZE_LARGE_TOOLBAR);
	wiki_image = gtk_image_new_from_icon_name("text-x-generic", GTK_ICON_SIZE_LARGE_TOOLBAR);
	home_image = gtk_image_new_from_icon_name("tab-new", GTK_ICON_SIZE_LARGE_TOOLBAR);

	import_button = gtk_tool_button_new(import_image, NULL);
	bulk_import_button = gtk_tool_button_new(bulk_import_image, NULL);
	edit_button = gtk_tool_button_new(edit_image, NULL);
	favourite_button = gtk_tool_button_new(favourite_image, NULL);
	wiki_button = gtk_tool_button_new(wiki_image, NULL);
	home_button = gtk_tool_button_new(home_image, NULL);		/* replace this with a toggle button for safe mode; */

	warn_image = gtk_image_new_from_icon_name("gtk-dialog-warning", GTK_ICON_SIZE_DND);
	warn_widget = gtk_info_bar_new();
	warn_bar = warn_widget;

	warn_label = gtk_label_new("");
	warn_area = gtk_info_bar_get_content_area(GTK_INFO_BAR(warn_bar));

	error_image = gtk_image_new_from_icon_name("gtk-dialog-error", GTK_ICON_SIZE_DND);
	error_widget = gtk_info_bar_new();
	error_bar = error_widget;
	error_label = gtk_label_new("");
	error_area = gtk_info_bar_get_content_area(GTK_INFO_BAR(error_bar));

	status_bar = gtk_statusbar_new();

	/* Menus (e.g. File) */
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_menu_item), file_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), import_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), bulk_import_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), export_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), delete_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), quit_menu_item);

	g_signal_connect(import_menu_item, "activate", G_CALLBACK(import_activate), NULL);
	g_signal_connect(quit_menu_item, "activate", G_CALLBACK(quit_activate), NULL);

	gtk_widget_add_accelerator(import_menu_item, "activate", accel, GDK_KEY_i, GDK_CONTROL_MASK, GTK_ACCEL_LOCKED);
	gtk_widget_add_accelerator(import_menu_item, "activate", accel, GDK_KEY_o, GDK_CONTROL_MASK, GTK_ACCEL_LOCKED);

	/** Edit **/
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_menu_item), edit_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), edit_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), edit_file_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), bulk_edit_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), settings_menu_item);

	/** Image **/
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(image_menu_item), image_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), image_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(image_menu), open_img_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(image_menu), open_img_new_tab_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(image_menu), open_img_extern_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(image_menu), img_info_menu_item);

	/** Tags **/
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(tags_menu_item), tags_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), tags_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(tags_menu), new_tag_menu_item);
	g_signal_connect(new_tag_menu_item, "activate", G_CALLBACK(new_tag_activate), NULL);

	/** Wikis **/
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(wiki_menu_item), wiki_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), wiki_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(wiki_menu), open_wiki_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(wiki_menu), new_wiki_page);

	g_signal_connect(open_wiki_menu_item, "activate", G_CALLBACK(wiki), NULL);

	/** View **/
	/* Icon size submenu */
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_menu_item), view_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), view_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), safe_mode_menu_item);

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(thumb_size_menu_item), thumb_size_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), thumb_size_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(thumb_size_menu), small_icons_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(thumb_size_menu), medium_icons_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(thumb_size_menu), large_icons_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(thumb_size_menu), huge_icons_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(thumb_size_menu), gigantic_icons_menu_item);

	/* Set the state of the menu items */
	if (safe_mode == true)
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(safe_mode_menu_item), TRUE);
	else
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(safe_mode_menu_item), FALSE);

	set_icon_menu_items();

	g_signal_connect(safe_mode_menu_item, "activate", G_CALLBACK(safe_mode_toggle), NULL);
	g_signal_connect(small_icons_menu_item, "activate", G_CALLBACK(set_icon_size), 0);
	g_signal_connect(medium_icons_menu_item, "activate", G_CALLBACK(set_icon_size), (void *) 1);
	g_signal_connect(large_icons_menu_item, "activate", G_CALLBACK(set_icon_size), (void *) 2);
	g_signal_connect(huge_icons_menu_item, "activate", G_CALLBACK(set_icon_size), (void *) 3);
	g_signal_connect(gigantic_icons_menu_item, "activate", G_CALLBACK(set_icon_size), (void *) 4);

	/** Help **/
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(help_menu_item), help_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), help_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), help_menu_button);
	gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), console_menu_item);
	gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), about_menu_item);
	gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);

	g_signal_connect(console_menu_item, "activate", G_CALLBACK(console), NULL);
	g_signal_connect(about_menu_item, "activate", G_CALLBACK(about_activate), NULL);

	/* Toolbar */
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_BOTH_HORIZ);
	gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);

	/* Icons */
	/* Tooltips */
	gtk_widget_set_tooltip_text(GTK_WIDGET(import_button), "Import an image to Atsugami");
	gtk_widget_set_tooltip_text(GTK_WIDGET(bulk_import_button), "Import multiple images to Atsugami");
	gtk_widget_set_tooltip_text(GTK_WIDGET(edit_button), "Edit the tags of the selected image");
	gtk_widget_set_tooltip_text(GTK_WIDGET(favourite_button), "Add the selected image to your favourites");
	gtk_widget_set_tooltip_text(GTK_WIDGET(wiki_button), "Open the wiki");
	gtk_widget_set_tooltip_text(GTK_WIDGET(home_button), "Go home");

	/* Add widgets to the toolbar */
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(import_button), 0);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(bulk_import_button), 1);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(edit_button), 2);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(favourite_button), 3);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(wiki_button), 5);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM(home_button), 6);

	/* Toolbar callbacks */
	g_signal_connect(import_button, "clicked", G_CALLBACK(import_activate), NULL);
	//g_signal_connect(bulk_import_button, "clicked", G_CALLBACK(NULL), NULL);
	//g_signal_connect(edit_button, "clicked", G_CALLBACK(NULL), NULL);
	//g_signal_connect(favourite_button, "clicked", G_CALLBACK(NULL), NULL);
	g_signal_connect(wiki_button, "clicked", G_CALLBACK(wiki), NULL);
	g_signal_connect(home_button, "clicked", G_CALLBACK(tab), NULL);
	
	/* Warning info bar */
	gtk_widget_set_no_show_all(warn_widget, FALSE);

	gtk_container_add(GTK_CONTAINER(warn_area), warn_image);
	gtk_container_add(GTK_CONTAINER(warn_area), warn_label);
	gtk_info_bar_set_revealed(GTK_INFO_BAR(warn_bar), FALSE);
	
	/* Add warn_bar to vbox */
	gtk_info_bar_set_message_type(GTK_INFO_BAR(warn_bar), GTK_MESSAGE_WARNING);
	gtk_widget_show(warn_bar);
	gtk_box_pack_start(GTK_BOX(vbox), warn_bar, FALSE, FALSE, 0);

	gtk_widget_set_no_show_all(error_widget, FALSE);
	gtk_container_add(GTK_CONTAINER(error_area), GTK_WIDGET(error_image));
	gtk_container_add(GTK_CONTAINER(error_area), error_label);
	gtk_info_bar_set_revealed(GTK_INFO_BAR(error_bar), FALSE);
	gtk_box_pack_start(GTK_BOX(vbox), error_bar, FALSE, FALSE, 0);

	/* Add error_bar to vbox */
	gtk_info_bar_set_message_type(GTK_INFO_BAR(error_bar), GTK_MESSAGE_ERROR);

	/* Show error info bar if the connection fails */
	gtk_box_pack_end(GTK_BOX(vbox), status_bar, FALSE, FALSE, 0);
	if (PQstatus(conn) != CONNECTION_OK) {
		char *errMsg;

		errMsg = (char *) malloc(strlen(PQerrorMessage(conn)) + 1);
		sprintf(errMsg, "\n%s", PQerrorMessage(conn));

		gtk_info_bar_set_revealed(GTK_INFO_BAR(error_bar), TRUE);
		fprintf(stderr, "Atsugami: %s", PQerrorMessage(conn));

		gtk_label_set_text(GTK_LABEL(error_label), errMsg);
	} else {
		tab();
		file_count();
	}

	/* Gtk Notebook */
	// FIXME: ctrl+tab should go to the next tab, and ctrl+shift+tab should go to the previous tab.
	prev_tab = gtk_button_new();
	next_tab = gtk_button_new();

	gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), TRUE);
	gtk_widget_set_hexpand(notebook, TRUE);

	gtk_container_add(GTK_CONTAINER(vbox), GTK_WIDGET(notebook));
	gtk_notebook_set_action_widget(GTK_NOTEBOOK(notebook), prev_tab, GTK_PACK_START);
	gtk_notebook_set_action_widget(GTK_NOTEBOOK(notebook), next_tab, GTK_PACK_END);

	gtk_widget_add_accelerator(prev_tab, "clicked", accel, GDK_KEY_Tab, GDK_CONTROL_MASK|GDK_SHIFT_MASK, GTK_ACCEL_LOCKED);
	gtk_widget_add_accelerator(next_tab, "clicked", accel, GDK_KEY_Tab, GDK_CONTROL_MASK, GTK_ACCEL_LOCKED);
	g_signal_connect(prev_tab, "clicked", G_CALLBACK(prev_tab_cb), GTK_NOTEBOOK(notebook));
	g_signal_connect(next_tab, "clicked", G_CALLBACK(next_tab_cb), GTK_NOTEBOOK(notebook));

	/* Show window and vbox */
	gtk_window_set_title(GTK_WINDOW(window), "Atsugami");
	gtk_widget_show_all(vbox);
	gtk_widget_show_all(window);

	gtk_main();
	return 0;
}
