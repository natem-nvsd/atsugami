include config.mk

SRC = atsugami-gtk.c #functions.c
OBJ = ${SRC:.c=.o}

all: options atsugami

options:
	@echo atsugami-gtk build options:
	@echo "CFLAGS	= ${CFLAGS}"
	@echo "LDFLAGS	= ${LDFLAGS}"
	@echo "CC	= ${CC}"

.c.o:
	${CC} -c ${SRC} ${CFLAGS} $<

atsugami: cleanobj
#	${CC} -c ${SRC} ${CFLAGS} $<
#	${CC} ${OBJ} -o atsugami ${LDFLAGS}
	cc -g -o atsugami atsugami-gtk.c  `pkg-config --cflags --libs gtk+-2.0`

clean:
	rm -f atsugami ${OBJ}

cleanobj:
	rm -f ${OBJ}

install: atsugami 
	cp -f atsugami ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/atsugami

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/atsugami

void:

#.PHONY: all #options clean install
