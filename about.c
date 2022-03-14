/* about.c */
#include "atsugami.h"
#include <gtk/gtk.h>

void about_activate(void) {
	/* Normally, I wouldn't put a function like this on multiple lines, but this is very long. */
	gtk_show_about_dialog(GTK_WINDOW(window),
			      "program-name", "Atsugami",
			      "version", __VERSION,
			      "copyright", "© 2021-2022, Nate Morrison et al.",
			      //"website", "https://atsugami.moe",
			      "website", "https://github.com/natem-nvsd/atsugami",
			      //"authors", "Nate Morrison, fredgido",
			      "license", "Copyright (c) 2021-2022, Nate Morrison et al.\n All rights reserved.\n\n Redistribution and use in source and binary forms, with or without\n modification, are permitted provided that the following conditions are met:\n\n 1. Redistributions of source code must retain the above copyright notice, this\n list of conditions and the following disclaimer.\n 2. Redistributions in binary form must reproduce the above copyright notice,\n this list of conditions and the following disclaimer in the documentation\n and/or other materials provided with the distribution.\n\n THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND\n ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED\n WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR\n ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES\n (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\n LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND\n ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\n (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS\n SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n\n",
			      "logo-icon-name", "image-x-generic",
			      "title", "About Atsugami",
			      NULL);
}
