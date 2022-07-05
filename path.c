/* path.c
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

extern char *get_thumb_path(void) {
	PGresult *path_res;

	path_res = PQexec(conn, "SELECT thumb_dir FROM public.settings;");

	char *path_base = PQgetvalue(path_res, 0, 0);
	char *size_name = get_icon_size_name();
	char *path;
	size_t size;

	size = (strlen(path_base) + strlen(size_name) + 2);

	path = (char *) malloc(size);
	strcpy(path, "");
	sprintf(path, "%s/%s/", path_base, size_name);

	return path;
}

extern char *get_file_path(void) {
	PGresult *path_res;
	char *path_base;

	path_res = PQexec(conn, "SELECT store_dir FROM public.settings;");

	size_t size = (PQfsize(path_res, 0) + 1);

	path_base = (char *) malloc(size);
	sprintf(path_base, "%s/", PQgetvalue(path_res, 0, 0));

	return path_base;
}

extern char *get_thumb_dir(void) {
	PGresult *path_res;
	char *path;

	path_res = PQexec(conn, "SELECT thumb_dir FROM public.settings;");
	path = PQgetvalue(path_res, 0, 0);

	return path;
}

extern char *get_thumb_path_by_id(const int id) {
	char *path, *path_base, *size;

	path_base = get_thumb_dir();
	switch (id) {
		case 0: {
			size = "small";
			break;
		}

		case 1: {
			size = "medium";
			break;
		}

		case 2: {
			size = "large";
			break;
		}

		case 3: {
			size = "huge";
			break;
		}

		case 4: {
			size = "gigantic";
			break;
		}

	}

	path = (char *) malloc(strlen(size) + strlen(path_base));
	sprintf(path, "%s/%s", path_base, size);

	return path;
}
