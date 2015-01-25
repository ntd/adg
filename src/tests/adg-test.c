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


#if 0// GLIB_CHECK_VERSION(2, 38, 0)

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
adg_test_add_func(const char *testpath, GCallback test_func)
{
    g_test_set_nonfatal_assertions();
    g_test_add(testpath, _AdgFixture, NULL,
               _adg_setup, (gpointer) test_func, _adg_teardown);
}

#else

#include <stdlib.h>

/* Using adg_nop() would require to pull in the whole libadg library:
 * better to replicate that trivial function instead.
 */
static void
_adg_nop(void)
{
}


static void
_adg_test_func(GCallback test_func)
{
    GLogFunc old_logger;

    /* Run a test in a forked environment, without showing log messages */
    old_logger = g_log_set_default_handler((GLogFunc) _adg_nop, NULL);
    if (g_test_trap_fork(0, G_TEST_TRAP_SILENCE_STDERR)) {
        test_func();
        exit(0);
    }
    g_log_set_default_handler(old_logger, NULL);

    /* On failed test, rerun it without hiding the log messages */
    if (!g_test_trap_has_passed())
        test_func();
}


void
adg_test_add_func(const char *testpath, GCallback test_func)
{
    g_test_add_data_func(testpath, (gconstpointer) test_func,
                         (GTestDataFunc) _adg_test_func);
}

#endif
