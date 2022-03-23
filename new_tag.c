/* new_tag.c */
#include "atsugami.h"
#include "import.h"
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdio.h>

PGresult *tagres;
GtkWidget *tag_entry, *combo_box_entry, *tag_vbox;

static void close_tab(void) {
	gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), tag_vbox);

	if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook)) == 0)
		quit_activate();
}

static int apply_cb(void) {
	size_t txt_size = strlen(gtk_entry_get_text(GTK_ENTRY(tag_entry)));
	char *text, *tag_query, category[2];

	text = (char *) malloc(txt_size);
	tag_query = (char *) malloc(80 + txt_size);
	tagres = PQexec(conn, "BEGIN TRANSACTION;");
	strcpy(category, gtk_entry_get_text(GTK_ENTRY(combo_box_entry)));
	strcpy(text, gtk_entry_get_text(GTK_ENTRY(tag_entry)));

	PQclear(tagres);
	sprintf(tag_query, "INSERT INTO public.tags (name) VALUES ('%s');", text);
	tagres = PQexec(conn, tag_query);

	if (PQresultStatus(tagres) != PGRES_COMMAND_OK) {
		PQclear(tagres);

		tagres = PQexec(conn, "ROLLBACK TRANSACTION;");
		PQclear(tagres);
		gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), tag_vbox);
		free(text);
		free(tag_query);

		return 1;
	}

	sprintf(tag_query, "SELECT id FROM public.tags WHERE name LIKE '%s';", text);
	tagres = PQexec(conn, tag_query);
	sprintf(tag_query, "INSERT INTO public.tags_categories (tag_id, category_id) VALUES (%s, %s);", PQgetvalue(tagres, 0, 0), category);
	PQclear(tagres);
	tagres = PQexec(conn, tag_query);

	if (PQresultStatus(tagres) != PGRES_COMMAND_OK) {
		PQclear(tagres);

		tagres = PQexec(conn, "ROLLBACK TRANSACTION;");
		PQclear(tagres);
		gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), tag_vbox);
		free(text);
		free(tag_query);

		return 1;
	}

	if (PQresultStatus(tagres) == PGRES_COMMAND_OK) {
		PQclear(tagres);

		tagres = PQexec(conn, "COMMIT TRANSACTION;");
		PQclear(tagres);
		free(text);
		free(tag_query);
		gtk_notebook_detach_tab(GTK_NOTEBOOK(notebook), tag_vbox);
	}

	return 0;
}

extern void new_tag_activate(void) {
	GtkWidget *label, *combo_box, *ok_button, *tab_label_box, *tab_icon, *tab_label, *tab_close;
	gint page_count = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
	
	tag_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_container_set_border_width(GTK_CONTAINER(tag_vbox), 10);
	gtk_box_set_baseline_position(GTK_BOX(tag_vbox), GTK_BASELINE_POSITION_TOP);
	
	/* Label */
	label = gtk_label_new("Select a category and enter a value.");
	gtk_widget_set_valign(label, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(tag_vbox), label, FALSE, FALSE, 0);

	/* Combo box */
	combo_box = gtk_combo_box_text_new();
	combo_box_entry = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(tag_vbox), combo_box, FALSE, FALSE, 0);
	gtk_entry_set_activates_default(GTK_ENTRY(combo_box_entry), TRUE);
	g_object_bind_property(combo_box, "active-id", combo_box_entry, "text", G_BINDING_DEFAULT);

	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo_box), "0", "Artist");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo_box), "2", "Character");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo_box), "1", "Copyright");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo_box), "3", "General");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo_box), "4", "Meta");
	
	/* Entry */
	tag_entry = gtk_entry_new();
	gtk_entry_set_activates_default(GTK_ENTRY(tag_entry), TRUE);
	gtk_widget_set_valign(tag_entry, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(tag_vbox), tag_entry, FALSE, FALSE, 0);
	
	/* Ok button */
	ok_button = gtk_button_new_with_label("Add");
	gtk_box_pack_start(GTK_BOX(tag_vbox), ok_button, FALSE, FALSE, 0);
	g_signal_connect(ok_button, "clicked", G_CALLBACK(apply_cb), NULL);

	/* Tab label */
	tab_label_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
	tab_icon = gtk_image_new_from_icon_name("list-add", GTK_ICON_SIZE_MENU);
	tab_label = gtk_label_new("New tag");
	tab_close = gtk_button_new_from_icon_name("window-close-symbolic", GTK_ICON_SIZE_MENU);

	gtk_button_set_relief(GTK_BUTTON(tab_close), GTK_RELIEF_NONE);
	gtk_widget_add_accelerator(tab_close, "clicked", accel, GDK_KEY_w, GDK_CONTROL_MASK, GTK_ACCEL_LOCKED);
	g_signal_connect(GTK_BUTTON(tab_close), "clicked", G_CALLBACK(close_tab), NULL);

	gtk_box_pack_start(GTK_BOX(tab_label_box), tab_icon, FALSE, FALSE, 4);
	gtk_box_pack_start(GTK_BOX(tab_label_box), tab_label, FALSE, FALSE, 4);
	gtk_box_pack_start(GTK_BOX(tab_label_box), tab_close, FALSE, FALSE, 4);

	gtk_widget_show_all(tag_vbox);
	gtk_widget_show_all(tab_label_box);
	gtk_container_add(GTK_CONTAINER(notebook), tag_vbox);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook), tag_vbox, tab_label_box);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), page_count);
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), tag_vbox, TRUE);
}
