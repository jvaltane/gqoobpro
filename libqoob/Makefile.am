SUBDIRS = src

pcfiles = libqoob.pc

all-local: $(pcfiles)

%.pc: %.pc
	cp $< $@

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = $(pcfiles)

EXTRA_DIST = libqoob.pc.in autogen.sh data/86-qoobpro.rules TODO README COPYING

CLEANFILES = $(pcfiles) $(pcfiles_uninstalled)
