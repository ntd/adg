/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2021  Nicola Fontana <ntd at entidi.it>
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


static cairo_path_data_t arc_data[] = {
    { .header = { CPML_MOVE, 2 }},
    { .point = { 0, 3 }},

    { .header = { CPML_ARC, 3 }},
    { .point = { 3, 0 }},
    { .point = { 0, -3 }}
};

CpmlPrimitive arc = {
    NULL,
    &arc_data[1],
    &arc_data[2]
};


static void
_cpml_sanity_info(gint i)
{
    switch (i) {
    case 1:
        cpml_arc_info(NULL, NULL, NULL, NULL, NULL);
        break;
    default:
        g_test_trap_assert_failed();
        break;
    }
}

static void
_cpml_sanity_to_cairo(gint i)
{
    cairo_t *cr;

    cr = adg_test_cairo_context();

    switch (i) {
    case 1:
        cpml_arc_to_cairo(NULL, cr);
        break;
    case 2:
        cpml_arc_to_cairo(&arc, NULL);
        break;
    default:
        g_test_trap_assert_failed();
        break;
    }

    cairo_destroy(cr);
}

static void
_cpml_sanity_to_curves(int i)
{
    CpmlSegment segment;

    switch (i) {
    case 1:
        cpml_arc_to_curves(NULL, &segment, 1);
        break;
    case 2:
        cpml_arc_to_curves(&arc, NULL, 1);
        break;
    case 3:
        /* XXX: this generates a division by 0 but on Travis, for some reason,
         * that is not enough to crash the process:
         * https://travis-ci.org/ntd/adg/builds/189592982
         *
         * Disabling for now.
         */
        cpml_arc_to_curves(&arc, &segment, 0);
        break;
    default:
        g_test_trap_assert_failed();
        break;
    }
}

static void
_cpml_method_info(void)
{
    CpmlPair center;
    double r, start, end;

    /* Passing NULL to the output arguments is valid */
    g_assert_true(cpml_arc_info(&arc, NULL, NULL, NULL, NULL));

    g_assert_true(cpml_arc_info(&arc, &center, &r, &start, &end));

    adg_assert_isapprox(center.x, 0);
    adg_assert_isapprox(center.y, 0);
    adg_assert_isapprox(r, 3);
    adg_assert_isapprox(start, M_PI_2);
    adg_assert_isapprox(end, -M_PI_2);
}

static void
_cpml_method_to_curves(void)
{
    cairo_path_data_t data[4*2];
    CpmlSegment segment = { NULL, data, 0 };

    /* Approximate with a single curve */
    cpml_arc_to_curves(&arc, &segment, 1);

    g_assert_cmpint(data[0].header.type, ==, CPML_CURVE);
    adg_assert_isapprox(data[1].point.x, 4);
    adg_assert_isapprox(data[1].point.y, 3);
    adg_assert_isapprox(data[2].point.x, 4);
    adg_assert_isapprox(data[2].point.y, -3);
    adg_assert_isapprox(data[3].point.x, 0);
    adg_assert_isapprox(data[3].point.y, -3);

    /* Approximate with two curves */
    cpml_arc_to_curves(&arc, &segment, 2);

    g_assert_cmpint(data[0].header.type, ==, CPML_CURVE);
    adg_assert_isapprox(data[1].point.x, 1.65685425);
    adg_assert_isapprox(data[1].point.y, 3);
    adg_assert_isapprox(data[2].point.x, 3);
    adg_assert_isapprox(data[2].point.y, 1.65685425);
    adg_assert_isapprox(data[3].point.x, 3);
    adg_assert_isapprox(data[3].point.y, 0);

    g_assert_cmpint(data[4].header.type, ==, CPML_CURVE);
    adg_assert_isapprox(data[5].point.x, 3);
    adg_assert_isapprox(data[5].point.y, -1.65685425);
    adg_assert_isapprox(data[6].point.x, 1.65685425);
    adg_assert_isapprox(data[6].point.y, -3);
    adg_assert_isapprox(data[7].point.x, 0);
    adg_assert_isapprox(data[7].point.y, -3);
}



int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_traps("/cpml/arc/sanity/info", _cpml_sanity_info, 1);
    adg_test_add_traps("/cpml/arc/sanity/to-cairo", _cpml_sanity_to_cairo, 2);
    adg_test_add_traps("/cpml/arc/sanity/to-curves", _cpml_sanity_to_curves, 2);

    g_test_add_func("/cpml/arc/method/info", _cpml_method_info);
    g_test_add_func("/cpml/arc/method/to-curves", _cpml_method_to_curves);

    return g_test_run();
}
