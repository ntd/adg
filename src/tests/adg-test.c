/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2015  Nicola Fontana <ntd at entidi.it>
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


#include <config.h>
#include "adg-test.h"


typedef struct {
    void (*func)(gpointer);
    gpointer data;
} _TestData;


/* Using adg_nop() would require to pull in the whole libadg library:
 * better to replicate that trivial function instead.
 */
static void
_adg_nop(void)
{
}


void
adg_test_init(int *p_argc, char **p_argv[])
{
#if GLIB_CHECK_VERSION(2, 34, 0)
#else
    /* On GLib older than 2.34.0 g_type_init() *must* be called */
    g_type_init();
#endif
    g_test_init(p_argc, p_argv, NULL);

    g_log_set_always_fatal(0);

    /* When building in silent mode (default), the ADG_QUIET
     * environment variable is set to 1 by the Makefile and the
     * warnings are discarded to reduce visual cluttering.
     */
    if (g_getenv("ADG_QUIET") != NULL)
        g_log_set_default_handler((GLogFunc) _adg_nop, NULL);

    g_test_bug_base("http://dev.entidi.com/p/adg/issues/%s/");
}

const gpointer
adg_test_invalid_pointer(void)
{
    static int junk[10] = { 0 };
    return junk;
}

static void
_adg_test_func(_TestData *test_data)
{
    test_data->func(test_data->data);
    g_free(test_data);
}

void
adg_test_add_func_full(const char *testpath,
                       GCallback test_func, gpointer user_data)
{
    _TestData *test_data = g_new(_TestData, 1);
    test_data->func = (gpointer) test_func;
    test_data->data = user_data;

    g_test_add_data_func(testpath, test_data, (GTestDataFunc) _adg_test_func);
}

void
adg_test_add_func(const char *testpath, GCallback test_func)
{
    adg_test_add_func_full(testpath, test_func, NULL);
}

static void
_adg_property_check(GCallback test_func, gconstpointer user_data)
{
    gpointer result;
    GType type;
    GObject *object;

    type = GPOINTER_TO_INT(user_data);
    object = g_object_new(type, NULL);
    result = GINT_TO_POINTER(0xdead);
    g_assert_cmpint(GPOINTER_TO_INT(result), ==, 0xdead);

    g_object_set(object, "unknown", NULL, NULL);
    g_object_get(object, "unknown", &result, NULL);
    g_assert_cmpint(GPOINTER_TO_INT(result), ==, 0xdead);
    g_object_get(object, "unexistent", &result, NULL);
    g_assert_cmpint(GPOINTER_TO_INT(result), ==, 0xdead);

    g_object_unref(object);
}

void
adg_test_add_property_check(const gchar *testpath, GType type)
{
    adg_test_add_func_full(testpath, G_CALLBACK(_adg_property_check),
                           GINT_TO_POINTER(type));
}
