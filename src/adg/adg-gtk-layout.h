/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011,2012  Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADG_GTK_LAYOUT_H__
#define __ADG_GTK_LAYOUT_H__


G_BEGIN_DECLS

#define ADG_GTK_TYPE_LAYOUT             (adg_gtk_layout_get_type())
#define ADG_GTK_LAYOUT(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_GTK_TYPE_LAYOUT, AdgGtkLayout))
#define ADG_GTK_LAYOUT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_GTK_TYPE_LAYOUT, AdgGtkLayoutClass))
#define ADG_GTK_IS_LAYOUT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_GTK_TYPE_LAYOUT))
#define ADG_GTK_IS_LAYOUT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_GTK_TYPE_LAYOUT))
#define ADG_GTK_LAYOUT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_GTK_TYPE_LAYOUT, AdgGtkLayoutClass))

typedef struct _AdgGtkLayout        AdgGtkLayout;
typedef struct _AdgGtkLayoutClass   AdgGtkLayoutClass;

struct _AdgGtkLayout {
    /*< private >*/
    AdgGtkArea           parent;
    gpointer             data;
};

struct _AdgGtkLayoutClass {
    /*< private >*/
    AdgGtkAreaClass      parent_class;
};


GType           adg_gtk_layout_get_type         (void) G_GNUC_CONST;

GtkWidget *     adg_gtk_layout_new              (void);
GtkWidget *     adg_gtk_layout_new_with_canvas  (AdgCanvas      *canvas);
void            adg_gtk_layout_set_hadjustment  (AdgGtkLayout   *layout,
                                                 GtkAdjustment  *hadjustment);
GtkAdjustment * adg_gtk_layout_get_hadjustment  (AdgGtkLayout   *layout);
void            adg_gtk_layout_set_vadjustment  (AdgGtkLayout   *layout,
                                                 GtkAdjustment  *vadjustment);
GtkAdjustment * adg_gtk_layout_get_vadjustment  (AdgGtkLayout   *layout);


G_END_DECLS


#endif /* __ADG_GTK_LAYOUT_H__ */
