# config.mk

# Paths
PREFIX = /usr/local

# GTK libraries
GTKINC = `pkg-config --cflags gtk+-3.0`
GTKLIB = `pkg-config --libs gtk+-3.0`

# GDK Pixbuf libraries
GDKPIXBUFINC = `pkg-config --cflags gdk-pixbuf-2.0`
GDKPIXBUFLIB = `pkg-config --libs gdk-pixbuf-2.0`

# GtkImageView
GTKIMAGEVIEWINC = `pkg-config --cflags gtkimageview`
#GTKIMAGEVIEWLIB = `pkg-config --libs gtkimageview`
GTKIMAGEVIEWLIB = -L/usr/local/lib -l:gtkimageview.so

# PostgreSQL libraries
LPQINC = `pkg-config --cflags libpq`
LPQLIB = `pkg-config --libs libpq`

# libmd libraries (Linux)
#MDINC = `pkg-config --cflags libmd`
#MDLIB = `pkg-config --libs libmd`
# (Free)BSD
MDINC = -lmd
MDLIB = -lmd

# includes and libs
INCS = $(GTKINC) $(GDKPIXBUFINC) $(GDKINC) $(GTKIMAGEVIEWINC) 
LIBS = $(GTKLIB) $(GDKPIXBUFLIB) $(LPQLIB) $(GTKIMAGEVIEWLIB) $(MDLIB) -lgthread-2.0

# flags
CFLAGS = -g -std=c11 -pedantic -Wall -O -D_FORTIFY_SOURCE=2 ${INCS}
LDFLAGS += ${LIBS}

# compiler and linker
CC = cc
