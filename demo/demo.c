/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2019  Nicola Fontana <ntd at entidi.it>
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


/* Whether the program is running installed or uninstalled */
gboolean is_installed = TRUE;

/* The base directory where all files must be referred,
 * usually left unset _adg_init() on POSIX systems. */
gchar *basedir = NULL;


void
_demo_init(gint argc, gchar *argv[])
{
    gchar  *name;

    if (argc < 1 || argv == NULL || argv[0] == NULL) {
        g_error(_("Invalid arguments: arg[0] not set"));
        g_assert_not_reached();
    }

    name = g_path_get_basename(argv[0]);
    if (name != NULL) {
        is_installed = strstr(name, "uninstalled") == NULL;
        g_free(name);
    }

    basedir = g_path_get_dirname(argv[0]);
    if (basedir == NULL)
        basedir = g_strdup("");
}

gchar *
_demo_file(const gchar *file_name)
{
    static gchar *pkg_data_dir = NULL;

    if (! is_installed) {
        /* Running uninstalled: look up the file in BUILDDIR before and
         * in SRCDIR after, returning the first match */
        return adg_find_file(file_name, BUILDDIR, SRCDIR, NULL);
    }

    /* Otherwise, look up the file only in PKGDATADIR */
    if (pkg_data_dir == NULL) {
#ifdef G_OS_WIN32
        /* On windows, PKGDATADIR is relative to basedir */
        pkg_data_dir = g_build_filename(basedir, PKGDATADIR, NULL);
#else
        pkg_data_dir = g_strdup(PKGDATADIR);
#endif
    }

    return adg_find_file(file_name, pkg_data_dir, NULL);
}
