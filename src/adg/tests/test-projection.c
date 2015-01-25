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
_adg_test_axis_dress(void)
{
    AdgProjection *projection;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress axis_dress;

    projection = adg_projection_new(ADG_PROJECTION_SCHEME_FIRST_ANGLE);
    valid_dress_1 = ADG_DRESS_LINE_GRID;
    valid_dress_2 = ADG_DRESS_LINE_FRAME;
    incompatible_dress = ADG_DRESS_COLOR_STROKE;

    /* Using the public APIs */
    adg_projection_set_axis_dress(projection, valid_dress_1);
    axis_dress = adg_projection_get_axis_dress(projection);
    g_assert_cmpint(axis_dress, ==, valid_dress_1);

    adg_projection_set_axis_dress(projection, incompatible_dress);
    axis_dress = adg_projection_get_axis_dress(projection);
    g_assert_cmpint(axis_dress, ==, valid_dress_1);

    adg_projection_set_axis_dress(projection, valid_dress_2);
    axis_dress = adg_projection_get_axis_dress(projection);
    g_assert_cmpint(axis_dress, ==, valid_dress_2);

    /* Using GObject property methods */
    g_object_set(projection, "axis-dress", valid_dress_1, NULL);
    g_object_get(projection, "axis-dress", &axis_dress, NULL);
    g_assert_cmpint(axis_dress, ==, valid_dress_1);

    g_object_set(projection, "axis-dress", incompatible_dress, NULL);
    g_object_get(projection, "axis-dress", &axis_dress, NULL);
    g_assert_cmpint(axis_dress, ==, valid_dress_1);

    g_object_set(projection, "axis-dress", valid_dress_2, NULL);
    g_object_get(projection, "axis-dress", &axis_dress, NULL);
    g_assert_cmpint(axis_dress, ==, valid_dress_2);

    adg_entity_destroy(ADG_ENTITY(projection));
}

static void
_adg_test_scheme(void)
{
    AdgProjection *projection;
    AdgProjectionScheme valid_scheme_1, valid_scheme_2, invalid_scheme;
    AdgProjectionScheme scheme;

    projection = adg_projection_new(G_MAXINT);
    valid_scheme_1 = ADG_PROJECTION_SCHEME_THIRD_ANGLE;
    valid_scheme_2 = ADG_PROJECTION_SCHEME_UNDEFINED;
    invalid_scheme = G_MININT;

    /* Using the public APIs */
    adg_projection_set_scheme(projection, valid_scheme_1);
    scheme = adg_projection_get_scheme(projection);
    g_assert_cmpint(scheme, ==, valid_scheme_1);

    adg_projection_set_scheme(projection, invalid_scheme);
    scheme = adg_projection_get_scheme(projection);
    g_assert_cmpint(scheme, ==, valid_scheme_1);

    adg_projection_set_scheme(projection, valid_scheme_2);
    scheme = adg_projection_get_scheme(projection);
    g_assert_cmpint(scheme, ==, valid_scheme_2);

    /* Using GObject property methods */
    g_object_set(projection, "scheme", valid_scheme_1, NULL);
    g_object_get(projection, "scheme", &scheme, NULL);
    g_assert_cmpint(scheme, ==, valid_scheme_1);

    g_object_set(projection, "scheme", invalid_scheme, NULL);
    g_object_get(projection, "scheme", &scheme, NULL);
    g_assert_cmpint(scheme, ==, valid_scheme_1);

    g_object_set(projection, "scheme", valid_scheme_2, NULL);
    g_object_get(projection, "scheme", &scheme, NULL);
    g_assert_cmpint(scheme, ==, valid_scheme_2);

    adg_entity_destroy(ADG_ENTITY(projection));
}

static void
_adg_test_symbol_dress(void)
{
    AdgProjection *projection;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress symbol_dress;

    projection = adg_projection_new(ADG_PROJECTION_SCHEME_UNDEFINED);
    valid_dress_1 = ADG_DRESS_LINE;
    valid_dress_2 = ADG_DRESS_LINE_STROKE;
    incompatible_dress = ADG_DRESS_COLOR_FILL;

    /* Using the public APIs */
    adg_projection_set_symbol_dress(projection, valid_dress_1);
    symbol_dress = adg_projection_get_symbol_dress(projection);
    g_assert_cmpint(symbol_dress, ==, valid_dress_1);

    adg_projection_set_symbol_dress(projection, incompatible_dress);
    symbol_dress = adg_projection_get_symbol_dress(projection);
    g_assert_cmpint(symbol_dress, ==, valid_dress_1);

    adg_projection_set_symbol_dress(projection, valid_dress_2);
    symbol_dress = adg_projection_get_symbol_dress(projection);
    g_assert_cmpint(symbol_dress, ==, valid_dress_2);

    /* Using GObject property methods */
    g_object_set(projection, "symbol-dress", valid_dress_1, NULL);
    g_object_get(projection, "symbol-dress", &symbol_dress, NULL);
    g_assert_cmpint(symbol_dress, ==, valid_dress_1);

    g_object_set(projection, "symbol-dress", incompatible_dress, NULL);
    g_object_get(projection, "symbol-dress", &symbol_dress, NULL);
    g_assert_cmpint(symbol_dress, ==, valid_dress_1);

    g_object_set(projection, "symbol-dress", valid_dress_2, NULL);
    g_object_get(projection, "symbol-dress", &symbol_dress, NULL);
    g_assert_cmpint(symbol_dress, ==, valid_dress_2);

    adg_entity_destroy(ADG_ENTITY(projection));
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/projection/property/axis-dress", _adg_test_axis_dress);
    adg_test_add_func("/adg/projection/property/scheme", _adg_test_scheme);
    adg_test_add_func("/adg/projection/property/symbol-dress", _adg_test_symbol_dress);
    adg_test_add_property_check("/adg/projection/property/???", ADG_TYPE_PROJECTION);

    return g_test_run();
}
