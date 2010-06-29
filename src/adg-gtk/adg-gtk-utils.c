/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010  Nicola Fontana <ntd at entidi.it>
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


/**
 * SECTION:adg-gtk-utils
 * @Section_Id:gtk-utilities
 * @title: GTK+ utilities
 * @short_description: Assorted macros and backward compatible fallbacks
 *
 * Collection of macros and functions that do not fit inside any other topic.
 **/


#include "adg-gtk-internal.h"
#include "adg-gtk-utils.h"


#if GTK_CHECK_VERSION(2, 14, 0)
#else

/**
 * gtk_widget_get_window:
 * @widget: a #GtkWidget
 *
 * Returns the widget's window if it is realized, %NULL otherwise.
 * This is an API fallback for GTK+ prior to 2.14.
 *
 * Return value: @widget's window.
 **/
GdkWindow *
gtk_widget_get_window(GtkWidget *widget)
{
  g_return_val_if_fail (GTK_IS_WIDGET (widget), NULL);

  return widget->window;
}

#endif

/**
 * adg_gtk_window_hide_here:
 * @window: a #GtkWindow
 *
 * A convenient function that hides @window and tries to store the
 * current position. Any subsequent call to gtk_widget_show() will
 * hopefully reopen the window at the same position.
 *
 * It can be used instead of gtk_widget_hide() or by connecting it
 * to a #GtkDialog::response signal, for instance:
 *
 * |[
 * g_signal_connect(dialog, "response",
 *                  G_CALLBACK(adg_gtk_window_hide_here), NULL);
 * ]|
 **/
void
adg_gtk_window_hide_here(GtkWindow *window)
{
    gint x, y;

    g_return_if_fail(GTK_IS_WINDOW(window));

    gtk_window_get_position(window, &x, &y);
    gtk_widget_hide((GtkWidget *) window);
    gtk_window_set_position(window, GTK_WIN_POS_NONE);
    gtk_window_move(window, x, y);
}

/**
 * adg_canvas_set_paper:
 * @canvas: an #AdgCanvas
 * @paper_name: a paper name
 * @orientation: the page orientation
 *
 * A convenient function to set the size of @canvas using a
 * @paper_name and an @orientation value. This should be a
 * PWG 5101.1-2002 paper name and it will be passed as is to
 * gtk_paper_size_new(), so use any valid name accepted by
 * that function.
 *
 * To reset this size, you could use adg_canvas_set_size() with a
 * %NULL size: in this way the size will match the boundary boxes
 * of the entities contained by the canvas.
 *
 * Furthermore, the margins will be set to their default values,
 * that is the margins returned by the #GtkPaperSize API.
 * If you want to use your own margins on a named paper size,
 * set them <emphasis>after</emphasis> the call to this function.
 **/
void
adg_canvas_set_paper(AdgCanvas *canvas, const gchar *paper_name,
                     GtkPageOrientation orientation)
{
    GtkPageSetup *page_setup;
    GtkPaperSize *paper_size;

    g_return_if_fail(ADG_IS_CANVAS(canvas));
    g_return_if_fail(paper_name != NULL);

    page_setup = gtk_page_setup_new();
    paper_size = gtk_paper_size_new(paper_name);

    gtk_page_setup_set_paper_size(page_setup, paper_size);
    gtk_page_setup_set_orientation(page_setup, orientation);
    gtk_paper_size_free(paper_size);

    adg_canvas_set_page_setup(canvas, page_setup);
    g_object_unref(page_setup);
}

/**
 * adg_canvas_set_page_setup:
 * @canvas: an #AdgCanvas
 * @page_setup: the page setup
 *
 * A convenient function to setup the page of @canvas so it can
 * also be subsequentially used for printing. It is allowed to
 * pass %NULL for @page_setup to unset the setup data from @canvas.
 *
 * A reference to @page_setup is added, so there is no need to keep
 * alive this object outside this function. The @page_setup pointer
 * is stored in the associative key %_adg_page_setup and can be
 * retrieved at any time with:
 *
 * |[
 * page_setup = g_object_get_data(G_OBJECT(canvas), "_adg_page_setup");
 * ]|
 *
 * The size and margins provided by @page_setup are used to set the
 * size and margins of @canvas much in the same way as what
 * adg_canvas_set_paper() does. This means if you set a page and
 * then unset it, the canvas will retain size and margins of the
 * original page although @page_setup will not be used for printing.
 * You must unset the size with adg_canvas_set_size() with a %NULL size.
 *
 * |[
 * // By default, canvas does not have an explicit size
 * adg_canvas_set_page_setup(canvas, a4);
 * // Here canvas has the size and margins specified by a4
 * adg_canvas_set_page_setup(canvas, NULL);
 * // Now the only difference is that canvas is no more bound
 * // to the a4 page setup, so the following will return NULL:
 * page_setup = g_object_get_data(G_OBJECT(canvas), "_adg_page_setup");
 * // To restore the original status and have an autocomputed size:
 * adg_canvas_set_size(canvas, NULL);
 * ]|
 **/
void
adg_canvas_set_page_setup(AdgCanvas *canvas, GtkPageSetup *page_setup)
{
    gdouble top, right, bottom, left;
    AdgPair size;

    g_return_if_fail(ADG_IS_CANVAS(canvas));

    if (page_setup == NULL) {
        /* By convention, NULL resets the default page but
         * does not change any other property */
        g_object_set_data((GObject *) canvas, "_adg_page_setup", NULL);
        return;
    }

    g_return_if_fail(GTK_IS_PAGE_SETUP(page_setup));

    top = gtk_page_setup_get_top_margin(page_setup, GTK_UNIT_POINTS);
    right = gtk_page_setup_get_right_margin(page_setup, GTK_UNIT_POINTS);
    bottom = gtk_page_setup_get_bottom_margin(page_setup, GTK_UNIT_POINTS);
    left = gtk_page_setup_get_left_margin(page_setup, GTK_UNIT_POINTS);
    size.x = gtk_page_setup_get_page_width(page_setup, GTK_UNIT_POINTS);
    size.y = gtk_page_setup_get_page_height(page_setup, GTK_UNIT_POINTS);

    adg_canvas_set_size(canvas, &size);
    adg_canvas_set_margins(canvas, top, right, bottom, left);

    g_object_ref(page_setup);
    g_object_set_data_full((GObject *) canvas, "_adg_page_setup",
                           page_setup, g_object_unref);
}
