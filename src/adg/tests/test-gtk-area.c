/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2017  Nicola Fontana <ntd at entidi.it>
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


#include <config.h>
#include <adg-test.h>
#include <adg.h>


static AdgGtkArea *
_adg_gtk_area_new(void)
{
    AdgGtkArea *area;
    AdgCanvas *canvas;

    canvas = adg_test_canvas();
    area = ADG_GTK_AREA(adg_gtk_area_new_with_canvas(canvas));
    g_object_unref(canvas);

    return area;
}

static void
_adg_behavior_translation(void)
{
    AdgGtkArea *area;
    AdgCanvas *canvas;
    GdkEventMotion event_motion;
    GdkEventButton event_button;
    gboolean stop;
    const cairo_matrix_t *map;

    area = _adg_gtk_area_new();
    canvas = adg_gtk_area_get_canvas(area);

    /* Translating requires an arranged canvas */
    adg_entity_arrange(ADG_ENTITY(canvas));

    /* Translating (local space) */
    map = adg_entity_get_local_map(ADG_ENTITY(canvas));
    adg_assert_isapprox(map->x0, 0);
    adg_assert_isapprox(map->y0, 0);

    event_button.type = GDK_BUTTON_PRESS;
    event_button.button = 2;
    event_button.x = 10;
    event_button.y = 20;
    g_signal_emit_by_name(area, "button-press-event", &event_button, NULL, &stop);

    event_motion.type = GDK_MOTION_NOTIFY;
    event_motion.state = GDK_BUTTON2_MASK;
    event_motion.x = 100;
    event_motion.y = 200;
    g_signal_emit_by_name(area, "motion-notify-event", &event_motion, NULL, &stop);
    map = adg_entity_get_local_map(ADG_ENTITY(canvas));
    adg_assert_isapprox(map->x0, 90);
    adg_assert_isapprox(map->y0, 180);

    /* Translating (global space) */
    map = adg_gtk_area_get_render_map(area);
    adg_assert_isapprox(map->x0, 0);
    adg_assert_isapprox(map->y0, 0);

    event_button.x = 30;
    event_button.y = 40;
    g_signal_emit_by_name(area, "button-press-event", &event_button, NULL, &stop);

    event_motion.state |= GDK_SHIFT_MASK;
    event_motion.x = 300;
    event_motion.y = 400;
    g_signal_emit_by_name(area, "motion-notify-event", &event_motion, NULL, &stop);
    map = adg_gtk_area_get_render_map(area);
    adg_assert_isapprox(map->x0, 270);
    adg_assert_isapprox(map->y0, 360);

    gtk_widget_destroy(GTK_WIDGET(area));
}

static void
_adg_property_canvas(void)
{
    AdgGtkArea *area;
    AdgCanvas *valid_canvas, *invalid_canvas, *canvas;

    area = ADG_GTK_AREA(adg_gtk_area_new());
    valid_canvas = adg_canvas_new();
    invalid_canvas = adg_test_invalid_pointer();

    g_object_ref(valid_canvas);

    /* Using the public APIs */
    adg_test_signal(area, "canvas-changed");
    adg_gtk_area_set_canvas(area, NULL);
    g_assert_false(adg_test_signal_check(FALSE));
    canvas = adg_gtk_area_get_canvas(area);
    g_assert_null(canvas);

    adg_gtk_area_set_canvas(area, valid_canvas);
    g_assert_true(adg_test_signal_check(FALSE));
    canvas = adg_gtk_area_get_canvas(area);
    g_assert_true(canvas == valid_canvas);

    adg_gtk_area_set_canvas(area, invalid_canvas);
    g_assert_false(adg_test_signal_check(FALSE));
    canvas = adg_gtk_area_get_canvas(area);
    g_assert_true(canvas == valid_canvas);

    /* Setting the canvas to NULL is a valid operation
     * and means to unset the canvas */
    adg_gtk_area_set_canvas(area, NULL);
    g_assert_true(adg_test_signal_check(FALSE));

    /* Resetting the same value is a no-op */
    adg_gtk_area_set_canvas(area, NULL);
    g_assert_false(adg_test_signal_check(FALSE));
    canvas = adg_gtk_area_get_canvas(area);
    g_assert_null(canvas);

    /* Using GObject property methods */
    g_object_set(area, "canvas", NULL, NULL);
    g_assert_false(adg_test_signal_check(FALSE));
    g_object_get(area, "canvas", &canvas, NULL);
    g_assert_null(canvas);

    g_object_set(area, "canvas", valid_canvas, NULL);
    g_assert_true(adg_test_signal_check(FALSE));
    g_object_get(area, "canvas", &canvas, NULL);
    g_assert_true(canvas == valid_canvas);
    adg_entity_destroy(ADG_ENTITY(canvas));

    g_object_set(area, "canvas", invalid_canvas, NULL);
    g_assert_false(adg_test_signal_check(FALSE));
    g_object_get(area, "canvas", &canvas, NULL);
    g_assert_true(canvas == valid_canvas);
    adg_entity_destroy(ADG_ENTITY(canvas));

    g_object_set(area, "canvas", NULL, NULL);
    g_assert_true(adg_test_signal_check(TRUE));
    g_object_get(area, "canvas", &canvas, NULL);
    g_assert_null(canvas);

    adg_gtk_area_set_canvas(area, valid_canvas);
    gtk_widget_destroy(GTK_WIDGET(area));
    adg_entity_destroy(ADG_ENTITY(valid_canvas));
}

static void
_adg_property_factor(void)
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
    adg_assert_isapprox(factor, valid_factor1);

    adg_gtk_area_set_factor(area, invalid_factor);
    factor = adg_gtk_area_get_factor(area);
    adg_assert_isapprox(factor, valid_factor1);

    adg_gtk_area_set_factor(area, valid_factor2);
    factor = adg_gtk_area_get_factor(area);
    adg_assert_isapprox(factor, valid_factor2);

    /* Using GObject property methods */
    g_object_set(area, "factor", valid_factor1, NULL);
    g_object_get(area, "factor", &factor, NULL);
    adg_assert_isapprox(factor, valid_factor1);

    g_object_set(area, "factor", invalid_factor, NULL);
    g_object_get(area, "factor", &factor, NULL);
    adg_assert_isapprox(factor, valid_factor1);

    g_object_set(area, "factor", valid_factor2, NULL);
    g_object_get(area, "factor", &factor, NULL);
    adg_assert_isapprox(factor, valid_factor2);

    gtk_widget_destroy(GTK_WIDGET(area));
}

static void
_adg_property_autozoom(void)
{
    AdgGtkArea *area;
    gboolean invalid_boolean;
    gboolean has_autozoom;

    area = (AdgGtkArea *) adg_gtk_area_new();
    invalid_boolean = (gboolean) 1234;

    /* Using the public APIs */
    adg_gtk_area_switch_autozoom(area, FALSE);
    has_autozoom = adg_gtk_area_has_autozoom(area);
    g_assert_false(has_autozoom);

    adg_gtk_area_switch_autozoom(area, invalid_boolean);
    has_autozoom = adg_gtk_area_has_autozoom(area);
    g_assert_false(has_autozoom);

    adg_gtk_area_switch_autozoom(area, TRUE);
    has_autozoom = adg_gtk_area_has_autozoom(area);
    g_assert_true(has_autozoom);

    /* Using GObject property methods */
    g_object_set(area, "autozoom", invalid_boolean, NULL);
    g_object_get(area, "autozoom", &has_autozoom, NULL);
    g_assert_true(has_autozoom);

    g_object_set(area, "autozoom", FALSE, NULL);
    g_object_get(area, "autozoom", &has_autozoom, NULL);
    g_assert_false(has_autozoom);

    g_object_set(area, "autozoom", TRUE, NULL);
    g_object_get(area, "autozoom", &has_autozoom, NULL);
    g_assert_true(has_autozoom);

    gtk_widget_destroy(GTK_WIDGET(area));
}

static void
_adg_property_render_map(void)
{
    AdgGtkArea *area;
    const cairo_matrix_t *identity_map;
    cairo_matrix_t null_map, dummy_map;
    const cairo_matrix_t *render_map;
    cairo_matrix_t *render_map_dup;

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
    g_assert_true(adg_matrix_equal(render_map, &null_map));

    adg_gtk_area_transform_render_map(area, &dummy_map, ADG_TRANSFORM_AFTER);
    render_map = adg_gtk_area_get_render_map(area);
    g_assert_true(adg_matrix_equal(render_map, &null_map));

    adg_gtk_area_set_render_map(area, identity_map);
    render_map = adg_gtk_area_get_render_map(area);
    g_assert_true(adg_matrix_equal(render_map, identity_map));

    adg_gtk_area_set_render_map(area, NULL);
    render_map = adg_gtk_area_get_render_map(area);
    g_assert_true(adg_matrix_equal(render_map, identity_map));

    adg_gtk_area_transform_render_map(area, &dummy_map, ADG_TRANSFORM_BEFORE);
    render_map = adg_gtk_area_get_render_map(area);
    g_assert_true(adg_matrix_equal(render_map, &dummy_map));

    /* Using GObject property methods */
    g_object_set(area, "render-map", &null_map, NULL);
    g_object_get(area, "render-map", &render_map_dup, NULL);
    g_assert_true(adg_matrix_equal(render_map_dup, &null_map));
    g_free(render_map_dup);

    g_object_set(area, "render-map", NULL, NULL);
    g_object_get(area, "render-map", &render_map_dup, NULL);
    g_assert_true(adg_matrix_equal(render_map_dup, &null_map));
    g_free(render_map_dup);

    g_object_set(area, "render-map", identity_map, NULL);
    g_object_get(area, "render-map", &render_map_dup, NULL);
    g_assert_true(adg_matrix_equal(render_map_dup, identity_map));
    g_free(render_map_dup);

    gtk_widget_destroy(GTK_WIDGET(area));
}

static void
_adg_method_get_extents(void)
{
    AdgGtkArea *area;
    const CpmlExtents *extents;
    AdgCanvas *canvas;

    area = ADG_GTK_AREA(adg_gtk_area_new());
    canvas = adg_test_canvas();

    /* Sanity check */
    g_assert_null(adg_gtk_area_get_extents(NULL));

    /* With no canvas, the extents should be undefined */
    extents = adg_gtk_area_get_extents(area);
    g_assert_false(extents->is_defined);

    adg_gtk_area_set_canvas(area, canvas);
    g_object_unref(canvas);
    extents = adg_gtk_area_get_extents(area);
    g_assert_true(extents->is_defined);
    adg_assert_isapprox(extents->org.x, 0);
    adg_assert_isapprox(extents->org.y, 0);
    adg_assert_isapprox(extents->size.x, 1);
    adg_assert_isapprox(extents->size.y, 1);

    gtk_widget_destroy(GTK_WIDGET(area));
}

static void
_adg_method_get_zoom(void)
{
    AdgGtkArea *area;
    AdgCanvas *canvas;

    area = ADG_GTK_AREA(adg_gtk_area_new());
    canvas = adg_test_canvas();

    /* Sanity check */
    adg_assert_isapprox(adg_gtk_area_get_zoom(NULL), 0);

    adg_assert_isapprox(adg_gtk_area_get_zoom(area), 1);

    adg_gtk_area_set_canvas(area, canvas);
    g_object_unref(canvas);
    adg_assert_isapprox(adg_gtk_area_get_zoom(area), 1);

    gtk_widget_destroy(GTK_WIDGET(area));
}

static void
_adg_method_switch_autozoom(void)
{
    AdgGtkArea *area;
    GtkAllocation allocation;

    area = _adg_gtk_area_new();
    allocation.width = 100;
    allocation.height = 100;

    /* Allocation does not work if the widget is not visible: without a
     * top-level this will hopefully do not require an X server running */
    gtk_widget_show(GTK_WIDGET(area));

    /* Without autozoom the zoom factor is left to 1 */
    gtk_widget_size_allocate(GTK_WIDGET(area), &allocation);
    adg_assert_isapprox(adg_gtk_area_get_zoom(area), 1);

    /* The allocation phase is one-shot, so I must destroy and recreate
     * the AdgGtkArea widget every time to trigger the size allocation */
    gtk_widget_destroy(GTK_WIDGET(area));
    area = _adg_gtk_area_new();
    gtk_widget_show(GTK_WIDGET(area));

    /* With autozoom */
    adg_gtk_area_switch_autozoom(area, TRUE);
    gtk_widget_size_allocate(GTK_WIDGET(area), &allocation);
    adg_assert_isapprox(adg_gtk_area_get_zoom(area), 100);

    gtk_widget_destroy(GTK_WIDGET(area));
    area = _adg_gtk_area_new();
    gtk_widget_show(GTK_WIDGET(area));

    /* Trying different allocation size */
    adg_gtk_area_switch_autozoom(area, TRUE);
    allocation.width = 200;
    allocation.height = 200;
    gtk_widget_size_allocate(GTK_WIDGET(area), &allocation);
    adg_assert_isapprox(adg_gtk_area_get_zoom(area), 200);

    gtk_widget_destroy(GTK_WIDGET(area));
}

static void
_adg_method_reset(void)
{
    GtkWidget *window;
    AdgGtkArea *area;
    AdgCanvas *canvas;
    cairo_matrix_t map;

    /* Sanity check */
    adg_gtk_area_reset(NULL);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    area = ADG_GTK_AREA(adg_gtk_area_new());
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(area));

    canvas = adg_test_canvas();
    adg_gtk_area_set_canvas(area, canvas);
    g_object_unref(canvas);

    cairo_matrix_init_scale(&map, 2, 2);

    adg_gtk_area_reset(area);
    adg_gtk_area_reset(area);
    adg_assert_isapprox(adg_gtk_area_get_zoom(area), 1);

    adg_gtk_area_set_render_map(area, &map);
    adg_assert_isapprox(adg_gtk_area_get_zoom(area), 2);

    adg_gtk_area_reset(area);
    adg_assert_isapprox(adg_gtk_area_get_zoom(area), 1);

    gtk_widget_destroy(window);
}

static void
_adg_method_extents_changed(void)
{
    AdgGtkArea *area;
    CpmlExtents extents;

    area = ADG_GTK_AREA(adg_gtk_area_new());
    extents.is_defined = FALSE;

    /* Sanity check */
    adg_gtk_area_extents_changed(NULL, &extents);

    adg_test_signal(area, "extents-changed");
    adg_gtk_area_extents_changed(area, &extents);
    g_assert_true(adg_test_signal_check(FALSE));

    adg_gtk_area_extents_changed(area, NULL);
    g_assert_true(adg_test_signal_check(TRUE));

    gtk_widget_destroy(GTK_WIDGET(area));
}

static void
_adg_method_canvas_changed(void)
{
    AdgGtkArea *area;
    AdgCanvas *canvas;

    area = ADG_GTK_AREA(adg_gtk_area_new());
    canvas = adg_canvas_new();

    /* Sanity check */
    adg_gtk_area_canvas_changed(NULL, canvas);

    adg_test_signal(area, "canvas-changed");
    adg_gtk_area_canvas_changed(area, canvas);
    g_assert_true(adg_test_signal_check(FALSE));

    adg_gtk_area_canvas_changed(area, NULL);
    g_assert_true(adg_test_signal_check(TRUE));

    gtk_widget_destroy(GTK_WIDGET(area));
    adg_entity_destroy(ADG_ENTITY(canvas));
}

static void
_adg_method_scroll_event(void)
{
    AdgGtkArea *area;
    AdgCanvas *canvas;
    GdkEventScroll event;
    gboolean stop;
    const cairo_matrix_t *map;

    area = _adg_gtk_area_new();
    canvas = adg_gtk_area_get_canvas(area);

    /* Zooming requires an arranged canvas */
    adg_entity_arrange(ADG_ENTITY(canvas));

    map = adg_entity_get_local_map(ADG_ENTITY(canvas));
    adg_assert_isapprox(map->xx, 1);

    /* Zoom in (local space) */
    event.type = GDK_SCROLL;
    event.direction = GDK_SCROLL_UP;
    event.state = 0;
    g_signal_emit_by_name(area, "scroll-event", &event, NULL, &stop);
    map = adg_entity_get_local_map(ADG_ENTITY(canvas));
    g_assert_cmpfloat(map->xx, >, 1);

    /* Zoom out (local space) */
    event.direction = GDK_SCROLL_DOWN;
    g_signal_emit_by_name(area, "scroll-event", &event, NULL, &stop);
    map = adg_entity_get_local_map(ADG_ENTITY(canvas));
    adg_assert_isapprox(map->xx, 1);
    g_signal_emit_by_name(area, "scroll-event", &event, NULL, &stop);
    map = adg_entity_get_local_map(ADG_ENTITY(canvas));
    g_assert_cmpfloat(map->xx, <, 1);

    adg_assert_isapprox(adg_gtk_area_get_zoom(area), 1);

    /* Zoom out (global space) */
    event.state = GDK_SHIFT_MASK;
    g_signal_emit_by_name(area, "scroll-event", &event, NULL, &stop);
    g_assert_cmpfloat(adg_gtk_area_get_zoom(area), <, 1);

    /* Zoom in (global space) */
    event.direction = GDK_SCROLL_UP;
    g_signal_emit_by_name(area, "scroll-event", &event, NULL, &stop);
    adg_assert_isapprox(adg_gtk_area_get_zoom(area), 1);
    g_signal_emit_by_name(area, "scroll-event", &event, NULL, &stop);
    g_assert_cmpfloat(adg_gtk_area_get_zoom(area), >, 1);

    gtk_widget_destroy(GTK_WIDGET(area));
}

static void
_adg_method_motion_event(void)
{
    AdgGtkArea *area;
    AdgCanvas *canvas;
    GdkEventMotion event;
    gboolean stop;
    const cairo_matrix_t *map;

    area = _adg_gtk_area_new();
    canvas = adg_gtk_area_get_canvas(area);

    /* Checking motion requires an arranged canvas */
    adg_entity_arrange(ADG_ENTITY(canvas));

    map = adg_entity_get_local_map(ADG_ENTITY(canvas));
    adg_assert_isapprox(map->x0, 0);
    adg_assert_isapprox(map->y0, 0);

    /* Motion (local space) */
    event.type = GDK_MOTION_NOTIFY;
    event.state = GDK_BUTTON2_MASK;
    event.x = 10;
    event.y = 20;
    g_signal_emit_by_name(area, "motion-notify-event", &event, NULL, &stop);
    map = adg_entity_get_local_map(ADG_ENTITY(canvas));
    g_assert_cmpfloat(map->x0, >, 0);
    g_assert_cmpfloat(map->y0, >, 0);
    event.x = -event.x;
    event.y = -event.y;
    g_signal_emit_by_name(area, "motion-notify-event", &event, NULL, &stop);
    map = adg_entity_get_local_map(ADG_ENTITY(canvas));
    g_assert_cmpfloat(map->x0, <, 0);
    g_assert_cmpfloat(map->y0, <, 0);
    event.x = 0;
    event.y = 0;
    g_signal_emit_by_name(area, "motion-notify-event", &event, NULL, &stop);
    map = adg_entity_get_local_map(ADG_ENTITY(canvas));
    adg_assert_isapprox(map->x0, 0);
    adg_assert_isapprox(map->y0, 0);

    map = adg_gtk_area_get_render_map(area);
    adg_assert_isapprox(map->x0, 0);
    adg_assert_isapprox(map->y0, 0);

    /* Motion (global space) */
    event.state |= GDK_SHIFT_MASK;
    event.x = 30;
    event.y = 40;
    g_signal_emit_by_name(area, "motion-notify-event", &event, NULL, &stop);
    map = adg_gtk_area_get_render_map(area);
    g_assert_cmpfloat(map->x0, >, 0);
    g_assert_cmpfloat(map->y0, >, 0);
    event.x = -event.x;
    event.y = -event.y;
    g_signal_emit_by_name(area, "motion-notify-event", &event, NULL, &stop);
    map = adg_gtk_area_get_render_map(area);
    g_assert_cmpfloat(map->x0, <, 0);
    g_assert_cmpfloat(map->y0, <, 0);
    event.x = 0;
    event.y = 0;
    g_signal_emit_by_name(area, "motion-notify-event", &event, NULL, &stop);
    map = adg_gtk_area_get_render_map(area);
    adg_assert_isapprox(map->x0, 0);
    adg_assert_isapprox(map->y0, 0);

    gtk_widget_destroy(GTK_WIDGET(area));
}

#ifdef GTK2_ENABLED

static void
_adg_method_size_request(void)
{
    AdgGtkArea *area;
    AdgCanvas *canvas;
    GtkRequisition requisition;

    area = ADG_GTK_AREA(adg_gtk_area_new());
    canvas = adg_test_canvas();

    gtk_widget_size_request(GTK_WIDGET(area), &requisition);
    g_assert_cmpint(requisition.width, ==, 0);
    g_assert_cmpint(requisition.height, ==, 0);

    /* Size requests are cached so recreate the area widget */
    gtk_widget_destroy(GTK_WIDGET(area));

    area = ADG_GTK_AREA(adg_gtk_area_new());
    adg_gtk_area_set_canvas(area, canvas);
    g_object_unref(canvas);

    gtk_widget_size_request(GTK_WIDGET(area), &requisition);
    g_assert_cmpint(requisition.width, ==, 1);
    g_assert_cmpint(requisition.height, ==, 1);

    gtk_widget_destroy(GTK_WIDGET(area));
}

#endif


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);
    gtk_init_check(&argc, &argv);

    adg_test_add_object_checks("/adg-gtk/area/type/object", ADG_GTK_TYPE_AREA);

    g_test_add_func("/adg-gtk/area/behavior/translation", _adg_behavior_translation);

    g_test_add_func("/adg-gtk/area/property/canvas", _adg_property_canvas);
    g_test_add_func("/adg-gtk/area/property/factor", _adg_property_factor);
    g_test_add_func("/adg-gtk/area/property/autozoom", _adg_property_autozoom);
    g_test_add_func("/adg-gtk/area/property/render-map", _adg_property_render_map);

    g_test_add_func("/adg-gtk/area/method/get-extents", _adg_method_get_extents);
    g_test_add_func("/adg-gtk/area/method/get-zoom", _adg_method_get_zoom);
    g_test_add_func("/adg-gtk/area/method/switch-autozoom", _adg_method_switch_autozoom);
    g_test_add_func("/adg-gtk/area/method/reset", _adg_method_reset);
    g_test_add_func("/adg-gtk/area/method/extents-changed", _adg_method_extents_changed);
    g_test_add_func("/adg-gtk/area/method/canvas-changed", _adg_method_canvas_changed);
    g_test_add_func("/adg-gtk/area/method/scroll-event", _adg_method_scroll_event);
    g_test_add_func("/adg-gtk/area/method/motion-event", _adg_method_motion_event);
#ifdef GTK2_ENABLED
    g_test_add_func("/adg-gtk/area/method/size-request", _adg_method_size_request);
#endif

    return g_test_run();
}
