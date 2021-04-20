/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2020  Nicola Fontana <ntd at entidi.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#include "demo.h"
#include <string.h>


#if GLIB_CHECK_VERSION(2, 58, 0)

static gchar *
_demo_absolutepath(const gchar *path)
{
    return g_canonicalize_filename(path, NULL);
}

#else

#include <limits.h>
#include <stdlib.h>

static gchar *
_demo_absolutepath(const gchar *path)
{
    char abspath[PATH_MAX];
    return g_strdup(realpath(path, abspath));
}

#endif


/* Base folder all installed files should be referred to. */
static gchar *pkg_data_dir = NULL;


static gchar *
_demo_basedir(const gchar *program)
{
    gchar *dir, *absdir;

    dir = g_path_get_dirname(program);
    if (dir == NULL) {
        return NULL;
    }

    absdir = _demo_absolutepath(dir);
    g_free(dir);
    return absdir;
}

void
_demo_init(gint argc, gchar *argv[])
{
    gchar *basedir;

    if (argc < 1 || argv == NULL || argv[0] == NULL) {
        g_error(_("Invalid arguments: arg[0] not set"));
        g_assert_not_reached();
    }

    basedir = _demo_basedir(argv[0]);
    if (basedir == NULL) {
        /* This should never happen but... just in case */
        pkg_data_dir = ".";
        return;
    }

    g_print("basedir = '%s'\nbuilddir= '%s'\n", basedir, BUILDDIR);
    if (! g_str_has_prefix(basedir, BUILDDIR)) {
        /* Installed program: set pkg_data_dir */
#ifdef G_OS_WIN32
        gchar *parent = g_path_get_dirname(dirname);
        /* XXX: `pkg_data_dir` will be leaked... who cares? */
        pkg_data_dir = g_build_filename(parent, PKGDATADIR, NULL);
        g_free(parent);
#else
        pkg_data_dir = PKGDATADIR;
#endif
    }
    g_free(basedir);
}

gchar *
_demo_file(const gchar *file_name)
{
    if (pkg_data_dir == NULL) {
        /* Running uninstalled: look up the file in BUILDDIR first and
         * SRCDIR later, returning the first match */
        return adg_find_file(file_name, BUILDDIR, SRCDIR, NULL);
    }

    /* Running installed: look up the file only in `pkg_data_dir` */
    return adg_find_file(file_name, pkg_data_dir, NULL);
}
