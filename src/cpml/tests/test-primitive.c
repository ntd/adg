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
_cpml_test_basic(void)
{
    cairo_bool_t found;
    CpmlSegment segment;
    CpmlPrimitive primitive, primitive_copy;

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


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/cpml/primitive/basic", _cpml_test_basic);
    adg_test_add_func("/cpml/primitive/type_get_n_points", _cpml_test_type_get_n_points);
    adg_test_add_func("/cpml/primitive/get_n_points", _cpml_test_get_n_points);
    adg_test_add_func("/cpml/primitive/set_point", _cpml_test_set_point);
    adg_test_add_func("/cpml/primitive/put_point", _cpml_test_put_point);

    return g_test_run();
}
