/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2008, Nicola Fontana <ntd at entidi.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the 
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 */


#ifndef __ADG_CANVAS_H__
#define __ADG_CANVAS_H__

#include <adg/adg-container.h>
#include <adg/adg-style.h>


G_BEGIN_DECLS

#define ADG_TYPE_CANVAS             (adg_canvas_get_type ())
#define ADG_CANVAS(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), ADG_TYPE_CANVAS, AdgCanvas))
#define ADG_CANVAS_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), ADG_TYPE_CANVAS, AdgCanvasClass))
#define ADG_IS_CANVAS(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ADG_TYPE_CANVAS))
#define ADG_IS_CANVAS_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), ADG_TYPE_CANVAS))
#define ADG_CANVAS_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), ADG_TYPE_CANVAS, AdgCanvasClass))

/* AdgCanvas declared in adg-entity.h */
typedef struct _AdgCanvasClass   AdgCanvasClass;
typedef struct _AdgCanvasPrivate AdgCanvasPrivate;


struct _AdgCanvas {
    AdgContainer	 container;
    /*< private >*/
    AdgCanvasPrivate	*priv;
};

struct _AdgCanvasClass {
    AdgContainerClass	 parent_class;
};


GType                    adg_canvas_get_type            (void) G_GNUC_CONST;

AdgCanvas *              adg_canvas_new                 (void);

G_END_DECLS


#endif /* __ADG_CANVAS_H__ */
