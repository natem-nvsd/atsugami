/***************************************************************\
*	misc.c (c) 2022, by Nate Morrison			*
*								*
*	Functions too short for their own file			*
*								*
\***************************************************************/

#include "atsugami.h"
#include <gtk/gtk.h>

/* Debugging functions */
extern void dbg_err(const char *msg) {
	printf("\033[1m\033[38;5;196mERROR:\033[0m %s\n", msg);
}

extern void dbg_info(const char *msg) {
	printf("\033[1m\033[38;5;033mNOTICE:\033[0m %s\n", msg);
}

extern void dbg_print(const char *msg) {
	printf("\033[1mDEBUGGING:\033[0m %s\n", msg);
}

extern void dbg_warn(const char *msg) {
	printf("\033[1m\033[38;5;011mNOTICE:\033[0m %s\n", msg);
}

/* Next tab callback */
extern void next_tab_cb(GtkNotebook *notebook) {
	printf("next tab\n");
	gtk_notebook_next_page(notebook);
}

/* Quit callback */
extern void quit_activate(void) {
	PQfinish(conn);
	exit(0);
}

/* Previous tab callback */
extern void prev_tab_cb(GtkNotebook *notebook) {
	printf("prev tab\n");
	gtk_notebook_prev_page(notebook);
}

/* Search callback */
extern void search_cb(GtkWidget **entry) {
	char *query;

	query = (char *) malloc(strlen(gtk_entry_get_text(GTK_ENTRY(entry))));
	sprintf(query, "%s", gtk_entry_get_text(GTK_ENTRY(entry)));
	printf("query: '%s'\n", query);

	//gtk_widget_destroy(tab_child);
	//tab_child = search();
}
