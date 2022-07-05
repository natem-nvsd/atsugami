/* colours.c
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
#include <libpq-fe.h>
#include <stdbool.h>

extern char *colour_getter(const int id) {
	char *colour;
	/* Colour IDs:
	 * 0	Artist
	 * 1	Copyright
	 * 2	Character
	 * 3	General & Pool
	 * 4	Meta
	 * 5	Deleted
	 * 6	Preview has children
	 * 7	Preview has parent
	 * 8	Parent notice
	 * 9	Child notice
	 * 10	Deleted notice
	 * 11	Resized
	 * 12	Low post count
	 */

	if (dark_theme == true) {
		switch (id) {
			case 0: {
				/* Artist */
				colour = "#ffff8a8a8b8b";
				break;
			}

			case 1: {
				/* Copyright */
				colour = "#c7c79797ffff";
				break;
			}

			case 2: {
				/* Character */
				colour = "#3535c6c64a4a";
				break;
			}

			case 3: {
				/* General */
				GtkStyleContext *contxt;
				GdkRGBA *col;

				contxt = gtk_style_context_new();
				col = NULL;

				gtk_style_context_get_color(contxt, GTK_STATE_FLAG_NORMAL, col);
				colour = gdk_rgba_to_string(col);
//				colour = "#00009b9be6e6";

				break;
			}

			case 4: {
				/* Meta */
				colour = "#eaead0d08483";
				break;
			}

			case 5: {
				/* Deleted */
				colour = "#ababababbcbc";
				break;
			}

			case 6: {
				/* Preview has children */
				colour = "#3535c6c64a4a";
				break;
			}

			case 7: {
				/* Preview has parent */
				colour = "#fdfd92920000";
				break;
			}

			case 8: {
				/* Parent notice */
				colour = "#000037370808";
				break;
			}

			case 9: {
				/* Child notice */
				colour = "#38382d2d0000";
				break;
			}

			case 10: {
				/* Deleted notice */
				colour = "#5c5c13131313";
				break;
			}

			case 11: {
				/* Resized notice */
				colour = "#424117176565";
				break;
			}

			case 12: {
				/* Tag low file count */
				colour = "#ffff5a5a5b5a";
				break;
			}
		}
	} else {
		switch (id) {
			case 0: {
				/* Artist */
				colour = "#c0c000000403";
				break;
			}

			case 1: {
				/* Copyright */
				colour = "#a8a80000aaaa";
				break;
			}

			case 2: {
				/* Character */
				colour = "#0000abab2c2c";
				break;
			}

			case 3: {
				/* General */
				GtkStyleContext *contxt;
				GdkRGBA *col;

				contxt = gtk_style_context_new();
				col = NULL;

				gtk_style_context_get_color(contxt, GTK_STATE_FLAG_NORMAL, col);
				colour = gdk_rgba_to_string(col);
//				colour = "#00007574f8f8";

				break;
			}

			case 4: {
				/* Meta */
				colour = "#fdfd92920000";
				break;
			}

			case 5: {
				/* Deleted */
				colour = "#1e1e1e1e2c2c";
				break;
			}

			case 6: {
				/* Preview has children */
				colour = "#3535c6c64a4a";
				break;
			}

			case 7: {
				/* Preview has parent */
				colour = "#ccccaaaa0000";
				break;
			}

			case 8: {
				/* Parent notice */
				colour = "#e8e8fcfce9e9";
				break;
			}

			case 9: {
				/* Child notice */
				colour = "#fdfdf6f6e6e6";
				break;
			}

			case 10: {
				/* Deleted notice */
				colour = "#fbfbe3e3e3e3";
				break;
			}

			case 11: {
				/* Resized notice */
				colour = "#f8f8e1e1f8f8";
				break;
			}

			case 12: {
				/* Tag low file count */
				colour = "#eded24232626";
				break;
			}
		}
	}

	return colour;
}
