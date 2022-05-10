/* import.c */
#include "atsugami.h"
#include "import.h"
#include <gtk-3.0/gtk/gtk.h>
#include <libpq-fe.h>
#include <sha256.h>
#include <string.h>
#include <sys/types.h>

PGresult *import_res;
char import_file_path[10240];	// This is a terrible way of doing this. There will be a buffer overflow with large paths
char file_sha256[65];
int response;

static void update_preview_cb(GtkWidget *file_chooser, GtkWidget *preview) {
	char *file_name;
	GdkPixbuf *pixbuf;
	bool have_preview;

	file_name = gtk_file_chooser_get_preview_filename(GTK_FILE_CHOOSER(file_chooser));
	pixbuf = gdk_pixbuf_new_from_file_at_size(file_name, 500, 500, NULL);
	have_preview = (pixbuf != NULL);
	free(file_name);

	gtk_image_set_from_pixbuf(GTK_IMAGE(preview), pixbuf);
	if (pixbuf)
		g_object_unref(pixbuf);
	
	gtk_file_chooser_set_preview_widget_active(GTK_FILE_CHOOSER(file_chooser), have_preview);
}

/* The "Import" function */
static void open_response_cb(gpointer user_data) {
	GtkWidget *fc = user_data;
	char *file_path, *dir, *query;

	printf("\n\nCLICKED\n\n");
	if (GTK_RESPONSE_ACCEPT) {
		printf("ACCEPT\n");
		file_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fc));
		dir = gtk_file_chooser_get_current_folder_uri(GTK_FILE_CHOOSER(fc));
		gtk_widget_destroy(fc);

		query = (char *) malloc(69 + strlen(file_path));
		sprintf(query, "UPDATE public.settings SET last_dir = '%s';", dir);
		import_res = PQexec(conn, query);
		
		/* Copy the path into `import_file_path`, since `file_path` is cleared when the dialog is destroyed. */
		if (strlen(file_path) > 10239) {	// this is fine for now
			fprintf(stderr, "Path too long.\n");
			return;
		}

		PQclear(import_res);
		strcpy(import_file_path, file_path);
		SHA256_File(import_file_path, file_sha256);
		sprintf(query, "SELECT EXISTS (SELECT sha256 FROM public.files WHERE sha256 = '%s');", file_sha256);

		import_res = PQexec(conn, query);
		if (strcmp(PQgetvalue(import_res, 0, 0), "f") == 0) {
			/* Run the wizard; the file chooser window is destroyed before the wizard opens. */
			import_wizard();
		} else {
			GtkWidget *dup_diag;
			 GtkDialogFlags diag_flags = GTK_RESPONSE_ACCEPT;

			dup_diag = gtk_message_dialog_new(GTK_WINDOW(window), diag_flags, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Duplicate file");
			gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dup_diag), "This file has already been imported, so you cannot import it again.");
			gtk_dialog_run(GTK_DIALOG(dup_diag));
			gtk_widget_destroy(dup_diag);
		}

		PQclear(import_res);
	} else {
		printf("CANCEL\n");
		gtk_widget_destroy(fc);
	}
}

extern void import_activate(void) {
	GtkFileChooserAction open_action = GTK_FILE_CHOOSER_ACTION_OPEN;
	static char *query = "SELECT last_dir FROM public.settings;";
	GtkWidget *file_chooser, *preview;
	GtkFileFilter *filter0, *filter1;

	import_res = PQexec(conn, query);
	file_chooser = gtk_file_chooser_dialog_new("Import an Image", NULL, open_action, "_Cancel", GTK_RESPONSE_CANCEL, "_Import", GTK_RESPONSE_ACCEPT, NULL);

	/* Filter 0 */
	filter0 = gtk_file_filter_new();
	preview = gtk_image_new();

	gtk_file_filter_add_pattern(filter0, "*.gif");
	gtk_file_filter_add_pattern(filter0, "*.jfif");
	gtk_file_filter_add_pattern(filter0, "*.jpg");
	gtk_file_filter_add_pattern(filter0, "*.jpeg");
	gtk_file_filter_add_pattern(filter0, "*.png");
	gtk_file_filter_add_pattern(filter0, "*.svg");
	//gtk_file_filter_add_pattern(filter0, "*.webp");		// Uncomment when support for webp has been added
	gtk_file_filter_set_name(filter0, "Image files");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_chooser), filter0);

	gtk_file_chooser_set_preview_widget(GTK_FILE_CHOOSER(file_chooser), preview);
	g_signal_connect(file_chooser, "update-preview", G_CALLBACK(update_preview_cb), preview);

	/* Filter 1 */
	filter1 = gtk_file_filter_new();

	gtk_file_filter_add_pattern(filter1, "*");
	gtk_file_filter_set_name(filter1, "All files");

	/* show the dialog */
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_chooser), filter1);
	g_signal_connect(file_chooser, "response", G_CALLBACK(open_response_cb), file_chooser);
	gtk_file_chooser_set_current_folder_uri(GTK_FILE_CHOOSER(file_chooser), PQgetvalue(import_res, 0, 0));
	PQclear(import_res);

	response = gtk_dialog_run(GTK_DIALOG(file_chooser));
}
