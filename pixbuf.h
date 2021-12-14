/* This code is from the GDK Pixbuf code demo in program "gtk3-demo"
* Modifications have been made where needed. */

#include <gtk/gtk>

GtkWidget *do_pixbufs(GtkWidget *do_widget) {
	if (!window) {

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_screen(GTK_WINDOW(window), gtk_widget_get_screen(do_widget));
	gtk_window_set_title(GTK_WINDOW(window), "Pixbufs");
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_widget_destroyed), &window);

	if (!load_pixbufs (&error)) {
		GtkWidget *dialog;

		g_error_free(error);

		g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);

		gtk_widget_show(dialog);
        }
	else {
		gtk_widget_set_size_request(window, back_width, back_height);

		frame = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, back_width, back_height);

		da = gtk_drawing_area_new();

		g_signal_connect(da, "draw", G_CALLBACK (draw_cb), NULL);

		gtk_container_add(GTK_CONTAINER(window), da);

		gtk_widget_add_tick_callback(da, on_tick, NULL, NULL);
	}
}
