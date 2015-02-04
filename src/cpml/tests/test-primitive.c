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


static void
_cpml_test_browsing(void)
{
    CpmlSegment segment;
    CpmlPrimitive primitive, primitive_copy;

    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());

    cpml_primitive_from_segment(&primitive, &segment);
    g_assert_cmpfloat((primitive.org)->point.x, ==, 0);
    g_assert_cmpfloat((primitive.org)->point.y, ==, 1);
    g_assert_cmpint((primitive.data)->header.type, ==, CPML_LINE);
    g_assert_true(cpml_primitive_next(&primitive));
    g_assert_cmpfloat((primitive.org)->point.x, ==, 3);
    g_assert_cmpfloat((primitive.org)->point.y, ==, 1);
    g_assert_cmpint((primitive.data)->header.type, ==, CPML_ARC);
    g_assert_true(cpml_primitive_next(&primitive));
    g_assert_cmpfloat((primitive.org)->point.x, ==, 6);
    g_assert_cmpfloat((primitive.org)->point.y, ==, 7);
    g_assert_cmpint((primitive.data)->header.type, ==, CPML_CURVE);
    g_assert_true(cpml_primitive_next(&primitive));
    g_assert_cmpfloat((primitive.org)->point.x, ==, -2);
    g_assert_cmpfloat((primitive.org)->point.y, ==, 2);
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
_cpml_test_sanity_from_segment(gint i)
{
    CpmlPrimitive primitive;
    CpmlSegment segment;

    switch (i) {
    case 1:
        cpml_primitive_from_segment(NULL, &segment);
        break;
    case 2:
        cpml_primitive_from_segment(&primitive, NULL);
        break;
    default:
        g_test_trap_assert_failed();
        break;
    }
}

static void
_cpml_test_sanity_get_n_points(gint i)
{
    switch (i) {
    case 1:
        cpml_primitive_get_n_points(NULL);
        break;
    default:
        g_test_trap_assert_failed();
        break;
    }
}

static void
_cpml_test_sanity_put_point(gint i)
{
    CpmlSegment segment;
    CpmlPrimitive primitive;
    CpmlPair pair;

    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());
    cpml_primitive_from_segment(&primitive, &segment);

    switch (i) {
    case 1:
        cpml_primitive_put_point(NULL, 0, &pair);
        break;
    case 2:
        cpml_primitive_put_point(&primitive, 0, NULL);
        break;
    default:
        g_test_trap_assert_failed();
        break;
    }
}

static void
_cpml_test_sanity_get_length(gint i)
{
    switch (i) {
    case 1:
        cpml_primitive_get_length(NULL);
        break;
    default:
        g_test_trap_assert_failed();
        break;
    }
}

static void
_cpml_test_sanity_put_intersections(gint i)
{
    CpmlPrimitive primitive;
    CpmlPair pair;

    switch (i) {
    case 1:
        cpml_primitive_put_intersections(NULL, &primitive, 2, &pair);
        break;
    case 2:
        cpml_primitive_put_intersections(&primitive, NULL, 2, &pair);
        break;
    case 3:
        cpml_primitive_put_intersections(&primitive, &primitive, 2, NULL);
        break;
    default:
        g_test_trap_assert_failed();
        break;
    }
}

static void
_cpml_test_sanity_put_intersections_with_segment(gint i)
{
    CpmlPrimitive primitive;
    CpmlSegment segment;
    CpmlPair pair;

    switch (i) {
    case 1:
        cpml_primitive_put_intersections_with_segment(NULL, &segment, 2, &pair);
        break;
    case 2:
        cpml_primitive_put_intersections_with_segment(&primitive, NULL, 2, &pair);
        break;
    case 3:
        cpml_primitive_put_intersections_with_segment(&primitive, &segment, 2, NULL);
        break;
    default:
        g_test_trap_assert_failed();
        break;
    }
}

static void
_cpml_test_sanity_join(gint i)
{
    CpmlPrimitive primitive;

    switch (i) {
    case 1:
        cpml_primitive_join(NULL, &primitive);
        break;
    case 2:
        cpml_primitive_join(&primitive, NULL);
        break;
    default:
        g_test_trap_assert_failed();
        break;
    }
}

static void
_cpml_test_sanity_dump(gint i)
{
    switch (i) {
    case 1:
        cpml_segment_dump(NULL);
        break;
    default:
        g_test_trap_assert_failed();
        break;
    }
}

static void
_cpml_test_sanity_to_cairo(gint i)
{
    CpmlPrimitive primitive;

    switch (i) {
    case 1:
        cpml_primitive_to_cairo(NULL, adg_test_cairo_context());
        break;
    case 2:
        cpml_primitive_to_cairo(&primitive, NULL);
        break;
    default:
        g_test_trap_assert_failed();
        break;
    }
}

static void
_cpml_test_from_segment(void)
{
    CpmlSegment segment;
    CpmlPrimitive primitive;

    g_assert_true(cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path()));

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

    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());

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
    gsize data_size;
    CpmlSegment original, *segment;
    CpmlPrimitive primitive;
    CpmlPair pair, pair2;
    int equality;

    /* Work on a copy to not modify the original path data */
    cpml_segment_from_cairo(&original, (cairo_path_t *) adg_test_path());
    data_size = original.num_data * sizeof(cairo_path_data_t);
    segment = cpml_segment_deep_dup(&original);

    /* Line */
    cpml_primitive_from_segment(&primitive, segment);

    equality = memcmp(original.data, segment->data, data_size);
    g_assert_cmpint(equality, ==, 0);
    cpml_primitive_put_point(&primitive, 0, &pair);
    pair.x += 1;
    cpml_primitive_set_point(&primitive, 0, &pair);
    equality = memcmp(original.data, segment->data, data_size);
    g_assert_cmpint(equality, !=, 0);
    pair.x -= 1;
    cpml_primitive_set_point(&primitive, 0, &pair);
    equality = memcmp(original.data, segment->data, data_size);
    g_assert_cmpint(equality, ==, 0);
    cpml_primitive_put_point(&primitive, 1, &pair);
    pair.y += 1;
    cpml_primitive_set_point(&primitive, 1, &pair);
    equality = memcmp(original.data, segment->data, data_size);
    g_assert_cmpint(equality, !=, 0);
    /* On a CPML_LINE primitives, -1 and 1 indices are equals */
    cpml_primitive_put_point(&primitive, -1, &pair2);
    g_assert_cmpfloat(pair.x, ==, pair2.x);
    g_assert_cmpfloat(pair.y, ==, pair2.y);
    memcpy(segment->data, original.data, data_size);
    equality = memcmp(original.data, segment->data, data_size);
    g_assert_cmpint(equality, ==, 0);
    cpml_primitive_put_point(&primitive, 2, &pair);
    pair.x += 1;
    pair.y += 1;
    /* This should be a NOP without segfaults */
    cpml_primitive_set_point(&primitive, 2, &pair);
    equality = memcmp(original.data, segment->data, data_size);
    g_assert_cmpint(equality, ==, 0);

    /* From now on, memcpy() is assumed to force equality (as already
     * proved by the previous assertions) and pair2 is used as a
     * different-from-everything pair, that is setting pair2 on any
     * point will break the equality between segment->data and
     * original.data
     */
    pair2.x = 12345;
    pair2.y = 54321;

    /* Arc */
    cpml_primitive_next(&primitive);

    cpml_primitive_set_point(&primitive, 0, &pair2);
    equality = memcmp(original.data, segment->data, data_size);
    g_assert_cmpint(equality, !=, 0);
    memcpy(segment->data, original.data, data_size);
    cpml_primitive_set_point(&primitive, 1, &pair2);
    equality = memcmp(original.data, segment->data, data_size);
    g_assert_cmpint(equality, !=, 0);
    memcpy(segment->data, original.data, data_size);
    cpml_primitive_set_point(&primitive, 2, &pair2);
    equality = memcmp(original.data, segment->data, data_size);
    g_assert_cmpint(equality, !=, 0);
    memcpy(segment->data, original.data, data_size);
    cpml_primitive_set_point(&primitive, 3, &pair2);
    equality = memcmp(original.data, segment->data, data_size);
    g_assert_cmpint(equality, ==, 0);

    /* Curve */
    cpml_primitive_next(&primitive);

    cpml_primitive_set_point(&primitive, 0, &pair2);
    equality = memcmp(original.data, segment->data, data_size);
    g_assert_cmpint(equality, !=, 0);
    memcpy(segment->data, original.data, data_size);
    cpml_primitive_set_point(&primitive, 1, &pair2);
    equality = memcmp(original.data, segment->data, data_size);
    g_assert_cmpint(equality, !=, 0);
    memcpy(segment->data, original.data, data_size);
    cpml_primitive_set_point(&primitive, 2, &pair2);
    equality = memcmp(original.data, segment->data, data_size);
    g_assert_cmpint(equality, !=, 0);
    memcpy(segment->data, original.data, data_size);
    cpml_primitive_set_point(&primitive, 3, &pair2);
    equality = memcmp(original.data, segment->data, data_size);
    g_assert_cmpint(equality, !=, 0);
    memcpy(segment->data, original.data, data_size);
    cpml_primitive_set_point(&primitive, 4, &pair2);
    equality = memcmp(original.data, segment->data, data_size);
    g_assert_cmpint(equality, ==, 0);

    /* Close */
    cpml_primitive_next(&primitive);

    cpml_primitive_set_point(&primitive, 0, &pair2);
    equality = memcmp(original.data, segment->data, data_size);
    g_assert_cmpint(equality, !=, 0);
    memcpy(segment->data, original.data, data_size);
    cpml_primitive_set_point(&primitive, 1, &pair2);
    equality = memcmp(original.data, segment->data, data_size);
    g_assert_cmpint(equality, !=, 0);
    memcpy(segment->data, original.data, data_size);
    cpml_primitive_set_point(&primitive, 2, &pair2);
    equality = memcmp(original.data, segment->data, data_size);
    g_assert_cmpint(equality, ==, 0);

    g_free(segment);
}

static void
_cpml_test_put_point(void)
{
    CpmlSegment segment;
    CpmlPrimitive primitive;
    CpmlPair pair;

    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());

    /* Line */
    cpml_primitive_from_segment(&primitive, &segment);

    cpml_primitive_put_point(&primitive, 0, &pair);
    g_assert_cmpfloat(pair.x, ==, 0);
    g_assert_cmpfloat(pair.y, ==, 1);
    cpml_primitive_put_point(&primitive, 1, &pair);
    g_assert_cmpfloat(pair.x, ==, 3);
    g_assert_cmpfloat(pair.y, ==, 1);
    cpml_primitive_put_point(&primitive, 2, &pair);
    g_assert_cmpfloat(pair.x, ==, 3);
    g_assert_cmpfloat(pair.y, ==, 1);
    /* The negative indices are checked only against CPML_LINE */
    cpml_primitive_put_point(&primitive, -1, &pair);
    g_assert_cmpfloat(pair.x, ==, 3);
    g_assert_cmpfloat(pair.y, ==, 1);
    cpml_primitive_put_point(&primitive, -2, &pair);
    g_assert_cmpfloat(pair.x, ==, 0);
    g_assert_cmpfloat(pair.y, ==, 1);
    cpml_primitive_put_point(&primitive, -3, &pair);
    g_assert_cmpfloat(pair.x, ==, 0);
    g_assert_cmpfloat(pair.y, ==, 1);

    /* Arc */
    cpml_primitive_next(&primitive);

    cpml_primitive_put_point(&primitive, 0, &pair);
    g_assert_cmpfloat(pair.x, ==, 3);
    g_assert_cmpfloat(pair.y, ==, 1);
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
    g_assert_cmpfloat(pair.x, ==, -2);
    g_assert_cmpfloat(pair.y, ==, 2);
    cpml_primitive_put_point(&primitive, 4, &pair);
    g_assert_cmpfloat(pair.x, ==, -2);
    g_assert_cmpfloat(pair.y, ==, 2);

    /* Close */
    cpml_primitive_next(&primitive);

    cpml_primitive_put_point(&primitive, 0, &pair);
    g_assert_cmpfloat(pair.x, ==, -2);
    g_assert_cmpfloat(pair.y, ==, 2);
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
    CpmlSegment segment;
    CpmlPrimitive primitive;

    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());
    cpml_segment_next(&segment);
    cpml_primitive_from_segment(&primitive, &segment);

    g_assert_cmpfloat(cpml_primitive_get_length(&primitive), ==, 1);

    cpml_primitive_next(&primitive);
    g_assert_cmpfloat(cpml_primitive_get_length(&primitive), ==, 2);
}

static void
_cpml_test_put_intersections(void)
{
    CpmlSegment segment;
    CpmlPrimitive primitive1, primitive2;
    CpmlPair pair[2];

    /* Set primitive1 to 1.1 (first segment, first primitive) */
    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());
    cpml_primitive_from_segment(&primitive1, &segment);

    /* Set primitive2 to 2.1 (second segment, first primitive) */
    cpml_segment_next(&segment);
    cpml_primitive_from_segment(&primitive2, &segment);

    /* primitive1 (1.1) does not intersect primitive2 (2.1) */
    g_assert_cmpuint(cpml_primitive_put_intersections(&primitive1, &primitive2, 2, pair), ==, 0);

    cpml_primitive_next(&primitive2);

    /* primitive1 (1.1) intersects primitive2 (2.2) in (1, 1) */
    g_assert_cmpuint(cpml_primitive_put_intersections(&primitive1, &primitive2, 2, pair), ==, 1);
    g_assert_cmpfloat(pair[0].x, ==, 1);
    g_assert_cmpfloat(pair[0].y, ==, 1);
    g_assert_cmpint(cpml_primitive_is_inside(&primitive1, pair), ==, 1);
    g_assert_cmpint(cpml_primitive_is_inside(&primitive2, pair), ==, 1);

    /* Check the intersection is not returned when not requested */
    g_assert_cmpuint(cpml_primitive_put_intersections(&primitive1, &primitive2, 0, pair), ==, 0);

    cpml_primitive_next(&primitive1);

    /* primitive1 (1.2) does not intersect primitive2 (2.2) */
    g_assert_cmpuint(cpml_primitive_put_intersections(&primitive1, &primitive2, 2, pair), ==, 0);

    cpml_primitive_next(&primitive1);

    /* primitive1 (1.3) does not intersect primitive2 (2.2) */
    g_assert_cmpuint(cpml_primitive_put_intersections(&primitive1, &primitive2, 2, pair), ==, 0);

    cpml_primitive_next(&primitive1);

    /* primitive1 (1.4) intersects primitive2 (2.2), but outside their boundaries */
    g_assert_cmpuint(cpml_primitive_put_intersections(&primitive1, &primitive2, 2, pair), ==, 1);
    g_assert_cmpfloat(pair[0].x, ==, 1);
    g_assert_cmpfloat(pair[0].y, ==, -1);
    g_assert_cmpint(cpml_primitive_is_inside(&primitive1, pair), ==, 0);
    g_assert_cmpint(cpml_primitive_is_inside(&primitive2, pair), ==, 0);
}

static void
_cpml_test_put_intersections_with_segment(void)
{
    CpmlSegment segment;
    CpmlPrimitive primitive;
    CpmlPair pair[4];

    /* Set primitive to first segment, first primitive */
    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());
    cpml_primitive_from_segment(&primitive, &segment);

    /* Set segment to the second segment */
    cpml_segment_next(&segment);

    /* primitive (1.1) intersects segment (2) in (1, 1) */
    g_assert_cmpuint(cpml_primitive_put_intersections_with_segment(&primitive, &segment, 4, pair), ==, 1);
    g_assert_cmpfloat(pair[0].x, ==, 1);
    g_assert_cmpfloat(pair[0].y, ==, 1);

    cpml_primitive_next(&primitive);

    /* primitive (1.1) does not intersect segment (2) */
    g_assert_cmpuint(cpml_primitive_put_intersections_with_segment(&primitive, &segment, 4, pair), ==, 0);

    /* Set primitive to second segment, first primitive */
    cpml_primitive_from_segment(&primitive, &segment);

    /* Set segment to the first segment */
    cpml_segment_reset(&segment);

    /* primitive (2.1) intersects segment (1) in extrapolation.
     * TODO: change this behavior! They must not intersect. */
    g_assert_cmpuint(cpml_primitive_put_intersections_with_segment(&primitive, &segment, 4, pair), ==, 1);
    g_assert_cmpfloat(pair[0].x, ==, 2);
    g_assert_cmpfloat(pair[0].y, ==, 0);

    cpml_primitive_next(&primitive);

    /* primitive (2.2) wrongly intersects segment (1) */
    g_assert_cmpuint(cpml_primitive_put_intersections_with_segment(&primitive, &segment, 4, pair), ==, 1);
    g_assert_cmpfloat(pair[0].x, ==, 2);
    g_assert_cmpfloat(pair[0].y, ==, 0);
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

static void
_cpml_test_to_cairo(void)
{
    cairo_t *cr;
    CpmlSegment segment;
    CpmlPrimitive primitive;
    int length, last_length;

    cr = adg_test_cairo_context();
    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());
    cpml_primitive_from_segment(&primitive, &segment);

    g_assert_cmpint(adg_test_cairo_num_data(cr), ==, 0);

    length = 0;
    do {
        last_length = length;
        cpml_primitive_to_cairo(&primitive, cr);
        length = adg_test_cairo_num_data(cr);
        g_assert_cmpint(length, >, last_length);
    } while (cpml_primitive_next(&primitive));

    cairo_destroy(cr);
}

static void
_cpml_test_dump(gint i)
{
    CpmlSegment segment;
    CpmlPrimitive primitive;

    switch (i) {
    case 1:
        cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());
        cpml_primitive_from_segment(&primitive, &segment);
        cpml_primitive_dump(&primitive, 1);

        cpml_primitive_next(&primitive);
        cpml_primitive_dump(&primitive, 1);
        break;
    case 2:
        g_test_trap_assert_passed();
        g_test_trap_assert_stderr_unmatched("?");
        g_test_trap_assert_stdout("*NULL*");
        g_test_trap_assert_stdout("*move*");
        g_test_trap_assert_stdout("*line*");
    }
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    g_test_add_func("/cpml/primitive/behavior/browsing", _cpml_test_browsing);

    adg_test_add_traps("/cpml/primitive/sanity/from-segment", _cpml_test_sanity_from_segment, 1);
    adg_test_add_traps("/cpml/primitive/sanity/get-n-points", _cpml_test_sanity_get_n_points, 1);
    adg_test_add_traps("/cpml/primitive/sanity/put-point", _cpml_test_sanity_put_point, 2);
    adg_test_add_traps("/cpml/primitive/sanity/get-length", _cpml_test_sanity_get_length, 1);
    adg_test_add_traps("/cpml/primitive/sanity/put-intersections", _cpml_test_sanity_put_intersections, 3);
    adg_test_add_traps("/cpml/primitive/sanity/put-intersections-with-segment", _cpml_test_sanity_put_intersections_with_segment, 3);
    adg_test_add_traps("/cpml/primitive/sanity/join", _cpml_test_sanity_join, 2);
    adg_test_add_traps("/cpml/primitive/sanity/to-cairo", _cpml_test_sanity_to_cairo, 2);
    adg_test_add_traps("/cpml/primitive/sanity/dump", _cpml_test_sanity_dump, 1);

    g_test_add_func("/cpml/primitive/method/from-segment", _cpml_test_from_segment);
    g_test_add_func("/cpml/primitive/method/type-get-n-points", _cpml_test_type_get_n_points);
    g_test_add_func("/cpml/primitive/method/get-n-points", _cpml_test_get_n_points);
    g_test_add_func("/cpml/primitive/method/set-point", _cpml_test_set_point);
    g_test_add_func("/cpml/primitive/method/put-point", _cpml_test_put_point);
    g_test_add_func("/cpml/primitive/method/get-length", _cpml_test_get_length);
    g_test_add_func("/cpml/primitive/method/put-intersections", _cpml_test_put_intersections);
    g_test_add_func("/cpml/primitive/method/put-intersections-with-segment", _cpml_test_put_intersections_with_segment);
    g_test_add_func("/cpml/primitive/method/join", _cpml_test_join);
    g_test_add_func("/cpml/primitive/method/to-cairo", _cpml_test_to_cairo);
    adg_test_add_traps("/cpml/primitive/method/dump", _cpml_test_dump, 1);

    return g_test_run();
}
