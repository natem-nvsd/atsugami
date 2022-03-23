/* main.h */
#define EOL -1
#define __VERSION "0.83.0"

//typedef _Bool bool;

#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
#include "types.h"
#include <gtk/gtk.h>
#include <libpq-fe.h>

/* Status macros for use once a "recycle bin" has been implemented */
#define STATUS_ACTIVE	0	// The file shows up when 
#define STATUS_DELETED	1	// The image is hidden from general parts of Atsugami, but the image has not been deleted from disk.
				// Images with this status can be accessed from a "recycle bin"-like view
#define STATUS_EXPUNGED	2	// The image has been deleted from the disk; thumbnails may be kept, and metadata remains to prevent re-importing

enum {
	SHA256_COL,
	COL_PIXBUF,
	NUM_COLS
};

extern void dbg_err(const char *msg);
extern void dbg_info(const char *msg);
extern void dbg_print(const char *msg);
extern void dbg_warn(const char *msg);

extern PGconn *conn;
extern GtkTreeModel *completion_model;
extern GtkTreePath *ico_tree_path;
extern GtkTreeIter ico_tree_iter;
extern GtkListStore *completion_list, *ico_list_store;
extern GtkAccelGroup *accel;
extern CallBackData *viewer_data;

extern void about_activate(void);
extern GtkTooltip *att(char *sha256, GtkWidget *rel2, GtkCellRenderer *cell);
extern int completion_bootstrap(void);
extern char conninfo[];
extern void console(void);
extern void destroy_window(gpointer user_data);
extern void exit(int ret_code);
extern void file_count(void);
extern void file_count_update(void);
extern GtkWidget *home_page(void);
extern void import_activate(void);
extern void import_wizard(void);
extern void list_tags(void);
extern void new_artist_activate(void);
extern void new_character_activate(void);
extern void new_copyright_activate(void);
extern void new_tag_activate(void);
extern void new_meta_tag_activate(void);
extern void next_tab_cb(GtkNotebook *notebook);
extern GtkWidget *notebook, *file_label, *vbox, *window;
extern void prev_tab_cb(GtkNotebook *notebook);
extern void quit_activate(void);
extern void search_cb(GtkWidget **entry);
extern void tab(void);
extern void wiki(void);
extern GtkWidget *viewer(GtkWidget *parent_tag_tv, GtkTextBuffer *parent_tag_tb, char *sha256[65]);
#endif
