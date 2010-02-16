/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009 Nicola Fontana <ntd at entidi.it>
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


#include "test-internal.h"


static void
null_handler(const gchar *log_domain, GLogLevelFlags log_level,
             const gchar *message, gpointer user_data)
{
}


void
test_init(int *p_argc, char **p_argv[])
{
    g_type_init();
    g_test_init(p_argc, p_argv, NULL);

    /* Shut up all the library messages apart G_LOG_LEVEL_ERROR
     * (always fatal by design): on my opinion a failing test
     * is a test that ends unexpectedly, not the one properly
     * reacting to invalid values.
     *
     * For some unknown reason, g_log_set_fatal_mask("adg", 0)
     * does not work: the domain seems to be ignored. A better
     * way would be to use g_test_log_set_fatal_handler() and
     * provide a callback that ignores the "adg" domain (I've
     * tested this approach and it works) but that function
     * will pull in a glib-2.22 dependency... */
    g_log_set_always_fatal(0);
    g_log_set_default_handler(null_handler, NULL);
}
