include config.mk

TARGET=atsugami
# Redo compilation order to optimize compile times
SRC=main.c misc.c about.c console.c file_count.c import_wizard.c import.c new_tag.c notebook.c list_tags.c tab.c types.c wiki.c viewer.c search.c lex.c tab_completion.c icon_size.c path.c completion.c
OBJ=${SRC:.c=.o}

all: options atsugami

options:
	@echo atsugami build options:
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

install-only:
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
