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
#include <cpml.h>
#include <string.h>


static cairo_path_data_t data[] = {
    { .header = { CPML_MOVE, 2 }},
    { .point = { 0, 1 }},

    /* Line */
    { .header = { CPML_LINE, 2 }},
    { .point = { 2, 3 }},

    /* Arc */
    { .header = { CPML_ARC, 3 }},
    { .point = { 4, 5 }},
    { .point = { 6, 7 }},

    /* Curve */
    { .header = { CPML_CURVE, 4 }},
    { .point = { 8, 9 }},
    { .point = { 10, 11 }},
    { .point = { 12, 13 }},

    /* Closing primive */
    { .header = { CPML_CLOSE, 1 }}
};

cairo_path_t path = {
    CAIRO_STATUS_SUCCESS,
    data,
    G_N_ELEMENTS(data)
};


static void
_cpml_test_browsing(void)
{
    CpmlSegment segment;
    CpmlPrimitive primitive, primitive_copy;

    cpml_segment_from_cairo(&segment, &path);

    cpml_primitive_from_segment(&primitive, &segment);
    g_assert_cmpfloat((primitive.org)->point.x, ==, 0);
    g_assert_cmpfloat((primitive.org)->point.y, ==, 1);
    g_assert_cmpint((primitive.data)->header.type, ==, CPML_LINE);
    g_assert_true(cpml_primitive_next(&primitive));
    g_assert_cmpfloat((primitive.org)->point.x, ==, 2);
    g_assert_cmpfloat((primitive.org)->point.y, ==, 3);
    g_assert_cmpint((primitive.data)->header.type, ==, CPML_ARC);
    g_assert_true(cpml_primitive_next(&primitive));
    g_assert_cmpfloat((primitive.org)->point.x, ==, 6);
    g_assert_cmpfloat((primitive.org)->point.y, ==, 7);
    g_assert_cmpint((primitive.data)->header.type, ==, CPML_CURVE);
    g_assert_true(cpml_primitive_next(&primitive));
    g_assert_cmpfloat((primitive.org)->point.x, ==, 12);
    g_assert_cmpfloat((primitive.org)->point.y, ==, 13);
    g_assert_cmpint((primitive.data)->header.type, ==, CPML_CLOSE);
    g_assert_false(cpml_primitive_next(&primitive));

    cpml_primitive_reset(&primitive);
    g_assert_true(cpml_primitive_next(&primitive));
    cpml_primitive_reset(&primitive);
    cpml_primitive_reset(&primitive);
    g_assert_true(cpml_primitive_next(&primitive));
    g_assert_true(cpml_primitive_next(&primitive));
    g_assert_true(cpml_primitive_next(&primitive));
    g_assert_false(cpml_primitive_next(&primitive));

    cpml_primitive_copy(&primitive_copy, &primitive);
    g_assert_false(cpml_primitive_next(&primitive_copy));
    cpml_primitive_reset(&primitive);
    g_assert_false(cpml_primitive_next(&primitive_copy));
    cpml_primitive_reset(&primitive_copy);
    g_assert_true(cpml_primitive_next(&primitive_copy));
}

static void
_cpml_test_from_segment(void)
{
    CpmlSegment segment;
    CpmlPrimitive primitive;

    g_assert_true(cpml_segment_from_cairo(&segment, &path));

    cpml_primitive_from_segment(&primitive, &segment);
    g_assert_nonnull(primitive.segment);
    g_assert_nonnull(primitive.org);
    g_assert_nonnull(primitive.data);

    cpml_primitive_from_segment(&primitive, NULL);
    g_assert_null(primitive.segment);
    g_assert_null(primitive.org);
    g_assert_null(primitive.data);

    cpml_primitive_from_segment(&primitive, &segment);
    g_assert_nonnull(primitive.segment);
    g_assert_nonnull(primitive.org);
    g_assert_nonnull(primitive.data);
}

static void
_cpml_test_type_get_n_points(void)
{
    size_t n_points;

    n_points = cpml_primitive_type_get_n_points(CPML_MOVE);
    g_assert_cmpuint(n_points, ==, 0);

    n_points = cpml_primitive_type_get_n_points(CPML_LINE);
    g_assert_cmpuint(n_points, ==, 2);

    n_points = cpml_primitive_type_get_n_points(CPML_ARC);
    g_assert_cmpuint(n_points, ==, 3);

    n_points = cpml_primitive_type_get_n_points(CPML_CURVE);
    g_assert_cmpuint(n_points, ==, 4);

    n_points = cpml_primitive_type_get_n_points(CPML_CLOSE);
    g_assert_cmpuint(n_points, ==, 2);
}

static void
_cpml_test_get_n_points(void)
{
    CpmlSegment segment;
    CpmlPrimitive primitive;
    size_t n_points;

    cpml_segment_from_cairo(&segment, &path);

    /* Line */
    cpml_primitive_from_segment(&primitive, &segment);
    n_points = cpml_primitive_get_n_points(&primitive);
    g_assert_cmpuint(n_points, ==, 2);

    /* Arc */
    cpml_primitive_next(&primitive);
    n_points = cpml_primitive_get_n_points(&primitive);
    g_assert_cmpuint(n_points, ==, 3);

    /* Curve */
    cpml_primitive_next(&primitive);
    n_points = cpml_primitive_get_n_points(&primitive);
    g_assert_cmpuint(n_points, ==, 4);

    /* Close: although the end point is not needed, the CPML API
     * returns 2 points to treat this primitive as a CPML_LINE */
    cpml_primitive_next(&primitive);
    n_points = cpml_primitive_get_n_points(&primitive);
    g_assert_cmpuint(n_points, ==, 2);
}

static void
_cpml_test_set_point(void)
{
    gsize size;
    cairo_path_data_t data_copy[G_N_ELEMENTS(data)];
    cairo_path_t path_copy = {
        CAIRO_STATUS_SUCCESS,
        data_copy,
        G_N_ELEMENTS(data)
    };
    CpmlSegment segment;
    CpmlPrimitive primitive;
    CpmlPair pair, pair2;
    int equality;

    size = sizeof(data_copy);
    memcpy(data_copy, data, size);

    cpml_segment_from_cairo(&segment, &path_copy);

    /* Line */
    cpml_primitive_from_segment(&primitive, &segment);

    equality = memcmp(data_copy, data, size);
    g_assert_cmpint(equality, ==, 0);
    cpml_primitive_put_point(&primitive, 0, &pair);
    pair.x += 1;
    cpml_primitive_set_point(&primitive, 0, &pair);
    equality = memcmp(data_copy, data, size);
    g_assert_cmpint(equality, !=, 0);
    pair.x -= 1;
    cpml_primitive_set_point(&primitive, 0, &pair);
    equality = memcmp(data_copy, data, size);
    g_assert_cmpint(equality, ==, 0);
    cpml_primitive_put_point(&primitive, 1, &pair);
    pair.y += 1;
    cpml_primitive_set_point(&primitive, 1, &pair);
    equality = memcmp(data_copy, data, size);
    g_assert_cmpint(equality, !=, 0);
    /* On a CPML_LINE primitives, -1 and 1 indices are equals */
    cpml_primitive_put_point(&primitive, -1, &pair2);
    g_assert_cmpfloat(pair.x, ==, pair2.x);
    g_assert_cmpfloat(pair.y, ==, pair2.y);
    memcpy(data_copy, data, size);
    equality = memcmp(data_copy, data, size);
    g_assert_cmpint(equality, ==, 0);
    cpml_primitive_put_point(&primitive, 2, &pair);
    pair.x += 1;
    pair.y += 1;
    /* This should be a NOP without segfaults */
    cpml_primitive_set_point(&primitive, 2, &pair);
    equality = memcmp(data_copy, data, size);
    g_assert_cmpint(equality, ==, 0);

    /* From now on, memcpy() is assumed to force equality (as yet
     * proved by the previous assertions) and pair2 is used as a
     * different-from-everything pair, that is setting pair2 on any
     * point will break the equality between data and data_copy
     */
    pair2.x = 12345;
    pair2.y = 54321;

    /* Arc */
    cpml_primitive_next(&primitive);

    cpml_primitive_set_point(&primitive, 0, &pair2);
    equality = memcmp(data_copy, data, size);
    g_assert_cmpint(equality, !=, 0);
    memcpy(data_copy, data, size);
    cpml_primitive_set_point(&primitive, 1, &pair2);
    equality = memcmp(data_copy, data, size);
    g_assert_cmpint(equality, !=, 0);
    memcpy(data_copy, data, size);
    cpml_primitive_set_point(&primitive, 2, &pair2);
    equality = memcmp(data_copy, data, size);
    g_assert_cmpint(equality, !=, 0);
    memcpy(data_copy, data, size);
    cpml_primitive_set_point(&primitive, 3, &pair2);
    equality = memcmp(data_copy, data, size);
    g_assert_cmpint(equality, ==, 0);

    /* Curve */
    cpml_primitive_next(&primitive);

    cpml_primitive_set_point(&primitive, 0, &pair2);
    equality = memcmp(data_copy, data, size);
    g_assert_cmpint(equality, !=, 0);
    memcpy(data_copy, data, size);
    cpml_primitive_set_point(&primitive, 1, &pair2);
    equality = memcmp(data_copy, data, size);
    g_assert_cmpint(equality, !=, 0);
    memcpy(data_copy, data, size);
    cpml_primitive_set_point(&primitive, 2, &pair2);
    equality = memcmp(data_copy, data, size);
    g_assert_cmpint(equality, !=, 0);
    memcpy(data_copy, data, size);
    cpml_primitive_set_point(&primitive, 3, &pair2);
    equality = memcmp(data_copy, data, size);
    g_assert_cmpint(equality, !=, 0);
    memcpy(data_copy, data, size);
    cpml_primitive_set_point(&primitive, 4, &pair2);
    equality = memcmp(data_copy, data, size);
    g_assert_cmpint(equality, ==, 0);

    /* Close */
    cpml_primitive_next(&primitive);

    cpml_primitive_set_point(&primitive, 0, &pair2);
    equality = memcmp(data_copy, data, size);
    g_assert_cmpint(equality, !=, 0);
    memcpy(data_copy, data, size);
    cpml_primitive_set_point(&primitive, 1, &pair2);
    equality = memcmp(data_copy, data, size);
    g_assert_cmpint(equality, !=, 0);
    memcpy(data_copy, data, size);
    cpml_primitive_set_point(&primitive, 2, &pair2);
    equality = memcmp(data_copy, data, size);
    g_assert_cmpint(equality, ==, 0);
}

static void
_cpml_test_put_point(void)
{
    CpmlSegment segment;
    CpmlPrimitive primitive;
    CpmlPair pair;

    cpml_segment_from_cairo(&segment, &path);

    /* Line */
    cpml_primitive_from_segment(&primitive, &segment);

    cpml_primitive_put_point(&primitive, 0, &pair);
    g_assert_cmpfloat(pair.x, ==, 0);
    g_assert_cmpfloat(pair.y, ==, 1);
    cpml_primitive_put_point(&primitive, 1, &pair);
    g_assert_cmpfloat(pair.x, ==, 2);
    g_assert_cmpfloat(pair.y, ==, 3);
    cpml_primitive_put_point(&primitive, 2, &pair);
    g_assert_cmpfloat(pair.x, ==, 2);
    g_assert_cmpfloat(pair.y, ==, 3);
    /* The negative indices are checked only against CPML_LINE */
    cpml_primitive_put_point(&primitive, -1, &pair);
    g_assert_cmpfloat(pair.x, ==, 2);
    g_assert_cmpfloat(pair.y, ==, 3);
    cpml_primitive_put_point(&primitive, -2, &pair);
    g_assert_cmpfloat(pair.x, ==, 0);
    g_assert_cmpfloat(pair.y, ==, 1);
    cpml_primitive_put_point(&primitive, -3, &pair);
    g_assert_cmpfloat(pair.x, ==, 0);
    g_assert_cmpfloat(pair.y, ==, 1);

    /* Arc */
    cpml_primitive_next(&primitive);

    cpml_primitive_put_point(&primitive, 0, &pair);
    g_assert_cmpfloat(pair.x, ==, 2);
    g_assert_cmpfloat(pair.y, ==, 3);
    cpml_primitive_put_point(&primitive, 1, &pair);
    g_assert_cmpfloat(pair.x, ==, 4);
    g_assert_cmpfloat(pair.y, ==, 5);
    cpml_primitive_put_point(&primitive, 2, &pair);
    g_assert_cmpfloat(pair.x, ==, 6);
    g_assert_cmpfloat(pair.y, ==, 7);
    cpml_primitive_put_point(&primitive, 3, &pair);
    g_assert_cmpfloat(pair.x, ==, 6);
    g_assert_cmpfloat(pair.y, ==, 7);

    /* Curve */
    cpml_primitive_next(&primitive);

    cpml_primitive_put_point(&primitive, 0, &pair);
    g_assert_cmpfloat(pair.x, ==, 6);
    g_assert_cmpfloat(pair.y, ==, 7);
    cpml_primitive_put_point(&primitive, 1, &pair);
    g_assert_cmpfloat(pair.x, ==, 8);
    g_assert_cmpfloat(pair.y, ==, 9);
    cpml_primitive_put_point(&primitive, 2, &pair);
    g_assert_cmpfloat(pair.x, ==, 10);
    g_assert_cmpfloat(pair.y, ==, 11);
    cpml_primitive_put_point(&primitive, 3, &pair);
    g_assert_cmpfloat(pair.x, ==, 12);
    g_assert_cmpfloat(pair.y, ==, 13);
    cpml_primitive_put_point(&primitive, 4, &pair);
    g_assert_cmpfloat(pair.x, ==, 12);
    g_assert_cmpfloat(pair.y, ==, 13);

    /* Close */
    cpml_primitive_next(&primitive);

    cpml_primitive_put_point(&primitive, 0, &pair);
    g_assert_cmpfloat(pair.x, ==, 12);
    g_assert_cmpfloat(pair.y, ==, 13);
    cpml_primitive_put_point(&primitive, 1, &pair);
    g_assert_cmpfloat(pair.x, ==, 0);
    g_assert_cmpfloat(pair.y, ==, 1);
    cpml_primitive_put_point(&primitive, 2, &pair);
    g_assert_cmpfloat(pair.x, ==, 0);
    g_assert_cmpfloat(pair.y, ==, 1);
}

static void
_cpml_test_get_length(void)
{
    cairo_path_data_t rectangle_data[] = {
        { .header = { CPML_MOVE, 2 }},
        { .point = { 0, 0 }},

        { .header = { CPML_LINE, 2 }},
        { .point = { 2, 0 }},

        { .header = { CPML_LINE, 2 }},
        { .point = { 2, 1 }},

        { .header = { CPML_LINE, 2 }},
        { .point = { 0, 1 }},

        { .header = { CPML_CLOSE, 1 }}
    };
    cairo_path_t rectangle = {
        CAIRO_STATUS_SUCCESS,
        rectangle_data,
        G_N_ELEMENTS(rectangle_data)
    };
    CpmlSegment segment;
    CpmlPrimitive primitive;

    g_assert_cmpfloat(cpml_primitive_get_length(NULL), ==, 0);

    cpml_segment_from_cairo(&segment, &rectangle);

    cpml_primitive_from_segment(&primitive, &segment);
    g_assert_cmpfloat(cpml_primitive_get_length(&primitive), ==, 2);

    cpml_primitive_next(&primitive);
    g_assert_cmpfloat(cpml_primitive_get_length(&primitive), ==, 1);

    cpml_primitive_next(&primitive);
    g_assert_cmpfloat(cpml_primitive_get_length(&primitive), ==, 2);

    cpml_primitive_next(&primitive);
    g_assert_cmpfloat(cpml_primitive_get_length(&primitive), ==, 1);
}

static void
_cpml_test_put_intersections(void)
{
    cairo_path_data_t path1_data[] = {
        { .header = { CPML_MOVE, 2 }},
        { .point = { 98, 1 }},

        { .header = { CPML_LINE, 2 }},
        { .point = { 102, 1 }},

        { .header = { CPML_LINE, 2 }},
        { .point = { 102, 0 }},

        { .header = { CPML_LINE, 2 }},
        { .point = { 98, 0 }}
    }, path2_data[] = {
        { .header = { CPML_MOVE, 2 }},
        { .point = { 100, -1 }},

        { .header = { CPML_LINE, 2 }},
        { .point = { 100, 2 }}
    };
    cairo_path_t path1 = {
        CAIRO_STATUS_SUCCESS,
        path1_data,
        G_N_ELEMENTS(path1_data)
    }, path2 = {
        CAIRO_STATUS_SUCCESS,
        path2_data,
        G_N_ELEMENTS(path2_data)
    };
    CpmlSegment segment;
    CpmlPrimitive primitive, primitive2;
    CpmlPair pair[2];

    cpml_segment_from_cairo(&segment, &path2);
    cpml_primitive_from_segment(&primitive, &segment);

    cpml_segment_from_cairo(&segment, &path1);
    cpml_primitive_from_segment(&primitive2, &segment);

    g_assert_cmpint(cpml_primitive_put_intersections(NULL, &primitive2, 2, pair), ==, 0);
    g_assert_cmpint(cpml_primitive_put_intersections(&primitive, NULL, 2, pair), ==, 0);
    g_assert_cmpint(cpml_primitive_put_intersections(NULL, NULL, 2, pair), ==, 0);
    g_assert_cmpint(cpml_primitive_put_intersections(&primitive, &primitive2, 0, pair), ==, 0);
    g_assert_cmpint(cpml_primitive_put_intersections(&primitive, &primitive2, 2, NULL), ==, 0);

    /* The first primitive of path1 intersects (really) path2 in (0, 1) */
    g_assert_cmpint(cpml_primitive_put_intersections(&primitive, &primitive2, 2, pair), ==, 1);
    g_assert_cmpfloat(pair[0].x, ==, 100);
    g_assert_cmpfloat(pair[0].y, ==, 1);
    g_assert_cmpint(cpml_primitive_is_inside(&primitive, pair), ==, 1);
    g_assert_cmpint(cpml_primitive_is_inside(&primitive2, pair), ==, 1);
    g_assert_cmpint(cpml_primitive_put_intersections(&primitive2, &primitive, 2, pair), ==, 1);
    g_assert_cmpfloat(pair[0].x, ==, 100);
    g_assert_cmpfloat(pair[0].y, ==, 1);
    g_assert_cmpint(cpml_primitive_is_inside(&primitive2, pair), ==, 1);
    g_assert_cmpint(cpml_primitive_is_inside(&primitive, pair), ==, 1);

    /* The second primitive does not intersect */
    cpml_primitive_next(&primitive2);
    g_assert_cmpint(cpml_primitive_put_intersections(&primitive, &primitive2, 2, pair), ==, 0);
    g_assert_cmpint(cpml_primitive_put_intersections(&primitive2, &primitive, 2, pair), ==, 0);

    /* The third primitive intersects in (0, 0) */
    cpml_primitive_next(&primitive2);
    g_assert_cmpint(cpml_primitive_put_intersections(&primitive, &primitive2, 2, pair), ==, 1);
    g_assert_cmpfloat(pair[0].x, ==, 100);
    g_assert_cmpfloat(pair[0].y, ==, 0);
    g_assert_cmpint(cpml_primitive_put_intersections(&primitive2, &primitive, 2, pair), ==, 1);
    g_assert_cmpfloat(pair[0].x, ==, 100);
    g_assert_cmpfloat(pair[0].y, ==, 0);

    g_assert_cmpint(cpml_primitive_put_intersections_with_segment(NULL, &segment, 2, pair), ==, 0);
    g_assert_cmpint(cpml_primitive_put_intersections_with_segment(&primitive, NULL, 2, pair), ==, 0);
    g_assert_cmpint(cpml_primitive_put_intersections_with_segment(NULL, NULL, 2, pair), ==, 0);
    g_assert_cmpint(cpml_primitive_put_intersections_with_segment(&primitive, &segment, 0, pair), ==, 0);
    g_assert_cmpint(cpml_primitive_put_intersections_with_segment(&primitive, &segment, 2, NULL), ==, 0);

    /* path2 intesects the whole path1 in two points */
    g_assert_cmpint(cpml_primitive_put_intersections_with_segment(&primitive, &segment, 2, pair), ==, 2);
    g_assert_cmpfloat(pair[0].x, ==, 100);
    g_assert_cmpfloat(pair[0].y, ==, 1);
    g_assert_cmpfloat(pair[1].x, ==, 100);
    g_assert_cmpfloat(pair[1].y, ==, 0);

    cpml_segment_from_cairo(&segment, &path);
    cpml_primitive_from_segment(&primitive2, &segment);

    /* Check line-line intesection */
    g_assert_cmpint(cpml_primitive_put_intersections(&primitive2, &primitive, 2, pair), ==, 1);
    g_assert_cmpint(cpml_primitive_put_intersections(&primitive, &primitive2, 2, pair), ==, 1);

    /* Check the returned intersection is an hypothetical intersection */
    g_assert_cmpint(cpml_primitive_is_inside(&primitive2, pair), ==, 0);
    g_assert_cmpint(cpml_primitive_is_inside(&primitive, pair), ==, 0);

    /* Check line-arc intesection */
    cpml_primitive_next(&primitive2);
    g_assert_cmpint(cpml_primitive_put_intersections(&primitive2, &primitive, 2, pair), ==, 0);
    g_assert_cmpint(cpml_primitive_put_intersections(&primitive, &primitive2, 2, pair), ==, 0);

    /* Check line-curve intesection */
    cpml_primitive_next(&primitive2);
    g_assert_cmpint(cpml_primitive_put_intersections(&primitive2, &primitive, 2, pair), ==, 0);
    g_assert_cmpint(cpml_primitive_put_intersections(&primitive, &primitive2, 2, pair), ==, 0);

    /* Check line-close intesection */
    cpml_primitive_next(&primitive2);
    g_assert_cmpint(cpml_primitive_put_intersections(&primitive2, &primitive, 2, pair), ==, 1);
    g_assert_cmpint(cpml_primitive_put_intersections(&primitive, &primitive2, 2, pair), ==, 1);

    /* Check the returned intersection is an hypothetical intersection */
    g_assert_cmpint(cpml_primitive_is_inside(&primitive2, pair), ==, 0);
    g_assert_cmpint(cpml_primitive_is_inside(&primitive, pair), ==, 0);

    g_assert_false(cpml_primitive_next(&primitive2));
}

static void
_cpml_test_join(void)
{
    cairo_path_data_t path_data[] = {
        { .header = { CPML_MOVE, 2 }},
        { .point = { 0, 0 }},

        { .header = { CPML_LINE, 2 }},
        { .point = { 2, 0 }},

        { .header = { CPML_LINE, 2 }},
        { .point = { 2, 2 }},

        { .header = { CPML_LINE, 2 }},
        { .point = { 1, 2 }},

        { .header = { CPML_LINE, 2 }},
        { .point = { 1, -2 }}
    };
    cairo_path_t path = {
        CAIRO_STATUS_SUCCESS,
        path_data,
        G_N_ELEMENTS(path_data)
    };
    CpmlSegment segment;
    CpmlPrimitive primitive1, primitive2;

    cpml_segment_from_cairo(&segment, &path);

    cpml_primitive_from_segment(&primitive1, &segment);
    cpml_primitive_copy(&primitive2, &primitive1);
    cpml_primitive_next(&primitive2);

    /* primitive1 and primitive2 are already joint */
    g_assert_cmpint(cpml_primitive_join(&primitive1, &primitive2), ==, 1);
    g_assert_cmpfloat((primitive2.org)->point.x, ==, 2);
    g_assert_cmpfloat((primitive2.org)->point.y, ==, 0);

    cpml_primitive_next(&primitive2);
    /* Now primitive1 and primitive2 are divergent,
     * hence cannot be joined */
    g_assert_cmpint(cpml_primitive_join(&primitive1, &primitive2), ==, 0);

    cpml_primitive_next(&primitive2);
    g_assert_cmpint(cpml_primitive_join(&primitive1, &primitive2), ==, 1);
    g_assert_cmpfloat((primitive2.org)->point.x, ==, 1);
    g_assert_cmpfloat((primitive2.org)->point.y, ==, 0);
}

static int
_cpml_path_length(cairo_t *cr)
{
    cairo_path_t *path = cairo_copy_path(cr);
    int length = path->num_data;
    cairo_path_destroy(path);

    return length;
}

static void
_cpml_test_to_cairo(void)
{
    cairo_t *cr;
    CpmlSegment segment;
    CpmlPrimitive primitive;
    int length, last_length;

    cr = adg_test_cairo_context();
    cpml_segment_from_cairo(&segment, &path);
    cpml_primitive_from_segment(&primitive, &segment);

    g_assert_cmpint(_cpml_path_length(cr), ==, 0);
    cpml_primitive_to_cairo(NULL, cr);
    g_assert_cmpint(_cpml_path_length(cr), ==, 0);
    cpml_primitive_to_cairo(&primitive, NULL);
    g_assert_cmpint(_cpml_path_length(cr), ==, 0);

    length = 0;
    do {
        last_length = length;
        cpml_primitive_to_cairo(&primitive, cr);
        length = _cpml_path_length(cr);
        g_assert_cmpint(length, >, last_length);
    } while (cpml_primitive_next(&primitive));

    cairo_destroy(cr);
}

static void
_cpml_test_dump(void)
{
#if GLIB_CHECK_VERSION(2, 38, 0)
    if (g_test_subprocess()) {
        CpmlSegment segment;
        CpmlPrimitive primitive;

        /* This should not crash the process */
        cpml_primitive_dump(NULL, 1);

        cpml_segment_from_cairo(&segment, &path);
        cpml_primitive_from_segment(&primitive, &segment);
        cpml_primitive_dump(&primitive, 1);

        cpml_primitive_next(&primitive);
        cpml_primitive_dump(&primitive, 1);

        return;
    }

    g_test_trap_subprocess(NULL, 0, 0);
    g_test_trap_assert_passed();
    g_test_trap_assert_stderr_unmatched("?");
    g_test_trap_assert_stdout("*NULL*");
    g_test_trap_assert_stdout("*move*");
    g_test_trap_assert_stdout("*line*");
#endif
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/cpml/primitive/behavior/browsing", _cpml_test_browsing);

    adg_test_add_func("/cpml/primitive/method/from-segment", _cpml_test_from_segment);
    adg_test_add_func("/cpml/primitive/method/type-get-n-points", _cpml_test_type_get_n_points);
    adg_test_add_func("/cpml/primitive/method/get-n-points", _cpml_test_get_n_points);
    adg_test_add_func("/cpml/primitive/method/set-point", _cpml_test_set_point);
    adg_test_add_func("/cpml/primitive/method/put-point", _cpml_test_put_point);
    adg_test_add_func("/cpml/primitive/method/get-length", _cpml_test_get_length);
    adg_test_add_func("/cpml/primitive/method/put-intersections", _cpml_test_put_intersections);
    adg_test_add_func("/cpml/primitive/method/join", _cpml_test_join);
    adg_test_add_func("/cpml/primitive/method/to-cairo", _cpml_test_to_cairo);
    adg_test_add_func("/cpml/primitive/method/dump", _cpml_test_dump);

    return g_test_run();
}
