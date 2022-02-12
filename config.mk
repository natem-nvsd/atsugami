# config.mk

# Paths
PREFIX = /usr/local

# GTK libraries
GTKINC = `pkg-config --cflags gtk+-3.0`
GTKLIB = `pkg-config --libs gtk+-3.0`

# GDK Pixbuf libraries
GDKPIXBUFINC = `pkg-config --cflags gdk-pixbuf-2.0`
GDKPIXBUFLIB = `pkg-config --libs gdk-pixbuf-2.0`

# PostgreSQL libraries
LPQINC = `pkg-config --cflags libpq`
LPQLIB = `pkg-config --libs libpq`

# libmd libraries 
MDINC = `pkg-config --cflags libmd`
MDLIB = `pkg-config --libs libmd`

# includes and libs
INCS = $(GTKINC) $(GDKPIXBUFINC) $(GDKINC) $(LPQINC) $(MDINC)
LIBS = $(GTKLIB) $(GDKPIXBUFLIB) $(LPQLIB) $(LPQLIB) $(MDLIB) -lgthread-2.0

# flags
CFLAGS   = -g -std=c11 -pedantic -Wall -O -Wno-deprecated-declarations -D_FORTIFY_SOURCE=2 ${INCS}
LDFLAGS  += ${LIBS}

# compiler and linker
#CC = cc
CC = clang
