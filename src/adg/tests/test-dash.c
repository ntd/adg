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


#include <adg-test.h>
#include <adg.h>


static void
_adg_test_dashes(void)
{
    AdgDash *dash;
    gint num_dashes;
    const gdouble *dashes;
    const gdouble dashes_array[] = { 1., 2., 3. };

    dash = adg_dash_new();
    g_assert_nonnull(dash);
    num_dashes = adg_dash_get_num_dashes(dash);
    g_assert_cmpint(num_dashes, ==, 0);
    dashes = adg_dash_get_dashes(dash);
    g_assert_null(dashes);

    adg_dash_append_dash(dash, 1234.);
    num_dashes = adg_dash_get_num_dashes(dash);
    g_assert_cmpint(num_dashes, ==, 1);
    dashes = adg_dash_get_dashes(dash);
    g_assert_nonnull(dashes);
    g_assert_cmpfloat(dashes[0], ==, 1234.);

    adg_dash_append_dashes(dash, 2, 0., 4321.);
    num_dashes = adg_dash_get_num_dashes(dash);
    g_assert_cmpint(num_dashes, ==, 3);
    dashes = adg_dash_get_dashes(dash);
    g_assert_nonnull(dashes);
    g_assert_cmpfloat(dashes[0], ==, 1234.);
    g_assert_cmpfloat(dashes[1], ==, 0.);
    g_assert_cmpfloat(dashes[2], ==, 4321.);

    adg_dash_clear_dashes(dash);
    num_dashes = adg_dash_get_num_dashes(dash);
    g_assert_cmpint(num_dashes, ==, 0);
    dashes = adg_dash_get_dashes(dash);
    g_assert_null(dashes);

    adg_dash_append_dashes_array(dash, 3, dashes_array);
    num_dashes = adg_dash_get_num_dashes(dash);
    g_assert_cmpint(num_dashes, ==, 3);
    dashes = adg_dash_get_dashes(dash);
    g_assert_nonnull(dashes);
    g_assert_cmpfloat(dashes[0], ==, 1.);
    g_assert_cmpfloat(dashes[1], ==, 2.);
    g_assert_cmpfloat(dashes[2], ==, 3.);

    adg_dash_destroy(dash);

    dash = adg_dash_new_with_dashes(3, 1., 2., 3.);
    g_assert_nonnull(dash);
    num_dashes = adg_dash_get_num_dashes(dash);
    g_assert_cmpint(num_dashes, ==, 3);
    dashes = adg_dash_get_dashes(dash);
    g_assert_nonnull(dashes);
    g_assert_cmpfloat(dashes[0], ==, 1.);
    g_assert_cmpfloat(dashes[1], ==, 2.);
    g_assert_cmpfloat(dashes[2], ==, 3.);

    adg_dash_destroy(dash);
}

static void
_adg_test_offset(void)
{
    AdgDash *dash;
    gdouble offset;

    dash = adg_dash_new();

    /* Ensure the default is 0 */
    offset = adg_dash_get_offset(dash);
    g_assert_cmpfloat(offset, ==, 0);

    /* Check some special values */
    adg_dash_set_offset(dash, G_MINDOUBLE);
    offset = adg_dash_get_offset(dash);
    g_assert_cmpfloat(offset, ==, G_MINDOUBLE);

    adg_dash_set_offset(dash, G_MAXDOUBLE);
    offset = adg_dash_get_offset(dash);
    g_assert_cmpfloat(offset, ==, G_MAXDOUBLE);

    adg_dash_set_offset(dash, 0);
    offset = adg_dash_get_offset(dash);
    g_assert_cmpfloat(offset, ==, 0);

    adg_dash_destroy(dash);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/dash/property/dashes", _adg_test_dashes);
    adg_test_add_func("/adg/dash/property/offset", _adg_test_offset);

    return g_test_run();
}
