/* view.c
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
#include <libpq-fe.h>
#include <stdbool.h>

extern void check_safe_mode(void) {
	PGresult *res;

	res = PQexec(conn, "SELECT safe_mode FROM public.settings;");
	if (strcmp(PQgetvalue(res, 0, 0), "f") == 0) {
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(safe_mode_mi), FALSE);
		safe_mode = false;
	} else {
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(safe_mode_mi), TRUE);
		safe_mode = true;
	}

	PQclear(res);
}

extern void safe_mode_toggle(void) {
	PGresult *res;

	res = PQexec(conn, "SELECT safe_mode FROM public.settings;");
	if (strcmp(PQgetvalue(res, 0, 0), "f") == 0) {
		PQclear(res);

		res = PQexec(conn, "UPDATE public.settings SET safe_mode = 't';");
		safe_mode = true;
	} else {
		PQclear(res);

		res = PQexec(conn, "UPDATE public.settings SET safe_mode = 'f';");
		safe_mode = false;
	}

	PQclear(res);
	file_count();
}
