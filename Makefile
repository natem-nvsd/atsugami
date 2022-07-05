include config.mk

TARGET=atsugami
# Sort this alphabetically later
SRC = about.c colours.c completion_model.c console.c file_count.c icon_size.c importers.c lex.c main.c misc.c path.c pixbuf.c search.c tab_completion.c tag_list.c view.c
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

install-only:
	cp -f atsugami ${DESTDIR}${PREFIX}/bin
	strip ${DESTDIR}${PREFIX}/bin/atsugami
	chmod 755 ${DESTDIR}${PREFIX}/bin/atsugami
	strip -d ${DESTDIR}${PREFIX}/bin/atsugami

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/atsugami

void:

#.PHONY: all #options clean install
