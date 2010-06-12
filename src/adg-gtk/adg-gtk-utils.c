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
 * adg_canvas_set_paper:
 * @canvas: an #AdgCanvas
 * @paper_name: a paper name or %NULL
 *
 * A convenient function to set the size of @canvas using a
 * @paper_name value. This should be a PWG 5101.1-2002 paper name
 * and it will be passed as is to gtk_paper_size_new(), so use
 * any valid name accepted by that function.
 *
 * Using %NULL will reset the size, that is it will be resolved to
 * a call to adg_canvas_set_size() with a %NULL size.
 *
 * When a valid value is passed, the margins will be modified
 * accordingly to the default margins, as returned by the temporary
 * #GtkPaperSize instance. If you want to use your own margins,
 * set them to your values <emphasis>after</emphasis> this call.
 **/
void
adg_canvas_set_paper(AdgCanvas *canvas, const gchar *paper_name,
                     GtkPageOrientation orientation)
{
    GtkPaperSize *paper;
    AdgPair size;
    gdouble margin;

    g_return_if_fail(ADG_IS_CANVAS(canvas));

    if (paper_name == NULL) {
        /* Unset the canvas size */
        adg_canvas_set_size(canvas, NULL);
        return;
    }

    paper = gtk_paper_size_new(paper_name);

    switch (orientation) {
    case GTK_PAGE_ORIENTATION_PORTRAIT:
    case GTK_PAGE_ORIENTATION_REVERSE_PORTRAIT:
        size.x = gtk_paper_size_get_width(paper, GTK_UNIT_POINTS);
        size.y = gtk_paper_size_get_height(paper, GTK_UNIT_POINTS);
        break;
    default:
        size.y = gtk_paper_size_get_width(paper, GTK_UNIT_POINTS);
        size.x = gtk_paper_size_get_height(paper, GTK_UNIT_POINTS);
        break;
    }
    adg_canvas_set_size(canvas, &size);

    margin = gtk_paper_size_get_default_top_margin(paper, GTK_UNIT_POINTS);
    adg_canvas_set_top_margin(canvas, margin);
    margin = gtk_paper_size_get_default_right_margin(paper, GTK_UNIT_POINTS);
    adg_canvas_set_right_margin(canvas, margin);
    margin = gtk_paper_size_get_default_bottom_margin(paper, GTK_UNIT_POINTS);
    adg_canvas_set_bottom_margin(canvas, margin);
    margin = gtk_paper_size_get_default_left_margin(paper, GTK_UNIT_POINTS);
    adg_canvas_set_left_margin(canvas, margin);

    gtk_paper_size_free(paper);
}
