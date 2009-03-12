#include <config.h>

#include "demo.h"

gchar *
demo_find_data_file(const gchar *file)
{
    gchar *path;

    path = g_build_filename(SOURCEDIR, file, NULL);
    if (g_file_test(path, G_FILE_TEST_EXISTS))
        return path;

    g_free(path);
    path = g_build_filename(PKGDATADIR, PACKAGE_NAME, file, NULL);
    if (g_file_test(path, G_FILE_TEST_EXISTS))
        return path;

    g_free(path);
    return NULL;
}
