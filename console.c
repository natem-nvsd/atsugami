/***************************************************************\
* console.c (c) 2022 by Nate Morrison				*
*								*
* Crappy SQL console. I'm tired of switching between Atsugami 	*
* and xterm every time I need to make changes to the database.	*
*								*
\***************************************************************/
#include "main.h"
#include "callbacks.h"
#include <fcntl.h>
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GtkTextBuffer *console_buffer;
GtkWidget *console_entry, *console_view;

static void console_cb(void) {
	PGresult *con_res;
	PQprintOpt options = {0};
	FILE *cachefile;
	char *cachepath, *buf;
	size_t bufsize, write_size;

	con_res = PQexec(conn, "SELECT conf_dir FROM public.settings;");
	cachepath = (char *) malloc(strlen(PQgetvalue(con_res, 0, 0)) + 15);

	sprintf(cachepath, "%s/console_cache", PQgetvalue(con_res, 0, 0));
	PQclear(con_res);

	con_res = PQexec(conn, gtk_entry_get_text(GTK_ENTRY(console_entry)));
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
	//fseek(cachefile, 0, SEEK_END);
	fseek(cachefile, 0, SEEK_SET);
	PQprint(cachefile, con_res, &options);
	PQclear(con_res);

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
	GtkWidget *console_page, *console_scrolled;
	/*
	FILE *cachefile;
	char *cachepath, *buf;
	size_t bufsize, write_size;
	*/

	console_view = gtk_text_view_new();
	console_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(console_view));
	console_page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	console_entry = gtk_entry_new();
	console_scrolled = gtk_scrolled_window_new(NULL, NULL);
	page_count = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));

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

	gtk_widget_show_all(console_page);
	gtk_container_add(GTK_CONTAINER(notebook), console_page);
	gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(notebook), console_page, "Console");
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), page_count);
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), console_page, TRUE);
	g_signal_connect(console_entry, "activate", G_CALLBACK(console_cb), NULL);
}
