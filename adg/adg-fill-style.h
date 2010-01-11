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


#ifndef __ADG_FILL_STYLE_H__
#define __ADG_FILL_STYLE_H__

#include <adg/adg-style.h>
#include <adg/adg-pattern.h>
#include <cpml/cpml-extents.h>


G_BEGIN_DECLS

#define ADG_TYPE_FILL_STYLE             (adg_fill_style_get_type())
#define ADG_FILL_STYLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), ADG_TYPE_FILL_STYLE, AdgFillStyle))
#define ADG_FILL_STYLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), ADG_TYPE_FILL_STYLE, AdgFillStyleClass))
#define ADG_IS_FILL_STYLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), ADG_TYPE_FILL_STYLE))
#define ADG_IS_FILL_STYLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), ADG_TYPE_FILL_STYLE))
#define ADG_FILL_STYLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), ADG_TYPE_FILL_STYLE, AdgFillStyleClass))


typedef struct _AdgFillStyle        AdgFillStyle;
typedef struct _AdgFillStyleClass   AdgFillStyleClass;

struct _AdgFillStyle {
    /*< private >*/
    AdgStyle             parent;
    gpointer             data;
};

struct _AdgFillStyleClass {
    /*< private >*/
    AdgStyleClass        parent_class;
    /*< public >*/
    void                (*set_extents)          (AdgFillStyle   *fill_style,
                                                 const CpmlExtents *extents);
};


GType           adg_fill_style_get_type         (void) G_GNUC_CONST;
AdgPattern *    adg_fill_style_get_pattern      (AdgFillStyle   *fill_style);
void            adg_fill_style_set_pattern      (AdgFillStyle   *fill_style,
                                                 AdgPattern     *pattern);
const CpmlExtents *
                adg_fill_style_get_extents      (AdgFillStyle   *fill_style);
void            adg_fill_style_set_extents      (AdgFillStyle   *fill_style,
                                                 const CpmlExtents *extents);

G_END_DECLS


#endif /* __ADG_FILL_STYLE_H__ */
