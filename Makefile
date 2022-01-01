include config.mk

TARGET=atsugami
#SRC=main.c about.c import.c wizard.c new_artist.c error_dialogs.c 
SRC=main.c about.c import.c wizard.c error_dialogs.c new_artist.c new_copyright.c new_character.c new_tag.c
OBJ=${SRC:.c=.o}

all: options atsugami

options:
	@echo atsugami-gtk build options:
	@echo "CFLAGS	= ${CFLAGS}"
	@echo "LDFLAGS	= ${LDFLAGS}"
	@echo "CC	= ${CC}"

.c.o:
	${CC} -c ${SRC} ${CFLAGS} $<

atsugami: clean
	${CC} -c ${SRC} ${CFLAGS} $<
	${CC} ${OBJ} -o atsugami ${LDFLAGS}
	rm -f ${OBJ}

clean:
	rm -f atsugami ${OBJ}

cleanbin:
	rm -f atsugami

cleanobj:
	rm -f ${OBJ}

install: atsugami 
	cp -f atsugami ${DESTDIR}${PREFIX}/bin
	strip ${DESTDIR}${PREFIX}/bin/atsugami
	chmod 755 ${DESTDIR}${PREFIX}/bin/atsugami
	strip -d ${DESTDIR}${PREFIX}/bin/atsugami
	mkdir -p ${DESTDIR}${PREFIX}/share/atsugami
	cp -f atsugami.glade ${DESTDIR}${PREFIX}/share/atsugami/
	chmod 644 ${DESTDIR}${PREFIX}/share/atsugami/atsugami.glade

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/atsugami

void:

#.PHONY: all #options clean install
