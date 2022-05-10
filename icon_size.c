/***************************************************************\
*	misc.c (c) 2022, by Nate Morrison			*
*								*
*	Functions too short for their own file			*
*								*
\***************************************************************/

#include "atsugami.h"
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>

/* Icon sizes */
extern char *get_icon_size_name(void) {
	PGresult *size_res;
	char *icon_size, *icon_size_raw;
	int icon_size_id;

	size_res = PQexec(conn, "SELECT thumb_size FROM public.settings;");
	icon_size_raw = (char *) malloc(sizeof(int));
	sprintf(icon_size_raw, "%s", PQgetvalue(size_res, 0, 0));
	icon_size_id = atoi(icon_size_raw);

	switch (icon_size_id) {
		case 0: {
			icon_size = (char *) malloc(5);
			icon_size = "small";
			break;
		}

		case 1: {
			icon_size = (char *) malloc(6);
			icon_size = "medium";
			break;
		}

		case 2: {
			icon_size = (char *) malloc(5);
			icon_size = "large";
			break;
		}

		case 3: {
			icon_size = (char *) malloc(4);
			icon_size = "huge";
			break;
		}

		case 4: {
			icon_size = (char *) malloc(8);
			icon_size = "gigantic";
			break;
		}
	}


	PQclear(size_res);
	free(icon_size_raw);
	return icon_size;
}

extern int get_icon_size(void) {
	/* Returns the width of a thumbnail corresponding with its id (0-4) */
	PGresult *size_res;
	char *icon_size_raw = NULL;
	int icon_size_id, icon_size;

	size_res = PQexec(conn, "SELECT thumb_size FROM public.settings;");
	icon_size_raw = (char *) malloc(sizeof(int));
	sprintf(icon_size_raw, "%s", PQgetvalue(size_res, 0, 0));
	icon_size_id = atoi(icon_size_raw);

	switch (icon_size_id) {
		case 0: {
			icon_size = 150;
			break;
		}

		case 1: {
			icon_size = 180;
			break;
		}

		case 2: {
			icon_size = 225;
			break;
		}

		case 3: {
			icon_size = 270;
			break;
		}

		case 4: {
			icon_size = 360;
			break;
		}
	}

	PQclear(size_res);
	free(icon_size_raw);
	return icon_size;
}

extern void set_icon_menu_items(void) {
	PGresult *size_res;
	char *icon_size_raw = NULL;
	int icon_size_id;

	size_res = PQexec(conn, "SELECT thumb_size FROM public.settings;");
	icon_size_raw = (char *) malloc(sizeof(int));
	sprintf(icon_size_raw, "%s", PQgetvalue(size_res, 0, 0));
	icon_size_id = atoi(icon_size_raw);

	switch (icon_size_id) {
		case 0: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(medium_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(large_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(huge_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gigantic_icons_menu_item), FALSE);

			gtk_widget_set_sensitive(medium_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(large_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(huge_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(gigantic_icons_menu_item, TRUE);

			size_res = PQexec(conn, "UPDATE public.settings SET thumb_size = 0;");
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(small_icons_menu_item), TRUE);
			gtk_widget_set_sensitive(small_icons_menu_item, FALSE);
			PQclear(size_res);

			break;
		}

		case 1: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(small_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(large_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(huge_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gigantic_icons_menu_item), FALSE);

			gtk_widget_set_sensitive(small_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(large_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(huge_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(gigantic_icons_menu_item, TRUE);

			size_res = PQexec(conn, "UPDATE public.settings SET thumb_size = 1;");
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(medium_icons_menu_item), TRUE);
			gtk_widget_set_sensitive(medium_icons_menu_item, FALSE);
			PQclear(size_res);

			break;
		}

		case 2: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(small_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(medium_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(huge_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gigantic_icons_menu_item), FALSE);

			gtk_widget_set_sensitive(small_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(medium_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(huge_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(gigantic_icons_menu_item, TRUE);

			size_res = PQexec(conn, "UPDATE public.settings SET thumb_size = 2;");
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(large_icons_menu_item), TRUE);
			gtk_widget_set_sensitive(large_icons_menu_item, FALSE);
			PQclear(size_res);

			break;
		}

		case 3: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(small_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(medium_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(huge_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gigantic_icons_menu_item), FALSE);

			gtk_widget_set_sensitive(small_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(medium_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(large_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(gigantic_icons_menu_item, TRUE);

			size_res = PQexec(conn, "UPDATE public.settings SET thumb_size = 3;");
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(huge_icons_menu_item), TRUE);
			gtk_widget_set_sensitive(huge_icons_menu_item, FALSE);
			PQclear(size_res);

			break;
		}

		case 4: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(small_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(medium_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(large_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(huge_icons_menu_item), FALSE);

			gtk_widget_set_sensitive(small_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(medium_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(large_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(huge_icons_menu_item, TRUE);

			size_res = PQexec(conn, "UPDATE public.settings SET thumb_size = 4;");
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gigantic_icons_menu_item), TRUE);
			gtk_widget_set_sensitive(gigantic_icons_menu_item, FALSE);
			PQclear(size_res);

			break;
		}
	}

	PQclear(size_res);
	free(icon_size_raw);
}

extern void set_icon_size(gpointer user_data) {
	PGresult *size_res;
	int *size_id = (int *) user_data;

	switch (*size_id) {
		case 0: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(medium_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(large_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(huge_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gigantic_icons_menu_item), FALSE);

			gtk_widget_set_sensitive(medium_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(large_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(huge_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(gigantic_icons_menu_item, TRUE);

			size_res = PQexec(conn, "UPDATE public.settings SET thumb_size = 0;");
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(small_icons_menu_item), TRUE);
			gtk_widget_set_sensitive(small_icons_menu_item, FALSE);
			PQclear(size_res);

			break;
		}

		case 1: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(small_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(large_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(huge_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gigantic_icons_menu_item), FALSE);

			gtk_widget_set_sensitive(small_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(large_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(huge_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(gigantic_icons_menu_item, TRUE);

			size_res = PQexec(conn, "UPDATE public.settings SET thumb_size = 1;");
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(medium_icons_menu_item), TRUE);
			gtk_widget_set_sensitive(medium_icons_menu_item, FALSE);
			PQclear(size_res);

			break;
		}

		case 2: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(small_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(medium_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(huge_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gigantic_icons_menu_item), FALSE);

			gtk_widget_set_sensitive(small_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(medium_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(huge_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(gigantic_icons_menu_item, TRUE);

			size_res = PQexec(conn, "UPDATE public.settings SET thumb_size = 2;");
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(large_icons_menu_item), TRUE);
			gtk_widget_set_sensitive(large_icons_menu_item, FALSE);
			PQclear(size_res);

			break;
		}

		case 3: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(small_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(medium_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(huge_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gigantic_icons_menu_item), FALSE);

			gtk_widget_set_sensitive(small_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(medium_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(large_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(gigantic_icons_menu_item, TRUE);

			size_res = PQexec(conn, "UPDATE public.settings SET thumb_size = 3;");
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(huge_icons_menu_item), TRUE);
			gtk_widget_set_sensitive(huge_icons_menu_item, FALSE);
			PQclear(size_res);

			break;
		}

		case 4: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(small_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(medium_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(large_icons_menu_item), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(huge_icons_menu_item), FALSE);

			gtk_widget_set_sensitive(small_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(medium_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(large_icons_menu_item, TRUE);
			gtk_widget_set_sensitive(huge_icons_menu_item, TRUE);

			size_res = PQexec(conn, "UPDATE public.settings SET thumb_size = 4;");
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gigantic_icons_menu_item), TRUE);
			gtk_widget_set_sensitive(gigantic_icons_menu_item, FALSE);
			PQclear(size_res);

			break;
		}

	}

	return;
}
