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

    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());

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
_cpml_test_sanity_copy(gint i)
{
    CpmlPrimitive primitive;
    CpmlSegment segment;

    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());
    cpml_primitive_from_segment(&primitive, &segment);

    switch (i) {
    case 1:
        cpml_primitive_copy(NULL, &primitive);
        break;
    case 2:
        cpml_primitive_copy(&primitive, NULL);
        break;
    default:
        g_test_trap_assert_failed();
        break;
    }
}

static void
_cpml_test_sanity_copy_data(gint i)
{
    CpmlPrimitive primitive;
    CpmlSegment segment;

    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());
    cpml_primitive_from_segment(&primitive, &segment);

    switch (i) {
    case 1:
        cpml_primitive_copy_data(NULL, &primitive);
        break;
    case 2:
        cpml_primitive_copy_data(&primitive, NULL);
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
_cpml_test_sanity_put_extents(gint i)
{
    CpmlSegment segment;
    CpmlPrimitive primitive;
    CpmlExtents extents;

    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());
    cpml_primitive_from_segment(&primitive, &segment);

    switch (i) {
    case 1:
        cpml_primitive_put_extents(NULL, &extents);
        break;
    case 2:
        cpml_primitive_put_extents(&primitive, NULL);
        break;
    default:
        g_test_trap_assert_failed();
        break;
    }
}

static void
_cpml_test_sanity_put_pair_at(gint i)
{
    CpmlSegment segment;
    CpmlPrimitive primitive;
    CpmlPair pair;

    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());
    cpml_primitive_from_segment(&primitive, &segment);

    switch (i) {
    case 1:
        cpml_primitive_put_pair_at(NULL, 1, &pair);
        break;
    case 2:
        cpml_primitive_put_pair_at(&primitive, 1, NULL);
        break;
    default:
        g_test_trap_assert_failed();
        break;
    }
}

static void
_cpml_test_sanity_put_vector_at(gint i)
{
    CpmlSegment segment;
    CpmlPrimitive primitive;
    CpmlVector vector;

    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());
    cpml_primitive_from_segment(&primitive, &segment);

    switch (i) {
    case 1:
        cpml_primitive_put_vector_at(NULL, 1, &vector);
        break;
    case 2:
        cpml_primitive_put_vector_at(&primitive, 1, NULL);
        break;
    default:
        g_test_trap_assert_failed();
        break;
    }
}

static void
_cpml_test_sanity_get_closest_pos(gint i)
{
    CpmlSegment segment;
    CpmlPrimitive primitive;
    CpmlPair pair = { 1, 1 };

    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());
    cpml_primitive_from_segment(&primitive, &segment);

    switch (i) {
    case 1:
        cpml_primitive_get_closest_pos(NULL, &pair);
        break;
    case 2:
        cpml_primitive_get_closest_pos(&primitive, NULL);
        break;
    default:
        g_test_trap_assert_failed();
        break;
    }
}

static void
_cpml_test_sanity_set_point(gint i)
{
    CpmlSegment segment;
    CpmlPrimitive primitive;
    CpmlPair pair = { 1, 1 };

    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());
    cpml_primitive_from_segment(&primitive, &segment);

    switch (i) {
    case 1:
        cpml_primitive_set_point(NULL, 1, &pair);
        break;
    case 2:
        cpml_primitive_set_point(&primitive, 1, NULL);
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
_cpml_test_sanity_put_intersections(gint i)
{
    CpmlSegment segment;
    CpmlPrimitive primitive1, primitive2;
    CpmlPair pair;

    /* Set primitive1 to 1.1 and primitive 2 to 2.2,
     * so there is an intersection point in (1, 1) */
    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());
    cpml_primitive_from_segment(&primitive1, &segment);

    cpml_segment_next(&segment);
    cpml_primitive_from_segment(&primitive2, &segment);
    cpml_primitive_next(&primitive2);

    switch (i) {
    case 1:
        cpml_primitive_put_intersections(NULL, &primitive2, 2, &pair);
        break;
    case 2:
        cpml_primitive_put_intersections(&primitive1, NULL, 2, &pair);
        break;
    case 3:
        cpml_primitive_put_intersections(&primitive1, &primitive2, 2, NULL);
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

    /* Set primitive 1.1 and segment to 2
     * so there is an intersection point in (1,1) */
    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());
    cpml_primitive_from_segment(&primitive, &segment);

    cpml_segment_next(&segment);

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
    CpmlSegment segment;
    CpmlPrimitive primitive;

    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());
    cpml_primitive_from_segment(&primitive, &segment);

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
_cpml_test_sanity_offset(gint i)
{
    switch (i) {
    case 1:
        cpml_primitive_offset(NULL, 1);
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
    CpmlSegment segment;
    CpmlPrimitive primitive;

    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());
    cpml_primitive_from_segment(&primitive, &segment);

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
_cpml_test_copy(void)
{
    CpmlSegment segment;
    CpmlPrimitive original;
    CpmlPrimitive primitive = { NULL, NULL, NULL };

    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());
    cpml_primitive_from_segment(&original, &segment);

    g_assert_false(original.segment == primitive.segment);
    g_assert_false(original.org == primitive.org);
    g_assert_false(original.data == primitive.data);

    cpml_primitive_copy(&primitive, &original);

    g_assert_true(original.segment == primitive.segment);
    g_assert_true(original.org == primitive.org);
    g_assert_true(original.data == primitive.data);
}

static void
_cpml_test_copy_data(void)
{
    CpmlSegment segment;
    CpmlPrimitive original, *primitive;
    int n;

    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());
    cpml_primitive_from_segment(&original, &segment);

    /* Check incompatible primitives are not copied */
    primitive = cpml_primitive_deep_dup(&original);
    ++primitive->data[0].header.length;
    g_assert_cmpint(cpml_primitive_copy_data(primitive, &original), ==, 0);
    --primitive->data[0].header.length;
    ++primitive->data[0].header.type;
    g_assert_cmpint(cpml_primitive_copy_data(primitive, &original), ==, 0);
    --primitive->data[0].header.type;

    do {
        primitive = cpml_primitive_deep_dup(&original);
        ++primitive->org->point.x;
        ++primitive->org->point.y;
        for (n = 1; n < primitive->data[0].header.length; ++n) {
            ++primitive->data[n].point.x;
            ++primitive->data[n].point.y;
        }
        g_assert_cmpfloat(primitive->org->point.x, !=, original.org->point.x);
        g_assert_cmpfloat(primitive->org->point.y, !=, original.org->point.y);
        for (n = 1; n < primitive->data[0].header.length; ++n) {
            g_assert_cmpfloat(primitive->data[n].point.x, !=, original.data[n].point.x);
            g_assert_cmpfloat(primitive->data[n].point.y, !=, original.data[n].point.y);
        }
        g_assert_cmpint(cpml_primitive_copy_data(primitive, &original), ==, 1);
        g_assert_cmpfloat(primitive->org->point.x, ==, original.org->point.x);
        g_assert_cmpfloat(primitive->org->point.y, ==, original.org->point.y);
        for (n = 1; n < primitive->data[0].header.length; ++n) {
            g_assert_cmpfloat(primitive->data[n].point.x, ==, original.data[n].point.x);
            g_assert_cmpfloat(primitive->data[n].point.y, ==, original.data[n].point.y);
        }
        g_free(primitive);
    } while (cpml_primitive_next(&original));
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
_cpml_test_put_extents(void)
{
    CpmlSegment segment;
    CpmlPrimitive primitive;
    CpmlExtents extents;

    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());

    /* Line */
    cpml_primitive_from_segment(&primitive, &segment);
    cpml_primitive_put_extents(&primitive, &extents);
    g_assert_true(extents.is_defined);
    g_assert_cmpfloat(extents.org.x, ==, 0);
    g_assert_cmpfloat(extents.org.y, ==, 1);
    g_assert_cmpfloat(extents.size.x, ==, 3);
    g_assert_cmpfloat(extents.size.y, ==, 0);

    /* Arc: the extents are computed precisely... let's ensure
     * at least all the 3 points are included */
    cpml_primitive_next(&primitive);
    cpml_primitive_put_extents(&primitive, &extents);
    g_assert_true(extents.is_defined);
    g_assert_cmpfloat(extents.org.x, <=, 3);
    g_assert_cmpfloat(extents.org.y, <=, 1);
    g_assert_cmpfloat(extents.size.x, >=, 3);
    g_assert_cmpfloat(extents.size.y, >=, 6);

    /* Curve: actually the extents are computed by using the
     * convex hull (hence the exact coordinates of the points) */
    cpml_primitive_next(&primitive);
    cpml_primitive_put_extents(&primitive, &extents);
    g_assert_true(extents.is_defined);
    g_assert_cmpfloat(extents.org.x, ==, -2);
    g_assert_cmpfloat(extents.org.y, ==, 2);
    g_assert_cmpfloat(extents.size.x, ==, 12);
    g_assert_cmpfloat(extents.size.y, ==, 9);

    /* Close */
    cpml_primitive_next(&primitive);
    cpml_primitive_put_extents(&primitive, &extents);
    g_assert_true(extents.is_defined);
    g_assert_cmpfloat(extents.org.x, ==, -2);
    g_assert_cmpfloat(extents.org.y, ==, 1);
    g_assert_cmpfloat(extents.size.x, ==, 2);
    g_assert_cmpfloat(extents.size.y, ==, 1);
}

static void
_cpml_test_put_pair_at(void)
{
    CpmlSegment segment;
    CpmlPrimitive primitive;
    CpmlPair pair;

    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());

    /* Line */
    cpml_primitive_from_segment(&primitive, &segment);
    cpml_primitive_put_pair_at(&primitive, 0, &pair);
    g_assert_cmpfloat(pair.x, ==, 0);
    g_assert_cmpfloat(pair.y, ==, 1);
    cpml_primitive_put_pair_at(&primitive, 1, &pair);
    g_assert_cmpfloat(pair.x, ==, 3);
    g_assert_cmpfloat(pair.y, ==, 1);
    cpml_primitive_put_pair_at(&primitive, 0.5, &pair);
    g_assert_cmpfloat(pair.x, ==, 1.5);
    g_assert_cmpfloat(pair.y, ==, 1);

    /* Arc */
    cpml_primitive_next(&primitive);
    cpml_primitive_put_pair_at(&primitive, 0, &pair);
    g_assert_cmpfloat(pair.x, ==, 3);
    g_assert_cmpfloat(pair.y, ==, 1);
    cpml_primitive_put_pair_at(&primitive, 1, &pair);
    g_assert_cmpfloat(pair.x, ==, 6);
    g_assert_cmpfloat(pair.y, ==, 7);
    cpml_primitive_put_pair_at(&primitive, 0.5, &pair);
    adg_assert_isapprox(pair.x, 3.669);
    adg_assert_isapprox(pair.y, 4.415);

    /* Close */
    cpml_primitive_next(&primitive);
    /* TODO: not yet implemented
     * cpml_primitive_put_pair_at(&primitive, 0, &pair);
     * g_assert_cmpfloat(pair.x, ==, 6);
     * g_assert_cmpfloat(pair.y, ==, 7);
     * cpml_primitive_put_pair_at(&primitive, 1, &pair);
     * g_assert_cmpfloat(pair.x, ==, -2);
     * g_assert_cmpfloat(pair.y, ==, 2);
     * cpml_primitive_put_pair_at(&primitive, 0.5, &pair);
     * g_assert_cmpfloat(pair.x, ==, 1);
     * g_assert_cmpfloat(pair.y, ==, 1); */

    /* Close */
    cpml_primitive_next(&primitive);
    cpml_primitive_put_pair_at(&primitive, 0, &pair);
    g_assert_cmpfloat(pair.x, ==, -2);
    g_assert_cmpfloat(pair.y, ==, 2);
    cpml_primitive_put_pair_at(&primitive, 1, &pair);
    g_assert_cmpfloat(pair.x, ==, 0);
    g_assert_cmpfloat(pair.y, ==, 1);
    cpml_primitive_put_pair_at(&primitive, 0.5, &pair);
    g_assert_cmpfloat(pair.x, ==, -1);
    g_assert_cmpfloat(pair.y, ==, 1.5);
}

static void
_cpml_test_put_vector_at(void)
{
    CpmlSegment segment;
    CpmlPrimitive primitive;
    CpmlVector vector;

    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());

    /* Line */
    cpml_primitive_from_segment(&primitive, &segment);
    cpml_primitive_put_vector_at(&primitive, 0, &vector);
    g_assert_cmpfloat(vector.x, ==, 3);
    g_assert_cmpfloat(vector.y, ==, 0);
    cpml_primitive_put_vector_at(&primitive, 1, &vector);
    g_assert_cmpfloat(vector.x, ==, 3);
    g_assert_cmpfloat(vector.y, ==, 0);
    cpml_primitive_put_vector_at(&primitive, 0.5, &vector);
    g_assert_cmpfloat(vector.x, ==, 3);
    g_assert_cmpfloat(vector.y, ==, 0);

    /* Arc */
    cpml_primitive_next(&primitive);
    cpml_primitive_put_vector_at(&primitive, 0, &vector);
    adg_assert_isapprox(vector.x, -0.077);
    adg_assert_isapprox(vector.y, 0.997);
    cpml_primitive_put_vector_at(&primitive, 1, &vector);
    adg_assert_isapprox(vector.x, 0.844);
    adg_assert_isapprox(vector.y, 0.537);
    cpml_primitive_put_vector_at(&primitive, 0.5, &vector);
    adg_assert_isapprox(vector.x, 0.447);
    adg_assert_isapprox(vector.y, 0.894);

    /* Close */
    cpml_primitive_next(&primitive);
    /* TODO: not yet implemented
     * cpml_primitive_put_vector_at(&primitive, 0, &vector);
     * g_assert_cmpfloat(vector.x, ==, 6);
     * g_assert_cmpfloat(vector.y, ==, 7);
     * cpml_primitive_put_vector_at(&primitive, 1, &vector);
     * g_assert_cmpfloat(vector.x, ==, -2);
     * g_assert_cmpfloat(vector.y, ==, 2);
     * cpml_primitive_put_vector_at(&primitive, 0.5, &vector);
     * g_assert_cmpfloat(vector.x, ==, 1);
     * g_assert_cmpfloat(vector.y, ==, 1); */

    /* Close */
    cpml_primitive_next(&primitive);
    cpml_primitive_put_vector_at(&primitive, 0, &vector);
    g_assert_cmpfloat(vector.x, ==, 2);
    g_assert_cmpfloat(vector.y, ==, -1);
    cpml_primitive_put_vector_at(&primitive, 1, &vector);
    g_assert_cmpfloat(vector.x, ==, 2);
    g_assert_cmpfloat(vector.y, ==, -1);
    cpml_primitive_put_vector_at(&primitive, 0.5, &vector);
    g_assert_cmpfloat(vector.x, ==, 2);
    g_assert_cmpfloat(vector.y, ==, -1);
}

static void
_cpml_test_get_closest_pos(void)
{
    CpmlSegment segment;
    CpmlPrimitive primitive;
    CpmlPair pair;

    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());
    cpml_primitive_from_segment(&primitive, &segment);

    /* Line */
    pair.x = 0; pair.y = 1;
    g_assert_cmpfloat(cpml_primitive_get_closest_pos(&primitive, &pair), ==, 0);
    pair.x = 3; pair.y = 1;
    g_assert_cmpfloat(cpml_primitive_get_closest_pos(&primitive, &pair), ==, 1);
    pair.x = -1; pair.y = -1;
    g_assert_cmpfloat(cpml_primitive_get_closest_pos(&primitive, &pair), ==, 0);
    pair.x = 4; pair.y = 2;
    g_assert_cmpfloat(cpml_primitive_get_closest_pos(&primitive, &pair), ==, 1);
    pair.x = 1.5; pair.y = 0;
    g_assert_cmpfloat(cpml_primitive_get_closest_pos(&primitive, &pair), ==, 0.5);

    /* Arc */
    cpml_primitive_next(&primitive);
    /* TODO: not yet implemented
     * pair.x = 3; pair.y = 1;
     * g_assert_cmpfloat(cpml_primitive_get_closest_pos(&primitive, &pair), ==, 0);
     * pair.x = 6; pair.y = 7;
     * g_assert_cmpfloat(cpml_primitive_get_closest_pos(&primitive, &pair), ==, 1);
     * pair.x = 0; pair.y = 0;
     * g_assert_cmpfloat(cpml_primitive_get_closest_pos(&primitive, &pair), ==, 0);
     * pair.x = 10; pair.y = 10;
     * g_assert_cmpfloat(cpml_primitive_get_closest_pos(&primitive, &pair), ==, 1);
     */

    /* Close */
    cpml_primitive_next(&primitive);
    /* TODO: not yet implemented
     * pair.x = 6; pair.y = 7;
     * g_assert_cmpfloat(cpml_primitive_get_closest_pos(&primitive, &pair), ==, 0);
     * pair.x = -2; pair.y = 2;
     * g_assert_cmpfloat(cpml_primitive_get_closest_pos(&primitive, &pair), ==, 1);
     * pair.x = 10; pair.y = 10;
     * g_assert_cmpfloat(cpml_primitive_get_closest_pos(&primitive, &pair), ==, 0);
     * pair.x = 0; pair.y = 0;
     * g_assert_cmpfloat(cpml_primitive_get_closest_pos(&primitive, &pair), ==, 1);
     */

    /* Close */
    cpml_primitive_next(&primitive);
    pair.x = -2; pair.y = 2;
    g_assert_cmpfloat(cpml_primitive_get_closest_pos(&primitive, &pair), ==, 0);
    pair.x = 0; pair.y = 1;
    g_assert_cmpfloat(cpml_primitive_get_closest_pos(&primitive, &pair), ==, 1);
    pair.x = -3; pair.y = 3;
    g_assert_cmpfloat(cpml_primitive_get_closest_pos(&primitive, &pair), ==, 0);
    pair.x = 1; pair.y = 0;
    g_assert_cmpfloat(cpml_primitive_get_closest_pos(&primitive, &pair), ==, 1);
    pair.x = -1; pair.y = 1.5;
    g_assert_cmpfloat(cpml_primitive_get_closest_pos(&primitive, &pair), ==, 0.5);
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
_cpml_test_offset(void)
{
    CpmlSegment original, *segment;
    CpmlPrimitive primitive, line, curve;
    CpmlPrimitive *backup;

    /* Work on a copy to avoid modifying the original cairo path */
    cpml_segment_from_cairo(&original, (cairo_path_t *) adg_test_path());
    segment = cpml_segment_deep_dup(&original);
    cpml_primitive_from_segment(&primitive, segment);

    /* Offsetting and de-offsetting can introduce rounding errors
     * so we use adg_assert_isapprox instead of g_assert_cmpfloat */

    /* Line */
    cpml_primitive_copy(&line, &primitive);
    cpml_primitive_offset(&primitive, 1);
    adg_assert_isapprox((primitive.org)->point.x, 0);
    adg_assert_isapprox((primitive.org)->point.y, 2);
    adg_assert_isapprox(primitive.data[1].point.x, 3);
    adg_assert_isapprox(primitive.data[1].point.y, 2);
    cpml_primitive_offset(&primitive, -1);
    adg_assert_isapprox((primitive.org)->point.x, 0);
    adg_assert_isapprox((primitive.org)->point.y, 1);
    adg_assert_isapprox(primitive.data[1].point.x, 3);
    adg_assert_isapprox(primitive.data[1].point.y, 1);

    /* Arc */
    cpml_primitive_next(&primitive);
    cpml_primitive_offset(&primitive, 1);
    adg_assert_isapprox((primitive.org)->point.x, 2.003);
    adg_assert_isapprox((primitive.org)->point.y, 0.923);
    adg_assert_isapprox(primitive.data[1].point.x, 3.156);
    adg_assert_isapprox(primitive.data[1].point.y, 5.537);
    adg_assert_isapprox(primitive.data[2].point.x, 5.463);
    adg_assert_isapprox(primitive.data[2].point.y, 7.844);
    cpml_primitive_offset(&primitive, -1);
    adg_assert_isapprox((primitive.org)->point.x, 3);
    adg_assert_isapprox((primitive.org)->point.y, 1);
    adg_assert_isapprox(primitive.data[1].point.x, 4);
    adg_assert_isapprox(primitive.data[1].point.y, 5);
    adg_assert_isapprox(primitive.data[2].point.x, 6);
    adg_assert_isapprox(primitive.data[2].point.y, 7);

    /* Curve */
    cpml_primitive_next(&primitive);
    cpml_primitive_copy(&curve, &primitive);
    /* The offset algorithm for curves is an approximation, so
     * offsetting +1 and -1 does not return the original curve.
     * Keeping a backup around to restore the original data.
     */
    backup = cpml_primitive_deep_dup(&curve);
    /* Testing different algorithms */
    cpml_curve_offset_algorithm(CPML_CURVE_OFFSET_ALGORITHM_GEOMETRICAL);
    cpml_primitive_offset(&primitive, 1);
    adg_assert_isapprox((primitive.org)->point.x, 5.293);
    adg_assert_isapprox((primitive.org)->point.y, 7.707);
    adg_assert_isapprox(primitive.data[1].point.x, 7.889);
    adg_assert_isapprox(primitive.data[1].point.y, 8.515);
    adg_assert_isapprox(primitive.data[2].point.x, 11.196);
    adg_assert_isapprox(primitive.data[2].point.y, 9.007);
    adg_assert_isapprox(primitive.data[3].point.x, -1.4);
    adg_assert_isapprox(primitive.data[3].point.y, 1.2);
    cpml_primitive_copy_data(&primitive, backup);
    cpml_curve_offset_algorithm(CPML_CURVE_OFFSET_ALGORITHM_BAIOCA);
    cpml_primitive_offset(&primitive, 1);
    adg_assert_isapprox((primitive.org)->point.x, 5.293);
    adg_assert_isapprox((primitive.org)->point.y, 7.707);
    adg_assert_isapprox(primitive.data[1].point.x, 6.901);
    adg_assert_isapprox(primitive.data[1].point.y, 9.315);
    adg_assert_isapprox(primitive.data[2].point.x, 10.806);
    adg_assert_isapprox(primitive.data[2].point.y, 10.355);
    adg_assert_isapprox(primitive.data[3].point.x, -1.4);
    adg_assert_isapprox(primitive.data[3].point.y, 1.2);
    cpml_primitive_copy_data(&primitive, backup);
    cpml_curve_offset_algorithm(CPML_CURVE_OFFSET_ALGORITHM_HANDCRAFT);
    cpml_primitive_offset(&primitive, 1);
    adg_assert_isapprox((primitive.org)->point.x, 5.293);
    adg_assert_isapprox((primitive.org)->point.y, 7.707);
    adg_assert_isapprox(primitive.data[1].point.x, -5.758);
    adg_assert_isapprox(primitive.data[1].point.y, -3.344);
    adg_assert_isapprox(primitive.data[2].point.x, 24.987);
    adg_assert_isapprox(primitive.data[2].point.y, 20.99);
    adg_assert_isapprox(primitive.data[3].point.x, -1.4);
    adg_assert_isapprox(primitive.data[3].point.y, 1.2);
    cpml_primitive_copy_data(&primitive, backup);

    g_free(backup);
    cpml_curve_offset_algorithm(CPML_CURVE_OFFSET_ALGORITHM_DEFAULT);

    /* Close: this primitive does not own data points but should
     * modify the points of the previous and next primitives */
    cpml_primitive_next(&primitive);
    cpml_primitive_offset(&primitive, 1);
    adg_assert_isapprox((curve.org)->point.x, 6);
    adg_assert_isapprox((curve.org)->point.y, 7);
    adg_assert_isapprox(curve.data[3].point.x, -1.553);
    adg_assert_isapprox(curve.data[3].point.y, 2.894);
    adg_assert_isapprox((line.org)->point.x, 0.447);
    adg_assert_isapprox((line.org)->point.y, 1.894);
    adg_assert_isapprox(line.data[1].point.x, 3);
    adg_assert_isapprox(line.data[1].point.y, 1);
    cpml_primitive_offset(&primitive, -1);
    adg_assert_isapprox((curve.org)->point.x, 6);
    adg_assert_isapprox((curve.org)->point.y, 7);
    adg_assert_isapprox(curve.data[3].point.x, -2);
    adg_assert_isapprox(curve.data[3].point.y, 2);
    adg_assert_isapprox((line.org)->point.x, 0);
    adg_assert_isapprox((line.org)->point.y, 1);
    adg_assert_isapprox(line.data[1].point.x, 3);
    adg_assert_isapprox(line.data[1].point.y, 1);

    g_free(segment);
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
    adg_test_add_traps("/cpml/primitive/sanity/copy", _cpml_test_sanity_copy, 2);
    adg_test_add_traps("/cpml/primitive/sanity/copy-data", _cpml_test_sanity_copy_data, 2);
    adg_test_add_traps("/cpml/primitive/sanity/get-n-points", _cpml_test_sanity_get_n_points, 1);
    adg_test_add_traps("/cpml/primitive/sanity/get-length", _cpml_test_sanity_get_length, 1);
    adg_test_add_traps("/cpml/primitive/sanity/put-extents", _cpml_test_sanity_put_extents, 2);
    adg_test_add_traps("/cpml/primitive/sanity/put-pair-at", _cpml_test_sanity_put_pair_at, 2);
    adg_test_add_traps("/cpml/primitive/sanity/put-vector-at", _cpml_test_sanity_put_vector_at, 2);
    adg_test_add_traps("/cpml/primitive/sanity/get-closest-pos", _cpml_test_sanity_get_closest_pos, 2);
    adg_test_add_traps("/cpml/primitive/sanity/set-point", _cpml_test_sanity_set_point, 2);
    adg_test_add_traps("/cpml/primitive/sanity/put-point", _cpml_test_sanity_put_point, 2);
    adg_test_add_traps("/cpml/primitive/sanity/put-intersections", _cpml_test_sanity_put_intersections, 3);
    adg_test_add_traps("/cpml/primitive/sanity/put-intersections-with-segment", _cpml_test_sanity_put_intersections_with_segment, 3);
    adg_test_add_traps("/cpml/primitive/sanity/offset", _cpml_test_sanity_offset, 1);
    adg_test_add_traps("/cpml/primitive/sanity/join", _cpml_test_sanity_join, 2);
    adg_test_add_traps("/cpml/primitive/sanity/to-cairo", _cpml_test_sanity_to_cairo, 2);
    adg_test_add_traps("/cpml/primitive/sanity/dump", _cpml_test_sanity_dump, 1);

    g_test_add_func("/cpml/primitive/method/from-segment", _cpml_test_from_segment);
    g_test_add_func("/cpml/primitive/method/copy", _cpml_test_copy);
    g_test_add_func("/cpml/primitive/method/copy-data", _cpml_test_copy_data);
    g_test_add_func("/cpml/primitive/method/type-get-n-points", _cpml_test_type_get_n_points);
    g_test_add_func("/cpml/primitive/method/get-n-points", _cpml_test_get_n_points);
    g_test_add_func("/cpml/primitive/method/get-length", _cpml_test_get_length);
    g_test_add_func("/cpml/primitive/method/put-extents", _cpml_test_put_extents);
    g_test_add_func("/cpml/primitive/method/put-pair-at", _cpml_test_put_pair_at);
    g_test_add_func("/cpml/primitive/method/put-vector-at", _cpml_test_put_vector_at);
    g_test_add_func("/cpml/primitive/method/get-closest-pos", _cpml_test_get_closest_pos);
    g_test_add_func("/cpml/primitive/method/set-point", _cpml_test_set_point);
    g_test_add_func("/cpml/primitive/method/put-point", _cpml_test_put_point);
    g_test_add_func("/cpml/primitive/method/put-intersections", _cpml_test_put_intersections);
    g_test_add_func("/cpml/primitive/method/put-intersections-with-segment", _cpml_test_put_intersections_with_segment);
    g_test_add_func("/cpml/primitive/method/offset", _cpml_test_offset);
    g_test_add_func("/cpml/primitive/method/join", _cpml_test_join);
    g_test_add_func("/cpml/primitive/method/to-cairo", _cpml_test_to_cairo);
    adg_test_add_traps("/cpml/primitive/method/dump", _cpml_test_dump, 1);

    return g_test_run();
}
