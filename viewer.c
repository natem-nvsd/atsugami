/* viewer.c: Built-in image viewer */
#include "atsugami.h"
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>

GtkWidget *viewer_vbox;

static void close_tab(void) {
	gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), viewer_vbox);

	if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook)) == 0)
		quit_activate();
}

extern GtkWidget *viewer(GtkWidget *parent_tag_tv, GtkTextBuffer *parent_tag_tb, char *sha256[65]) {
	PGresult *viewer_res;
	GtkWidget *viewer_scrolled_window, *viewer_tv, *viewer_image, *viewer_viewport, *tab_label_box, *tab_icon, *tab_label, *tab_close;
	GtkTextBuffer *viewer_tb;
	GtkTextIter start_iter, end_iter;
	GtkTextTag *title, *sub_title;
	char *buffer = NULL, *tag_id, query[111], *path = NULL;
	size_t buf_size;

	viewer_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	viewer_scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	viewer_tv = gtk_text_view_new();
	viewer_tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(viewer_tv));
	viewer_viewport = gtk_viewport_new(NULL, NULL);
	title = gtk_text_buffer_create_tag(viewer_tb, "title", "scale", 1.5, "weight", "bold", NULL);
	//sub_title = gtk_text_buffer_create_tag(viewer_tb, "sub_title", "scale", 1.2, "weight", "semibold", "style", "italic", NULL);		// FIXME
	sub_title = gtk_text_buffer_create_tag(viewer_tb, "sub_title", "scale", 1.2, "weight", "semibold", NULL);

	/* Set tag_id */
	sprintf(query, "SELECT id FROM public.files WHERE sha256 = '%s';", *sha256);
	viewer_res = PQexec(conn, query);
	tag_id = (char *) malloc(sizeof(long));
	strcpy(tag_id, PQgetvalue(viewer_res, 0, 0));
	PQclear(viewer_res);

	/* Set image */
	viewer_res = PQexec(conn, "SELECT store_dir FROM public.settings;");
	buffer = PQgetvalue(viewer_res, 0, 0);
	path = (char *) malloc(strlen(buffer) + 66);
	sprintf(path, "%s/%s", buffer, *sha256);
	viewer_image = gtk_image_new_from_file(path);
	free(path);

	/* Add widgets to the vbox */
	gtk_box_pack_start(GTK_BOX(viewer_vbox), viewer_scrolled_window, TRUE, TRUE, 0);

	/* Image viewer */
	gtk_container_add(GTK_CONTAINER(viewer_scrolled_window), viewer_viewport);
	gtk_container_add(GTK_CONTAINER(viewer_viewport), viewer_image);

	/* Set Text view properties */
	gtk_text_view_set_editable(GTK_TEXT_VIEW(viewer_tv), FALSE);
	gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(viewer_tv), FALSE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(viewer_tv), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(viewer_tv), GTK_WRAP_WORD_CHAR);
	gtk_text_view_set_top_margin(GTK_TEXT_VIEW(viewer_tv), 8);
	gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(viewer_tv), 8);
	gtk_text_view_set_left_margin(GTK_TEXT_VIEW(viewer_tv), 8);
	gtk_text_view_set_right_margin(GTK_TEXT_VIEW(viewer_tv), 8);

	/* Fill the text buffer */
	sprintf(query, "SELECT commentary FROM public.commentary WHERE file_id = %s;", tag_id);
	viewer_res = PQexec(conn, query);

	if (PQntuples(viewer_res) == 0)
		buf_size = 0;
	else
		buf_size = strlen(PQgetvalue(viewer_res, 0, 0));

	if (buf_size > 0) {
		gtk_box_pack_start(GTK_BOX(viewer_vbox), viewer_tv, TRUE, TRUE, 8);
		buffer = (char *) malloc(buf_size + 33);

		sprintf(buffer, "Artist's commentary\nOriginal\n%s\n", PQgetvalue(viewer_res, 0, 0));
		gtk_text_buffer_set_text(viewer_tb, buffer, -1);
		free(buffer);
		free(tag_id);

		/* Apply tags */
		gtk_text_buffer_get_iter_at_offset(viewer_tb, &start_iter, 0);
		gtk_text_buffer_get_iter_at_offset(viewer_tb, &end_iter, 20);
		gtk_text_buffer_apply_tag(viewer_tb, title, &start_iter, &end_iter);

		gtk_text_buffer_get_iter_at_offset(viewer_tb, &start_iter, 20);
		gtk_text_buffer_get_iter_at_offset(viewer_tb, &end_iter, 29);
		gtk_text_buffer_apply_tag(viewer_tb, sub_title, &start_iter, &end_iter);
	}

	tab_label_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
	tab_icon = gtk_image_new_from_icon_name("image-x-generic", GTK_ICON_SIZE_MENU);
	tab_label = gtk_label_new("Viewer");
	tab_close = gtk_button_new_from_icon_name("window-close-symbolic", GTK_ICON_SIZE_MENU);

	/* Tab label */
	gtk_button_set_relief(GTK_BUTTON(tab_close), GTK_RELIEF_NONE);
	gtk_widget_add_accelerator(tab_close, "clicked", accel, GDK_KEY_w, GDK_CONTROL_MASK, GTK_ACCEL_LOCKED);
	g_signal_connect(GTK_BUTTON(tab_close), "clicked", G_CALLBACK(close_tab), NULL);

	gtk_box_pack_start(GTK_BOX(tab_label_box), tab_icon, FALSE, FALSE, 4);
	gtk_box_pack_start(GTK_BOX(tab_label_box), tab_label, FALSE, FALSE, 4);
	gtk_box_pack_start(GTK_BOX(tab_label_box), tab_close, FALSE, FALSE, 4);
	
	gtk_container_add(GTK_CONTAINER(notebook), viewer_vbox);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook), viewer_vbox, tab_label_box);

	gtk_widget_show_all(tab_label_box);
	gtk_widget_show_all(viewer_vbox);
	return viewer_vbox;
}
