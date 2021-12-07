#include <gtk/gtk.h>

int main(int argc, char * argv []) {
	GtkBuilder *builder; 
	GtkWidget *app;

	gtk_init(&argc, &argv);

	// builder = gtk_builder_new();
	// gtk_builder_add_from_file(builder,"glade/window_main.glade", NULL）;
	//  Updated in October 2019: The following line replaces the 2 line above
	builder = gtk_builder_new_from_file("glade/new_atsugami.glade");

		app = GTK_WIDGET(gtk_builder_get_object(builder, "app"));
	gtk_builder_connect_signals(builder, NULL);

	g_object_unref(builder);

	gtk_widget_show(app);                
	gtk_main();

         return 0;
}

//Turn off the window call
/*invalid ON_WINDOW_MAIN_DESTROY ()
{
	gtk_main_quit();
}*/
