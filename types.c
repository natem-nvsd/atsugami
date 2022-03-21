/***************************************************************\
*	types.c		(c) 2022, by Nate Morrison		*
*								*
*	Function definitions of prototypes defined in types.h.	*
*								*
\***************************************************************/

#include "types.h"
#include <gtk/gtk.h>
#include <stdlib.h>

extern CallBackData *CallBackData_new(void) {
	CallBackData cb_data;
	CallBackData *cb_data_ptr;

	cb_data_ptr = malloc(sizeof(CallBackData));
	cb_data_ptr = &cb_data;

	return cb_data_ptr;
}

extern void CallBackData_destroy(CallBackData *cb_data) {
	cb_data = NULL;
	free(cb_data);
}

/** Setters **/
extern void CallBackData_set_list_store(CallBackData *cb_data, GtkListStore *list_store) {
	cb_data->extern_list_store = list_store;
}

extern void CallBackData_set_parent(CallBackData *cb_data, GtkWidget *parent) {
	cb_data->parent = parent;
}

extern void CallBackData_set_child(CallBackData *cb_data, GtkWidget *child) {
	cb_data->child = child;
}

extern void CallBackData_set_text_view(CallBackData *cb_data, GtkWidget *text_view) {
	cb_data->text_view = text_view;
}

extern void CallBackData_set_icon_view(CallBackData *cb_data, GtkIconView *icon_view) {
	cb_data->icon_view = icon_view;
}

extern void CallBackData_set_text_buffer(CallBackData *cb_data, GtkTextBuffer *text_buffer) {
	cb_data->text_buffer = text_buffer;
}

extern void CallBackData_set_tree_iter(CallBackData *cb_data, GtkTreeIter tree_iter) {
	cb_data->tree_iter = tree_iter;
}

extern void CallBackData_set_tree_path(CallBackData *cb_data, GtkTreePath *tree_path) {
	cb_data->tree_path = tree_path;
}

extern void CallBackData_set_tree_model(CallBackData *cb_data, GtkTreeModel *tree_model) {
	cb_data->tree_model = tree_model;
}

extern void CallBackData_set_test_int(CallBackData *cb_data, int test_int) {
	cb_data->test_int = test_int;
}

/** Getters **/
extern GtkListStore *CallBackData_get_list_store(CallBackData *cb_data) {
	return cb_data->extern_list_store;
}

extern GtkWidget *CallBackData_get_parent(CallBackData *cb_data) {
	return cb_data->parent;
}

extern GtkWidget *CallBackData_get_child(CallBackData *cb_data) {
	return cb_data->child;
}

extern GtkWidget *CallBackData_get_text_view(CallBackData *cb_data) {
	return cb_data->text_view;
}

extern GtkIconView *CallBackData_get_icon_view(CallBackData *cb_data) {
	return cb_data->icon_view;
}

extern GtkTextBuffer *CallBackData_get_text_buffer(CallBackData *cb_data) {
	return cb_data->text_buffer;
}

extern GtkTreeIter *CallBackData_get_tree_iter(CallBackData *cb_data) {
	return &cb_data->tree_iter;
}

extern GtkTreePath *CallBackData_get_tree_path(CallBackData *cb_data) {
	return cb_data->tree_path;
}

extern GtkTreeModel *CallBackData_get_tree_model(CallBackData *cb_data) {
	return cb_data->tree_model;
}

extern int CallBackData_get_test_int(CallBackData *cb_data) {
	return cb_data->test_int;
}

/* Advanced tooltip */
extern AdvancedTooltip AdvancedTooltip_new(GtkWidget *relative_to) {
	AdvancedTooltip advanced_tooltip;
	AdvancedTooltip *advanced_tooltip_ptr;

	//advanced_tooltip = &(advanced_tooltip->tooltip);
//	advanced_tooltip = AdvancedTooltip;
	//advanced_tooltip->tooltip = gtk_popover_new(relative_to);
	advanced_tooltip_ptr = malloc(sizeof(AdvancedTooltip));
	advanced_tooltip_ptr = &advanced_tooltip;


	advanced_tooltip.tooltip = gtk_popover_new(relative_to);

	return advanced_tooltip;
}

extern void AdvancedTooltip_destroy(AdvancedTooltip advanced_tooltip) {
	free(&advanced_tooltip);
}
