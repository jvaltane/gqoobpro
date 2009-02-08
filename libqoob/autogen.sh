#!/bin/sh

(glibtoolize --version) < /dev/null > /dev/null 2>&1 && LIBTOOLIZE=glibtoolize || LIBTOOLIZE=libtoolize

set -x
$LIBTOOLIZE --force  --automake
aclocal
autoconf
autoheader
automake --add-missing --foreign
