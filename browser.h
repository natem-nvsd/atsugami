/* browser.h
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

#ifndef ATSUGAMI_BROWSER_H
#define ATSUGAMI_BROWSER_H

#include <gtk/gtk.h>

G_BEGIN_DECLS


#define ATSUGAMI_TYPE_BROWSER                  (atsugami_browser_get_type())
#define ATSUGAMI_BROWSER(obj)			(G_TYPE_CHECK_INSTANCE_CAST((obj), ATSUGAMI_TYPE_BROWSER, AtsugamiBrowser))
#define ATSUGAMI_BROWSER_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass), ATSUGAMI_BROWSER, AtsugamiBrowserClass))
#define ATSUGAMI_IS_BROWSER(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), ATSUGAMI_TYPE_BROWSER))
#define ATSUGAMI_IS_BROWSER_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), ATSUGAMI_TYPE_BROWSER))
#define ATSUGAMI_BROWSER_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), G_TYPE_BROWSER, AtsugamiBrowserClass))


typedef struct _AtsugamiBrowser		AtsugamiBrowser;
typedef struct _AtsugamiBrowserPrivate	AtsugamiBrowserPrivate;
typedef struct _AtsugamiBrowserClass	AtsugamiBrowserClass;


struct _AtsugamiBrowserClass {
	GtkScrolledWindowClass parent_class;
};

struct _AtsugamiBrowser {
	GtkScrolledWindow container;
};

extern GType atsugami_browser_get_type(void) G_GNUC_CONST;
extern GtkWidget *atsugami_browser_new(void);

G_END_DECLS

#endif

