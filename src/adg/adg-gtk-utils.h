/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2019  Nicola Fontana <ntd at entidi.it>
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


#if !defined(__ADG_H__)
#error "Only <adg.h> can be included directly."
#endif


#ifndef __ADG_GTK_UTILS_H__
#define __ADG_GTK_UTILS_H__

#define ADG_GTK_MODIFIERS  (GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_MOD1_MASK)


G_BEGIN_DECLS

#if GTK_CHECK_VERSION(2, 14, 0)
#else
GdkWindow *     gtk_widget_get_window           (GtkWidget      *widget);
#endif

#if GTK_CHECK_VERSION(2, 20, 0)
#else
gboolean        gtk_widget_get_realized         (GtkWidget      *widget);
#endif

#if GTK_CHECK_VERSION(2, 54, 0)
#else

GObject *       g_object_new_with_properties    (GType          object_type,
                                                 guint          n_properties,
                                                 const char    *names[],
                                                 const GValue   values[]);

#endif

void            adg_gtk_window_hide_here        (GtkWindow      *window);
void            adg_gtk_toggle_button_sensitivize
                                                (GtkToggleButton*toggle_button,
                                                 GtkWidget      *widget);
void            adg_gtk_use_default_icons       (const gchar    *dir);

G_END_DECLS


#endif /* __ADG_GTK_UTILS_H__ */
