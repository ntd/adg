/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011  Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADG_CANVAS_H__
#define __ADG_CANVAS_H__


G_BEGIN_DECLS

#define ADG_TYPE_CANVAS             (adg_canvas_get_type())
#define ADG_CANVAS(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_CANVAS, AdgCanvas))
#define ADG_CANVAS_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_CANVAS, AdgCanvasClass))
#define ADG_IS_CANVAS(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_CANVAS))
#define ADG_IS_CANVAS_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_CANVAS))
#define ADG_CANVAS_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_CANVAS, AdgCanvasClass))

#if 0
/* AdgCanvas declared in adg-entity.h */
typedef struct _AdgCanvas        AdgCanvas;
#endif
typedef struct _AdgCanvasClass   AdgCanvasClass;


struct _AdgCanvas {
    /*< private >*/
    AdgContainer         parent;
    gpointer             data;
};

struct _AdgCanvasClass {
    /*< private >*/
    AdgContainerClass    parent_class;
};


GType           adg_canvas_get_type             (void) G_GNUC_CONST;

AdgCanvas *     adg_canvas_new                  (void);
void            adg_canvas_set_size             (AdgCanvas      *canvas,
                                                 const AdgPair  *pair);
void            adg_canvas_set_size_explicit    (AdgCanvas      *canvas,
                                                 gdouble         x,
                                                 gdouble         y);
const AdgPair * adg_canvas_get_size             (AdgCanvas      *canvas);
void            adg_canvas_set_background_dress (AdgCanvas      *canvas,
                                                 AdgDress        dress);
AdgDress        adg_canvas_get_background_dress (AdgCanvas      *canvas);
void            adg_canvas_set_frame_dress      (AdgCanvas      *canvas,
                                                 AdgDress        dress);
AdgDress        adg_canvas_get_frame_dress      (AdgCanvas      *canvas);
void            adg_canvas_set_title_block      (AdgCanvas      *canvas,
                                                 AdgTitleBlock  *title_block);
AdgTitleBlock * adg_canvas_get_title_block      (AdgCanvas      *canvas);
void            adg_canvas_set_top_margin       (AdgCanvas      *canvas,
                                                 gdouble         value);
gdouble         adg_canvas_get_top_margin       (AdgCanvas      *canvas);
void            adg_canvas_set_right_margin     (AdgCanvas      *canvas,
                                                 gdouble         value);
gdouble         adg_canvas_get_right_margin     (AdgCanvas      *canvas);
void            adg_canvas_set_bottom_margin    (AdgCanvas      *canvas,
                                                 gdouble         value);
gdouble         adg_canvas_get_bottom_margin    (AdgCanvas      *canvas);
void            adg_canvas_set_left_margin      (AdgCanvas      *canvas,
                                                 gdouble         value);
gdouble         adg_canvas_get_left_margin      (AdgCanvas      *canvas);
void            adg_canvas_set_margins          (AdgCanvas      *canvas,
                                                 gdouble         top,
                                                 gdouble         right,
                                                 gdouble         bottom,
                                                 gdouble         left);
void            adg_canvas_apply_margins        (AdgCanvas      *canvas,
                                                 CpmlExtents    *extents);
void            adg_canvas_switch_frame         (AdgCanvas      *canvas,
                                                 gboolean        new_state);
gboolean        adg_canvas_has_frame            (AdgCanvas      *canvas);
void            adg_canvas_set_top_padding      (AdgCanvas      *canvas,
                                                 gdouble         value);
gdouble         adg_canvas_get_top_padding      (AdgCanvas      *canvas);
void            adg_canvas_set_right_padding    (AdgCanvas      *canvas,
                                                 gdouble         value);
gdouble         adg_canvas_get_right_padding    (AdgCanvas      *canvas);
void            adg_canvas_set_bottom_padding   (AdgCanvas      *canvas,
                                                 gdouble         value);
gdouble         adg_canvas_get_bottom_padding   (AdgCanvas      *canvas);
void            adg_canvas_set_left_padding     (AdgCanvas      *canvas,
                                                 gdouble         value);
gdouble         adg_canvas_get_left_padding     (AdgCanvas      *canvas);
void            adg_canvas_set_paddings         (AdgCanvas      *canvas,
                                                 gdouble         top,
                                                 gdouble         right,
                                                 gdouble         bottom,
                                                 gdouble         left);

G_END_DECLS


#endif /* __ADG_CANVAS_H__ */
