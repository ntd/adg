#include "demo.h"

/**
 * demo_find_data_file:
 * @file: the name of the file to look for
 * @caller: caller program (argv[0])
 *
 * Looks for @file in the data path. This wrapper is needed
 * to allow running the program both when uninstalled and
 * installed: in the former case, @file must be present in
 * the parent directory of the caller file while in the
 * latter case it must reside in $(pkgdatadir).
 *
 * The function looks for paths in the following order:
 *
 * - $(top_scrdir)/demo
 * - $(top_builddir)/demo
 * - $(pkgdatadir)
 *
 * Returns: the full path to @file to be freed with g_free()
 *          or %NULL if not found or on errors
 **/
gchar *
demo_find_data_file(const gchar *file, const gchar *caller)
{
    static gchar *data_root = NULL;

    if (data_root == NULL) {
        if (caller == NULL || strstr(caller, LT_OBJDIR) == NULL) {
            /* Supposedly, this program is installed */
            data_root = PKGDATADIR;
        } else {
            /* Program not installed: set data_root to
             * the parent directory of the caller dirname */
            gchar *caller_dirname = g_path_get_dirname(caller);
            data_root = g_build_filename(caller_dirname, "..", NULL);
            g_free(caller_dirname);
        }
    }

    g_print("File: %s\n", g_build_filename(data_root, file, NULL));
    return g_build_filename(data_root, file, NULL);
}
