include config.mk

TARGET=atsugami
SRC=gtk.c
OBJ=${SRC:.c=.o}

all: options atsugami

options:
	@echo atsugami-gtk build options:
	@echo "CFLAGS	= ${CFLAGS}"
	@echo "LDFLAGS	= ${LDFLAGS}"
	@echo "CC	= ${CC}"

.c.o:
	${CC} -c ${SRC} ${CFLAGS} $<

atsugami: cleanobj
	${CC} -c ${SRC} ${CFLAGS} $<
	${CC} ${OBJ} -o atsugami ${LDFLAGS}

clean:
	rm -f atsugami ${OBJ}

cleanobj:
	rm -f ${OBJ}

install: atsugami 
	cp -f atsugami ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/atsugami
	mkdir ${DESTDIR}${PREFIX}/share/atsugami
	cp -f src/new_atsugami.glade ${DESTDIR}${PREFIX}/share/atsugami
	chmod 644 ${DESTDIR}${PREFIX}/share/atsugami/new_atsugami.glade

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/atsugami

void:

#.PHONY: all #options clean install
