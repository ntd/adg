## Process this file with automake to produce Makefile.in.

ACLOCAL_AMFLAGS=	-I m4


EXTRA_DIST=		ABOUT-NLS \
			AUTHORS \
			config.rpath \
			COPYING \
			ChangeLog \
			INSTALL \
			m4/ChangeLog \
			NEWS \
			README

SUBDIRS=		cpml \
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
