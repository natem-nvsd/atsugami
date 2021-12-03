# atsugami version
VERSION = 0.6

# Paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

X11INC = `pkg-config --cflags x11`
X11LIB = `pkg-config --libs x11`

GLIBINC = `pkg-config --cflags glib-2.0`
GLIBLIB = `pkg-config --libs glib-2.0`

GDKINC = -I/usr/local/include/gtk-2.0

GTKINC = `pkg-config --cflags gtk+-2.0 gcr-3`
GTKLIB = `pkg-config --libs gtk+-2.0 gcr-3`

CAIROINC = `pkg-config --cflags cairo`
CAIROLIB = `pkg-config --libs cairo`

GDKPIXBUFINC = `pkg-config --cflags gdk-pixbuf-2.0`
GDKPIXBUFLIB = `pkg-config --libs gdk-pixbuf-2.0`

ATKINC = `pkg-config --cflags atk`
ATKLIB = `pkg-config --libs atk`

# PostgreSQL libraries
LPQINC = /usr/local/include
LPQLIB = /usr/local/lib

# includes and libs
INCS = $(X11INC) $(GTKINC) $(GLIBINC) $(CAIROINC) $(GDKPIXBUFINC) $(ATKINC) $(GDKINC)
LIBS = $(X11LIB) $(GTKLIB) $(GLIBLIB) $(CAIROLIB) $(GDKPIXBUFLIB) $(ATKLIB) -lgthread-2.0

# flags
CPPFLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_POSIX_C_SOURCE=2 -DVERSION=\"${VERSION}\"
CFLAGS   = -g -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Os -D_FORTIFY_SOURCE=2 ${INCS} # Flags removed: -v
LDFLAGS  += ${LIBS} -lpq
#PQFLAGS = -lpq

# compiler and linker
CC = cc
