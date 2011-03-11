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
    { .point = { 0, 0 }},
    { .header = { CPML_LINE, 2 }},
    { .point = { 1, 1 }},
};

CpmlPath path = {
    CAIRO_STATUS_SUCCESS,
    data,
    G_N_ELEMENTS(data)
};


static void
_cpml_test_get_n_points(void)
{
    cairo_bool_t done;
    CpmlSegment segment;
    CpmlPrimitive primitive;
    size_t n_points;

    done = cpml_segment_from_cairo(&segment, &path);
    g_assert(done);

    cpml_primitive_from_segment(&primitive, &segment);
    n_points = cpml_primitive_get_n_points(&primitive);
    g_assert_cmpuint(n_points, ==, 2);
}


int
main(int argc, char *argv[])
{
    cpml_test_init(&argc, &argv);

    cpml_test_add_func("/cpml/primitive/get_n_points", _cpml_test_get_n_points);

    return g_test_run();
}
