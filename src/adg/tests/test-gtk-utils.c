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


#include <adg-test.h>
#include <adg.h>

#if GTK_CHECK_VERSION(2, 18, 0)
#else

/* Backward compatible fallback */
gboolean
gtk_widget_get_visible(GtkWidget *widget)
{
    g_return_val_if_fail(GTK_IS_WIDGET(widget));
    return GTK_WIDGET_VISIBLE(widget);
}

#endif


static void
_adg_method_widget_get_window(void)
{
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_assert_null(gtk_widget_get_window(window));
}

static void
_adg_method_widget_get_realized(void)
{
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_assert_false(gtk_widget_get_realized(window));
    gtk_widget_destroy(window);
}

static void
_adg_method_window_hide_here(void)
{
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    /* I cannot call gtk_widget_show because there likely be no screens
     * for realizing window. Just commenting for now.
     * gtk_widget_show(window);
     * g_assert_true(gtk_widget_get_visible(window));
     */

    adg_gtk_window_hide_here(GTK_WINDOW(window));
    g_assert_false(gtk_widget_get_visible(window));

    gtk_widget_destroy(window);
}

static void
_adg_method_toggle_button_sensitivize(void)
{
    GtkToggleButton *button = (GtkToggleButton *) gtk_toggle_button_new();
    GtkWidget *widget = gtk_toggle_button_new();

    gtk_toggle_button_set_active(button, FALSE);
    adg_gtk_toggle_button_sensitivize(button, widget);
    g_assert_false(gtk_widget_get_sensitive(widget));

    gtk_toggle_button_set_active(button, TRUE);
    adg_gtk_toggle_button_sensitivize(button, widget);
    g_assert_true(gtk_widget_get_sensitive(widget));

    g_object_ref_sink(button);
    g_object_unref(button);

    g_object_ref_sink(widget);
    g_object_unref(widget);
}

static void
_adg_method_use_default_icons(void)
{
    GList *list;

    list = gtk_window_get_default_icon_list();
    g_assert_null(list);

    /* Check it does not crash passing NULL */
    adg_gtk_use_default_icons(NULL);

    list = gtk_window_get_default_icon_list();
    g_assert_null(list);

    /* Set to the demo directory, where the real icons *must* be presents */
    adg_gtk_use_default_icons(SRCDIR "/../../../demo");

    list = gtk_window_get_default_icon_list();
    g_assert_nonnull(list);
    g_list_free(list);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);
    gtk_init_check(&argc, &argv);

    g_test_add_func("/adg/method/widget-get-window", _adg_method_widget_get_window);
    g_test_add_func("/adg/method/widget-get-realized", _adg_method_widget_get_realized);
    g_test_add_func("/adg/method/window-hide-here", _adg_method_window_hide_here);
    g_test_add_func("/adg/method/toggle-button-sensitivize", _adg_method_toggle_button_sensitivize);
    g_test_add_func("/adg/method/use-default-icons", _adg_method_use_default_icons);

    return g_test_run();
}
