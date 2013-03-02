/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011,2012,2013  Nicola Fontana <ntd at entidi.it>
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
#include "test-internal.h"


void
adg_test_init(int *p_argc, char **p_argv[])
{
    g_type_init();
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

static void
_adg_log_handler(const gchar *log_domain, GLogLevelFlags log_level,
                 const gchar *message, gpointer user_data)
{
}

static void
_adg_test_func(gconstpointer user_data)
{
    GCallback test_func;
    GLogFunc previous_handler;

    test_func = (GCallback) user_data;

    /* Run a test in a forked environment, without showing log messages */
    previous_handler = g_log_set_default_handler(_adg_log_handler, NULL);
    if (g_test_trap_fork(0, G_TEST_TRAP_SILENCE_STDERR)) {
        test_func();
        exit(0);
    }
    g_log_set_default_handler(previous_handler, NULL);

    /* On failed test, rerun it without hiding the log messages */
    if (!g_test_trap_has_passed())
        test_func();
}

void
adg_test_add_func(const char *testpath, GCallback test_func)
{
    g_test_add_data_func(testpath, (gconstpointer) test_func, _adg_test_func);
}
