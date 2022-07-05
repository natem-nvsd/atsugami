/* main.c
 * Copyright (c) 2021-2022 by Nate Morrison.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "atsugami.h"
#include <ctype.h>
#include <errno.h>
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

GtkWidget *notebook;
GtkWidget *status_bar;
GtkWidget *small_mi;
GtkWidget *medium_mi;
GtkWidget *large_mi;
GtkWidget *huge_mi;
GtkWidget *gigantic_mi;
GtkWidget *safe_mode_mi;
PGconn *conn;
bool safe_mode;
bool dark_theme;
int msg_id;

int main(int argc, char **argv) {
	PGresult *main_res;

	/** Connect to the database **/
	char *user_name = getenv("USER");
	char *conninfo;
	char *db_name;

	/* dbname */
	db_name = (char *) malloc(strlen(user_name) + 21);
	sprintf(db_name, "dbname=%s_atsugami", user_name);
	if (user_name == NULL) {
		fprintf(stderr, "%s: Your $USER environment variable has not been set.\n", argv[0]);
		exit(1);
	}

	conninfo = db_name;
	conn = PQconnectdb(conninfo);

	/* Safe Mode */
	main_res = PQexec(conn, "SELECT safe_mode FROM public.settings;");
	if (strcmp(PQgetvalue(main_res, 0, 0), "f") == 0)
		safe_mode = false;
	else
		safe_mode = true;
	PQclear(main_res);

	gtk_init(&argc, &argv);
	g_object_get(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", &dark_theme, NULL);

	/** Window **/
	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *menu_bar;
	GtkWidget *new_tab_btn;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	notebook = gtk_notebook_new();
	menu_bar = gtk_menu_bar_new();
	status_bar = gtk_statusbar_new();
	new_tab_btn = gtk_button_new_from_icon_name("tab-new", GTK_ICON_SIZE_BUTTON);

	g_signal_connect(window, "destroy", G_CALLBACK(quit_activate), NULL);
//	g_signal_connect(new_tab_btn, "activate", G_CALLBACK(atsugami_browser_new), NULL);		// Uncomment once the browser widget has been written.

	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), status_bar, FALSE, TRUE, 0);
	gtk_notebook_set_action_widget(GTK_NOTEBOOK(notebook), new_tab_btn, GTK_PACK_END);

	/** Menu bar **/
	GtkWidget *menu;
	GtkWidget *menu_item;

	/* File Menu */
	GtkWidget *import_mi;
	GtkWidget *bulk_import_mi;
	GtkWidget *export_mi;
	GtkWidget *delete_mi;
	GtkWidget *quit_mi;

	menu = gtk_menu_new();
	menu_item = gtk_menu_item_new_with_mnemonic("_File");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item);

	import_mi = gtk_menu_item_new_with_mnemonic("_Import");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), import_mi);

	bulk_import_mi= gtk_menu_item_new_with_mnemonic("_Bulk Import");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), bulk_import_mi);

	export_mi = gtk_menu_item_new_with_mnemonic("E_xport");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), export_mi);

	delete_mi = gtk_menu_item_new_with_mnemonic("_Delete");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), delete_mi);

	quit_mi = gtk_menu_item_new_with_mnemonic("_Quit");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), quit_mi);
	g_signal_connect(quit_mi, "activate", G_CALLBACK(quit_activate), NULL);

	/* Edit Menu */
	GtkWidget *edit_mi;
	GtkWidget *bulk_edit_mi;
	GtkWidget *edit_tag_mi;
	GtkWidget *edit_wiki_mi;
	GtkWidget *preferences_mi;

	menu = gtk_menu_new();
	menu_item = gtk_menu_item_new_with_mnemonic("_Edit");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item);

	edit_mi = gtk_menu_item_new_with_mnemonic("_Edit");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), edit_mi);

	bulk_edit_mi = gtk_menu_item_new_with_mnemonic("_Bulk Edit");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), bulk_edit_mi);

	edit_tag_mi = gtk_menu_item_new_with_mnemonic("_Edit Tag");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), edit_tag_mi);

	edit_wiki_mi = gtk_menu_item_new_with_mnemonic("_Edit Wiki Page");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), edit_wiki_mi);

	preferences_mi = gtk_menu_item_new_with_mnemonic("_Preferences");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), preferences_mi);


	/* Image Menu */
	GtkWidget *open_mi;
	GtkWidget *open_new_tab_mi;
	GtkWidget *copy_img_uri;
	GtkWidget *properties_mi;

	menu = gtk_menu_new();
	menu_item = gtk_menu_item_new_with_mnemonic("_Image");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item);

	open_mi = gtk_menu_item_new_with_mnemonic("_Open");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), open_mi);

	open_new_tab_mi = gtk_menu_item_new_with_mnemonic("Open In New _Tab");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), open_new_tab_mi);

	copy_img_uri = gtk_menu_item_new_with_mnemonic("_Copy File Path");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), copy_img_uri);

	properties_mi = gtk_menu_item_new_with_mnemonic("_Properties");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), properties_mi);


	/* Tag Menu */
	GtkWidget *new_tag_mi;
	GtkWidget *tag_list_mi;

	menu = gtk_menu_new();
	menu_item = gtk_menu_item_new_with_mnemonic("_Tags");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item);

	new_tag_mi = gtk_menu_item_new_with_mnemonic("_New Tag");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), new_tag_mi);

	tag_list_mi = gtk_menu_item_new_with_mnemonic("Tag _List");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), tag_list_mi);
	g_signal_connect(tag_list_mi, "activate", G_CALLBACK(atsugami_tag_list_new), NULL);


	/* View Menu */
	GtkWidget *icon_size_menu;

	menu = gtk_menu_new();
	menu_item = gtk_menu_item_new_with_mnemonic("_View");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item);

	safe_mode_mi = gtk_check_menu_item_new_with_mnemonic("_Safe Mode");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), safe_mode_mi);
	g_signal_connect(safe_mode_mi, "activate", G_CALLBACK(safe_mode_toggle), NULL);

	icon_size_menu = gtk_menu_new();
	menu_item = gtk_menu_item_new_with_mnemonic("_Icon Size");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), icon_size_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

	small_mi = gtk_check_menu_item_new_with_mnemonic("_Small");
	gtk_menu_shell_append(GTK_MENU_SHELL(icon_size_menu), small_mi);

	medium_mi = gtk_check_menu_item_new_with_mnemonic("_Medium");
	gtk_menu_shell_append(GTK_MENU_SHELL(icon_size_menu), medium_mi);

	large_mi = gtk_check_menu_item_new_with_mnemonic("_Large");
	gtk_menu_shell_append(GTK_MENU_SHELL(icon_size_menu), large_mi);

	huge_mi = gtk_check_menu_item_new_with_mnemonic("_Huge");
	gtk_menu_shell_append(GTK_MENU_SHELL(icon_size_menu), huge_mi);

	gigantic_mi = gtk_check_menu_item_new_with_mnemonic("_Gigantic");
	gtk_menu_shell_append(GTK_MENU_SHELL(icon_size_menu), gigantic_mi);


	/* Wiki Menu */
	GtkWidget *open_wiki_mi;
	GtkWidget *new_wiki_mi;

	menu = gtk_menu_new();
	menu_item = gtk_menu_item_new_with_mnemonic("_Wiki");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item);

	open_wiki_mi = gtk_menu_item_new_with_mnemonic("_Open Wiki");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), open_wiki_mi);

	new_wiki_mi = gtk_menu_item_new_with_mnemonic("_New Wiki Page");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), new_wiki_mi);


	/* Database Menu */
	GtkWidget *console_mi;
	GtkWidget *db_info_mi;

	menu = gtk_menu_new();
	menu_item = gtk_menu_item_new_with_mnemonic("_Database");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item);

	console_mi = gtk_menu_item_new_with_mnemonic("_Console");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), console_mi);
	g_signal_connect(console_mi, "activate", G_CALLBACK(atsugami_console_new), NULL);

	db_info_mi = gtk_menu_item_new_with_mnemonic("_Database Info");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), db_info_mi);


	/* Help Menu */
	GtkWidget *help_mi;
	GtkWidget *about_mi;

	menu = gtk_menu_new();
	menu_item = gtk_menu_item_new_with_mnemonic("_Help");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item);

	help_mi = gtk_menu_item_new_with_mnemonic("_Help");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), help_mi);

	about_mi = gtk_menu_item_new_with_mnemonic("_About");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), about_mi);


	/** Other stuff **/
	check_safe_mode();
	file_count();
	gtk_widget_show_all(window);
	gtk_main();
}
