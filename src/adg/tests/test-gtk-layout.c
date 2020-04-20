/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2020  Nicola Fontana <ntd at entidi.it>
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


static AdgGtkLayout *
_adg_gtk_layout_new(void)
{
    AdgGtkLayout *layout;
    AdgCanvas *canvas;

    canvas = adg_test_canvas();
    layout = ADG_GTK_LAYOUT(adg_gtk_layout_new_with_canvas(canvas));
    g_object_unref(canvas);

    return layout;
}

static void
_adg_property_hadjustment(void)
{
    AdgGtkLayout *layout;
    GtkAdjustment *valid_adjustment, *invalid_adjustment, *hadjustment;

    layout = ADG_GTK_LAYOUT(adg_gtk_layout_new());
    valid_adjustment = (GtkAdjustment *) gtk_adjustment_new(0, 0, 0, 0, 0, 0);
    invalid_adjustment = adg_test_invalid_pointer();

    g_object_ref(valid_adjustment);

    /* Using the public APIs */
    g_test_message("The implementation must provide a fallback adjustment");
    hadjustment = adg_gtk_layout_get_hadjustment(layout);
    g_assert_true(GTK_IS_ADJUSTMENT(hadjustment));

    adg_gtk_layout_set_hadjustment(layout, NULL);
    hadjustment = adg_gtk_layout_get_hadjustment(layout);
    g_assert_true(GTK_IS_ADJUSTMENT(hadjustment));

    adg_gtk_layout_set_hadjustment(layout, valid_adjustment);
    hadjustment = adg_gtk_layout_get_hadjustment(layout);
    g_assert_true(hadjustment == valid_adjustment);

    adg_gtk_layout_set_hadjustment(layout, invalid_adjustment);
    hadjustment = adg_gtk_layout_get_hadjustment(layout);
    g_assert_true(hadjustment == valid_adjustment);

    adg_gtk_layout_set_hadjustment(layout, NULL);
    hadjustment = adg_gtk_layout_get_hadjustment(layout);
    g_assert_true(hadjustment != valid_adjustment);
    g_assert_true(GTK_IS_ADJUSTMENT(hadjustment));

    /* Using GObject property methods */
    g_object_set(layout, "hadjustment", NULL, NULL);
    g_object_get(layout, "hadjustment", &hadjustment, NULL);
    g_assert_true(GTK_IS_ADJUSTMENT(hadjustment));
    g_object_unref(hadjustment);

    g_object_set(layout, "hadjustment", valid_adjustment, NULL);
    g_object_get(layout, "hadjustment", &hadjustment, NULL);
    g_assert_true(hadjustment == valid_adjustment);
    g_object_unref(hadjustment);

    g_object_set(layout, "hadjustment", invalid_adjustment, NULL);
    g_object_get(layout, "hadjustment", &hadjustment, NULL);
    g_assert_true(hadjustment == valid_adjustment);
    g_object_unref(hadjustment);

    g_object_set(layout, "hadjustment", NULL, NULL);
    g_object_get(layout, "hadjustment", &hadjustment, NULL);
    g_assert_true(hadjustment != valid_adjustment);
    g_assert_true(GTK_IS_ADJUSTMENT(hadjustment));
    g_object_unref(hadjustment);

    gtk_widget_destroy(GTK_WIDGET(layout));
    g_object_unref(valid_adjustment);
}

static void
_adg_property_vadjustment(void)
{
    AdgGtkLayout *layout;
    GtkAdjustment *valid_adjustment, *invalid_adjustment, *vadjustment;

    layout = ADG_GTK_LAYOUT(adg_gtk_layout_new());
    valid_adjustment = (GtkAdjustment *) gtk_adjustment_new(0, 0, 0, 0, 0, 0);
    invalid_adjustment = adg_test_invalid_pointer();

    g_object_ref(valid_adjustment);

    /* Using the public APIs */
    g_test_message("The implementation must provide a fallback adjustment");
    vadjustment = adg_gtk_layout_get_vadjustment(layout);
    g_assert_true(GTK_IS_ADJUSTMENT(vadjustment));

    adg_gtk_layout_set_vadjustment(layout, NULL);
    vadjustment = adg_gtk_layout_get_vadjustment(layout);
    g_assert_true(GTK_IS_ADJUSTMENT(vadjustment));

    adg_gtk_layout_set_vadjustment(layout, valid_adjustment);
    vadjustment = adg_gtk_layout_get_vadjustment(layout);
    g_assert_true(vadjustment == valid_adjustment);

    adg_gtk_layout_set_vadjustment(layout, invalid_adjustment);
    vadjustment = adg_gtk_layout_get_vadjustment(layout);
    g_assert_true(vadjustment == valid_adjustment);

    adg_gtk_layout_set_vadjustment(layout, NULL);
    vadjustment = adg_gtk_layout_get_vadjustment(layout);
    g_assert_true(vadjustment != valid_adjustment);
    g_assert_true(GTK_IS_ADJUSTMENT(vadjustment));

    /* Using GObject property methods */
    g_object_set(layout, "vadjustment", NULL, NULL);
    g_object_get(layout, "vadjustment", &vadjustment, NULL);
    g_assert_true(GTK_IS_ADJUSTMENT(vadjustment));
    g_object_unref(vadjustment);

    g_object_set(layout, "vadjustment", valid_adjustment, NULL);
    g_object_get(layout, "vadjustment", &vadjustment, NULL);
    g_assert_true(vadjustment == valid_adjustment);
    g_object_unref(vadjustment);

    g_object_set(layout, "vadjustment", invalid_adjustment, NULL);
    g_object_get(layout, "vadjustment", &vadjustment, NULL);
    g_assert_true(vadjustment == valid_adjustment);
    g_object_unref(vadjustment);

    g_object_set(layout, "vadjustment", NULL, NULL);
    g_object_get(layout, "vadjustment", &vadjustment, NULL);
    g_assert_true(vadjustment != valid_adjustment);
    g_assert_true(GTK_IS_ADJUSTMENT(vadjustment));
    g_object_unref(vadjustment);

    gtk_widget_destroy(GTK_WIDGET(layout));
    g_object_unref(valid_adjustment);
}

#ifdef GTK3_ENABLED

static void
_adg_property_hscroll_policy(void)
{
    AdgGtkLayout *layout;
    GtkScrollablePolicy policy;

    layout = ADG_GTK_LAYOUT(adg_gtk_layout_new());

    /* Using the public APIs */
    g_assert_cmpint(gtk_scrollable_get_hscroll_policy(GTK_SCROLLABLE(layout)), ==, GTK_SCROLL_MINIMUM);

    gtk_scrollable_set_hscroll_policy(GTK_SCROLLABLE(layout), GTK_SCROLL_NATURAL);
    g_assert_cmpint(gtk_scrollable_get_hscroll_policy(GTK_SCROLLABLE(layout)), ==, GTK_SCROLL_NATURAL);

    g_object_set(layout, "hscroll-policy", GTK_SCROLL_MINIMUM, NULL);
    g_object_get(layout, "hscroll-policy", &policy, NULL);
    g_assert_cmpint(policy, ==, GTK_SCROLL_MINIMUM);

    gtk_widget_destroy(GTK_WIDGET(layout));
}

static void
_adg_property_vscroll_policy(void)
{
    AdgGtkLayout *layout;
    GtkScrollablePolicy policy;

    layout = ADG_GTK_LAYOUT(adg_gtk_layout_new());

    /* Using the public APIs */
    g_assert_cmpint(gtk_scrollable_get_vscroll_policy(GTK_SCROLLABLE(layout)), ==, GTK_SCROLL_MINIMUM);

    gtk_scrollable_set_vscroll_policy(GTK_SCROLLABLE(layout), GTK_SCROLL_NATURAL);
    g_assert_cmpint(gtk_scrollable_get_vscroll_policy(GTK_SCROLLABLE(layout)), ==, GTK_SCROLL_NATURAL);

    g_object_set(layout, "vscroll-policy", GTK_SCROLL_MINIMUM, NULL);
    g_object_get(layout, "vscroll-policy", &policy, NULL);
    g_assert_cmpint(policy, ==, GTK_SCROLL_MINIMUM);

    gtk_widget_destroy(GTK_WIDGET(layout));
}

#endif

static void
_adg_method_size_allocate(void)
{
    AdgGtkLayout *layout;
    AdgCanvas *canvas;
    cairo_matrix_t scale_x200;
    GtkAllocation allocation;
    GtkAdjustment *adjustment;

    layout = ADG_GTK_LAYOUT(adg_gtk_layout_new());
    cairo_matrix_init_scale(&scale_x200, 200, 200);

    /* Allocation does not work if the widget is not visible: without a
     * top-level this will hopefully do not require an X server running */
    gtk_widget_show(GTK_WIDGET(layout));

    adjustment = adg_gtk_layout_get_hadjustment(layout);
    g_assert_nonnull(adjustment);
    adg_assert_isapprox(gtk_adjustment_get_lower(adjustment), 0);
    adg_assert_isapprox(gtk_adjustment_get_upper(adjustment), 0);
    adg_assert_isapprox(gtk_adjustment_get_page_size(adjustment), 0);
    adg_assert_isapprox(gtk_adjustment_get_value(adjustment), 0);

    adjustment = adg_gtk_layout_get_vadjustment(layout);
    g_assert_nonnull(adjustment);
    adg_assert_isapprox(gtk_adjustment_get_lower(adjustment), 0);
    adg_assert_isapprox(gtk_adjustment_get_upper(adjustment), 0);
    adg_assert_isapprox(gtk_adjustment_get_page_size(adjustment), 0);
    adg_assert_isapprox(gtk_adjustment_get_value(adjustment), 0);

    allocation.width = 100;
    allocation.height = 100;
    gtk_widget_size_allocate(GTK_WIDGET(layout), &allocation);

    adjustment = adg_gtk_layout_get_hadjustment(layout);
    g_assert_nonnull(adjustment);
    adg_assert_isapprox(gtk_adjustment_get_lower(adjustment), 0);
    adg_assert_isapprox(gtk_adjustment_get_upper(adjustment), 0);
    adg_assert_isapprox(gtk_adjustment_get_page_size(adjustment), 0);
    adg_assert_isapprox(gtk_adjustment_get_value(adjustment), 0);

    adjustment = adg_gtk_layout_get_vadjustment(layout);
    g_assert_nonnull(adjustment);
    adg_assert_isapprox(gtk_adjustment_get_lower(adjustment), 0);
    adg_assert_isapprox(gtk_adjustment_get_upper(adjustment), 0);
    adg_assert_isapprox(gtk_adjustment_get_page_size(adjustment), 0);
    adg_assert_isapprox(gtk_adjustment_get_value(adjustment), 0);

    /* The allocation phase is one-shot, so I must destroy and recreate
     * the AdgGtkLayout widget every time to trigger the size allocation */
    gtk_widget_destroy(GTK_WIDGET(layout));
    layout = _adg_gtk_layout_new();
    canvas = adg_gtk_area_get_canvas(ADG_GTK_AREA(layout));
    gtk_widget_show(GTK_WIDGET(layout));

    adg_entity_transform_global_map(ADG_ENTITY(canvas),
                                    &scale_x200,
                                    ADG_TRANSFORM_AFTER);

    adjustment = adg_gtk_layout_get_hadjustment(layout);
    g_assert_nonnull(adjustment);
    adg_assert_isapprox(gtk_adjustment_get_lower(adjustment), 0);
    adg_assert_isapprox(gtk_adjustment_get_upper(adjustment), 0);
    adg_assert_isapprox(gtk_adjustment_get_page_size(adjustment), 0);
    adg_assert_isapprox(gtk_adjustment_get_value(adjustment), 0);

    adjustment = adg_gtk_layout_get_vadjustment(layout);
    g_assert_nonnull(adjustment);
    adg_assert_isapprox(gtk_adjustment_get_lower(adjustment), 0);
    adg_assert_isapprox(gtk_adjustment_get_upper(adjustment), 0);
    adg_assert_isapprox(gtk_adjustment_get_page_size(adjustment), 0);
    adg_assert_isapprox(gtk_adjustment_get_value(adjustment), 0);

    allocation.width = 100;
    allocation.height = 100;
    gtk_widget_size_allocate(GTK_WIDGET(layout), &allocation);

    adjustment = adg_gtk_layout_get_hadjustment(layout);
    g_assert_nonnull(adjustment);
    adg_assert_isapprox(gtk_adjustment_get_lower(adjustment), -50);
    adg_assert_isapprox(gtk_adjustment_get_upper(adjustment), 150);
    adg_assert_isapprox(gtk_adjustment_get_page_size(adjustment), 100);
    adg_assert_isapprox(gtk_adjustment_get_value(adjustment), 0);

    adjustment = adg_gtk_layout_get_vadjustment(layout);
    g_assert_nonnull(adjustment);
    adg_assert_isapprox(gtk_adjustment_get_lower(adjustment), -50);
    adg_assert_isapprox(gtk_adjustment_get_upper(adjustment), 150);
    adg_assert_isapprox(gtk_adjustment_get_page_size(adjustment), 100);
    adg_assert_isapprox(gtk_adjustment_get_value(adjustment), 0);

    allocation.width = 20;
    allocation.height = 30;
    gtk_widget_size_allocate(GTK_WIDGET(layout), &allocation);

    adjustment = adg_gtk_layout_get_hadjustment(layout);
    g_assert_nonnull(adjustment);
    adg_assert_isapprox(gtk_adjustment_get_lower(adjustment), -50);
    adg_assert_isapprox(gtk_adjustment_get_upper(adjustment), 150);
    adg_assert_isapprox(gtk_adjustment_get_page_size(adjustment), 20);
    adg_assert_isapprox(gtk_adjustment_get_value(adjustment), 0);

    adjustment = adg_gtk_layout_get_vadjustment(layout);
    g_assert_nonnull(adjustment);
    adg_assert_isapprox(gtk_adjustment_get_lower(adjustment), -50);
    adg_assert_isapprox(gtk_adjustment_get_upper(adjustment), 150);
    adg_assert_isapprox(gtk_adjustment_get_page_size(adjustment), 30);
    adg_assert_isapprox(gtk_adjustment_get_value(adjustment), 0);

    adg_gtk_area_canvas_changed(ADG_GTK_AREA(layout), NULL);

    gtk_widget_destroy(GTK_WIDGET(layout));
}

static void
_adg_method_value_changed(void)
{
    AdgGtkLayout *layout;
    AdgCanvas *canvas;
    cairo_matrix_t scale_x200;
    GtkAllocation allocation;
    GtkAdjustment *adjustment;

    layout = _adg_gtk_layout_new();
    canvas = adg_gtk_area_get_canvas(ADG_GTK_AREA(layout));
    cairo_matrix_init_scale(&scale_x200, 200, 200);

    adg_entity_transform_global_map(ADG_ENTITY(canvas),
                                    &scale_x200,
                                    ADG_TRANSFORM_AFTER);

    /* Allocation does not work if the widget is not visible: without a
     * top-level this will hopefully do not require an X server running */
    gtk_widget_show(GTK_WIDGET(layout));

    allocation.width = 100;
    allocation.height = 100;
    gtk_widget_size_allocate(GTK_WIDGET(layout), &allocation);

    adjustment = adg_gtk_layout_get_hadjustment(layout);
    gtk_adjustment_set_value(adjustment, 100);

    adjustment = adg_gtk_layout_get_vadjustment(layout);
    gtk_adjustment_set_value(adjustment, 40);

    adjustment = adg_gtk_layout_get_hadjustment(layout);
    g_assert_nonnull(adjustment);
    adg_assert_isapprox(gtk_adjustment_get_lower(adjustment), -50);
    adg_assert_isapprox(gtk_adjustment_get_upper(adjustment), 150);
    adg_assert_isapprox(gtk_adjustment_get_page_size(adjustment), 100);
#ifdef GTK2_ENABLED
    adg_assert_isapprox(gtk_adjustment_get_value(adjustment), 100);
#endif
#ifdef GTK3_ENABLED
    adg_assert_isapprox(gtk_adjustment_get_value(adjustment), 50);
#endif

    adjustment = adg_gtk_layout_get_vadjustment(layout);
    g_assert_nonnull(adjustment);
    adg_assert_isapprox(gtk_adjustment_get_lower(adjustment), -50);
    adg_assert_isapprox(gtk_adjustment_get_upper(adjustment), 150);
    adg_assert_isapprox(gtk_adjustment_get_page_size(adjustment), 100);
    adg_assert_isapprox(gtk_adjustment_get_value(adjustment), 40);

    gtk_widget_destroy(GTK_WIDGET(layout));
}

static void
_adg_method_set_parent(void)
{
    AdgGtkLayout *layout;
    GtkScrolledWindow *scrolled_window;
    GtkPolicyType hpolicy, vpolicy;
    gint width, height;

    layout = _adg_gtk_layout_new();
    scrolled_window = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));

    gtk_scrolled_window_set_policy(scrolled_window,
                                   GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);
    gtk_scrolled_window_get_policy(scrolled_window, &hpolicy, &vpolicy);
    g_assert_cmpint(hpolicy, ==, GTK_POLICY_ALWAYS);
    g_assert_cmpint(vpolicy, ==, GTK_POLICY_ALWAYS);

    gtk_widget_get_size_request(GTK_WIDGET(scrolled_window), &width, &height);
    g_assert_cmpint(width, ==, -1);
    g_assert_cmpint(height, ==, -1);

    /* Adding layout inside scrolled_window triggers some customization
     * of the latter */
    gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(layout));

    /* Ensure the scrolling policy is set to NEVER */
    gtk_scrolled_window_get_policy(scrolled_window, &hpolicy, &vpolicy);
    g_assert_cmpint(hpolicy, ==, GTK_POLICY_NEVER);
    g_assert_cmpint(vpolicy, ==, GTK_POLICY_NEVER);

    /* Check the scrolled window size has been updated */
    gtk_widget_get_size_request(GTK_WIDGET(scrolled_window), &width, &height);

    /* The current implementation adds 1 pixel margin, so 1x1 becomes 3x3 */
    g_assert_cmpint(width, ==, 3);
    g_assert_cmpint(height, ==, 3);

    gtk_widget_destroy(GTK_WIDGET(scrolled_window));
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);
    gtk_init_check(&argc, &argv);

    adg_test_add_object_checks("/adg/gtk/layout/type/object", ADG_GTK_TYPE_LAYOUT);

    g_test_add_func("/adg-gtk/layout/property/hadjustment", _adg_property_hadjustment);
    g_test_add_func("/adg-gtk/layout/property/vadjustment", _adg_property_vadjustment);
#ifdef GTK3_ENABLED
    g_test_add_func("/adg-gtk/layout/property/hscroll-policy", _adg_property_hscroll_policy);
    g_test_add_func("/adg-gtk/layout/property/vscroll-policy", _adg_property_vscroll_policy);
#endif

    g_test_add_func("/adg-gtk/layout/method/size-allocate", _adg_method_size_allocate);
    g_test_add_func("/adg-gtk/layout/method/value-changed", _adg_method_value_changed);
    g_test_add_func("/adg-gtk/layout/method/parent-set", _adg_method_set_parent);

    return g_test_run();
}
