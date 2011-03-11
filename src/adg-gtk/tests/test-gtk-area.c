/* ADG - Automatic Drawing Generation
 * Copyright (C) 2010,2011  Nicola Fontana <ntd at entidi.it>
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
#include <adg-gtk.h>


static void
_adg_test_canvas(void)
{
    AdgGtkArea *area;
    AdgCanvas *valid_canvas, *invalid_canvas, *canvas;

    area = ADG_GTK_AREA(adg_gtk_area_new());
    valid_canvas = adg_canvas_new();
    invalid_canvas = adg_gtk_test_invalid_pointer();

    g_object_ref(valid_canvas);

    /* Using the public APIs */
    adg_gtk_area_set_canvas(area, NULL);
    canvas = adg_gtk_area_get_canvas(area);
    g_assert(canvas == NULL);

    adg_gtk_area_set_canvas(area, valid_canvas);
    canvas = adg_gtk_area_get_canvas(area);
    g_assert(canvas == valid_canvas);

    adg_gtk_area_set_canvas(area, invalid_canvas);
    canvas = adg_gtk_area_get_canvas(area);
    g_assert(canvas == valid_canvas);

    adg_gtk_area_set_canvas(area, NULL);
    canvas = adg_gtk_area_get_canvas(area);
    g_assert(canvas == NULL);

    /* Using GObject property methods */
    g_object_set(area, "canvas", NULL, NULL);
    g_object_get(area, "canvas", &canvas, NULL);
    g_assert(canvas == NULL);

    g_object_set(area, "canvas", valid_canvas, NULL);
    g_object_get(area, "canvas", &canvas, NULL);
    g_assert(canvas == valid_canvas);
    g_object_unref(canvas);

    g_object_set(area, "canvas", invalid_canvas, NULL);
    g_object_get(area, "canvas", &canvas, NULL);
    g_assert(canvas == valid_canvas);
    g_object_unref(canvas);

    g_object_set(area, "canvas", NULL, NULL);
    g_object_get(area, "canvas", &canvas, NULL);
    g_assert(canvas == NULL);

    g_object_unref(area);
    g_object_unref(valid_canvas);
}

static void
_adg_test_factor(void)
{
    AdgGtkArea *area;
    gdouble valid_factor1, valid_factor2, invalid_factor, factor;

    area = ADG_GTK_AREA(adg_gtk_area_new());
    valid_factor1 = 10;
    valid_factor2 = 1;
    invalid_factor = 0.5;

    /* Using the public APIs */
    adg_gtk_area_set_factor(area, valid_factor1);
    factor = adg_gtk_area_get_factor(area);
    g_assert_cmpfloat(factor, ==, valid_factor1);

    adg_gtk_area_set_factor(area, invalid_factor);
    factor = adg_gtk_area_get_factor(area);
    g_assert_cmpfloat(factor, ==, valid_factor1);

    adg_gtk_area_set_factor(area, valid_factor2);
    factor = adg_gtk_area_get_factor(area);
    g_assert_cmpfloat(factor, ==, valid_factor2);

    /* Using GObject property methods */
    g_object_set(area, "factor", valid_factor1, NULL);
    g_object_get(area, "factor", &factor, NULL);
    g_assert_cmpfloat(factor, ==, valid_factor1);

    g_object_set(area, "factor", invalid_factor, NULL);
    g_object_get(area, "factor", &factor, NULL);
    g_assert_cmpfloat(factor, ==, valid_factor1);

    g_object_set(area, "factor", valid_factor2, NULL);
    g_object_get(area, "factor", &factor, NULL);
    g_assert_cmpfloat(factor, ==, valid_factor2);

    g_object_unref(area);
}

static void
_adg_test_autozoom(void)
{
    AdgGtkArea *area;
    gboolean invalid_boolean;
    gboolean has_autozoom;

    area = (AdgGtkArea *) adg_gtk_area_new();
    invalid_boolean = (gboolean) 1234;

    /* Using the public APIs */
    adg_gtk_area_switch_autozoom(area, FALSE);
    has_autozoom = adg_gtk_area_has_autozoom(area);
    g_assert(!has_autozoom);

    adg_gtk_area_switch_autozoom(area, invalid_boolean);
    has_autozoom = adg_gtk_area_has_autozoom(area);
    g_assert(!has_autozoom);

    adg_gtk_area_switch_autozoom(area, TRUE);
    has_autozoom = adg_gtk_area_has_autozoom(area);
    g_assert(has_autozoom);

    /* Using GObject property methods */
    g_object_set(area, "autozoom", invalid_boolean, NULL);
    g_object_get(area, "autozoom", &has_autozoom, NULL);
    g_assert(has_autozoom);

    g_object_set(area, "autozoom", FALSE, NULL);
    g_object_get(area, "autozoom", &has_autozoom, NULL);
    g_assert(!has_autozoom);

    g_object_set(area, "autozoom", TRUE, NULL);
    g_object_get(area, "autozoom", &has_autozoom, NULL);
    g_assert(has_autozoom);

    g_object_unref(area);
}

static void
_adg_test_render_map(void)
{
    AdgGtkArea *area;
    const AdgMatrix *identity_map;
    AdgMatrix null_map, dummy_map;
    const AdgMatrix *render_map;
    AdgMatrix *render_map_dup;

    area = ADG_GTK_AREA(adg_gtk_area_new());
    identity_map = adg_matrix_identity();

    /* A null map is a kind of degenerated matrix: it must be
     * treated as valid value by the API */
    cairo_matrix_init(&null_map, 0, 0, 0, 0, 0, 0);
    /* A general purpose map value without translations */
    cairo_matrix_init(&dummy_map, 1, 2, 3, 4, 0, 0);

    /* Using the public APIs */
    adg_gtk_area_set_render_map(area, &null_map);
    render_map = adg_gtk_area_get_render_map(area);
    g_assert(adg_matrix_equal(render_map, &null_map));

    adg_gtk_area_transform_render_map(area, &dummy_map, ADG_TRANSFORM_AFTER);
    render_map = adg_gtk_area_get_render_map(area);
    g_assert(adg_matrix_equal(render_map, &null_map));

    adg_gtk_area_set_render_map(area, identity_map);
    render_map = adg_gtk_area_get_render_map(area);
    g_assert(adg_matrix_equal(render_map, identity_map));

    adg_gtk_area_set_render_map(area, NULL);
    render_map = adg_gtk_area_get_render_map(area);
    g_assert(adg_matrix_equal(render_map, identity_map));

    adg_gtk_area_transform_render_map(area, &dummy_map, ADG_TRANSFORM_BEFORE);
    render_map = adg_gtk_area_get_render_map(area);
    g_assert(adg_matrix_equal(render_map, &dummy_map));

    /* Using GObject property methods */
    g_object_set(area, "render-map", &null_map, NULL);
    g_object_get(area, "render-map", &render_map_dup, NULL);
    g_assert(adg_matrix_equal(render_map_dup, &null_map));
    g_free(render_map_dup);

    g_object_set(area, "render-map", NULL, NULL);
    g_object_get(area, "render-map", &render_map_dup, NULL);
    g_assert(adg_matrix_equal(render_map_dup, &null_map));
    g_free(render_map_dup);

    g_object_set(area, "render-map", identity_map, NULL);
    g_object_get(area, "render-map", &render_map_dup, NULL);
    g_assert(adg_matrix_equal(render_map_dup, identity_map));
    g_free(render_map_dup);

    g_object_unref(area);
}


int
main(int argc, char *argv[])
{
    adg_gtk_test_init(&argc, &argv);

    adg_gtk_test_add_func("/adg/gtk/area/canvas", _adg_test_canvas);
    adg_gtk_test_add_func("/adg/gtk/area/factor", _adg_test_factor);
    adg_gtk_test_add_func("/adg/gtk/area/autozoom", _adg_test_autozoom);
    adg_gtk_test_add_func("/adg/gtk/area/render-map", _adg_test_render_map);

    return g_test_run();
}
