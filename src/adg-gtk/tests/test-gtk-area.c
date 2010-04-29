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


#include <test-internal.h>
#include <adg-gtk.h>


static void
_adg_test_canvas(void)
{
    AdgGtkArea *area;
    AdgCanvas *valid_canvas, *invalid_canvas, *canvas;

    area = ADG_GTK_AREA(adg_gtk_area_new());
    valid_canvas = adg_canvas_new();
    invalid_canvas = adg_test_invalid_pointer();

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


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/gtk/area/canvas", _adg_test_canvas);
    adg_test_add_func("/adg/gtk/area/factor", _adg_test_factor);

    return g_test_run();
}
