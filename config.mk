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

# json-c
JSONINC = `pkg-config --cflags json-c`
JSONLIB = `pkg-config --libs json-c`

# libcurl
CURLINC = `pkg-config --cflags libcurl`
CURLLIB = `pkg-config --libs libcurl`

# PCRE2
PCRE2INC = `pcre2-config --cflags-posix`
PCRE2LIB = `pcre2-config --libs-posix`

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
INCS = $(CURLINC) $(GTKINC) $(GDKPIXBUFINC) $(GTKIMAGEVIEWINC) $(JSONINC) $(LPQINC) $(MDINC) $(PCRE2INC)
LIBS = $(CURLLIB) $(GTKLIB) $(GDKPIXBUFLIB) $(GTKIMAGEVIEWLIB) $(JSONLIB) $(LPQLIB) $(MDLIB) $(PCRE2LIB) -lgthread-2.0

# flags
CFLAGS = -g -std=c11 -pedantic -Wall -O -D_FORTIFY_SOURCE=2 ${INCS}
LDFLAGS += ${LIBS}

# compiler and linker
CC = cc
