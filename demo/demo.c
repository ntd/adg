#include <config.h>

#include "demo.h"

static gboolean
adg_file_test(const gchar *filename, GFileTest test)
{
#ifdef _DEBUG
    g_debug("Looking for '%s'\n", filename);
#endif
    return g_file_test(filename, test);
}

/**
 * demo_find_data_file:
 * @file: the name of the file to look for
 * @caller: caller program (argv[0])
 *
 * Looks for @file in the data paths. This wrapper is needed
 * to allow running the program both when uninstalled and
 * installed: in the former case the path must be
 * $(top_builddir)/demo while in the latter it should be
 * $(pkgdatadir).
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
    gchar *path;

    path = g_build_filename(DEMOBUILDDIR, file, NULL);
    if (adg_file_test(path, G_FILE_TEST_EXISTS))
        return path;

    g_free(path);
    path = g_build_filename(DEMOSRCDIR, file, NULL);
    if (adg_file_test(path, G_FILE_TEST_EXISTS))
        return path;

    g_free(path);
    path = g_build_filename(PKGDATADIR, PACKAGE_NAME, file, NULL);
    if (adg_file_test(path, G_FILE_TEST_EXISTS))
        return path;

    g_free(path);
    return NULL;
}
