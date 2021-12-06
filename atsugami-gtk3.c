#include <gtk/gtk.h>

typedef GtkApplication Atsugami;
typedef GtkApplicationClass AtsugamiClass;

G_DEFINE_TYPE (Atsugami, atsugami, GTK_TYPE_APPLICATION)

typedef struct {
	GtkApplicationWindow parent_instance;

	GtkWidget *message;
	GtkWidget *infobar;
	GtkWidget *status;
	GtkWidget *menubar;
	GMenuModel *toolmenu;
	GtkTextBuffer *buffer;

	int width;
	int height;
	gboolean maximized;
	gboolean fullscreen;
} AtsugamiWindow;
typedef GtkApplicationWindowClass AtsugamiWindowClass;

G_DEFINE_TYPE (AtsugamiWindow, atsugami_window, GTK_TYPE_APPLICATION_WINDOW)
	static void create_window (GApplication *app, const char *contents);

static void show_action_dialog (GSimpleAction *action) {
	const char *name;
	GtkWidget *dialog;

	name = g_action_get_name (G_ACTION (action));
	dialog = gtk_message_dialog_new (NULL,
					 GTK_DIALOG_DESTROY_WITH_PARENT,
					 GTK_MESSAGE_INFO,
					 GTK_BUTTONS_CLOSE,
					 "You activated action: \"%s"\"",
					 NAME);
	.
