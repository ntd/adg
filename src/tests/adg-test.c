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
} _FuncData;

typedef struct {
    GType type;
    gpointer instance;
} _BoxedData;


/* Using adg_nop() would require to pull in the whole libadg stack:
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
_adg_test_func(_FuncData *func_data)
{
    func_data->func(func_data->data);
    g_free(func_data);
}

void
adg_test_add_func_full(const char *testpath,
                       GCallback test_func, gpointer user_data)
{
    _FuncData *func_data = g_new(_FuncData, 1);
    func_data->func = (gpointer) test_func;
    func_data->data = user_data;

    g_test_add_data_func(testpath, func_data, (GTestDataFunc) _adg_test_func);
}

void
adg_test_add_func(const char *testpath, GCallback test_func)
{
    adg_test_add_func_full(testpath, test_func, NULL);
}

static void
_adg_boxed_checks(_BoxedData *boxed_data)
{
    gpointer replica;

    g_assert_true(G_TYPE_IS_BOXED(boxed_data->type));

    g_assert_null(g_boxed_copy(boxed_data->type, NULL));

    replica = g_boxed_copy(boxed_data->type, boxed_data->instance);
    g_assert_nonnull(replica);

    g_boxed_free(boxed_data->type, replica);
    g_boxed_free(boxed_data->type, boxed_data->instance);

    g_free(boxed_data);
}

void
adg_test_add_boxed_checks(const gchar *testpath, GType type, gpointer instance)
{
    _BoxedData *boxed_data = g_new(_BoxedData, 1);
    boxed_data->type = type;
    boxed_data->instance = instance;

    adg_test_add_func_full(testpath, G_CALLBACK(_adg_boxed_checks), boxed_data);
}

static void
_adg_object_checks(gconstpointer user_data)
{
    GType type = GPOINTER_TO_INT(user_data);
    g_assert_true(G_TYPE_IS_OBJECT(type));

    if (! G_TYPE_IS_ABSTRACT(type)) {
        gpointer result = GINT_TO_POINTER(0xdead);
        GObject *object = g_object_new(type, NULL);

        g_assert_nonnull(object);
        g_assert_true(G_IS_OBJECT(object));

        /* Check that unknown or unexistent properties does
         * not return values (result should pass unmodified)
         */
        g_assert_cmpint(GPOINTER_TO_INT(result), ==, 0xdead);
        g_object_set(object, "unknown", NULL, NULL);
        g_object_get(object, "unknown", &result, NULL);
        g_assert_cmpint(GPOINTER_TO_INT(result), ==, 0xdead);
        g_object_get(object, "unexistent", &result, NULL);
        g_assert_cmpint(GPOINTER_TO_INT(result), ==, 0xdead);

        /* Check object lifetime: this can be done with a weak pointer
         * that will "nullifies" result after object is destructed
         */
        g_object_add_weak_pointer(object, &result);
        g_object_ref(object);
        g_object_unref(object);
        g_assert_nonnull(result);
        g_object_unref(object);
        g_assert_null(result);

        result = GINT_TO_POINTER(0xdead);
        object = g_object_new(type, NULL);

        g_object_add_weak_pointer(object, &result);
        g_assert_nonnull(result);
        g_object_ref(object);
        g_object_ref(object);
        g_object_run_dispose(object);
        g_assert_null(result);
    }
}

void
adg_test_add_object_checks(const gchar *testpath, GType type)
{
    adg_test_add_func_full(testpath,
                           G_CALLBACK(_adg_object_checks),
                           GINT_TO_POINTER(type));
}
