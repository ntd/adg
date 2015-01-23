include $(top_srcdir)/build/Makefile.am.common

SUBDIRS=			tests \
				cpml \
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
