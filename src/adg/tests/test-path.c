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
_adg_method_get_current_point(void)
{
    AdgPath *path;
    const CpmlPair *cp;

    /* Check sanity */
    g_assert_null(adg_path_get_current_point(NULL));

    path = adg_path_new();
    g_assert_null(adg_path_get_current_point(path));

    adg_path_move_to_explicit(path, 1, 2);
    cp = adg_path_get_current_point(path);
    g_assert_nonnull(cp);
    g_assert_cmpfloat(cp->x, ==, 1);
    g_assert_cmpfloat(cp->y, ==, 2);

    adg_path_line_to_explicit(path, 3, 4);
    cp = adg_path_get_current_point(path);
    g_assert_nonnull(cp);
    g_assert_cmpfloat(cp->x, ==, 3);
    g_assert_cmpfloat(cp->y, ==, 4);

    adg_path_arc_to_explicit(path, 5, 6, 7, 8);
    cp = adg_path_get_current_point(path);
    g_assert_nonnull(cp);
    g_assert_cmpfloat(cp->x, ==, 7);
    g_assert_cmpfloat(cp->y, ==, 8);

    adg_path_curve_to_explicit(path, 9, 10, 11, 12, 13, 14);
    cp = adg_path_get_current_point(path);
    g_assert_nonnull(cp);
    g_assert_cmpfloat(cp->x, ==, 13);
    g_assert_cmpfloat(cp->y, ==, 14);

    adg_path_close(path);
    g_assert_null(adg_path_get_current_point(path));

    g_object_unref(path);
}

static void
_adg_method_has_current_point(void)
{
    AdgPath *path;

    /* Check sanity */
    g_assert_false(adg_path_has_current_point(NULL));

    path = adg_path_new();
    g_assert_false(adg_path_has_current_point(path));

    adg_path_move_to_explicit(path, 0, 0);
    g_assert_true(adg_path_has_current_point(path));
    adg_path_close(path);
    g_assert_false(adg_path_has_current_point(path));

    g_object_unref(path);
}

static void
_adg_method_last_primitive(void)
{
    AdgPath *path;
    const CpmlPrimitive *primitive;

    /* Check sanity */
    g_assert_null(adg_path_last_primitive(NULL));

    path = adg_path_new();
    g_assert_null(adg_path_last_primitive(path));

    adg_path_move_to_explicit(path, 1, 2);
    g_assert_null(adg_path_last_primitive(path));

    adg_path_line_to_explicit(path, 3, 4);
    primitive = adg_path_last_primitive(path);
    g_assert_nonnull(primitive);
    g_assert_cmpint(primitive->data->header.type, ==, CPML_LINE);
    g_assert_cmpint(primitive->data->header.length, ==, 2);
    g_assert_cmpfloat(primitive->org->point.x, ==, 1);
    g_assert_cmpfloat(primitive->org->point.y, ==, 2);
    g_assert_cmpfloat(primitive->data[1].point.x, ==, 3);
    g_assert_cmpfloat(primitive->data[1].point.y, ==, 4);

    adg_path_arc_to_explicit(path, 5, 6, 7, 8);
    primitive = adg_path_last_primitive(path);
    g_assert_nonnull(primitive);
    g_assert_cmpint(primitive->data->header.type, ==, CPML_ARC);
    g_assert_cmpint(primitive->data->header.length, ==, 3);
    g_assert_cmpfloat(primitive->org->point.x, ==, 3);
    g_assert_cmpfloat(primitive->org->point.y, ==, 4);
    g_assert_cmpfloat(primitive->data[1].point.x, ==, 5);
    g_assert_cmpfloat(primitive->data[1].point.y, ==, 6);
    g_assert_cmpfloat(primitive->data[2].point.x, ==, 7);
    g_assert_cmpfloat(primitive->data[2].point.y, ==, 8);

    adg_path_curve_to_explicit(path, 9, 10, 11, 12, 13, 14);
    primitive = adg_path_last_primitive(path);
    g_assert_nonnull(primitive);
    g_assert_cmpint(primitive->data->header.type, ==, CPML_CURVE);
    g_assert_cmpint(primitive->data->header.length, ==, 4);
    g_assert_cmpfloat(primitive->org->point.x, ==, 7);
    g_assert_cmpfloat(primitive->org->point.y, ==, 8);
    g_assert_cmpfloat(primitive->data[1].point.x, ==, 9);
    g_assert_cmpfloat(primitive->data[1].point.y, ==, 10);
    g_assert_cmpfloat(primitive->data[2].point.x, ==, 11);
    g_assert_cmpfloat(primitive->data[2].point.y, ==, 12);
    g_assert_cmpfloat(primitive->data[3].point.x, ==, 13);
    g_assert_cmpfloat(primitive->data[3].point.y, ==, 14);

    adg_path_close(path);
    primitive = adg_path_last_primitive(path);
    g_assert_nonnull(primitive);
    g_assert_cmpint(primitive->data->header.type, ==, CPML_CLOSE);
    g_assert_cmpint(primitive->data->header.length, ==, 1);
    g_assert_cmpfloat(primitive->org->point.x, ==, 13);
    g_assert_cmpfloat(primitive->org->point.y, ==, 14);

    /* This line is invalid because there is no current point */
    adg_path_line_to_explicit(path, 15, 16);
    g_assert_null(adg_path_last_primitive(path));

    g_object_unref(path);
}

static void
_adg_method_over_primitive(void)
{
    AdgPath *path;
    const CpmlPrimitive *primitive;

    /* Check sanity */
    g_assert_null(adg_path_over_primitive(NULL));

    path = adg_path_new();
    g_assert_null(adg_path_over_primitive(path));

    adg_path_move_to_explicit(path, 1, 2);
    g_assert_null(adg_path_over_primitive(path));

    adg_path_line_to_explicit(path, 3, 4);
    g_assert_null(adg_path_over_primitive(path));

    adg_path_arc_to_explicit(path, 5, 6, 7, 8);
    primitive = adg_path_over_primitive(path);
    g_assert_nonnull(primitive);
    g_assert_cmpint(primitive->data->header.type, ==, CPML_LINE);
    g_assert_cmpint(primitive->data->header.length, ==, 2);
    g_assert_cmpfloat(primitive->org->point.x, ==, 1);
    g_assert_cmpfloat(primitive->org->point.y, ==, 2);
    g_assert_cmpfloat(primitive->data[1].point.x, ==, 3);
    g_assert_cmpfloat(primitive->data[1].point.y, ==, 4);

    adg_path_curve_to_explicit(path, 9, 10, 11, 12, 13, 14);
    primitive = adg_path_over_primitive(path);
    g_assert_nonnull(primitive);
    g_assert_cmpint(primitive->data->header.type, ==, CPML_ARC);
    g_assert_cmpint(primitive->data->header.length, ==, 3);
    g_assert_cmpfloat(primitive->org->point.x, ==, 3);
    g_assert_cmpfloat(primitive->org->point.y, ==, 4);
    g_assert_cmpfloat(primitive->data[1].point.x, ==, 5);
    g_assert_cmpfloat(primitive->data[1].point.y, ==, 6);
    g_assert_cmpfloat(primitive->data[2].point.x, ==, 7);
    g_assert_cmpfloat(primitive->data[2].point.y, ==, 8);

    adg_path_close(path);
    primitive = adg_path_over_primitive(path);
    g_assert_nonnull(primitive);
    g_assert_cmpint(primitive->data->header.type, ==, CPML_CURVE);
    g_assert_cmpint(primitive->data->header.length, ==, 4);
    g_assert_cmpfloat(primitive->org->point.x, ==, 7);
    g_assert_cmpfloat(primitive->org->point.y, ==, 8);
    g_assert_cmpfloat(primitive->data[1].point.x, ==, 9);
    g_assert_cmpfloat(primitive->data[1].point.y, ==, 10);
    g_assert_cmpfloat(primitive->data[2].point.x, ==, 11);
    g_assert_cmpfloat(primitive->data[2].point.y, ==, 12);
    g_assert_cmpfloat(primitive->data[3].point.x, ==, 13);
    g_assert_cmpfloat(primitive->data[3].point.y, ==, 14);

    adg_path_move_to_explicit(path, 15, 16);
    primitive = adg_path_over_primitive(path);
    g_assert_nonnull(primitive);
    g_assert_cmpint(primitive->data->header.type, ==, CPML_CLOSE);
    g_assert_cmpint(primitive->data->header.length, ==, 1);
    g_assert_cmpfloat(primitive->org->point.x, ==, 13);
    g_assert_cmpfloat(primitive->org->point.y, ==, 14);

    /* A new segment clear the over primitive */
    adg_path_move_to_explicit(path, 17, 18);
    g_assert_null(adg_path_over_primitive(path));

    g_object_unref(path);
}

static void
_adg_method_append_primitive(void)
{
    AdgPath *path;
    CpmlSegment segment;
    CpmlPrimitive primitive;

    path = adg_path_new();
    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());
    cpml_primitive_from_segment(&primitive, &segment);

    /* Check sanity */
    adg_path_append_primitive(NULL, &primitive);
    adg_path_append_primitive(path, NULL);

    adg_path_move_to_explicit(path, 0, 0);
    g_assert_null(adg_path_last_primitive(path));

    adg_path_move_to_explicit(path, 0, 1);

    adg_path_append_primitive(path, &primitive);
    g_assert_cmpint(adg_path_last_primitive(path)->data->header.type, ==, CPML_LINE);

    cpml_primitive_next(&primitive);
    adg_path_close(path);
    adg_path_move_to_explicit(path, 0, 0);
    adg_path_append_primitive(path, &primitive);
    g_assert_null(adg_path_last_primitive(path));

    adg_path_line_to_explicit(path, 3, 1);
    adg_path_append_primitive(path, &primitive);
    g_assert_cmpint(adg_path_last_primitive(path)->data->header.type, ==, CPML_ARC);

    cpml_primitive_next(&primitive);
    adg_path_close(path);
    adg_path_move_to_explicit(path, 0, 0);
    adg_path_append_primitive(path, &primitive);
    g_assert_null(adg_path_last_primitive(path));

    adg_path_move_to_explicit(path, 6, 7);
    adg_path_append_primitive(path, &primitive);
    g_assert_cmpint(adg_path_last_primitive(path)->data->header.type, ==, CPML_CURVE);

    cpml_primitive_next(&primitive);
    adg_path_close(path);
    adg_path_move_to_explicit(path, 0, 0);
    adg_path_append_primitive(path, &primitive);
    g_assert_null(adg_path_last_primitive(path));

    adg_path_move_to_explicit(path, -2, 2);
    adg_path_append_primitive(path, &primitive);
    g_assert_cmpint(adg_path_last_primitive(path)->data->header.type, ==, CPML_CLOSE);

    /* Invalid primitives must be discarded */
    adg_path_move_to_explicit(path, 0, 0);
    adg_path_arc_to_explicit(path, 1, 2, 0, 1);
    cpml_primitive_reset(&primitive);
    primitive.org = NULL;
    adg_path_append_primitive(path, &primitive);
    g_assert_cmpint(adg_path_last_primitive(path)->data->header.type, ==, CPML_ARC);

    cpml_primitive_reset(&primitive);
    primitive.data = NULL;
    adg_path_append_primitive(path, &primitive);
    g_assert_cmpint(adg_path_last_primitive(path)->data->header.type, ==, CPML_ARC);

    cpml_primitive_reset(&primitive);
    adg_path_append_primitive(path, &primitive);
    g_assert_cmpint(adg_path_last_primitive(path)->data->header.type, ==, CPML_LINE);

    g_object_unref(path);
}

static void
_adg_method_append_segment(void)
{
    AdgPath *path;
    CpmlSegment segment;

    path = adg_path_new();

    /* Check sanity */
    adg_path_append_segment(NULL, &segment);
    adg_path_append_primitive(path, NULL);

    /* First segment */
    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());
    adg_path_append_segment(path, &segment);
    g_assert_nonnull(adg_path_last_primitive(path));
    g_assert_nonnull(adg_path_over_primitive(path));
    g_assert_cmpint(adg_path_last_primitive(path)->data->header.type, ==, CPML_CLOSE);
    g_assert_cmpint(adg_path_over_primitive(path)->data->header.type, ==, CPML_CURVE);

    /* Second segment */
    cpml_segment_next(&segment);
    adg_path_append_segment(path, &segment);
    g_assert_nonnull(adg_path_last_primitive(path));
    g_assert_nonnull(adg_path_over_primitive(path));
    g_assert_cmpint(adg_path_over_primitive(path)->data->header.type, ==, CPML_LINE);
    g_assert_cmpint(adg_path_last_primitive(path)->data->header.type, ==, CPML_LINE);

    /* Third segment */
    cpml_segment_next(&segment);
    adg_path_append_segment(path, &segment);
    g_assert_nonnull(adg_path_last_primitive(path));
    g_assert_nonnull(adg_path_over_primitive(path));
    g_assert_cmpint(adg_path_over_primitive(path)->data->header.type, ==, CPML_CURVE);
    g_assert_cmpint(adg_path_last_primitive(path)->data->header.type, ==, CPML_CLOSE);

    /* Forth segment */
    cpml_segment_next(&segment);
    adg_path_append_segment(path, &segment);
    g_assert_nonnull(adg_path_last_primitive(path));
    g_assert_nonnull(adg_path_over_primitive(path));
    g_assert_cmpint(adg_path_over_primitive(path)->data->header.type, ==, CPML_ARC);
    g_assert_cmpint(adg_path_last_primitive(path)->data->header.type, ==, CPML_ARC);

    /* Fifth segment */
    cpml_segment_next(&segment);
    adg_path_append_segment(path, &segment);
    g_assert_nonnull(adg_path_last_primitive(path));
    g_assert_nonnull(adg_path_over_primitive(path));
    g_assert_cmpint(adg_path_over_primitive(path)->data->header.type, ==, CPML_ARC);
    g_assert_cmpint(adg_path_last_primitive(path)->data->header.type, ==, CPML_CLOSE);

    g_object_unref(path);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_object_checks("/adg/path/type/object", ADG_TYPE_PATH);

    g_test_add_func("/adg/path/method/get-current-point", _adg_method_get_current_point);
    g_test_add_func("/adg/path/method/has-current-point", _adg_method_has_current_point);
    g_test_add_func("/adg/path/method/last-primitive", _adg_method_last_primitive);
    g_test_add_func("/adg/path/method/over-primitive", _adg_method_over_primitive);
    g_test_add_func("/adg/path/method/append-primitive", _adg_method_append_primitive);
    g_test_add_func("/adg/path/method/append-segment", _adg_method_append_segment);

    return g_test_run();
}
