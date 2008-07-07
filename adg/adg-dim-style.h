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


#ifndef __ADG_DIM_STYLE_H__
#define __ADG_DIM_STYLE_H__

#include <adg/adg-style.h>
#include <adg/adg-enums.h>


G_BEGIN_DECLS


#define ADG_TYPE_DIM_STYLE             (adg_dim_style_get_type ())
#define ADG_DIM_STYLE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), ADG_TYPE_DIM_STYLE, AdgDimStyle))
#define ADG_DIM_STYLE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), ADG_TYPE_DIM_STYLE, AdgDimStyleClass))
#define ADG_IS_DIM_STYLE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ADG_TYPE_DIM_STYLE))
#define ADG_IS_DIM_STYLE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), ADG_TYPE_DIM_STYLE))
#define ADG_DIM_STYLE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), ADG_TYPE_DIM_STYLE, AdgDimStyleClass))

typedef struct _AdgDimStyle        AdgDimStyle;
typedef struct _AdgDimStyleClass   AdgDimStyleClass;
typedef struct _AdgDimStylePrivate AdgDimStylePrivate;

struct _AdgDimStyle
{
  AdgStyle		 style;

  /*< private >*/
  AdgDimStylePrivate	*priv;
};

struct _AdgDimStyleClass
{
  AdgStyleClass		 parent_class;
};


GType		adg_dim_style_get_type	(void) G_GNUC_CONST;
AdgStyle *	adg_dim_style_new	(void);
AdgStyle *	adg_dim_style_from_id	(AdgDimStyleId		 id);


G_END_DECLS


#endif /* __ADG_DIM_STYLE_H__ */
