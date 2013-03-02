/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011,2012,2013  Nicola Fontana <ntd at entidi.it>
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


#include "test-internal.h"


static cairo_path_data_t data[] = {
    /* Useless heading CPML_MOVE */
    { .header = { CPML_MOVE, 2 }},
    { .point = { 0, 0 }},

    /* First segment: a couple of lines */
    { .header = { CPML_MOVE, 2 }},
    { .point = { 0, 1 }},
    { .header = { CPML_LINE, 2 }},
    { .point = { 2, 3 }},
    { .header = { CPML_LINE, 2 }},
    { .point = { 4, 5 }},

    /* Another useless CPML_MOVE with useless embedded data */
    { .header = { CPML_MOVE, 3 }},
    { .point = { 0, 0 }},
    { .point = { 0, 0 }},

    /* Second segment: a Bézier curve with a trailing CPML_CLOSE */
    { .header = { CPML_MOVE, 2 }},
    { .point = { 6, 7 }},
    { .header = { CPML_CURVE, 4 }},
    { .point = { 8, 9 }},
    { .point = { 10, 11 }},
    { .point = { 12, 13 }},
    { .header = { CPML_CLOSE, 1 }},

    /* A valid cairo segment considered invalid by CPML
     * because does not have a heading CPML_MOVE */
    { .header = { CPML_LINE, 2 }},
    { .point = { 0, 0 }},
    { .header = { CPML_CLOSE, 1 }},

    /* Another valid cairo segment invalid in CPML */
    { .header = { CPML_CLOSE, 1 }},

    /* Third segment: a couple of arcs */
    { .header = { CPML_MOVE, 2 }},
    { .point = { 14, 15 }},
    { .header = { CPML_ARC, 3 }},
    { .point = { 16, 17 }},
    { .point = { 18, 19 }},
    { .header = { CPML_ARC, 3 }},
    { .point = { 20, 21 }},
    { .point = { 22, 23 }},

    /* Forth segment: a floating CPML_CLOSE */
    { .header = { CPML_MOVE, 2 }},
    { .point = { 24, 25 }},
    { .header = { CPML_CLOSE, 1 }}
};

static cairo_path_data_t noop_data[] = {
    /* Useless heading CPML_MOVE */
    { .header = { CPML_MOVE, 2 }},
    { .point = { 0, 1 }},
    { .header = { CPML_MOVE, 4 }},
    { .point = { 2, 3 }},
    { .point = { 4, 5 }},
    { .point = { 6, 7 }},
    { .header = { CPML_MOVE, 2 }},
    { .point = { 8, 9 }}
};

CpmlPath path = {
    CAIRO_STATUS_SUCCESS,
    data,
    G_N_ELEMENTS(data)
};

CpmlPath noop_path = {
    CAIRO_STATUS_SUCCESS,
    noop_data,
    G_N_ELEMENTS(noop_data)
};


static void
_cpml_test_basic(void)
{
    cairo_bool_t found, empty;
    CpmlSegment segment, segment_copy;

    /* Checking APIs */
    empty = cpml_path_is_empty(NULL);
    g_assert(empty);
    empty = cpml_path_is_empty(&noop_path);
    g_assert(! empty);
    found = cpml_segment_from_cairo(&segment, &noop_path);
    g_assert(! found);
    empty = cpml_path_is_empty(&path);
    g_assert(! empty);

    /* CPML entry point */
    found = cpml_segment_from_cairo(&segment, &path);
    g_assert(found);

    /* First segment */
    g_assert_cmpint(segment.data[0].header.type, ==, CPML_MOVE);
    g_assert_cmpint(segment.data[2].header.type, ==, CPML_LINE);

    /* Second segment */
    found = cpml_segment_next(&segment);
    g_assert(found);
    g_assert_cmpint(segment.data[0].header.type, ==, CPML_MOVE);
    g_assert_cmpint(segment.data[2].header.type, ==, CPML_CURVE);

    /* Third segment */
    found = cpml_segment_next(&segment);
    g_assert(found);
    g_assert_cmpint(segment.data[0].header.type, ==, CPML_MOVE);
    g_assert_cmpint(segment.data[2].header.type, ==, CPML_ARC);

    /* Forth segment */
    found = cpml_segment_next(&segment);
    g_assert(found);
    g_assert_cmpint(segment.data[0].header.type, ==, CPML_MOVE);
    g_assert_cmpint(segment.data[2].header.type, ==, CPML_CLOSE);

    /* Copy and boundaries check */
    cpml_segment_copy(&segment_copy, &segment);
    found = cpml_segment_next(&segment);
    g_assert(! found);
    found = cpml_segment_next(&segment_copy);
    g_assert(! found);
    cpml_segment_reset(&segment_copy);
    g_assert_cmpint(segment_copy.data[0].header.type, ==, CPML_MOVE);
    g_assert_cmpint(segment_copy.data[2].header.type, ==, CPML_LINE);
}


int
main(int argc, char *argv[])
{
    cpml_test_init(&argc, &argv);

    cpml_test_add_func("/cpml/segment/basic", _cpml_test_basic);

    return g_test_run();
}
