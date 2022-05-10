/* main.h */
#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
#include "types.h"
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdbool.h>

/* Status macros for use once a "recycle bin" has been implemented */
#define STATUS_ACTIVE	0	// The file shows up when 
#define STATUS_DELETED	1	// The image is hidden from general parts of Atsugami, but the image has not been deleted from disk.
				// Images with this status can be accessed from a "recycle bin"-like view
#define STATUS_EXPUNGED	2	// The image has been deleted from the disk; thumbnails may be kept, and metadata remains to prevent re-importing

#define EOL -1
#define __VERSION "0.84.0"

enum {
	SHA256_COL,
	COL_PIXBUF,
	TOOLTIP_COL,
	NUM_COLS
};

/* Completion enum */
enum {
	COMPLETION_MODEL_NAME_COLUMN,
	COMPLETION_MODEL_CATEGORY_COLUMN,
	COMPLETION_MODEL_N_COLUMN
};

extern int msg_id;
extern bool safe_mode;
extern GtkWidget *about_menu_item, *bulk_edit_menu_item, *bulk_import_image, *bulk_import_menu_item, *console_menu_item, *delete_menu_item, *edit_file_menu_item, *edit_image, *edit_menu, *edit_menu_item, *error_area, *error_bar, *error_image, *error_label, *error_widget, *export_menu_item, *favourite_image, *file_menu, *file_menu_item, *gigantic_icons_menu_item, *help_menu, *help_menu_button, *help_menu_item, *home_image, *huge_icons_menu_item, *image_menu, *image_menu_item, *img_info_menu_item, *import_image, *import_menu_item, *large_icons_menu_item, *medium_icons_menu_item, *menu_bar, *new_tag_menu_item, *new_wiki_page,*next_tab, *open_img_extern_menu_item, *open_img_menu_item, *open_img_new_tab_menu_item, *open_wiki_menu_item, *prev_tab, *quit_menu_item, *safe_mode_menu_item, *settings_menu_item, *small_icons_menu_item, *tags_menu, *tags_menu_item, *thumb_size_menu, *thumb_size_menu_item, *toolbar, *view_menu, *view_menu_item, *warn_area, *warn_bar, *warn_image, *warn_label, *warn_widget, *wiki_image, *wiki_menu, *wiki_menu_item;

extern void dbg_err(const char *msg);
extern void dbg_info(const char *msg);
extern void dbg_print(const char *msg);
extern void dbg_warn(const char *msg);

extern PGconn *conn;
extern GtkTreePath *ico_tree_path;
extern GtkAccelGroup *accel;
extern CallBackData *viewer_data;

extern void about_activate(void);
extern GtkTooltip *att(char *sha256, GtkWidget *relative_to, GtkCellRenderer *cell);
extern int completion_bootstrap(void);
//extern char conninfo[];
extern char *conninfo;
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
extern GtkWidget *notebook, *vbox, *window;
extern void prev_tab_cb(GtkNotebook *notebook);
extern void quit_activate(void);
extern GtkListStore *search_cb(GtkWidget *entry, const char *query);
extern GtkWidget *status_bar;
extern void tab(void);
extern void wiki(void);
extern void viewer(GtkWidget *icon_view, GtkTreePath *tree_path, gpointer user_data);
extern void safe_mode_toggle(void);
extern long word_count(const char *str);
extern bool tab_completion(const GtkEventControllerKey *event_controller, const unsigned int keycode, const GdkModifierType state, const GtkWidget *entry);
extern void set_icon_size(gpointer user_data);
extern int get_icon_size(void);
extern char *get_icon_size_name(void);
extern void set_icon_menu_items(void);
extern char *get_thumb_path(void);
extern char *get_file_path(void);
extern void pixbuf_loader(GtkListStore *list_store);
extern GtkTreeModel *completion_model_generator(const int category);
extern void entry_completion_set_model(GtkEntryCompletion *entry_completion, GtkTreeModel *entry_completion_model);
extern char *get_thumb_dir(void);
extern char *get_thumb_path_by_id(const int id);






#endif
