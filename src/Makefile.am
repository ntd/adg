include $(top_srcdir)/build/Makefile.am.common
SUBDIRS=			cpml \
				adg
if HAVE_GTK2
SUBDIRS+=			adg-gtk
endif

if HAVE_GLADE
adg_catalogdir=			$(GLADE_CATALOGDIR)
adg_catalog_DATA=		adg.xml
endif

libadg_includedir=		$(includedir)/adg-1
libadg_include_DATA=		adg.h \
				adg-gtk.h \
				cpml.h

pkgconfig_DATA=			cpml-1.pc \
				adg-1.pc \
				adg-gtk-1.pc

EXTRA_DIST=			adg.h \
				adg-gtk.h \
				cpml.h \
				adg.xml \
				cpml-1.pc.in \
				adg-1.pc.in \
				adg-gtk-1.pc.in
