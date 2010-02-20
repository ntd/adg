/* ADG - Automatic Drawing Generation
 * Copyright (C) 2010  Nicola Fontana <ntd at entidi.it>
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


static void
test_source(void)
{
    AdgEdges *edges;
    AdgTrail *trail;
    AdgTrail *source;

    edges = adg_edges_new();
    trail = ADG_TRAIL(adg_path_new());

    g_object_set(edges, "source", NULL, NULL);
    source = adg_edges_get_source(edges);
    g_assert(source == NULL);

    adg_edges_set_source(edges, trail);
    source = adg_edges_get_source(edges);
    g_assert(source == trail);

    adg_edges_set_source(edges, NULL);
    source = adg_edges_get_source(edges);
    g_assert(source == NULL);

    g_object_unref(edges);
    g_object_unref(trail);
}


static void
test_critical_angle(void)
{
    AdgEdges *edges;
    gdouble critical_angle;
    gdouble valid_value, invalid_value;

    edges = adg_edges_new();
    valid_value = G_PI / 10;
    invalid_value = G_PI + 1;

    adg_edges_set_critical_angle(edges, valid_value);
    critical_angle = adg_edges_get_critical_angle(edges);
    g_assert_cmpfloat(critical_angle, ==, valid_value);

    adg_edges_set_critical_angle(edges, invalid_value);
    critical_angle = adg_edges_get_critical_angle(edges);
    g_assert_cmpfloat(critical_angle, !=, invalid_value);

    g_object_set(edges, "critical-angle", valid_value, NULL);
    critical_angle = adg_edges_get_critical_angle(edges);
    g_assert_cmpfloat(critical_angle, ==, valid_value);

    g_object_set(edges, "critical-angle", invalid_value, NULL);
    critical_angle = adg_edges_get_critical_angle(edges);
    g_assert_cmpfloat(critical_angle, !=, invalid_value);

    g_object_unref(edges);
}


int
main(int argc, char *argv[])
{
    test_init(&argc, &argv);

    g_test_add_func("/adg/edges/source", test_source);
    g_test_add_func("/adg/edges/critical-angle", test_critical_angle);

    return g_test_run();
}
