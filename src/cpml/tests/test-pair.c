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
#include <math.h>


static CpmlPair org = { .x = 0, .y = 0 };
static CpmlPair nord = { .x = 0, .y = 1 };
static CpmlPair diag = { .x = -3, .y = -4 };
static CpmlPair diag3 = { .x = -9, .y = -12 };
static CpmlPair junk = { .x = -12345.54321, .y = 9876543210.123456789 };


static void
_cpml_test_pair_misc(void)
{
    cairo_bool_t equals;
    cairo_path_data_t cairo_pair;
    CpmlPair pair;

    equals = cpml_pair_equal(&org, &junk);
    g_assert_false(equals);

    equals = cpml_pair_equal(&org, NULL);
    g_assert_false(equals);

    equals = cpml_pair_equal(NULL, NULL);
    g_assert_true(equals);

    /* Just check the following calls will not crash */
    cpml_pair_copy(&pair, NULL);
    cpml_pair_copy(&pair, &pair);

    cpml_pair_copy(&pair, &org);
    equals = cpml_pair_equal(&pair, &nord);
    g_assert_false(equals);

    cpml_pair_to_cairo(&nord, &cairo_pair);
    cpml_pair_from_cairo(&pair, &cairo_pair);
    equals = cpml_pair_equal(&pair, &nord);
    g_assert_true(equals);

    cpml_pair_copy(&pair, NULL);
    equals = cpml_pair_equal(&pair, &nord);
    g_assert_true(equals);

    cpml_pair_copy(&pair, &diag);
    equals = cpml_pair_equal(&pair, &nord);
    g_assert_false(equals);

    cpml_pair_to_cairo(&org, &cairo_pair);
    cpml_pair_from_cairo(&pair, &cairo_pair);
    equals = cpml_pair_equal(&pair, &org);
    g_assert_true(equals);
}

static void
_cpml_test_pair_transform(void)
{
    CpmlPair pair;
    cairo_matrix_t matrix;

    g_test_message("Checking for translation transformations...");
    cairo_matrix_init_translate(&matrix, junk.x, junk.y);
    cpml_pair_copy(&pair, &org);
    cpml_pair_transform(&pair, &matrix);
    g_assert_cmpfloat(pair.x, ==, junk.x);
    g_assert_cmpfloat(pair.y, ==, junk.y);

    g_test_message("Checking for scaling transformations...");
    cairo_matrix_init_scale(&matrix, 3, 3);
    cpml_pair_copy(&pair, &diag);
    cpml_pair_transform(&pair, &matrix);
    g_assert_cmpfloat(pair.x, ==, diag3.x);
    g_assert_cmpfloat(pair.y, ==, diag3.y);

    g_test_message("Checking for assorted transformations...");
    cairo_matrix_init_scale(&matrix, 3, 3);
    cairo_matrix_translate(&matrix, diag.x, diag.y);
    cairo_matrix_translate(&matrix, -junk.x, -junk.y);
    cpml_pair_copy(&pair, &junk);
    cpml_pair_transform(&pair, &matrix);
    g_assert_cmpfloat(pair.x, ==, diag3.x);
    g_assert_cmpfloat(pair.y, ==, diag3.y);
}

static void
_cpml_test_pair_distance(void)
{
    double distance, squared_distance;

    distance = cpml_pair_distance(NULL, NULL);
    g_assert_cmpfloat(distance, ==, 0);

    distance = cpml_pair_distance(&org, NULL);
    g_assert_cmpfloat(distance, ==, 0);

    distance = cpml_pair_distance(NULL, &org);
    g_assert_cmpfloat(distance, ==, 0);

    distance = cpml_pair_distance(&nord, &org);
    g_assert_cmpfloat(distance, ==, 1);

    squared_distance = cpml_pair_squared_distance(NULL, NULL);
    g_assert_cmpfloat(squared_distance, ==, 0);

    squared_distance = cpml_pair_squared_distance(&org, NULL);
    g_assert_cmpfloat(squared_distance, ==, 0);

    squared_distance = cpml_pair_squared_distance(NULL, &org);
    g_assert_cmpfloat(squared_distance, ==, 0);

    squared_distance = cpml_pair_squared_distance(&nord, &org);
    g_assert_cmpfloat(squared_distance, ==, 1);
}

static void
_cpml_test_vector_angle(void)
{
    double angle, angle2;
    CpmlVector vector;

    angle = cpml_vector_angle(&nord);
    g_assert_cmpfloat(angle, ==, M_PI_2);

    angle = M_PI_2;
    cpml_vector_from_angle(&vector, angle);
    g_assert_cmpfloat(vector.x, ==, nord.x);
    g_assert_cmpfloat(vector.y, ==, nord.y);

    angle = cpml_vector_angle(&diag);
    angle2 = cpml_vector_angle(&diag3);
    g_assert_cmpfloat(angle, ==, angle2);

    angle = 1.234567;
    cpml_vector_from_angle(&vector, angle);
    angle2 = cpml_vector_angle(&vector);
    g_assert_cmpfloat(angle, ==, angle2);

    g_test_message("By convention, the vector (0,0) is considered a 0° angle");
    angle = cpml_vector_angle(&org);
    g_assert_cmpfloat(angle, ==, 0);

    cpml_pair_copy(&vector, &org);
    cpml_vector_normal(&vector);
    g_assert_cmpfloat(vector.x, ==, org.x);
    g_assert_cmpfloat(vector.y, ==, org.y);

    g_test_message("Checking cpml_vector_normal() API...");
    vector.x = 1;
    vector.y = 1;
    cpml_vector_normal(&vector);
    g_assert_cmpfloat(vector.x, ==, -1);
    g_assert_cmpfloat(vector.y, ==, 1);

    cpml_vector_normal(&vector);
    g_assert_cmpfloat(vector.x, ==, -1);
    g_assert_cmpfloat(vector.y, ==, -1);

    cpml_vector_normal(&vector);
    g_assert_cmpfloat(vector.x, ==, 1);
    g_assert_cmpfloat(vector.y, ==, -1);

    cpml_vector_normal(&vector);
    g_assert_cmpfloat(vector.x, ==, 1);
    g_assert_cmpfloat(vector.y, ==, 1);
}

static void
_cpml_test_vector_length(void)
{
    CpmlVector vector;
    double length;

    cpml_pair_copy(&vector, &junk);
    cpml_vector_set_length(&vector, 0);
    g_assert_cmpfloat(vector.x, ==, org.x);
    g_assert_cmpfloat(vector.y, ==, org.y);

    cpml_vector_set_length(&vector, 1234);
    g_assert_cmpfloat(vector.x, ==, org.x);
    g_assert_cmpfloat(vector.y, ==, org.y);

    cpml_pair_copy(&vector, &diag3);
    cpml_vector_set_length(&vector, 5);
    g_assert_cmpfloat(vector.x, ==, diag.x);
    g_assert_cmpfloat(vector.y, ==, diag.y);

    g_test_message("Using integer comparison to overcome rounding errors");

    cpml_vector_set_length(&vector, 10);
    length = cpml_pair_distance(&vector, NULL);
    g_assert_cmpint(round(length), ==, 10);

    cpml_vector_set_length(&vector, 5);
    g_assert_cmpint(round(vector.x), ==, diag.x);
    g_assert_cmpint(round(vector.y), ==, diag.y);
}

static void
_cpml_test_vector_transform(void)
{
    CpmlVector vector;
    cairo_matrix_t matrix;

    g_test_message("Vectors are not affected by translations");
    cairo_matrix_init_translate(&matrix, junk.x, junk.y);
    cpml_pair_copy(&vector, &org);
    cpml_vector_transform(&vector, &matrix);
    g_assert_cmpfloat(vector.x, ==, org.x);
    g_assert_cmpfloat(vector.y, ==, org.y);

    g_test_message("Checking scaling transformations...");
    cairo_matrix_init_scale(&matrix, 3, 3);
    cpml_pair_copy(&vector, &diag);
    cpml_vector_transform(&vector, &matrix);
    g_assert_cmpfloat(vector.x, ==, diag3.x);
    g_assert_cmpfloat(vector.y, ==, diag3.y);

    g_test_message("Checking assorted transformations...");
    cairo_matrix_init_scale(&matrix, 3, 3);
    cairo_matrix_translate(&matrix, diag.x, diag.y);
    cairo_matrix_translate(&matrix, -junk.x, -junk.y);
    cpml_pair_copy(&vector, &diag);
    cpml_vector_transform(&vector, &matrix);
    g_assert_cmpfloat(vector.x, ==, diag3.x);
    g_assert_cmpfloat(vector.y, ==, diag3.y);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    g_test_add_func("/cpml/pair/behavior/misc", _cpml_test_pair_misc);

    g_test_add_func("/cpml/pair/method/transform", _cpml_test_pair_transform);
    g_test_add_func("/cpml/pair/method/distance", _cpml_test_pair_distance);
    g_test_add_func("/cpml/vector/method/angle", _cpml_test_vector_angle);
    g_test_add_func("/cpml/vector/method/length", _cpml_test_vector_length);
    g_test_add_func("/cpml/vector/method/transform", _cpml_test_vector_transform);

    return g_test_run();
}
