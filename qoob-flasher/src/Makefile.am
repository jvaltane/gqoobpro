bin_PROGRAMS = qoob-flasher

qoob_flasher_SOURCES = qoob-flasher.c qoob-flasher-util.c

noinst_HEADERS = qoob-flasher-util.h

qoob_flasher_LDADD = $(libqoob_LIBS)

AM_CFLAGS = 	$(debug_CFLAGS)		\
		$(libqoob_CFLAGS)

man_MANS = qoob-flasher.1

EXTRA_DIST = qoob-flasher.1
