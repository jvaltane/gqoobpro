lib_LTLIBRARIES = libqoob.la


libqoob_la_SOURCES =  qoob.c			\
		      qoob-usb.c		\
		      qoob-error.c		\
		      qoob-file.c

libqoob_la_LDFLAGS = $(libusb_LIBS)

libqoob_includedir = $(includedir)/libqoob

libqoob_include_HEADERS = qoob.h		\
			  qoob-usb.h		\
			  qoob-error.h		\
			  qoob-file.h		\
			  qoob-defaults.h

AM_CFLAGS = $(debug_CFLAGS)			\
	    $(libusb_CFLAGS)
