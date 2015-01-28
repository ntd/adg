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
_adg_test_frame_dress(void)
{
    AdgLogo *logo;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress frame_dress;

    logo = adg_logo_new();
    valid_dress_1 = ADG_DRESS_LINE_STROKE;
    valid_dress_2 = ADG_DRESS_LINE_FILL;
    incompatible_dress = ADG_DRESS_COLOR;

    /* Using the public APIs */
    adg_logo_set_frame_dress(logo, valid_dress_1);
    frame_dress = adg_logo_get_frame_dress(logo);
    g_assert_cmpint(frame_dress, ==, valid_dress_1);

    adg_logo_set_frame_dress(logo, incompatible_dress);
    frame_dress = adg_logo_get_frame_dress(logo);
    g_assert_cmpint(frame_dress, ==, valid_dress_1);

    adg_logo_set_frame_dress(logo, valid_dress_2);
    frame_dress = adg_logo_get_frame_dress(logo);
    g_assert_cmpint(frame_dress, ==, valid_dress_2);

    /* Using GObject property methods */
    g_object_set(logo, "frame-dress", valid_dress_1, NULL);
    g_object_get(logo, "frame-dress", &frame_dress, NULL);
    g_assert_cmpint(frame_dress, ==, valid_dress_1);

    g_object_set(logo, "frame-dress", incompatible_dress, NULL);
    g_object_get(logo, "frame-dress", &frame_dress, NULL);
    g_assert_cmpint(frame_dress, ==, valid_dress_1);

    g_object_set(logo, "frame-dress", valid_dress_2, NULL);
    g_object_get(logo, "frame-dress", &frame_dress, NULL);
    g_assert_cmpint(frame_dress, ==, valid_dress_2);

    adg_entity_destroy(ADG_ENTITY(logo));
}

static void
_adg_test_screen_dress(void)
{
    AdgLogo *logo;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress screen_dress;

    logo = adg_logo_new();
    valid_dress_1 = ADG_DRESS_LINE_STROKE;
    valid_dress_2 = ADG_DRESS_LINE_FILL;
    incompatible_dress = ADG_DRESS_COLOR;

    /* Using the public APIs */
    adg_logo_set_screen_dress(logo, valid_dress_1);
    screen_dress = adg_logo_get_screen_dress(logo);
    g_assert_cmpint(screen_dress, ==, valid_dress_1);

    adg_logo_set_screen_dress(logo, incompatible_dress);
    screen_dress = adg_logo_get_screen_dress(logo);
    g_assert_cmpint(screen_dress, ==, valid_dress_1);

    adg_logo_set_screen_dress(logo, valid_dress_2);
    screen_dress = adg_logo_get_screen_dress(logo);
    g_assert_cmpint(screen_dress, ==, valid_dress_2);

    /* Using GObject property methods */
    g_object_set(logo, "screen-dress", valid_dress_1, NULL);
    g_object_get(logo, "screen-dress", &screen_dress, NULL);
    g_assert_cmpint(screen_dress, ==, valid_dress_1);

    g_object_set(logo, "screen-dress", incompatible_dress, NULL);
    g_object_get(logo, "screen-dress", &screen_dress, NULL);
    g_assert_cmpint(screen_dress, ==, valid_dress_1);

    g_object_set(logo, "screen-dress", valid_dress_2, NULL);
    g_object_get(logo, "screen-dress", &screen_dress, NULL);
    g_assert_cmpint(screen_dress, ==, valid_dress_2);

    adg_entity_destroy(ADG_ENTITY(logo));
}

static void
_adg_test_symbol_dress(void)
{
    AdgLogo *logo;
    AdgDress valid_dress_1, valid_dress_2, incompatible_dress;
    AdgDress symbol_dress;

    logo = adg_logo_new();
    valid_dress_1 = ADG_DRESS_LINE_STROKE;
    valid_dress_2 = ADG_DRESS_LINE_FILL;
    incompatible_dress = ADG_DRESS_COLOR;

    /* Using the public APIs */
    adg_logo_set_symbol_dress(logo, valid_dress_1);
    symbol_dress = adg_logo_get_symbol_dress(logo);
    g_assert_cmpint(symbol_dress, ==, valid_dress_1);

    adg_logo_set_symbol_dress(logo, incompatible_dress);
    symbol_dress = adg_logo_get_symbol_dress(logo);
    g_assert_cmpint(symbol_dress, ==, valid_dress_1);

    adg_logo_set_symbol_dress(logo, valid_dress_2);
    symbol_dress = adg_logo_get_symbol_dress(logo);
    g_assert_cmpint(symbol_dress, ==, valid_dress_2);

    /* Using GObject property methods */
    g_object_set(logo, "symbol-dress", valid_dress_1, NULL);
    g_object_get(logo, "symbol-dress", &symbol_dress, NULL);
    g_assert_cmpint(symbol_dress, ==, valid_dress_1);

    g_object_set(logo, "symbol-dress", incompatible_dress, NULL);
    g_object_get(logo, "symbol-dress", &symbol_dress, NULL);
    g_assert_cmpint(symbol_dress, ==, valid_dress_1);

    g_object_set(logo, "symbol-dress", valid_dress_2, NULL);
    g_object_get(logo, "symbol-dress", &symbol_dress, NULL);
    g_assert_cmpint(symbol_dress, ==, valid_dress_2);

    adg_entity_destroy(ADG_ENTITY(logo));
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_object_checks("/adg/logo/type/object", ADG_TYPE_LOGO);
    adg_test_add_entity_checks("/adg/logo/type/entity", ADG_TYPE_LOGO);

    adg_test_add_global_space_checks("/adg/logo/behavior/global-space", adg_logo_new());
    adg_test_add_local_space_checks("/adg/logo/behavior/local-space", adg_logo_new());

    adg_test_add_func("/adg/logo/property/frame-dress", _adg_test_frame_dress);
    adg_test_add_func("/adg/logo/property/screen-dress", _adg_test_screen_dress);
    adg_test_add_func("/adg/logo/property/symbol-dress", _adg_test_symbol_dress);

    return g_test_run();
}
