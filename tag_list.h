/* tag_list.h
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

#ifndef ATSUGAMI_TAG_LIST_H
#define ATSUGAMI_TAG_LIST_H

#include <gtk/gtk.h>

enum {
	NAME_COL,
	CATEGORY_COL,
	TAG_COUNT_COL,
	TAG_COLOUR_COL,
	BLACKLIST_COL,
	N_COLS
};

G_BEGIN_DECLS
#define ATSUGAMI_TYPE_TAG_LIST                  (atsugami_tag_list_get_type())
#define ATSUGAMI_TAG_LIST(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), ATSUGAMI_TYPE_TAG_LIST, AtsugamiTagList))
#define ATSUGAMI_TAG_LIST_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass), ATSUGAMI_TAG_LIST, AtsugamiTagListClass))
#define ATSUGAMI_IS_TAG_LIST(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), ATSUGAMI_TYPE_TAG_LIST))
#define ATSUGAMI_IS_TAG_LIST_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), ATSUGAMI_TYPE_TAG_LIST))
#define ATSUGAMI_TAG_LIST_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), G_TYPE_TAG_LIST, AtsugamiTagListClass))


typedef struct _AtsugamiTagList		AtsugamiTagList;
typedef struct _AtsugamiTagListPrivate	AtsugamiTagListPrivate;
typedef struct _AtsugamiTagListClass	AtsugamiTagListClass;


struct _AtsugamiTagList {
	GtkContainer container;

	// Private
	AtsugamiTagListPrivate *pte;
};

struct _AtsugamiTagListClass {
	GtkContainerClass parent_class;

//	void (*draw)		(AtsugamiTagList *tag_list, GtkWidget *widget);
};

struct _AtsugamiTagListPrivate {
	/* Label widgets */
	GtkWidget *label_hbox;
	GtkWidget *label_label;
	GtkWidget *label_icon;
	GtkWidget *label_button;
	
	/* Conainer */
	GtkContainer *container;
	GtkWidget *child;
	GtkWidget *vbox;
	GtkWidget *tree_view;
	
	GtkTreeViewColumn *column;
	GtkTreeModel *tree_model;
	GtkCellRenderer *renderer;
};

enum {
	LAST_WIDGET_PROPERTY,
	PROP_HADJUSTMENT,
	PROP_VADJUSTMENT,
	PROP_HSCROLL_POLICY,
	PROP_VSCROLL_POLICY,

	LAST_PROPERTY
};

extern GType atsugami_tag_list_get_type(void);
extern GtkWidget *atsugami_tag_list_new(void);
extern void atsugami_tag_list_add(GtkContainer *container, GtkWidget *widget);
extern void atsugami_tag_list_set_child(AtsugamiTagList *tag_list, GtkWidget *widget);

G_END_DECLS
#endif
