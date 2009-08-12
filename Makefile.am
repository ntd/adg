XSLTPROC=			`which xsltproc`

ACLOCAL_AMFLAGS=		-I m4
DISTCHECK_CONFIGURE_FLAGS=	--enable-gtk-doc

BUILT_SOURCES=			ChangeLog \
				README \
				NEWS \
				TODO

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
	@echo Updating $@
	@if test -d "$(srcdir)/.git"; then \
	  (cd "$(srcdir)" && git log --stat > $@); \
	else \
	  echo A git checkout is required to generate $@ >&2; \
	fi

# Update the README from the xml version
README: $(srcdir)/docs/gnu/docbook2txt.xsl $(srcdir)/docs/gnu/README.xml
	@echo Updating $@
	@if test -x $(XSLTPROC); then \
	  $(XSLTPROC) -o $@ $^; \
	else \
	  echo libxslt-1.0.19 or later is required in order to generate $@; \
	fi

# Update the NEWS from the xml version
NEWS: $(srcdir)/docs/gnu/docbook2txt.xsl $(srcdir)/docs/gnu/NEWS.xml
	@echo Updating $@
	@if test -x $(XSLTPROC); then \
	  $(XSLTPROC) -o $@ $^; \
	else \
	  echo libxslt-1.0.19 or later is required in order to generate $@; \
	fi

# Update the TODO from the xml version
TODO: $(srcdir)/docs/gnu/docbook2txt.xsl $(srcdir)/docs/gnu/TODO.xml
	@echo Updating $@
	@if test -x $(XSLTPROC); then \
	  $(XSLTPROC) -o $@ $^; \
	else \
	  echo libxslt-1.0.19 or later is required in order to generate $@; \
	fi
