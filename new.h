/* new.h */
#ifndef __IMPORT_H__
#define __IMPORT_H__

#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#pragma clang diagnostic ignored "-Wincompatible-pointer-types"

#include <glib.h>
#include <gtk/gtk.h>

extern void new_artist_activate(void);
extern void new_character_activate(void);
extern void new_copyright_activate(void);
extern void new_tag_activate(void);

#endif
