#include "atsugami.h"
#include "types.h"
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

	path_res = PQexec(conn, "SELECT store_dir FROM public.settings;");

	char *path_base, *size_name = get_icon_size_name();
	size_t size = (PQfsize(path_res, 0) + strlen(size_name) + 2);

	path_base = (char *) malloc(size);
	sprintf(path_base, "%s/%s/", PQgetvalue(path_res, 0, 0), size_name);
	char *path = path_base;

	return path;
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
