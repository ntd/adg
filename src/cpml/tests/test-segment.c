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


static void
_cpml_test_browsing(void)
{
    CpmlSegment segment;
    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());

    /* First segment */
    g_assert_cmpint(segment.data[0].header.type, ==, CPML_MOVE);
    g_assert_cmpint(segment.data[2].header.type, ==, CPML_LINE);

    cpml_segment_reset(&segment);
    cpml_segment_reset(&segment);
    g_assert_cmpint(segment.data[0].header.type, ==, CPML_MOVE);
    g_assert_cmpint(segment.data[2].header.type, ==, CPML_LINE);

    /* Second segment */
    g_assert_cmpint(cpml_segment_next(&segment), ==, 1);
    g_assert_cmpint(segment.data[0].header.type, ==, CPML_MOVE);
    g_assert_cmpint(segment.data[2].header.type, ==, CPML_LINE);
    g_assert_cmpint(segment.data[4].header.type, ==, CPML_LINE);

    /* Third segment */
    g_assert_cmpint(cpml_segment_next(&segment), ==, 1);
    g_assert_cmpint(segment.data[0].header.type, ==, CPML_MOVE);
    g_assert_cmpint(segment.data[2].header.type, ==, CPML_CURVE);

    /* Forth segment */
    g_assert_cmpint(cpml_segment_next(&segment), ==, 1);
    g_assert_cmpint(segment.data[0].header.type, ==, CPML_MOVE);
    g_assert_cmpint(segment.data[2].header.type, ==, CPML_ARC);

    /* Fifth segment */
    g_assert_cmpint(cpml_segment_next(&segment), ==, 1);
    g_assert_cmpint(segment.data[0].header.type, ==, CPML_MOVE);
    g_assert_cmpint(segment.data[2].header.type, ==, CPML_CLOSE);

    g_assert_cmpint(cpml_segment_next(&segment), ==, 0);

    cpml_segment_reset(&segment);
    g_assert_cmpint(segment.data[0].header.type, ==, CPML_MOVE);
    g_assert_cmpint(segment.data[2].header.type, ==, CPML_LINE);
}

static void
_cpml_test_sanity_from_cairo(gint i)
{
    cairo_path_t *path = (cairo_path_t *) adg_test_path();
    CpmlSegment segment;

    switch (i) {
    case 1:
        cpml_segment_from_cairo(NULL, path);
        break;
    case 2:
        cpml_segment_from_cairo(&segment, NULL);
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
        cpml_segment_get_length(NULL);
        break;
    default:
        g_test_trap_assert_failed();
        break;
    }
}

static void
_cpml_test_sanity_put_intersections(gint i)
{
    CpmlSegment segment1, segment2;
    CpmlPair pair;

    /* Segment 1 and segment 2 intersects at least in (1,1) */
    cpml_segment_from_cairo(&segment1, (cairo_path_t *) adg_test_path());
    cpml_segment_copy(&segment2, &segment1);
    cpml_segment_next(&segment2);

    switch (i) {
    case 1:
        cpml_segment_put_intersections(NULL, &segment2, 2, &pair);
        break;
    case 2:
        cpml_segment_put_intersections(&segment1, NULL, 2, &pair);
        break;
    case 3:
        cpml_segment_put_intersections(&segment1, &segment2, 2, NULL);
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
        cpml_segment_offset(NULL, 1);
        break;
    default:
        g_test_trap_assert_failed();
        break;
    }
}

static void
_cpml_test_sanity_transform(gint i)
{
    CpmlSegment segment;
    cairo_matrix_t matrix;

    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());
    cairo_matrix_init_identity(&matrix);

    switch (i) {
    case 1:
        cpml_segment_transform(NULL, &matrix);
        break;
    case 2:
        cpml_segment_transform(&segment, NULL);
        break;
    default:
        g_test_trap_assert_failed();
        break;
    }
}

static void
_cpml_test_sanity_reverse(gint i)
{
    switch (i) {
    case 1:
        cpml_segment_reverse(NULL);
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

    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());

    switch (i) {
    case 1:
        cpml_segment_to_cairo(NULL, adg_test_cairo_context());
        break;
    case 2:
        cpml_segment_to_cairo(&segment, NULL);
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
_cpml_test_from_cairo(void)
{
    CpmlSegment segment;
    cairo_path_data_t noop_data[] = {
        /* Useless heading primitives */
        { .header = { CPML_MOVE, 2 }},
        { .point = { 0, 1 }},
        { .header = { CPML_MOVE, 4 }},
        { .point = { 2, 3 }},
        { .point = { 4, 5 }},
        { .point = { 6, 7 }},
        { .header = { CPML_MOVE, 2 }},
        { .point = { 8, 9 }}
    };
    cairo_path_t noop_path = {
        CAIRO_STATUS_SUCCESS,
        noop_data,
        G_N_ELEMENTS(noop_data)
    };
    cairo_path_t empty_path = {
        CAIRO_STATUS_SUCCESS,
        noop_data,
        0
    };

    g_assert_cmpint(cpml_segment_from_cairo(&segment, &noop_path), ==, 0);
    g_assert_cmpint(cpml_segment_from_cairo(&segment, &empty_path), ==, 0);
    g_assert_cmpint(cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path()), ==, 1);
}

static void
_cpml_test_get_length(void)
{
    CpmlSegment segment;

    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());

    /* First segment: not a round number so avoiding == */
    g_assert_cmpfloat(cpml_segment_get_length(&segment), >, 0);

    cpml_segment_next(&segment);

    /* Second segment */
    g_assert_cmpfloat(cpml_segment_get_length(&segment), ==, 3);
    g_assert_cmpfloat(cpml_segment_get_length(&segment), ==, 3);

    cpml_segment_next(&segment);

    /* Third segment */
    /* TODO: Bézier curve length not yet implemented
     * g_assert_cmpfloat(cpml_segment_get_length(&segment), >, 0); */

    cpml_segment_next(&segment);

    /* Forth segment */
    g_assert_cmpfloat(cpml_segment_get_length(&segment), ==, 0);

    cpml_segment_next(&segment);

    /* Fifth segment */
    g_assert_cmpfloat(cpml_segment_get_length(&segment), ==, 0);
}

static void
_cpml_test_put_intersections(void)
{
    CpmlSegment segment1, segment2;
    CpmlPair pair[10];

    cpml_segment_from_cairo(&segment1, (cairo_path_t *) adg_test_path());
    cpml_segment_copy(&segment2, &segment1);

    cpml_segment_next(&segment2);

    /* The first segment intersects the second segment in (1, 1).
     * TODO: avoid extrapolated intersections! Check
     * cpml_primitive_put_intersections */
    g_assert_cmpuint(cpml_segment_put_intersections(&segment1, &segment2, 10, pair), ==, 3);
    g_assert_cmpfloat(pair[0].x, ==, 1);
    g_assert_cmpfloat(pair[0].y, ==, 1);
    g_assert_cmpfloat(pair[1].x, ==, 2);
    g_assert_cmpfloat(pair[1].y, ==, 0);

    cpml_segment_next(&segment2);

    /* The third segment intersects the first segment 5 times.
     * TODO: check if this is true */
    g_assert_cmpuint(cpml_segment_put_intersections(&segment1, &segment2, 10, pair), ==, 5);
}

static void
_cpml_test_offset(void)
{
    CpmlSegment original, *segment;

    /* Work on a copy to avoid modifying adg_test_path() data */
    cpml_segment_from_cairo(&original, (cairo_path_t *) adg_test_path());
    segment = cpml_segment_deep_dup(&original);

    /* TODO: provide tests for arcs and curves */
    cpml_segment_offset(segment, 1);
    g_free(segment);

    cpml_segment_next(&original);
    segment = cpml_segment_deep_dup(&original);
    cpml_segment_offset(segment, 1);

    g_assert_cmpint(segment->data[0].header.type, ==, CPML_MOVE);
    g_assert_cmpfloat(segment->data[1].point.x, ==, 0);
    g_assert_cmpfloat(segment->data[1].point.y, ==, 1);

    g_assert_cmpint(segment->data[2].header.type, ==, CPML_LINE);
    g_assert_cmpfloat(segment->data[3].point.x, ==, 0);
    g_assert_cmpfloat(segment->data[3].point.y, ==, 1);

    g_assert_cmpint(segment->data[4].header.type, ==, CPML_LINE);
    g_assert_cmpfloat(segment->data[5].point.x, ==, 0);
    g_assert_cmpfloat(segment->data[5].point.y, ==, 2);

    g_free(segment);
}

static void
_cpml_test_transform(void)
{
    CpmlSegment original, *segment;
    cairo_matrix_t matrix;

    /* Work on a copy to avoid modifying adg_test_path() data */
    cpml_segment_from_cairo(&original, (cairo_path_t *) adg_test_path());
    segment = cpml_segment_deep_dup(&original);

    cairo_matrix_init_translate(&matrix, 1, 2);
    cpml_segment_transform(segment, &matrix);

    g_assert_cmpint(segment->data[0].header.type, ==, CPML_MOVE);
    g_assert_cmpfloat(segment->data[1].point.x, ==, 1);
    g_assert_cmpfloat(segment->data[1].point.y, ==, 3);

    g_assert_cmpint(segment->data[2].header.type, ==, CPML_LINE);
    g_assert_cmpfloat(segment->data[3].point.x, ==, 4);
    g_assert_cmpfloat(segment->data[3].point.y, ==, 3);

    g_assert_cmpint(segment->data[4].header.type, ==, CPML_ARC);
    g_assert_cmpfloat(segment->data[5].point.x, ==, 5);
    g_assert_cmpfloat(segment->data[5].point.y, ==, 7);
    g_assert_cmpfloat(segment->data[6].point.x, ==, 7);
    g_assert_cmpfloat(segment->data[6].point.y, ==, 9);

    g_assert_cmpint(segment->data[7].header.type, ==, CPML_CURVE);
    g_assert_cmpfloat(segment->data[8].point.x, ==, 9);
    g_assert_cmpfloat(segment->data[8].point.y, ==, 11);
    g_assert_cmpfloat(segment->data[9].point.x, ==, 11);
    g_assert_cmpfloat(segment->data[9].point.y, ==, 13);
    g_assert_cmpfloat(segment->data[10].point.x, ==, -1);
    g_assert_cmpfloat(segment->data[10].point.y, ==, 4);

    g_assert_cmpint(segment->data[11].header.type, ==, CPML_CLOSE);

    g_free(segment);
}

#include <stdio.h>
static void
_cpml_test_reverse(void)
{
    CpmlSegment original, *segment;

    /* Work on a copy to avoid modifying adg_test_path() data */
    cpml_segment_from_cairo(&original, (cairo_path_t *) adg_test_path());
    segment = cpml_segment_deep_dup(&original);

    /* First segment */
    cpml_segment_reverse(segment);

    g_assert_cmpint(segment->num_data, ==, 12);

    g_assert_cmpint(segment->data[0].header.type, ==, CPML_MOVE);
    g_assert_cmpfloat(segment->data[1].point.x, ==, -2);
    g_assert_cmpfloat(segment->data[1].point.y, ==, 2);

    g_assert_cmpint(segment->data[2].header.type, ==, CPML_CURVE);
    g_assert_cmpfloat(segment->data[3].point.x, ==, 10);
    g_assert_cmpfloat(segment->data[3].point.y, ==, 11);
    g_assert_cmpfloat(segment->data[4].point.x, ==, 8);
    g_assert_cmpfloat(segment->data[4].point.y, ==, 9);
    g_assert_cmpfloat(segment->data[5].point.x, ==, 6);
    g_assert_cmpfloat(segment->data[5].point.y, ==, 7);

    g_assert_cmpint(segment->data[6].header.type, ==, CPML_ARC);
    g_assert_cmpfloat(segment->data[7].point.x, ==, 4);
    g_assert_cmpfloat(segment->data[7].point.y, ==, 5);
    g_assert_cmpfloat(segment->data[8].point.x, ==, 3);
    g_assert_cmpfloat(segment->data[8].point.y, ==, 1);

    g_assert_cmpint(segment->data[9].header.type, ==, CPML_LINE);
    g_assert_cmpfloat(segment->data[10].point.x, ==, 0);
    g_assert_cmpfloat(segment->data[10].point.y, ==, 1);

    g_assert_cmpint(segment->data[11].header.type, ==, CPML_CLOSE);

    g_free(segment);
}

static void
_cpml_test_to_cairo(void)
{
    cairo_t *cr;
    CpmlSegment segment;
    int length, last_length;

    cr = adg_test_cairo_context();
    cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());

    g_assert_cmpint(adg_test_cairo_num_data(cr), ==, 0);

    length = 0;
    do {
        last_length = length;
        cpml_segment_to_cairo(&segment, cr);
        length = adg_test_cairo_num_data(cr);
        g_assert_cmpint(length, >, last_length);
    } while (cpml_segment_next(&segment));

    cairo_destroy(cr);
}

static void
_cpml_test_dump(gint i)
{
    CpmlSegment segment;

    switch (i) {
    case 1:
        cpml_segment_from_cairo(&segment, (cairo_path_t *) adg_test_path());
        cpml_segment_dump(&segment);
        break;
    default:
        g_test_trap_assert_passed();
        g_test_trap_assert_stderr_unmatched("?");
        g_test_trap_assert_stdout("*move*line*arc*curve*close*");
        break;
    }
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    g_test_add_func("/cpml/segment/behavior/browsing", _cpml_test_browsing);

    adg_test_add_traps("/cpml/segment/sanity/from-cairo", _cpml_test_sanity_from_cairo, 2);
    adg_test_add_traps("/cpml/segment/sanity/get-length", _cpml_test_sanity_get_length, 1);
    adg_test_add_traps("/cpml/segment/sanity/put-intersections", _cpml_test_sanity_put_intersections, 3);
    adg_test_add_traps("/cpml/segment/sanity/offset", _cpml_test_sanity_offset, 1);
    adg_test_add_traps("/cpml/segment/sanity/transform", _cpml_test_sanity_transform, 2);
    adg_test_add_traps("/cpml/segment/sanity/reverse", _cpml_test_sanity_reverse, 1);
    adg_test_add_traps("/cpml/segment/sanity/to-cairo", _cpml_test_sanity_to_cairo, 2);
    adg_test_add_traps("/cpml/segment/sanity/dump", _cpml_test_sanity_dump, 1);

    g_test_add_func("/cpml/segment/method/from-cairo", _cpml_test_from_cairo);
    g_test_add_func("/cpml/segment/method/get-length", _cpml_test_get_length);
    g_test_add_func("/cpml/segment/method/put-intersections", _cpml_test_put_intersections);
    g_test_add_func("/cpml/segment/method/offset", _cpml_test_offset);
    g_test_add_func("/cpml/segment/method/transform", _cpml_test_transform);
    g_test_add_func("/cpml/segment/method/reverse", _cpml_test_reverse);
    g_test_add_func("/cpml/segment/method/to-cairo", _cpml_test_to_cairo);
    adg_test_add_traps("/cpml/segment/method/dump", _cpml_test_dump, 1);

    return g_test_run();
}
