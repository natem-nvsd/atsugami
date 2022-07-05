/* console.c
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
 *
 *
 * Crappy SQL console. I'm tired of switching between Atsugami
 * and xterm every time I need to make changes to the database.
 */

#include "atsugami.h"
#include "console.h"
#include <fcntl.h>
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void atsugami_console_finalise(GObject *object);
static void atsugami_console_realise(GtkWidget *widget);
static void atsugami_console_unrealise(GtkWidget *widget);
static void atsugami_console_map(GtkWidget *widget);
static void atsugami_console_unmap(GtkWidget *widget);
//static gint atsugami_console_grab_broke_event(GtkWidget *widget, GdkEventGrabBroken *event);
//static gint atsugami_enter_notify_event(GtkWidget *widget, GdkEventCrossing *event);
//static gint atsugami_leave_notify_event(GtkWidget *widget, GdkEventCrossing *event);
//static void atsugami_state_changed(GtkWidget *widget, GtkStateType previous_state);
//static void atsugami_console_grab_notify(GtkWidget *widget, gboolean was_grabbed);

struct _AtsugamiConsolePrivate {
	/* Tab label */
	GtkWidget *hbox;
	GtkWidget *widget;
	GtkWidget *label_button;
	
	/* Widgets the user will see */
	GtkWidget *scrolled_window;
	GtkWidget *text_view;
	GtkWidget *entry;

	/* misc. */
	GtkAction *action;
	GdkWindow *window;
	GtkTextBuffer *text_buffer;
	PGresult *res;

	guint constructed	: 1;
};

G_DEFINE_TYPE_WITH_CODE(AtsugamiConsole, atsugami_console, GTK_TYPE_WIDGET, G_ADD_PRIVATE(AtsugamiConsole) G_IMPLEMENT_INTERFACE(GTK_TYPE_CONTAINER, NULL))

extern GtkWidget *atsugami_console_new(void) {
	return g_object_new(ATSUGAMI_TYPE_CONSOLE, NULL);
}

static void atsugami_console_class_init(AtsugamiConsoleClass *klass) {
	GObjectClass *gobject_class;
	GtkWidgetClass *widget_class;
	GtkContainerClass *container_class;

	gobject_class = G_OBJECT_CLASS(klass);
	widget_class = (GtkWidgetClass *) klass;
	container_class = (GtkContainerClass *) klass;

	gobject_class->finalize			= atsugami_console_finalise;
//	widget_class->draw			= atsugami_console_draw;
	widget_class->realize			= atsugami_console_realise;
	widget_class->unrealize			= atsugami_console_unrealise;
	widget_class->map			= atsugami_console_map;
	widget_class->unmap			= atsugami_console_unmap;
//	widget_class->grab_broken_event		= atsugami_console_grab_broke_event;
//	widget_class->enter_notify_event	= atsugami_enter_notify_event;
//	widget_class->leave_notify_event	= atsugami_leave_notify_event;
//	widget_class->state_changed		= atsugami_state_changed;
//	widget_class->grab_notify		= atsugami_console_grab_notify;
}

static void atsugami_console_finalise(GObject *object) {
	AtsugamiConsole *console;
	AtsugamiConsolePrivate *pte;

	console = (AtsugamiConsole *) object;
	pte = (AtsugamiConsolePrivate *) (console);

	g_clear_object(&pte->text_buffer);
	G_OBJECT_CLASS(atsugami_console_parent_class)->finalize(object);
}

/*
static gboolean atsugami_console_draw(GtkWidget *widget, cairo_t *cr) {
	AtsugamiConsole *console;
	AtsugamiConsolePrivate *pte;

}
*/

static void atsugami_console_realise(GtkWidget *widget) {
	AtsugamiConsolePrivate *pte;
	GtkAllocation allocation;
	GdkWindowAttr attributes = { 0, };
	GdkWindow *window;

	pte = (AtsugamiConsolePrivate *) (ATSUGAMI_CONSOLE(widget));

	gtk_widget_get_allocation(widget, &allocation);
	gtk_widget_set_realized(widget, TRUE);

	attributes.x = allocation.x;
	attributes.y = allocation.y;
	attributes.width = allocation.width;
	attributes.height = allocation.height;
	attributes.window_type = GDK_WINDOW_CHILD;
	attributes.event_mask = gtk_widget_get_events(widget);
	attributes.wclass = GDK_INPUT_ONLY;

	window = gtk_widget_get_parent_window(widget);
	gtk_widget_set_window(widget, window);
	g_object_ref(G_OBJECT(window));

	window = gdk_window_new(gtk_widget_get_parent_window(widget), &attributes, GDK_WA_X | GDK_WA_Y);
	pte->window = window;

	gtk_widget_register_window(widget, pte->window);
	gdk_window_set_user_data(window, widget);
}

static void atsugami_console_unrealise(GtkWidget *widget) {
	AtsugamiConsolePrivate *pte;

	pte = (AtsugamiConsolePrivate *) (ATSUGAMI_CONSOLE(widget));
	if (pte->window) {
		gtk_widget_unregister_window(widget, pte->window);
		gdk_window_destroy(pte->window);
		pte->window = NULL;
	}

	GTK_WIDGET_CLASS(atsugami_console_parent_class)->unrealize(widget);
}

static void atsugami_console_map(GtkWidget *widget) {
	AtsugamiConsolePrivate *pte;

	pte = (AtsugamiConsolePrivate *) widget;

	gdk_window_show(pte->window);
	GTK_WIDGET_CLASS(atsugami_console_parent_class)->map(widget);
}

static void atsugami_console_unmap(GtkWidget *widget) {
	AtsugamiConsolePrivate *pte;

	pte = (AtsugamiConsolePrivate *) widget;
	GTK_WIDGET_CLASS(atsugami_console_parent_class)->unmap(widget);
}

static void console_cb(AtsugamiConsole *console) {
	PQprintOpt options = {0};
	FILE *cachefile;
	char *cachepath, *buf;
	size_t bufsize, write_size;
	AtsugamiConsolePrivate *pte;

	pte = (AtsugamiConsolePrivate *) (console);
	pte->res = PQexec(conn, "SELECT conf_dir FROM public.settings;");
	cachepath = (char *) malloc(strlen(PQgetvalue(pte->res, 0, 0)) + 15);

	sprintf(cachepath, "%s/console_cache", PQgetvalue(pte->res, 0, 0));
	PQclear(pte->res);

	pte->res = PQexec(conn, gtk_entry_get_text(GTK_ENTRY(pte->entry)));
	cachefile = fopen(cachepath, "ab+");

	options.header		= 1;	/* Printoutput field headings and row count */
	options.align		= 1;	/* Fill align the fields */
	options.standard	= 1;	/* Old brain dead format */
	options.html3		= 0;	/* Output HTML tables */
	options.expanded	= 0;	/* Expand tables */
	options.pager		= 0;	/* Use pager for output if needed */
	options.fieldSep	= "|";	/* Field sparator */
	options.tableOpt	= 0;	/* Attributes for HTML table element */
	options.caption		= 0;	/* HTML table caption */

	/* Print the query result to a cache file */
	fseek(cachefile, 0, SEEK_SET);
	PQprint(cachefile, pte->res, &options);
	PQclear(pte->res);

	bufsize = ftell(cachefile);
	buf = (char *) malloc(bufsize);
	fseek(cachefile, 0, SEEK_SET);
	write_size = fread(buf, sizeof(char), bufsize, cachefile);

	gtk_text_buffer_set_text(pte->text_buffer, buf, write_size);
	gtk_entry_set_text(GTK_ENTRY(pte->entry), "");
	fclose(cachefile);
	free(buf);
	free(cachepath);
	buf = NULL;
	cachepath = NULL;
}

static void atsugami_console_init(AtsugamiConsole *console) {
	AtsugamiConsolePrivate *pte;
	GtkWidget *widget;
	int page_count;

	pte = (AtsugamiConsolePrivate *) (console);
	widget = GTK_WIDGET(console);
	pte->text_view = gtk_text_view_new();
	pte->text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(pte->text_view));
	pte->text_view = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	pte->entry = gtk_entry_new();
	pte->scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	page_count = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));

	/* Scrolled window */
	gtk_container_add(GTK_CONTAINER(pte->scrolled_window), pte->text_view);
	gtk_widget_set_vexpand(pte->text_view, TRUE);

	gtk_scrolled_window_set_propagate_natural_height(GTK_SCROLLED_WINDOW(pte->scrolled_window), TRUE);
	gtk_scrolled_window_set_kinetic_scrolling(GTK_SCROLLED_WINDOW(pte->scrolled_window), TRUE);
	gtk_widget_set_vexpand(pte->scrolled_window, TRUE);
	gtk_box_pack_start(GTK_BOX(pte->text_view), pte->scrolled_window, TRUE, TRUE, 0);

	/* Add pte->text_view to pte->text_view */
	gtk_container_set_border_width(GTK_CONTAINER(pte->text_view), 10);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(pte->text_view), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(pte->text_view), GTK_WRAP_WORD_CHAR);
	gtk_text_view_set_monospace(GTK_TEXT_VIEW(pte->text_view), TRUE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(pte->text_view), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(pte->text_view), GTK_WRAP_CHAR);

	/* Add pte->entry to pte->text_view */
	gtk_entry_set_activates_default(GTK_ENTRY(pte->entry), TRUE);
	gtk_widget_set_valign(pte->entry, GTK_ALIGN_START);
	gtk_entry_set_placeholder_text(GTK_ENTRY(pte->entry), "Enter a SQL command:");
	gtk_box_pack_start(GTK_BOX(pte->text_view), pte->entry, FALSE, FALSE, 0);

	pte->hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
	pte->widget = gtk_image_new_from_icon_name("utilities-terminal", GTK_ICON_SIZE_MENU);
	gtk_box_pack_start(GTK_BOX(pte->hbox), pte->widget, FALSE, FALSE, 4);

	pte->widget = gtk_label_new("Console");
	gtk_box_pack_start(GTK_BOX(pte->hbox), pte->widget, FALSE, FALSE, 4);

	pte->label_button = gtk_button_new_from_icon_name("window-close-symbolic", GTK_ICON_SIZE_MENU);

	/* Tab label */
	gtk_button_set_relief(GTK_BUTTON(pte->label_button), GTK_RELIEF_NONE);
//	gtk_widget_add_accelerator(pte->label_button, "clicked", accel, GDK_KEY_w, GDK_CONTROL_MASK, GTK_ACCEL_LOCKED);
//	g_signal_connect(GTK_BUTTON(pte->label_button), "clicked", G_CALLBACK(close_tab), NULL);

	gtk_box_pack_start(GTK_BOX(pte->hbox), pte->label_button, FALSE, FALSE, 4);

	gtk_widget_show_all(pte->hbox);
	gtk_widget_show_all(pte->text_view);
	gtk_container_add(GTK_CONTAINER(notebook), pte->text_view);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook), pte->text_view, pte->hbox);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), page_count);
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), pte->text_view, TRUE);
	g_signal_connect(pte->entry, "activate", G_CALLBACK(console_cb), NULL);
}
