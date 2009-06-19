/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009 Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADG_WIDGET_H__
#define __ADG_WIDGET_H__

#include <gtk/gtk.h>
#include <adg/adg-canvas.h>


G_BEGIN_DECLS

#define ADG_TYPE_WIDGET             (adg_widget_get_type())
#define ADG_WIDGET(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_WIDGET, AdgWidget))
#define ADG_WIDGET_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_WIDGET, AdgWidget))
#define ADG_IS_WIDGET(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_WIDGET))
#define ADG_IS_WIDGET_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_WIDGET))
#define ADG_WIDGET_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_WIDGET, AdgWidgetClass))

typedef struct _AdgWidget        AdgWidget;
typedef struct _AdgWidgetClass   AdgWidgetClass;
typedef struct _AdgWidgetPrivate AdgWidgetPrivate;

struct _AdgWidget {
    /*< private >*/
    GtkDrawingArea       object;
    AdgWidgetPrivate    *priv;
};

struct _AdgWidgetClass {
    GtkDrawingAreaClass  parent_class;

    /* Virtual Table */
    void                (*canvas_changed)       (AdgWidget      *widget);
};


GType                   adg_widget_get_type     (void) G_GNUC_CONST;

GtkWidget *             adg_widget_new          (AdgCanvas      *canvas);

AdgCanvas *             adg_widget_get_canvas   (AdgWidget      *widget);
void                    adg_widget_set_canvas   (AdgWidget      *widget,
                                                 AdgCanvas      *canvas);

G_END_DECLS


#endif /* __ADG_WIDGET_H__ */
