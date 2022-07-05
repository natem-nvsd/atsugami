/* about.c
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
 * */

#include "atsugami.h"
#include "about.h"
#include "copyright.h"

static void atsugami_about_finalize(GObject *object);

G_DEFINE_TYPE_WITH_CODE(AtsugamiAbout, atsugami_about, GTK_TYPE_WIDGET, G_ADD_PRIVATE(AtsugamiAbout) G_IMPLEMENT_INTERFACE(GTK_TYPE_WINDOW, NULL))

static void atsugami_about_class_init(AtsugamiAboutClass *about_class) {
	GObjectClass *object_class;
	GtkWidgetClass *widget_class;

	object_class = G_OBJECT_CLASS(about_class);
	widget_class = GTK_WIDGET_CLASS(about_class);

	object_class->finalize = atsugami_about_finalize;
//	widget_class->draw
//	widget_class->realize
//	widget_class->unrealize
//	widget_class->size_allocate
//	widget_class->map
//	widget_class->unmap
}

extern void atsugami_about_init(AtsugamiAbout *about) {
	AtsugamiAboutPrivate *pte;
	GtkWidget *widget;

	pte = atsugami_about_get_instance_private(about);

	pte->contributers[0] = "Nate Morrison";
	pte->window = gtk_about_dialog_new();
	
	gtk_about_dialog_set_logo_icon_name(GTK_ABOUT_DIALOG(pte->window), "image-x-generic");
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(pte->window), "Atsugami");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(pte->window), __VERSION);
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(pte->window), "© 2021-2022 by Nate Morrison et al.");
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(pte->window), "https://github.com/natem-nvsd/atsugami");
	//gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(pte->window), contributers_ptr);
	gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(pte->window), LICENSE);
	gtk_about_dialog_set_wrap_license(GTK_ABOUT_DIALOG(pte->window), TRUE);
}

static void atsugami_about_finalize(GObject *object) {
	AtsugamiAbout *about;
	AtsugamiAboutPrivate *pte;

	about = (AtsugamiAbout *) object;
	pte = (AtsugamiAboutPrivate *) (about);

	g_clear_object(&pte->widget);
	g_clear_object(&pte->window);
	G_OBJECT_CLASS(atsugami_about_parent_class)->finalize(object);
}
