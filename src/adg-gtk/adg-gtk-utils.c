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
