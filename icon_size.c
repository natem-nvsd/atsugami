/* icon_size.h
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
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(medium_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(large_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(huge_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gigantic_mi), FALSE);

			gtk_widget_set_sensitive(medium_mi, TRUE);
			gtk_widget_set_sensitive(large_mi, TRUE);
			gtk_widget_set_sensitive(huge_mi, TRUE);
			gtk_widget_set_sensitive(gigantic_mi, TRUE);

			size_res = PQexec(conn, "UPDATE public.settings SET thumb_size = 0;");
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(small_mi), TRUE);
			gtk_widget_set_sensitive(small_mi, FALSE);
			PQclear(size_res);

			break;
		}

		case 1: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(small_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(large_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(huge_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gigantic_mi), FALSE);

			gtk_widget_set_sensitive(small_mi, TRUE);
			gtk_widget_set_sensitive(large_mi, TRUE);
			gtk_widget_set_sensitive(huge_mi, TRUE);
			gtk_widget_set_sensitive(gigantic_mi, TRUE);

			size_res = PQexec(conn, "UPDATE public.settings SET thumb_size = 1;");
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(medium_mi), TRUE);
			gtk_widget_set_sensitive(medium_mi, FALSE);
			PQclear(size_res);

			break;
		}

		case 2: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(small_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(medium_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(huge_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gigantic_mi), FALSE);

			gtk_widget_set_sensitive(small_mi, TRUE);
			gtk_widget_set_sensitive(medium_mi, TRUE);
			gtk_widget_set_sensitive(huge_mi, TRUE);
			gtk_widget_set_sensitive(gigantic_mi, TRUE);

			size_res = PQexec(conn, "UPDATE public.settings SET thumb_size = 2;");
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(large_mi), TRUE);
			gtk_widget_set_sensitive(large_mi, FALSE);
			PQclear(size_res);

			break;
		}

		case 3: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(small_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(medium_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(huge_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gigantic_mi), FALSE);

			gtk_widget_set_sensitive(small_mi, TRUE);
			gtk_widget_set_sensitive(medium_mi, TRUE);
			gtk_widget_set_sensitive(large_mi, TRUE);
			gtk_widget_set_sensitive(gigantic_mi, TRUE);

			size_res = PQexec(conn, "UPDATE public.settings SET thumb_size = 3;");
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(huge_mi), TRUE);
			gtk_widget_set_sensitive(huge_mi, FALSE);
			PQclear(size_res);

			break;
		}

		case 4: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(small_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(medium_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(large_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(huge_mi), FALSE);

			gtk_widget_set_sensitive(small_mi, TRUE);
			gtk_widget_set_sensitive(medium_mi, TRUE);
			gtk_widget_set_sensitive(large_mi, TRUE);
			gtk_widget_set_sensitive(huge_mi, TRUE);

			size_res = PQexec(conn, "UPDATE public.settings SET thumb_size = 4;");
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gigantic_mi), TRUE);
			gtk_widget_set_sensitive(gigantic_mi, FALSE);
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
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(medium_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(large_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(huge_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gigantic_mi), FALSE);

			gtk_widget_set_sensitive(medium_mi, TRUE);
			gtk_widget_set_sensitive(large_mi, TRUE);
			gtk_widget_set_sensitive(huge_mi, TRUE);
			gtk_widget_set_sensitive(gigantic_mi, TRUE);

			size_res = PQexec(conn, "UPDATE public.settings SET thumb_size = 0;");
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(small_mi), TRUE);
			gtk_widget_set_sensitive(small_mi, FALSE);
			PQclear(size_res);

			break;
		}

		case 1: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(small_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(large_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(huge_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gigantic_mi), FALSE);

			gtk_widget_set_sensitive(small_mi, TRUE);
			gtk_widget_set_sensitive(large_mi, TRUE);
			gtk_widget_set_sensitive(huge_mi, TRUE);
			gtk_widget_set_sensitive(gigantic_mi, TRUE);

			size_res = PQexec(conn, "UPDATE public.settings SET thumb_size = 1;");
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(medium_mi), TRUE);
			gtk_widget_set_sensitive(medium_mi, FALSE);
			PQclear(size_res);

			break;
		}

		case 2: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(small_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(medium_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(huge_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gigantic_mi), FALSE);

			gtk_widget_set_sensitive(small_mi, TRUE);
			gtk_widget_set_sensitive(medium_mi, TRUE);
			gtk_widget_set_sensitive(huge_mi, TRUE);
			gtk_widget_set_sensitive(gigantic_mi, TRUE);

			size_res = PQexec(conn, "UPDATE public.settings SET thumb_size = 2;");
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(large_mi), TRUE);
			gtk_widget_set_sensitive(large_mi, FALSE);
			PQclear(size_res);

			break;
		}

		case 3: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(small_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(medium_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(huge_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gigantic_mi), FALSE);

			gtk_widget_set_sensitive(small_mi, TRUE);
			gtk_widget_set_sensitive(medium_mi, TRUE);
			gtk_widget_set_sensitive(large_mi, TRUE);
			gtk_widget_set_sensitive(gigantic_mi, TRUE);

			size_res = PQexec(conn, "UPDATE public.settings SET thumb_size = 3;");
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(huge_mi), TRUE);
			gtk_widget_set_sensitive(huge_mi, FALSE);
			PQclear(size_res);

			break;
		}

		case 4: {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(small_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(medium_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(large_mi), FALSE);
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(huge_mi), FALSE);

			gtk_widget_set_sensitive(small_mi, TRUE);
			gtk_widget_set_sensitive(medium_mi, TRUE);
			gtk_widget_set_sensitive(large_mi, TRUE);
			gtk_widget_set_sensitive(huge_mi, TRUE);

			size_res = PQexec(conn, "UPDATE public.settings SET thumb_size = 4;");
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(gigantic_mi), TRUE);
			gtk_widget_set_sensitive(gigantic_mi, FALSE);
			PQclear(size_res);

			break;
		}

	}

	return;
}
