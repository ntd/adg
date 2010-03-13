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
#include <adg/adg-widget.h>


static void
_adg_test_canvas(void)
{
    AdgWidget *widget;
    AdgCanvas *valid_canvas, *invalid_canvas, *canvas;

    widget = ADG_WIDGET(adg_widget_new());
    valid_canvas = adg_canvas_new();
    invalid_canvas = adg_test_invalid_pointer();

    g_object_ref(valid_canvas);

    /* Using the public APIs */
    adg_widget_set_canvas(widget, NULL);
    canvas = adg_widget_get_canvas(widget);
    g_assert(canvas == NULL);

    adg_widget_set_canvas(widget, valid_canvas);
    canvas = adg_widget_get_canvas(widget);
    g_assert(canvas == valid_canvas);

    adg_widget_set_canvas(widget, invalid_canvas);
    canvas = adg_widget_get_canvas(widget);
    g_assert(canvas == valid_canvas);

    adg_widget_set_canvas(widget, NULL);
    canvas = adg_widget_get_canvas(widget);
    g_assert(canvas == NULL);

    /* Using GObject property methods */
    g_object_set(widget, "canvas", NULL, NULL);
    g_object_get(widget, "canvas", &canvas, NULL);
    g_assert(canvas == NULL);

    g_object_set(widget, "canvas", valid_canvas, NULL);
    g_object_get(widget, "canvas", &canvas, NULL);
    g_assert(canvas == valid_canvas);
    g_object_unref(canvas);

    g_object_set(widget, "canvas", invalid_canvas, NULL);
    g_object_get(widget, "canvas", &canvas, NULL);
    g_assert(canvas == valid_canvas);
    g_object_unref(canvas);

    g_object_set(widget, "canvas", NULL, NULL);
    g_object_get(widget, "canvas", &canvas, NULL);
    g_assert(canvas == NULL);

    g_object_unref(widget);
    g_object_unref(valid_canvas);
}

static void
_adg_test_factor(void)
{
    AdgWidget *widget;
    gdouble valid_factor1, valid_factor2, invalid_factor, factor;

    widget = ADG_WIDGET(adg_widget_new());
    valid_factor1 = 10;
    valid_factor2 = 1;
    invalid_factor = 0.5;

    /* Using the public APIs */
    adg_widget_set_factor(widget, valid_factor1);
    factor = adg_widget_get_factor(widget);
    g_assert_cmpfloat(factor, ==, valid_factor1);

    adg_widget_set_factor(widget, invalid_factor);
    factor = adg_widget_get_factor(widget);
    g_assert_cmpfloat(factor, ==, valid_factor1);

    adg_widget_set_factor(widget, valid_factor2);
    factor = adg_widget_get_factor(widget);
    g_assert_cmpfloat(factor, ==, valid_factor2);

    /* Using GObject property methods */
    g_object_set(widget, "factor", valid_factor1, NULL);
    g_object_get(widget, "factor", &factor, NULL);
    g_assert_cmpfloat(factor, ==, valid_factor1);

    g_object_set(widget, "factor", invalid_factor, NULL);
    g_object_get(widget, "factor", &factor, NULL);
    g_assert_cmpfloat(factor, ==, valid_factor1);

    g_object_set(widget, "factor", valid_factor2, NULL);
    g_object_get(widget, "factor", &factor, NULL);
    g_assert_cmpfloat(factor, ==, valid_factor2);

    g_object_unref(widget);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/widget/canvas", _adg_test_canvas);
    adg_test_add_func("/adg/widget/factor", _adg_test_factor);

    return g_test_run();
}
