/* ADG - Automatic Drawing Generation
 * Copyright (C) 2011  Nicola Fontana <ntd at entidi.it>
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


typedef void  (*CpmlCallback)              (void);


void
cpml_test_init(int *p_argc, char **p_argv[])
{
    g_test_init(p_argc, p_argv, NULL);
    g_log_set_always_fatal(0);
    g_test_bug_base("http://dev.entidi.com/p/adg/issues/%s/");
}

static void
_cpml_log_handler(const gchar *log_domain, GLogLevelFlags log_level,
                  const gchar *message, gpointer user_data)
{
}

static void
_cpml_test_func(gconstpointer user_data)
{
    CpmlCallback test_func;
    GLogFunc previous_handler;

    test_func = (CpmlCallback) user_data;

    /* Run a test in a forked environment, without showing log messages */
    previous_handler = g_log_set_default_handler(_cpml_log_handler, NULL);
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
cpml_test_add_func(const char *testpath, void (*test_func)(void))
{
    g_test_add_data_func(testpath, (gconstpointer) test_func, _cpml_test_func);
}
