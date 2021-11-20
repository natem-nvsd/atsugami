# atsugami version
VERSION = 0.10

# Paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

# PostgreSQL libraries
LPQINC = /usr/local/include
LPQLIB = /usr/local/lib

# includes and libs
INCS = -I${LPQINC}
LIBS = -L${LPQLIB}

# flags
CPPFLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_POSIX_C_SOURCE=2 -DVERSION=\"${VERSION}\"
CFLAGS   = -g -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Os -D_FORTIFY_SOURCE=2 ${INCS} # Flags removed: -v
LDFLAGS  += ${LIBS} -lpq
#PQFLAGS = -lpq

# compiler and linker
CC = cc
