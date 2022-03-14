/***************************************************************\
*	types.h		(c) 2022, by Nate Morrison		*
*								*
*	Custom types for Atsugami.				*
*								*
\***************************************************************/

#ifndef CALLBACK_DATA_H
#define CALLBACK_DATA_H
#include <gtk/gtk.h>

/* General-purpose callback data */
typedef struct _CallBackData {
	GtkListStore	*extern_list_store;
	GtkWidget	*parent;
	GtkWidget	*child;
	GtkWidget	*text_view;
	GtkIconView	*icon_view;
	GtkTextBuffer	*text_buffer;
	GtkTreeIter	tree_iter;
	GtkTreePath	*tree_path;
	GtkTreeModel	*tree_model;
	int		test_int;
} CallBackData;

extern CallBackData *CallBackData_new(void);
extern void CallBackData_destroy(CallBackData *cb_data);

/** Setter methods **/
extern void CallBackData_set_list_store(CallBackData	*cb_data, GtkListStore	*list_store);
extern void CallBackData_set_parent(CallBackData	*cb_data, GtkWidget	*parent);
extern void CallBackData_set_child(CallBackData		*cb_data, GtkWidget	*child);
extern void CallBackData_set_text_view(CallBackData	*cb_data, GtkWidget	*text_view);
extern void CallBackData_set_icon_view(CallBackData	*cb_data, GtkIconView	*icon_view);
extern void CallBackData_set_text_buffer(CallBackData	*cb_data, GtkTextBuffer	*text_buffer);
extern void CallBackData_set_tree_iter(CallBackData	*cb_data, GtkTreeIter	tree_iter);
extern void CallBackData_set_tree_path(CallBackData	*cb_data, GtkTreePath	*tree_path);
extern void CallBackData_set_tree_model(CallBackData	*cb_data, GtkTreeModel	*tree_model);
extern void CallBackData_set_test_int(CallBackData	*cb_data, int 		test_int);

/** Getter methods **/
extern GtkListStore	*CallBackData_get_list_store(CallBackData	*cb_data);
extern GtkWidget	*CallBackData_get_parent(CallBackData		*cb_data);
extern GtkWidget	*CallBackData_get_child(CallBackData		*cb_data);
extern GtkWidget	*CallBackData_get_text_view(CallBackData	*cb_data);
extern GtkIconView	*CallBackData_get_icon_view(CallBackData	*cb_data);
extern GtkTextBuffer	*CallBackData_get_text_buffer(CallBackData	*cb_data);
extern GtkTreeIter	*CallBackData_get_tree_iter(CallBackData	*cb_data);
extern GtkTreePath	*CallBackData_get_tree_path(CallBackData	*cb_data);
extern GtkTreeModel	*CallBackData_get_tree_model(CallBackData	*cb_data);
extern int		CallBackData_get_test_int(CallBackData		*cb_data);
#endif
