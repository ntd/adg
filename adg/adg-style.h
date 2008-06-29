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


#ifndef __ADG_STYLE_H__
#define __ADG_STYLE_H__

#include <glib-object.h>
#include <cairo.h>

#include <adg/adg-pair.h>


G_BEGIN_DECLS


/* Forward declarations */
typedef struct _AdgLineStyle  AdgLineStyle;
typedef struct _AdgFontStyle  AdgFontStyle;
typedef struct _AdgArrowStyle AdgArrowStyle;


#define ADG_TYPE_STYLE             (adg_style_get_type ())
#define ADG_STYLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), ADG_TYPE_STYLE, AdgStyle))
#define ADG_STYLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), ADG_TYPE_STYLE, AdgStyleClass))
#define ADG_IS_STYLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ADG_TYPE_STYLE))
#define ADG_IS_STYLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), ADG_TYPE_STYLE))
#define ADG_STYLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), ADG_TYPE_STYLE, AdgStyleClass))

typedef struct _AdgStyle        AdgStyle;
typedef struct _AdgStyleClass   AdgStyleClass;
typedef struct _AdgStylePrivate AdgStylePrivate;

struct _AdgStyle
{
  GObject		 object;

  /*< private >*/
  AdgStylePrivate	*priv;
};

struct _AdgStyleClass
{
  GObjectClass		 parent_class;
};

GType			 adg_style_get_type		(void) G_GNUC_CONST;


#define ADG_TYPE_DIM_STYLE              (adg_dim_style_get_type ())

typedef struct _AdgDimStyle        AdgDimStyle;

struct _AdgDimStyle
{
  cairo_pattern_t       *pattern;

  AdgStyle		*label_style;
  AdgStyle		*tolerance_style;
  AdgStyle		*note_style;
  AdgStyle		*line_style;
  AdgStyle		*arrow_style;

  double                 from_offset;
  double                 to_offset;
  double                 baseline_spacing;
  AdgPair                quote_offset;
  AdgPair                tolerance_offset;
  double                 tolerance_spacing;
  AdgPair                note_offset;

  gchar                 *measure_format;
  gchar			*measure_tag;
};

/**
 * AdgDimStyleId:
 * @ADG_DIM_STYLE_ISO: ISO standard
 * @ADG_DIM_STYLE_LAST: start of user-defined styles
 *
 * Numeric representation of dimension styles.
 * Some standard dimension styles are predefined.
 */
typedef enum
{
  ADG_DIM_STYLE_ISO,
  ADG_DIM_STYLE_LAST
} AdgDimStyleId;

GType           adg_dim_style_get_type  (void) G_GNUC_CONST;
AdgDimStyle *   adg_dim_style_from_id   (AdgDimStyleId   id);
AdgDimStyleId   adg_dim_style_register  (AdgDimStyle    *new_style);


G_END_DECLS


#endif /* __ADG_STYLE_H__ */
