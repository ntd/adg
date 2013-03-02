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


#include "test-internal.h"


static void
_adg_test_scale_factor(void)
{
    g_assert_cmpfloat(adg_scale_factor(""), ==, 0);
    g_assert_cmpfloat(adg_scale_factor(NULL), ==, 0);
    g_assert_cmpfloat(adg_scale_factor("3"), ==, 3);
    g_assert_cmpfloat(adg_scale_factor("3:3"), ==, 1);
    g_assert_cmpfloat(adg_scale_factor("+010 garbage"), ==, 10);

    /* No idea if sign+space is invalid on every atoi implementation */
    g_assert_cmpfloat(adg_scale_factor("+ 3"), ==, 0);

    g_assert_cmpfloat(adg_scale_factor("-1:1"), ==, 0);
    g_assert_cmpfloat(adg_scale_factor("1:-1"), ==, 0);
    g_assert_cmpfloat(adg_scale_factor(" +5 : 05 garbage"), ==, 1);
    g_assert_cmpfloat(adg_scale_factor("1:0"), ==, 0);
    g_assert_cmpfloat(adg_scale_factor("1:"), ==, 0);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/utils/scale-factor", _adg_test_scale_factor);

    return g_test_run();
}
