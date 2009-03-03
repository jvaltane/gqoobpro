SUBDIRS = src

pcfiles = libqoob.pc

all-local: $(pcfiles)

%.pc: %.pc
	cp $< $@

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = $(pcfiles)

EXTRA_DIST = libqoob.pc.in autogen.sh data/86-qoobpro.rules \
	data/qoobpro-usbhid-ignore TODO README COPYING NEWS ChangeLog \
        packaging/debian/changelog packaging/debian/compat \
	packaging/debian/control packaging/debian/copyright \
	packaging/debian/rules
CLEANFILES = $(pcfiles) $(pcfiles_uninstalled)
