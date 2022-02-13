include $(top_srcdir)/autotools/Makefile.am.common


SUBDIRS=
if HAVE_TEST_FRAMEWORK
SUBDIRS+=			tests
endif
SUBDIRS+=			cpml \
				adg


if HAVE_GLADE
adg_catalogdir=			$(GLADE_CATALOGDIR)
adg_catalog_DATA=		adg.xml
endif

libadg_includedir=		$(includedir)/adg-1
libadg_include_DATA=		adg.h \
				cpml.h

EXTRA_DIST=			adg.h \
				cpml.h \
				adg.xml
