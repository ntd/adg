/* ADG - Automatic Drawing Generation
 * Copyright (C) 2011  Nicola Fontana <ntd at entidi.it>
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


#include <test-internal.h>


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

CpmlPath path = {
    CAIRO_STATUS_SUCCESS,
    data,
    G_N_ELEMENTS(data)
};


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
_cpml_test_basic(void)
{
    cairo_bool_t found;
    CpmlSegment segment;
    CpmlPrimitive primitive, primitive_copy;
    size_t n_points;

    found = cpml_segment_from_cairo(&segment, &path);
    g_assert(found);

    /* Browsing API */
    cpml_primitive_from_segment(&primitive, &segment);
    g_assert(primitive.data != NULL);
    found = cpml_primitive_next(&primitive);
    g_assert(found);
    found = cpml_primitive_next(&primitive);
    g_assert(found);
    found = cpml_primitive_next(&primitive);
    g_assert(found);

    /* Checking primitive copy and corner case browsing */
    cpml_primitive_copy(&primitive_copy, &primitive);
    found = cpml_primitive_next(&primitive);
    g_assert(! found);
    cpml_primitive_reset(&primitive);
    g_assert(primitive.data != NULL);
    g_assert_cmpint((primitive.data)->header.type, ==, CPML_LINE);
    found = cpml_primitive_next(&primitive_copy);
    g_assert(! found);
    cpml_primitive_reset(&primitive_copy);
    g_assert(primitive_copy.data != NULL);
    g_assert_cmpint((primitive_copy.data)->header.type, ==, CPML_LINE);
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
_cpml_test_get_point(void)
{
    CpmlSegment segment;
    CpmlPrimitive primitive;
    const cairo_path_data_t *point;

    cpml_segment_from_cairo(&segment, &path);

    /* Line */
    cpml_primitive_from_segment(&primitive, &segment);
    point = cpml_primitive_get_point(&primitive, 0);
    g_assert(point != NULL);
    g_assert_cmpfloat(point->point.x, ==, 0);
    g_assert_cmpfloat(point->point.y, ==, 1);
    point = cpml_primitive_get_point(&primitive, 1);
    g_assert(point != NULL);
    g_assert_cmpfloat(point->point.x, ==, 2);
    g_assert_cmpfloat(point->point.y, ==, 3);
    point = cpml_primitive_get_point(&primitive, 2);
    g_assert(point == NULL);

    /* The negative indices are checked only against CPML_LINE */
    point = cpml_primitive_get_point(&primitive, -1);
    g_assert(point != NULL);
    g_assert_cmpfloat(point->point.x, ==, 2);
    g_assert_cmpfloat(point->point.y, ==, 3);
    point = cpml_primitive_get_point(&primitive, -2);
    g_assert(point != NULL);
    g_assert_cmpfloat(point->point.x, ==, 0);
    g_assert_cmpfloat(point->point.y, ==, 1);
    point = cpml_primitive_get_point(&primitive, -3);
    g_assert(point == NULL);

    /* Arc */
    cpml_primitive_next(&primitive);
    point = cpml_primitive_get_point(&primitive, 0);
    g_assert(point != NULL);
    g_assert_cmpfloat(point->point.x, ==, 2);
    g_assert_cmpfloat(point->point.y, ==, 3);
    point = cpml_primitive_get_point(&primitive, 1);
    g_assert(point != NULL);
    g_assert_cmpfloat(point->point.x, ==, 4);
    g_assert_cmpfloat(point->point.y, ==, 5);
    point = cpml_primitive_get_point(&primitive, 2);
    g_assert(point != NULL);
    g_assert_cmpfloat(point->point.x, ==, 6);
    g_assert_cmpfloat(point->point.y, ==, 7);
    point = cpml_primitive_get_point(&primitive, 3);
    g_assert(point == NULL);

    /* Curve */
    cpml_primitive_next(&primitive);
    point = cpml_primitive_get_point(&primitive, 0);
    g_assert(point != NULL);
    g_assert_cmpfloat(point->point.x, ==, 6);
    g_assert_cmpfloat(point->point.y, ==, 7);
    point = cpml_primitive_get_point(&primitive, 1);
    g_assert(point != NULL);
    g_assert_cmpfloat(point->point.x, ==, 8);
    g_assert_cmpfloat(point->point.y, ==, 9);
    point = cpml_primitive_get_point(&primitive, 2);
    g_assert(point != NULL);
    g_assert_cmpfloat(point->point.x, ==, 10);
    g_assert_cmpfloat(point->point.y, ==, 11);
    point = cpml_primitive_get_point(&primitive, 3);
    g_assert(point != NULL);
    g_assert_cmpfloat(point->point.x, ==, 12);
    g_assert_cmpfloat(point->point.y, ==, 13);
    point = cpml_primitive_get_point(&primitive, 4);
    g_assert(point == NULL);

    /* Close */
    cpml_primitive_next(&primitive);
    point = cpml_primitive_get_point(&primitive, 0);
    g_assert(point != NULL);
    g_assert_cmpfloat(point->point.x, ==, 12);
    g_assert_cmpfloat(point->point.y, ==, 13);
    point = cpml_primitive_get_point(&primitive, 1);
    g_assert(point != NULL);
    g_assert_cmpfloat(point->point.x, ==, 0);
    g_assert_cmpfloat(point->point.y, ==, 1);
    point = cpml_primitive_get_point(&primitive, 2);
    g_assert(point == NULL);
}


int
main(int argc, char *argv[])
{
    cpml_test_init(&argc, &argv);

    cpml_test_add_func("/cpml/primitive/type_get_n_points", _cpml_test_type_get_n_points);
    cpml_test_add_func("/cpml/primitive/basic", _cpml_test_basic);
    cpml_test_add_func("/cpml/primitive/get_n_points", _cpml_test_get_n_points);
    cpml_test_add_func("/cpml/primitive/get_point", _cpml_test_get_point);

    return g_test_run();
}
