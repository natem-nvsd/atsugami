include config.mk

TARGET=atsugami
SRC=main.c about.c import.c wizard.c
OBJ=${SRC:.c=.o}

all: options atsugami

options:
	@echo atsugami-gtk build options:
	@echo "CFLAGS	= ${CFLAGS}"
	@echo "LDFLAGS	= ${LDFLAGS}"
	@echo "CC	= ${CC}"

.c.o:
	${CC} -c ${SRC} ${CFLAGS} $<

atsugami: cleanbin
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
	chmod 755 ${DESTDIR}${PREFIX}/bin/atsugami
	mkdir -p ${DESTDIR}${PREFIX}/share/atsugami
	cp -f atsugami.glade ${DESTDIR}${PREFIX}/share/atsugami/
	chmod 644 ${DESTDIR}${PREFIX}/share/atsugami/atsugami.glade

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/atsugami

void:

#.PHONY: all #options clean install
