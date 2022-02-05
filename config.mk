# config.mk

# Paths
PREFIX = /usr

GTKINC = `pkg-config --cflags gtk+-3.0`
GTKLIB = `pkg-config --libs gtk+-3.0`

GDKPIXBUFINC = `pkg-config --cflags gdk-pixbuf-2.0`
GDKPIXBUFLIB = `pkg-config --libs gdk-pixbuf-2.0`

# PostgreSQL libraries
LPQINC = `pkg-config --cflags libpq`
LPQLIB = `pkg-config --libs libpq`

# MagickWand libraries
MAGICKWANDINC = `pkg-config --cflags MagickWand`
MAGICKWANDLIB = `pkg-config --libs MagickWand`

# includes and libs
INCS = $(GTKINC) $(GDKPIXBUFINC) $(GDKINC) $(MAGICKWANDINC) $(LPQINC)
LIBS = $(GTKLIB) $(GDKPIXBUFLIB) $(LPQLIB) $(MAGICKWANDLIB) -lgthread-2.0

# flags
CFLAGS   = -g -std=c11 -pedantic -Wall -Wno-deprecated-declarations -D_FORTIFY_SOURCE=2 ${INCS}
LDFLAGS  += ${LIBS} -lpq -lmd

# compiler and linker
CC = cc
