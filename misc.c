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
	printf("\033[5m\033[1m\033[38;5;196mERROR:\033[0m %s\n", msg);
}

extern void dbg_info(const char *msg) {
	printf("\033[5m\033[1m\033[38;5;033mNOTICE:\033[0m %s\n", msg);
}

extern void dbg_print(const char *msg) {
	printf("\033[5m\033[1mDEBUGGING:\033[0m %s\n", msg);
}

extern void dbg_warn(const char *msg) {
	printf("\033[5m\033[1m\033[38;5;011mWARNING:\033[0m %s\n", msg);
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

/* Safe Mode */
extern void safe_mode_toggle(void) {
	PGresult *safe_res;

	safe_res = PQexec(conn, "SELECT safe_mode FROM public.settings;");
	if (strcmp(PQgetvalue(safe_res, 0, 0), "f") == 0) {
		PQclear(safe_res);
		safe_res = PQexec(conn, "UPDATE public.settings SET safe_mode = 't';");
		PQclear(safe_res);
		printf("Safe mode enabled.\n");
	} else {
		PQclear(safe_res);
		safe_res = PQexec(conn, "UPDATE public.settings SET safe_mode = 'f';");
		PQclear(safe_res);
		printf("Safe mode disabled.\n");
	}

}
