MSG_GEN=			echo '  GEN   '$@;
MSG_XSLTERROR=			echo ' *WARN  libxslt-1.0.19 or later is required in order to generate '$@;
ACLOCAL_AMFLAGS=		-I m4
DISTCHECK_CONFIGURE_FLAGS=	--enable-gtk-doc

BUILT_SOURCES=			ChangeLog \
				README \
				NEWS \
				TODO \
				CONTRIBUTING

EXTRA_DIST=			ABOUT-NLS \
				config.rpath \
				COPYING \
				ChangeLog-0.2.1 \
				INSTALL \
				m4/ChangeLog \
				adg.doap \
				$(BUILT_SOURCES)

SUBDIRS=			cpml \
				adg \
				demo \
				docs \
				po

# Creating ChangeLog files from git log:
# idea stolen from cairo/build/Makefile.am.changelog
ChangeLog:
	@if test -d "$(srcdir)/.git"; then \
	  $(MSG_GEN)(cd "$(srcdir)" && git log --stat > $@); \
	else \
	  echo A git checkout is required to generate $@ >&2; \
	fi

# Autogenerate text docs
README: $(srcdir)/docs/gnu/docbook2txt.xsl $(srcdir)/docs/gnu/README.xml
NEWS: $(srcdir)/docs/gnu/docbook2txt.xsl $(srcdir)/docs/gnu/NEWS.xml
TODO: $(srcdir)/docs/gnu/docbook2txt.xsl $(srcdir)/docs/gnu/TODO.xml
CONTRIBUTING: $(srcdir)/docs/gnu/docbook2txt.xsl $(srcdir)/docs/gnu/CONTRIBUTING.xml
	@if test -x $(XSLTPROC); then \
	  $(MSG_GEN)$(XSLTPROC) -o $@ $^; \
	else \
	  $(MSG_XSLTERROR) \
	fi
