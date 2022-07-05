/* atsugami.h
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

#ifndef ATSUGAMI_H_INCLUDED
#define ATSUGAMI_H_INCLUDED

#include "classes.h"
#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <stdbool.h>

#define __VERSION	"0.86.0"

/* Tags */
#define ARTIST		0
#define COPYRIGHT	1
#define CHARACTER	2
#define GENERAL		3
#define META		4
#define DELETED		5
#define CHILD_PREVIEW	6
#define PARENT_PREVIEW	7
#define PARENT_NOTICE	8
#define CHILD_NOTICE	9
#define DELETED_NOTICE	10
#define RESIZED		11
#define LOW_COUNT	12
#define POOL		3

enum {
	COMPLETION_MODEL_NAME_COLUMN,
	COMPLETION_MODEL_CATEGORY_COLUMN,
	COMPLETION_MODEL_N_COLUMN
};

enum {
        SHA256_COL,
        COL_PIXBUF,
        TOOLTIP_COL,
        NUM_COLS
};

/** Variables **/
extern GtkWidget *notebook;
extern GtkWidget *status_bar;
extern PGconn *conn;
extern bool safe_mode;
extern bool dark_theme;
extern int msg_id;

/* Icon sizes */
extern GtkWidget *small_mi;
extern GtkWidget *medium_mi;
extern GtkWidget *large_mi;
extern GtkWidget *huge_mi;
extern GtkWidget *gigantic_mi;
extern GtkWidget *safe_mode_mi;

/** Functions **/
extern void check_safe_mode(void);
extern char *colour_getter(const int id);
extern void dbg_err(const char *msg);
extern void dbg_info(const char *msg);
extern void dbg_print(const char *msg);
extern void dbg_warn(const char *msg);
extern void file_count(void);
extern void file_count_update(void);
extern char *get_file_path(void);
extern char *get_icon_size_name(void);
extern char *get_thumb_dir(void);
extern char *get_thumb_path(void);
extern char *get_thumb_path_by_id(const int id);
extern int get_icon_size(void);
extern void quit_activate(void);
extern void safe_mode_toggle(void);
extern char *search_input_sanitiser(char *str);
extern void set_icon_menu_items(void);
extern void set_icon_size(gpointer user_data);
extern char *tag_input_sanitiser(char *str);
extern long word_count(const char *str);

#endif
