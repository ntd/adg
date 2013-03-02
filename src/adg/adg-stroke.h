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


#if !defined(__ADG_H__)
#error "Only <adg.h> can be included directly."
#endif


#ifndef __ADG_STROKE_H__
#define __ADG_STROKE_H__


G_BEGIN_DECLS

#define ADG_TYPE_STROKE             (adg_stroke_get_type())
#define ADG_STROKE(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_STROKE, AdgStroke))
#define ADG_STROKE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_STROKE, AdgStrokeClass))
#define ADG_IS_STROKE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_STROKE))
#define ADG_IS_STROKE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_STROKE))
#define ADG_STROKE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_STROKE, AdgStrokeClass))

typedef struct _AdgStroke        AdgStroke;
typedef struct _AdgStrokeClass   AdgStrokeClass;

struct _AdgStroke {
    /*< private >*/
    AdgEntity           parent;
    gpointer            data;
};

struct _AdgStrokeClass {
    /*< private >*/
    AdgEntityClass      parent_class;
};


GType           adg_stroke_get_type             (void) G_GNUC_CONST;

AdgStroke *     adg_stroke_new                  (AdgTrail       *trail);

void            adg_stroke_set_line_dress       (AdgStroke      *stroke,
                                                 AdgDress        dress);
AdgDress        adg_stroke_get_line_dress       (AdgStroke      *stroke);
void            adg_stroke_set_trail            (AdgStroke      *stroke,
                                                 AdgTrail       *trail);
AdgTrail *      adg_stroke_get_trail            (AdgStroke      *stroke);

G_END_DECLS


#endif /* __ADG_STROKE_H__ */
