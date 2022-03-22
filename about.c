/* about.c */
#include "atsugami.h"
#include "copyright.h"
#include <gtk/gtk.h>

void about_activate(void) {
	GtkWidget *about_diag;
	const gchar **contributers_ptr = NULL;
	const char *contributers = "Nate Morrison";

	//*contributers_ptr = contributers;
	about_diag = gtk_about_dialog_new();

	gtk_about_dialog_set_logo_icon_name(GTK_ABOUT_DIALOG(about_diag), "image-x-generic");
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about_diag), "Atsugami");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_diag), __VERSION);
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about_diag), "© 2021-2022 by Nate Morrison et al.");
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about_diag), "https://github.com/natem-nvsd/atsugami");
	//gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about_diag), contributers_ptr);
	gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(about_diag), LICENSE);
	gtk_about_dialog_set_wrap_license(GTK_ABOUT_DIALOG(about_diag), TRUE);

	//gtk_widget_show_all(about_diag);
	gtk_dialog_run(GTK_DIALOG(about_diag));
	gtk_widget_destroy(about_diag);
}
