/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2017  Nicola Fontana <ntd at entidi.it>
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
    adg_assert_isapprox(cp->x, 1);
    adg_assert_isapprox(cp->y, 2);

    adg_path_line_to_explicit(path, 3, 4);
    cp = adg_path_get_current_point(path);
    g_assert_nonnull(cp);
    adg_assert_isapprox(cp->x, 3);
    adg_assert_isapprox(cp->y, 4);

    adg_path_arc_to_explicit(path, 5, 6, 7, 8);
    cp = adg_path_get_current_point(path);
    g_assert_nonnull(cp);
    adg_assert_isapprox(cp->x, 7);
    adg_assert_isapprox(cp->y, 8);

    adg_path_curve_to_explicit(path, 9, 10, 11, 12, 13, 14);
    cp = adg_path_get_current_point(path);
    g_assert_nonnull(cp);
    adg_assert_isapprox(cp->x, 13);
    adg_assert_isapprox(cp->y, 14);

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

    adg_path_line_to_explicit(path, 0, 0);
    g_assert_true(adg_path_has_current_point(path));

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
    adg_assert_isapprox(primitive->org->point.x, 1);
    adg_assert_isapprox(primitive->org->point.y, 2);
    adg_assert_isapprox(primitive->data[1].point.x, 3);
    adg_assert_isapprox(primitive->data[1].point.y, 4);

    adg_path_arc_to_explicit(path, 5, 6, 7, 8);
    primitive = adg_path_last_primitive(path);
    g_assert_nonnull(primitive);
    g_assert_cmpint(primitive->data->header.type, ==, CPML_ARC);
    g_assert_cmpint(primitive->data->header.length, ==, 3);
    adg_assert_isapprox(primitive->org->point.x, 3);
    adg_assert_isapprox(primitive->org->point.y, 4);
    adg_assert_isapprox(primitive->data[1].point.x, 5);
    adg_assert_isapprox(primitive->data[1].point.y, 6);
    adg_assert_isapprox(primitive->data[2].point.x, 7);
    adg_assert_isapprox(primitive->data[2].point.y, 8);

    adg_path_move_to_explicit(path, 0, 0);
    primitive = adg_path_last_primitive(path);
    g_assert_nonnull(primitive);
    g_assert_cmpint(primitive->data->header.type, ==, CPML_ARC);

    adg_path_move_to_explicit(path, 1, 1);
    primitive = adg_path_last_primitive(path);
    g_assert_nonnull(primitive);
    g_assert_cmpint(primitive->data->header.type, ==, CPML_ARC);

    adg_path_curve_to_explicit(path, 9, 10, 11, 12, 13, 14);
    primitive = adg_path_last_primitive(path);
    g_assert_nonnull(primitive);
    g_assert_cmpint(primitive->data->header.type, ==, CPML_CURVE);
    g_assert_cmpint(primitive->data->header.length, ==, 4);
    adg_assert_isapprox(primitive->org->point.x, 1);
    adg_assert_isapprox(primitive->org->point.y, 1);
    adg_assert_isapprox(primitive->data[1].point.x, 9);
    adg_assert_isapprox(primitive->data[1].point.y, 10);
    adg_assert_isapprox(primitive->data[2].point.x, 11);
    adg_assert_isapprox(primitive->data[2].point.y, 12);
    adg_assert_isapprox(primitive->data[3].point.x, 13);
    adg_assert_isapprox(primitive->data[3].point.y, 14);

    adg_path_close(path);
    primitive = adg_path_last_primitive(path);
    g_assert_nonnull(primitive);
    g_assert_cmpint(primitive->data->header.type, ==, CPML_CLOSE);
    g_assert_cmpint(primitive->data->header.length, ==, 1);
    adg_assert_isapprox(primitive->org->point.x, 13);
    adg_assert_isapprox(primitive->org->point.y, 14);

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
    adg_assert_isapprox(primitive->org->point.x, 1);
    adg_assert_isapprox(primitive->org->point.y, 2);
    adg_assert_isapprox(primitive->data[1].point.x, 3);
    adg_assert_isapprox(primitive->data[1].point.y, 4);

    adg_path_curve_to_explicit(path, 9, 10, 11, 12, 13, 14);
    primitive = adg_path_over_primitive(path);
    g_assert_nonnull(primitive);
    g_assert_cmpint(primitive->data->header.type, ==, CPML_ARC);
    g_assert_cmpint(primitive->data->header.length, ==, 3);
    adg_assert_isapprox(primitive->org->point.x, 3);
    adg_assert_isapprox(primitive->org->point.y, 4);
    adg_assert_isapprox(primitive->data[1].point.x, 5);
    adg_assert_isapprox(primitive->data[1].point.y, 6);
    adg_assert_isapprox(primitive->data[2].point.x, 7);
    adg_assert_isapprox(primitive->data[2].point.y, 8);

    adg_path_close(path);
    primitive = adg_path_over_primitive(path);
    g_assert_nonnull(primitive);
    g_assert_cmpint(primitive->data->header.type, ==, CPML_CURVE);
    g_assert_cmpint(primitive->data->header.length, ==, 4);
    adg_assert_isapprox(primitive->org->point.x, 7);
    adg_assert_isapprox(primitive->org->point.y, 8);
    adg_assert_isapprox(primitive->data[1].point.x, 9);
    adg_assert_isapprox(primitive->data[1].point.y, 10);
    adg_assert_isapprox(primitive->data[2].point.x, 11);
    adg_assert_isapprox(primitive->data[2].point.y, 12);
    adg_assert_isapprox(primitive->data[3].point.x, 13);
    adg_assert_isapprox(primitive->data[3].point.y, 14);

    adg_path_move_to_explicit(path, 15, 16);
    primitive = adg_path_over_primitive(path);
    g_assert_nonnull(primitive);
    g_assert_cmpint(primitive->data->header.type, ==, CPML_CURVE);
    g_assert_cmpint(primitive->data->header.length, ==, 4);
    adg_assert_isapprox(primitive->org->point.x, 7);
    adg_assert_isapprox(primitive->org->point.y, 8);
    adg_assert_isapprox(primitive->data[1].point.x, 9);
    adg_assert_isapprox(primitive->data[1].point.y, 10);
    adg_assert_isapprox(primitive->data[2].point.x, 11);
    adg_assert_isapprox(primitive->data[2].point.y, 12);
    adg_assert_isapprox(primitive->data[3].point.x, 13);
    adg_assert_isapprox(primitive->data[3].point.y, 14);

    adg_path_line_to_explicit(path, 17, 18);
    primitive = adg_path_over_primitive(path);
    g_assert_nonnull(primitive);
    g_assert_cmpint(primitive->data->header.type, ==, CPML_CLOSE);
    g_assert_cmpint(primitive->data->header.length, ==, 1);
    adg_assert_isapprox(primitive->org->point.x, 13);
    adg_assert_isapprox(primitive->org->point.y, 14);

    g_object_unref(path);
}

static void
_adg_method_append_primitive(void)
{
    AdgPath *path;
    CpmlSegment segment;
    CpmlPrimitive primitive;
    const CpmlPrimitive *last;

    path = adg_path_new();
    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());
    cpml_primitive_from_segment(&primitive, &segment);

    /* Check sanity */
    adg_path_append_primitive(NULL, &primitive);
    adg_path_append_primitive(path, NULL);

    adg_path_move_to_explicit(path, 0, 1);

    adg_path_append_primitive(path, &primitive);
    last = adg_path_last_primitive(path);
    g_assert_nonnull(last);
    g_assert_cmpint(last->data->header.type, ==, CPML_LINE);

    cpml_primitive_next(&primitive);
    adg_path_append_primitive(path, &primitive);
    last = adg_path_last_primitive(path);
    g_assert_nonnull(last);
    g_assert_cmpint(last->data->header.type, ==, CPML_ARC);

    cpml_primitive_next(&primitive);
    adg_path_append_primitive(path, &primitive);
    last = adg_path_last_primitive(path);
    g_assert_nonnull(last);
    g_assert_cmpint(last->data->header.type, ==, CPML_CURVE);

    cpml_primitive_next(&primitive);
    adg_path_append_primitive(path, &primitive);
    last = adg_path_last_primitive(path);
    g_assert_nonnull(last);
    g_assert_cmpint(last->data->header.type, ==, CPML_CLOSE);

    /* Now trying to reappending with mismatching start point */
    adg_path_move_to_explicit(path, -1, -2);

    cpml_primitive_reset(&primitive);
    adg_path_append_primitive(path, &primitive);
    last = adg_path_last_primitive(path);
    g_assert_nonnull(last);
    g_assert_cmpint(last->data->header.type, ==, CPML_CLOSE);

    cpml_primitive_next(&primitive);
    adg_path_append_primitive(path, &primitive);
    last = adg_path_last_primitive(path);
    g_assert_nonnull(last);
    g_assert_cmpint(last->data->header.type, ==, CPML_CLOSE);

    cpml_primitive_next(&primitive);
    adg_path_append_primitive(path, &primitive);
    last = adg_path_last_primitive(path);
    g_assert_nonnull(last);
    g_assert_cmpint(last->data->header.type, ==, CPML_CLOSE);

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
    adg_path_append_segment(path, NULL);

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

static void
_adg_method_append_cairo_path(void)
{
    AdgPath *path;
    const cairo_path_t *cairo_path;

    path = adg_path_new();
    cairo_path = adg_test_path();

    /* Check sanity */
    adg_path_append_cairo_path(NULL, cairo_path);
    adg_path_append_cairo_path(path, NULL);

    /* Ensure the path is initially empty */
    g_assert_null(adg_path_last_primitive(path));

    adg_path_append_cairo_path(path, cairo_path);

    /* Check the path is no more empty */
    g_assert_nonnull(adg_path_last_primitive(path));

    g_object_unref(path);
}

static void
_adg_method_append_trail(void)
{
    AdgPath *path;
    AdgTrail *trail;
    const CpmlPair *pair;

    path = adg_path_new();
    trail = ADG_TRAIL(adg_path_new());

    /* The trail must be non-empty and with a named pair */
    adg_path_append_cairo_path(ADG_PATH(trail), adg_test_path());
    adg_model_set_named_pair_explicit(ADG_MODEL(trail), "test", 1, 2);

    /* Check sanity */
    adg_path_append_trail(NULL, trail);
    adg_path_append_trail(path, NULL);

    /* Ensure path is initially empty */
    g_assert_null(adg_path_last_primitive(path));

    adg_path_append_trail(path, trail);

    /* Check that path is no more empty */
    g_assert_nonnull(adg_path_last_primitive(path));

    /* Check that "test" named pair has been transferred to path */
    pair = adg_model_get_named_pair(ADG_MODEL(path), "test");
    g_assert_nonnull(pair);
    adg_assert_isapprox(pair->x, 1);
    adg_assert_isapprox(pair->y, 2);

    g_object_unref(path);
}

static void
_adg_method_move_to(void)
{
    AdgPath *path = adg_path_new();
    CpmlPair pair = { 1, 2 };
    const CpmlPair *cp;

    /* Check sanity */
    adg_path_move_to(NULL, &pair);
    adg_path_move_to(path, NULL);
    adg_path_move_to_explicit(NULL, 3, 4);

    adg_path_move_to(path, &pair);
    cp = adg_path_get_current_point(path);
    adg_assert_isapprox(cp->x, 1);
    adg_assert_isapprox(cp->y, 2);

    adg_path_move_to_explicit(path, 3, 4);
    cp = adg_path_get_current_point(path);
    adg_assert_isapprox(cp->x, 3);
    adg_assert_isapprox(cp->y, 4);

    g_object_unref(path);
}

static void
_adg_method_line_to(void)
{
    AdgPath *path = adg_path_new();
    CpmlPair pair = { 1, 2 };
    const CpmlPair *cp;

    /* Check sanity */
    adg_path_line_to(NULL, &pair);
    adg_path_line_to(path, NULL);
    adg_path_line_to_explicit(NULL, 3, 4);

    /* This should fail because there is no current point */
    adg_path_line_to(path, &pair);
    g_assert_null(adg_path_last_primitive(path));

    adg_path_move_to_explicit(path, 0, 0);
    adg_path_line_to(path, &pair);
    g_assert_nonnull(adg_path_last_primitive(path));
    cp = adg_path_get_current_point(path);
    adg_assert_isapprox(cp->x, 1);
    adg_assert_isapprox(cp->y, 2);

    adg_path_line_to_explicit(path, 3, 4);
    cp = adg_path_get_current_point(path);
    adg_assert_isapprox(cp->x, 3);
    adg_assert_isapprox(cp->y, 4);

    g_object_unref(path);
}

static void
_adg_method_arc_to(void)
{
    AdgPath *path = adg_path_new();
    CpmlPair pair[] = {
        { 1, 2 },
        { 3, 4 }
    };
    const CpmlPair *cp;

    /* Check sanity */
    adg_path_arc_to(NULL, &pair[0], &pair[1]);
    adg_path_arc_to(path, NULL, &pair[1]);
    adg_path_arc_to(path, &pair[0], NULL);
    adg_path_arc_to_explicit(NULL, 5, 6, 7, 8);

    /* This should fail because there is no current point */
    adg_path_arc_to(path, &pair[0], &pair[1]);
    g_assert_null(adg_path_last_primitive(path));

    adg_path_move_to_explicit(path, 0, 0);
    adg_path_arc_to(path, &pair[0], &pair[1]);
    g_assert_nonnull(adg_path_last_primitive(path));
    cp = adg_path_get_current_point(path);
    adg_assert_isapprox(cp->x, 3);
    adg_assert_isapprox(cp->y, 4);

    adg_path_arc_to_explicit(path, 5, 6, 7, 8);
    cp = adg_path_get_current_point(path);
    adg_assert_isapprox(cp->x, 7);
    adg_assert_isapprox(cp->y, 8);

    g_object_unref(path);
}

static void
_adg_method_curve_to(void)
{
    AdgPath *path = adg_path_new();
    CpmlPair pair[] = {
        { 1, 2 },
        { 3, 4 },
        { 5, 6 },
    };
    const CpmlPair *cp;

    /* Check sanity */
    adg_path_curve_to(NULL, &pair[0], &pair[1], &pair[2]);
    adg_path_curve_to(path, NULL, &pair[1], &pair[2]);
    adg_path_curve_to(path, &pair[0], NULL, &pair[2]);
    adg_path_curve_to(path, &pair[0], &pair[1], NULL);
    adg_path_curve_to_explicit(NULL, 7, 8, 9, 10, 11, 12);

    /* This should fail because there is no current point */
    adg_path_curve_to(path, &pair[0], &pair[1], &pair[2]);
    g_assert_null(adg_path_last_primitive(path));

    adg_path_move_to_explicit(path, 0, 0);
    adg_path_curve_to(path, &pair[0], &pair[1], &pair[2]);
    g_assert_nonnull(adg_path_last_primitive(path));
    cp = adg_path_get_current_point(path);
    adg_assert_isapprox(cp->x, 5);
    adg_assert_isapprox(cp->y, 6);

    adg_path_curve_to_explicit(path, 7, 8, 9, 10, 11, 12);
    cp = adg_path_get_current_point(path);
    adg_assert_isapprox(cp->x, 11);
    adg_assert_isapprox(cp->y, 12);

    g_object_unref(path);
}

static void
_adg_method_arc(void)
{
    AdgPath *path = adg_path_new();
    CpmlPair pair = { 3, 4 };
    cairo_path_t *cairo_path;
    CpmlSegment segment;
    CpmlPrimitive primitive;

    /* Check sanity */
    adg_path_arc(NULL, &pair, 1, 2, 3);
    adg_path_arc(path, NULL, 1, 2, 3);
    adg_path_arc_explicit(NULL, 1, 2, 3, 4, 5);

    /* This should *not* fail because this primitive
     * automatically adds a leading CPML_MOVE */
    adg_path_arc(path, &pair, 2, 0, G_PI_2);
    g_assert_nonnull(adg_path_last_primitive(path));

    /* Disconnected arcs should be automatically
     * joined with CPML_LINEs */
    adg_path_arc_explicit(path, 3, 4, 2, G_PI, -G_PI);
    g_assert_nonnull(adg_path_last_primitive(path));

    /* Check that the result is the expected one */
    cairo_path = adg_trail_cairo_path(ADG_TRAIL(path));
    g_assert_nonnull(cairo_path);
    g_assert_true(cpml_segment_from_cairo(&segment, cairo_path));

    cpml_primitive_from_segment(&primitive, &segment);
    g_assert_cmpint(primitive.data[0].header.type, ==, CPML_ARC);
    g_assert_cmpint(primitive.data[0].header.length, ==, 3);
    adg_assert_isapprox((primitive.org)->point.x, 5);
    adg_assert_isapprox((primitive.org)->point.y, 4);
    adg_assert_isapprox(primitive.data[1].point.x, 4.414);
    adg_assert_isapprox(primitive.data[1].point.y, 5.414);
    adg_assert_isapprox(primitive.data[2].point.x, 3);
    adg_assert_isapprox(primitive.data[2].point.y, 6);

    g_assert_true(cpml_primitive_next(&primitive));
    g_assert_cmpint(primitive.data[0].header.type, ==, CPML_LINE);
    g_assert_cmpint(primitive.data[0].header.length, ==, 2);
    adg_assert_isapprox(primitive.data[1].point.x, 1);
    adg_assert_isapprox(primitive.data[1].point.y, 4);

    g_assert_true(cpml_primitive_next(&primitive));
    g_assert_cmpint(primitive.data[0].header.type, ==, CPML_ARC);
    g_assert_cmpint(primitive.data[0].header.length, ==, 3);
    adg_assert_isapprox(primitive.data[1].point.x, 5);
    adg_assert_isapprox(primitive.data[1].point.y, 4);
    adg_assert_isapprox(primitive.data[2].point.x, 1);
    adg_assert_isapprox(primitive.data[2].point.y, 4);

    g_assert_false(cpml_primitive_next(&primitive));

    g_object_unref(path);
}

static void
_adg_method_chamfer(void)
{
    AdgPath *path;
    cairo_path_t *cairo_path;
    CpmlSegment segment;
    CpmlPrimitive primitive;

    /* Sanity checks */
    adg_path_chamfer(NULL, 1, 2);

    /* Perform an easy chamfer */
    path = adg_path_new();
    adg_path_move_to_explicit(path, 0, 0);
    adg_path_line_to_explicit(path, 0, 8);
    adg_path_chamfer(path, 2, 3);
    adg_path_line_to_explicit(path, 10, 8);

    /* Check that the result is the expected one */
    cairo_path = adg_trail_cairo_path(ADG_TRAIL(path));
    g_assert_nonnull(cairo_path);
    g_assert_true(cpml_segment_from_cairo(&segment, cairo_path));

    cpml_primitive_from_segment(&primitive, &segment);
    g_assert_cmpint(primitive.data[0].header.type, ==, CPML_LINE);
    g_assert_cmpint(primitive.data[0].header.length, ==, 2);
    adg_assert_isapprox((primitive.org)->point.x, 0);
    adg_assert_isapprox((primitive.org)->point.y, 0);
    adg_assert_isapprox(primitive.data[1].point.x, 0);
    adg_assert_isapprox(primitive.data[1].point.y, 6);

    g_assert_true(cpml_primitive_next(&primitive));
    g_assert_cmpint(primitive.data[0].header.type, ==, CPML_LINE);
    g_assert_cmpint(primitive.data[0].header.length, ==, 2);
    adg_assert_isapprox((primitive.org)->point.x, 0);
    adg_assert_isapprox((primitive.org)->point.y, 6);
    adg_assert_isapprox(primitive.data[1].point.x, 3);
    adg_assert_isapprox(primitive.data[1].point.y, 8);

    g_assert_true(cpml_primitive_next(&primitive));
    g_assert_cmpint(primitive.data[0].header.type, ==, CPML_LINE);
    g_assert_cmpint(primitive.data[0].header.length, ==, 2);
    adg_assert_isapprox((primitive.org)->point.x, 3);
    adg_assert_isapprox((primitive.org)->point.y, 8);
    adg_assert_isapprox(primitive.data[1].point.x, 10);
    adg_assert_isapprox(primitive.data[1].point.y, 8);

    g_assert_false(cpml_primitive_next(&primitive));

    /* TODO: perform more complex tests */

    g_object_unref(path);
}

static void
_adg_method_fillet(void)
{
    AdgPath *path;
    cairo_path_t *cairo_path;
    CpmlSegment segment;
    CpmlPrimitive primitive;

    /* Sanity checks */
    adg_path_fillet(NULL, 1);

    /* Perform an easy fillet */
    path = adg_path_new();
    adg_path_move_to_explicit(path, 0, 0);
    adg_path_line_to_explicit(path, 0, 8);
    adg_path_fillet(path, 3);
    adg_path_line_to_explicit(path, 10, 8);

    /* Check that the result is the expected one */
    cairo_path = adg_trail_cairo_path(ADG_TRAIL(path));
    g_assert_nonnull(cairo_path);
    g_assert_true(cpml_segment_from_cairo(&segment, cairo_path));

    cpml_primitive_from_segment(&primitive, &segment);
    g_assert_cmpint(primitive.data[0].header.type, ==, CPML_LINE);
    g_assert_cmpint(primitive.data[0].header.length, ==, 2);
    adg_assert_isapprox((primitive.org)->point.x, 0);
    adg_assert_isapprox((primitive.org)->point.y, 0);
    adg_assert_isapprox(primitive.data[1].point.x, 0);
    adg_assert_isapprox(primitive.data[1].point.y, 5);

    g_assert_true(cpml_primitive_next(&primitive));
    g_assert_cmpint(primitive.data[0].header.type, ==, CPML_ARC);
    g_assert_cmpint(primitive.data[0].header.length, ==, 3);
    adg_assert_isapprox((primitive.org)->point.x, 0);
    adg_assert_isapprox((primitive.org)->point.y, 5);
    adg_assert_isapprox(primitive.data[1].point.x, 0.879);
    adg_assert_isapprox(primitive.data[1].point.y, 7.121);
    adg_assert_isapprox(primitive.data[2].point.x, 3);
    adg_assert_isapprox(primitive.data[2].point.y, 8);

    g_assert_true(cpml_primitive_next(&primitive));
    g_assert_cmpint(primitive.data[0].header.type, ==, CPML_LINE);
    g_assert_cmpint(primitive.data[0].header.length, ==, 2);
    adg_assert_isapprox((primitive.org)->point.x, 3);
    adg_assert_isapprox((primitive.org)->point.y, 8);
    adg_assert_isapprox(primitive.data[1].point.x, 10);
    adg_assert_isapprox(primitive.data[1].point.y, 8);

    g_assert_false(cpml_primitive_next(&primitive));

    /* TODO: perform more complex tests */

    g_object_unref(path);
}

static void
_adg_method_reflect(void)
{
    AdgPath *path;
    CpmlPair pair;
    const CpmlPair *p;
    cairo_path_t *cairo_path;
    CpmlSegment segment;
    CpmlPrimitive primitive;

    path = adg_path_new();

    /* Check sanity */
    adg_path_reflect(NULL, &pair);
    adg_path_reflect_explicit(NULL, 1, 2);
    adg_path_reflect_explicit(path, 0, 0);

    adg_model_set_named_pair_explicit(ADG_MODEL(path), "P1", 1, 2);

    pair.x = -2;
    pair.y = -3;
    adg_model_set_named_pair(ADG_MODEL(path), "P2", &pair);

    adg_path_move_to_explicit(path, 0, 1);
    adg_path_line_to_explicit(path, 2, 3);
    adg_path_arc_to_explicit(path, 4, 5, 6, 7);
    adg_path_curve_to_explicit(path, 8, 9, 10, 11, 12, 13);

    /* Not specifying the vector means reflect on y=0 */
    adg_path_reflect(path, NULL);

    /* Check that the result is the expected one */
    cairo_path = adg_trail_cairo_path(ADG_TRAIL(path));
    g_assert_nonnull(cairo_path);
    g_assert_true(cpml_segment_from_cairo(&segment, cairo_path));

    /* Skip the original primitives */
    cpml_primitive_from_segment(&primitive, &segment);  /* CPML_LINE */
    g_assert_true(cpml_primitive_next(&primitive));     /* CPML_ARC */
    g_assert_true(cpml_primitive_next(&primitive));     /* CPML_CURVE */

    /* This line is the automatic joint between the
     * original primitives and the reversed ones */
    g_assert_true(cpml_primitive_next(&primitive));
    g_assert_cmpint(primitive.data[0].header.type, ==, CPML_LINE);
    g_assert_cmpint(primitive.data[0].header.length, ==, 2);
    adg_assert_isapprox((primitive.org)->point.x, 12);
    adg_assert_isapprox((primitive.org)->point.y, 13);
    adg_assert_isapprox(primitive.data[1].point.x, 12);
    adg_assert_isapprox(primitive.data[1].point.y, -13);

    g_assert_true(cpml_primitive_next(&primitive));
    g_assert_cmpint(primitive.data[0].header.type, ==, CPML_CURVE);
    g_assert_cmpint(primitive.data[0].header.length, ==, 4);
    adg_assert_isapprox(primitive.data[1].point.x, 10);
    adg_assert_isapprox(primitive.data[1].point.y, -11);
    adg_assert_isapprox(primitive.data[2].point.x, 8);
    adg_assert_isapprox(primitive.data[2].point.y, -9);
    adg_assert_isapprox(primitive.data[3].point.x, 6);
    adg_assert_isapprox(primitive.data[3].point.y, -7);

    g_assert_true(cpml_primitive_next(&primitive));
    g_assert_cmpint(primitive.data[0].header.type, ==, CPML_ARC);
    g_assert_cmpint(primitive.data[0].header.length, ==, 3);
    adg_assert_isapprox(primitive.data[1].point.x, 4);
    adg_assert_isapprox(primitive.data[1].point.y, -5);
    adg_assert_isapprox(primitive.data[2].point.x, 2);
    adg_assert_isapprox(primitive.data[2].point.y, -3);

    g_assert_true(cpml_primitive_next(&primitive));
    g_assert_cmpint(primitive.data[0].header.type, ==, CPML_LINE);
    g_assert_cmpint(primitive.data[0].header.length, ==, 2);
    adg_assert_isapprox(primitive.data[1].point.x, 0);
    adg_assert_isapprox(primitive.data[1].point.y, -1);

    g_assert_false(cpml_primitive_next(&primitive));

    /* Check if the named pairs have been duplicated and mirrored */
    p = adg_model_get_named_pair(ADG_MODEL(path), "P1");
    adg_assert_isapprox(p->x, 1);
    adg_assert_isapprox(p->y, 2);

    p = adg_model_get_named_pair(ADG_MODEL(path), "-P1");
    adg_assert_isapprox(p->x, 1);
    adg_assert_isapprox(p->y, -2);

    p = adg_model_get_named_pair(ADG_MODEL(path), "P2");
    adg_assert_isapprox(p->x, -2);
    adg_assert_isapprox(p->y, -3);

    p = adg_model_get_named_pair(ADG_MODEL(path), "-P2");
    adg_assert_isapprox(p->x, -2);
    adg_assert_isapprox(p->y, 3);

    g_object_unref(path);

    path = adg_path_new();

    pair.x = 10;
    pair.y = 20;
    adg_model_set_named_pair(ADG_MODEL(path), "P1", &pair);

    adg_model_set_named_pair_explicit(ADG_MODEL(path), "P2", -20, -30);

    adg_path_move_to_explicit(path, 0, 10);
    adg_path_line_to_explicit(path, 20, 30);

    /* Reflect on the x=0 axis */
    adg_path_reflect_explicit(path, 0, 100);

    /* Check that the result is the expected one */
    cairo_path = adg_trail_cairo_path(ADG_TRAIL(path));
    g_assert_nonnull(cairo_path);
    g_assert_true(cpml_segment_from_cairo(&segment, cairo_path));

    /* Skip the original primitives */
    cpml_primitive_from_segment(&primitive, &segment);  /* CPML_LINE */

    /* This line is the automatic joint between the
     * original primitives and the reversed ones */
    g_assert_true(cpml_primitive_next(&primitive));
    g_assert_cmpint(primitive.data[0].header.type, ==, CPML_LINE);
    g_assert_cmpint(primitive.data[0].header.length, ==, 2);
    adg_assert_isapprox((primitive.org)->point.x, 20);
    adg_assert_isapprox((primitive.org)->point.y, 30);
    adg_assert_isapprox(primitive.data[1].point.x, -20);
    adg_assert_isapprox(primitive.data[1].point.y, 30);

    g_assert_true(cpml_primitive_next(&primitive));
    g_assert_cmpint(primitive.data[0].header.type, ==, CPML_LINE);
    g_assert_cmpint(primitive.data[0].header.length, ==, 2);
    adg_assert_isapprox(primitive.data[1].point.x, 0);
    adg_assert_isapprox(primitive.data[1].point.y, 10);

    /* Check if the named pairs have been duplicated and mirrored */
    p = adg_model_get_named_pair(ADG_MODEL(path), "P1");
    adg_assert_isapprox(p->x, 10);
    adg_assert_isapprox(p->y, 20);

    p = adg_model_get_named_pair(ADG_MODEL(path), "-P1");
    adg_assert_isapprox(p->x, -10);
    adg_assert_isapprox(p->y, 20);

    p = adg_model_get_named_pair(ADG_MODEL(path), "P2");
    adg_assert_isapprox(p->x, -20);
    adg_assert_isapprox(p->y, -30);

    p = adg_model_get_named_pair(ADG_MODEL(path), "-P2");
    adg_assert_isapprox(p->x, 20);
    adg_assert_isapprox(p->y, -30);

    g_object_unref(path);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_object_checks("/adg/path/type/object", ADG_TYPE_PATH);
    adg_test_add_model_checks("/adg/path/type/model", ADG_TYPE_PATH);

    g_test_add_func("/adg/path/method/get-current-point", _adg_method_get_current_point);
    g_test_add_func("/adg/path/method/has-current-point", _adg_method_has_current_point);
    g_test_add_func("/adg/path/method/last-primitive", _adg_method_last_primitive);
    g_test_add_func("/adg/path/method/over-primitive", _adg_method_over_primitive);
    g_test_add_func("/adg/path/method/append-primitive", _adg_method_append_primitive);
    g_test_add_func("/adg/path/method/append-segment", _adg_method_append_segment);
    g_test_add_func("/adg/path/method/append-cairo-path", _adg_method_append_cairo_path);
    g_test_add_func("/adg/path/method/append-trail", _adg_method_append_trail);
    g_test_add_func("/adg/path/method/move-to", _adg_method_move_to);
    g_test_add_func("/adg/path/method/line-to", _adg_method_line_to);
    g_test_add_func("/adg/path/method/arc-to", _adg_method_arc_to);
    g_test_add_func("/adg/path/method/curve-to", _adg_method_curve_to);
    g_test_add_func("/adg/path/method/arc", _adg_method_arc);
    g_test_add_func("/adg/path/method/chamfer", _adg_method_chamfer);
    g_test_add_func("/adg/path/method/fillet", _adg_method_fillet);
    g_test_add_func("/adg/path/method/reflect", _adg_method_reflect);

    return g_test_run();
}
