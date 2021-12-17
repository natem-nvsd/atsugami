# atsugami version
VERSION = 0.6

# Paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

GDKINC = -I/usr/local/include/gtk-2.0

GTKINC = `pkg-config --cflags gtk+-3.0 gcr-3`
GTKLIB = `pkg-config --libs gtk+-3.0 gcr-3`

GDKPIXBUFINC = `pkg-config --cflags gdk-pixbuf-2.0`
GDKPIXBUFLIB = `pkg-config --libs gdk-pixbuf-2.0`

#ATKINC = `pkg-config --cflags atk`
#ATKLIB = `pkg-config --libs atk`

# PostgreSQL libraries
LPQINC = /usr/local/include
LPQLIB = /usr/local/lib

# includes and libs
INCS = $(X11INC) $(GTKINC) $(CAIROINC) $(GDKPIXBUFINC) $(GDKINC)
LIBS = $(X11LIB) $(GTKLIB) $(CAIROLIB) $(GDKPIXBUFLIB) -lgthread-2.0

# flags
CPPFLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_POSIX_C_SOURCE=2 -DVERSION=\"${VERSION}\"
								    # Gentoo users: "Muh optimizations"
#CFLAGS   = -g -std=c11 -pedantic -Wall -Wno-deprecated-declarations -O3 -D_FORTIFY_SOURCE=2 ${INCS} # Flags removed: -v
CFLAGS   = -g -std=c11 -pedantic -Wall -Wno-deprecated-declarations -D_FORTIFY_SOURCE=2 ${INCS} # Flags removed: -v
LDFLAGS  += ${LIBS} -lpq
#PQFLAGS = -lpq

# compiler and linker
CC = cc
