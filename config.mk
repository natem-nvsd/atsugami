# config.mk

# Paths
PREFIX = /usr
MANPREFIX = ${PREFIX}/share/man

GTKINC = `pkg-config --cflags gtk+-3.0 gcr-3`
GTKLIB = `pkg-config --libs gtk+-3.0 gcr-3`

GDKPIXBUFINC = `pkg-config --cflags gdk-pixbuf-2.0`
GDKPIXBUFLIB = `pkg-config --libs gdk-pixbuf-2.0`

# PostgreSQL libraries
LPQINC = `pkg-config --cflags libpq`
LPQLIB = `pkg-config --libs libpq`

# includes and libs
INCS = $(X11INC) $(GTKINC) $(CAIROINC) $(GDKPIXBUFINC) $(GDKINC) $(LPQINC)
LIBS = $(X11LIB) $(GTKLIB) $(CAIROLIB) $(GDKPIXBUFLIB) $(LPQLIB) -lgthread-2.0

# flags
CFLAGS   = -g -std=c11 -pedantic -Wall -Wno-deprecated-declarations -D_FORTIFY_SOURCE=2 ${INCS}
LDFLAGS  += ${LIBS} -lpq -lmd

# compiler and linker
CC = cc
