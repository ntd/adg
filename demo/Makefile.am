include $(top_srcdir)/build/Makefile.am.common

AM_CPPFLAGS=			-I$(top_srcdir)/src \
				-I$(top_builddir)/src \
				-DPKGDATADIR='"$(pkgdatadir)"' \
				-DSRCDIR='"$(abs_srcdir)"' \
				-DBUILDDIR='"$(abs_builddir)"'
AM_CFLAGS=			$(GTK3_CFLAGS) $(GTK2_CFLAGS) \
				$(CAIRO_CFLAGS) \
				$(GOBJECT2_CFLAGS)
LDADD=				$(top_builddir)/src/cpml/libcpml-1.la \
				$(top_builddir)/src/adg/libadg-1.la

# On Windows, do not spawn the command prompt window (shown by default)
# by setting a flag in the PE header of the executables
if OS_WINDOWS
AM_CFLAGS+=			-mwindows
endif


bin_PROGRAMS=			adg-demo-uninstalled \
				cpml-demo-uninstalled

adg_demo_uninstalled_SOURCES=	adg-demo.c \
				demo.c \
				demo.h

if HAVE_GTK3
cpml_demo_uninstalled_SOURCES=	cpml-demo.c \
				demo.c \
				demo.h
endif

if HAVE_GTK2
cpml_demo_uninstalled_SOURCES=	cpml-demo-gtk2.c \
				demo.c \
				demo.h
endif

nodist_pkgdata_DATA=		adg-demo.ui \
				cpml-demo.ui
dist_pkgdata_DATA=		adg-16.png \
				adg-32.png \
				adg-48.png \
				adg-64.png \
				adg-128.png


EXTRA_DIST=			cpml-demo.ui.in \
				adg-demo.ui.in


# adg-demo-uninstalled and cpml-demo-uninstalled are renamed after installation:
# the uninstalled version should not be checked for --help and --version.
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT= \
				adg-demo-uninstalled$(EXEEXT) \
				cpml-demo-uninstalled$(EXEEXT)

install-exec-hook:
	$(AM_V_at)cd $(DESTDIR)$(bindir) ; \
	mv -f adg-demo-uninstalled$(EXEEXT) adg-demo$(EXEEXT) ; \
	mv -f cpml-demo-uninstalled$(EXEEXT) cpml-demo$(EXEEXT)

uninstall-hook:
	$(AM_V_at)cd $(DESTDIR)$(bindir) ; \
	rm -f adg-demo$(EXEEXT) ; \
	rm -f cpml-demo$(EXEEXT)
