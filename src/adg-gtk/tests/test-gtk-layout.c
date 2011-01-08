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
_adg_test_hadjustment(void)
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
    g_assert(GTK_IS_ADJUSTMENT(hadjustment));

    adg_gtk_layout_set_hadjustment(layout, NULL);
    hadjustment = adg_gtk_layout_get_hadjustment(layout);
    g_assert(GTK_IS_ADJUSTMENT(hadjustment));

    adg_gtk_layout_set_hadjustment(layout, valid_adjustment);
    hadjustment = adg_gtk_layout_get_hadjustment(layout);
    g_assert(hadjustment == valid_adjustment);

    adg_gtk_layout_set_hadjustment(layout, invalid_adjustment);
    hadjustment = adg_gtk_layout_get_hadjustment(layout);
    g_assert(hadjustment == valid_adjustment);

    adg_gtk_layout_set_hadjustment(layout, NULL);
    hadjustment = adg_gtk_layout_get_hadjustment(layout);
    g_assert(hadjustment != valid_adjustment && GTK_IS_ADJUSTMENT(hadjustment));

    /* Using GObject property methods */
    g_object_set(layout, "hadjustment", NULL, NULL);
    g_object_get(layout, "hadjustment", &hadjustment, NULL);
    g_assert(GTK_IS_ADJUSTMENT(hadjustment));
    g_object_unref(hadjustment);

    g_object_set(layout, "hadjustment", valid_adjustment, NULL);
    g_object_get(layout, "hadjustment", &hadjustment, NULL);
    g_assert(hadjustment == valid_adjustment);
    g_object_unref(hadjustment);

    g_object_set(layout, "hadjustment", invalid_adjustment, NULL);
    g_object_get(layout, "hadjustment", &hadjustment, NULL);
    g_assert(hadjustment == valid_adjustment);
    g_object_unref(hadjustment);

    g_object_set(layout, "hadjustment", NULL, NULL);
    g_object_get(layout, "hadjustment", &hadjustment, NULL);
    g_assert(hadjustment != valid_adjustment && GTK_IS_ADJUSTMENT(hadjustment));
    g_object_unref(hadjustment);

    g_object_unref(layout);
    g_object_unref(valid_adjustment);
}

static void
_adg_test_vadjustment(void)
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
    g_assert(GTK_IS_ADJUSTMENT(vadjustment));

    adg_gtk_layout_set_vadjustment(layout, NULL);
    vadjustment = adg_gtk_layout_get_vadjustment(layout);
    g_assert(GTK_IS_ADJUSTMENT(vadjustment));

    adg_gtk_layout_set_vadjustment(layout, valid_adjustment);
    vadjustment = adg_gtk_layout_get_vadjustment(layout);
    g_assert(vadjustment == valid_adjustment);

    adg_gtk_layout_set_vadjustment(layout, invalid_adjustment);
    vadjustment = adg_gtk_layout_get_vadjustment(layout);
    g_assert(vadjustment == valid_adjustment);

    adg_gtk_layout_set_vadjustment(layout, NULL);
    vadjustment = adg_gtk_layout_get_vadjustment(layout);
    g_assert(vadjustment != valid_adjustment && GTK_IS_ADJUSTMENT(vadjustment));

    /* Using GObject property methods */
    g_object_set(layout, "vadjustment", NULL, NULL);
    g_object_get(layout, "vadjustment", &vadjustment, NULL);
    g_assert(GTK_IS_ADJUSTMENT(vadjustment));
    g_object_unref(vadjustment);

    g_object_set(layout, "vadjustment", valid_adjustment, NULL);
    g_object_get(layout, "vadjustment", &vadjustment, NULL);
    g_assert(vadjustment == valid_adjustment);
    g_object_unref(vadjustment);

    g_object_set(layout, "vadjustment", invalid_adjustment, NULL);
    g_object_get(layout, "vadjustment", &vadjustment, NULL);
    g_assert(vadjustment == valid_adjustment);
    g_object_unref(vadjustment);

    g_object_set(layout, "vadjustment", NULL, NULL);
    g_object_get(layout, "vadjustment", &vadjustment, NULL);
    g_assert(vadjustment != valid_adjustment && GTK_IS_ADJUSTMENT(vadjustment));
    g_object_unref(vadjustment);

    g_object_unref(layout);
    g_object_unref(valid_adjustment);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_func("/adg/gtk/layout/hadjustment", _adg_test_hadjustment);
    adg_test_add_func("/adg/gtk/layout/vadjustment", _adg_test_vadjustment);

    return g_test_run();
}
