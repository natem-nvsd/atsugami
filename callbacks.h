/* callbacks.h */
#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

#include <glib.h>
#include <gtk/gtk.h>

extern void console(void);
extern void exit(int ret_code);
extern void about_activate(void);
extern void file_count(const GtkWidget *label, const GtkWidget *box);
extern void file_count_update(const GtkWidget *label, const GtkWidget *box);
extern void import_activate(void);
extern void import_wizard(void);
extern void list_tags(void);
extern void new_artist_activate(void);
extern void new_character_activate(void);
extern void new_copyright_activate(void);
extern void new_tag_activate(void);
extern void new_meta_tag_activate(void);
extern void home_page(void);
extern void notbook_reload(GtkWidget *list_store);
extern void notebook_reload_relay(void);

#endif
