/* import.c */
#include "import.h"
#include <gtk-3.0/gtk/gtk.h>
#include "main.h"
#include <string.h>
#include "import_wizard.h"
#include <sha256.h>

gchar import_file_path[10240];
char file_sha256[65];

/* The "Import" function */
static void open_response_cb(GtkNativeDialog *dialog, gint response_id, gpointer user_data) {
	GtkFileChooserNative *native = user_data;
	char *file_path;

	if (response_id == GTK_RESPONSE_ACCEPT) {
		/* Get the path of the file selected when the user presses "Import" */
		file_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(native));
		
		/* Copy the path into `import_file_path`, since `file_path` is cleared when the dialog is destroyed. */
		strcpy(import_file_path, file_path);
		SHA256_File(import_file_path, file_sha256);
		printf("%s\n", file_sha256);

		/* Run the wizard; the file chooser window is destroyed before the wizard opens. */
		import_wizard();
	}

	gtk_native_dialog_destroy(GTK_NATIVE_DIALOG(native));
	g_object_unref(native);
}

/* The import window itself */
extern void import_activate(gpointer user_data) {
	GApplication *app = user_data;
	GtkFileChooserNative *native;
	GtkFileFilter *filter0;
	GtkFileFilter *filter1;

	native = gtk_file_chooser_native_new("Import File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, "_Import", "_Cancel");

	filter0 = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter0, "*.gif");
	gtk_file_filter_add_pattern(filter0, "*.jfif");
	gtk_file_filter_add_pattern(filter0, "*.jpg");
	gtk_file_filter_add_pattern(filter0, "*.jpeg");
	gtk_file_filter_add_pattern(filter0, "*.png");
	gtk_file_filter_add_pattern(filter0, "*.svg");
	gtk_file_filter_set_name(filter0, "Image files");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(native), filter0);

	filter1 = gtk_file_filter_new();
	gtk_file_filter_add_pattern(filter1, "*");
	gtk_file_filter_set_name(filter1, "All files");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(native), filter1);


	g_object_set_data_full(G_OBJECT(native), "app", g_object_ref(app), g_object_unref);
	g_signal_connect(native, "response", G_CALLBACK(open_response_cb), native);

	gtk_native_dialog_show(GTK_NATIVE_DIALOG(native));
}
