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
#include <adg.h>


static void
_adg_behavior_misc(void)
{
    AdgPath *path;
    AdgEdges *edges;
    CpmlSegment segment;
    gint n;

    path = adg_path_new();
    adg_path_move_to_explicit(path, 0, 5);
    adg_path_line_to_explicit(path, 1, 6);
    adg_path_line_to_explicit(path, 2, 3);
    adg_path_line_to_explicit(path, 3, 1);
    adg_path_reflect(path, NULL);

    edges = adg_edges_new_with_source(ADG_TRAIL(path));

    /* Check repetition gives the same results */
    for (n = 0; n < 2; ++n) {
        g_assert_true(adg_trail_put_segment(ADG_TRAIL(edges), 1, &segment));
        g_assert_nonnull(segment.data);
        g_assert_cmpint(segment.num_data, ==, 4);
        g_assert_cmpint(segment.data[0].header.type, ==, CPML_MOVE);
        adg_assert_isapprox(segment.data[1].point.x, 1);
        adg_assert_isapprox(segment.data[1].point.y, 6);
        g_assert_cmpint(segment.data[2].header.type, ==, CPML_LINE);
        adg_assert_isapprox(segment.data[3].point.x, 1);
        adg_assert_isapprox(segment.data[3].point.y, -6);

        g_assert_true(cpml_segment_next(&segment));
        g_assert_nonnull(segment.data);
        g_assert_cmpint(segment.num_data, ==, 4);
        g_assert_cmpint(segment.data[0].header.type, ==, CPML_MOVE);
        adg_assert_isapprox(segment.data[1].point.x, 2);
        adg_assert_isapprox(segment.data[1].point.y, 3);
        g_assert_cmpint(segment.data[2].header.type, ==, CPML_LINE);
        adg_assert_isapprox(segment.data[3].point.x, 2);
        adg_assert_isapprox(segment.data[3].point.y, -3);

        g_assert_false(cpml_segment_next(&segment));
    }

    /* Check that destroying path unsets the source property */
    g_object_unref(path);
    g_assert_null(adg_edges_get_source(edges));

    g_object_unref(edges);
}

static void
_adg_property_source(void)
{
    AdgEdges *edges;
    AdgTrail *valid_trail, *invalid_trail;
    AdgTrail *source;

    edges = adg_edges_new();
    valid_trail = ADG_TRAIL(adg_path_new());
    invalid_trail = adg_test_invalid_pointer();

    /* Using the public APIs */
    adg_edges_set_source(edges, valid_trail);
    source = adg_edges_get_source(edges);
    g_assert_true(source == valid_trail);

    adg_edges_set_source(edges, invalid_trail);
    source = adg_edges_get_source(edges);
    g_assert_true(source == valid_trail);

    adg_edges_set_source(edges, NULL);
    source = adg_edges_get_source(edges);
    g_assert_null(source);

    /* Using GObject property methods */
    g_object_set(edges, "source", valid_trail, NULL);
    g_object_get(edges, "source", &source, NULL);
    g_assert_true(source == valid_trail);
    g_object_unref(source);

    g_object_set(edges, "source", invalid_trail, NULL);
    g_object_get(edges, "source", &source, NULL);
    g_assert_true(source == valid_trail);
    g_object_unref(source);

    g_object_set(edges, "source", NULL, NULL);
    g_object_get(edges, "source", &source, NULL);
    g_assert_null(source);

    g_object_unref(edges);
    g_object_unref(valid_trail);
}

static void
_adg_property_axis_angle(void)
{
    AdgEdges *edges;
    gdouble valid_value, invalid_value;
    gdouble axis_angle;

    edges = adg_edges_new();
    valid_value = G_PI / 10;
    invalid_value = G_PI + 1;

    /* Using the public APIs */
    adg_edges_set_axis_angle(edges, valid_value);
    axis_angle = adg_edges_get_axis_angle(edges);
    adg_assert_isapprox(axis_angle, valid_value);

    adg_edges_set_axis_angle(edges, invalid_value);
    axis_angle = adg_edges_get_axis_angle(edges);
    g_assert_cmpfloat(axis_angle, !=, invalid_value);

    /* Using GObject property methods */
    g_object_set(edges, "axis-angle", valid_value, NULL);
    g_object_get(edges, "axis-angle", &axis_angle, NULL);
    adg_assert_isapprox(axis_angle, valid_value);

    g_object_set(edges, "axis-angle", invalid_value, NULL);
    g_object_get(edges, "axis-angle", &axis_angle, NULL);
    g_assert_cmpfloat(axis_angle, !=, invalid_value);

    g_object_unref(edges);
}

static void
_adg_property_critical_angle(void)
{
    AdgEdges *edges;
    gdouble valid_value, invalid_value;
    gdouble critical_angle;

    edges = adg_edges_new();
    valid_value = G_PI / 10;
    invalid_value = G_PI + 1;

    /* Using the public APIs */
    adg_edges_set_critical_angle(edges, valid_value);
    critical_angle = adg_edges_get_critical_angle(edges);
    adg_assert_isapprox(critical_angle, valid_value);

    adg_edges_set_critical_angle(edges, invalid_value);
    critical_angle = adg_edges_get_critical_angle(edges);
    g_assert_cmpfloat(critical_angle, !=, invalid_value);

    /* Using GObject property methods */
    g_object_set(edges, "critical-angle", valid_value, NULL);
    g_object_get(edges, "critical-angle", &critical_angle, NULL);
    adg_assert_isapprox(critical_angle, valid_value);

    g_object_set(edges, "critical-angle", invalid_value, NULL);
    g_object_get(edges, "critical-angle", &critical_angle, NULL);
    g_assert_cmpfloat(critical_angle, !=, invalid_value);

    g_object_unref(edges);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_object_checks("/adg/edges/type/object", ADG_TYPE_EDGES);
    adg_test_add_model_checks("/adg/edges/type/model", ADG_TYPE_EDGES);

    g_test_add_func("/adg/edges/behavior/misc", _adg_behavior_misc);

    g_test_add_func("/adg/edges/property/source", _adg_property_source);
    g_test_add_func("/adg/edges/property/axis-angle", _adg_property_axis_angle);
    g_test_add_func("/adg/edges/property/critical-angle", _adg_property_critical_angle);

    return g_test_run();
}
