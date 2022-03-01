/* callbacks.h */
#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

#include <glib.h>
#include <gtk/gtk.h>

extern void console(void);
extern void exit(int ret_code);
extern void about_activate(void);
extern void file_count(void);
extern void file_count_update(void);
extern void import_activate(void);
extern void import_wizard(void);
extern void list_tags(void);
extern void new_artist_activate(void);
extern void new_character_activate(void);
extern void new_copyright_activate(void);
extern void new_tag_activate(void);
extern void new_meta_tag_activate(void);
extern void home_page(GtkWidget *parent);
extern void notbook_reload(GtkWidget *list_store);
extern void notebook_reload_relay(void);
extern void tab(void);
extern void viewer(gpointer user_data);	// user_data is an image path.

#endif
