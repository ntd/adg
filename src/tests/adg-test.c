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
    g_test_bug_base("http://dev.entidi.com/p/adg/issues/%s/");
}

const gpointer
adg_test_invalid_pointer(void)
{
    static int junk[10] = { 0 };
    return junk;
}


#if GLIB_CHECK_VERSION(2, 38, 0)

/* Instead of using the deprecated fork approach (it does not always work)
 * use the newly introduced g_test_set_nonfatal_assertions() API to avoid
 * aborting the program and check the result at the end of the test.
 */

typedef struct {
    GSList *errors;
    GLogFunc old_logger;
} _AdgFixture;


typedef struct {
    gchar *log_domain;
    GLogLevelFlags log_level;
    gchar *message;
} _AdgCapture;

static void
_adg_capture_handler(const gchar *log_domain, GLogLevelFlags log_level,
                     const gchar *message, _AdgFixture *fixture)
{
    _AdgCapture *capture = g_new(_AdgCapture, 1);
    capture->log_domain = g_strdup(log_domain);
    capture->log_level = log_level;
    capture->message = g_strdup(message);
    fixture->errors = g_slist_append(fixture->errors, capture);
}

static void
_adg_capture_free(_AdgCapture *capture)
{
    g_free(capture->log_domain);
    g_free(capture->message);
    g_free(capture);
}

static void
_adg_setup(_AdgFixture *fixture, gconstpointer user_data)
{
    fixture->errors = NULL;
    fixture->old_logger = g_log_set_default_handler((GLogFunc) _adg_capture_handler, fixture);
}

static void
_adg_teardown(_AdgFixture *fixture, gconstpointer user_data)
{
    g_log_set_default_handler(fixture->old_logger, NULL);

    if (g_test_failed()) {
        GSList *node;
        _AdgCapture *capture;

        for (node = fixture->errors; node != NULL; node = node->next) {
            capture = node->data;
            g_log(capture->log_domain, capture->log_level,
                  "%s", capture->message);
        }
    }

    g_slist_free_full(fixture->errors, (GDestroyNotify) _adg_capture_free);
}

void
adg_test_add_func_full(const char *testpath,
                       GCallback test_func, gpointer user_data)
{
    g_test_set_nonfatal_assertions();
    g_test_add(testpath, _AdgFixture, user_data,
               _adg_setup, (gpointer) test_func, _adg_teardown);
}

#else

#include <stdlib.h>

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

static void
_adg_test_func(_TestData *test_data)
{
    GLogFunc old_logger;

    /* Run a test in a forked environment, without showing log messages */
    old_logger = g_log_set_default_handler((GLogFunc) _adg_nop, NULL);
    if (g_test_trap_fork(0, G_TEST_TRAP_SILENCE_STDERR)) {
        test_data->func(test_data->data);
        exit(0);
    }
    g_log_set_default_handler(old_logger, NULL);

    /* On failed test, rerun it without hiding the log messages */
    if (!g_test_trap_has_passed())
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

#endif

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

    g_object_set(object, "unknown", NULL, NULL);
    result = GINT_TO_POINTER(1);
    g_assert_cmpint(GPOINTER_TO_INT(result), ==, 1);
    g_object_get(object, "unknown", &result, NULL);
    g_assert_cmpint(GPOINTER_TO_INT(result), ==, 1);
    g_object_get(object, "unexistent", &result, NULL);
    g_assert_cmpint(GPOINTER_TO_INT(result), ==, 1);

    g_object_unref(object);
}

void
adg_test_add_property_check(const gchar *testpath, GType type)
{
    adg_test_add_func_full(testpath, G_CALLBACK(_adg_property_check),
                           GINT_TO_POINTER(type));
}
