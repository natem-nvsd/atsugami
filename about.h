/* about.h
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

#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED

#include <gtk/gtk.h>
#include <libpq-fe.h>

#define ATSUGAMI_TYPE_ABOUT		(atsugami_about_get_type())
#define ATSUGAMI_ABOUT(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), ATSUGAMI_TYPE_ABOUT, AtsugamiAbout))
#define ATSUGAMI_ABOUT_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), ATUSGAMI_TYPE_ABOUT, AtsugamiAboutClass))
#define ATSUGAMI_IS_ABOUT(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), ATSUGAMI_TYPE_ABOUT))
#define ATSGUAMI_IS_ABOUT_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), ATSUGAMI_TYPE_ABOUT))
#define ATSUGAMI_ABOUT_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), ATSUGAMI_TYPE_ABOUT, AtsugamiAboutClass))


typedef struct _AtsugamiAbout		AtsugamiAbout;
typedef struct _AtsugamiAboutPrivate	AtsugamiAboutPrivate;
typedef struct _AtsugamiAboutClass	AtsugamiAboutClass;


struct _AtsugamiAbout {
	GtkWidget parent_instance;
};

struct _AtsugamiAboutPrivate {
	GtkWidget *widget;
	GtkWidget *window;
	const char *contributers[1];
};

struct _AtsugamiAboutClass {
	GtkWidgetClass parent_class;
};


extern GtkWidget *atsugami_about_new(void);
static void atsugami_about_finalize(GObject *object);

#endif
