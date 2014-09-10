/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011,2012,2013  Nicola Fontana <ntd at entidi.it>
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
 *
 * Since: 1.0
 **/


/**
 * ADG_GTK_MODIFIERS:
 *
 * A GDK mask of the key/mouse modifiers accepted by the GTK+ widgets
 * of the ADG library. This means the state of the specified modifiers
 * is always checked: for example %GDK_CONTROL_MASK and %GDK_SHIFT_MASK
 * are included, hence keeping <keycap>CTRL</keycap>
 * and <keycap>SHIFT</keycap> pressed is different from keeping
 * only <keycap>SHIFT</keycap> pressed. %GDK_LOCK_MASK instead is not
 * considered, so having it enabled or disabled does not make any
 * difference while monitoring the status <keycap>SHIFT</keycap>
 * or <keycap>CTRL</keycap>.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"
#include <gtk/gtk.h>

#include "adg-gtk-utils.h"


#if GTK_CHECK_VERSION(2, 14, 0)
#else

/**
 * gtk_widget_get_window:
 * @widget: a #GtkWidget
 *
 * Returns the widget's window if it is realized
 * or <constant>NULL</constant> otherwise.
 *
 * This is a fallback API for GTK+ prior to 2.14.
 *
 * Return value: @widget's window.
 *
 * Since: 1.0
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
 * <informalexample><programlisting language="C">
 * g_signal_connect(dialog, "response",
 *                  G_CALLBACK(adg_gtk_window_hide_here), NULL);
 * </programlisting></informalexample>
 *
 * Since: 1.0
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
 * adg_gtk_toggle_button_sensitivize:
 * @toggle_button: a #GtkToggleButton
 * @widget: the #GtkWidget
 *
 * Assigns the value of the #GtkToggleButton:active property of
 * @toggle_button to the #GtkWidget:sensitive property of @widget.
 * Useful to set or reset the sensitiveness of @widget depending
 * of the state of a check button, for example:
 *
 * <informalexample><programlisting language="C">
 * g_signal_connect(toggle_button, "toggled",
 *                  G_CALLBACK(adg_gtk_toggle_button_sensitivize), widget1);
 * g_signal_connect(toggle_button, "toggled",
 *                  G_CALLBACK(adg_gtk_toggle_button_sensitivize), widget2);
 * g_signal_connect(toggle_button, "toggled",
 *                  G_CALLBACK(adg_gtk_toggle_button_sensitivize), widget3);
 * </programlisting></informalexample>
 *
 * Since: 1.0
 **/
void
adg_gtk_toggle_button_sensitivize(GtkToggleButton *toggle_button,
                                  GtkWidget *widget)
{
    gboolean is_active;

    g_return_if_fail(GTK_IS_TOGGLE_BUTTON(toggle_button));
    g_return_if_fail(GTK_IS_WIDGET(widget));

    is_active = gtk_toggle_button_get_active(toggle_button);
    gtk_widget_set_sensitive(widget, is_active);
}

/**
 * adg_gtk_use_default_icons:
 * @dir: the directory where the icons should be installed
 *
 * Sets the default icon list of every #GtkWindow to a hand-coded
 * list of ADG icons. Check gtk gtk_window_set_default_icon_list()
 * for further details.
 **/
void
adg_gtk_use_default_icons(const gchar *dir)
{
    const gchar **p_file;
    const gchar *files[] = {
        "adg-16.png",
        "adg-32.png",
        "adg-48.png",
        "adg-64.png",
        "adg-128.png",
        NULL
    };
    GList *list;
    gchar *path;
    GdkPixbuf *icon;

    list = NULL;
    for (p_file = files; *p_file != NULL; ++p_file) {
        path = adg_find_file(*p_file, dir, NULL);
        if (path == NULL)
            continue;
        icon = gdk_pixbuf_new_from_file(path, NULL);
        g_free(path);
        if (icon != NULL)
            list = g_list_append(list, icon);
    }

    if (list != NULL) {
        gtk_window_set_default_icon_list(list);
        g_list_free(list);
    }
}
