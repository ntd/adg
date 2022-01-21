/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2022  Nicola Fontana <ntd at entidi.it>
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
_adg_method_strcmp0(void)
{
    g_assert_cmpint(g_strcmp0(NULL, NULL), ==, 0);
    g_assert_cmpint(g_strcmp0("", ""), ==, 0);
    g_assert_cmpint(g_strcmp0(NULL, "test"), <, 0);
    g_assert_cmpint(g_strcmp0("test", NULL), >, 0);
    g_assert_cmpint(g_strcmp0("test", "test"), ==, 0);
}

static void
_adg_method_is_string_empty(void)
{
    g_assert_true(adg_is_string_empty(""));
    g_assert_true(adg_is_string_empty(NULL));
    g_assert_false(adg_is_string_empty("test"));
}

static void
_adg_method_is_enum_value(void)
{
    g_assert_true(adg_is_enum_value(ADG_THREE_STATE_ON, ADG_TYPE_THREE_STATE));
    g_assert_false(adg_is_enum_value(-1, ADG_TYPE_THREE_STATE));
    g_assert_false(adg_is_enum_value(485, ADG_TYPE_THREE_STATE));
}

static void
_adg_method_is_boolean_value(void)
{
    g_assert_true(adg_is_boolean_value(TRUE));
    g_assert_true(adg_is_boolean_value(FALSE));
    g_assert_false(adg_is_boolean_value(177));
}

static void
_adg_method_string_replace(void)
{
    gchar *result;

    g_assert_null(adg_string_replace(NULL, "first", "second"));
    g_assert_null(adg_string_replace("original", NULL, "second"));
    g_assert_null(adg_string_replace("original", NULL, "second"));

    result = adg_string_replace("The first arg", "first", "second");
    g_assert_cmpstr(result, ==, "The second arg");
    g_free(result);

    result = adg_string_replace("The first arg", "first", NULL);
    g_assert_cmpstr(result, ==, "The  arg");
    g_free(result);
}

static void
_adg_method_find_file(void)
{
    gchar *result;

    g_assert_null(adg_find_file(NULL));
    g_assert_null(adg_find_file("test-utils.c", NULL));

    result = adg_find_file("test-utils.c", SRCDIR, NULL);
    g_assert_nonnull(result);
    g_free(result);

    result = adg_find_file("test-utils.c", "unexistentdirectory", SRCDIR, NULL);
    g_assert_nonnull(result);
    g_free(result);
}

static void
_adg_method_scale_factor(void)
{
    adg_assert_isapprox(adg_scale_factor(""), 0);
    adg_assert_isapprox(adg_scale_factor(NULL), 0);
    adg_assert_isapprox(adg_scale_factor("3"), 3);
    adg_assert_isapprox(adg_scale_factor("3:3"), 1);
    adg_assert_isapprox(adg_scale_factor("+010 garbage"), 10);

    /* No idea if sign+space is invalid on every atof implementation */
    adg_assert_isapprox(adg_scale_factor("+ 3"), 0);

    adg_assert_isapprox(adg_scale_factor("-1:1"), -1);
    adg_assert_isapprox(adg_scale_factor("1:-1"), -1);
    adg_assert_isapprox(adg_scale_factor(" +5 : 05 garbage"), 1);
    adg_assert_isapprox(adg_scale_factor("1:0"), 0);
    adg_assert_isapprox(adg_scale_factor("1:"), 0);

    adg_assert_isapprox(adg_scale_factor("3:2"), 1.5);
    adg_assert_isapprox(adg_scale_factor("3.2:3.20"), 1);
    adg_assert_isapprox(adg_scale_factor("3.25:1"), 3.25);
    adg_assert_isapprox(adg_scale_factor("1:0.33333333333"), 3);
}

static void
_adg_method_type_from_filename(void)
{
    g_assert_cmpint(adg_type_from_filename("noextension"), ==, CAIRO_SURFACE_TYPE_XLIB);
    g_assert_cmpint(adg_type_from_filename("a.png"), ==, CAIRO_SURFACE_TYPE_IMAGE);
    g_assert_cmpint(adg_type_from_filename("a.PNG"), ==, CAIRO_SURFACE_TYPE_IMAGE);
    g_assert_cmpint(adg_type_from_filename("a.svg"), ==, CAIRO_SURFACE_TYPE_SVG);
    g_assert_cmpint(adg_type_from_filename("a.pdf"), ==, CAIRO_SURFACE_TYPE_PDF);
    g_assert_cmpint(adg_type_from_filename("a.ps"), ==, CAIRO_SURFACE_TYPE_PS);
    g_assert_cmpint(adg_type_from_filename("a.unknown"), ==, CAIRO_SURFACE_TYPE_XLIB);
}

static void
_adg_method_clone(void)
{
    AdgCanvas *canvas, *clone;
    AdgTitleBlock *title_block;

    /* Sanity check */
    g_assert_null(adg_object_clone(NULL));

    canvas = adg_canvas_new();
    title_block = adg_title_block_new();

    adg_canvas_set_background_dress(canvas, ADG_DRESS_COLOR_ANNOTATION);
    adg_canvas_set_title_block(canvas, title_block);
    g_object_unref(title_block);
    adg_canvas_set_margins(canvas, 1, 2, 3, 4);
    adg_canvas_switch_frame(canvas, FALSE);

    /* Check that the original canvas matches the expected states */
    g_assert_cmpint(adg_canvas_get_background_dress(canvas), ==, ADG_DRESS_COLOR_ANNOTATION);
    g_assert_true(adg_canvas_get_title_block(canvas) == title_block);
    adg_assert_isapprox(adg_canvas_get_top_margin(canvas), 1);
    adg_assert_isapprox(adg_canvas_get_right_margin(canvas), 2);
    adg_assert_isapprox(adg_canvas_get_bottom_margin(canvas), 3);
    adg_assert_isapprox(adg_canvas_get_left_margin(canvas), 4);
    g_assert_false(adg_canvas_has_frame(canvas));

    clone = ADG_CANVAS(adg_object_clone(G_OBJECT(canvas)));
    g_object_unref(canvas);

    /* Check the states on the clone are the same */
    g_assert_cmpint(adg_canvas_get_background_dress(clone), ==, ADG_DRESS_COLOR_ANNOTATION);
    g_assert_true(adg_canvas_get_title_block(clone) == title_block);
    adg_assert_isapprox(adg_canvas_get_top_margin(clone), 1);
    adg_assert_isapprox(adg_canvas_get_right_margin(clone), 2);
    adg_assert_isapprox(adg_canvas_get_bottom_margin(clone), 3);
    adg_assert_isapprox(adg_canvas_get_left_margin(clone), 4);
    g_assert_false(adg_canvas_has_frame(clone));

    g_object_unref(clone);
}

static void
_adg_method_nop(void)
{
    /* Just check for this function existence */
    adg_nop();
}

static void
_adg_method_round(void)
{
    adg_assert_isapprox(adg_round(0, 12), 0);
    adg_assert_isapprox(adg_round(3.2222222, 1), 3.2);
    adg_assert_isapprox(adg_round(3.2222222, 2), 3.22);
    adg_assert_isapprox(adg_round(-1234.56, 0), -1235);
}

static void
_adg_method_unescaped_strchr(void)
{
    const gchar *result;

    /* Sanity check */
    g_assert_null(adg_unescaped_strchr(NULL, '\0'));

    result = adg_unescaped_strchr("\\a\\b\\abab", 'a');
    g_assert_cmpstr(result, ==, "ab");

    result = adg_unescaped_strchr("\\a\\b\\abab", 'b');
    g_assert_cmpstr(result, ==, "bab");

    result = adg_unescaped_strchr("1\\22", '1');
    g_assert_cmpstr(result, ==, "1\\22");

    result = adg_unescaped_strchr("1\\22", '2');
    g_assert_cmpstr(result, ==, "2");

    result = adg_unescaped_strchr("122", '3');
    g_assert_null(result);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    g_test_add_func("/adg/method/strcpm0", _adg_method_strcmp0);
    g_test_add_func("/adg/method/is-string-empty", _adg_method_is_string_empty);
    g_test_add_func("/adg/method/is-enum-value", _adg_method_is_enum_value);
    g_test_add_func("/adg/method/is-boolean-value", _adg_method_is_boolean_value);
    g_test_add_func("/adg/method/string-replace", _adg_method_string_replace);
    g_test_add_func("/adg/method/find-file", _adg_method_find_file);
    g_test_add_func("/adg/method/scale-factor", _adg_method_scale_factor);
    g_test_add_func("/adg/method/type-from-filename", _adg_method_type_from_filename);
    g_test_add_func("/adg/method/clone", _adg_method_clone);
    g_test_add_func("/adg/method/nop", _adg_method_nop);
    g_test_add_func("/adg/method/round", _adg_method_round);
    g_test_add_func("/adg/method/unescaped-strchr", _adg_method_unescaped_strchr);

    return g_test_run();
}
