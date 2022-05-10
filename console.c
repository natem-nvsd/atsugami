/***************************************************************\
* console.c (c) 2022 by Nate Morrison				*
*								*
* Crappy SQL console. I'm tired of switching between Atsugami 	*
* and xterm every time I need to make changes to the database.	*
*								*
\***************************************************************/
#include "atsugami.h"
#include <fcntl.h>
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GtkTextBuffer *console_buffer;
GtkWidget *console_page, *console_entry, *console_view;
PGresult *console_res;

static void close_tab(void) {
	gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), console_page);

	if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook)) == 0)
		quit_activate();
}

static void console_cb(void) {
	PQprintOpt options = {0};
	FILE *cachefile;
	char *cachepath, *buf;
	size_t bufsize, write_size;

	console_res = PQexec(conn, "SELECT conf_dir FROM public.settings;");
	cachepath = (char *) malloc(strlen(PQgetvalue(console_res, 0, 0)) + 15);

	sprintf(cachepath, "%s/console_cache", PQgetvalue(console_res, 0, 0));
	PQclear(console_res);

	console_res = PQexec(conn, gtk_entry_get_text(GTK_ENTRY(console_entry)));
	cachefile = fopen(cachepath, "ab+");

	options.header		= 1;	/* Printoutput field headings and row count */
	options.align		= 1;	/* Fill align the fields */
	options.standard	= 1;	/* Old brain dead format */
	options.html3		= 0;	/* Output HTML tables */
	options.expanded	= 0;	/* Expand tables */
	options.pager		= 0;	/* Use pager for output if needed */
	options.fieldSep	= "|";	/* Field sparator */
	options.tableOpt	= 0;	/* Attributes for HTML table element */
	options.caption		= 0;	/* HTML table caption */

	/* Print the query result to a cache file */
	fseek(cachefile, 0, SEEK_SET);
	PQprint(cachefile, console_res, &options);
	PQclear(console_res);

	bufsize = ftell(cachefile);
	buf = (char *) malloc(bufsize);
	fseek(cachefile, 0, SEEK_SET);
	write_size = fread(buf, sizeof(char), bufsize, cachefile);

	gtk_text_buffer_set_text(console_buffer, buf, write_size);
	gtk_entry_set_text(GTK_ENTRY(console_entry), "");
	fclose(cachefile);
	free(buf);
	free(cachepath);
}

extern void console(void) {
	int page_count;
	GtkWidget *console_scrolled, *tab_label_box, *tab_icon, *tab_label, *tab_close;
	FILE *cachefile;
	char *cachepath;
	//size_t bufsize, write_size;

	console_res = PQexec(conn, "SELECT conf_dir FROM public.settings;");
	cachepath = (char *) malloc(strlen(PQgetvalue(console_res, 0, 0)) + 15);
	sprintf(cachepath, "%s/console_cache", PQgetvalue(console_res, 0, 0));
	cachefile = fopen(cachepath, "w");
	console_view = gtk_text_view_new();
	console_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(console_view));
	console_page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	console_entry = gtk_entry_new();
	console_scrolled = gtk_scrolled_window_new(NULL, NULL);
	page_count = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));

	PQclear(console_res);
	/* Cache file */
	fprintf(cachefile, " ");

	/* Scrolled window */
	gtk_container_add(GTK_CONTAINER(console_scrolled), console_view);
	gtk_widget_set_vexpand(console_view, TRUE);

	gtk_scrolled_window_set_propagate_natural_height(GTK_SCROLLED_WINDOW(console_scrolled), TRUE);
	gtk_scrolled_window_set_kinetic_scrolling(GTK_SCROLLED_WINDOW(console_scrolled), TRUE);
	gtk_widget_set_vexpand(console_scrolled, TRUE);
	gtk_box_pack_start(GTK_BOX(console_page), console_scrolled, TRUE, TRUE, 0);

	/* Add console_view to console_page */
	gtk_container_set_border_width(GTK_CONTAINER(console_page), 10);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(console_view), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(console_view), GTK_WRAP_WORD_CHAR);
	gtk_text_view_set_monospace(GTK_TEXT_VIEW(console_view), TRUE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(console_view), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(console_view), GTK_WRAP_CHAR);

	/* Add console_entry to console_page */
	gtk_entry_set_activates_default(GTK_ENTRY(console_entry), TRUE);
	gtk_widget_set_valign(console_entry, GTK_ALIGN_START);
	gtk_entry_set_placeholder_text(GTK_ENTRY(console_entry), "Enter a SQL command:");
	gtk_box_pack_start(GTK_BOX(console_page), console_entry, FALSE, FALSE, 0);

	tab_label_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
	tab_icon = gtk_image_new_from_icon_name("utilities-terminal", GTK_ICON_SIZE_MENU);
	tab_label = gtk_label_new("Console");
	tab_close = gtk_button_new_from_icon_name("window-close-symbolic", GTK_ICON_SIZE_MENU);

	/* Tab label */
	gtk_button_set_relief(GTK_BUTTON(tab_close), GTK_RELIEF_NONE);
	gtk_widget_add_accelerator(tab_close, "clicked", accel, GDK_KEY_w, GDK_CONTROL_MASK, GTK_ACCEL_LOCKED);
	g_signal_connect(GTK_BUTTON(tab_close), "clicked", G_CALLBACK(close_tab), NULL);

	gtk_box_pack_start(GTK_BOX(tab_label_box), tab_icon, FALSE, FALSE, 4);
	gtk_box_pack_start(GTK_BOX(tab_label_box), tab_label, FALSE, FALSE, 4);
	gtk_box_pack_start(GTK_BOX(tab_label_box), tab_close, FALSE, FALSE, 4);

	gtk_widget_show_all(tab_label_box);
	gtk_widget_show_all(console_page);
	gtk_container_add(GTK_CONTAINER(notebook), console_page);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook), console_page, tab_label_box);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), page_count);
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), console_page, TRUE);
	g_signal_connect(console_entry, "activate", G_CALLBACK(console_cb), NULL);
	fclose(cachefile);
	free(cachepath);
}
