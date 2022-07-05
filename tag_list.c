/* list_tags.c
 * Copyright (c) 2021-2022 by Nate Morrison.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "atsugami.h"
#include "tag_list.h"
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdbool.h>

G_DEFINE_TYPE_WITH_CODE(AtsugamiTagList, atsugami_tag_list, GTK_TYPE_CONTAINER, G_ADD_PRIVATE(AtsugamiTagList) G_IMPLEMENT_INTERFACE(GTK_TYPE_CONTAINER, NULL))

static GtkTreeModel *tag_tree_model_gen(void) {
	int row_count, row_now;
	GtkTreeIter tree_iter;
	GtkListStore *list_store;
	PGresult *list_res;

	list_res = PQexec(conn, "SELECT tags.name, tags_categories.category_id, (SELECT COUNT(tag_id) FROM public.files_tags AS files_tags WHERE files_tags.tag_id = tags.id) AS counter, tags.blacklisted FROM public.tags AS tags INNER JOIN public.tags_categories AS tags_categories ON tags.id = tags_categories.tag_id ORDER BY tags.name ASC;");
	row_count = PQntuples(list_res);

	list_store = gtk_list_store_new(5, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_STRING, G_TYPE_BOOLEAN, -1);

	for (row_now = 0; row_now < row_count; row_now++) {
		int catid, counter;
		bool blacklisted;
		char *tree_path_str, *colour;

		tree_path_str = (char *) malloc(sizeof(int) + 2);
		sprintf(tree_path_str, "0:%d", row_now);
		GtkTreePath *tree_path = gtk_tree_path_new_from_string(tree_path_str);

		catid = atoi(PQgetvalue(list_res, row_now, 1));
		counter = atoi(PQgetvalue(list_res, row_now, 2));
		colour = colour_getter(catid);

		if ((PQgetvalue(list_res, row_now, 3))[0] == 'f')
			blacklisted = false;
		else
			blacklisted = true;

		gtk_list_store_append(list_store, &tree_iter);
		gtk_list_store_set(list_store, &tree_iter, NAME_COL, PQgetvalue(list_res, row_now, 0), TAG_COLOUR_COL, colour, CATEGORY_COL, catid, TAG_COUNT_COL, counter, BLACKLIST_COL, blacklisted, -1);
		gtk_tree_path_append_index(tree_path, row_now);

		free(tree_path_str);
		gtk_tree_path_free(tree_path);
		tree_path_str = NULL;
	}

	PQclear(list_res);

	return GTK_TREE_MODEL(list_store);
}

static void add_columns(AtsugamiTagList *tag_list) {
	AtsugamiTagListPrivate *pte;

//	pte = tag_list->pte;
	pte = atsugami_tag_list_get_instance_private(tag_list);
	pte->tree_model = gtk_tree_view_get_model(GTK_TREE_VIEW(pte->tree_view));
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(pte->tree_view), TRUE);
	gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(pte->tree_view), GTK_TREE_VIEW_GRID_LINES_HORIZONTAL);

	/* Name Column */
	pte->renderer = gtk_cell_renderer_text_new();
	pte->column = gtk_tree_view_column_new_with_attributes("Name", pte->renderer, "text", NAME_COL, "foreground", TAG_COLOUR_COL, NULL);

	gtk_tree_view_column_set_sizing(pte->column, GTK_TREE_VIEW_COLUMN_GROW_ONLY);
	gtk_tree_view_column_set_sort_column_id(pte->column, NAME_COL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(pte->tree_view), pte->column);

	/* Count column */
	pte->renderer = gtk_cell_renderer_text_new();
	pte->column = gtk_tree_view_column_new_with_attributes("Count", pte->renderer, "text", TAG_COUNT_COL, NULL);

	gtk_tree_view_column_set_sizing(pte->column, GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_sort_column_id(pte->column, TAG_COUNT_COL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(pte->tree_view), pte->column);

	/* Blacklist column */
	pte->renderer = gtk_cell_renderer_toggle_new();
	pte->column = gtk_tree_view_column_new_with_attributes("Blacklisted", pte->renderer, "active", BLACKLIST_COL, NULL);

	gtk_tree_view_column_set_sizing(pte->column, GTK_TREE_VIEW_COLUMN_FIXED);
	gtk_tree_view_column_set_sort_column_id(pte->column, BLACKLIST_COL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(pte->tree_view), pte->column);
}

GtkWidget *atsugami_tag_list_new(void) {
	return g_object_new(ATSUGAMI_TYPE_TAG_LIST, NULL);
}

static void atsugami_tag_list_init(AtsugamiTagList *tag_list) {
	AtsugamiTagListPrivate *pte;

	tag_list->pte = atsugami_tag_list_get_instance_private(tag_list);
//	pte = tag_list->pte;

	gtk_widget_set_can_focus(GTK_WIDGET(tag_list), TRUE);

	/* List Store & Tree Model */
	pte->tree_model = tag_tree_model_gen();
	pte->tree_view = gtk_tree_view_new_with_model(pte->tree_model);
	gtk_tree_view_set_search_column(GTK_TREE_VIEW(pte->tree_view), NAME_COL);
	add_columns(tag_list);
	g_object_unref(pte->tree_model);

	/* Page layout */
	pte->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	pte->child = gtk_scrolled_window_new(NULL, NULL);

	atsugami_tag_list_set_child(tag_list, pte->child);
	gtk_container_add(GTK_CONTAINER(pte->container), pte->vbox);
	gtk_box_pack_start(GTK_BOX(pte->vbox), pte->tree_view, TRUE, TRUE, 0);

	/* Tab label */
	pte->label_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	pte->label_label = gtk_label_new("Tag list");
	pte->label_icon = gtk_image_new_from_icon_name("view-list-symbolic", GTK_ICON_SIZE_MENU);
	pte->label_button = gtk_button_new_from_icon_name("window-close-symbolic", GTK_ICON_SIZE_MENU);

	gtk_button_set_relief(GTK_BUTTON(pte->label_button), GTK_RELIEF_NONE);
	gtk_box_pack_start(GTK_BOX(pte->label_hbox), pte->label_icon, TRUE, TRUE, 4);
	gtk_box_pack_start(GTK_BOX(pte->label_hbox), pte->label_label, TRUE, TRUE, 4);
	gtk_box_pack_start(GTK_BOX(pte->label_hbox), pte->label_button, TRUE, TRUE, 4);
//	g_signal_connect(pte->label_button, "activate", G_CALLBACK(close_tab), (void *) pte);
	
//	gtk_container_add(GTK_CONTAINER(notebook), pte->container);
}

static void atsugami_tag_list_set_property(GObject *object, guint id, const GValue *value, GParamSpec *pspec) {
	AtsugamiTagList *tag_list;

	tag_list = (AtsugamiTagList *) object;
}

static void atsugami_tag_list_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec) {
}

static void atsugami_tag_list_finalise(GObject *object) {
}

static gboolean atsugami_tag_list_draw(GtkWidget *tag_list, cairo_t *ct) {
	AtsugamiTagListPrivate *pte;

	pte = atsugami_tag_list_get_instance_private(ATSUGAMI_TAG_LIST(tag_list));

	gtk_widget_show_all(GTK_WIDGET(tag_list));
	gtk_widget_show_all(pte->label_hbox);

	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook), GTK_WIDGET(tag_list), pte->label_hbox);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), (gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook)) - 1));

	return GDK_EVENT_PROPAGATE;;
}

static void atsugami_tag_list_realise(GtkWidget *widget) {
}

static void atsugami_tag_list_unrealise(GtkWidget *widget) {
}

static void atsugami_tag_list_size_allocate(GtkWidget *widget, GtkAllocation *alloc) {
}

static void atsugami_tag_list_map(GtkWidget *widget) {
}

static void atsugami_tag_list_unmap(GtkWidget *widget) {
}

static void atsugami_tag_list_get_preferred_width(GtkWidget *tag_list, int *minimal, int *natural) {
}

static void atsugami_tag_list_get_preferred_height(GtkWidget *widget, gint *x, gint *y) {
	void *re;

}

void atsugami_tag_list_set_child(AtsugamiTagList *tag_list, GtkWidget *widget) {
	GtkContainer *container;

	container = GTK_CONTAINER(tag_list);
//	gtk_container_add(GTK_CONTAINER(container), widget);
	atsugami_tag_list_add(GTK_CONTAINER(container), widget);
}

void atsugami_tag_list_add(GtkContainer *container, GtkWidget *widget) {
	AtsugamiTagListPrivate *pte;
//	GtkContainerPrivate *pte;

	pte = ATSUGAMI_TAG_LIST(container)->pte;
	pte->container = container;
	gtk_container_add(GTK_CONTAINER(pte->container), widget);
//	atsugami_tag_list_set_child(ATSUGAMI_TAG_LIST(container), widget);
}

static void atsugami_tag_list_class_init(AtsugamiTagListClass *class) {
	GObjectClass *object_class;
	GtkWidgetClass *widget_class;
	GtkContainerClass *container_class;

	object_class = G_OBJECT_CLASS(class);
	widget_class = GTK_WIDGET_CLASS(class);
	container_class = GTK_CONTAINER_CLASS(class);

	object_class->set_property	= atsugami_tag_list_set_property;
	object_class->get_property	= atsugami_tag_list_get_property;
	object_class->finalize		= atsugami_tag_list_finalise;

	widget_class->draw			= atsugami_tag_list_draw;
	widget_class->realize			= atsugami_tag_list_realise;
	widget_class->unrealize			= atsugami_tag_list_unrealise;
	widget_class->size_allocate		= atsugami_tag_list_size_allocate;
	widget_class->map			= atsugami_tag_list_map;
	widget_class->unmap			= atsugami_tag_list_unmap;
	widget_class->get_preferred_width	= atsugami_tag_list_get_preferred_width;
	widget_class->get_preferred_height	= atsugami_tag_list_get_preferred_height;

	container_class->add			= atsugami_tag_list_add;
//	container_class->remove			= atsugami_tag_list_remove;
//	container_class->forall			= atsugami_tag_list_forall;
//	container_class->child_type		= atsugami_tag_list_child_type;
//	container_class->set_child_property	= atsugami_tag_list_set_child_property;
//	container_class->get_child_property	= atsugami_tag_list_get_child_property;
//	container_class->get_path_for_child	= atsugami_tag_list_get_path_for_child;
	gtk_container_class_handle_border_width(container_class);
}
