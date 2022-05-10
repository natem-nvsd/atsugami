/* This file is practically the same as one of the functions at
 * https://discourse.gnome.org/t/gtkentrycompletion-complete-when-the-user-presses-tab/8965
 * by madmurphy */
#include <gtk/gtk.h>
#include <stdbool.h>

extern bool tab_completion(const GtkEventControllerKey *event_controller, const unsigned int keycode, const GdkModifierType state, const GtkWidget *entry) {
	int start, end;

	if (keycode == GDK_KEY_Tab && gtk_editable_get_selection_bounds(GTK_EDITABLE(entry), &start, &end)) {
	/* // These two should do the same thing.
	if (keycode == GDK_KEY_Tab) {
		gtk_editable_get_selection_bounds(GTK_EDITABLE(entry), &start, &end)
	*/
		gtk_editable_set_position(GTK_EDITABLE(entry), end);
		return true;
	}

	return false;
}
